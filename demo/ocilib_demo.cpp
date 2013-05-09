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

using namespace ocilib;

#include <iostream>
#include <fstream>
#include <iomanip>

/* --------------------------------------------------------------------------------------------- *
 * COMMON DEFINES
 * --------------------------------------------------------------------------------------------- */

#define ARG_DB     1
#define ARG_USER   2
#define ARG_PWD    3
#define ARG_HOME   4

#define ARG_COUNT  5

#define SIZE_BUF   2048
#define SIZE_TAB   5

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
        {test_object_insert,     TRUE}/*,
        {test_object_fetch,      TRUE},
        {test_scrollable_cursor, TRUE},
        {test_collection,        TRUE},
        {test_ref,               TRUE},
        {test_directpath,        FALSE}*/
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

        std::cout << "Connecting to " << usr << "/" << pwd << "@" << dbs << std::endl << std::endl;

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
         std::cout << ex.GetMessage() << std::endl;
    }

    Environment::Cleanup();

    std::cout << "\nPress any key to exit...";

    getchar(); 
 
    return EXIT_SUCCESS;
}


/* --------------------------------------------------------------------------------------------- *
 * print_version
 * --------------------------------------------------------------------------------------------- */

void print_version(void)
{
    std::cout << "\n>>>>> OCILIB BUILD INFORMATION \n\n";

    if (Environment::GetImportMode() == OCI_IMPORT_MODE_LINKAGE)
        std::cout << "OCI import mode         : LINKAGE\n";
    else
        std::cout << "OCI import mode         : RUNTIME\n";

    if (Environment::GetCharsetMetaData() == OCI_CHAR_ANSI)
        std::cout << "MetaData  char type     : ANSI\n";
    else
        std::cout << "MetaData  char type     : WIDE\n";

    if (Environment::GetCharsetUserData() == OCI_CHAR_ANSI)
        std::cout << "UserData  char type     : ANSI\n";
    else
        std::cout << "UserData  char type     : WIDE\n";

    std::cout << "\n>>>>> VERSIONS INFORMATION \n\n";
    std::cout << "OCILIB major    version : " << OCILIB_MAJOR_VERSION     << std::endl;
    std::cout << "OCILIB minor    version : " << OCILIB_MINOR_VERSION     << std::endl;
    std::cout << "OCILIB revision version : " << OCILIB_REVISION_VERSION  << std::endl;
    std::cout << "OCI compile     version : " << OCI_VER_MAJ(Environment::GetCompileVersion()) << std::endl;
    std::cout << "OCI runtime     version : " << OCI_VER_MAJ(Environment::GetRuntimeVersion()) << std::endl;
    std::cout << "Server major    version : " << con.GetServerMajorVersion()    << std::endl;
    std::cout << "Server minor    version : " << con.GetServerMinorVersion()    << std::endl;
    std::cout << "Server revision version : " << con.GetServerRevisionVersion() << std::endl;
    std::cout << "Connection      version : " << con.GetVersion() << std::endl;

    std::cout << "\n>>>>> SERVER VERSION BANNER \n\n";
    std::cout << con.GetServerVersion() << std::endl << std::endl;
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
         std::cout << ex.GetMessage() << std::endl;
    }
}

/* --------------------------------------------------------------------------------------------- *
 * create_tables
 * --------------------------------------------------------------------------------------------- */

void create_tables(void)
{
    std::cout << "\n>>>>> CREATE TABLES FOR DEMO \n\n";

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
    std::cout << "\n>>>>> DROPPING TABLES AND TYPES \n\n";

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
    std::cout << "\n>>>>> SIMPLE TEST FETCH WITH META DATA\n\n";

    Statement st(con);
    st.Execute(MT("select * from test_fetch"));

    Resultset rs = st.GetResultset();
    for(int i = 1, n = rs.GetColumnCount(); i <= n; i++)
    {
        Column col = rs.GetColumn(i);
        std::cout << "> Field : #" << i << " - Name : " << col.GetName() << std::endl;
    }

    std::cout << std::endl;

    while (rs.Next())
    {
        std::cout << "> code   : "   << rs.Get<int>(1)
                  << ", action : " << rs.Get<dstring>(2)
                  << ", price  : " << rs.Get<double>(3)
                  << ", date   : " << rs.Get<Date>(4).ToString()
                  <<  std::endl;
    }

    std::cout << std::endl << rs.GetCount() << " row(s) fetched" << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_bind1
 * --------------------------------------------------------------------------------------------- */

void test_bind1(void)
{
    int code = 1;

    std::cout << "\n>>>>> TEST BINDING \n\n";

    Statement st(con);
    st.Prepare(MT("select * from test_fetch where code = :code"));
    st.Bind(MT(":code"), code, OCI_BDM_IN);
    st.Execute();

    Resultset rs = st.GetResultset();
    while (rs.Next())
    {
        std::cout << "> code   : "   << rs.Get<int>(1)
                  << ", action : " << rs.Get<dstring>(2)
                  << ", price  : " << rs.Get<double>(3)
                  << ", date   : " << rs.Get<Date>(4).ToString()
                  <<  std::endl;
    }

    std::cout << std::endl << rs.GetCount() << " row(s) fetched" << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_bind2
 * --------------------------------------------------------------------------------------------- */

void test_bind2(void)
{
    std::cout << "\n>>>>> SINGLE BINDING \n\n";

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

    /* lob */
    Clob clob(con);
    clob.Write(MT("lob value00"));

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
    st.Bind(MT(":val_str"), str, str.size(), OCI_BDM_IN);

    /* bind oracle types arrays */

    st.Bind(MT(":val_date"), date, OCI_BDM_IN);
    st.Bind(MT(":val_lob"),  clob, OCI_BDM_IN);
    st.Bind(MT(":val_file"), file, OCI_BDM_IN);

    /* do insert */

    st.Execute();
    con.Commit();

    std::cout << std::endl << st.GetAffectedRows() << " row(s) inserted" << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_piecewise_insert
 * --------------------------------------------------------------------------------------------- */

void test_piecewise_insert(void)
{
    std::cout << "\n>>>>> TEST PIECEWISE INSERTING\n\n";

    std::ifstream file(OCI_SHARED_LIB, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        std::streamoff size = file.tellg();
        file.seekg (0, std::ios::beg);
        std::cout << std::endl << size << " bytes to write" << std::endl;
              
        Statement st(con);
        BLong lg(st);
        st.Prepare(MT("insert into test_long_raw(code, content values (1, :data)"));
        st.Bind(MT(":data"), lg, (int) size, OCI_BDM_IN);
        st.Execute();

        char buffer[SIZE_BUF];
        int bytesRead;

        while (bytesRead = (int) file.readsome(buffer, SIZE_BUF))
        {
            lg.Write(buffer, bytesRead);
        }

        std::cout << std::endl << lg.GetSize() << " bytes written" << std::endl;
        
        file.close();
        con.Commit();
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_piecewise_fetch
 * --------------------------------------------------------------------------------------------- */

void test_piecewise_fetch(void)
{
    std::cout << "\n>>>>> TEST PIECEWISE FETCHING\n\n";

    Statement st(con);
    st.Execute(MT("select content from test_long_raw where code = 1"));

    Resultset rs1 = st.GetResultset();
    while (rs1.Next())
    {
        BLong lg = rs1.Get<BLong>(1);
        
        char buffer[SIZE_BUF];
        int bytesRead;

        while ((bytesRead = lg.Read(buffer, SIZE_BUF))) {}

        std::cout << std::endl << lg.GetSize() << " bytes read" << std::endl;
    }

    std::cout << std::endl << rs1.GetCount() << " row(s) fetched" << std::endl;

    std::cout << "\n>>>>> TEST LONG MAPPED TO STRING\n\n";

    st.Execute(MT("select content from test_long_str where code = 1"));
    st.SetLongMode(OCI_LONG_IMPLICIT);

    Resultset rs2 = st.GetResultset();
    while (rs2.Next())
    {
        dstring str = rs2.Get<dstring>(1);

        std::cout << str << std::endl;
        std::cout << str.size() << " bytes read" << std::endl;
    }

    std::cout << std::endl << rs2.GetCount() << " row(s) fetched" << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_lob
 * --------------------------------------------------------------------------------------------- */

void test_lob(void)
{
    std::cout << "\n>>>>> TEST LOB MANIPULATION\n\n";

    Statement st(con);
    st.Execute(MT("select code, content from test_lob where code=1 for update"));
    
    Resultset rs= st.GetResultset();
    while (rs.Next())
    {
        Clob clob = rs.Get<Clob>(2);

        clob.Write(DT("today, "));
        clob.Append(DT("i'm going to the cinema ! "));
        clob.Seek( OCI_SEEK_SET, 0);

        std::cout << "> code : " << rs.Get<int>(1) << ", content : " << clob.Read(SIZE_STR) << std::endl;
    }

    con.Commit();

    std::cout << std::endl << rs.GetCount() << " row(s) fetched" << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_nested_table
 * --------------------------------------------------------------------------------------------- */

void test_nested_table(void)
{
    std::cout << "\n>>>>> TEST NESTED TABLE \n\n";

    Statement st(con);
    st.Execute(MT("select article, cursor(select sysdate from dual) from test_fetch"));
    
    Resultset rs= st.GetResultset();
    while (rs.Next())
    {
        Statement st2 = rs.Get<Statement>(2);
        Resultset rs2 = st2.GetResultset();

        while (rs2.Next())
        {
            std::cout << "Article : " << rs.Get<dstring>(1) << ", Date : " << rs2.Get<dstring>(1) << std::endl;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_ref_cursor
 * --------------------------------------------------------------------------------------------- */

void test_ref_cursor(void)
{
    std::cout << "\n>>>>> TEST REF CURSOR  \n\n";

    Statement stBind(con);

    Statement st(con);
    st.Prepare(MT("begin open :c for select * from test_fetch; end;"));
    st.Bind( MT(":c"), stBind, OCI_BDM_OUT);
    st.Execute();

    Resultset rs = stBind.GetResultset();
    while (rs.Next())
    {
        std::cout << "> code   : "   << rs.Get<int>(1)
                  << ", action : " << rs.Get<dstring>(2)
                  << ", price  : " << rs.Get<double>(3)
                  << ", date   : " << rs.Get<Date>(4).ToString()
                  <<  std::endl;
    }

   std::cout << std::endl << rs.GetCount() << " row(s) fetched" << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_plsql
 * --------------------------------------------------------------------------------------------- */

void test_plsql(void)
{
    std::cout << "\n>>>>> TEST PL/SQL OUTPUT BIND\n\n";

    int res = 0;
    Statement st(con);

    st.Prepare(MT("begin :res := trunc(sysdate+1)-trunc(sysdate-1); end;"));
    st.Bind( MT(":res"), res, OCI_BDM_OUT);
    st.Execute();
    std::cout << "PL/SQL : trunc(sysdate+1)-trunc(sysdate-1)" << std::endl;
    std::cout << "Result : " << res << std::endl;


#ifndef OCI_CHARSET_ANSI

   /* Oracle 8i has some troubles with SERVER OUTPUT in unicode */
   if (con.GetRuntimeVersion() < OCI_9_0) 
        return;     

#endif

    std::cout << "\n>>>>> TEST PL/SQL SERVER OUTPUT\n\n" ;

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
        std::cout << line << std::endl;
    }

    con.DisableServerOutput();
}

/* --------------------------------------------------------------------------------------------- *
 * test_dates
 * --------------------------------------------------------------------------------------------- */

void test_dates(void)
{
    std::cout << "\n>>>>> TEST DATETIME MANIPULATION\n\n";

    Date d1, d2;

    d1.FromString("13041978 20:20:12", "DDMMYYYY HH24:MI:SS");
    std::cout << d1.ToString("DD/MM/YYYY HH24:MI:SS") << std::endl;

    d1.SysDate();
    std::cout << d1.ToString("DD/MM/YYYY HH24:MI:SS") << std::endl;

    d1.AddDays(5);
    d1.AddMonths(2);
    std::cout << "Date + 5 days and 2 months is " << d1.ToString("DD/MM/YYYY HH24:MI:SS") << std::endl;

    d2.Assign(d1);
        
    d1.LastDay();
    std::cout << "Last day of the month : " << d1.ToString("DD/MM/YYYY HH24:MI:SS") << std::endl;

    std::cout << "Number of days until the end of the months : " << d1.DaysBetween(d2) << std::endl;
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
        std::cout << "\n>>>>> TEST TIMESTAMP\n\n";

        Timestamp tm(OCI_TIMESTAMP);

        tm.SysTimestamp();
        std::cout << "Current timestamp : " << tm.ToString("DD/MM/YYYY HH24:MI:SS:FF3") << std::endl;

        /* intervals raw oci functions have some troubles with Oracle 9i. So let's
           use it for the demo only if we're using 10g or above */

        if (version >= OCI_10_1)
        {
            std::cout << "\n>>>>> TEST INTERVAL \n\n";

            Interval itv(OCI_INTERVAL_DS);
            itv.SetDaySecond(1,1,1,1,0);
            tm.AddInterval(itv);
            std::cout << "Current timestamp + Interval :" << tm.ToString("DD/MM/YYYY HH24:MI:SS:FF3") << std::endl;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_describe
 * --------------------------------------------------------------------------------------------- */

void test_describe(void)
{
    std::cout << "\n>>>>> TEST DESCRIBING TABLE \n\n";
    std::cout << "Column Name         Type                " << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout <<  std::setiosflags(std::ios::left);

    TypeInfo table(con, "test_fetch", OCI_TIF_TABLE);

    for(int i = 1, n = table.GetColumnCount(); i <= n; i++)
    {
        Column col = table.GetColumn(i);

        std::cout <<  std::setw(20) << col.GetName().c_str() 
                  <<  std::setw(20) << col.GetFullSQLType().c_str() 
                  <<  std::endl;
    }

    /* TEST DESCRIBING TYPE ------------------------------------------------- */

    std::cout << "\n>>>>> TEST DESCRIBING TYPE \n\n";
    std::cout << "Column Name         Type                " << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout <<  std::setiosflags(std::ios::left);

    TypeInfo type(con, "test_t", OCI_TIF_TYPE);

    for(int i = 1, n = type.GetColumnCount(); i <= n; i++)
    {
        Column col = type.GetColumn(i);

        std::cout <<  std::setw(20) << col.GetName().c_str() 
                  <<  std::setw(20) << col.GetFullSQLType().c_str() 
                  <<  std::endl;
    }  
}

/* --------------------------------------------------------------------------------------------- *
 * test_returning
 * --------------------------------------------------------------------------------------------- */

void test_returning(void)
{
    std::cout << "\n>>>>> TEST RETURNING CLAUSE \n\n";

    Statement st(con);
    st.Prepare("update test_lob set code = code + 1 returning code, content into :i, :l");
    st.Register<int>(":i");
    st.Register<Clob>(":l");
    st.Execute();

    Resultset rs = st.GetResultset();
    while (rs.Next())
    {
        Clob clob = rs.Get<Clob>(2);
        clob.Append(DT("(modified)"));
        clob.Seek(OCI_SEEK_SET, 0);

        std::cout << "> code : " << rs.Get<int>(1) << " - " << clob.Read((int)clob.GetLength()) << std::endl;
    }

    con.Commit();
 
    std::cout << std::endl << rs.GetCount() << " row(s) fetched" << std::endl;
}

/* --------------------------------------------------------------------------------------------- *
 * test_returning_array
 * --------------------------------------------------------------------------------------------- */

void test_returning_array(void)
{
    std::cout << "\n>>>>> TEST ARRAY BINDING WITH RETURNING CLAUSE \n\n";

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
        str += "Name";
        str += ( (i+1) +'0');
        tab_str.push_back(str);

        Date date;
        date.SysDate();
        tab_date.push_back(date);

        Clob clob(con);
        clob.Write("Lob value " + str);
        tab_lob.push_back(clob);

        File file(con, MT("mydir"), "File" + str);
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
    std::cout << std::endl << st.GetAffectedRows() << " row(s) inserted" << std::endl;

    Resultset rs = st.GetResultset();
    while (!rs.IsNull())
    {
        while (rs.Next())
        {
            std::cout << "Row #" << rs.GetCount() << "---------------" << std::endl;

            std::cout << ".... val_int    : " << rs.Get<int>(MT(":OUT_INT"))     << std::endl;
            std::cout << ".... val_dbl    : " << rs.Get<double>(MT(":OUT_DBL"))  << std::endl;
            std::cout << ".... val_flt    : " << rs.Get<float>(MT(":OUT_FLT"))   << std::endl;
            std::cout << ".... val_str    : " << rs.Get<dstring>(MT(":OUT_STR")) << std::endl;

            Date date = rs.Get<Date>(MT(":OUT_DATE"));
            std::cout << ".... val_date   : " << date.ToString(MT("YYYY-MM-DD HH24:MI:SS")) << std::endl;

            Clob clob = rs.Get<Clob>(MT(":OUT_LOB"));
            std::cout << ".... val_lob    : " << clob.Read(SIZE_BUF) << std::endl;

            File file = rs.Get<File>(MT(":OUT_FILE"));
            std::cout << ".... val_file   : " << file.GetDirectory() << "/" << file.GetName() << std::endl;
        }

        rs = st.GetNextResultset();
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_object_insert
 * --------------------------------------------------------------------------------------------- */

void test_object_insert(void)
{
    std::cout << "\n>>>>> TEST OBJECT BINDING \n\n";

    Clob clob(con);
    File file(con,  MT("mydir"), MT("myfile"));
    Date date;
    date.SysDate();

    Object obj2(TypeInfo(con, "type_t", OCI_TIF_TYPE));

    obj2.Set<int>    (MT("ID"), 1);
    obj2.Set<dstring>(MT("NAME"), DT("USB KEY 2go"));

    Object obj1(TypeInfo(con, "test_t", OCI_TIF_TYPE));

    obj1.Set<int>    (MT("VAL_INT"), 1);
    obj1.Set<double> (MT("VAL_DBL"), 3.14);
    obj1.Set<float>  (MT("VAL_FLT"), (float) 3.14);
    obj1.Set<dstring>(MT("VAL_STR"), DT("USB KEY 2go"));
    obj1.Set<void *> (MT("VAL_RAW"), "0123456789", 10);
    obj1.Set<Date>   (MT("VAL_DATE"), date);
    obj1.Set<Object> (MT("VAL_OBJ"), obj2);
    obj1.Set<Clob>   (MT("VAL_LOB"), clob);
    obj1.Set<File>   (MT("VAL_FILE"), file);

    Statement st(con);
    st.Prepare("insert into test_object values(:obj)");
    st.Bind(":obj", obj1, OCI_BDM_IN);
    st.Execute();

    std::cout << "Rows inserted :  " << st.GetAffectedRows() << std::endl;
 
    con.Commit();
}
