/*
+-----------------------------------------------------------------------------------------+
|                                                                                         |
|                               OCILIB - C Driver for Oracle                              |
|                                                                                         |
|                                (C Wrapper for Oracle OCI)                               |
|                                                                                         |
|                              Website : http://www.ocilib.net                            |
|                                                                                         |
|             Copyright (c) 2007-2015 Vincent ROGIER <vince.rogier@ocilib.net>            |
|                                                                                         |
+-----------------------------------------------------------------------------------------+
|                                                                                         |
|             This library is free software; you can redistribute it and/or               |
|             modify it under the terms of the GNU Lesser General Public                  |
|             License as published by the Free Software Foundation; either                |
|             version 2 of the License, or (at your option) any later version.            |
|                                                                                         |
|             This library is distributed in the hope that it will be useful,             |
|             but WITHOUT ANY WARRANTY; without even the implied warranty of              |
|             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           |
|             Lesser General Public License for more details.                             |
|                                                                                         |
|             You should have received a copy of the GNU Lesser General Public            |
|             License along with this library; if not, write to the Free                  |
|             Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.          |
|                                                                                         |
+-----------------------------------------------------------------------------------------+
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

#define text(s) OTEXT(s)
#define endl  std::endl

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
#define cout             std::wcout
#define ostringstream    std::wostringstream
#else
#define cout             std::cout
#define ostringstream    std::ostringstream
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

        cout << text("Connecting to ") << usr << text("/") << pwd << text("@") << dbs << endl << endl;

        con.Open(dbs, usr, pwd, Environment::SessionDefault);

        print_version();
        drop_tables();
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
        cout << ex.what() << endl;
    }

    if (con)
    {
        drop_tables();
        con.Close();
    }

    Environment::Cleanup();

    cout << text("\nPress any key to exit...");

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
    cout << text("> code   : ") << rs.Get<int>(1);
    cout << text(", action : ") << rs.Get<ostring>(2);
    cout << text(", price  : ") << rs.Get<double>(3);
    cout << text(", date   : ") << rs.Get<Date>(4);
    cout << endl;
    return true;
}

bool PrintProductFromObject(const Product &p)
{
    cout << text("> code   : ") << p.code;
    cout << text(", action : ") << p.action;
    cout << text(", price  : ") << p.price;
    cout << text(", date   : ") << p.date;
    cout << endl;
    return true;
}


/* --------------------------------------------------------------------------------------------- *
* print_version
* --------------------------------------------------------------------------------------------- */

void print_version(void)
{
    cout << text("\n>>>>> OCILIB BUILD INFORMATION \n\n");

    if (Environment::GetImportMode() == Environment::ImportLinkage)
        cout << text("OCI import mode         : LINKAGE\n");
    else
        cout << text("OCI import mode         : RUNTIME\n");

    if (Environment::GetCharset() == Environment::CharsetAnsi)
        cout << text("Charset type            : ANSI\n");
    else
        cout << text("Charset type            : WIDE\n");

    cout << text("\n>>>>> VERSIONS INFORMATION \n\n");
    cout << text("OCILIB major    version : ") << OCILIB_MAJOR_VERSION << endl;
    cout << text("OCILIB minor    version : ") << OCILIB_MINOR_VERSION << endl;
    cout << text("OCILIB revision version : ") << OCILIB_REVISION_VERSION << endl;
    cout << text("OCI compile     version : ") << Environment::GetCompileMajorVersion() << endl;
    cout << text("OCI runtime     version : ") << Environment::GetRuntimeMajorVersion() << endl;
    cout << text("Server major    version : ") << con.GetServerMajorVersion() << endl;
    cout << text("Server minor    version : ") << con.GetServerMinorVersion() << endl;
    cout << text("Server revision version : ") << con.GetServerRevisionVersion() << endl;
    cout << text("Connection      version : ") << static_cast<unsigned int>(con.GetVersion()) << endl;

    cout << text("\n>>>>> SERVER VERSION BANNER \n\n");
    cout << con.GetServerVersion() << endl << endl;
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
            cout << text("Oracle Error => ");
            break;
        case Exception::OracleWarning:
            cout << text("Oracle Error => ");
            break;
        case Exception::OcilibError:
            cout << text("OCILIB Error => ");
            break;
        default:
            cout << text("Unknown Error => ");
            break;
        }

        cout << ex.what() << endl;
    }
}

/* --------------------------------------------------------------------------------------------- *
* create_tables
* --------------------------------------------------------------------------------------------- */

void create_tables(void)
{
    cout << text("\n>>>>> CREATE TABLES FOR DEMO \n\n");

    /* create types for the demo */
    execute_ddl(text("create type type_t as OBJECT (id int, name varchar2(50))"));

    execute_ddl(text("create type test_t as object ")
        text("( ")
        text("    val_int  number, ")
        text("    val_flt  float, ")
        text("    val_dbl  float, ")
        text("    val_str  varchar2(30), ")
        text("    val_date date, ")
        text("    val_lob  clob, ")
        text("    val_file bfile, ")
        text("    val_obj  type_t, ")
        text("    val_raw  raw(10) ")
        text(")"));

    execute_ddl(text("create type t_tab1_emp as VARRAY(100) of varchar2(50)"));

    execute_ddl(text("create type t_tab2_emp as table of varchar2(50)"));

    /* create table for the demo */
    execute_ddl(text("create table test_fetch(code int, article ")
        text("varchar2(30), price float, creation date)"));

    execute_ddl(text("create table test_long_raw(code int, content long raw)"));

    execute_ddl(text("create table test_long_str(code int, content long)"));

    execute_ddl(text("create table test_lob(code int, content clob)"));

    execute_ddl(text("create table test_object(val test_t)"));

    execute_ddl(text("create table test_table_obj of type_t"));

    execute_ddl(text("create table test_array ")
        text("( ")
        text("    val_int  number, ")
        text("    val_dbl  float, ")
        text("    val_flt  float, ")
        text("    val_str  varchar2(30), ")
        text("    val_date date, ")
        text("    val_lob  clob, ")
        text("    val_file bfile ")
        text(")")
        );

    execute_ddl(text("create table test_coll_varray ")
        text("( ")
        text("    departement number, ")
        text("    employees   t_tab1_emp ")
        text(")")
        );

    execute_ddl(text("create table test_coll_nested ")
        text("( ")
        text("    departement number, ")
        text("    employees   t_tab2_emp ")
        text(") nested table employees store as test_table_emp")
        );

    execute_ddl(text("create table test_directpath(val_int number(8,4), ")
        text(" val_str varchar2(30), val_date date)"));

    /* insert data into the demo tables */
    execute_ddl(text("insert into test_fetch ")
        text("(code, article, price, creation) ")
        text("values (1, 'shoes', 3.14, to_date('1978-12-23', 'YYYY-MM-DD'))"));

    execute_ddl(text("insert into test_fetch ")
        text("(code, article, price, creation) ")
        text("values (2, 'shirt', 5.99, to_date('1999-09-12', 'YYYY-MM-DD'))"));

    execute_ddl(text("insert into test_lob(code,content)  ")
        text("values (1, EMPTY_CLOB())"));

    execute_ddl(text("insert into test_long_str(code,content) ")
        text("values (1, 'Rugby rocks !')"));

    execute_ddl(text("insert into test_coll_varray(departement,employees) ")
        text("values (1, t_tab1_emp('Peter', 'John', 'Paula', 'Gina'))"));

    execute_ddl(text("insert into test_coll_varray(departement,employees) ")
        text("values (2, t_tab1_emp('Ben', 'Alice', 'Joel', 'Maria'))"));

    execute_ddl(text("insert into test_coll_nested(departement,employees) ")
        text("values (1, t_tab2_emp('Vince', 'Richard', 'Rita', 'Sophia'))"));

    execute_ddl(text("insert into test_coll_nested(departement,employees) ")
        text("values (2, t_tab2_emp('Paul', 'Sarah', 'Robert', 'Zoe'))"));

    execute_ddl(text("insert into test_table_obj values(type_t(1, 'shoes'))"));
    execute_ddl(text("insert into test_table_obj values(type_t(2, 'pen'))"));

    con.Commit();
}

/* --------------------------------------------------------------------------------------------- *
* drop_tables
* --------------------------------------------------------------------------------------------- */

void drop_tables(void)
{
    cout << text("\n>>>>> DROPPING TABLES AND TYPES \n\n");

    execute_ddl(text("drop table test_fetch"));
    execute_ddl(text("drop table test_long_str"));
    execute_ddl(text("drop table test_long_raw"));
    execute_ddl(text("drop table test_lob"));
    execute_ddl(text("drop table test_array"));
    execute_ddl(text("drop table test_object"));
    execute_ddl(text("drop table test_coll_varray"));
    execute_ddl(text("drop table test_coll_nested"));
    execute_ddl(text("drop table test_table_obj"));
    execute_ddl(text("drop table test_directpath"));
    execute_ddl(text("drop type  test_t"));
    execute_ddl(text("drop type  type_t"));
    execute_ddl(text("drop type  t_tab1_emp"));
    execute_ddl(text("drop type  t_tab2_emp"));
}

/* --------------------------------------------------------------------------------------------- *
* test_fetch
* --------------------------------------------------------------------------------------------- */

void test_fetch(void)
{
    cout << text("\n>>>>> SIMPLE TEST FETCH WITH META DATA\n\n");

    Statement st(con);
    st.Execute(text("select * from test_fetch"));

    Resultset rs = st.GetResultset();
    for (int i = 1, n = rs.GetColumnCount(); i <= n; i++)
    {
        cout << text("> Field : #") << i << text(" - Name : ") << rs.GetColumn(i).GetName() << endl;
    }

    cout << endl;

    while (rs++)
    {
        PrintProductFromQuery(rs);
    }

    cout << endl << rs.GetCount() << text(" row(s) fetched") << endl;
}

/* --------------------------------------------------------------------------------------------- *
* test_fetch_translate
* --------------------------------------------------------------------------------------------- */

void test_fetch_translate(void)
{
    cout << text("\n>>>>> SIMPLE TEST FETCH  WITH ROW TRANSLATION TO USER TYPE \n\n");

    Statement st(con);
    st.Execute(text("select * from test_fetch"));

    Resultset rs = st.GetResultset();

    cout << endl;

    while (rs++)
    {
        Product p;

        if (rs.Get(p, FillProductFromQuery))
        {
            PrintProductFromObject(p);
        }
    }

    cout << endl << rs.GetCount() << text(" row(s) fetched") << endl;
}

/* --------------------------------------------------------------------------------------------- *
* test_foreach
* --------------------------------------------------------------------------------------------- */

void test_foreach(void)
{
    cout << text("\n>>>>> FOREACH FETCH\n\n");

    Statement st(con);
    st.Execute(text("select * from test_fetch"), PrintProductFromQuery);
}

/* --------------------------------------------------------------------------------------------- *
* test_foreach_translate
* --------------------------------------------------------------------------------------------- */

void test_foreach_translate(void)
{
    cout << text("\n>>>>> FOREACH FETCH WITH TRANSLATION\n\n");

    Statement st(con);
    st.Execute(text("select * from test_fetch"), PrintProductFromObject, CreateProductFromQuery);
}

/* --------------------------------------------------------------------------------------------- *
* test_bind1
* --------------------------------------------------------------------------------------------- */

void test_bind1(void)
{
    unsigned int code = 1, count = 0;

    cout << text("\n>>>>> TEST BINDING \n\n");

    Statement st(con);
    st.Prepare(text("select * from test_fetch where code = :code"));
    st.Bind(text(":code"), code, BindInfo::In);
    count = st.ExecutePrepared(PrintProductFromObject, CreateProductFromQuery);

    cout << endl << count << text(" row(s) fetched") << endl;
}

/* --------------------------------------------------------------------------------------------- *
* test_bind2
* --------------------------------------------------------------------------------------------- */

void test_bind2(void)
{
    cout << text("\n>>>>> SINGLE BINDING \n\n");

    Statement st(con);
    st.Prepare(text("insert into test_array ")
        text("( ")
        text("   val_int,  val_dbl, val_flt, val_str, val_date, ")
        text("   val_lob, val_file ")
        text(") ")
        text("values ")
        text("( ")
        text("   :val_int, :val_dbl, :val_flt, :val_str, :val_date, ")
        text("   :val_lob, :val_file ")
        text(") "));

    /* create objects and set values */

    TestArray t;

    t.val_date = Date::SysDate();
    t.val_int  = 1;
    t.val_dbl  = 3.14;
    t.val_flt  = 3.14f;
    t.val_str  = text("Name00");
    t.val_lob  = Clob(con);
    t.val_file = File(con);

    t.val_lob.Write(text("lob value00"));
    t.val_file.SetInfos(text("mydir"), text("file00.txt"));

    /* bind scalar C types arrays */

    st.Bind(text(":val_int"), t.val_int, BindInfo::In);
    st.Bind(text(":val_dbl"), t.val_dbl, BindInfo::In);
    st.Bind(text(":val_flt"), t.val_flt, BindInfo::In);
    st.Bind(text(":val_str"), t.val_str, static_cast<unsigned int>(t.val_str.size()), BindInfo::In);

    /* bind oracle types arrays */

    st.Bind(text(":val_date"), t.val_date, BindInfo::In);
    st.Bind(text(":val_lob"), t.val_lob, BindInfo::In);
    st.Bind(text(":val_file"), t.val_file, BindInfo::In);

    /* do insert */

    st.ExecutePrepared();
    con.Commit();

    cout << endl << st.GetAffectedRows() << text(" row(s) inserted") << endl;
}

/* --------------------------------------------------------------------------------------------- *
* test_piecewise_insert
* --------------------------------------------------------------------------------------------- */

void test_piecewise_insert(void)
{
    cout << text("\n>>>>> TEST PIECEWISE INSERTING\n\n");

    std::ifstream file(OCI_SHARED_LIB, std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open())
    {
        size_t size = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios::beg);
        cout << endl << size << text(" bytes to write") << endl;

        Statement st(con);
        Blong lg(st);
        st.Prepare(text("insert into test_long_raw(code, content) values (1, :data)"));
        st.SetLongMaxSize(static_cast<unsigned int>(size));
        st.Bind(text(":data"), lg, static_cast<unsigned int>(size), BindInfo::In);
        st.ExecutePrepared();

        unsigned char *strBuffer = new unsigned char[size];
        file.read(reinterpret_cast<char *>(strBuffer), size);
        lg.Write(Raw(strBuffer, strBuffer + size));

        delete[] strBuffer;

        cout << endl << lg.GetLength() << text(" bytes written") << endl;

        file.close();
        con.Commit();
    }
}

/* --------------------------------------------------------------------------------------------- *
* test_piecewise_fetch
* --------------------------------------------------------------------------------------------- */

void test_piecewise_fetch(void)
{
    cout << text("\n>>>>> TEST PIECEWISE FETCHING\n\n");

    Statement st(con);
    st.Execute(text("select content from test_long_raw where code = 1"));

    Resultset rs1 = st.GetResultset();
    while (rs1++)
    {
        Blong lg = rs1.Get<Blong>(1);
        cout << endl << lg.GetLength() << text(" bytes read") << endl;
    }

    cout << endl << rs1.GetCount() << text(" row(s) fetched") << endl;

    cout << text("\n>>>>> TEST LONG MAPPED TO STRING\n\n");

    st.Execute(text("select content from test_long_str where code = 1"));
    st.SetLongMode(Statement::LongImplicit);

    Resultset rs2 = st.GetResultset();
    while (rs2++)
    {
        ostring str = rs2.Get<ostring>(1);

        cout << str << endl;
        cout << str.size() << text(" bytes read") << endl;
    }

    cout << endl << rs2.GetCount() << text(" row(s) fetched") << endl;
}

/* --------------------------------------------------------------------------------------------- *
* test_lob
* --------------------------------------------------------------------------------------------- */

void test_lob(void)
{
    cout << text("\n>>>>> TEST LOB MANIPULATION\n\n");

    Statement st(con);
    st.Execute(text("select code, content from test_lob where code=1 for update"));

    Resultset rs = st.GetResultset();
    while (rs++)
    {
        Clob clob = rs.Get<Clob>(2);

        clob.Write(text("today, "));
        clob.Append(text("i'm going to the cinema ! "));
        clob.Seek(SeekSet, 0);

        cout << text("> code : ") << rs.Get<int>(1) << text(", content : ") << clob.Read(SizeString) << endl;
    }

    con.Commit();

    cout << endl << rs.GetCount() << text(" row(s) fetched") << endl;
}

/* --------------------------------------------------------------------------------------------- *
* test_nested_table
* --------------------------------------------------------------------------------------------- */

void test_nested_table(void)
{
    cout << text("\n>>>>> TEST NESTED TABLE \n\n");

    Statement st(con);
    st.Execute(text("select article, cursor(select sysdate from dual) from test_fetch"));

    Resultset rs = st.GetResultset();
    while (rs++)
    {
        Statement st2 = rs.Get<Statement>(2);
        Resultset rs2 = st2.GetResultset();
        while (rs2++)
        {
            cout << text("Article : ") << rs.Get<ostring>(1) << text(", Date : ") << rs2.Get<ostring>(1) << endl;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
* test_ref_cursor
* --------------------------------------------------------------------------------------------- */

void test_ref_cursor(void)
{
    cout << text("\n>>>>> TEST REF CURSOR  \n\n");

    Statement stBind(con);

    Statement st(con);
    st.Prepare(text("begin open :c for select * from test_fetch; end;"));
    st.Bind(text(":c"), stBind, BindInfo::Out);
    st.ExecutePrepared();

    Resultset rs = stBind.GetResultset();
    while (rs++)
    {
        PrintProductFromQuery(rs);
    }

    cout << endl << rs.GetCount() << text(" row(s) fetched") << endl;
}

/* --------------------------------------------------------------------------------------------- *
* test_plsql
* --------------------------------------------------------------------------------------------- */

void test_plsql(void)
{
    cout << text("\n>>>>> TEST PL/SQL OUTPUT BIND\n\n");

    int res = 0;
    Statement st(con);

    st.Prepare(text("begin :res := trunc(sysdate+1)-trunc(sysdate-1); end;"));
    st.Bind(text(":res"), res, BindInfo::Out);
    st.ExecutePrepared();
    cout << text("PL/SQL : trunc(sysdate+1)-trunc(sysdate-1)") << endl;
    cout << text("Result : ") << res << endl;

#ifndef OCI_CHARSET_ANSI
    /* Oracle 8i has some troubles with SERVER OUTPUT in unicode */
    if (Environment::GetRuntimeVersion() < Oracle9iR1)
        return;
#endif

    cout << text("\n>>>>> TEST PL/SQL SERVER OUTPUT\n\n");

    /* server output */

    con.EnableServerOutput(32000, 5, 255);

    st.Execute(text("begin ")
        text("   dbms_output.put_line('First  line'); ")
        text("   dbms_output.put_line('Second line'); ")
        text("   dbms_output.put_line('Third  line'); ")
        text("end;")
        );

    ostring line;
    while (con.GetServerOutput(line))
    {
        cout << line << endl;
    }

    con.DisableServerOutput();
}

/* --------------------------------------------------------------------------------------------- *
* test_dates
* --------------------------------------------------------------------------------------------- */

void test_dates(void)
{
    cout << text("\n>>>>> TEST DATETIME MANIPULATION\n\n");

    Date date(text("1978-04-13"));
    cout << date << endl;

    date.SysDate();
    cout << date << endl;

    date.AddDays(5);
    date.AddMonths(2);

    cout << text("Date + 5 days and 2 months is ") << date << endl;
    cout << text("Last day of the month : ") << date.LastDay() << endl;
    cout << text("Number of days until the end of the months : ") << date.LastDay().DaysBetween(date) << endl;
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
        cout << text("\n>>>>> TEST TIMESTAMP\n\n");

        Timestamp tm = Timestamp::SysTimestamp();

        cout << text("Current timestamp: ") << tm << endl;

        /* intervals raw oci functions have some troubles with Oracle 9i. So let's
        use it for the demo only if we're using 10g or above */

        if (version >= Oracle10gR1)
        {
            cout << text("\n>>>>> TEST INTERVAL \n\n");

            Interval itv(Interval::DaySecond);
            itv.SetDaySecond(1, 1, 1, 1, 0);
            tm += itv;
            cout << text("Current timestamp + Interval: ") << tm << endl;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
* test_describe
* --------------------------------------------------------------------------------------------- */

void test_describe(void)
{
    cout << text("\n>>>>> TEST DESCRIBING TABLE \n\n");
    cout << text("Column Name         Type                ") << endl;
    cout << text("----------------------------------------") << endl;
    cout << std::setiosflags(std::ios::left);

    TypeInfo table(con, text("test_fetch"), TypeInfo::Table);

    for (int i = 1, n = table.GetColumnCount(); i <= n; i++)
    {
        Column col = table.GetColumn(i);

        cout << std::setw(20) << col.GetName().c_str()
            << std::setw(20) << col.GetFullSQLType().c_str()
            << endl;
    }

    /* TEST DESCRIBING TYPE ------------------------------------------------- */

    cout << text("\n>>>>> TEST DESCRIBING TYPE \n\n");
    cout << text("Column Name         Type                ") << endl;
    cout << text("----------------------------------------") << endl;
    cout << std::setiosflags(std::ios::left);

    TypeInfo type(con, text("test_t"), TypeInfo::Type);

    for (int i = 1, n = type.GetColumnCount(); i <= n; i++)
    {
        Column col = type.GetColumn(i);

        cout << std::setw(20) << col.GetName().c_str()
            << std::setw(20) << col.GetFullSQLType().c_str()
            << endl;
    }
}

/* --------------------------------------------------------------------------------------------- *
* test_returning
* --------------------------------------------------------------------------------------------- */

void test_returning(void)
{
    cout << text("\n>>>>> TEST RETURNING CLAUSE \n\n");

    Statement st(con);
    st.Prepare(text("update test_lob set code = code + 1 returning code, content into :i, :l"));
    st.Register<int>(text(":i"));
    st.Register<Clob>(text(":l"));
    st.ExecutePrepared();

    Resultset rs = st.GetResultset();
    while (rs++)
    {
        Clob clob = rs.Get<Clob>(2);
        clob.Append(text("(modified)"));
        clob.Seek(SeekSet, 0);

        cout << text("> code : ") << rs.Get<int>(1) << text(" - ") << clob.Read(static_cast<unsigned int>(clob.GetLength())) << endl;
    }

    con.Commit();

    cout << endl << rs.GetCount() << text(" row(s) fetched") << endl;
}

/* --------------------------------------------------------------------------------------------- *
* test_returning_array
* --------------------------------------------------------------------------------------------- */

void test_returning_array(void)
{
    cout << text("\n>>>>> TEST ARRAY BINDING WITH RETURNING CLAUSE \n\n");

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
        str += text("Name");
        str += ((i + 1) + '0');
        tab_str.push_back(str);

        tab_date.push_back(Date::SysDate());

        Clob clob(con);
        clob.Write(text("Lob value ") + str);
        tab_lob.push_back(clob);

        ostring fileName;
        fileName += text("File");
        fileName += ((i + 1) + '0');
        File file(con, text("Mydir"), fileName);
        tab_file.push_back(file);
    }

    Statement st(con);

    st.Prepare(text("insert into test_array ")
        text("( ")
        text("   val_int,  val_dbl, val_flt, val_str, val_date, ")
        text("   val_lob, val_file ")
        text(") ")
        text("values ")
        text("( ")
        text("   :val_int, :val_dbl, :val_flt, :val_str, :val_date, ")
        text("   :val_lob, :val_file ")
        text(") ")
        text("returning")
        text("  val_int,  val_dbl, val_flt, val_str, val_date, ")
        text("   val_lob, val_file ")
        text("into  ")
        text("  :out_int, :out_dbl, :out_flt,  :out_str, :out_date, ")
        text("   :out_lob, :out_file "));

    st.SetBindArraySize(ElemCount);

    /* bind vectors */
    st.Bind(text(":val_int"), tab_int, BindInfo::In);
    st.Bind(text(":val_dbl"), tab_dbl, BindInfo::In);
    st.Bind(text(":val_flt"), tab_flt, BindInfo::In);
    st.Bind(text(":val_date"), tab_date, BindInfo::In);
    st.Bind(text(":val_lob"), tab_lob, BindInfo::In);
    st.Bind(text(":val_file"), tab_file, BindInfo::In);
    st.Bind(text(":val_str"), tab_str, 30, BindInfo::In);

    /* register output */
    st.Register<int    >(text(":out_int"));
    st.Register<double >(text(":out_dbl"));
    st.Register<float  >(text(":out_flt"));
    st.Register<Date   >(text(":out_date"));
    st.Register<Clob   >(text(":out_lob"));
    st.Register<File   >(text(":out_file"));
    st.Register<ostring>(text(":out_str"), 30);

    st.ExecutePrepared();
    cout << endl << st.GetAffectedRows() << text(" row(s) inserted") << endl;

    int rowIndex = 0;

    Resultset rs = st.GetResultset();
    while (!rs.IsNull())
    {
        while (rs++)
        {
            cout << text("Row #") << ++rowIndex << text("---------------") << endl;

            cout << text(".... val_int    : ") << rs.Get<int>(text(":OUT_INT")) << endl;
            cout << text(".... val_dbl    : ") << rs.Get<double>(text(":OUT_DBL")) << endl;
            cout << text(".... val_flt    : ") << rs.Get<float>(text(":OUT_FLT")) << endl;
            cout << text(".... val_str    : ") << rs.Get<ostring>(text(":OUT_STR")) << endl;
            cout << text(".... val_date   : ") << rs.Get<Date>(text(":OUT_DATE")) << endl;

            Clob clob = rs.Get<Clob>(text(":OUT_LOB"));
            cout << text(".... val_lob    : ") << clob.Read(SizeBuffer) << endl;

            File file = rs.Get<File>(text(":OUT_FILE"));
            cout << text(".... val_file   : ") << file.GetDirectory() << text("/") << file.GetName() << endl;
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

    cout << text("\n>>>>> TEST OBJECT BINDING \n\n");

    Clob clob(con);
    clob.Write(text("Lob Value"));
    File file(con, text("mydir"), text("myfile"));
    Date date = Date::SysDate();

    Object obj2(TypeInfo(con, text("type_t"), TypeInfo::Type));

    obj2.Set<int>(text("ID"), 1);
    obj2.Set<ostring>(text("NAME"), text("USB KEY 2go"));

    Object obj1(TypeInfo(con, text("test_t"), TypeInfo::Type));

    obj1.Set<int>(text("VAL_INT"), 1);
    obj1.Set<double>(text("VAL_DBL"), 3.14);
    obj1.Set<float>(text("VAL_FLT"), (float) 3.14);
    obj1.Set<ostring>(text("VAL_STR"), text("USB KEY 2go"));
    obj1.Set<Raw>(text("VAL_RAW"), rawData);
    obj1.Set<Date>(text("VAL_DATE"), date);
    obj1.Set<Object>(text("VAL_OBJ"), obj2);
    obj1.Set<Clob>(text("VAL_LOB"), clob);
    obj1.Set<File>(text("VAL_FILE"), file);

    Statement st(con);
    st.Prepare(text("insert into test_object values(:obj)"));
    st.Bind(text(":obj"), obj1, BindInfo::In);
    st.ExecutePrepared();

    cout << text("Rows inserted :  ") << st.GetAffectedRows() << endl;

    con.Commit();
}

/* --------------------------------------------------------------------------------------------- *
* test_object_fetch
* --------------------------------------------------------------------------------------------- */

void test_object_fetch(void)
{
    cout << text("\n>>>>> TEST OBJECT FETCHING \n\n");

    Statement st(con);
    st.Execute(text("select val from test_object for update"));

    Resultset rs = st.GetResultset();
    while (rs++)
    {
        Object obj = rs.Get<Object>(1);

        cout << text(".... val_int      : ") << obj.Get<int>(text("VAL_INT")) << endl;
        cout << text(".... val_dbl      : ") << obj.Get<double>(text("VAL_DBL")) << endl;
        cout << text(".... val_flt      : ") << obj.Get<float>(text("VAL_FLT")) << endl;
        cout << text(".... val_str      : ") << obj.Get<ostring>(text("VAL_STR")) << endl;
        cout << text(".... val_date     : ") << obj.Get<Date>(text("VAL_DATE")) << endl;

        Clob clob = obj.Get<Clob>(text("VAL_LOB"));
        cout << text(".... val_lob      : ") << clob.Read(SizeBuffer) << endl;

        File file = obj.Get<File>(text("VAL_FILE"));
        cout << text(".... val_file     : ") << file.GetDirectory() << text("/") << file.GetName() << endl;

        Raw raw = obj.Get<Raw>(text("VAL_RAW"));
        raw.push_back(0);
        cout << text(".... val_raw      : ") << reinterpret_cast<char *>(raw.data()) << endl;

        Object obj2 = obj.Get<Object>(text("VAL_OBJ"));
        cout << text(".... val_obj.code : ") << obj2.Get<int>(text("ID")) << endl;
        cout << text(".... val_obj.name : ") << obj2.Get<ostring>(text("NAME")) << endl;
    }

    con.Commit();

    cout << text("\n>>>>> TEST OBJECT FETCHING  AS STRING \n\n");

    st.Execute(text("select val from test_object"));

    rs = st.GetResultset();
    while (rs++)
    {
        cout << rs.Get<Object>(1) << endl;
    }
}

/* --------------------------------------------------------------------------------------------- *
* test_scrollable_cursor
* --------------------------------------------------------------------------------------------- */

void test_scrollable_cursor(void)
{
    if (Environment::GetRuntimeVersion() > Oracle9iR1)
    {
        cout << text("\n>>>>> TEST SCROLLABLE CURSORS \n\n");

        Statement st(con);

        st.SetFetchMode(Statement::FetchScrollable);
        st.Execute(text("select table_name from user_tables where ")
            text("table_name like 'TEST_%' order by table_name"));

        Resultset rs = st.GetResultset();

        rs.Last();
        cout << text("Total rows : ") << rs.GetCount() << endl;

        cout << text("... Go to row 1\n");
        rs.First();
        cout << text("table ") << rs.Get<ostring>(1) << endl;

        cout << text("... Enumerate from row 2 to row ") << rs.GetCount() << text(" ") << endl;
        while (rs++)
        {
            cout << text("table ") << rs.Get<ostring>(1) << endl;
        }

        cout << text("... Enumerate from row ") << rs.GetCount() - 1 << text(" back to row 1") << endl;
        while (rs.Prev())
        {
            cout << text("table ") << rs.Get<ostring>(1) << endl;
        }

        cout << text("... Go to the 3th row") << endl;
        rs.Seek(Resultset::SeekAbsolute, 3);
        cout << text("table ") << rs.Get<ostring>(1) << endl;

        cout << text("... Fetch the next 2 rows") << endl;
        while (rs.GetCurrentRow() < 5 && rs++)
        {
            cout << text("table ") << rs.Get<ostring>(1) << endl;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
* test_collection
* --------------------------------------------------------------------------------------------- */
void test_collection(void)
{
    cout << text("\n>>>>> TEST VARRAY BINDING WITH ITERATOR \n\n");

    int i = 1;

    Statement st(con);
    TypeInfo type(con, text("T_TAB1_EMP"), TypeInfo::Type);
    Collection<ostring> coll(type);

    st.Prepare(text("begin")
        text("    select employees into :tab_emp ")
        text("    from test_coll_varray ")
        text("    where departement = :id; ")
        text("end;"));

    st.Bind(text(":tab_emp"), coll, BindInfo::In);
    st.Bind(text(":id"), i, BindInfo::In);
    st.ExecutePrepared();

    cout << text("Department ID #") << i << endl;

    Collection<ostring>::iterator it1 = coll.begin();
    Collection<ostring>::iterator it2 = coll.end();

    for (; it1 != it2; ++it1)
    {
        cout << text("... Employee : ") << static_cast<ostring>(*it1) << endl;
    }

    cout << text("\n>>>>> TEST VARRAY PRINTING \n\n");
    cout << coll << endl;

    cout << text("\n>>>>> TEST VARRAY FETCHING WITH ITERATOR \n\n");

    st.Execute(text("SELECT * from test_coll_varray"));

    Resultset rs = st.GetResultset();
    while (rs++)
    {
        cout << text("Department ID #") << rs.Get<int>(1) << endl;

        coll = rs.Get<Collection<ostring> >(2);

        Collection<ostring>::iterator it1 = coll.begin();
        Collection<ostring>::iterator it2 = coll.end();

        for (; it1 != it2; ++it1)
        {
            cout << text("... Employee : ") << static_cast<ostring>(*it1) << endl;
        }
    }

    cout << text("\n>>>>> TEST NESTED TABLE FETCHING WITH INDEX ACCESS \n\n");

    st.Execute(text("SELECT * from test_coll_nested"));

    rs = st.GetResultset();
    while (rs++)
    {
        cout << text("Department ID #") << rs.Get<int>(1) << endl;

        coll = rs.Get<Collection<ostring> >(2);
        for (unsigned int index = 1, n = coll.GetSize(); index <= n; index++)
        {
            cout << text("... Employee : ") << static_cast<ostring>(coll[index]) << endl;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
* test_ref
* --------------------------------------------------------------------------------------------- */
void test_ref(void)
{
    cout << text("\n>>>>> TEST REF FETCHING \n\n");

    Statement st(con);

    st.Execute(text("select ref(e) from test_table_obj e"));

    Resultset rs = st.GetResultset();
    while (rs++)
    {
        Reference ref = rs.Get<Reference>(1);
        Object   obj = ref.GetObject();

        cout << obj.Get<int>(text("ID")) << text(" - ") << obj.Get<ostring>(text("NAME")) << endl;
    }

    cout << text("\n>>>>> TEST REF PL/SQL BINDING \n\n");

    Reference ref(TypeInfo(con, text("type_t"), TypeInfo::Type));

    st.Prepare(text("begin ")
        text("  select ref(e) into :r from test_table_obj e where e.id = 1; ")
        text("end; "));

    st.Bind(text(":r"), ref, BindInfo::InOut);
    st.ExecutePrepared();

    Object obj = ref.GetObject();
    cout << obj.Get<int>(text("ID")) << text(" - ") << obj.Get<ostring>(text("NAME")) << endl;
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

        cout << text("\n>>>>> TEST DIRECT PATH (10 loads of 100 rows) \n\n");

        int i = 0, j = 0, n = DirPathLoadSize;

        DirectPath dp(TypeInfo(con, text("test_directpath"), TypeInfo::Table), DirPathColumnCount, n);

        /* optional attributes to set */

        dp.SetBufferSize(DirPathBufferSize);
        dp.SetNoLog(true);
        dp.SetParallel(true);

        /* describe the target table */

        dp.SetColumn(1, text("VAL_INT"), DirPathSizeColumn1);
        dp.SetColumn(2, text("VAL_STR"), DirPathSizeColumn2);
        dp.SetColumn(3, text("VAL_DATE"), DirPathSizeColumn3, text("YYYYMMDD"));

        /* prepare the load */

        dp.Prepare();

        n = dp.GetMaxRows();

        for (i = 0; i < DirPathLoadCount; i++)
        {
            dp.Reset();

            for (j = 1; j <= n; j++)
            {
                ostringstream val1, val2, val3;

                /* fill test values */

                val1 << std::setfill(text('0')) << std::setw(4) << j + (i * DirPathLoadCount);
                val2 << text("value ") << std::setfill(text('0')) << std::setw(5) << j + (i * DirPathLoadCount);
                val3 << std::setfill(text('0')) << std::setw(2) << (j % 23) + 1 + 2000 << std::setw(2) << (j % 11) + 1 << (j % 23) + 1;

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

        cout << std::setw(4) << dp.GetRowCount() << text(" row(s) loaded") << endl;
    }
}

