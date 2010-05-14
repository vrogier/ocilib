/*
   +----------------------------------------------------------------------+
   |                                                                      |
   |                     OCILIB - C Driver for Oracle                     |
   |                                                                      |
   |                      (C Wrapper for Oracle OCI)                      |
   |                                                                      |
   |                         DEMO SOURCE FILE                             |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                      Website : http://ocilib.net                     |
   +----------------------------------------------------------------------+
   |               Copyright (c) 2007-2010 Vincent ROGIER                 |
   +----------------------------------------------------------------------+
   | This library is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU Lesser General Public           |
   | License as published by the Free Software Foundation; either         |
   | version 2 of the License, or (at your option) any later version.     |
   |                                                                      |
   | This library is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    |
   | Lesser General Public License for more details.                      |
   |                                                                      |
   | You should have received a copy of the GNU Lesser General Public     |
   | License along with this library; if not, write to the Free           |
   | Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   |
   +----------------------------------------------------------------------+
   |          Author: Vincent ROGIER <vince.rogier@gmail.com>             |
   +----------------------------------------------------------------------+
*/

#include "ocilib_demo.h"

/* ------------------------------------------------------------------------ *
 * prototypes
 * ------------------------------------------------------------------------ */

void err_handler(OCI_Error *err);
void print_version(void);

void cleanup(void);
void disconnect(void);

void create_tables(void);
void drop_tables(void);

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
        {test_format,            TRUE},
        {test_immediate,         TRUE},
        {test_immediate_format,  TRUE},
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

/* ------------------------------------------------------------------------ *
 * variables
 * ------------------------------------------------------------------------ */

static OCI_Connection *cn = NULL;
static OCI_Statement  *st = NULL;
static OCI_Resultset  *rs = NULL;

static mtext str[SIZE_STR+1];
static dtext temp[SIZE_STR+1];

static int nb_err  = 0;
static int nb_warn = 0;

/* ------------------------------------------------------------------------ *
 * err_handler
 * ------------------------------------------------------------------------ */

void err_handler(OCI_Error *err)
{
    int err_type = OCI_ErrorGetType(err);

    print_text("\n");

    if (err_type == OCI_ERR_WARNING)
    {
         print_text("> WARNING : ");
         nb_warn++;
    }
    else
    {
        print_text("> ERROR   : ");
        nb_err++;
    }

    print_mstr(OCI_ErrorGetString(err));
    print_text("\n");
}

/* ------------------------------------------------------------------------ *
 * main
 * ------------------------------------------------------------------------ */

int mtmain(int argc, mtarg* argv[])
{
    mtext home[SIZE_STR+1] = MT("");
    mtext dbs [SIZE_STR+1] = MT("");
    mtext usr [SIZE_STR+1] = MT("");
    mtext pwd [SIZE_STR+1] = MT("");

    size_t i;

    /* CHECK COMMAND LINE --------------------------------------------------- */

    if (argc < (ARG_COUNT-1))
    {
        return EXIT_FAILURE;
    }

    /* GET ARGUMENTS ---------------------------------------------------------*/

    GET_ARG(dbs, ARG_DB);
    GET_ARG(usr, ARG_USER);
    GET_ARG(pwd, ARG_PWD);

    if(argc == ARG_COUNT)
        GET_ARG(home, ARG_HOME);

    /* INITIALIZE OCI ------------------------------------------------------- */

    if (!OCI_Initialize(err_handler, home, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    OCI_EnableWarnings(TRUE);

    /* CONNECTION TO SERVER ------------------------------------------------- */

    print_text("Connecting to ");
    print_args(usr);
    print_text("/");
    print_args(pwd);
    print_text("@");
    print_args(dbs);
    print_text("\n\n");

    cn = OCI_ConnectionCreate(dbs, usr, pwd, OCI_SESSION_DEFAULT);

    if (cn)
    {
        st = OCI_StatementCreate(cn);

        print_version();
        create_tables();

        /* execute tests */

        for (i = 0; i < ARRAY_COUNT(tab_test); i++)
        {
            if (tab_test[i].execute)
                tab_test[i].proc();
        }

        drop_tables();

        disconnect();
    }
    else
    {
        print_mstr(OCI_ErrorGetString(OCI_GetLastError()));
    }

    cleanup();

    print_text("\npress any key to exit...");

    getchar();

    return EXIT_SUCCESS;
}

/* ------------------------------------------------------------------------ *
 * cleanup
 * ------------------------------------------------------------------------ */

void cleanup(void)
{
    OCI_Cleanup();

    print_frmt("\n%i errors   \n", nb_err);
    print_frmt("\n%i warnings \n\n", nb_warn);
}

/* ------------------------------------------------------------------------ *
 * disconnect
 * ------------------------------------------------------------------------ */

void disconnect(void)
{
    OCI_ConnectionFree(cn);
}

/* ------------------------------------------------------------------------ *
 * print_version
 * ------------------------------------------------------------------------ */

void print_version(void)
{
    print_text("\n>>>>> OCILIB BUILD INFORMATION \n\n");

    if (OCI_GetImportMode() == OCI_IMPORT_MODE_LINKAGE)
        print_text("OCI import mode         : LINKAGE\n");
    else
        print_text("OCI import mode         : RUNTIME\n");

    if (OCI_GetCharsetMetaData() == OCI_CHAR_ANSI)
        print_text("MetaData  char type     : ANSI\n");
    else
        print_text("MetaData  char type     : WIDE\n");

    if (OCI_GetCharsetUserData() == OCI_CHAR_ANSI)
        print_text("UserData  char type     : ANSI\n");
    else
        print_text("UserData  char type     : WIDE\n");

    print_text("\n>>>>> VERSIONS INFORMATION \n\n");

    print_frmt("OCILIB major    version : %i\n",   OCILIB_MAJOR_VERSION);
    print_frmt("OCILIB minor    version : %i\n",   OCILIB_MINOR_VERSION);
    print_frmt("OCILIB revision version : %i\n\n", OCILIB_REVISION_VERSION);

    /* print all versions */
    print_frmt("OCI compile     version : %i\n",   OCI_VER_MAJ(OCI_GetOCICompileVersion()));
    print_frmt("OCI runtime     version : %i\n\n", OCI_VER_MAJ(OCI_GetOCIRuntimeVersion()));

    print_frmt("Server major    version : %i\n",   OCI_GetServerMajorVersion(cn));
    print_frmt("Server minor    version : %i\n",   OCI_GetServerMinorVersion(cn));
    print_frmt("Server revision version : %i\n\n", OCI_GetServerRevisionVersion(cn));

    print_frmt("Connection      version : %i\n\n", OCI_GetVersionConnection(cn));

    print_text("\n>>>>> SERVER VERSION BANNER \n\n");

    /* print server string version */

    print_mstr(OCI_GetVersionServer(cn));

    print_text("\n\n");

}

/* ------------------------------------------------------------------------ *
 * create_tables
 * ------------------------------------------------------------------------ */

void create_tables(void)
{
    print_text("\n>>>>> CREATE TABLES FOR DEMO \n\n");

    /* create types for the demo */
    OCI_ExecuteStmt(st, MT("create type type_t as OBJECT (id int, name varchar2(50))"));

    OCI_ExecuteStmt(st, MT("create type test_t as object ")
                    MT("( ")
                    MT("    val_int  number, ")
                    MT("    val_flt  float, ")
                    MT("    val_str  varchar2(30), ")
                    MT("    val_date date, ")
                    MT("    val_lob  clob, ")
                    MT("    val_file bfile, ")
                    MT("    val_obj  type_t, ")
                    MT("    val_raw  raw(10) ")
                    MT(")"));

    OCI_ExecuteStmt(st, MT("create type t_tab1_emp as VARRAY(100) of varchar2(50)"));

    OCI_ExecuteStmt(st, MT("create type t_tab2_emp as table of varchar2(50)"));

    /* create table for the demo */
    OCI_ExecuteStmt(st, MT("create table test_fetch(code int, article ")
                        MT("varchar2(30), price float, creation date)"));

    OCI_ExecuteStmt(st, MT("create table test_long_raw(code int, content long raw)"));

    OCI_ExecuteStmt(st, MT("create table test_long_str(code int, content long)"));

    OCI_ExecuteStmt(st, MT("create table test_lob(code int, content clob)"));

    OCI_ExecuteStmt(st, MT("create table test_object(val test_t)"));

    OCI_ExecuteStmt(st, MT("create table test_table_obj of type_t"));

    OCI_ExecuteStmt(st, MT("create table test_array ")
                        MT("( ")
                        MT("    val_int  number, ")
                        MT("    val_flt  float, ")
                        MT("    val_str  varchar2(30), ")
                        MT("    val_date date, ")
                        MT("    val_lob  clob, ")
                        MT("    val_file bfile ")
                        MT(")")
                    );

    OCI_ExecuteStmt(st, MT("create table test_coll_varray ")
                        MT("( ")
                        MT("    departement number, ")
                        MT("    employees   t_tab1_emp ")
                        MT(")")
                    );

    OCI_ExecuteStmt(st, MT("create table test_coll_nested ")
                        MT("( ")
                        MT("    departement number, ")
                        MT("    employees   t_tab2_emp ")
                        MT(") nested table employees store as test_table_emp")
                    );

    OCI_ExecuteStmt(st, MT("create table test_directpath(val_int number(8,4), ")
                        MT(" val_str varchar2(30), val_date date)"));

    /* insert data into the demo tables */
    OCI_ExecuteStmt(st, MT("insert into test_fetch ")
                        MT("(code, article, price, creation) ")
                        MT("values (1, 'shoes', 3.14, to_date('1978-12-23', 'YYYY-MM-DD'))"));

    OCI_ExecuteStmt(st, MT("insert into test_fetch ")
                        MT("(code, article, price, creation) ")
                        MT("values (2, 'shirt', 5.99, to_date('1999-09-12', 'YYYY-MM-DD'))"));

    OCI_ExecuteStmt(st, MT("insert into test_lob(code,content)  ")
                        MT("values (1, EMPTY_CLOB())"));

    OCI_ExecuteStmt(st, MT("insert into test_long_str(code,content) ")
                        MT("values (1, 'Rugby rocks !')"));

    OCI_ExecuteStmt(st, MT("insert into test_coll_varray(departement,employees) ")
                        MT("values (1, t_tab1_emp('Peter', 'John', 'Paula', 'Gina'))"));

    OCI_ExecuteStmt(st, MT("insert into test_coll_varray(departement,employees) ")
                        MT("values (2, t_tab1_emp('Ben', 'Alice', 'Joel', 'Maria'))"));

    OCI_ExecuteStmt(st, MT("insert into test_coll_nested(departement,employees) ")
                        MT("values (1, t_tab2_emp('Vince', 'Richard', 'Rita', 'Sophia'))"));

    OCI_ExecuteStmt(st, MT("insert into test_coll_nested(departement,employees) ")
                        MT("values (2, t_tab2_emp('Paul', 'Sarah', 'Robert', 'Zoe'))"));

    OCI_ExecuteStmt(st, MT("insert into test_table_obj values(type_t(1, 'shoes'))"));
    OCI_ExecuteStmt(st, MT("insert into test_table_obj values(type_t(2, 'pen'))"));

    OCI_Commit(cn);
}

/* ------------------------------------------------------------------------ *
 * drop_tables
 * ------------------------------------------------------------------------ */

void drop_tables(void)
{
    print_text("\n>>>>> DROPPING TABLES AND TYPES \n\n");

    OCI_ExecuteStmt(st, MT("drop table test_fetch"));
    OCI_ExecuteStmt(st, MT("drop table test_long_str"));
    OCI_ExecuteStmt(st, MT("drop table test_long_raw"));
    OCI_ExecuteStmt(st, MT("drop table test_lob"));
    OCI_ExecuteStmt(st, MT("drop table test_array"));
    OCI_ExecuteStmt(st, MT("drop table test_object"));
    OCI_ExecuteStmt(st, MT("drop table test_coll_varray"));
    OCI_ExecuteStmt(st, MT("drop table test_coll_nested"));
    OCI_ExecuteStmt(st, MT("drop table test_table_obj"));
    OCI_ExecuteStmt(st, MT("drop table test_directpath"));

    OCI_ExecuteStmt(st, MT("drop type  test_t"));
    OCI_ExecuteStmt(st, MT("drop type  type_t"));
    OCI_ExecuteStmt(st, MT("drop type  t_tab1_emp"));
    OCI_ExecuteStmt(st, MT("drop type  t_tab2_emp"));

}

/* ------------------------------------------------------------------------ *
 * test_format
 * ------------------------------------------------------------------------ */

void test_format(void)
{
    int code = 1;

    print_text("\n>>>>> TEST FORMATTING \n\n");

    OCI_ExecuteStmtFmt(st, MT("select * from test_fetch where code = %i"), code);

    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
    {
        print_frmt("> code : %i", OCI_GetInt(rs, 1));
        print_text(", action : "); print_dstr(OCI_GetString(rs, 2));
        print_frmt(", price : %g", OCI_GetDouble(rs,3));
        print_text(", date : "); print_dstr(OCI_GetString(rs, 4));
        print_text("\n");
   }

    print_frmt("\n%d row(s) fetched\n", OCI_GetRowCount(rs));
}


/* ------------------------------------------------------------------------ *
 * test_immediate
 * ------------------------------------------------------------------------ */

void test_immediate(void)
{
    int code = 1;

    print_text("\n>>>>> TEST IMMEDIATE \n\n");

    OCI_Immediate(cn, MT("select code, article from test_fetch where code = 1"),
                  OCI_ARG_INT, &code,
                  OCI_ARG_TEXT, temp);

    print_frmt("> code : %i ", code);
    print_text("- article : "); print_dstr(temp);
    print_text("\n");
}

/* ------------------------------------------------------------------------ *
 * test_immediate_format
 * ------------------------------------------------------------------------ */

void test_immediate_format(void)
{
    int code = 1;

    print_text("\n>>>>> TEST IMMEDIATE FORMATTED\n\n");

    OCI_ImmediateFmt(cn, MT("select article from test_fetch where code = %i"),
                     code, OCI_ARG_TEXT, temp);

    print_text("> article : "); print_dstr(temp);  print_text("\n");
}

/* ------------------------------------------------------------------------ *
 * test_fetch
 * ------------------------------------------------------------------------ */

void test_fetch(void)
{
    int i, n;

    print_text("\n>>>>> SIMPLE TEST FETCH WITH META DATA\n\n");

    /* execute query in one go */
    OCI_ExecuteStmt(st, MT("select * from test_fetch"));

    rs = OCI_GetResultset(st);
    n  = OCI_GetColumnCount(rs);

    /* print resultset columns info */
    for(i = 1; i <= n; i++)
    {
        OCI_Column *col = OCI_GetColumn(rs, i);

        print_frmt("> Field : #%i ", i);
        print_text("- Name  : "); print_mstr(OCI_ColumnGetName(col));
        print_text("\n");
    }

    print_text("\n");

    /* print resultset content */
    while (OCI_FetchNext(rs))
    {
        print_frmt("> code : %i", OCI_GetInt(rs, 1));
        print_text(", action : "); print_dstr(OCI_GetString(rs, 2));
        print_frmt(", price : %g", OCI_GetDouble(rs,3));
        print_text(", date : "); print_dstr(OCI_GetString(rs, 4));
        print_text("\n");
    }

    print_frmt("\n%d row(s) fetched\n", OCI_GetRowCount(rs));
}

/* ------------------------------------------------------------------------ *
 * test_bind1
 * ------------------------------------------------------------------------ */

void test_bind1(void)
{
    int code = 1;

    print_text("\n>>>>> TEST BINDING \n\n");

    /* execute query in three steps */
    OCI_Prepare(st, MT("select * from test_fetch where code = :code"));
    OCI_BindInt(st, MT(":code"), &code);
    OCI_Execute(st);

    rs = OCI_GetResultset(st);

    /* print resultset content */
    while (OCI_FetchNext(rs))
    {
        print_frmt("> code : %i", OCI_GetInt(rs, 1));
        print_text(", action : "); print_dstr(OCI_GetString(rs, 2));
        print_frmt(", price : %g", OCI_GetDouble(rs,3));
        print_text(", date : "); print_dstr(OCI_GetString(rs, 4));
        print_text("\n");
    }

    print_frmt("\n%d row(s) fetched\n", OCI_GetRowCount(rs));
}

/* ------------------------------------------------------------------------ *
 * test_bind2
 * ------------------------------------------------------------------------ */

void test_bind2(void)
{
    OCI_Lob *lob;
    OCI_Date *date;
    OCI_File *file;

    int i;
    double flt;

    print_text("\n>>>>> SINGLE BINDING \n\n");

   /* prepare SQL */

    OCI_Prepare(st, MT("insert into test_array ")
                    MT("( ")
                    MT("   val_int,  val_flt, val_str, val_date, ")
                    MT( "   val_lob, val_file ")
                    MT( ") " )
                    MT( "values ")
                    MT( "( ")
                    MT( "   :val_int, :val_flt, :val_str, :val_date, ")
                    MT( "   :val_lob, :val_file ")
                    MT(") "));

    /* create objects and set values */

    /* date */
    date = OCI_DateCreate(cn);
    OCI_DateSysDate(date);

    /* lob */
    lob = OCI_LobCreate(cn, OCI_CLOB);
    sprint_dt(temp, SIZE_STR, DT("lob value00"));
    OCI_LobWrite(lob, temp, (unsigned int) dtslen(temp));

    /* file */
    file = OCI_FileCreate(cn, OCI_BFILE);
    OCI_FileSetName(file, MT("mydir"), MT("file00.txt"));

    /* scalar types */
    i   = 1;
    flt = 3.14;
    sprint_dt(temp, 30, DT("Name00"));

    /* bind scalar C types arrays */

    OCI_BindInt(st, MT(":val_int"),  &i);
    OCI_BindDouble(st, MT(":val_flt"), &flt);
    OCI_BindString(st, MT(":val_str"), (dtext*) temp, 30);

    /* bind oracle types arrays */

    OCI_BindDate(st, MT(":val_date"), date);
    OCI_BindLob(st, MT(":val_lob"), lob);
    OCI_BindFile(st, MT(":val_file"), file);

    /* do insert */

    OCI_Execute(st);

    print_frmt("Row inserted : %d\n\n", OCI_GetAffectedRows(st));

    /* free objets */

    OCI_DateFree(date);
    OCI_LobFree(lob);
    OCI_FileFree(file);

    /* commit; */

    OCI_Commit(cn);
}

/* ------------------------------------------------------------------------ *
 * test_piecewise_insert
 * ------------------------------------------------------------------------ */

void test_piecewise_insert(void)
{
    FILE *f;

    print_text("\n>>>>> TEST PIECEWISE INSERTING\n\n");

    /* this code could have been used with a text file in a LONG column
       as well...  */

    /* open the app file in for reading*/

    f = fopen(EXE_NAME, "rb");

    if (f)
    {
        int n;
        OCI_Long *lg;
        unsigned char buffer[SIZE_BUF];

        fseek (f , 0 , SEEK_END);
        n = ftell(f);
        rewind (f);

        print_frmt("\n%d bytes to write\n", n);

        lg = OCI_LongCreate(st, OCI_BLONG);

        /* execute query in three steps */
        OCI_Prepare(st,  MT("insert into test_long_raw(code, content) ")
                         MT("values (1, :data)"));

        OCI_BindLong(st, MT(":data"), lg, n);
        OCI_Execute(st);

        /* write data into table by chunks of 2048 bytes */
        while ((n = (int) fread(buffer, 1, sizeof(buffer), f)))
        {
            OCI_LongWrite(lg, buffer, n);
        }

        print_frmt("\n%d bytes written\n", OCI_LongGetSize(lg));
        fclose(f);

        OCI_LongFree(lg);
        OCI_Commit(cn);
    }
}

/* ------------------------------------------------------------------------ *
 * test_piecewise_fetch
 * ------------------------------------------------------------------------ */

void test_piecewise_fetch(void)
{
    OCI_Long *lg;
    unsigned char buffer[SIZE_BUF];
    int n;

    print_text("\n>>>>> TEST PIECEWISE FETCHING\n\n");

    /* execute query in one go */
    OCI_ExecuteStmt(st, MT("select content from test_long_raw ")
                              MT("where code = 1"));

    rs = OCI_GetResultset(st);

    /* read data by chunks of 2048 bytes*/
    while (OCI_FetchNext(rs))
    {
        lg = OCI_GetLong(rs, 1);

        while ((n = OCI_LongRead(lg, buffer, sizeof(buffer)))) {}

        print_frmt("\n%d bytes read\n", OCI_LongGetSize(lg));
    }

    print_frmt("\n%d row(s) fetched\n", OCI_GetRowCount(rs));

    print_text("\n>>>>> TEST LONG MAPPED TO STRING\n\n");

    OCI_ExecuteStmt(st, MT("select content from test_long_str ")
                        MT("where code = 1"));
    OCI_SetLongMode(st, OCI_LONG_IMPLICIT);
    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
    {
        const dtext *long_str = OCI_GetString(rs, 1);

        print_dstr(long_str);
        print_frmt("\n%d bytes read\n", dtslen(long_str));
    }

    print_frmt("\n%d row(s) fetched\n", OCI_GetRowCount(rs));
}

/* ------------------------------------------------------------------------ *
 * test_lob
 * ------------------------------------------------------------------------ */

void test_lob(void)
{
    OCI_Lob *lob;

    print_text("\n>>>>> TEST LOB MANIPULATION\n\n");

    OCI_ExecuteStmt(st, MT("select code, content from test_lob ")
                        MT("where code=1 for update"));

    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
    {
        lob  = OCI_GetLob(rs, 2);

        OCI_LobWrite(lob, DT("today, "), 7);
        OCI_LobAppend(lob, DT("i'm going to the cinema ! "), 26);
        OCI_LobSeek(lob, 0, OCI_SEEK_SET);

        temp[OCI_LobRead(lob, temp, SIZE_STR)] = 0;

        print_frmt("> code    : %i", OCI_GetInt(rs, 1));
        print_text(", content : "); print_dstr(temp);
        print_text("\n");
    }

    OCI_Commit(cn);

    print_frmt("\n%d row(s) fetched\n", OCI_GetRowCount(rs));
}

/* ------------------------------------------------------------------------ *
 * test_nested_table
 * ------------------------------------------------------------------------ */

void test_nested_table(void)
{
    print_text("\n>>>>> TEST NESTED TABLE \n\n");

    OCI_ExecuteStmt(st, MT("select article, cursor(select sysdate ")
                        MT("from dual) from test_fetch"));

    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
    {
        OCI_Statement *st2 = OCI_GetStatement(rs, 2);
        OCI_Resultset *rs2 = OCI_GetResultset(st2);

        while (OCI_FetchNext(rs2))
        {
            print_text("> article : "); print_dstr(OCI_GetString(rs, 1));
            print_text(", date : ");    print_dstr(OCI_GetString(rs2, 1));
            print_text("\n");
        }
    }
}

/* ------------------------------------------------------------------------ *
 * test_ref_cursor
 * ------------------------------------------------------------------------ */

void test_ref_cursor(void)
{
    OCI_Statement* st2 = OCI_StatementCreate(cn);

    print_text("\n>>>>> TEST REF CURSOR \n\n");

    OCI_Prepare(st, MT("begin open :c for select * from test_fetch; end;"));
    OCI_BindStatement(st, MT(":c"), st2);
    OCI_Execute(st);

    rs = OCI_GetResultset(st2);

    /* print resultset content */
    while (OCI_FetchNext(rs))
    {
        print_frmt("> code : %i", OCI_GetInt(rs, 1));
        print_text(", action : "); print_dstr(OCI_GetString(rs, 2));
        print_frmt(", price : %g", OCI_GetDouble(rs,3));
        print_text(", date : "); print_dstr(OCI_GetString(rs, 4));
        print_text("\n");
    }

    print_frmt("\n%d row(s) fetched\n", OCI_GetRowCount(rs));

    OCI_StatementFree(st2);
}

/* ------------------------------------------------------------------------ *
 * test_plsql
 * ------------------------------------------------------------------------ */

void test_plsql(void)
{
    int res = 0;
    const dtext *p=NULL;

    print_text("\n>>>>> TEST PL/SQL OUTPUT BIND\n\n");

    OCI_Prepare(st, MT("begin :res := trunc(sysdate+1)-trunc(sysdate-1); end;"));
    OCI_BindInt(st, MT(":res"), &res);
    OCI_Execute(st);

    print_text("\nPL/SQL : trunc(sysdate+1)-trunc(sysdate-1)\n");
    print_frmt("\nResult : %i\n", res);

    print_text("\n>>>>> TEST PL/SQL SERVER OUTPUT\n\n");

    /* server output */

    OCI_ServerEnableOutput(cn, 32000, 5, 255);

    OCI_ExecuteStmt(st, MT("begin ")
                        MT("   dbms_output.put_line('First  line'); ")
                        MT("   dbms_output.put_line('Second line'); ")
                        MT("   dbms_output.put_line('Third  line'); ")
                        MT("end;")
                   );

    while ((p = OCI_ServerGetOutput(cn)))
    {
        print_dstr(p);
        print_text("\n");
    }

    OCI_ServerDisableOutput(cn);
}

/* ------------------------------------------------------------------------ *
 * test_dates
 * ------------------------------------------------------------------------ */

void test_dates(void)
{
    OCI_Date *date, *date2;

    print_text("\n>>>>> TEST DATETIME MANIPULATION\n\n");

    date  = OCI_DateCreate(NULL);
    date2 = OCI_DateCreate(NULL);

    OCI_DateFromText(date, MT("1978-04-13 20:20:12"), MT("YYYY-MM-DD HH24:MI:SS"));
    OCI_DateToText(date, MT("YYYY-MM-DD HH24:MI:SS"), SIZE_STR, str);
    print_text("\nDate                     : "); print_mstr(str);

    OCI_DateSysDate(date);
    OCI_DateToText(date, MT("YYYY-MM-DD HH24:MI:SS"), SIZE_STR, str);
    print_text("\nSysdate                  : "); print_mstr(str);

    OCI_DateAddDays(date, 5);
    OCI_DateAddMonths(date, 2);
    OCI_DateToText(date, MT("YYYY-MM-DD HH24:MI:SS"), SIZE_STR, str);
    print_text("\nDate + 5 days + 2 months : "); print_mstr(str);

    OCI_DateAssign(date2, date);
    OCI_DateLastDay(date);
    OCI_DateToText(date, MT("YYYY-MM-DD"), SIZE_STR, str);
    print_text("\nLast day of the month    : "); print_mstr(str);

    print_frmt("\nEnd of the month - date  : %i days",
               OCI_DateDaysBetween(date, date2));
    print_text("\n");

    OCI_DateFree(date);
    OCI_DateFree(date2);
}

/* ------------------------------------------------------------------------ *
 * test_timestamp
 * ------------------------------------------------------------------------ */

void test_timestamp(void)
{
#ifndef OCI_CHARSET_ANSI

    unsigned int version = OCI_GetOCIRuntimeVersion();

    /* Oracle 9i has some troubles with formatting Intervals/timestamps in
       an UTF16 context... */

    if ((version >= OCI_9_0) || (version < OCI_10_1))
        return;
#endif

    if (OCI_GetOCIRuntimeVersion() >= OCI_9_0)
    {
        OCI_Timestamp *tmsp;

        print_text("\n>>>>> TEST TIMESTAMP\n\n");

        tmsp = OCI_TimestampCreate(NULL, OCI_TIMESTAMP);

        OCI_TimestampSysTimeStamp(tmsp);

        OCI_TimestampToText(tmsp, MT("YYYY-MM-DD HH24:MI:SS:FF\n"), SIZE_STR, str, 3);


        print_text("Current timestamp            : "); print_mstr(str);

        /* intervals raw oci functions have some troubles with Oracle 9i. So let's
           use it for the demo only if we're using 10g or above */

        if (OCI_GetOCIRuntimeVersion() >= OCI_10_1)
        {
            OCI_Interval *itv;

            print_text("\n>>>>> TEST INTERVAL \n\n");

            itv = OCI_IntervalCreate(NULL, OCI_INTERVAL_DS);

            OCI_IntervalSetDaySecond(itv, 1,1,1,1,0);

            OCI_IntervalToText(itv, 3, 3, SIZE_STR, str);

            print_text("Interval                     : "); print_mstr(str);
            print_text("\n");

            OCI_TimestampIntervalAdd(tmsp, itv);

            OCI_TimestampToText(tmsp, MT("YYYY-MM-DD HH24:MI:SS:FF\n"), SIZE_STR, str, 3);

            print_text("Current timestamp + Interval : "); print_mstr(str);

            OCI_IntervalFree(itv);
        }

        OCI_TimestampFree(tmsp);
    }
}

/* ------------------------------------------------------------------------ *
 * test_describe
 * ------------------------------------------------------------------------ */

void test_describe(void)
{
    OCI_Column   *col;
    OCI_TypeInfo *tbl;

    int i, n;

    print_text("\n>>>>> TEST DESCRIBING TABLE \n\n");

    tbl = OCI_TypeInfoGet(cn, MT("test_fetch"), OCI_TIF_TABLE);

    if (tbl)
    {
        print_text("Column Name         NULL ?  Type                        \n");
        print_text("--------------------------------------------------------\n");

        n = OCI_TypeInfoGetColumnCount(tbl);

        for(i = 1; i <= n; i++)
        {
            col = OCI_TypeInfoGetColumn(tbl, i);

            OCI_ColumnGetFullSQLType(col, str, SIZE_STR);

#if defined(OCI_CHARSET_WIDE)
  #if !defined(_WINDOWS)
            printf("%-20ls%-8ls%-30ls\n",
  #else
            wprintf(MT("%-20s%-8s%-30s\n"),
  #endif
#else
            printf("%-20s%-8s%-30s\n",
#endif
                    OCI_ColumnGetName(col),
                    OCI_ColumnGetNullable(col) == TRUE ? MT("Y") : MT("N"),
                    str);
        }
    }

    OCI_TypeInfoFree(tbl);

    /* TEST DESCRIBING TYPE ------------------------------------------------- */

    print_text("\n>>>>> TEST DESCRIBING TYPE \n\n");

    tbl = OCI_TypeInfoGet(cn, MT("test_t"), OCI_TIF_TYPE);

    if (tbl)
    {
        print_text("Column Name         Type                           \n");
        print_text("---------------------------------------------------\n");

        n = OCI_TypeInfoGetColumnCount(tbl);

        for(i = 1; i <= n; i++)
        {
            col = OCI_TypeInfoGetColumn(tbl, i);

            OCI_ColumnGetFullSQLType(col, str, SIZE_STR);

#if defined(OCI_CHARSET_WIDE)
  #if !defined(_WINDOWS)
            printf("%-20ls%-30ls\n",
  #else
            wprintf(MT("%-20s%-30s\n"),
  #endif
#else
            printf("%-20s%-30s\n",
#endif
                    OCI_ColumnGetName(col),  str);
        }
    }

    OCI_TypeInfoFree(tbl);
}

/* ------------------------------------------------------------------------ *
 * test_returning
 * ------------------------------------------------------------------------ */

void test_returning(void)
{
    OCI_Lob *lob;

    print_text("\n>>>>> TEST RETURNING CLAUSE \n\n");

    OCI_Prepare(st, MT("update test_lob set code = code + 1 returning code, content into :i, :l"));
    OCI_RegisterLob(st, MT(":l"), OCI_CLOB);
    OCI_RegisterInt(st, MT(":i"));
    OCI_Execute(st);

    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
    {
        lob = OCI_GetLob(rs, 2);

        OCI_LobAppend(lob, DT("(modified)"), 10);
        OCI_LobSeek(lob, 0, OCI_SEEK_SET);

        temp[OCI_LobRead(lob, temp, SIZE_STR)] = 0;

        print_frmt("> code : %i - ", OCI_GetInt(rs, 1));
        print_dstr(temp);
        print_text("\n");
   }

    print_frmt("\n%d row(s) fetched\n", OCI_GetRowCount(rs));

    OCI_Commit(cn);
}

/* ------------------------------------------------------------------------ *
 * test_returning_array
 * ------------------------------------------------------------------------ */

void test_returning_array(void)
{
    OCI_Lob *lob;
    OCI_Date *date;
    OCI_File *file;
    int i;

    /* arrays */
    int       tab_int [SIZE_TAB];
    dtext     tab_str [SIZE_TAB][31];
    double    tab_flt [SIZE_TAB];

    OCI_Date  **tab_date = OCI_DateArrayCreate(cn, SIZE_TAB);
    OCI_Lob   **tab_lob  = OCI_LobArrayCreate(cn,  OCI_CLOB, SIZE_TAB);
    OCI_File  **tab_file = OCI_FileArrayCreate(cn, OCI_BFILE, SIZE_TAB);

    print_text("\n>>>>> TEST ARRAY BINDING WITH RETURNING CLAUSE \n\n");

    /* prepare SQL */

    OCI_Prepare(st, MT("insert into test_array ")
                    MT("( ")
                    MT("   val_int,  val_flt, val_str, val_date, ")
                    MT( "   val_lob, val_file ")
                    MT( ") " )
                    MT( "values ")
                    MT( "( ")
                    MT( "   :val_int, :val_flt, :val_str, :val_date, ")
                    MT( "   :val_lob, :val_file ")
                    MT(") ")
                    MT("returning")
                    MT( "  val_int,  val_flt, val_str, val_date, ")
                    MT("   val_lob, val_file ")
                    MT("into  " )
                    MT( "  :out_int, :out_flt,  :out_str, :out_date, ")
                    MT("   :out_lob, :out_file "));

    /* Set Array Size */

    OCI_BindArraySetSize(st, SIZE_TAB);

    /* bind scalar C types arrays */

    OCI_BindArrayOfInts(st, MT(":val_int"),  tab_int, 0);
    OCI_BindArrayOfDoubles(st, MT(":val_flt"), tab_flt, 0);
    OCI_BindArrayOfStrings(st, MT(":val_str"), (dtext*) tab_str, 30, 0);

    /* bind oracle types arrays */

    OCI_BindArrayOfDates(st, MT(":val_date"), tab_date, 0);
    OCI_BindArrayOfLobs(st, MT(":val_lob"), tab_lob, OCI_CLOB, 0);
    OCI_BindArrayOfFiles(st, MT(":val_file"), tab_file, OCI_BFILE, 0);

    /* register C scalar types output */

    OCI_RegisterInt(st, MT(":out_int"));
    OCI_RegisterDouble(st, MT(":out_flt"));
    OCI_RegisterString(st, MT(":out_str"), 30);

    /* bind oracle types outputs */

    OCI_RegisterDate(st, MT(":out_date"));
    OCI_RegisterLob(st, MT(":out_lob"), OCI_CLOB);
    OCI_RegisterFile(st, MT(":out_file"), OCI_BFILE);

    /* create objects and set values */

    for(i = 0; i < SIZE_TAB; i++)
    {
        /* scalar types */
        tab_int[i] = i+1;
        tab_flt[i] = 3.14*(double)(i+1);
        sprint_dt(tab_str[i], 30, DT("Name%02i"), i+1);

        /* date */
        OCI_DateSysDate(tab_date[i]);

        /* lob */
        sprint_dt(temp, SIZE_STR, DT("lob value%02i"), i+1);
        OCI_LobWrite(tab_lob[i], temp, (unsigned int) dtslen(temp));

        /* file */
        sprint_mt(str, SIZE_STR, MT("file%02i.txt"), i+1);
        OCI_FileSetName(tab_file[i], MT("mydir"), str);
    }

    OCI_Execute(st);

    print_frmt("Row inserted : %d\n\n", OCI_GetAffectedRows(st));

    /* free objects */

    OCI_DateArrayFree(tab_date);
    OCI_LobArrayFree(tab_lob);
    OCI_FileArrayFree(tab_file);

    /* Get back data  from the returning clause
       Obviously, each entry of the array will return a resultset containing
       1 row only (DML insert)
    */

    rs = OCI_GetResultset(st);

    i = 1;

    while (rs != NULL)
    {
        while (OCI_FetchNext(rs))
        {
            print_frmt("Row # %d-------------------\n", i);

            print_frmt(".... val_int    : %i\n", OCI_GetInt2(rs, MT(":OUT_INT")));
            print_frmt(".... val_flt    : %g\n", OCI_GetDouble2(rs, MT(":OUT_FLT")));
            print_text(".... val_str    : "); print_dstr(OCI_GetString2(rs, MT(":OUT_STR")));
            print_text("\n");

            date = OCI_GetDate(rs, 4);
            OCI_DateToText(date, MT("YYYY-MM-DD HH24:MI:SS"), SIZE_STR, str);
            print_text(".... val_date   : "); print_mstr(str);
            print_text("\n");

            lob  = OCI_GetLob(rs, 5);
            temp[OCI_LobRead(lob, temp, 100)]=0;
            print_text(".... val_lob    : "); print_dstr(temp);
            print_text("\n");

            file = OCI_GetFile(rs, 6);
            print_text(".... val_file   : ");
            print_mstr(OCI_FileGetDirectory(file));
            print_text("/");
            print_mstr(OCI_FileGetName(file));
            print_text("\n");

            print_text("\n");
            i++;
        }

        rs = OCI_GetNextResultset(st);
    }
}

/* ------------------------------------------------------------------------ *
 * test_object_insert
 * ------------------------------------------------------------------------ */

void test_object_insert(void)
{
    OCI_Date *date;
    OCI_File *file;
    OCI_Lob  *lob;
    OCI_Object *obj;
    OCI_Object *obj2;

    char rawbuf[11];

    strcpy(rawbuf, "0123456789");

    print_text("\n>>>>> TEST OBJECT BINDING \n\n");

    /* create types for the demo */

    obj  = OCI_ObjectCreate(cn, OCI_TypeInfoGet(cn, MT("test_t"), OCI_TIF_TYPE));

    OCI_ObjectSetInt(obj, MT("VAL_INT"), 1);
    OCI_ObjectSetDouble(obj, MT("VAL_FLT"), 3.14);
    OCI_ObjectSetString(obj, MT("VAL_STR"), DT("USB KEY 2go"));
    OCI_ObjectSetRaw(obj, MT("VAL_RAW"), rawbuf, 10);

    date = OCI_DateCreate(cn);
    OCI_DateSysDate(date);
    OCI_ObjectSetDate(obj, MT("VAL_DATE"), date);

    obj2 = OCI_ObjectCreate(cn, OCI_TypeInfoGet(cn, MT("TYPE_T"), OCI_TIF_TYPE));
    OCI_ObjectSetInt(obj2, MT("ID"), 1);

    OCI_ObjectSetString(obj2, MT("NAME"), DT("USB KEY 2go"));
    OCI_ObjectSetObject(obj, MT("VAL_OBJ"), obj2);

    lob = OCI_LobCreate(cn, OCI_CLOB);
    OCI_ObjectSetLob(obj, MT("VAL_LOB"), lob);

    file = OCI_FileCreate(cn, OCI_BFILE);
    OCI_FileSetName(file, MT("mydir"), MT("myfile"));
    OCI_ObjectSetFile(obj, MT("VAL_FILE"), file);

    OCI_Prepare(st, MT("insert into test_object values(:obj)"));
    OCI_BindObject(st, MT(":obj"), obj);
    OCI_Execute(st);

    print_frmt("\n%d row(s) inserted\n", OCI_GetAffectedRows(st));

    OCI_Commit(cn);

    OCI_DateFree(date);
    OCI_LobFree(lob);
    OCI_FileFree(file);
    OCI_ObjectFree(obj2);
    OCI_ObjectFree(obj);
}

/* ------------------------------------------------------------------------ *
 * test_object_fetch
 * ------------------------------------------------------------------------ */

void test_object_fetch(void)
{
    OCI_Lob *lob;
    OCI_Date *date;
    OCI_File *file;
    OCI_Object *obj;
    OCI_Object *obj2;
    char rawbuf[11] = "";

    print_text("\n>>>>> TEST OBJECT FETCHING \n\n");

    OCI_ExecuteStmt(st, MT("select val from test_object for update"));

    rs = OCI_GetResultset(st);

    /* print resultset content */
    while (OCI_FetchNext(rs))
    {
        obj  = OCI_GetObject(rs, 1);

        print_frmt("val_int        : %i\n",  OCI_ObjectGetInt(obj, MT("VAL_INT")));
        print_frmt("val_flt        : %g\n",  OCI_ObjectGetDouble(obj, MT("VAL_FLT")));
        print_text("val_str        : "); print_dstr(OCI_ObjectGetString(obj, MT("VAL_STR")));
        print_text("\n");

        temp[OCI_ObjectGetRaw(obj, MT("VAL_RAW"), rawbuf, 10)] = 0;
        print_text("val_raw        : "); printf(rawbuf);
        print_text("\n");

        date = OCI_ObjectGetDate(obj, MT("VAL_DATE"));
        OCI_DateToText(date, MT("YYYY-MM-DD HH24:MI:SS"), SIZE_STR, str);
        print_text("val_date       : "); print_mstr(str);
        print_text("\n");

        lob = OCI_ObjectGetLob(obj, MT("VAL_LOB"));

        OCI_LobWrite(lob, DT("Hello from Mars !"), 17);
        OCI_LobSeek(lob, 0, OCI_SEEK_SET);

        temp[OCI_LobRead(lob, temp, SIZE_STR)]=0;

        print_text("val_lob        : "); print_dstr(temp);
        print_text("\n");

        file = OCI_ObjectGetFile(obj, MT("VAL_FILE"));
        print_text("val_file.dir   : "); print_mstr(OCI_FileGetDirectory(file));
        print_text("\n");
        print_text("val_file.name  : "); print_mstr(OCI_FileGetName(file));
        print_text("\n");

        obj2 = OCI_ObjectGetObject(obj, MT("VAL_OBJ"));
        print_frmt("val_obj.code   : %i\n", OCI_ObjectGetInt(obj2, MT("ID")));
        print_text("val_obj.name   : "); print_dstr(OCI_ObjectGetString(obj2, MT("NAME")));
        print_text("\n");

        printf("\n");;

    }

    OCI_Commit(cn);
}

/* ------------------------------------------------------------------------ *
 * test_scrollable_cursor
 * ------------------------------------------------------------------------ */

void test_scrollable_cursor(void)
{
    if (OCI_GetVersionConnection(cn) >= OCI_9_0)
    {
        print_text("\n>>>>> TEST SCROLLABLE CURSORS \n\n");

        OCI_SetFetchMode(st, OCI_SFM_SCROLLABLE);

        OCI_ExecuteStmt(st, MT("select table_name from user_tables where ")
                            MT("table_name like 'TEST_%' order by table_name"));

        rs = OCI_GetResultset(st);

        if (OCI_FetchLast(rs))
            print_frmt("... Resultset contains %i rows\n\n", OCI_GetRowCount(rs));

        print_text("... Go to row 1\n");
        if (OCI_FetchFirst(rs))
        {
            print_dstr(OCI_GetString(rs, 1));
            print_text("\n");
        }

        print_frmt("... Enumerate from row 2 to row %i\n", OCI_GetRowCount(rs));
        while (OCI_FetchNext(rs))
        {
            print_dstr(OCI_GetString(rs, 1));
            print_text("\n");
        }

        print_frmt("... Enumerate from row %i back to row 1\n", OCI_GetRowCount(rs)-1);
        while (OCI_FetchPrev(rs))
        {
            print_dstr(OCI_GetString(rs, 1));
            print_text("\n");
        }

        print_text("... Go to the 3th row\n");
        if (OCI_FetchSeek(rs, OCI_SFD_ABSOLUTE,  3))
        {
            print_dstr(OCI_GetString(rs, 1));
            print_text("\n");
        }

        print_text("... Fetch the next 2 rows\n");
        while ((OCI_GetCurrentRow(rs) < 5) && OCI_FetchNext(rs))
        {
            print_dstr(OCI_GetString(rs, 1));
            print_text("\n");
        }

        OCI_SetFetchMode(st, OCI_SFM_DEFAULT);
    }
}

/* ------------------------------------------------------------------------ *
 * test_collection
 * ------------------------------------------------------------------------ */

void test_collection(void)
{
    OCI_TypeInfo *type;
    OCI_Coll     *coll;
    OCI_Iter     *iter;
    OCI_Elem     *elem;
    int i, n;

    print_text("\n>>>>> TEST VARRAY BINDING \n\n");

    type = OCI_TypeInfoGet(cn, MT("T_TAB1_EMP"), OCI_TIF_TYPE);
    coll = OCI_CollCreate(type);

    OCI_Prepare(st, MT("begin")
                    MT("    select employees into :tab_emp ")
                    MT("    from test_coll_varray ")
                    MT("    where departement = :id; ")
                    MT("end;"));

    OCI_BindColl(st, MT(":tab_emp"), coll);
    OCI_BindInt(st, MT(":id"), &i);

    i = 1;

    OCI_Execute(st);

    iter = OCI_IterCreate(coll);
    elem = OCI_IterGetNext(iter);

    print_frmt("Department ID #%d\n\n", i);

    while (elem != NULL)
    {
        print_text("... Employee : ");
        print_dstr(OCI_ElemGetString(elem));
        print_text("\n");

        elem = OCI_IterGetNext(iter);
    }

    OCI_IterFree(iter);
    OCI_CollFree(coll);

    print_text("\n>>>>> TEST VARRAY FETCHING WITH ITERATOR \n\n");

    OCI_ExecuteStmt(st, MT("SELECT * from test_coll_varray"));

    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
    {
        coll = OCI_GetColl(rs, 2);

        iter = OCI_IterCreate(coll);
        elem = OCI_IterGetNext(iter);

        print_frmt("\nDepartment ID #%d\n\n", OCI_GetInt(rs, 1));

        while (elem != NULL)
        {
            print_text("... Employee : ");
            print_dstr(OCI_ElemGetString(elem));
            print_text("\n");

            elem = OCI_IterGetNext(iter);
        }

        OCI_IterFree(iter);
    }


    print_text("\n>>>>> TEST NESTED TABLE FETCHING WITH INDEX ACCESS \n\n");

    OCI_ExecuteStmt(st, MT("SELECT * from test_coll_nested"));

    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
    {
        coll = OCI_GetColl(rs, 2);

        print_frmt("\nDepartment ID #%d\n\n", OCI_GetInt(rs, 1));

        n = OCI_CollGetSize(coll);

        for(i = 1; i <= n; i++)
        {
            elem = OCI_CollGetAt(coll, i);

            print_text("... Employee : ");
            print_dstr(OCI_ElemGetString(elem));
            print_text("\n");

        }
    }
}

/* ------------------------------------------------------------------------ *
 * test_ref
 * ------------------------------------------------------------------------ */

void test_ref(void)
{
    OCI_Ref    *ref;
    OCI_Object *obj;

    print_text("\n>>>>> TEST REF FETCHING \n\n");

    OCI_ExecuteStmt(st, MT("select ref(e) from test_table_obj e"));
    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
    {
        ref = OCI_GetRef(rs, 1);

        obj = OCI_RefGetObject(ref);

        print_frmt("%i - ", OCI_ObjectGetInt(obj, MT("ID")));
        print_dstr(OCI_ObjectGetString(obj, MT("NAME")));
        print_text("\n");
    }

    print_text("\n>>>>> TEST REF PL/SQL BINDING \n\n");

    ref = OCI_RefCreate(cn, OCI_TypeInfoGet(cn, MT("type_t"), OCI_TIF_TYPE));

    OCI_Prepare(st, MT("begin ")
                    MT("  select ref(e) into :r from test_table_obj e where e.id = 1; ")
                    MT("end; "));

    OCI_BindRef(st, MT(":r"), ref);
    OCI_Execute(st);

    obj = OCI_RefGetObject(ref);
    print_frmt("%i - ", OCI_ObjectGetInt(obj, MT("ID")));
    print_dstr(OCI_ObjectGetString(obj, MT("NAME")));
    print_text("\n");

    OCI_RefFree(ref);
}

/* ------------------------------------------------------------------------ *
 * test_directpath
 * ------------------------------------------------------------------------ */

void test_directpath(void)
{
   /* Some OCI Direct path function fails (segfault) if the OCI version of the
      client is different than the server one.
      It happens with OCI 8i and 9i client. Apparently Oracle 10g and 11g seems to
      have fixed these problems.
      Anyway, we run this test case only if the major versions of client and server
      match
   */

   if (OCI_VER_MAJ(OCI_GetOCIRuntimeVersion()) == OCI_GetServerMajorVersion(cn))
   {
        OCI_DirPath *dp;
        OCI_TypeInfo *tbl;
        boolean res = TRUE;

        dtext val1[SIZE_COL1+1];
        dtext val2[SIZE_COL2+1];
        dtext val3[SIZE_COL3+1];

        int i = 0, j = 0, nb_rows = SIZE_ARRAY;
        int state;

       /* commit any previous pending modifications */

        OCI_Commit(cn);

        print_text("\n>>>>> TEST DIRECT PATH (10 loads of 100 rows) \n\n");

        tbl = OCI_TypeInfoGet(cn, MT("test_directpath"), OCI_TIF_TABLE);
        dp  = OCI_DirPathCreate(tbl, NULL, NUM_COLS, nb_rows);

        /* optional attributes to set */

        res = res && OCI_DirPathSetBufferSize(dp, 64000);
        res = res && OCI_DirPathSetNoLog(dp, TRUE);
        res = res && OCI_DirPathSetParallel(dp, TRUE);

        if(OCI_GetOCIRuntimeVersion() >= OCI_9_2)
        {
            res = res && OCI_DirPathSetCacheSize(dp, 100);
        }

        /* describe the target table */

        res = res && OCI_DirPathSetColumn(dp, 1, MT("VAL_INT"),  SIZE_COL1, NULL);
        res = res && OCI_DirPathSetColumn(dp, 2, MT("VAL_STR"),  SIZE_COL2, NULL);
        res = res && OCI_DirPathSetColumn(dp, 3, MT("VAL_DATE"), SIZE_COL3, MT("YYYYMMDD"));

        /* prepare the load */

        res = res && OCI_DirPathPrepare(dp);

        if (res)
        {
            nb_rows = OCI_DirPathGetMaxRows(dp);          
            
            for (i = 0; i < NB_LOAD ; i++)
            {
                OCI_DirPathReset(dp);

                for (j = 1; j <= nb_rows && res; j++)
                {
                    /* fill test values */

                    sprint_dt(val1, SIZE_COL1+1, DT("%04d"), i + (i*100));
                    sprint_dt(val2, SIZE_COL2+1, DT("value %05d"), j + (i*100));
                    sprint_dt(val3, SIZE_COL3+1, DT("%04d%02d%02d"), (j%23)+1 + 2000,
                                                                     (j%11)+1,
                                                                     (j%23)+1);

                    res = res && OCI_DirPathSetEntry(dp, j, 1, val1, (unsigned int) dtslen(val1), TRUE);
                    res = res && OCI_DirPathSetEntry(dp, j, 2, val2, (unsigned int) dtslen(val2), TRUE);
                    res = res && OCI_DirPathSetEntry(dp, j, 3, val3, (unsigned int) dtslen(val3), TRUE);
                }

               /* load data to the server */

                while (res == TRUE)
                {
                    state = OCI_DirPathConvert(dp);

                    if ((state == OCI_DPR_FULL) || (state == OCI_DPR_COMPLETE))
                        res = OCI_DirPathLoad(dp);

                    if (state == OCI_DPR_COMPLETE)
                        break;
                }
            }

            /* commits changes */

            res = res && OCI_DirPathFinish(dp);

            if (res)
            {
                print_frmt("%04d row(s) loaded\n", OCI_DirPathGetRowCount(dp));
            }

            /* free direct path object */
        }

        OCI_DirPathFree(dp);
   }
}

