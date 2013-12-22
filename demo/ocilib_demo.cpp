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
  #elif defined(OCI_CHARSET_MIXED)
      #pragma comment(lib, "ocilibm.lib")
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
    #define mtmain          wmain
    #define mtarg           mtext
  #else
    #define mtmain          main
    #define mtarg           char
  #endif
#else
  #define mtmain          main
  #define mtarg           char
#endif

#if defined(OCI_CHARSET_WIDE)
  #define mtcout            wcout        
  #define dtcout            wcout   
  #define dtostringstream   wostringstream 

#elif defined(OCI_CHARSET_MIXED)
  #define mtcout            cout        
  #define dtcout            wcout 
  #define dtostringstream   wostringstream 
#else
  #define mtcout            cout        
  #define dtcout            cout 
  #define dtostringstream   ostringstream 
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
void execute_ddl(mstring sql);

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

int mtmain(int argc, mtext* argv[])
{
    mstring home;
    mstring dbs;
    mstring usr;
    mstring pwd;

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
        Environment::Initialize(OCI_ENV_DEFAULT, home);
        Environment::EnableWarnings(true);

        std::mtcout << MT("Connecting to ") << usr << MT("/") << pwd << MT("@") << dbs << std::endl << std::endl;

        con.Open(dbs, usr, pwd, OCI_SESSION_DEFAULT);

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
         std::mtcout << ex.GetMessage() << std::endl;
    }

    Environment::Cleanup();

    std::mtcout << MT("\nPress any key to exit...");

    getchar();

    return EXIT_SUCCESS;
}

/* --------------------------------------------------------------------------------------------- *
 * print_version
 * --------------------------------------------------------------------------------------------- */

void print_version(void)
{
    std::mtcout << MT("\n>>>>> OCILIB BUILD INFORMATION \n\n");

    if (Environment::GetImportMode() == OCI_IMPORT_MODE_LINKAGE)
        std::mtcout << MT("OCI import mode         : LINKAGE\n");
    else
        std::mtcout << MT("OCI import mode         : RUNTIME\n");

    if (Environment::GetCharsetMetaData() == OCI_CHAR_ANSI)
        std::mtcout << MT("MetaData  char type     : ANSI\n");
    else
        std::mtcout << MT("MetaData  char type     : WIDE\n");

    if (Environment::GetCharsetUserData() == OCI_CHAR_ANSI)
        std::mtcout << MT("UserData  char type     : ANSI\n");
    else
        std::mtcout << MT("UserData  char type     : WIDE\n");

    std::mtcout << MT("\n>>>>> VERSIONS INFORMATION \n\n");
    std::mtcout << MT("OCILIB major    version : ") << OCILIB_MAJOR_VERSION     << std::endl;
    std::mtcout << MT("OCILIB minor    version : ") << OCILIB_MINOR_VERSION     << std::endl;
    std::mtcout << MT("OCILIB revision version : ") << OCILIB_REVISION_VERSION  << std::endl;
    std::mtcout << MT("OCI compile     version : ") << OCI_VER_MAJ(Environment::GetCompileVersion()) << std::endl;
    std::mtcout << MT("OCI runtime     version : ") << OCI_VER_MAJ(Environment::GetRuntimeVersion()) << std::endl;
    std::mtcout << MT("Server major    version : ") << con.GetServerMajorVersion()    << std::endl;
    std::mtcout << MT("Server minor    version : ") << con.GetServerMinorVersion()    << std::endl;
    std::mtcout << MT("Server revision version : ") << con.GetServerRevisionVersion() << std::endl;
    std::mtcout << MT("Connection      version : ") << con.GetVersion() << std::endl;

    std::mtcout << MT("\n>>>>> SERVER VERSION BANNER \n\n");
    std::mtcout << con.GetServerVersion() << std::endl << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * execute_ddl
 * --------------------------------------------------------------------------------------------- */

void execute_ddl(mstring sql)
{
    try
    {
        Statement st(con);
        st.Execute(sql);
    }
    catch(Exception &ex)
    {
         std::mtcout << ex.GetMessage() << std::endl;
    }
}

/* --------------------------------------------------------------------------------------------- *
 * create_tables
 * --------------------------------------------------------------------------------------------- */

void create_tables(void)
{
    std::mtcout << MT("\n>>>>> CREATE TABLES FOR DEMO \n\n");

    /* create types for the demo */
    execute_ddl( MT("create type type_t as OBJECT (id int, name varchar2(50))"));

    execute_ddl( MT("create type test_t as object ")
                MT("( ")
                MT("    val_int  number, ")
                MT("    val_flt  float, ")
                MT("    val_dbl  float, ")
                MT("    val_str  varchar2(30), ")
                MT("    val_date date, ")
                MT("    val_lob  clob, ")
                MT("    val_file bfile, ")
                MT("    val_obj  type_t, ")
                MT("    val_raw  raw(10) ")
                MT(")"));

    execute_ddl(MT("create type t_tab1_emp as VARRAY(100) of varchar2(50)"));

    execute_ddl(MT("create type t_tab2_emp as table of varchar2(50)"));

    /* create table for the demo */
    execute_ddl( MT("create table test_fetch(code int, article ")
                MT("varchar2(30), price float, creation date)"));

    execute_ddl(MT("create table test_long_raw(code int, content long raw)"));

    execute_ddl(MT("create table test_long_str(code int, content long)"));

    execute_ddl(MT("create table test_lob(code int, content clob)"));

    execute_ddl(MT("create table test_object(val test_t)"));

    execute_ddl(MT("create table test_table_obj of type_t"));

    execute_ddl( MT("create table test_array ")
                MT("( ")
                MT("    val_int  number, ")
                MT("    val_dbl  float, ")
                MT("    val_flt  float, ")
                MT("    val_str  varchar2(30), ")
                MT("    val_date date, ")
                MT("    val_lob  clob, ")
                MT("    val_file bfile ")
                MT(")")
            );

    execute_ddl (MT("create table test_coll_varray ")
                MT("( ")
                MT("    departement number, ")
                MT("    employees   t_tab1_emp ")
                MT(")")
            );

    execute_ddl( MT("create table test_coll_nested ")
                MT("( ")
                MT("    departement number, ")
                MT("    employees   t_tab2_emp ")
                MT(") nested table employees store as test_table_emp")
            );

    execute_ddl(MT("create table test_directpath(val_int number(8,4), ")
                        MT(" val_str varchar2(30), val_date date)"));

    /* insert data into the demo tables */
    execute_ddl( MT("insert into test_fetch ")
                MT("(code, article, price, creation) ")
                MT("values (1, 'shoes', 3.14, to_date('1978-12-23', 'YYYY-MM-DD'))"));

    execute_ddl( MT("insert into test_fetch ")
                MT("(code, article, price, creation) ")
                MT("values (2, 'shirt', 5.99, to_date('1999-09-12', 'YYYY-MM-DD'))"));

    execute_ddl(MT("insert into test_lob(code,content)  ")
                        MT("values (1, EMPTY_CLOB())"));

    execute_ddl( MT("insert into test_long_str(code,content) ")
                MT("values (1, 'Rugby rocks !')"));

    execute_ddl( MT("insert into test_coll_varray(departement,employees) ")
                MT("values (1, t_tab1_emp('Peter', 'John', 'Paula', 'Gina'))"));

    execute_ddl(MT("insert into test_coll_varray(departement,employees) ")
                        MT("values (2, t_tab1_emp('Ben', 'Alice', 'Joel', 'Maria'))"));

    execute_ddl( MT("insert into test_coll_nested(departement,employees) ")
                MT("values (1, t_tab2_emp('Vince', 'Richard', 'Rita', 'Sophia'))"));

    execute_ddl( MT("insert into test_coll_nested(departement,employees) ")
                MT("values (2, t_tab2_emp('Paul', 'Sarah', 'Robert', 'Zoe'))"));

    execute_ddl(MT("insert into test_table_obj values(type_t(1, 'shoes'))"));
    execute_ddl(MT("insert into test_table_obj values(type_t(2, 'pen'))"));

    con.Commit();
}

/* --------------------------------------------------------------------------------------------- *
 * drop_tables
 * --------------------------------------------------------------------------------------------- */

void drop_tables(void)
{
    std::mtcout << MT("\n>>>>> DROPPING TABLES AND TYPES \n\n");

    execute_ddl(MT("drop table test_fetch"));
    execute_ddl(MT("drop table test_long_str"));
    execute_ddl(MT("drop table test_long_raw"));
    execute_ddl(MT("drop table test_lob"));
    execute_ddl(MT("drop table test_array"));
    execute_ddl(MT("drop table test_object"));
    execute_ddl(MT("drop table test_coll_varray"));
    execute_ddl(MT("drop table test_coll_nested"));
    execute_ddl(MT("drop table test_table_obj"));
    execute_ddl(MT("drop table test_directpath"));
    execute_ddl(MT("drop type  test_t"));
    execute_ddl(MT("drop type  type_t"));
    execute_ddl(MT("drop type  t_tab1_emp"));
    execute_ddl(MT("drop type  t_tab2_emp"));
}

/* --------------------------------------------------------------------------------------------- *
 * test_fetch
 * --------------------------------------------------------------------------------------------- */

void test_fetch(void)
{
    std::mtcout << MT("\n>>>>> SIMPLE TEST FETCH WITH META DATA\n\n");

    Statement st(con);
    st.Execute(MT("select * from test_fetch"));

    Resultset rs = st.GetResultset();
    for(int i = 1, n = rs.GetColumnCount(); i <= n; i++)
    {
        Column col = rs.GetColumn(i);
        std::mtcout << MT("> Field : #") << i << MT(" - Name : ") << col.GetName() << std::endl;
    }

    std::mtcout << std::endl;

    while (rs++)
    {
        std::dtcout << DT("> code   : ") << rs.Get<int>(1);
        std::dtcout << DT(", action : ") << rs.Get<dstring>(2);
        std::dtcout << DT(", price  : ") << rs.Get<double>(3);
        std::dtcout << DT(", date   : ");
        std::mtcout << rs.Get<Date>(4).ToString() <<  std::endl;
    }

    std::mtcout << std::endl << rs.GetCount() << MT(" row(s) fetched") << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_bind1
 * --------------------------------------------------------------------------------------------- */

void test_bind1(void)
{
    int code = 1;

    std::mtcout << MT("\n>>>>> TEST BINDING \n\n");

    Statement st(con);
    st.Prepare(MT("select * from test_fetch where code = :code"));
    st.Bind(MT(":code"), code, OCI_BDM_IN);
    st.Execute();

    Resultset rs = st.GetResultset();
    while (rs++)
    {
        std::dtcout << DT("> code   : ") << rs.Get<int>(1);
        std::dtcout << DT(", action : ") << rs.Get<dstring>(2);
        std::dtcout << DT(", price  : ") << rs.Get<double>(3);
        std::dtcout << DT(", date   : ");
        std::mtcout << rs.Get<Date>(4).ToString() <<  std::endl;
    }

    std::mtcout << std::endl << rs.GetCount() << MT(" row(s) fetched") << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_bind2
 * --------------------------------------------------------------------------------------------- */

void test_bind2(void)
{
    std::mtcout << MT("\n>>>>> SINGLE BINDING \n\n");

    Statement st(con);
    st.Prepare( MT("insert into test_array ")
                MT("( ")
                MT("   val_int,  val_dbl, val_flt, val_str, val_date, ")
                MT( "   val_lob, val_file ")
                MT( ") " )
                MT( "values ")
                MT( "( ")
                MT( "   :val_int, :val_dbl, :val_flt, :val_str, :val_date, ")
                MT( "   :val_lob, :val_file ")
                MT(") "));

    /* create objects and set values */

    /* date */
    Date date;
    date.SysDate();

    date++;

    /* lob */
    Clob clob(con);
    clob.Write(DT("lob value00"));

    /* file */
    File file(con);
    file.SetInfos(MT("mydir"), MT("file00.txt"));

    /* scalar types */
    int i   = 1;
    double dbl = 3.14;
    float flt = (float) 3.14;
    dstring str = DT("Name00");

    /* bind scalar C types arrays */

    st.Bind(MT(":val_int"),  i,  OCI_BDM_IN);
    st.Bind(MT(":val_dbl"), dbl, OCI_BDM_IN);
    st.Bind(MT(":val_flt"), flt, OCI_BDM_IN);
    st.Bind(MT(":val_str"), str, (unsigned int) str.size(), OCI_BDM_IN);

    /* bind oracle types arrays */

    st.Bind(MT(":val_date"), date, OCI_BDM_IN);
    st.Bind(MT(":val_lob"),  clob, OCI_BDM_IN);
    st.Bind(MT(":val_file"), file, OCI_BDM_IN);

    /* do insert */

    st.Execute();
    con.Commit();

    std::mtcout << std::endl << st.GetAffectedRows() << MT(" row(s) inserted") << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_piecewise_insert
 * --------------------------------------------------------------------------------------------- */

void test_piecewise_insert(void)
{
    std::mtcout << MT("\n>>>>> TEST PIECEWISE INSERTING\n\n");

    std::ifstream file(OCI_SHARED_LIB, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        size_t size  = (size_t) file.tellg();
        file.seekg (0, std::ios::beg);
        std::mtcout << std::endl << size << MT(" bytes to write") << std::endl;

        Statement st(con);
        BLong lg(st);
        st.Prepare(MT("insert into test_long_raw(code, content) values (1, :data)"));
        st.SetLongMaxSize( (unsigned int) size);
        st.Bind(MT(":data"), lg, (int) size, OCI_BDM_IN);
        st.Execute();

        char * buffer = new char [size];

        file.read(buffer, size);
        lg.Write(buffer, (unsigned int) size);

        delete buffer;

        std::mtcout << std::endl << lg.GetSize() << MT(" bytes written") << std::endl;

        file.close();
        con.Commit();
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_piecewise_fetch
 * --------------------------------------------------------------------------------------------- */

void test_piecewise_fetch(void)
{
    std::mtcout << MT("\n>>>>> TEST PIECEWISE FETCHING\n\n");

    Statement st(con);
    st.Execute(MT("select content from test_long_raw where code = 1"));

    Resultset rs1 = st.GetResultset();
    while (rs1++)
    {
        BLong lg = rs1.Get<BLong>(1);

        char buffer[SIZE_BUF];
        int bytesRead;

        while ((bytesRead = lg.Read(buffer, SIZE_BUF))) {}

        std::mtcout << std::endl << lg.GetSize() << MT(" bytes read") << std::endl;
    }

    std::mtcout << std::endl << rs1.GetCount() << MT(" row(s) fetched") << std::endl;

    std::mtcout << MT("\n>>>>> TEST LONG MAPPED TO STRING\n\n");

    st.Execute(MT("select content from test_long_str where code = 1"));
    st.SetLongMode(OCI_LONG_IMPLICIT);

    Resultset rs2 = st.GetResultset();
    while (rs2++)
    {
        dstring str = rs2.Get<dstring>(1);

        std::dtcout << str << std::endl;
        std::mtcout << str.size() << MT(" bytes read") << std::endl;
    }

    std::mtcout << std::endl << rs2.GetCount() << MT(" row(s) fetched") << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_lob
 * --------------------------------------------------------------------------------------------- */

void test_lob(void)
{
    std::mtcout << MT("\n>>>>> TEST LOB MANIPULATION\n\n");

    Statement st(con);
    st.Execute(MT("select code, content from test_lob where code=1 for update"));

    Resultset rs= st.GetResultset();
    while (rs++)
    {
        Clob clob = rs.Get<Clob>(2);

        clob.Write(DT("today, "));
        clob.Append(DT("i'm going to the cinema ! "));
        clob.Seek( OCI_SEEK_SET, 0);

        std::dtcout << DT("> code : ") << rs.Get<int>(1) << DT(", content : ") << clob.Read(SIZE_STR) << std::endl;
    }

    con.Commit();

    std::mtcout << std::endl << rs.GetCount() << MT(" row(s) fetched") << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_nested_table
 * --------------------------------------------------------------------------------------------- */

void test_nested_table(void)
{
    std::mtcout << MT("\n>>>>> TEST NESTED TABLE \n\n");

    Statement st(con);
    st.Execute(MT("select article, cursor(select sysdate from dual) from test_fetch"));

    Resultset rs= st.GetResultset();
    while (rs++)
    {
        Statement st2 = rs.Get<Statement>(2);
        Resultset rs2 = st2.GetResultset();

        while (rs2++)
        {
            std::dtcout << DT("Article : ") << rs.Get<dstring>(1) << DT(", Date : ") << rs2.Get<dstring>(1) << std::endl;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_ref_cursor
 * --------------------------------------------------------------------------------------------- */

void test_ref_cursor(void)
{
    std::mtcout << MT("\n>>>>> TEST REF CURSOR  \n\n");

    Statement stBind(con);

    Statement st(con);
    st.Prepare(MT("begin open :c for select * from test_fetch; end;"));
    st.Bind( MT(":c"), stBind, OCI_BDM_OUT);
    st.Execute();

    Resultset rs = stBind.GetResultset();
    while (rs++)
    {
        std::dtcout << DT("> code   : ") << rs.Get<int>(1);
        std::dtcout << DT(", action : ") << rs.Get<dstring>(2);
        std::dtcout << DT(", price  : ") << rs.Get<double>(3);
        std::dtcout << DT(", date   : ");
        std::mtcout << rs.Get<Date>(4).ToString() <<  std::endl;
    }

   std::mtcout << std::endl << rs.GetCount() << MT(" row(s) fetched") << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_plsql
 * --------------------------------------------------------------------------------------------- */

void test_plsql(void)
{
    std::mtcout << MT("\n>>>>> TEST PL/SQL OUTPUT BIND\n\n");

    int res = 0;
    Statement st(con);

    st.Prepare(MT("begin :res := trunc(sysdate+1)-trunc(sysdate-1); end;"));
    st.Bind( MT(":res"), res, OCI_BDM_OUT);
    st.Execute();
    std::mtcout << MT("PL/SQL : trunc(sysdate+1)-trunc(sysdate-1)") << std::endl;
    std::mtcout << MT("Result : ") << res << std::endl;


#ifndef OCI_CHARSET_ANSI

   /* Oracle 8i has some troubles with SERVER OUTPUT in unicode */
   if (Environment::GetRuntimeVersion() < OCI_9_0)
        return;

#endif

    std::mtcout << MT("\n>>>>> TEST PL/SQL SERVER OUTPUT\n\n");

    /* server output */

    con.EnableServerOutput(32000, 5, 255);

    st.Execute( MT("begin ")
                MT("   dbms_output.put_line('First  line'); ")
                MT("   dbms_output.put_line('Second line'); ")
                MT("   dbms_output.put_line('Third  line'); ")
                MT("end;")
                );

    dstring line;
    while (con.GetServerOutput(line) )
    {
        std::dtcout << line << std::endl;
    }

    con.DisableServerOutput();
}

/* --------------------------------------------------------------------------------------------- *
 * test_dates
 * --------------------------------------------------------------------------------------------- */

void test_dates(void)
{
    std::mtcout << MT("\n>>>>> TEST DATETIME MANIPULATION\n\n");

    Date d1, d2;

    d1.FromString(MT("13041978 20:20:12"), MT("DDMMYYYY HH24:MI:SS"));
    std::mtcout << d1.ToString(MT("DD/MM/YYYY HH24:MI:SS")) << std::endl;

    d1.SysDate();
    std::mtcout << d1.ToString(MT("DD/MM/YYYY HH24:MI:SS")) << std::endl;

    d1.AddDays(5);
    d1.AddMonths(2);
    std::mtcout << MT("Date + 5 days and 2 months is ") << d1.ToString(MT("DD/MM/YYYY HH24:MI:SS")) << std::endl;

    d2.Assign(d1);

    d1.LastDay();
    std::mtcout << MT("Last day of the month : ") << d1.ToString(MT("DD/MM/YYYY HH24:MI:SS")) << std::endl;

    std::mtcout << MT("Number of days until the end of the months : ") << d1.DaysBetween(d2) << std::endl;
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
        std::mtcout << MT("\n>>>>> TEST TIMESTAMP\n\n");

        Timestamp tm(OCI_TIMESTAMP);

        tm.SysTimestamp();
        std::mtcout << MT("Current timestamp : ") << tm.ToString(MT("DD/MM/YYYY HH24:MI:SS:FF3")) << std::endl;

        /* intervals raw oci functions have some troubles with Oracle 9i. So let's
           use it for the demo only if we're using 10g or above */

        if (version >= OCI_10_1)
        {
            std::mtcout << MT("\n>>>>> TEST INTERVAL \n\n");

            Interval itv(OCI_INTERVAL_DS);
            itv.SetDaySecond(1,1,1,1,0);
            tm.AddInterval(itv);
            std::mtcout << MT("Current timestamp + Interval :") << tm.ToString(MT("DD/MM/YYYY HH24:MI:SS:FF3")) << std::endl;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_describe
 * --------------------------------------------------------------------------------------------- */

void test_describe(void)
{
    std::mtcout << MT("\n>>>>> TEST DESCRIBING TABLE \n\n");
    std::mtcout << MT("Column Name         Type                ") << std::endl;
    std::mtcout << MT("----------------------------------------") << std::endl;
    std::mtcout << std::setiosflags(std::ios::left);

    TypeInfo table(con, MT("test_fetch"), OCI_TIF_TABLE);

    for(int i = 1, n = table.GetColumnCount(); i <= n; i++)
    {
        Column col = table.GetColumn(i);

        std::mtcout <<  std::setw(20) << col.GetName().c_str()
                    <<  std::setw(20) << col.GetFullSQLType().c_str()
                    <<  std::endl;
    }

    /* TEST DESCRIBING TYPE ------------------------------------------------- */

    std::mtcout << MT("\n>>>>> TEST DESCRIBING TYPE \n\n");
    std::mtcout << MT("Column Name         Type                ") << std::endl;
    std::mtcout << MT("----------------------------------------") << std::endl;
    std::mtcout << std::setiosflags(std::ios::left);

    TypeInfo type(con, MT("test_t"), OCI_TIF_TYPE);

    for(int i = 1, n = type.GetColumnCount(); i <= n; i++)
    {
        Column col = type.GetColumn(i);

        std::mtcout <<  std::setw(20) << col.GetName().c_str()
                    <<  std::setw(20) << col.GetFullSQLType().c_str()
                    <<  std::endl;
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_returning
 * --------------------------------------------------------------------------------------------- */

void test_returning(void)
{
    std::mtcout << MT("\n>>>>> TEST RETURNING CLAUSE \n\n");

    Statement st(con);
    st.Prepare(MT("update test_lob set code = code + 1 returning code, content into :i, :l"));
    st.Register<int>(MT(":i"));
    st.Register<Clob>(MT(":l"));
    st.Execute();

    Resultset rs = st.GetResultset();
    while (rs++)
    {
        Clob clob = rs.Get<Clob>(2);
        clob.Append(DT("(modified)"));
        clob.Seek(OCI_SEEK_SET, 0);

        std::dtcout << DT("> code : ") << rs.Get<int>(1) << DT(" - ") << clob.Read((int)clob.GetLength()) << std::endl;
    }

    con.Commit();

    std::mtcout << std::endl << rs.GetCount() << MT(" row(s) fetched") << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_returning_array
 * --------------------------------------------------------------------------------------------- */

void test_returning_array(void)
{
    std::mtcout << MT("\n>>>>> TEST ARRAY BINDING WITH RETURNING CLAUSE \n\n");

    std::vector<int>     tab_int;
    std::vector<double>  tab_dbl;
    std::vector<float>   tab_flt;
    std::vector<dstring> tab_str;
    std::vector<Date>    tab_date;
    std::vector<Clob>    tab_lob;
    std::vector<File>    tab_file;

    for(int i = 0; i < SIZE_TAB; i++)
    {
        tab_int.push_back(i+1);
        tab_dbl.push_back(3.14*(double)(i+1));
        tab_flt.push_back((float) 3.14*(float)(i+1));

        dstring str;
        str += DT("Name");
        str += ( (i+1) +'0');
        tab_str.push_back(str);

        Date date;
        date.SysDate();
        tab_date.push_back(date);

        Clob clob(con);
        clob.Write(DT("Lob value ") + str);
        tab_lob.push_back(clob);

        mstring fileName;
        fileName += MT("File");
        fileName += ( (i+1) +'0');
        File file(con, MT("Mydir"), fileName);
        tab_file.push_back(file);
    }

    Statement st(con);

    st.Prepare( MT("insert into test_array ")
                MT("( ")
                MT("   val_int,  val_dbl, val_flt, val_str, val_date, ")
                MT( "   val_lob, val_file ")
                MT( ") " )
                MT( "values ")
                MT( "( ")
                MT( "   :val_int, :val_dbl, :val_flt, :val_str, :val_date, ")
                MT( "   :val_lob, :val_file ")
                MT(") ")
                MT("returning")
                MT( "  val_int,  val_dbl, val_flt, val_str, val_date, ")
                MT("   val_lob, val_file ")
                MT("into  " )
                MT( "  :out_int, :out_dbl, :out_flt,  :out_str, :out_date, ")
                MT("   :out_lob, :out_file "));

    st.SetBindArraySize(SIZE_TAB);

    /* bind vectors */
    st.Bind(MT(":val_int"),  tab_int, OCI_BDM_IN);
    st.Bind(MT(":val_dbl"),  tab_dbl, OCI_BDM_IN);
    st.Bind(MT(":val_flt"),  tab_flt, OCI_BDM_IN);
    st.Bind(MT(":val_date"), tab_date, OCI_BDM_IN);
    st.Bind(MT(":val_lob"),  tab_lob,  OCI_BDM_IN);
    st.Bind(MT(":val_file"), tab_file, OCI_BDM_IN);
    st.Bind(MT(":val_str"),  tab_str, 30, OCI_BDM_IN);

    /* register output */
    st.Register<int    >(MT(":out_int"));
    st.Register<double >(MT(":out_dbl"));
    st.Register<float  >(MT(":out_flt"));
    st.Register<Date   >(MT(":out_date"));
    st.Register<Clob   >(MT(":out_lob"));
    st.Register<File   >(MT(":out_file"));
    st.Register<dstring>(MT(":out_str"), 30);

    st.Execute();
    std::mtcout << std::endl << st.GetAffectedRows() << MT(" row(s) inserted") << std::endl;

    int rowIndex = 0;

    Resultset rs = st.GetResultset();
    while (!rs.IsNull())
    {
        while (rs++)
        {
            std::dtcout << DT("Row #") << ++rowIndex << DT("---------------") << std::endl;

            std::dtcout << DT(".... val_int    : ") << rs.Get<int>(MT(":OUT_INT"))     << std::endl;
            std::dtcout << DT(".... val_dbl    : ") << rs.Get<double>(MT(":OUT_DBL"))  << std::endl;
            std::dtcout << DT(".... val_flt    : ") << rs.Get<float>(MT(":OUT_FLT"))   << std::endl;
            std::dtcout << DT(".... val_str    : ") << rs.Get<dstring>(MT(":OUT_STR")) << std::endl;

            Date date = rs.Get<Date>(MT(":OUT_DATE"));
            std::mtcout << MT(".... val_date   : ") << date.ToString(MT("YYYY-MM-DD HH24:MI:SS")) << std::endl;

            Clob clob = rs.Get<Clob>(MT(":OUT_LOB"));
            std::dtcout << DT(".... val_lob    : ") << clob.Read(SIZE_BUF) << std::endl;

            File file = rs.Get<File>(MT(":OUT_FILE"));
            std::mtcout << MT(".... val_file   : ") << file.GetDirectory() << MT("/") << file.GetName() << std::endl;
        }

        rs = st.GetNextResultset();
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_object_insert
 * --------------------------------------------------------------------------------------------- */

void test_object_insert(void)
{
    std::mtcout << MT("\n>>>>> TEST OBJECT BINDING \n\n");

    Clob clob(con);
    clob.Write(DT("Lob Value"));
    File file(con,  MT("mydir"), MT("myfile"));
    Date date;
    date.SysDate();

    Object obj2(TypeInfo(con, MT("type_t"), OCI_TIF_TYPE));

    obj2.Set<int>    (MT("ID"), 1);
    obj2.Set<dstring>(MT("NAME"), DT("USB KEY 2go"));

    Object obj1(TypeInfo(con, MT("test_t"), OCI_TIF_TYPE));

    obj1.Set<int>    (MT("VAL_INT"), 1);
    obj1.Set<double> (MT("VAL_DBL"), 3.14);
    obj1.Set<float>  (MT("VAL_FLT"), (float) 3.14);
    obj1.Set<dstring>(MT("VAL_STR"), DT("USB KEY 2go"));
    obj1.Set<void *> (MT("VAL_RAW"), (void*) "0123456789", 10);
    obj1.Set<Date>   (MT("VAL_DATE"), date);
    obj1.Set<Object> (MT("VAL_OBJ"), obj2);
    obj1.Set<Clob>   (MT("VAL_LOB"), clob);
    obj1.Set<File>   (MT("VAL_FILE"), file);

    Statement st(con);
    st.Prepare(MT("insert into test_object values(:obj)"));
    st.Bind(MT(":obj"), obj1, OCI_BDM_IN);
    st.Execute();

    std::mtcout << MT("Rows inserted :  ") << st.GetAffectedRows() << std::endl;

    con.Commit();
}

/* --------------------------------------------------------------------------------------------- *
 * test_object_fetch
 * --------------------------------------------------------------------------------------------- */

void test_object_fetch(void)
{
    std::mtcout << MT("\n>>>>> TEST OBJECT FETCHING \n\n");

    Statement st(con);
    st.Execute(MT("select val from test_object for update"));

    Resultset rs = st.GetResultset();

    while (rs++)
    {
        Object obj  = rs.Get<Object>(1);

        std::dtcout << DT(".... val_int      : ") << obj.Get<int>(MT("VAL_INT"))     << std::endl;
        std::dtcout << DT(".... val_dbl      : ") << obj.Get<double>(MT("VAL_DBL"))  << std::endl;
        std::dtcout << DT(".... val_flt      : ") << obj.Get<float>(MT("VAL_FLT"))   << std::endl;
        std::dtcout << DT(".... val_str      : ") << obj.Get<dstring>(MT("VAL_STR")) << std::endl;

        Date date = obj.Get<Date>(MT("VAL_DATE"));
        std::mtcout << MT(".... val_date     : ") << date.ToString(MT("YYYY-MM-DD HH24:MI:SS")) << std::endl;

        Clob clob = obj.Get<Clob>(MT("VAL_LOB"));
        std::dtcout << DT(".... val_lob      : ") << clob.Read(SIZE_BUF) << std::endl;

        File file = obj.Get<File>(MT("VAL_FILE"));
        std::mtcout << MT(".... val_file     : ") << file.GetDirectory() << MT("/") << file.GetName() << std::endl;

        char buffer[11];
        unsigned int len = 10;
        obj.Get<void *>(MT("VAL_RAW"), (BufferPointer) &buffer, len);
        buffer[len] = 0;
        std::cout << ".... val_raw      : " << buffer << std::endl;

        Object obj2 = obj.Get<Object>(MT("VAL_OBJ"));
        std::dtcout << DT(".... val_obj.code : ") << obj2.Get<int>(MT("ID"))       << std::endl;
        std::dtcout << DT(".... val_obj.name : ") << obj2.Get<dstring>(MT("NAME")) << std::endl;
    }

    con.Commit();
}

/* --------------------------------------------------------------------------------------------- *
 * test_scrollable_cursor
 * --------------------------------------------------------------------------------------------- */

void test_scrollable_cursor(void)
{
    if (Environment::GetRuntimeVersion() > OCI_9_0)
    {   
        std::mtcout << MT("\n>>>>> TEST SCROLLABLE CURSORS \n\n");

        Statement st(con);
        
        st.SetFetchMode(OCI_SFM_SCROLLABLE);
        st.Execute(MT("select table_name from user_tables where ")
                   MT("table_name like 'TEST_%' order by table_name"));
   
        Resultset rs = st.GetResultset();
        
        rs.Last();
        std::mtcout << MT("Total rows : ") << rs.GetCount() << std::endl;
        
        std::mtcout << MT("... Go to row 1\n");
        rs.First();
        std::dtcout << DT("table ") << rs.Get<dstring>(1) << std::endl;

        std::mtcout << MT("... Enumerate from row 2 to row ") << rs.GetCount() << MT(" ") << std::endl;
        while (rs++)
        {
            std::dtcout << DT("table ") << rs.Get<dstring>(1) << std::endl;
        }

        std::mtcout << MT("... Enumerate from row ") << rs.GetCount() -1  << MT(" back to row 1") << std::endl;        
        while (rs.Prev())
        {
            std::dtcout << DT("table ") << rs.Get<dstring>(1) << std::endl;
        }

        std::mtcout << MT("... Go to the 3th row") << std::endl;
        rs.Seek(OCI_SFD_ABSOLUTE, 3);
        std::dtcout << DT("table ") << rs.Get<dstring>(1) << std::endl;

        std::mtcout << MT("... Fetch the next 2 rows") << std::endl;
        while (rs.GetCurrentRow() < 5 && rs++)
        {
            std::dtcout << DT("table ") << rs.Get<dstring>(1) << std::endl;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_collection
 * --------------------------------------------------------------------------------------------- */
void test_collection(void)
{
    std::mtcout << MT("\n>>>>> TEST VARRAY BINDING WITH ITERATOR \n\n");

    int i;

    Statement st(con);
    TypeInfo type(con, MT("T_TAB1_EMP"), OCI_TIF_TYPE);
    Collection coll(type);

    st.Prepare( MT("begin")
                MT("    select employees into :tab_emp ")
                MT("    from test_coll_varray ")
                MT("    where departement = :id; ")
                MT("end;"));

    st.Bind(MT(":tab_emp"), coll, OCI_BDM_IN);
    st.Bind(MT(":id"), i, OCI_BDM_IN);

    i = 1;

    st.Execute();

    std::mtcout << MT("Department ID #") << i << std::endl;

    CollectionIterator iter(coll);
    while (iter++)
    {
        std::dtcout << DT("... Employee : ") << iter.Get<dstring>() << std::endl;
    }

    std::mtcout << MT("\n>>>>> TEST VARRAY FETCHING WITH ITERATOR \n\n");

    st.Execute(MT("SELECT * from test_coll_varray"));
 
    Resultset rs = st.GetResultset();
    while (rs++)
    {
        std::dtcout << DT("Department ID #") << rs.Get<int>(1) << std::endl;

        coll = rs.Get<Collection>(2);
        CollectionIterator iter2(coll);
        while (iter2++)
        {
            std::dtcout << DT("... Employee : ") << iter2.Get<dstring>() << std::endl;
        }
    }

    std::mtcout << MT("\n>>>>> TEST NESTED TABLE FETCHING WITH INDEX ACCESS \n\n");

    st.Execute(MT("SELECT * from test_coll_nested"));

    rs = st.GetResultset();
    while (rs++)
    {
        std::dtcout << DT("Department ID #") << rs.Get<int>(1) << std::endl;

        coll = rs.Get<Collection>(2);
        for(int index = 1, n = coll.GetSize(); index <= n; index++)
        {
            std::dtcout << DT("... Employee : ") << coll.Get<dstring>(index) << std::endl;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_ref
 * --------------------------------------------------------------------------------------------- */
void test_ref(void)
{
    std::mtcout << MT("\n>>>>> TEST REF FETCHING \n\n");

    Statement st(con);

    st.Execute(MT("select ref(e) from test_table_obj e"));

    Resultset rs = st.GetResultset();
    while (rs++)
    {
        Reference ref = rs.Get<Reference>(1);
        Object   obj  = ref.GetObject();
        
        std::dtcout << obj.Get<int>(MT("ID")) << DT(" - ") << obj.Get<dstring>(MT("NAME")) << std::endl;
    }

    std::mtcout << MT("\n>>>>> TEST REF PL/SQL BINDING \n\n");

    Reference ref(TypeInfo(con, MT("type_t"), OCI_TIF_TYPE));

    st.Prepare (MT("begin ")
                MT("  select ref(e) into :r from test_table_obj e where e.id = 1; ")
                MT("end; "));

    st.Bind(MT(":r"), ref, OCI_BDM_IN_OUT);
    st.Execute();

    Object obj = ref.GetObject();
    std::dtcout << obj.Get<int>(MT("ID")) << DT(" - ") << obj.Get<dstring>(MT("NAME")) << std::endl;
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

        std::mtcout << MT("\n>>>>> TEST DIRECT PATH (10 loads of 100 rows) \n\n");
    
        int i = 0, j = 0, nb_rows = SIZE_ARRAY;

        DirectPath directPath(TypeInfo(con, MT("test_directpath"), OCI_TIF_TABLE), NUM_COLS, nb_rows);

        /* optional attributes to set */

        directPath.SetBufferSize(64000);
        directPath.SetNoLog(true);
        directPath.SetParallel(true);

        if(Environment::GetCompileVersion() >= OCI_9_2)
        {
           directPath.SetCacheSize(100);
        }

        /* describe the target table */

        directPath.SetColumn(1, MT("VAL_INT"),  SIZE_COL1);
        directPath.SetColumn(2, MT("VAL_STR"),  SIZE_COL2);
        directPath.SetColumn(3, MT("VAL_DATE"), SIZE_COL3, MT("YYYYMMDD"));

        /* prepare the load */

        directPath.Prepare();

        nb_rows = directPath.GetMaxRows();          
            
        for (i = 0; i < NB_LOAD ; i++)
        {
            directPath.Reset();

            for (j = 1; j <= nb_rows; j++)
            {             
                std::dtostringstream val1, val2, val3;

                /* fill test values */

                val1 << std::setfill(DT('0')) << std::setw(4) << j + (i*100);
                val2 << DT("value ") << std::setfill(DT('0')) << std::setw(5) << j + (i*100);
                val3 << std::setfill(DT('0')) << std::setw(2) << (j%23)+1 + 2000 <<  std::setw(2) << (j%11)+1 << (j%23)+1;

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

       std::mtcout << std::setw(4) << directPath.GetRowCount() << MT(" row(s) loaded") << std::endl;
   }
}

