/*
* OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
*
* Website: http://www.ocilib.net
*
* Copyright (c) 2007-2016 Vincent ROGIER <vince.rogier@ocilib.net>
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "ocilib.hpp"

#ifdef _MSC_VER

#if defined(OCI_CHARSET_WIDE)
#pragma comment(lib, "ocilibw.lib")
#elif defined(OCI_CHARSET_ANSI)
#pragma comment(lib, "ociliba.lib")
#endif

#endif

using namespace ocilib;

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

/* --------------------------------------------------------------------------------------------- *
* CONSTANTS
* --------------------------------------------------------------------------------------------- */

const int ArgDatabase = 1;
const int ArgUser = 2;
const int ArgPassword = 3;
const int ArgHome = 4;
const int ArgCount = 5;
const int SizeString = 260;
const int SizeBuffer = 2048;
const int ElemCount = 5;

const int DirPathBufferSize = 64000;
const int DirPathLoadSize = 100;
const int DirPathLoadCount = 10;
const int DirPathSizeColumn1 = 20;
const int DirPathSizeColumn2 = 30;
const int DirPathSizeColumn3 = 8;
const int DirPathColumnCount = 3;

/* --------------------------------------------------------------------------------------------- *
* DEFINES
* --------------------------------------------------------------------------------------------- */

#ifndef OCI_SHARED_LIB
#if defined(_WINDOWS)
#if defined(OCI_CHARSET_WIDE)
#define OCI_SHARED_LIB                   "ocilibw.dll"
#else
#define OCI_SHARED_LIB                   "ociliba.dll"
#endif
#elif defined(__APPLE__)
#define OCI_SHARED_LIB                   "libocilib.dylib"
#elif defined(__hppa)
#define OCI_SHARED_LIB                   "libocilib.sl"
#else
#define OCI_SHARED_LIB                   "libocilib.so"
#endif
#endif

#define otext(s) OTEXT(s)
#define oendl  std::endl

#if defined(OCI_CHARSET_WIDE)
#if defined(_MSC_VER)
#define omain           wmain
#define oarg            otext
#define oarglen         wcslen
#else
#define omain           main
#define oarg            char
#define oarglen         strlen
#endif
#else
#define omain             main
#define oarg              char
#define oarglen           strlen
#endif

#if defined(OCI_CHARSET_WIDE)
#define ocout             std::wcout
#define oostringstream    std::wostringstream
#else
#define ocout             std::cout
#define oostringstream    std::ostringstream
#endif

#define ARRAY_COUNT(t) (sizeof(t)/sizeof(t[0]))

/* --------------------------------------------------------------------------------------------- *
* LOCAL TYPES for the demo
* --------------------------------------------------------------------------------------------- */

struct Test
{
    void(*proc)(void); /* test procedure */
    int    execute;     /* do execute the procedure ? */
};

// User translation of the test_table SQL tables rows
struct Product
{
    int code;
    ostring action;
    double price;
    Date date;
};

// User translation of the test_array SQL table rows
struct TestArray
{
    int val_int;
    float val_flt;
    double val_dbl;
    ostring val_str;
    Date val_date;
    Clob val_lob;
    File val_file;
};


/* --------------------------------------------------------------------------------------------- *
* prototypes
* --------------------------------------------------------------------------------------------- */

void print_version(void);
void create_tables(void);
void drop_tables(void);
void execute_ddl(ostring sql);

void test_format(void);
void test_immediate(void);
void test_immediate_format(void);
void test_fetch(void);
void test_fetch_translate(void);
void test_foreach(void);
void test_foreach_translate(void);
void test_bind1(void);
void test_bind2(void);
void test_piecewise_insert(void);
void test_piecewise_fetch(void);
void test_lob(void);
void test_nested_table(void);
void test_ref_cursor(void);
void test_plsql(void);
void test_dates(void);
void test_timestamp(void);
void test_describe(void);
void test_returning(void);
void test_returning_array(void);
void test_object_insert(void);
void test_object_fetch(void);
void test_scrollable_cursor(void);
void test_collection(void);
void test_ref(void);
void test_directpath(void);

Product CreateProductFromQuery(const Resultset &rs);
bool FillProductFromQuery(const Resultset &rs, Product &p);
bool PrintProductFromQuery(const Resultset &rs);
bool PrintProductFromObject(const Product &p);

/* ocilib test functions array */

Test tab_test[] =
{
    { test_fetch, TRUE },
    { test_fetch_translate, TRUE },
    { test_foreach, TRUE },
    { test_foreach_translate, TRUE },
    { test_bind1, TRUE },
    { test_bind2, TRUE },
    { test_piecewise_insert, TRUE },
    { test_piecewise_fetch, TRUE },
    { test_lob, TRUE },
    { test_nested_table, TRUE },
    { test_ref_cursor, TRUE },
    { test_plsql, TRUE },
    { test_dates, TRUE },
    { test_timestamp, TRUE },
    { test_describe, TRUE },
    { test_returning, TRUE },
    { test_returning_array, TRUE },
    { test_object_insert, TRUE },
    { test_object_fetch, TRUE },
    { test_scrollable_cursor, TRUE },
    { test_collection, TRUE },
    { test_ref, TRUE },
    { test_directpath, TRUE }
};

/* --------------------------------------------------------------------------------------------- *
* variables
* --------------------------------------------------------------------------------------------- */

static Connection con;

inline ostring GetArg(oarg *arg)
{
    ostring res;

    if (arg)
    {
        size_t i = 0, size = oarglen(arg);

        res.resize(size);

        while (i < size) { res[i] = static_cast<char>(arg[i]); i++; }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
* main
* --------------------------------------------------------------------------------------------- */

int omain(int argc, oarg* argv[])
{
    ostring home;
    ostring dbs;
    ostring usr;
    ostring pwd;

    size_t i;

    /* CHECK COMMAND LINE --------------------------------------------------- */

    if (argc < (ArgCount - 1))
    {
        return EXIT_FAILURE;
    }

    /* GET ARGUMENTS ---------------------------------------------------------*/

    dbs = GetArg(argv[ArgDatabase]);
    usr = GetArg(argv[ArgUser]);
    pwd = GetArg(argv[ArgPassword]);

    if (argc == ArgCount)
    {
        home = GetArg(argv[ArgHome]);
    }

    try
    {
        Environment::Initialize(Environment::Default | Environment::Threaded, home);

        Environment::EnableWarnings(true);

        ocout << otext("Connecting to ") << usr << otext("/") << pwd << otext("@") << dbs << oendl << oendl;

        con.Open(dbs, usr, pwd, Environment::SessionDefault);

        print_version();
        create_tables();

        /* execute tests */

        for (i = 0; i < ARRAY_COUNT(tab_test); i++)
        {
            if (tab_test[i].execute)
                tab_test[i].proc();
        }

        drop_tables();
        con.Close();
    }
    catch (std::exception &ex)
    {
        ocout << ex.what() << oendl;
    }

    if (con)
    {
        drop_tables();
        con.Close();
    }

    Environment::Cleanup();

    ocout << otext("\nPress any key to exit...");

    getchar();

    return EXIT_SUCCESS;
}


/* --------------------------------------------------------------------------------------------- *
* Helpers methods
* --------------------------------------------------------------------------------------------- */

bool FillProductFromQuery(const Resultset &rs, Product &p)
{
    p.code = rs.Get<int>(1);
    p.action = rs.Get<ostring>(2);
    p.price = rs.Get<double>(3);
    p.date = rs.Get<Date>(4);


    return true;
}

Product CreateProductFromQuery(const Resultset &rs)
{
    Product p;

    rs.Get(1, p.code);
    rs.Get(2, p.action);
    rs.Get(3, p.price);
    rs.Get(4, p.date);

    return p;
}

bool PrintProductFromQuery(const Resultset &rs)
{
    ocout << otext("> code   : ") << rs.Get<int>(1);
    ocout << otext(", action : ") << rs.Get<ostring>(2);
    ocout << otext(", price  : ") << rs.Get<double>(3);
    ocout << otext(", date   : ") << rs.Get<Date>(4);
    ocout << oendl;
    return true;
}

bool PrintProductFromObject(const Product &p)
{
    ocout << otext("> code   : ") << p.code;
    ocout << otext(", action : ") << p.action;
    ocout << otext(", price  : ") << p.price;
    ocout << otext(", date   : ") << p.date;
    ocout << oendl;
    return true;
}


/* --------------------------------------------------------------------------------------------- *
* print_version
* --------------------------------------------------------------------------------------------- */

void print_version(void)
{
    ocout << otext("\n>>>>> OCILIB BUILD INFORMATION \n\n");

    if (Environment::GetImportMode() == Environment::ImportLinkage)
        ocout << otext("OCI import mode         : LINKAGE\n");
    else
        ocout << otext("OCI import mode         : RUNTIME\n");

    if (Environment::GetCharset() == Environment::CharsetAnsi)
        ocout << otext("Charset type            : ANSI\n");
    else
        ocout << otext("Charset type            : WIDE\n");

    ocout << otext("\n>>>>> VERSIONS INFORMATION \n\n");
    ocout << otext("OCILIB major    version : ") << OCILIB_MAJOR_VERSION << oendl;
    ocout << otext("OCILIB minor    version : ") << OCILIB_MINOR_VERSION << oendl;
    ocout << otext("OCILIB revision version : ") << OCILIB_REVISION_VERSION << oendl;
    ocout << otext("OCI compile     version : ") << Environment::GetCompileMajorVersion() << oendl;
    ocout << otext("OCI runtime     version : ") << Environment::GetRuntimeMajorVersion() << oendl;
    ocout << otext("Server major    version : ") << con.GetServerMajorVersion() << oendl;
    ocout << otext("Server minor    version : ") << con.GetServerMinorVersion() << oendl;
    ocout << otext("Server revision version : ") << con.GetServerRevisionVersion() << oendl;
    ocout << otext("Connection      version : ") << static_cast<unsigned int>(con.GetVersion()) << oendl;

    ocout << otext("\n>>>>> SERVER VERSION BANNER \n\n");
    ocout << con.GetServerVersion() << oendl << oendl;
}

/* --------------------------------------------------------------------------------------------- *
* execute_ddl
* --------------------------------------------------------------------------------------------- */

void execute_ddl(ostring sql)
{
    try
    {
        Statement st(con);
        st.Execute(sql);
    }
    catch (Exception &ex)
    {
        switch (ex.GetType().GetValue())
        {
        case Exception::OracleError:
            ocout << otext("Oracle Error => ");
            break;
        case Exception::OracleWarning:
            ocout << otext("Oracle Error => ");
            break;
        case Exception::OcilibError:
            ocout << otext("OCILIB Error => ");
            break;
        default:
            ocout << otext("Unknown Error => ");
            break;
        }

        ocout << ex.what() << oendl;
    }
}

/* --------------------------------------------------------------------------------------------- *
* create_tables
* --------------------------------------------------------------------------------------------- */

void create_tables(void)
{
    ocout << otext("\n>>>>> CREATE TABLES FOR DEMO \n\n");

    /* create types for the demo */
    execute_ddl(otext("create type type_t as OBJECT (id int, name varchar2(50))"));

    execute_ddl(otext("create type test_t as object ")
        otext("( ")
        otext("    val_int  number, ")
        otext("    val_flt  float, ")
        otext("    val_dbl  float, ")
        otext("    val_str  varchar2(30), ")
        otext("    val_date date, ")
        otext("    val_lob  clob, ")
        otext("    val_file bfile, ")
        otext("    val_obj  type_t, ")
        otext("    val_raw  raw(10) ")
        otext(")"));

    execute_ddl(otext("create type t_tab1_emp as VARRAY(100) of varchar2(50)"));

    execute_ddl(otext("create type t_tab2_emp as table of varchar2(50)"));

    /* create table for the demo */
    execute_ddl(otext("create table test_fetch(code int, article ")
        otext("varchar2(30), price float, creation date)"));

    execute_ddl(otext("create table test_long_raw(code int, content long raw)"));

    execute_ddl(otext("create table test_long_str(code int, content long)"));

    execute_ddl(otext("create table test_lob(code int, content clob)"));

    execute_ddl(otext("create table test_object(val test_t)"));

    execute_ddl(otext("create table test_table_obj of type_t"));

    execute_ddl(otext("create table test_array ")
        otext("( ")
        otext("    val_int  number, ")
        otext("    val_dbl  float, ")
        otext("    val_flt  float, ")
        otext("    val_str  varchar2(30), ")
        otext("    val_date date, ")
        otext("    val_lob  clob, ")
        otext("    val_file bfile ")
        otext(")")
        );

    execute_ddl(otext("create table test_coll_varray ")
        otext("( ")
        otext("    departement number, ")
        otext("    employees   t_tab1_emp ")
        otext(")")
        );

    execute_ddl(otext("create table test_coll_nested ")
        otext("( ")
        otext("    departement number, ")
        otext("    employees   t_tab2_emp ")
        otext(") nested table employees store as test_table_emp")
        );

    execute_ddl(otext("create table test_directpath(val_int number(8,4), ")
        otext(" val_str varchar2(30), val_date date)"));

    /* insert data into the demo tables */
    execute_ddl(otext("insert into test_fetch ")
        otext("(code, article, price, creation) ")
        otext("values (1, 'shoes', 3.14, to_date('1978-12-23', 'YYYY-MM-DD'))"));

    execute_ddl(otext("insert into test_fetch ")
        otext("(code, article, price, creation) ")
        otext("values (2, 'shirt', 5.99, to_date('1999-09-12', 'YYYY-MM-DD'))"));

    execute_ddl(otext("insert into test_lob(code,content)  ")
        otext("values (1, EMPTY_CLOB())"));

    execute_ddl(otext("insert into test_long_str(code,content) ")
        otext("values (1, 'Rugby rocks !')"));

    execute_ddl(otext("insert into test_coll_varray(departement,employees) ")
        otext("values (1, t_tab1_emp('Peter', 'John', 'Paula', 'Gina'))"));

    execute_ddl(otext("insert into test_coll_varray(departement,employees) ")
        otext("values (2, t_tab1_emp('Ben', 'Alice', 'Joel', 'Maria'))"));

    execute_ddl(otext("insert into test_coll_nested(departement,employees) ")
        otext("values (1, t_tab2_emp('Vince', 'Richard', 'Rita', 'Sophia'))"));

    execute_ddl(otext("insert into test_coll_nested(departement,employees) ")
        otext("values (2, t_tab2_emp('Paul', 'Sarah', 'Robert', 'Zoe'))"));

    execute_ddl(otext("insert into test_table_obj values(type_t(1, 'shoes'))"));
    execute_ddl(otext("insert into test_table_obj values(type_t(2, 'pen'))"));

    con.Commit();
}

/* --------------------------------------------------------------------------------------------- *
* drop_tables
* --------------------------------------------------------------------------------------------- */

void drop_tables(void)
{
    ocout << otext("\n>>>>> DROPPING TABLES AND TYPES \n\n");

    execute_ddl(otext("drop table test_fetch"));
    execute_ddl(otext("drop table test_long_str"));
    execute_ddl(otext("drop table test_long_raw"));
    execute_ddl(otext("drop table test_lob"));
    execute_ddl(otext("drop table test_array"));
    execute_ddl(otext("drop table test_object"));
    execute_ddl(otext("drop table test_coll_varray"));
    execute_ddl(otext("drop table test_coll_nested"));
    execute_ddl(otext("drop table test_table_obj"));
    execute_ddl(otext("drop table test_directpath"));
    execute_ddl(otext("drop type  test_t"));
    execute_ddl(otext("drop type  type_t"));
    execute_ddl(otext("drop type  t_tab1_emp"));
    execute_ddl(otext("drop type  t_tab2_emp"));
}

/* --------------------------------------------------------------------------------------------- *
* test_fetch
* --------------------------------------------------------------------------------------------- */

void test_fetch(void)
{
    ocout << otext("\n>>>>> SIMPLE TEST FETCH WITH META DATA\n\n");

    Statement st(con);
    st.Execute(otext("select * from test_fetch"));

    Resultset rs = st.GetResultset();
    for (int i = 1, n = rs.GetColumnCount(); i <= n; i++)
    {
        ocout << otext("> Field : #") << i << otext(" - Name : ") << rs.GetColumn(i).GetName() << oendl;
    }

    ocout << oendl;

    while (rs++)
    {
        PrintProductFromQuery(rs);
    }

    ocout << oendl << rs.GetCount() << otext(" row(s) fetched") << oendl;
}

/* --------------------------------------------------------------------------------------------- *
* test_fetch_translate
* --------------------------------------------------------------------------------------------- */

void test_fetch_translate(void)
{
    ocout << otext("\n>>>>> SIMPLE TEST FETCH  WITH ROW TRANSLATION TO USER TYPE \n\n");

    Statement st(con);
    st.Execute(otext("select * from test_fetch"));

    Resultset rs = st.GetResultset();

    ocout << oendl;

    while (rs++)
    {
        Product p;

        if (rs.Get(p, FillProductFromQuery))
        {
            PrintProductFromObject(p);
        }
    }

    ocout << oendl << rs.GetCount() << otext(" row(s) fetched") << oendl;
}

/* --------------------------------------------------------------------------------------------- *
* test_foreach
* --------------------------------------------------------------------------------------------- */

void test_foreach(void)
{
    ocout << otext("\n>>>>> FOREACH FETCH\n\n");

    Statement st(con);
    st.Execute(otext("select * from test_fetch"), PrintProductFromQuery);
}

/* --------------------------------------------------------------------------------------------- *
* test_foreach_translate
* --------------------------------------------------------------------------------------------- */

void test_foreach_translate(void)
{
    ocout << otext("\n>>>>> FOREACH FETCH WITH TRANSLATION\n\n");

    Statement st(con);
    st.Execute(otext("select * from test_fetch"), PrintProductFromObject, CreateProductFromQuery);
}

/* --------------------------------------------------------------------------------------------- *
* test_bind1
* --------------------------------------------------------------------------------------------- */

void test_bind1(void)
{
    unsigned int code = 1, count = 0;

    ocout << otext("\n>>>>> TEST BINDING \n\n");

    Statement st(con);
    st.Prepare(otext("select * from test_fetch where code = :code"));
    st.Bind(otext(":code"), code, BindInfo::In);
    count = st.ExecutePrepared(PrintProductFromObject, CreateProductFromQuery);

    ocout << oendl << count << otext(" row(s) fetched") << oendl;
}

/* --------------------------------------------------------------------------------------------- *
* test_bind2
* --------------------------------------------------------------------------------------------- */

void test_bind2(void)
{
    ocout << otext("\n>>>>> SINGLE BINDING \n\n");

    Statement st(con);
    st.Prepare(otext("insert into test_array ")
        otext("( ")
        otext("   val_int,  val_dbl, val_flt, val_str, val_date, ")
        otext("   val_lob, val_file ")
        otext(") ")
        otext("values ")
        otext("( ")
        otext("   :val_int, :val_dbl, :val_flt, :val_str, :val_date, ")
        otext("   :val_lob, :val_file ")
        otext(") "));

    /* create objects and set values */

    TestArray t;

    t.val_date = Date::SysDate();
    t.val_int = 1;
    t.val_dbl = 3.14;
    t.val_flt = 3.14f;
    t.val_str = otext("Name00");
    t.val_lob = Clob(con);
    t.val_file = File(con);

    t.val_lob.Write(otext("lob value00"));
    t.val_file.SetInfos(otext("mydir"), otext("file00.txt"));

    /* bind scalar C types arrays */

    st.Bind(otext(":val_int"), t.val_int, BindInfo::In);
    st.Bind(otext(":val_dbl"), t.val_dbl, BindInfo::In);
    st.Bind(otext(":val_flt"), t.val_flt, BindInfo::In);
    st.Bind(otext(":val_str"), t.val_str, static_cast<unsigned int>(t.val_str.size()), BindInfo::In);

    /* bind oracle types arrays */

    st.Bind(otext(":val_date"), t.val_date, BindInfo::In);
    st.Bind(otext(":val_lob"), t.val_lob, BindInfo::In);
    st.Bind(otext(":val_file"), t.val_file, BindInfo::In);

    /* do insert */

    st.ExecutePrepared();
    con.Commit();

    ocout << oendl << st.GetAffectedRows() << otext(" row(s) inserted") << oendl;
}

/* --------------------------------------------------------------------------------------------- *
* test_piecewise_insert
* --------------------------------------------------------------------------------------------- */

void test_piecewise_insert(void)
{
    ocout << otext("\n>>>>> TEST PIECEWISE INSERTING\n\n");

    std::ifstream file(OCI_SHARED_LIB, std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open())
    {
        size_t size = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios::beg);
        ocout << oendl << size << otext(" bytes to write") << oendl;

        Statement st(con);
        Blong lg(st);
        st.Prepare(otext("insert into test_long_raw(code, content) values (1, :data)"));
        st.SetLongMaxSize(static_cast<unsigned int>(size));
        st.Bind(otext(":data"), lg, static_cast<unsigned int>(size), BindInfo::In);
        st.ExecutePrepared();

        unsigned char *strBuffer = new unsigned char[size];
        file.read(reinterpret_cast<char *>(strBuffer), size);
        lg.Write(Raw(strBuffer, strBuffer + size));

        delete[] strBuffer;

        ocout << oendl << lg.GetLength() << otext(" bytes written") << oendl;

        file.close();
        con.Commit();
    }
}

/* --------------------------------------------------------------------------------------------- *
* test_piecewise_fetch
* --------------------------------------------------------------------------------------------- */

void test_piecewise_fetch(void)
{
    ocout << otext("\n>>>>> TEST PIECEWISE FETCHING\n\n");

    Statement st(con);
    st.Execute(otext("select content from test_long_raw where code = 1"));

    Resultset rs1 = st.GetResultset();
    while (rs1++)
    {
        Blong lg = rs1.Get<Blong>(1);
        ocout << oendl << lg.GetLength() << otext(" bytes read") << oendl;
    }

    ocout << oendl << rs1.GetCount() << otext(" row(s) fetched") << oendl;

    ocout << otext("\n>>>>> TEST LONG MAPPED TO STRING\n\n");

    st.Execute(otext("select content from test_long_str where code = 1"));
    st.SetLongMode(Statement::LongImplicit);

    Resultset rs2 = st.GetResultset();
    while (rs2++)
    {
        ostring str = rs2.Get<ostring>(1);

        ocout << str << oendl;
        ocout << str.size() << otext(" bytes read") << oendl;
    }

    ocout << oendl << rs2.GetCount() << otext(" row(s) fetched") << oendl;
}

/* --------------------------------------------------------------------------------------------- *
* test_lob
* --------------------------------------------------------------------------------------------- */

void test_lob(void)
{
    ocout << otext("\n>>>>> TEST LOB MANIPULATION\n\n");

    Statement st(con);
    st.Execute(otext("select code, content from test_lob where code=1 for update"));

    Resultset rs = st.GetResultset();
    while (rs++)
    {
        Clob clob = rs.Get<Clob>(2);

        clob.Write(otext("today, "));
        clob.Append(otext("i'm going to the cinema ! "));
        clob.Seek(SeekSet, 0);

        ocout << otext("> code : ") << rs.Get<int>(1) << otext(", content : ") << clob.Read(SizeString) << oendl;
    }

    con.Commit();

    ocout << oendl << rs.GetCount() << otext(" row(s) fetched") << oendl;
}

/* --------------------------------------------------------------------------------------------- *
* test_nested_table
* --------------------------------------------------------------------------------------------- */

void test_nested_table(void)
{
    ocout << otext("\n>>>>> TEST NESTED TABLE \n\n");

    Statement st(con);
    st.Execute(otext("select article, cursor(select sysdate from dual) from test_fetch"));

    Resultset rs = st.GetResultset();
    while (rs++)
    {
        Statement st2 = rs.Get<Statement>(2);
        Resultset rs2 = st2.GetResultset();
        while (rs2++)
        {
            ocout << otext("Article : ") << rs.Get<ostring>(1) << otext(", Date : ") << rs2.Get<ostring>(1) << oendl;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
* test_ref_cursor
* --------------------------------------------------------------------------------------------- */

void test_ref_cursor(void)
{
    ocout << otext("\n>>>>> TEST REF CURSOR  \n\n");

    Statement stBind(con);

    Statement st(con);
    st.Prepare(otext("begin open :c for select * from test_fetch; end;"));
    st.Bind(otext(":c"), stBind, BindInfo::Out);
    st.ExecutePrepared();

    Resultset rs = stBind.GetResultset();
    while (rs++)
    {
        PrintProductFromQuery(rs);
    }

    ocout << oendl << rs.GetCount() << otext(" row(s) fetched") << oendl;
}

/* --------------------------------------------------------------------------------------------- *
* test_plsql
* --------------------------------------------------------------------------------------------- */

void test_plsql(void)
{
    ocout << otext("\n>>>>> TEST PL/SQL OUTPUT BIND\n\n");

    int res = 0;
    Statement st(con);

    st.Prepare(otext("begin :res := trunc(sysdate+1)-trunc(sysdate-1); end;"));
    st.Bind(otext(":res"), res, BindInfo::Out);
    st.ExecutePrepared();
    ocout << otext("PL/SQL : trunc(sysdate+1)-trunc(sysdate-1)") << oendl;
    ocout << otext("Result : ") << res << oendl;

#ifndef OCI_CHARSET_ANSI
    /* Oracle 8i has some troubles with SERVER OUTPUT in unicode */
    if (Environment::GetRuntimeVersion() < Oracle9iR1)
        return;
#endif

    ocout << otext("\n>>>>> TEST PL/SQL SERVER OUTPUT\n\n");

    /* server output */

    con.EnableServerOutput(32000, 5, 255);

    st.Execute(otext("begin ")
        otext("   dbms_output.put_line('First  line'); ")
        otext("   dbms_output.put_line('Second line'); ")
        otext("   dbms_output.put_line('Third  line'); ")
        otext("end;")
        );

    ostring line;
    while (con.GetServerOutput(line))
    {
        ocout << line << oendl;
    }

    con.DisableServerOutput();
}

/* --------------------------------------------------------------------------------------------- *
* test_dates
* --------------------------------------------------------------------------------------------- */

void test_dates(void)
{
    ocout << otext("\n>>>>> TEST DATETIME MANIPULATION\n\n");

    Date date(otext("1978-04-13"));
    ocout << date << oendl;

    ocout << date.SysDate() << oendl;

    date.AddDays(5);
    date.AddMonths(2);

    ocout << otext("Date + 5 days and 2 months is ") << date << oendl;
    ocout << otext("Last day of the month : ") << date.LastDay() << oendl;
    ocout << otext("Number of days until the end of the months : ") << date.LastDay().DaysBetween(date) << oendl;
}

/* --------------------------------------------------------------------------------------------- *
* test_timestamp
* --------------------------------------------------------------------------------------------- */

void test_timestamp(void)
{
    unsigned int version = Environment::GetRuntimeVersion();

#ifndef OCI_CHARSET_ANSI
    /* Oracle 9i has some troubles with formatting Intervals/timestamps in
    an UTF16 context... */

    if ((version >= Oracle9iR1) || (version < Oracle10gR1))
        return;
#endif

    if (version >= Oracle9iR1)
    {
        ocout << otext("\n>>>>> TEST TIMESTAMP\n\n");

        Timestamp tm = Timestamp::SysTimestamp();

        ocout << otext("Current timestamp: ") << tm << oendl;

        /* intervals raw oci functions have some troubles with Oracle 9i. So let's
        use it for the demo only if we're using 10g or above */

        if (version >= Oracle10gR1)
        {
            ocout << otext("\n>>>>> TEST INTERVAL \n\n");

            Interval itv(Interval::DaySecond);
            itv.SetDaySecond(1, 1, 1, 1, 0);
            tm += itv;
            ocout << otext("Current timestamp + Interval: ") << tm << oendl;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
* test_describe
* --------------------------------------------------------------------------------------------- */

void test_describe(void)
{
    ocout << otext("\n>>>>> TEST DESCRIBING TABLE \n\n");
    ocout << otext("Column Name         Type                ") << oendl;
    ocout << otext("----------------------------------------") << oendl;
    ocout << std::setiosflags(std::ios::left);

    TypeInfo table(con, otext("test_fetch"), TypeInfo::Table);

    for (int i = 1, n = table.GetColumnCount(); i <= n; i++)
    {
        Column col = table.GetColumn(i);

        ocout << std::setw(20) << col.GetName().c_str()
            << std::setw(20) << col.GetFullSQLType().c_str()
            << oendl;
    }

    /* TEST DESCRIBING TYPE ------------------------------------------------- */

    ocout << otext("\n>>>>> TEST DESCRIBING TYPE \n\n");
    ocout << otext("Column Name         Type                ") << oendl;
    ocout << otext("----------------------------------------") << oendl;
    ocout << std::setiosflags(std::ios::left);

    TypeInfo type(con, otext("test_t"), TypeInfo::Type);

    for (int i = 1, n = type.GetColumnCount(); i <= n; i++)
    {
        Column col = type.GetColumn(i);

        ocout << std::setw(20) << col.GetName().c_str()
            << std::setw(20) << col.GetFullSQLType().c_str()
            << oendl;
    }
}

/* --------------------------------------------------------------------------------------------- *
* test_returning
* --------------------------------------------------------------------------------------------- */

void test_returning(void)
{
    ocout << otext("\n>>>>> TEST RETURNING CLAUSE \n\n");

    Statement st(con);
    st.Prepare(otext("update test_lob set code = code + 1 returning code, content into :i, :l"));
    st.Register<int>(otext(":i"));
    st.Register<Clob>(otext(":l"));
    st.ExecutePrepared();

    Resultset rs = st.GetResultset();
    while (rs++)
    {
        Clob clob = rs.Get<Clob>(2);
        clob.Append(otext("(modified)"));
        clob.Seek(SeekSet, 0);

        ocout << otext("> code : ") << rs.Get<int>(1) << otext(" - ") << clob.Read(static_cast<unsigned int>(clob.GetLength())) << oendl;
    }

    con.Commit();

    ocout << oendl << rs.GetCount() << otext(" row(s) fetched") << oendl;
}

/* --------------------------------------------------------------------------------------------- *
* test_returning_array
* --------------------------------------------------------------------------------------------- */

void test_returning_array(void)
{
    ocout << otext("\n>>>>> TEST ARRAY BINDING WITH RETURNING CLAUSE \n\n");

    std::vector<int>     tab_int;
    std::vector<double>  tab_dbl;
    std::vector<float>   tab_flt;
    std::vector<ostring> tab_str;
    std::vector<Date>    tab_date;
    std::vector<Clob>    tab_lob;
    std::vector<File>    tab_file;


    for (int i = 0; i < ElemCount; i++)
    {
        tab_int.push_back(i + 1);
        tab_dbl.push_back(3.14*static_cast<double>(i + 1));
        tab_flt.push_back(3.14f*static_cast<float>(i + 1));

        ostring str;
        str += otext("Name");
        str += ((i + 1) + '0');
        tab_str.push_back(str);

        tab_date.push_back(Date::SysDate());

        Clob clob(con);
        clob.Write(otext("Lob value ") + str);
        tab_lob.push_back(clob);

        ostring fileName;
        fileName += otext("File");
        fileName += ((i + 1) + '0');
        File file(con, otext("Mydir"), fileName);
        tab_file.push_back(file);
    }

    Statement st(con);

    st.Prepare(otext("insert into test_array ")
        otext("( ")
        otext("   val_int,  val_dbl, val_flt, val_str, val_date, ")
        otext("   val_lob, val_file ")
        otext(") ")
        otext("values ")
        otext("( ")
        otext("   :val_int, :val_dbl, :val_flt, :val_str, :val_date, ")
        otext("   :val_lob, :val_file ")
        otext(") ")
        otext("returning")
        otext("  val_int,  val_dbl, val_flt, val_str, val_date, ")
        otext("   val_lob, val_file ")
        otext("into  ")
        otext("  :out_int, :out_dbl, :out_flt,  :out_str, :out_date, ")
        otext("   :out_lob, :out_file "));

    st.SetBindArraySize(ElemCount);

    /* bind vectors */
    st.Bind(otext(":val_int"), tab_int, BindInfo::In);
    st.Bind(otext(":val_dbl"), tab_dbl, BindInfo::In);
    st.Bind(otext(":val_flt"), tab_flt, BindInfo::In);
    st.Bind(otext(":val_date"), tab_date, BindInfo::In);
    st.Bind(otext(":val_lob"), tab_lob, BindInfo::In);
    st.Bind(otext(":val_file"), tab_file, BindInfo::In);
    st.Bind(otext(":val_str"), tab_str, 30, BindInfo::In);

    /* register output */
    st.Register<int    >(otext(":out_int"));
    st.Register<double >(otext(":out_dbl"));
    st.Register<float  >(otext(":out_flt"));
    st.Register<Date   >(otext(":out_date"));
    st.Register<Clob   >(otext(":out_lob"));
    st.Register<File   >(otext(":out_file"));
    st.Register<ostring>(otext(":out_str"), 30);

    st.ExecutePrepared();
    ocout << oendl << st.GetAffectedRows() << otext(" row(s) inserted") << oendl;

    int rowIndex = 0;

    Resultset rs = st.GetResultset();
    while (!rs.IsNull())
    {
        while (rs++)
        {
            ocout << otext("Row #") << ++rowIndex << otext("---------------") << oendl;

            ocout << otext(".... val_int    : ") << rs.Get<int>(otext(":OUT_INT")) << oendl;
            ocout << otext(".... val_dbl    : ") << rs.Get<double>(otext(":OUT_DBL")) << oendl;
            ocout << otext(".... val_flt    : ") << rs.Get<float>(otext(":OUT_FLT")) << oendl;
            ocout << otext(".... val_str    : ") << rs.Get<ostring>(otext(":OUT_STR")) << oendl;
            ocout << otext(".... val_date   : ") << rs.Get<Date>(otext(":OUT_DATE")) << oendl;

            Clob clob = rs.Get<Clob>(otext(":OUT_LOB"));
            ocout << otext(".... val_lob    : ") << clob.Read(SizeBuffer) << oendl;

            File file = rs.Get<File>(otext(":OUT_FILE"));
            ocout << otext(".... val_file   : ") << file.GetDirectory() << otext("/") << file.GetName() << oendl;
        }

        rs = st.GetNextResultset();
    }
}

/* --------------------------------------------------------------------------------------------- *
* test_object_insert
* --------------------------------------------------------------------------------------------- */

void test_object_insert(void)
{
    unsigned char *constData = reinterpret_cast<unsigned char*>(const_cast< char *>("0123456789"));
    Raw rawData(constData, constData + 10);

    ocout << otext("\n>>>>> TEST OBJECT BINDING \n\n");

    Clob clob(con);
    clob.Write(otext("Lob Value"));
    File file(con, otext("mydir"), otext("myfile"));
    Date date = Date::SysDate();

    Object obj2(TypeInfo(con, otext("type_t"), TypeInfo::Type));

    obj2.Set<int>(otext("ID"), 1);
    obj2.Set<ostring>(otext("NAME"), otext("USB KEY 2go"));

    Object obj1(TypeInfo(con, otext("test_t"), TypeInfo::Type));

    obj1.Set<int>(otext("VAL_INT"), 1);
    obj1.Set<double>(otext("VAL_DBL"), 3.14);
    obj1.Set<float>(otext("VAL_FLT"), static_cast<float>(3.14));
    obj1.Set<ostring>(otext("VAL_STR"), otext("USB KEY 2go"));
    obj1.Set<Raw>(otext("VAL_RAW"), rawData);
    obj1.Set<Date>(otext("VAL_DATE"), date);
    obj1.Set<Object>(otext("VAL_OBJ"), obj2);
    obj1.Set<Clob>(otext("VAL_LOB"), clob);
    obj1.Set<File>(otext("VAL_FILE"), file);

    Statement st(con);
    st.Prepare(otext("insert into test_object values(:obj)"));
    st.Bind(otext(":obj"), obj1, BindInfo::In);
    st.ExecutePrepared();

    ocout << otext("Rows inserted :  ") << st.GetAffectedRows() << oendl;

    con.Commit();
}

/* --------------------------------------------------------------------------------------------- *
* test_object_fetch
* --------------------------------------------------------------------------------------------- */

void test_object_fetch(void)
{
    ocout << otext("\n>>>>> TEST OBJECT FETCHING \n\n");

    Statement st(con);
    st.Execute(otext("select val from test_object for update"));

    Resultset rs = st.GetResultset();
    while (rs++)
    {
        Object obj = rs.Get<Object>(1);

        ocout << otext(".... val_int      : ") << obj.Get<int>(otext("VAL_INT")) << oendl;
        ocout << otext(".... val_dbl      : ") << obj.Get<double>(otext("VAL_DBL")) << oendl;
        ocout << otext(".... val_flt      : ") << obj.Get<float>(otext("VAL_FLT")) << oendl;
        ocout << otext(".... val_str      : ") << obj.Get<ostring>(otext("VAL_STR")) << oendl;
        ocout << otext(".... val_date     : ") << obj.Get<Date>(otext("VAL_DATE")) << oendl;

        Clob clob = obj.Get<Clob>(otext("VAL_LOB"));
        ocout << otext(".... val_lob      : ") << clob.Read(SizeBuffer) << oendl;

        File file = obj.Get<File>(otext("VAL_FILE"));
        ocout << otext(".... val_file     : ") << file.GetDirectory() << otext("/") << file.GetName() << oendl;

        Raw raw = obj.Get<Raw>(otext("VAL_RAW"));
        raw.push_back(0);
        ocout << otext(".... val_raw      : ") << reinterpret_cast<char *>(raw.data()) << oendl;

        Object obj2 = obj.Get<Object>(otext("VAL_OBJ"));
        ocout << otext(".... val_obj.code : ") << obj2.Get<int>(otext("ID")) << oendl;
        ocout << otext(".... val_obj.name : ") << obj2.Get<ostring>(otext("NAME")) << oendl;
    }

    con.Commit();

    ocout << otext("\n>>>>> TEST OBJECT FETCHING  AS STRING \n\n");

    st.Execute(otext("select val from test_object"));

    rs = st.GetResultset();
    while (rs++)
    {
        ocout << rs.Get<Object>(1) << oendl;
    }
}

/* --------------------------------------------------------------------------------------------- *
* test_scrollable_cursor
* --------------------------------------------------------------------------------------------- */

void test_scrollable_cursor(void)
{
    if (Environment::GetRuntimeVersion() > Oracle9iR1)
    {
        ocout << otext("\n>>>>> TEST SCROLLABLE CURSORS \n\n");

        Statement st(con);

        st.SetFetchMode(Statement::FetchScrollable);
        st.Execute(otext("select table_name from user_tables where ")
            otext("table_name like 'TEST_%' order by table_name"));

        Resultset rs = st.GetResultset();

        rs.Last();
        ocout << otext("Total rows : ") << rs.GetCount() << oendl;

        ocout << otext("... Go to row 1\n");
        rs.First();
        ocout << otext("table ") << rs.Get<ostring>(1) << oendl;

        ocout << otext("... Enumerate from row 2 to row ") << rs.GetCount() << otext(" ") << oendl;
        while (rs++)
        {
            ocout << otext("table ") << rs.Get<ostring>(1) << oendl;
        }

        ocout << otext("... Enumerate from row ") << rs.GetCount() - 1 << otext(" back to row 1") << oendl;
        while (rs.Prev())
        {
            ocout << otext("table ") << rs.Get<ostring>(1) << oendl;
        }

        ocout << otext("... Go to the 3th row") << oendl;
        rs.Seek(Resultset::SeekAbsolute, 3);
        ocout << otext("table ") << rs.Get<ostring>(1) << oendl;

        ocout << otext("... Fetch the next 2 rows") << oendl;
        while (rs.GetCurrentRow() < 5 && rs++)
        {
            ocout << otext("table ") << rs.Get<ostring>(1) << oendl;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
* test_collection
* --------------------------------------------------------------------------------------------- */
void test_collection(void)
{
    ocout << otext("\n>>>>> TEST VARRAY BINDING WITH ITERATOR \n\n");

    int i = 1;

    Statement st(con);
    TypeInfo type(con, otext("T_TAB1_EMP"), TypeInfo::Type);
    Collection<ostring> coll(type);

    st.Prepare(otext("begin")
        otext("    select employees into :tab_emp ")
        otext("    from test_coll_varray ")
        otext("    where departement = :id; ")
        otext("end;"));

    st.Bind(otext(":tab_emp"), coll, BindInfo::In);
    st.Bind(otext(":id"), i, BindInfo::In);
    st.ExecutePrepared();

    ocout << otext("Department ID #") << i << oendl;

    Collection<ostring>::iterator it1 = coll.begin();
    Collection<ostring>::iterator it2 = coll.end();

    for (; it1 != it2; ++it1)
    {
        ocout << otext("... Employee : ") << static_cast<ostring>(*it1) << oendl;
    }

    ocout << otext("\n>>>>> TEST VARRAY PRINTING \n\n");
    ocout << coll << oendl;

    ocout << otext("\n>>>>> TEST VARRAY FETCHING WITH ITERATOR \n\n");

    st.Execute(otext("SELECT * from test_coll_varray"));

    Resultset rs = st.GetResultset();
    while (rs++)
    {
        ocout << otext("Department ID #") << rs.Get<int>(1) << oendl;

        coll = rs.Get<Collection<ostring> >(2);

        Collection<ostring>::iterator it3 = coll.begin();
        Collection<ostring>::iterator it4 = coll.end();

        for (; it3 != it4; ++it3)
        {
            ocout << otext("... Employee : ") << static_cast<ostring>(*it3) << oendl;
        }
    }

    ocout << otext("\n>>>>> TEST NESTED TABLE FETCHING WITH INDEX ACCESS \n\n");

    st.Execute(otext("SELECT * from test_coll_nested"));

    rs = st.GetResultset();
    while (rs++)
    {
        ocout << otext("Department ID #") << rs.Get<int>(1) << oendl;

        coll = rs.Get<Collection<ostring> >(2);
        for (unsigned int index = 1, n = coll.GetSize(); index <= n; index++)
        {
            ocout << otext("... Employee : ") << static_cast<ostring>(coll[index]) << oendl;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
* test_ref
* --------------------------------------------------------------------------------------------- */
void test_ref(void)
{
    ocout << otext("\n>>>>> TEST REF FETCHING \n\n");

    Statement st(con);

    st.Execute(otext("select ref(e) from test_table_obj e"));

    Resultset rs = st.GetResultset();
    while (rs++)
    {
        Reference ref = rs.Get<Reference>(1);
        Object   obj = ref.GetObject();

        ocout << obj.Get<int>(otext("ID")) << otext(" - ") << obj.Get<ostring>(otext("NAME")) << oendl;
    }

    ocout << otext("\n>>>>> TEST REF PL/SQL BINDING \n\n");

    Reference ref(TypeInfo(con, otext("type_t"), TypeInfo::Type));

    st.Prepare(otext("begin ")
        otext("  select ref(e) into :r from test_table_obj e where e.id = 1; ")
        otext("end; "));

    st.Bind(otext(":r"), ref, BindInfo::InOut);
    st.ExecutePrepared();

    Object obj = ref.GetObject();
    ocout << obj.Get<int>(otext("ID")) << otext(" - ") << obj.Get<ostring>(otext("NAME")) << oendl;
}


/* --------------------------------------------------------------------------------------------- *
* test_directpath
* --------------------------------------------------------------------------------------------- */

void test_directpath(void)
{
    /* Some OCI Direct path function fails (segfault) if the OCI version of the
    client is different than the server one.
    It happens with OCI 8i and 9i client. Apparently Oracle 10g and 11g seems to
    have fixed these problems.
    Anyway, we run this test case only if the major versions of client and server
    match
    */
    // if (Environment::GetRuntimeMajorVersion() == con.GetServerMajorVersion())
    {
        /* commit any previous pending modifications */

        con.Commit();

        ocout << otext("\n>>>>> TEST DIRECT PATH (10 loads of 100 rows) \n\n");

        int i = 0, j = 0, n = DirPathLoadSize;

        DirectPath dp(TypeInfo(con, otext("test_directpath"), TypeInfo::Table), DirPathColumnCount, n);

        /* optional attributes to set */

        dp.SetBufferSize(DirPathBufferSize);
        dp.SetNoLog(true);
        dp.SetParallel(true);

        /* describe the target table */

        dp.SetColumn(1, otext("VAL_INT"), DirPathSizeColumn1);
        dp.SetColumn(2, otext("VAL_STR"), DirPathSizeColumn2);
        dp.SetColumn(3, otext("VAL_DATE"), DirPathSizeColumn3, otext("YYYYMMDD"));

        /* prepare the load */

        dp.Prepare();

        n = dp.GetMaxRows();

        for (i = 0; i < DirPathLoadCount; i++)
        {
            dp.Reset();

            for (j = 1; j <= n; j++)
            {
                oostringstream val1, val2, val3;

                /* fill test values */

                val1 << std::setfill(otext('0')) << std::setw(4) << j + (i * DirPathLoadCount);
                val2 << otext("value ") << std::setfill(otext('0')) << std::setw(5) << j + (i * DirPathLoadCount);
                val3 << std::setfill(otext('0')) << std::setw(2) << (j % 23) + 1 + 2000 << std::setw(2) << (j % 11) + 1 << (j % 23) + 1;

                dp.SetEntry(j, 1, val1.str());
                dp.SetEntry(j, 2, val2.str());
                dp.SetEntry(j, 3, val3.str());
            }

            /* load data to the server */
            dp.Convert();
            dp.Load();
        }

        /* commits changes */

        dp.Finish();

        ocout << std::setw(4) << dp.GetRowCount() << otext(" row(s) loaded") << oendl;
    }
}
