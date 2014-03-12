/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2013 Vincent ROGIER <vince.rogier@ocilib.net>            |
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
 * COMMON DEFINES
 * --------------------------------------------------------------------------------------------- */

#define ARG_DB     1
#define ARG_USER   2
#define ARG_PWD    3
#define ARG_HOME   4

#define ARG_COUNT  5

#define SIZE_STR   260
#define SIZE_BUF   2048
#define SIZE_TAB   5

#define SIZE_ARRAY 100
#define NB_LOAD    10
#define SIZE_COL1  20
#define SIZE_COL2  30
#define SIZE_COL3  8
#define NUM_COLS   3

#ifndef OCI_SHARED_LIB
    #if defined(_WINDOWS)
        #define OCI_SHARED_LIB                   "oci.dll"
    #elif defined(__APPLE__)
        #define OCI_SHARED_LIB                   "libclntsh.dylib"
    #elif defined(__hppa)
        #define OCI_SHARED_LIB                   "libclntsh.sl"
    #else
        #define OCI_SHARED_LIB                   "libclntsh.so"
    #endif
#endif

#if defined(OCI_CHARSET_WIDE)
  #if defined(_MSC_VER)
    #define omain           wmain
    #define oarg            otext
  #else
    #define omain           main
    #define oarg            char
  #endif
#else
  #define omain             main
  #define oarg              char
#endif

#if defined(OCI_CHARSET_WIDE)
  #define ocout             wcout        
  #define oostringstream    wostringstream 
#else
  #define ocout             cout        
  #define oostringstream    ostringstream 
#endif

#define ARRAY_COUNT(t) (sizeof(t)/sizeof(t[0]))

typedef struct test_t
{
        void (*proc)(void); /* test procedure */
        int    execute;     /* do execute the procedure ? */
}test_t;


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


/* ocilib test functions array */

test_t tab_test[] =
{
        {test_fetch,             TRUE},
        {test_bind1,             TRUE},
        {test_bind2,             TRUE},
        {test_piecewise_insert,  TRUE},
        {test_piecewise_fetch,   TRUE},
        {test_lob,               TRUE},
        {test_nested_table,      TRUE},
        {test_ref_cursor,        TRUE},
        {test_plsql,             TRUE},
        {test_dates,             TRUE},
        {test_timestamp,         TRUE},
        {test_describe,          TRUE},
        {test_returning,         TRUE},
        {test_returning_array,   TRUE},
        {test_object_insert,     TRUE},
        {test_object_fetch,      TRUE},
        {test_scrollable_cursor, TRUE},
        {test_collection,        TRUE},
        {test_ref,               TRUE},
        {test_directpath,        TRUE}
};

/* --------------------------------------------------------------------------------------------- *
 * variables
 * --------------------------------------------------------------------------------------------- */

static Connection con;

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

    if (argc < (ARG_COUNT-1))
    {
        return EXIT_FAILURE;
    }

    /* GET ARGUMENTS ---------------------------------------------------------*/

    dbs = argv[ARG_DB];
    usr = argv[ARG_USER];
    pwd = argv[ARG_PWD];

    if(argc == ARG_COUNT)
    {
        home = argv[ARG_HOME];
    }

    try
    {
        Environment::Initialize(Environment::EnvDefault, home);
        Environment::EnableWarnings(true);

        std::ocout << OTEXT("Connecting to ") << usr << OTEXT("/") << pwd << OTEXT("@") << dbs << std::endl << std::endl;

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
    catch(Exception &ex)
    {
         std::ocout << ex.GetMessage() << std::endl;
    }

    Environment::Cleanup();

    std::ocout << OTEXT("\nPress any key to exit...");

    getchar();

    return EXIT_SUCCESS;
}

/* --------------------------------------------------------------------------------------------- *
 * print_version
 * --------------------------------------------------------------------------------------------- */

void print_version(void)
{
    std::ocout << OTEXT("\n>>>>> OCILIB BUILD INFORMATION \n\n");

    if (Environment::GetImportMode() == OCI_IMPORT_MODE_LINKAGE)
        std::ocout << OTEXT("OCI import mode         : LINKAGE\n");
    else
        std::ocout << OTEXT("OCI import mode         : RUNTIME\n");

    if (Environment::GetCharset() == OCI_CHAR_ANSI)
        std::ocout << OTEXT("Charset type            : ANSI\n");
    else
        std::ocout << OTEXT("Charset type            : WIDE\n");

    std::ocout << OTEXT("\n>>>>> VERSIONS INFORMATION \n\n");
    std::ocout << OTEXT("OCILIB major    version : ") << OCILIB_MAJOR_VERSION     << std::endl;
    std::ocout << OTEXT("OCILIB minor    version : ") << OCILIB_MINOR_VERSION     << std::endl;
    std::ocout << OTEXT("OCILIB revision version : ") << OCILIB_REVISION_VERSION  << std::endl;
    std::ocout << OTEXT("OCI compile     version : ") << OCI_VER_MAJ(Environment::GetCompileVersion()) << std::endl;
    std::ocout << OTEXT("OCI runtime     version : ") << OCI_VER_MAJ(Environment::GetRuntimeVersion()) << std::endl;
    std::ocout << OTEXT("Server major    version : ") << con.GetServerMajorVersion()    << std::endl;
    std::ocout << OTEXT("Server minor    version : ") << con.GetServerMinorVersion()    << std::endl;
    std::ocout << OTEXT("Server revision version : ") << con.GetServerRevisionVersion() << std::endl;
    std::ocout << OTEXT("Connection      version : ") << con.GetVersion() << std::endl;

    std::ocout << OTEXT("\n>>>>> SERVER VERSION BANNER \n\n");
    std::ocout << con.GetServerVersion() << std::endl << std::endl;
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
    catch(Exception &ex)
    {
         std::ocout << ex.GetMessage() << std::endl;
    }
}

/* --------------------------------------------------------------------------------------------- *
 * create_tables
 * --------------------------------------------------------------------------------------------- */

void create_tables(void)
{
    std::ocout << OTEXT("\n>>>>> CREATE TABLES FOR DEMO \n\n");

    /* create types for the demo */
    execute_ddl( OTEXT("create type type_t as OBJECT (id int, name varchar2(50))"));

    execute_ddl( OTEXT("create type test_t as object ")
                OTEXT("( ")
                OTEXT("    val_int  number, ")
                OTEXT("    val_flt  float, ")
                OTEXT("    val_dbl  float, ")
                OTEXT("    val_str  varchar2(30), ")
                OTEXT("    val_date date, ")
                OTEXT("    val_lob  clob, ")
                OTEXT("    val_file bfile, ")
                OTEXT("    val_obj  type_t, ")
                OTEXT("    val_raw  raw(10) ")
                OTEXT(")"));

    execute_ddl(OTEXT("create type t_tab1_emp as VARRAY(100) of varchar2(50)"));

    execute_ddl(OTEXT("create type t_tab2_emp as table of varchar2(50)"));

    /* create table for the demo */
    execute_ddl( OTEXT("create table test_fetch(code int, article ")
                OTEXT("varchar2(30), price float, creation date)"));

    execute_ddl(OTEXT("create table test_long_raw(code int, content long raw)"));

    execute_ddl(OTEXT("create table test_long_str(code int, content long)"));

    execute_ddl(OTEXT("create table test_lob(code int, content clob)"));

    execute_ddl(OTEXT("create table test_object(val test_t)"));

    execute_ddl(OTEXT("create table test_table_obj of type_t"));

    execute_ddl( OTEXT("create table test_array ")
                OTEXT("( ")
                OTEXT("    val_int  number, ")
                OTEXT("    val_dbl  float, ")
                OTEXT("    val_flt  float, ")
                OTEXT("    val_str  varchar2(30), ")
                OTEXT("    val_date date, ")
                OTEXT("    val_lob  clob, ")
                OTEXT("    val_file bfile ")
                OTEXT(")")
            );

    execute_ddl (OTEXT("create table test_coll_varray ")
                OTEXT("( ")
                OTEXT("    departement number, ")
                OTEXT("    employees   t_tab1_emp ")
                OTEXT(")")
            );

    execute_ddl( OTEXT("create table test_coll_nested ")
                OTEXT("( ")
                OTEXT("    departement number, ")
                OTEXT("    employees   t_tab2_emp ")
                OTEXT(") nested table employees store as test_table_emp")
            );

    execute_ddl(OTEXT("create table test_directpath(val_int number(8,4), ")
                        OTEXT(" val_str varchar2(30), val_date date)"));

    /* insert data into the demo tables */
    execute_ddl( OTEXT("insert into test_fetch ")
                OTEXT("(code, article, price, creation) ")
                OTEXT("values (1, 'shoes', 3.14, to_date('1978-12-23', 'YYYY-MM-DD'))"));

    execute_ddl( OTEXT("insert into test_fetch ")
                OTEXT("(code, article, price, creation) ")
                OTEXT("values (2, 'shirt', 5.99, to_date('1999-09-12', 'YYYY-MM-DD'))"));

    execute_ddl(OTEXT("insert into test_lob(code,content)  ")
                        OTEXT("values (1, EMPTY_CLOB())"));

    execute_ddl( OTEXT("insert into test_long_str(code,content) ")
                OTEXT("values (1, 'Rugby rocks !')"));

    execute_ddl( OTEXT("insert into test_coll_varray(departement,employees) ")
                OTEXT("values (1, t_tab1_emp('Peter', 'John', 'Paula', 'Gina'))"));

    execute_ddl(OTEXT("insert into test_coll_varray(departement,employees) ")
                        OTEXT("values (2, t_tab1_emp('Ben', 'Alice', 'Joel', 'Maria'))"));

    execute_ddl( OTEXT("insert into test_coll_nested(departement,employees) ")
                OTEXT("values (1, t_tab2_emp('Vince', 'Richard', 'Rita', 'Sophia'))"));

    execute_ddl( OTEXT("insert into test_coll_nested(departement,employees) ")
                OTEXT("values (2, t_tab2_emp('Paul', 'Sarah', 'Robert', 'Zoe'))"));

    execute_ddl(OTEXT("insert into test_table_obj values(type_t(1, 'shoes'))"));
    execute_ddl(OTEXT("insert into test_table_obj values(type_t(2, 'pen'))"));

    con.Commit();
}

/* --------------------------------------------------------------------------------------------- *
 * drop_tables
 * --------------------------------------------------------------------------------------------- */

void drop_tables(void)
{
    std::ocout << OTEXT("\n>>>>> DROPPING TABLES AND TYPES \n\n");

    execute_ddl(OTEXT("drop table test_fetch"));
    execute_ddl(OTEXT("drop table test_long_str"));
    execute_ddl(OTEXT("drop table test_long_raw"));
    execute_ddl(OTEXT("drop table test_lob"));
    execute_ddl(OTEXT("drop table test_array"));
    execute_ddl(OTEXT("drop table test_object"));
    execute_ddl(OTEXT("drop table test_coll_varray"));
    execute_ddl(OTEXT("drop table test_coll_nested"));
    execute_ddl(OTEXT("drop table test_table_obj"));
    execute_ddl(OTEXT("drop table test_directpath"));
    execute_ddl(OTEXT("drop type  test_t"));
    execute_ddl(OTEXT("drop type  type_t"));
    execute_ddl(OTEXT("drop type  t_tab1_emp"));
    execute_ddl(OTEXT("drop type  t_tab2_emp"));
}

/* --------------------------------------------------------------------------------------------- *
 * test_fetch
 * --------------------------------------------------------------------------------------------- */

void test_fetch(void)
{
    std::ocout << OTEXT("\n>>>>> SIMPLE TEST FETCH WITH META DATA\n\n");

    Statement st(con);
    st.Execute(OTEXT("select * from test_fetch"));

    Resultset rs = st.GetResultset();
    for(int i = 1, n = rs.GetColumnCount(); i <= n; i++)
    {
        Column col = rs.GetColumn(i);
        std::ocout << OTEXT("> Field : #") << i << OTEXT(" - Name : ") << col.GetName() << std::endl;
    }

    std::ocout << std::endl;

    while (rs++)
    {
        std::ocout << OTEXT("> code   : ") << rs.Get<int>(1);
        std::ocout << OTEXT(", action : ") << rs.Get<ostring>(2);
        std::ocout << OTEXT(", price  : ") << rs.Get<double>(3);
        std::ocout << OTEXT(", date   : ");
        std::ocout << rs.Get<Date>(4).ToString() <<  std::endl;
    }

    std::ocout << std::endl << rs.GetCount() << OTEXT(" row(s) fetched") << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_bind1
 * --------------------------------------------------------------------------------------------- */

void test_bind1(void)
{
    int code = 1;

    std::ocout << OTEXT("\n>>>>> TEST BINDING \n\n");

    Statement st(con);
    st.Prepare(OTEXT("select * from test_fetch where code = :code"));
    st.Bind(OTEXT(":code"), code);
    st.Execute();

    Resultset rs = st.GetResultset();
    while (rs++)
    {
        std::ocout << OTEXT("> code   : ") << rs.Get<int>(1);
        std::ocout << OTEXT(", action : ") << rs.Get<ostring>(2);
        std::ocout << OTEXT(", price  : ") << rs.Get<double>(3);
        std::ocout << OTEXT(", date   : ");
        std::ocout << rs.Get<Date>(4).ToString() <<  std::endl;
    }

    std::ocout << std::endl << rs.GetCount() << OTEXT(" row(s) fetched") << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_bind2
 * --------------------------------------------------------------------------------------------- */

void test_bind2(void)
{
    std::ocout << OTEXT("\n>>>>> SINGLE BINDING \n\n");

    Statement st(con);
    st.Prepare( OTEXT("insert into test_array ")
                OTEXT("( ")
                OTEXT("   val_int,  val_dbl, val_flt, val_str, val_date, ")
                OTEXT( "   val_lob, val_file ")
                OTEXT( ") " )
                OTEXT( "values ")
                OTEXT( "( ")
                OTEXT( "   :val_int, :val_dbl, :val_flt, :val_str, :val_date, ")
                OTEXT( "   :val_lob, :val_file ")
                OTEXT(") "));

    /* create objects and set values */

    /* date */
    Date date;
    date.SysDate();

    date++;

    /* lob */
    Clob clob(con);
    clob.Write(OTEXT("lob value00"));

    /* file */
    File file(con);
    file.SetInfos(OTEXT("mydir"), OTEXT("file00.txt"));

    /* scalar types */
    int i   = 1;
    double dbl = 3.14;
    float flt = (float) 3.14;
    ostring str = OTEXT("Name00");

    /* bind scalar C types arrays */

    st.Bind(OTEXT(":val_int"),  i);
    st.Bind(OTEXT(":val_dbl"), dbl);
    st.Bind(OTEXT(":val_flt"), flt);
    st.Bind(OTEXT(":val_str"), str, (unsigned int) str.size());

    /* bind oracle types arrays */

    st.Bind(OTEXT(":val_date"), date);
    st.Bind(OTEXT(":val_lob"),  clob);
    st.Bind(OTEXT(":val_file"), file);

    /* do insert */

    st.Execute();
    con.Commit();

    std::ocout << std::endl << st.GetAffectedRows() << OTEXT(" row(s) inserted") << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_piecewise_insert
 * --------------------------------------------------------------------------------------------- */

void test_piecewise_insert(void)
{
    std::ocout << OTEXT("\n>>>>> TEST PIECEWISE INSERTING\n\n");

    std::ifstream file(OCI_SHARED_LIB, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        size_t size  = (size_t) file.tellg();
        file.seekg (0, std::ios::beg);
        std::ocout << std::endl << size << OTEXT(" bytes to write") << std::endl;

        Statement st(con);
        BLong lg(st);
        st.Prepare(OTEXT("insert into test_long_raw(code, content) values (1, :data)"));
        st.SetLongMaxSize( (unsigned int) size);
        st.Bind(OTEXT(":data"), lg, (int) size);
        st.Execute();

        char * buffer = new char [size];

        file.read(buffer, size);
        lg.Write(buffer, (unsigned int) size);

        delete buffer;

        std::ocout << std::endl << lg.GetSize() << OTEXT(" bytes written") << std::endl;

        file.close();
        con.Commit();
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_piecewise_fetch
 * --------------------------------------------------------------------------------------------- */

void test_piecewise_fetch(void)
{
    std::ocout << OTEXT("\n>>>>> TEST PIECEWISE FETCHING\n\n");

    Statement st(con);
    st.Execute(OTEXT("select content from test_long_raw where code = 1"));

    Resultset rs1 = st.GetResultset();
    while (rs1++)
    {
        BLong lg = rs1.Get<BLong>(1);

        char buffer[SIZE_BUF];
        int bytesRead;

        while ((bytesRead = lg.Read(buffer, SIZE_BUF))) {}

        std::ocout << std::endl << lg.GetSize() << OTEXT(" bytes read") << std::endl;
    }

    std::ocout << std::endl << rs1.GetCount() << OTEXT(" row(s) fetched") << std::endl;

    std::ocout << OTEXT("\n>>>>> TEST LONG MAPPED TO STRING\n\n");

    st.Execute(OTEXT("select content from test_long_str where code = 1"));
    st.SetLongMode(Statement::LongImplicit);

    Resultset rs2 = st.GetResultset();
    while (rs2++)
    {
        ostring str = rs2.Get<ostring>(1);

        std::ocout << str << std::endl;
        std::ocout << str.size() << OTEXT(" bytes read") << std::endl;
    }

    std::ocout << std::endl << rs2.GetCount() << OTEXT(" row(s) fetched") << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_lob
 * --------------------------------------------------------------------------------------------- */

void test_lob(void)
{
    std::ocout << OTEXT("\n>>>>> TEST LOB MANIPULATION\n\n");

    Statement st(con);
    st.Execute(OTEXT("select code, content from test_lob where code=1 for update"));

    Resultset rs= st.GetResultset();
    while (rs++)
    {
        Clob clob = rs.Get<Clob>(2);

        clob.Write(OTEXT("today, "));
        clob.Append(OTEXT("i'm going to the cinema ! "));
        clob.Seek(Clob::SeekSet, 0);

        std::ocout << OTEXT("> code : ") << rs.Get<int>(1) << OTEXT(", content : ") << clob.Read(SIZE_STR) << std::endl;
    }

    con.Commit();

    std::ocout << std::endl << rs.GetCount() << OTEXT(" row(s) fetched") << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_nested_table
 * --------------------------------------------------------------------------------------------- */

void test_nested_table(void)
{
    std::ocout << OTEXT("\n>>>>> TEST NESTED TABLE \n\n");

    Statement st(con);
    st.Execute(OTEXT("select article, cursor(select sysdate from dual) from test_fetch"));

    Resultset rs= st.GetResultset();
    while (rs++)
    {
        Statement st2 = rs.Get<Statement>(2);
        Resultset rs2 = st2.GetResultset();

        while (rs2++)
        {
            std::ocout << OTEXT("Article : ") << rs.Get<ostring>(1) << OTEXT(", Date : ") << rs2.Get<ostring>(1) << std::endl;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_ref_cursor
 * --------------------------------------------------------------------------------------------- */

void test_ref_cursor(void)
{
    std::ocout << OTEXT("\n>>>>> TEST REF CURSOR  \n\n");

    Statement stBind(con);

    Statement st(con);
    st.Prepare(OTEXT("begin open :c for select * from test_fetch; end;"));
    st.Bind( OTEXT(":c"), stBind, BindInfo::Out);
    st.Execute();

    Resultset rs = stBind.GetResultset();
    while (rs++)
    {
        std::ocout << OTEXT("> code   : ") << rs.Get<int>(1);
        std::ocout << OTEXT(", action : ") << rs.Get<ostring>(2);
        std::ocout << OTEXT(", price  : ") << rs.Get<double>(3);
        std::ocout << OTEXT(", date   : ");
        std::ocout << rs.Get<Date>(4).ToString() <<  std::endl;
    }

   std::ocout << std::endl << rs.GetCount() << OTEXT(" row(s) fetched") << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_plsql
 * --------------------------------------------------------------------------------------------- */

void test_plsql(void)
{
    std::ocout << OTEXT("\n>>>>> TEST PL/SQL OUTPUT BIND\n\n");

    int res = 0;
    Statement st(con);

    st.Prepare(OTEXT("begin :res := trunc(sysdate+1)-trunc(sysdate-1); end;"));
    st.Bind( OTEXT(":res"), res, BindInfo::Out);
    st.Execute();
    std::ocout << OTEXT("PL/SQL : trunc(sysdate+1)-trunc(sysdate-1)") << std::endl;
    std::ocout << OTEXT("Result : ") << res << std::endl;


#ifndef OCI_CHARSET_ANSI

   /* Oracle 8i has some troubles with SERVER OUTPUT in unicode */
   if (Environment::GetRuntimeVersion() < OCI_9_0)
        return;

#endif

    std::ocout << OTEXT("\n>>>>> TEST PL/SQL SERVER OUTPUT\n\n");

    /* server output */

    con.EnableServerOutput(32000, 5, 255);

    st.Execute( OTEXT("begin ")
                OTEXT("   dbms_output.put_line('First  line'); ")
                OTEXT("   dbms_output.put_line('Second line'); ")
                OTEXT("   dbms_output.put_line('Third  line'); ")
                OTEXT("end;")
                );

    ostring line;
    while (con.GetServerOutput(line) )
    {
        std::ocout << line << std::endl;
    }

    con.DisableServerOutput();
}

/* --------------------------------------------------------------------------------------------- *
 * test_dates
 * --------------------------------------------------------------------------------------------- */

void test_dates(void)
{
    std::ocout << OTEXT("\n>>>>> TEST DATETIME MANIPULATION\n\n");

    Date date;

    date.FromString(OTEXT("13041978 20:20:12"), OTEXT("DDMMYYYY HH24:MI:SS"));
    std::ocout << date.ToString(OTEXT("DD/MM/YYYY HH24:MI:SS")) << std::endl;

    date.SysDate();
    std::ocout << date.ToString(OTEXT("DD/MM/YYYY HH24:MI:SS")) << std::endl;

    date.AddDays(5);
    date.AddMonths(2);
    std::ocout << OTEXT("Date + 5 days and 2 months is ") << date.ToString(OTEXT("DD/MM/YYYY HH24:MI:SS")) << std::endl;

    std::ocout << OTEXT("Last day of the month : ") << date.LastDay().ToString(OTEXT("DD/MM/YYYY HH24:MI:SS")) << std::endl;

    std::ocout << OTEXT("Number of days until the end of the months : ") << date.LastDay().DaysBetween(date) << std::endl;
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

    if ((version >= OCI_9_0) || (version < OCI_10_1))
        return;
#endif

    if (version >= OCI_9_0)
    {
        std::ocout << OTEXT("\n>>>>> TEST TIMESTAMP\n\n");

        Timestamp tm(Timestamp::NoTimeZone);

        tm.SysTimestamp();
        std::ocout << OTEXT("Current timestamp : ") << tm.ToString(OTEXT("DD/MM/YYYY HH24:MI:SS:FF3")) << std::endl;

        /* intervals raw oci functions have some troubles with Oracle 9i. So let's
           use it for the demo only if we're using 10g or above */

        if (version >= OCI_10_1)
        {
            std::ocout << OTEXT("\n>>>>> TEST INTERVAL \n\n");

            Interval itv(Interval::DaySecond);
            itv.SetDaySecond(1,1,1,1,0);
            tm.AddInterval(itv);
            std::ocout << OTEXT("Current timestamp + Interval :") << tm.ToString(OTEXT("DD/MM/YYYY HH24:MI:SS:FF3")) << std::endl;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_describe
 * --------------------------------------------------------------------------------------------- */

void test_describe(void)
{
    std::ocout << OTEXT("\n>>>>> TEST DESCRIBING TABLE \n\n");
    std::ocout << OTEXT("Column Name         Type                ") << std::endl;
    std::ocout << OTEXT("----------------------------------------") << std::endl;
    std::ocout << std::setiosflags(std::ios::left);

    TypeInfo table(con, OTEXT("test_fetch"), TypeInfo::ObjectTable);

    for(int i = 1, n = table.GetColumnCount(); i <= n; i++)
    {
        Column col = table.GetColumn(i);

        std::ocout <<  std::setw(20) << col.GetName().c_str()
                    <<  std::setw(20) << col.GetFullSQLType().c_str()
                    <<  std::endl;
    }

    /* TEST DESCRIBING TYPE ------------------------------------------------- */

    std::ocout << OTEXT("\n>>>>> TEST DESCRIBING TYPE \n\n");
    std::ocout << OTEXT("Column Name         Type                ") << std::endl;
    std::ocout << OTEXT("----------------------------------------") << std::endl;
    std::ocout << std::setiosflags(std::ios::left);

    TypeInfo type(con, OTEXT("test_t"), TypeInfo::ObjectType);

    for(int i = 1, n = type.GetColumnCount(); i <= n; i++)
    {
        Column col = type.GetColumn(i);

        std::ocout <<  std::setw(20) << col.GetName().c_str()
                    <<  std::setw(20) << col.GetFullSQLType().c_str()
                    <<  std::endl;
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_returning
 * --------------------------------------------------------------------------------------------- */

void test_returning(void)
{
    std::ocout << OTEXT("\n>>>>> TEST RETURNING CLAUSE \n\n");

    Statement st(con);
    st.Prepare(OTEXT("update test_lob set code = code + 1 returning code, content into :i, :l"));
    st.Register<int>(OTEXT(":i"));
    st.Register<Clob>(OTEXT(":l"));
    st.Execute();

    Resultset rs = st.GetResultset();
    while (rs++)
    {
        Clob clob = rs.Get<Clob>(2);
        clob.Append(OTEXT("(modified)"));
        clob.Seek(Clob::SeekSet, 0);

        std::ocout << OTEXT("> code : ") << rs.Get<int>(1) << OTEXT(" - ") << clob.Read((int)clob.GetLength()) << std::endl;
    }

    con.Commit();

    std::ocout << std::endl << rs.GetCount() << OTEXT(" row(s) fetched") << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_returning_array
 * --------------------------------------------------------------------------------------------- */

void test_returning_array(void)
{
    std::ocout << OTEXT("\n>>>>> TEST ARRAY BINDING WITH RETURNING CLAUSE \n\n");

    std::vector<int>     tab_int;
    std::vector<double>  tab_dbl;
    std::vector<float>   tab_flt;
    std::vector<ostring> tab_str;
    std::vector<Date>    tab_date;
    std::vector<Clob>    tab_lob;
    std::vector<File>    tab_file;

    for(int i = 0; i < SIZE_TAB; i++)
    {
        tab_int.push_back(i+1);
        tab_dbl.push_back(3.14*(double)(i+1));
        tab_flt.push_back((float) 3.14*(float)(i+1));

        ostring str;
        str += OTEXT("Name");
        str += ( (i+1) +'0');
        tab_str.push_back(str);

        Date date;
        date.SysDate();
        tab_date.push_back(date);

        Clob clob(con);
        clob.Write(OTEXT("Lob value ") + str);
        tab_lob.push_back(clob);

        ostring fileName;
        fileName += OTEXT("File");
        fileName += ( (i+1) +'0');
        File file(con, OTEXT("Mydir"), fileName);
        tab_file.push_back(file);
    }

    Statement st(con);

    st.Prepare( OTEXT("insert into test_array ")
                OTEXT("( ")
                OTEXT("   val_int,  val_dbl, val_flt, val_str, val_date, ")
                OTEXT( "   val_lob, val_file ")
                OTEXT( ") " )
                OTEXT( "values ")
                OTEXT( "( ")
                OTEXT( "   :val_int, :val_dbl, :val_flt, :val_str, :val_date, ")
                OTEXT( "   :val_lob, :val_file ")
                OTEXT(") ")
                OTEXT("returning")
                OTEXT( "  val_int,  val_dbl, val_flt, val_str, val_date, ")
                OTEXT("   val_lob, val_file ")
                OTEXT("into  " )
                OTEXT( "  :out_int, :out_dbl, :out_flt,  :out_str, :out_date, ")
                OTEXT("   :out_lob, :out_file "));

    st.SetBindArraySize(SIZE_TAB);

    /* bind vectors */
    st.Bind(OTEXT(":val_int"),  tab_int);
    st.Bind(OTEXT(":val_dbl"),  tab_dbl);
    st.Bind(OTEXT(":val_flt"),  tab_flt);
    st.Bind(OTEXT(":val_date"), tab_date);
    st.Bind(OTEXT(":val_lob"),  tab_lob);
    st.Bind(OTEXT(":val_file"), tab_file);
    st.Bind(OTEXT(":val_str"),  tab_str, 30);

    /* register output */
    st.Register<int    >(OTEXT(":out_int"));
    st.Register<double >(OTEXT(":out_dbl"));
    st.Register<float  >(OTEXT(":out_flt"));
    st.Register<Date   >(OTEXT(":out_date"));
    st.Register<Clob   >(OTEXT(":out_lob"));
    st.Register<File   >(OTEXT(":out_file"));
    st.Register<ostring>(OTEXT(":out_str"), 30);

    st.Execute();
    std::ocout << std::endl << st.GetAffectedRows() << OTEXT(" row(s) inserted") << std::endl;

    int rowIndex = 0;

    Resultset rs = st.GetResultset();
    while (!rs.IsNull())
    {
        while (rs++)
        {
            std::ocout << OTEXT("Row #") << ++rowIndex << OTEXT("---------------") << std::endl;

            std::ocout << OTEXT(".... val_int    : ") << rs.Get<int>(OTEXT(":OUT_INT"))     << std::endl;
            std::ocout << OTEXT(".... val_dbl    : ") << rs.Get<double>(OTEXT(":OUT_DBL"))  << std::endl;
            std::ocout << OTEXT(".... val_flt    : ") << rs.Get<float>(OTEXT(":OUT_FLT"))   << std::endl;
            std::ocout << OTEXT(".... val_str    : ") << rs.Get<ostring>(OTEXT(":OUT_STR")) << std::endl;

            Date date = rs.Get<Date>(OTEXT(":OUT_DATE"));
            std::ocout << OTEXT(".... val_date   : ") << date.ToString(OTEXT("YYYY-MM-DD HH24:MI:SS")) << std::endl;

            Clob clob = rs.Get<Clob>(OTEXT(":OUT_LOB"));
            std::ocout << OTEXT(".... val_lob    : ") << clob.Read(SIZE_BUF) << std::endl;

            File file = rs.Get<File>(OTEXT(":OUT_FILE"));
            std::ocout << OTEXT(".... val_file   : ") << file.GetDirectory() << OTEXT("/") << file.GetName() << std::endl;
        }

        rs = st.GetNextResultset();
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_object_insert
 * --------------------------------------------------------------------------------------------- */

void test_object_insert(void)
{
    std::ocout << OTEXT("\n>>>>> TEST OBJECT BINDING \n\n");

    Clob clob(con);
    clob.Write(OTEXT("Lob Value"));
    File file(con,  OTEXT("mydir"), OTEXT("myfile"));
    Date date;
    date.SysDate();

    Object obj2(TypeInfo(con, OTEXT("type_t"), TypeInfo::ObjectType));

    obj2.Set<int>    (OTEXT("ID"), 1);
    obj2.Set<ostring>(OTEXT("NAME"), OTEXT("USB KEY 2go"));

    Object obj1(TypeInfo(con, OTEXT("test_t"), TypeInfo::ObjectType));

    obj1.Set<int>    (OTEXT("VAL_INT"), 1);
    obj1.Set<double> (OTEXT("VAL_DBL"), 3.14);
    obj1.Set<float>  (OTEXT("VAL_FLT"), (float) 3.14);
    obj1.Set<ostring>(OTEXT("VAL_STR"), OTEXT("USB KEY 2go"));
    obj1.Set<void *> (OTEXT("VAL_RAW"), (void*) "0123456789", 10);
    obj1.Set<Date>   (OTEXT("VAL_DATE"), date);
    obj1.Set<Object> (OTEXT("VAL_OBJ"), obj2);
    obj1.Set<Clob>   (OTEXT("VAL_LOB"), clob);
    obj1.Set<File>   (OTEXT("VAL_FILE"), file);

    Statement st(con);
    st.Prepare(OTEXT("insert into test_object values(:obj)"));
    st.Bind(OTEXT(":obj"), obj1);
    st.Execute();

    std::ocout << OTEXT("Rows inserted :  ") << st.GetAffectedRows() << std::endl;

    con.Commit();
}

/* --------------------------------------------------------------------------------------------- *
 * test_object_fetch
 * --------------------------------------------------------------------------------------------- */

void test_object_fetch(void)
{
    std::ocout << OTEXT("\n>>>>> TEST OBJECT FETCHING \n\n");

    Statement st(con);
    st.Execute(OTEXT("select val from test_object for update"));

    Resultset rs = st.GetResultset();
    while (rs++)
    {
        Object obj  = rs.Get<Object>(1);

        std::ocout << OTEXT(".... val_int      : ") << obj.Get<int>(OTEXT("VAL_INT"))     << std::endl;
        std::ocout << OTEXT(".... val_dbl      : ") << obj.Get<double>(OTEXT("VAL_DBL"))  << std::endl;
        std::ocout << OTEXT(".... val_flt      : ") << obj.Get<float>(OTEXT("VAL_FLT"))   << std::endl;
        std::ocout << OTEXT(".... val_str      : ") << obj.Get<ostring>(OTEXT("VAL_STR")) << std::endl;

        Date date = obj.Get<Date>(OTEXT("VAL_DATE"));
        std::ocout << OTEXT(".... val_date     : ") << date.ToString(OTEXT("YYYY-MM-DD HH24:MI:SS")) << std::endl;

        Clob clob = obj.Get<Clob>(OTEXT("VAL_LOB"));
        std::ocout << OTEXT(".... val_lob      : ") << clob.Read(SIZE_BUF) << std::endl;

        File file = obj.Get<File>(OTEXT("VAL_FILE"));
        std::ocout << OTEXT(".... val_file     : ") << file.GetDirectory() << OTEXT("/") << file.GetName() << std::endl;

        char buffer[11];
        unsigned int len = 10;
        obj.Get<void *>(OTEXT("VAL_RAW"), (BufferPointer) &buffer, len);
        buffer[len] = 0;
        std::cout << ".... val_raw      : " << buffer << std::endl;

        Object obj2 = obj.Get<Object>(OTEXT("VAL_OBJ"));
        std::ocout << OTEXT(".... val_obj.code : ") << obj2.Get<int>(OTEXT("ID"))       << std::endl;
        std::ocout << OTEXT(".... val_obj.name : ") << obj2.Get<ostring>(OTEXT("NAME")) << std::endl;
    }

    con.Commit();

    std::ocout << OTEXT("\n>>>>> TEST OBJECT FETCHING  AS STRING \n\n");

    st.Execute(OTEXT("select val from test_object"));

    rs = st.GetResultset();
    while (rs++)
    {
        std::ocout <<  rs.Get<Object>(1).ToString() << std::endl;
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_scrollable_cursor
 * --------------------------------------------------------------------------------------------- */

void test_scrollable_cursor(void)
{
    if (Environment::GetRuntimeVersion() > OCI_9_0)
    {   
        std::ocout << OTEXT("\n>>>>> TEST SCROLLABLE CURSORS \n\n");

        Statement st(con);
        
        st.SetFetchMode(Statement::FetchScrollable);
        st.Execute(OTEXT("select table_name from user_tables where ")
                   OTEXT("table_name like 'TEST_%' order by table_name"));
   
        Resultset rs = st.GetResultset();
        
        rs.Last();
        std::ocout << OTEXT("Total rows : ") << rs.GetCount() << std::endl;
        
        std::ocout << OTEXT("... Go to row 1\n");
        rs.First();
        std::ocout << OTEXT("table ") << rs.Get<ostring>(1) << std::endl;

        std::ocout << OTEXT("... Enumerate from row 2 to row ") << rs.GetCount() << OTEXT(" ") << std::endl;
        while (rs++)
        {
            std::ocout << OTEXT("table ") << rs.Get<ostring>(1) << std::endl;
        }

        std::ocout << OTEXT("... Enumerate from row ") << rs.GetCount() -1  << OTEXT(" back to row 1") << std::endl;        
        while (rs.Prev())
        {
            std::ocout << OTEXT("table ") << rs.Get<ostring>(1) << std::endl;
        }

        std::ocout << OTEXT("... Go to the 3th row") << std::endl;
        rs.Seek(Resultset::SeekAbsolute, 3);
        std::ocout << OTEXT("table ") << rs.Get<ostring>(1) << std::endl;

        std::ocout << OTEXT("... Fetch the next 2 rows") << std::endl;
        while (rs.GetCurrentRow() < 5 && rs++)
        {
            std::ocout << OTEXT("table ") << rs.Get<ostring>(1) << std::endl;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_collection
 * --------------------------------------------------------------------------------------------- */
void test_collection(void)
{
    std::ocout << OTEXT("\n>>>>> TEST VARRAY BINDING WITH ITERATOR \n\n");

    int i;

    Statement st(con);
    TypeInfo type(con, OTEXT("T_TAB1_EMP"), TypeInfo::ObjectType);
    Collection coll(type);

    st.Prepare( OTEXT("begin")
                OTEXT("    select employees into :tab_emp ")
                OTEXT("    from test_coll_varray ")
                OTEXT("    where departement = :id; ")
                OTEXT("end;"));

    st.Bind(OTEXT(":tab_emp"), coll);
    st.Bind(OTEXT(":id"), i);

    i = 1;

    st.Execute();

    std::ocout << OTEXT("Department ID #") << i << std::endl;

    CollectionIterator iter(coll);
    while (iter++)
    {
        std::ocout << OTEXT("... Employee : ") << iter.Get<ostring>() << std::endl;
    }

    std::ocout << OTEXT("\n>>>>> TEST VARRAY PRINTING \n\n");
     
    std::ocout << coll.ToString() << std::endl;

    std::ocout << OTEXT("\n>>>>> TEST VARRAY FETCHING WITH ITERATOR \n\n");

    st.Execute(OTEXT("SELECT * from test_coll_varray"));
 
    Resultset rs = st.GetResultset();
    while (rs++)
    {
        std::ocout << OTEXT("Department ID #") << rs.Get<int>(1) << std::endl;

        coll = rs.Get<Collection>(2);
        CollectionIterator iter2(coll);
        while (iter2++)
        {
            std::ocout << OTEXT("... Employee : ") << iter2.Get<ostring>() << std::endl;
        }
    }

    std::ocout << OTEXT("\n>>>>> TEST NESTED TABLE FETCHING WITH INDEX ACCESS \n\n");

    st.Execute(OTEXT("SELECT * from test_coll_nested"));

    rs = st.GetResultset();
    while (rs++)
    {
        std::ocout << OTEXT("Department ID #") << rs.Get<int>(1) << std::endl;

        coll = rs.Get<Collection>(2);
        for(int index = 1, n = coll.GetSize(); index <= n; index++)
        {
            std::ocout << OTEXT("... Employee : ") << coll.Get<ostring>(index) << std::endl;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_ref
 * --------------------------------------------------------------------------------------------- */
void test_ref(void)
{
    std::ocout << OTEXT("\n>>>>> TEST REF FETCHING \n\n");

    Statement st(con);

    st.Execute(OTEXT("select ref(e) from test_table_obj e"));

    Resultset rs = st.GetResultset();
    while (rs++)
    {
        Reference ref = rs.Get<Reference>(1);
        Object   obj  = ref.GetObject();
        
        std::ocout << obj.Get<int>(OTEXT("ID")) << OTEXT(" - ") << obj.Get<ostring>(OTEXT("NAME")) << std::endl;
    }

    std::ocout << OTEXT("\n>>>>> TEST REF PL/SQL BINDING \n\n");

    Reference ref(TypeInfo(con, OTEXT("type_t"), TypeInfo::ObjectType));

    st.Prepare (OTEXT("begin ")
                OTEXT("  select ref(e) into :r from test_table_obj e where e.id = 1; ")
                OTEXT("end; "));

    st.Bind(OTEXT(":r"), ref, BindInfo::InOut);
    st.Execute();

    Object obj = ref.GetObject();
    std::ocout << obj.Get<int>(OTEXT("ID")) << OTEXT(" - ") << obj.Get<ostring>(OTEXT("NAME")) << std::endl;
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
    if(OCI_VER_MAJ(Environment::GetCompileVersion()) == con.GetServerMajorVersion())
    {
       /* commit any previous pending modifications */

        con.Commit();

        std::ocout << OTEXT("\n>>>>> TEST DIRECT PATH (10 loads of 100 rows) \n\n");
    
        int i = 0, j = 0, nb_rows = SIZE_ARRAY;

        DirectPath directPath(TypeInfo(con, OTEXT("test_directpath"), TypeInfo::ObjectTable), NUM_COLS, nb_rows);

        /* optional attributes to set */

        directPath.SetBufferSize(64000);
        directPath.SetNoLog(true);
        directPath.SetParallel(true);

        if(Environment::GetCompileVersion() >= OCI_9_2)
        {
           directPath.SetCacheSize(100);
        }

        /* describe the target table */

        directPath.SetColumn(1, OTEXT("VAL_INT"),  SIZE_COL1);
        directPath.SetColumn(2, OTEXT("VAL_STR"),  SIZE_COL2);
        directPath.SetColumn(3, OTEXT("VAL_DATE"), SIZE_COL3, OTEXT("YYYYMMDD"));

        /* prepare the load */

        directPath.Prepare();

        nb_rows = directPath.GetMaxRows();          
            
        for (i = 0; i < NB_LOAD ; i++)
        {
            directPath.Reset();

            for (j = 1; j <= nb_rows; j++)
            {             
                std::oostringstream val1, val2, val3;

                /* fill test values */

                val1 << std::setfill(OTEXT('0')) << std::setw(4) << j + (i*100);
                val2 << OTEXT("value ") << std::setfill(OTEXT('0')) << std::setw(5) << j + (i*100);
                val3 << std::setfill(OTEXT('0')) << std::setw(2) << (j%23)+1 + 2000 <<  std::setw(2) << (j%11)+1 << (j%23)+1;

                directPath.SetEntry(j, 1, val1.str());
                directPath.SetEntry(j, 2, val2.str());
                directPath.SetEntry(j, 3, val3.str());
            }

            /* load data to the server */
           directPath.Convert();
           directPath.Load();
        }

        /* commits changes */

        directPath.Finish();

       std::ocout << std::setw(4) << directPath.GetRowCount() << OTEXT(" row(s) loaded") << std::endl;
   }
}

