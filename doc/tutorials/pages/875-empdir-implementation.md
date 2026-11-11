---
title: empdir — the capstone source code
navtitle: empdir source
part: 9
level: advanced
sql: empdir-schema.sql
---

# empdir — the capstone source code

This page is the source companion to the [capstone](capstone.html): a complete,
single-file implementation of the `empdir` CLI in both languages.

**Objectives**

- see the full running-project architecture in one listing
- connect every piece you learned — pool, statements, binds, array DML,
  subscriptions, AQ, JSON DOM — in a real program

## Schema setup

On top of [`assets/sql/tutorial-schema.sql`](assets/sql/tutorial-schema.sql), the app needs two extra objects —
a JSON profile column and an AQ queue for jobs. Both live in
[`assets/sql/empdir-schema.sql`](assets/sql/empdir-schema.sql) — run it once as the connecting user:

```sql
alter table employees add (profile json);

create or replace type job_t as object (title varchar2(64), content varchar2(200));
begin
    dbms_aqadm.create_queue_table('job_queue_table', 'job_t');
    dbms_aqadm.create_queue('job_queue', 'job_queue_table');
    dbms_aqadm.start_queue('job_queue');
end;
/
```

## The C implementation

:::sample
```c
#include "ocilib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static OCI_Environment *g_env;
static OCI_Pool        *g_pool;

void err_handler(OCI_Error *err) { fprintf(stderr, "%s\n", OCI_ErrorGetString(err)); }

static OCI_Connection *checkout(void)
{
    return OCI_PoolGetConnection(g_env, g_pool, NULL);   /* free() releases back to the pool */
}

/* ------------------------------------------------------------------ list */

static int cmd_list(int dept)
{
    OCI_Connection *cn = checkout();
    OCI_Statement  *st = OCI_StatementCreate(cn);

    if (dept)
    {
        OCI_StatementPrepare(st, "select empno, ename, job, sal from employees "
                        "where deptno = :d order by empno");
        OCI_StatementBindInt32(st, ":d", &dept);
    }
    else
    {
        OCI_StatementPrepare(st, "select empno, ename, job, sal from employees order by empno");
    }

    OCI_StatementExecutePrepared(st);
    OCI_Resultset *rs = OCI_StatementGetResultset(st);

    while (OCI_ResultsetFetchNext(rs))
    {
        printf("%5i  %-10s  %-9s  %8.2f\n",
               OCI_ResultsetGetInt32(rs, 0), OCI_ResultsetGetString(rs, 1),
               OCI_ResultsetGetString(rs, 2), OCI_ResultsetGetDouble(rs, 3));
    }

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    return 0;
}

/* -------------------------------------------------------------------- get */

static int cmd_get(int empno)
{
    OCI_Connection *cn = checkout();
    OCI_Statement  *st = OCI_StatementCreate(cn);

    OCI_StatementPrepare(st, "select ename, job, sal, hiredate from employees where empno = :e");
    OCI_StatementBindInt32(st, ":e", &empno);
    OCI_StatementExecutePrepared(st);

    OCI_Resultset *rs = OCI_StatementGetResultset(st);
    if (OCI_ResultsetFetchNext(rs))
    {
        OCI_Date *d = OCI_ResultsetGetDate(rs, 3);
        otext buf[64];
        OCI_DateToText(d, OTEXT("YYYY-MM-DD"), 63, buf);
        printf("%s  %s  %.2f  %s\n",
               OCI_ResultsetGetString(rs, 0), OCI_ResultsetGetString(rs, 1),
               OCI_ResultsetGetDouble(rs, 2), buf);
    }
    else
    {
        printf("empno %d not found\n", empno);
    }

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    return 0;
}

/* -------------------------------------------------------------------- add */

static int cmd_add(int empno, const otext *name, const otext *job,
                   double sal, int deptno)
{
    OCI_Connection *cn = checkout();
    OCI_Statement  *st = OCI_StatementCreate(cn);

    OCI_StatementPrepare(st, "insert into employees (empno, ename, job, sal, deptno) "
                    "values (:e, :n, :j, :s, :d)");
    OCI_StatementBindInt32(st, ":e", &empno);
    OCI_StatementBindString(st, ":n", (otext *) name, 30);
    OCI_StatementBindString(st, ":j", (otext *) job, 20);
    OCI_StatementBindDouble(st, ":s", &sal);
    OCI_StatementBindInt32(st, ":d", &deptno);
    OCI_StatementExecutePrepared(st);

    printf("%u row(s) inserted\n", OCI_StatementGetAffectedRows(st));
    OCI_ConnectionCommit(cn);

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    return 0;
}

/* ------------------------------------------------------------------ raise */

static int cmd_raise(int deptno, double pct)
{
    OCI_Connection *cn = checkout();
    OCI_Statement  *st = OCI_StatementCreate(cn);

    OCI_StatementPrepare(st, "update employees set sal = sal * (1 + :p / 100) "
                    "where deptno = :d returning empno into :e");

    OCI_StatementBindDouble(st, ":p", &pct);
    OCI_StatementBindInt32(st, ":d", &deptno);
    OCI_StatementRegisterInt32(st, ":e");
    OCI_StatementExecutePrepared(st);

    printf("%u salary update(s)\n", OCI_StatementGetAffectedRows(st));
    OCI_ConnectionCommit(cn);

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    return 0;
}

/* ----------------------------------------------------------------- import */

static int cmd_import(const char *csv)
{
    FILE *f = fopen(csv, "r");
    if (!f)
    {
        fprintf(stderr, "cannot open %s\n", csv);
        return 1;
    }

    OCI_Connection *cn = checkout();
    OCI_Statement  *st = OCI_StatementCreate(cn);

    OCI_StatementPrepare(st, "insert into employees (empno, ename, sal) values (:e, :n, :s)");

    const int BATCH = 64;
    int      empno[64];
    otext    ename[64][31];
    double   sal[64];
    int      count = 0;

    OCI_StatementBindArraySetSize(st, BATCH);
    OCI_StatementBindInt32Array(st, ":e", empno, 0);
    OCI_StatementBindStringArray(st, ":n", (otext *) ename, 30, 0);
    OCI_StatementBindDoubleArray(st, ":s", sal, 0);

    char line[256];
    while (fgets(line, sizeof(line), f))
    {
        sscanf(line, "%d,%30[^,],%lf", &empno[count], ename[count], &sal[count]);
        if (++count == BATCH)
        {
            OCI_StatementBindArraySetSize(st, count);
            OCI_StatementExecutePrepared(st);
            count = 0;
        }
    }
    if (count)
    {
        OCI_StatementBindArraySetSize(st, count);
        OCI_StatementExecutePrepared(st);
    }

    printf("%u row(s) imported\n", OCI_StatementGetAffectedRows(st));
    OCI_ConnectionCommit(cn);

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    fclose(f);
    return 0;
}

/* ------------------------------------------------------------------ watch */

static void on_change(OCI_Event *event)
{
    printf("[cqn] type=%u object=%s rowid=%s\n",
           OCI_EventGetType(event), OCI_EventGetObject(event),
           OCI_EventGetRowid(event));
}

static int cmd_watch(void)
{
    OCI_Connection *cn = checkout();
    OCI_Statement  *st = OCI_StatementCreate(cn);
    OCI_Subscription *sub = OCI_SubscriptionCreate(
        cn, OTEXT("empdir-watch"), OCI_CNT_ALL, on_change, OCI_SRM_DEFAULT, 0);

    OCI_StatementPrepare(st, "select empno from employees");
    OCI_SubscriptionAddStatement(sub, st);     /* watch objects this stmt touches */
    OCI_StatementExecutePrepared(st);
    printf("watching employees — run DML in another session, Ctrl+C to stop\n");

    getchar();   /* stay subscribed until the user stops the program */

    OCI_SubscriptionUnregister(sub);
    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    return 0;
}

/* ------------------------------------------------------------------- jobs */

static int cmd_jobs_submit(const otext *title, const otext *content)
{
    OCI_Connection *cn = checkout();
    OCI_TypeInfo *inf = OCI_TypeInfoGet(cn, OTEXT("job_t"), OCI_TIF_TYPE);
    OCI_Enqueue  *enq = OCI_EnqueueCreate(inf, OTEXT("job_queue"));
    OCI_Message      *msg = OCI_MessageCreate(inf);
    OCI_Object   *job = OCI_ObjectCreate(inf);

    OCI_ObjectSetString(job, OTEXT("title"),   (otext *) title);
    OCI_ObjectSetString(job, OTEXT("content"), (otext *) content);
    OCI_MessageSetObject(msg, job);
    OCI_EnqueuePut(enq, msg);
    OCI_ConnectionCommit(cn);

    OCI_ObjectFree(job);
    OCI_MessageFree(msg);
    OCI_EnqueueFree(enq);
    OCI_TypeInfoFree(inf);
    OCI_ConnectionFree(cn);
    return 0;
}

static int cmd_jobs_work(void)
{
    OCI_Connection *cn = checkout();
    OCI_TypeInfo *inf = OCI_TypeInfoGet(cn, OTEXT("job_t"), OCI_TIF_TYPE);
    OCI_Dequeue  *deq = OCI_DequeueCreate(inf, OTEXT("job_queue"));
    OCI_Message      *msg;

    while ((msg = OCI_DequeueGet(deq)) != NULL)
    {
        OCI_Object *job = OCI_MessageGetObject(msg);
        printf("job: %s — %s\n",
               OCI_ObjectGetString(job, OTEXT("title")),
               OCI_ObjectGetString(job, OTEXT("content")));
    }
    OCI_ConnectionCommit(cn);

    OCI_DequeueFree(deq);
    OCI_TypeInfoFree(inf);
    OCI_ConnectionFree(cn);
    return 0;
}

/* ---------------------------------------------------------------- profile */

static int cmd_profile(int empno)
{
    OCI_Connection *cn = checkout();
    OCI_Statement  *st = OCI_StatementCreate(cn);

    OCI_StatementPrepare(st, "select profile from employees where empno = :e");
    OCI_StatementBindInt32(st, ":e", &empno);
    OCI_StatementExecutePrepared(st);

    OCI_Resultset *rs = OCI_StatementGetResultset(st);
    if (OCI_ResultsetFetchNext(rs) && !OCI_ResultsetIsColumnNull(rs, 1))
    {
        OCI_Json *doc = OCI_ResultsetGetJson(rs, 0);
        OCI_JsonNode *root = OCI_JsonGetRoot(doc);

        const otext *skill = OTEXT("<none>");
        OCI_JsonNode *skills = OCI_JsonNodeGetFieldByName(root, OTEXT("skills"));
        if (skills && OCI_JsonNodeGetItemCount(skills))
        {
            skill = OCI_JsonNodeGetString(OCI_JsonNodeGetItem(skills, 0), NULL);
        }

        printf("emp %d — %u field(s), first skill: %s\n",
               empno, OCI_JsonNodeGetFieldCount(root), skill);
        OCI_JsonFree(doc);          /* frees root & children with it */
    }
    else
    {
        printf("no profile for empno %d\n", empno);
    }

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    return 0;
}

/* -------------------------------------------------------------------- main */

int main(int argc, char **argv)
{
    g_env = OCI_EnvironmentCreate(err_handler, NULL,
                                  OCI_ENV_THREADED | OCI_ENV_EVENTS);
    OCI_EnvironmentSetSubscriptionPort(g_env, 5468);
    g_pool = OCI_PoolCreate(g_env, getenv("OCILIB_DEMO_DB"),
                          getenv("OCILIB_DEMO_USR"), getenv("OCILIB_DEMO_PWD"),
                          OCI_POOL_CONNECTION, OCI_SESSION_DEFAULT, 2, 8, 1);

    int rc = 1;
    const char *cmd = argc > 1 ? argv[1] : "";

    if (!strcmp(cmd, "list"))
        rc = cmd_list(argc > 3 && !strcmp(argv[2], "--dept") ? atoi(argv[3]) : 0);
    else if (!strcmp(cmd, "get") && argc > 2)
        rc = cmd_get(atoi(argv[2]));
    else if (!strcmp(cmd, "add") && argc > 6)
        /* ANSI build: argv strings are already otext */
        rc = cmd_add(atoi(argv[2]), argv[3], argv[4],
                     atof(argv[5]), atoi(argv[6]));
    else if (!strcmp(cmd, "raise") && argc > 3)
        rc = cmd_raise(atoi(argv[2]), atof(argv[3]));
    else if (!strcmp(cmd, "import") && argc > 2)
        rc = cmd_import(argv[2]);
    else if (!strcmp(cmd, "watch"))
        rc = cmd_watch();
    else if (!strcmp(cmd, "jobs") && argc > 2 && !strcmp(argv[2], "work"))
        rc = cmd_jobs_work();
    else if (!strcmp(cmd, "jobs") && argc > 4 && !strcmp(argv[2], "submit"))
        rc = cmd_jobs_submit(argv[3], argv[4]);
    else if (!strcmp(cmd, "profile") && argc > 2)
        rc = cmd_profile(atoi(argv[2]));
    else
        fprintf(stderr, "usage: empdir list|get|add|raise|import|watch|jobs|profile ...\n");

    OCI_PoolFree(g_env, g_pool);
    OCI_EnvironmentFree(g_env);
    return rc;
}
```

```cpp
#include "ocilib.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <array>

using namespace ocilib;

static Pool *g_pool;

static Connection checkout() { return g_pool->GetConnection(); }

/* ------------------------------------------------------------------ list */

static int cmd_list(int dept)
{
    Connection con = checkout();
    Statement  st(con);

    if (dept)
    {
        st.Prepare("select empno, ename, job, sal from employees "
                   "where deptno = :d order by empno");
        st.Bind(":d", dept);
    }
    else
    {
        st.Prepare("select empno, ename, job, sal from employees order by empno");
    }

    st.Execute();
    for (auto &&row : st.GetResultset().Rows())
    {
        std::cout << row.Get<int>(0) << "  " << row.Get<String>(1) << "  "
                  << row.Get<String>(2) << "  " << row.Get<double>(3) << "\n";
    }
    return 0;
}

/* -------------------------------------------------------------------- get */

static int cmd_get(int empno)
{
    Connection con = checkout();
    Statement  st(con);

    st.Prepare("select ename, job, sal, hiredate from employees where empno = :e");
    st.Bind(":e", empno);
    st.Execute();

    Resultset rs = st.GetResultset();
    if (rs.FetchNext())
    {
        std::cout << rs.Get<String>(0) << "  " << rs.Get<String>(1) << "  "
                  << rs.Get<double>(2) << "  " << rs.Get<Date>(3).ToString() << "\n";
    }
    else
    {
        std::cout << "empno " << empno << " not found\n";
    }
    return 0;
}

/* -------------------------------------------------------------------- add */

static int cmd_add(int empno, const String &name, const String &job,
                   double sal, int deptno)
{
    Connection con = checkout();
    Statement  st(con);

    st.Prepare("insert into employees (empno, ename, job, sal, deptno) "
               "values (:e, :n, :j, :s, :d)");
    st.Bind(":e", empno);
    st.Bind(":n", name, 30);
    st.Bind(":j", job, 20);
    st.Bind(":s", sal);
    st.Bind(":d", deptno);
    st.Execute();

    std::cout << st.GetAffectedRows() << " row(s) inserted\n";
    con.Commit();
    return 0;
}

/* ------------------------------------------------------------------ raise */

static int cmd_raise(int deptno, double pct)
{
    Connection con = checkout();
    Statement  st(con);

    st.Prepare("update employees set sal = sal * (1 + :p / 100) "
               "where deptno = :d returning empno into :e");
    st.Bind(":p", pct);
    st.Bind(":d", deptno);
    st.Register<int>(":e");
    st.Execute();

    std::cout << st.GetAffectedRows() << " salary update(s)\n";
    con.Commit();
    return 0;
}

/* ----------------------------------------------------------------- import */

static int cmd_import(const char *csv)
{
    std::ifstream f(csv);
    if (!f)
    {
        std::cerr << "cannot open " << csv << "\n";
        return 1;
    }

    Connection con = checkout();
    Statement  st(con);

    st.Prepare("insert into employees (empno, ename, sal) values (:e, :n, :s)");

    const size_t BATCH = 64;
    std::array<int, 64>    empno{};
    std::array<String, 64> ename;
    std::array<double, 64> sal{};
    size_t count = 0;

    st.SetBindArraySize(BATCH);
    st.Bind(":e", empno);
    st.Bind(":n", ename);
    st.Bind(":s", sal);

    std::string line;
    while (std::getline(f, line))
    {
        char name[31];
        if (sscanf(line.c_str(), "%d,%30[^,],%lf", &empno[count], name, &sal[count]) == 3)
        {
            ename[count] = name;
            if (++count == BATCH)
            {
                st.SetBindArraySize(count);
                st.Execute();
                count = 0;
            }
        }
    }
    if (count)
    {
        st.SetBindArraySize(count);
        st.Execute();
    }

    std::cout << st.GetAffectedRows() << " row(s) imported\n";
    con.Commit();
    return 0;
}

/* ------------------------------------------------------------------ watch */

static void on_change(Event &event)
{
    std::cout << "[cqn] type=" << event.GetType() << " object=" << event.GetObject()
              << " rowid=" << event.GetRowid() << "\n";
}

static int cmd_watch()
{
    Connection con = checkout();
    Statement  st(con);

    Subscription sub = con.Subscribe("empdir-watch", Subscription::NotifyAll,
                                     on_change, Subscription::SecureNone);
    st.Prepare("select empno from employees");
    sub.Register(st);
    st.Execute();
    std::cout << "watching employees — Ctrl+C to stop\n";

    std::cin.get();

    sub.Unregister();
    return 0;
}

/* ------------------------------------------------------------------- jobs */

static int cmd_jobs_submit(const String &title, const String &content)
{
    Connection con = checkout();
    TypeInfo  inf(con, "job_t", TypeInfo::Type);
    Enqueue   enq(inf, "job_queue");
    Message   msg(inf);
    Object    job(inf);

    job.Set("title",   title);
    job.Set("content", content);
    msg.SetObject(job);
    enq.Put(msg);
    con.Commit();
    return 0;
}

static int cmd_jobs_work()
{
    Connection con = checkout();
    TypeInfo  inf(con, "job_t", TypeInfo::Type);
    Dequeue   deq(inf, "job_queue");

    while (Message msg = deq.Get())
    {
        Object job = msg.GetObject();
        std::cout << "job: " << job.Get<String>("title") << " — "
                  << job.Get<String>("content") << "\n";
    }
    con.Commit();
    return 0;
}

/* ---------------------------------------------------------------- profile */

static int cmd_profile(int empno)
{
    Connection con = checkout();
    Statement  st(con);

    st.Prepare("select profile from employees where empno = :e");
    st.Bind(":e", empno);
    st.Execute();

    Resultset rs = st.GetResultset();
    if (rs.FetchNext() && !rs.IsNull(1))
    {
        Json doc = rs.Get<Json>(0);
        JsonNode root = doc.GetRoot();

        String skill = "<none>";
        JsonNode skills = root["skills"];
        if (skills && skills.GetItemCount())
        {
            skill = skills[0].GetString();
        }
        std::cout << "emp " << empno << " — " << root.GetFieldCount()
                  << " field(s), first skill: " << skill << "\n";
    }
    else
    {
        std::cout << "no profile for empno " << empno << "\n";
    }
    return 0;
}

/* -------------------------------------------------------------------- main */

int main(int argc, char **argv)
{
    try
    {
        Environment env(Environment::Threaded | Environment::Events);
        env.SetSubscriptionPort(5468);

        Pool pool(env, getenv("OCILIB_DEMO_DB"), getenv("OCILIB_DEMO_USR"),
                  getenv("OCILIB_DEMO_PWD"), Pool::ConnectionPool, 2, 8, 1);
        g_pool = &pool;

        int rc = 1;
        const char *cmd = argc > 1 ? argv[1] : "";

        if (!strcmp(cmd, "list"))
            rc = cmd_list(argc > 3 && !strcmp(argv[2], "--dept") ? atoi(argv[3]) : 0);
        else if (!strcmp(cmd, "get") && argc > 2)
            rc = cmd_get(atoi(argv[2]));
        else if (!strcmp(cmd, "add") && argc > 6)
            rc = cmd_add(atoi(argv[2]), argv[3], argv[4], atof(argv[5]), atoi(argv[6]));
        else if (!strcmp(cmd, "raise") && argc > 3)
            rc = cmd_raise(atoi(argv[2]), atof(argv[3]));
        else if (!strcmp(cmd, "import") && argc > 2)
            rc = cmd_import(argv[2]);
        else if (!strcmp(cmd, "watch"))
            rc = cmd_watch();
        else if (!strcmp(cmd, "jobs") && argc > 2 && !strcmp(argv[2], "work"))
            rc = cmd_jobs_work();
        else if (!strcmp(cmd, "jobs") && argc > 4 && !strcmp(argv[2], "submit"))
            rc = cmd_jobs_submit(argv[3], argv[4]);
        else if (!strcmp(cmd, "profile") && argc > 2)
            rc = cmd_profile(atoi(argv[2]));
        else
            std::cerr << "usage: empdir list|get|add|raise|import|watch|jobs|profile ...\n";

        return rc;
    }
    catch (const std::exception &ex)
    {
        std::cerr << ex.what() << "\n";
        return EXIT_FAILURE;
    }
}
```
:::

## What to notice in the code

- **Pool checkout is scoped** — every `cmd_*` takes a connection at the top and
  releases it at the bottom (`OCI_ConnectionFree` in C, destructor in C++).
- **No exception handling in C++ handlers** — `ocilib::Exception` propagates to
  the single `catch` in `main`; the C version relies on the error handler instead.
- **Same objects, two grammars** — statement → resultset → row getters, bind by
  name, commit on the connection. The C++ version is shorter only because RAII
  removes the `Free` calls and `Get<T>` removes the getter table.
- **`OCI_ENV_EVENTS` + port** are required before any subscription works — that's
  why they live next to `OCI_PoolCreate` in `main`.

## Try it yourself

1. Compile the C version (`cl /std:c11` or `gcc -std=c11 -locilib`) and run
   `list`, `get`, `add` against `tutorial-schema.sql`.
2. Extend `import` with a `--mode=dirpath` switch using `OCI_DirectPath` (page 815).
3. Change `watch` to `OCI_CNT_ROWS` and observe rowids instead of objects.
4. Add `empdir export <empno>` writing the employee as a JSON document.

## Recap

- `empdir` is one file, one pool, eight commands — every part of the API you
  learned shows up in a real role.
- C and C++ share the architecture; only the plumbing differs.

**Done.** The appendices are your quick-reference shelf from here.
