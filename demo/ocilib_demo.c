/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@gmail.com>
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

#include "ocilib_demo.h"

/* --------------------------------------------------------------------------------------------- *
 * prototypes
 * --------------------------------------------------------------------------------------------- */

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
void test_object_fetch_string(void);
void test_scrollable_cursor(void);
void test_collection(void);
void test_ref(void);
void test_directpath(void);
void test_bigint(void);
void test_number(void);


/* ocilib test functions array */

test_t tab_test[] =
{
        {test_format,            	TRUE},
        {test_immediate,         	TRUE},
        {test_immediate_format,  	TRUE},
        {test_fetch,             	TRUE},
        {test_bind1,             	TRUE},
        {test_bind2,             	TRUE},
        {test_piecewise_insert,  	TRUE},
        {test_piecewise_fetch,   	TRUE},
        {test_lob,               	TRUE},
        {test_nested_table,      	TRUE},
        {test_ref_cursor,        	TRUE},
        {test_plsql,             	TRUE},
        {test_dates,             	TRUE},
        {test_timestamp,         	TRUE},
        {test_describe,          	TRUE},
        {test_returning,         	TRUE},
        {test_returning_array,   	TRUE},
        {test_object_insert,     	TRUE},
        {test_object_fetch,      	TRUE},
        {test_object_fetch_string,  TRUE},
        {test_scrollable_cursor, 	TRUE},
        {test_collection,        	TRUE},
        {test_ref,               	TRUE},
        {test_directpath,           TRUE},
        {test_bigint,               TRUE},
        {test_number,               TRUE}
};

/* --------------------------------------------------------------------------------------------- *
 * variables
 * --------------------------------------------------------------------------------------------- */

static OCI_Environment *env = NULL;
static OCI_Connection  *cn = NULL;
static OCI_Statement   *st = NULL;
static OCI_Resultset   *rs = NULL;

static otext str[SIZE_STR+1];
static otext temp[SIZE_STR+1];

static OCI_Int32 nb_err  = 0;
static OCI_Int32 nb_warn = 0;

/* --------------------------------------------------------------------------------------------- *
 * err_handler
 * --------------------------------------------------------------------------------------------- */

void err_handler(OCI_Error *err)
{
    OCI_Int32 err_type = OCI_ErrorGetType(err);

    print_text("\n");

    if (err_type == OCI_ERR_TYPE_WARNING)
    {
         print_text("> WARNING : ");
         nb_warn++;
    }
    else
    {
        print_text("> ERROR   : ");
        nb_err++;
    }

    print_ostr(OCI_ErrorGetString(err));
    print_text("\n");
}

/* --------------------------------------------------------------------------------------------- *
 * main
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 omain(OCI_Int32 argc, oarg* argv[])
{
    otext home[SIZE_STR+1] = OTEXT("");
    otext dbs [SIZE_STR+1] = OTEXT("");
    otext usr [SIZE_STR+1] = OTEXT("");
    otext pwd [SIZE_STR+1] = OTEXT("");

    OCI_Size i;

    /* CHECK COMMAND LINE --------------------------------------------------- */

    if (argc < (ARG_COUNT-1))
    {
        return EXIT_FAILURE;
    }

    /* GET ARGUMENTS ---------------------------------------------------------*/

    GET_ARG(dbs, ARG_DB);
    GET_ARG(usr, ARG_USER);
    GET_ARG(pwd, ARG_PWD);

    if (argc == ARG_COUNT)
    {
        GET_ARG(home, ARG_HOME);
    }

    /* INITIALIZE OCI ------------------------------------------------------- */

    env = OCI_EnvironmentCreate(err_handler, home, OCI_ENV_DEFAULT | OCI_ENV_THREADED);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    OCI_EnvironmentEnableWarnings(env, TRUE);

    /* CONNECTION TO SERVER ------------------------------------------------- */

    print_text("Connecting to ");
    print_ostr(usr);
    print_text("/");
    print_ostr(pwd);
    print_text("@");
    print_ostr(dbs);
    print_text("\n\n");

    cn = OCI_ConnectionCreate(env, dbs, usr, pwd, OCI_SESSION_DEFAULT);

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
        print_ostr(OCI_ErrorGetString(OCI_GetLastError()));
    }

    cleanup();

    return EXIT_SUCCESS;
}

/* --------------------------------------------------------------------------------------------- *
 * cleanup
 * --------------------------------------------------------------------------------------------- */

void cleanup(void)
{
    OCI_EnvironmentFree(env);

    print_frmt("\n%i errors   \n", nb_err);
    print_frmt("\n%i warnings \n\n", nb_warn);
}

/* --------------------------------------------------------------------------------------------- *
 * disconnect
 * --------------------------------------------------------------------------------------------- */

void disconnect(void)
{
    OCI_ConnectionFree(cn);
}

/* --------------------------------------------------------------------------------------------- *
 * print_version
 * --------------------------------------------------------------------------------------------- */

void print_version(void)
{
    print_text("\n>>>>> OCILIB BUILD INFORMATION \n\n");

    if (OCI_EnvironmentGetImportMode(env) == OCI_IMPORT_MODE_LINKAGE)
        print_text("OCI import mode         : LINKAGE\n");
    else
        print_text("OCI import mode         : RUNTIME\n");

    if (OCI_EnvironmentGetCharset(env) == OCI_CHAR_MBCS)
        print_text("char type               : MBCS\n");
    else
        print_text("char type               : WIDE\n");

    print_text("\n>>>>> VERSIONS INFORMATION \n\n");

    print_frmt("OCILIB major    version : %i\n",   OCILIB_MAJOR_VERSION);
    print_frmt("OCILIB minor    version : %i\n",   OCILIB_MINOR_VERSION);
    print_frmt("OCILIB revision version : %i\n\n", OCILIB_REVISION_VERSION);

    /* print all versions */
    print_frmt("OCI compile     version : %u\n",   OCI_VER_MAJ(OCI_EnvironmentGetOCICompileVersion(env)));
    print_frmt("OCI runtime     version : %u\n\n", OCI_VER_MAJ(OCI_EnvironmentGetOCIRuntimeVersion(env)));

    print_frmt("Server major    version : %u\n",   OCI_ConnectionGetServerMajorVersion(cn));
    print_frmt("Server minor    version : %u\n",   OCI_ConnectionGetServerMinorVersion(cn));
    print_frmt("Server revision version : %u\n\n", OCI_ConnectionGetServerRevisionVersion(cn));

    print_frmt("Connection      version : %u\n\n", OCI_ConnectionGetVersion(cn));

    print_text("\n>>>>> SERVER VERSION BANNER \n\n");

    /* print server string version */

    print_ostr(OCI_ConnectionGetServerVersionString(cn));

    print_text("\n\n");

}

/* --------------------------------------------------------------------------------------------- *
 * create_tables
 * --------------------------------------------------------------------------------------------- */

void create_tables(void)
{
    print_text("\n>>>>> CREATE TABLES FOR DEMO \n\n");

    /* create types for the demo */
    OCI_StatementExecute(st, OTEXT("create type type_t as OBJECT (id int, name varchar2(50))"));

    OCI_StatementExecute(st, OTEXT("create type test_t as object ")
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

    OCI_StatementExecute(st, OTEXT("create type t_tab1_emp as VARRAY(100) of varchar2(50)"));

    OCI_StatementExecute(st, OTEXT("create type t_tab2_emp as table of varchar2(50)"));

    /* create table for the demo */
    OCI_StatementExecute(st, OTEXT("create table test_fetch(code int, article ")
                        OTEXT("varchar2(30), price float, creation date)"));

    OCI_StatementExecute(st, OTEXT("create table test_long_raw(code int, content long raw)"));

    OCI_StatementExecute(st, OTEXT("create table test_long_str(code int, content long)"));

    OCI_StatementExecute(st, OTEXT("create table test_lob(code int, content clob)"));

    OCI_StatementExecute(st, OTEXT("create table test_object(val test_t)"));

    OCI_StatementExecute(st, OTEXT("create table test_table_obj of type_t"));

    OCI_StatementExecute(st, OTEXT("create table test_array ")
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

    OCI_StatementExecute(st, OTEXT("create table test_coll_varray ")
                        OTEXT("( ")
                        OTEXT("    departement number, ")
                        OTEXT("    employees   t_tab1_emp ")
                        OTEXT(")")
                    );

    OCI_StatementExecute(st, OTEXT("create table test_coll_nested ")
                        OTEXT("( ")
                        OTEXT("    departement number, ")
                        OTEXT("    employees   t_tab2_emp ")
                        OTEXT(") nested table employees store as test_table_emp")
                    );

    OCI_StatementExecute(st, OTEXT("create table test_directpath(val_int number(8,4), ")
                        OTEXT(" val_str varchar2(30), val_date date)"));

    OCI_StatementExecute(st, OTEXT("create type test_num_coll_t as varray(10) of number"));

    OCI_StatementExecute(st, OTEXT("create type test_num_t as object (value number)"));

    OCI_StatementExecute(st, OTEXT("create table test_number (value number)"));


    /* insert data into the demo tables */
    OCI_StatementExecute(st, OTEXT("insert into test_fetch ")
                        OTEXT("(code, article, price, creation) ")
                        OTEXT("values (1, 'shoes', 3.14, to_date('1978-12-23', 'YYYY-MM-DD'))"));

    OCI_StatementExecute(st, OTEXT("insert into test_fetch ")
                        OTEXT("(code, article, price, creation) ")
                        OTEXT("values (2, 'shirt', 5.99, to_date('1999-09-12', 'YYYY-MM-DD'))"));

    OCI_StatementExecute(st, OTEXT("insert into test_lob(code,content)  ")
                        OTEXT("values (1, EMPTY_CLOB())"));

    OCI_StatementExecute(st, OTEXT("insert into test_long_str(code,content) ")
                        OTEXT("values (1, 'Rugby rocks !')"));

    OCI_StatementExecute(st, OTEXT("insert into test_coll_varray(departement,employees) ")
                        OTEXT("values (1, t_tab1_emp('Peter', 'John', 'Paula', 'Gina'))"));

    OCI_StatementExecute(st, OTEXT("insert into test_coll_varray(departement,employees) ")
                        OTEXT("values (2, t_tab1_emp('Ben', 'Alice', 'Joel', 'Maria'))"));

    OCI_StatementExecute(st, OTEXT("insert into test_coll_nested(departement,employees) ")
                        OTEXT("values (1, t_tab2_emp('Vince', 'Richard', 'Rita', 'Sophia'))"));

    OCI_StatementExecute(st, OTEXT("insert into test_coll_nested(departement,employees) ")
                        OTEXT("values (2, t_tab2_emp('Paul', 'Sarah', 'Robert', 'Zoe'))"));

    OCI_StatementExecute(st, OTEXT("insert into test_table_obj values(type_t(1, 'shoes'))"));
    OCI_StatementExecute(st, OTEXT("insert into test_table_obj values(type_t(2, 'pen'))"));

    OCI_StatementExecute(st, OTEXT("insert into test_number values(3.14)"));
    OCI_StatementExecute(st, OTEXT("insert into test_number values(5.28)"));

    OCI_ConnectionCommit(cn);
}

/* --------------------------------------------------------------------------------------------- *
 * drop_tables
 * --------------------------------------------------------------------------------------------- */

void drop_tables(void)
{
    print_text("\n>>>>> DROPPING TABLES AND TYPES \n\n");

    OCI_StatementExecute(st, OTEXT("drop table test_fetch"));
    OCI_StatementExecute(st, OTEXT("drop table test_long_str"));
    OCI_StatementExecute(st, OTEXT("drop table test_long_raw"));
    OCI_StatementExecute(st, OTEXT("drop table test_lob"));
    OCI_StatementExecute(st, OTEXT("drop table test_array"));
    OCI_StatementExecute(st, OTEXT("drop table test_object"));
    OCI_StatementExecute(st, OTEXT("drop table test_coll_varray"));
    OCI_StatementExecute(st, OTEXT("drop table test_coll_nested"));
    OCI_StatementExecute(st, OTEXT("drop table test_table_obj"));
    OCI_StatementExecute(st, OTEXT("drop table test_directpath"));
    OCI_StatementExecute(st, OTEXT("drop table test_number"));

    OCI_StatementExecute(st, OTEXT("drop type  test_t"));
    OCI_StatementExecute(st, OTEXT("drop type  type_t"));
    OCI_StatementExecute(st, OTEXT("drop type  t_tab1_emp"));
    OCI_StatementExecute(st, OTEXT("drop type  t_tab2_emp"));
    OCI_StatementExecute(st, OTEXT("drop type test_num_coll_t"));
    OCI_StatementExecute(st, OTEXT("drop type test_num_t"));
}

/* --------------------------------------------------------------------------------------------- *
 * test_format
 * --------------------------------------------------------------------------------------------- */

void test_format(void)
{
    OCI_Int32 code = 1;

    print_text("\n>>>>> TEST FORMATTING \n\n");

    OCI_StatementExecuteFormat(st, OTEXT("select * from test_fetch where code = %i"), code);

    rs = OCI_StatementGetResultset(st);

    while (OCI_ResultsetFetchNext(rs))
    {
        print_frmt("> code : %i", OCI_ResultsetGetInt32(rs, 0));
        print_text(", action : "); print_ostr(OCI_ResultsetGetString(rs, 1));
        print_frmt(", price : %g", OCI_ResultsetGetDouble(rs,2));
        print_text(", date : "); print_ostr(OCI_ResultsetGetString(rs, 3));
        print_text("\n");
   }

    print_frmt("\n%u row(s) fetched\n", OCI_ResultsetGetRowCount(rs));
}


/* --------------------------------------------------------------------------------------------- *
 * test_immediate
 * --------------------------------------------------------------------------------------------- */

void test_immediate(void)
{
    OCI_Int32 code = 1;

    print_text("\n>>>>> TEST IMMEDIATE \n\n");

    OCI_ConnectionImmediate(cn, OTEXT("select code, article from test_fetch where code = 1"),
                  OCI_ARG_INT32, &code,
                  OCI_ARG_TEXT, temp);

    print_frmt("> code : %i ", code);
    print_text("- article : "); print_ostr(temp);
    print_text("\n");
}

/* --------------------------------------------------------------------------------------------- *
 * test_immediate_format
 * --------------------------------------------------------------------------------------------- */

void test_immediate_format(void)
{
    OCI_Int32 code = 1;

    print_text("\n>>>>> TEST IMMEDIATE FORMATTED\n\n");

    OCI_ConnectionImmediateFormat(cn, OTEXT("select article from test_fetch where code = %i"),
                     code, OCI_ARG_TEXT, temp);

    print_text("> article : "); print_ostr(temp);  print_text("\n");
}

/* --------------------------------------------------------------------------------------------- *
 * test_fetch
 * --------------------------------------------------------------------------------------------- */

void test_fetch(void)
{
    OCI_Int32 i, n;

    print_text("\n>>>>> SIMPLE TEST FETCH WITH META DATA\n\n");

    /* execute query in one go */
    OCI_StatementExecute(st, OTEXT("select * from test_fetch"));

    rs = OCI_StatementGetResultset(st);
    n  = OCI_ResultsetGetColumnCount(rs);

    /* print resultset columns info */
    for(i = 0; i < n; i++)
    {
        OCI_Column *col = OCI_ResultsetGetColumn(rs, i);

        print_frmt("> Field : #%i ", i);
        print_text("- Name  : "); print_ostr(OCI_ColumnGetName(col));
        print_text("\n");
    }

    print_text("\n");

    /* print resultset content */
    while (OCI_ResultsetFetchNext(rs))
    {
        print_frmt("> code : %i", OCI_ResultsetGetInt32(rs, 0));
        print_text(", action : "); print_ostr(OCI_ResultsetGetString(rs, 1));
        print_frmt(", price : %g", OCI_ResultsetGetDouble(rs,2));
        print_text(", date : "); print_ostr(OCI_ResultsetGetString(rs, 3));
        print_text("\n");
    }

    print_frmt("\n%u row(s) fetched\n", OCI_ResultsetGetRowCount(rs));
}

/* --------------------------------------------------------------------------------------------- *
 * test_bind1
 * --------------------------------------------------------------------------------------------- */

void test_bind1(void)
{
    OCI_Int32 code = 1;

    print_text("\n>>>>> TEST BINDING \n\n");

    /* execute query in three steps */
    OCI_StatementPrepare(st, OTEXT("select * from test_fetch where code = :code"));
    OCI_StatementBindInt32(st, OTEXT(":code"), &code);
    OCI_StatementExecutePrepared(st);

    rs = OCI_StatementGetResultset(st);

    /* print resultset content */
    while (OCI_ResultsetFetchNext(rs))
    {
        print_frmt("> code : %i", OCI_ResultsetGetInt32(rs, 0));
        print_text(", action : "); print_ostr(OCI_ResultsetGetString(rs, 1));
        print_frmt(", price : %g", OCI_ResultsetGetDouble(rs,2));
        print_text(", date : "); print_ostr(OCI_ResultsetGetString(rs, 3));
        print_text("\n");
    }

    print_frmt("\n%u row(s) fetched\n", OCI_ResultsetGetRowCount(rs));
}

/* --------------------------------------------------------------------------------------------- *
 * test_bind2
 * --------------------------------------------------------------------------------------------- */

void test_bind2(void)
{
    OCI_Lob *lob;
    OCI_Date *date;
    OCI_File *file;

    OCI_UInt32 char_count, byte_count;

    print_text("\n>>>>> SINGLE BINDING \n\n");

   /* prepare SQL */

    OCI_StatementPrepare(st, OTEXT("insert into test_array ")
                    OTEXT("( ")
                    OTEXT("   val_int,  val_dbl, val_flt, val_str, val_date, ")
                    OTEXT("   val_lob, val_file ")
                    OTEXT(") " )
                    OTEXT("values ")
                    OTEXT("( ")
                    OTEXT("   :val_int, :val_dbl, :val_flt, :val_str, :val_date, ")
                    OTEXT("   :val_lob, :val_file ")
                    OTEXT(") "));

    /* create objects and set values */

    /* date */
    date = OCI_DateCreate(env);
    OCI_DateSysDate(date);

    /* lob */
    lob = OCI_LobCreate(cn, OCI_CLOB);
    osprintf(temp, SIZE_STR+1, OTEXT("lob value00"));
    char_count = (OCI_UInt32) ostrlen(temp);
    byte_count = 0;
    OCI_LobWrite(lob, temp, &char_count, &byte_count);

    /* file */
    file = OCI_FileCreate(cn, OCI_BFILE);
    OCI_FileSetName(file, OTEXT("mydir"), OTEXT("file00.txt"));

    /* scalar types */
    OCI_Int32 i = 1;
    OCI_Double dbl = 3.14;
    OCI_Float flt = (OCI_Float) 3.14;
    osprintf(temp, SIZE_STR+1, OTEXT("Name00"));

    /* bind scalar C types arrays */

    OCI_StatementBindInt32(st, OTEXT(":val_int"),  &i);
    OCI_StatementBindDouble(st, OTEXT(":val_dbl"), &dbl);
    OCI_StatementBindFloat(st, OTEXT(":val_flt"), &flt);
    OCI_StatementBindString(st, OTEXT(":val_str"), (otext*) temp, 30);

    /* bind oracle types arrays */

    OCI_StatementBindDate(st, OTEXT(":val_date"), date);
    OCI_StatementBindLob(st, OTEXT(":val_lob"), lob);
    OCI_StatementBindFile(st, OTEXT(":val_file"), file);

    /* do insert */

    OCI_StatementExecutePrepared(st);

    print_frmt("Row inserted : %u\n\n", OCI_StatementGetAffectedRows(st));

    /* free objets */

    OCI_DateFree(date);
    OCI_LobFree(lob);
    OCI_FileFree(file);

    /* commit; */

    OCI_ConnectionCommit(cn);
}

/* --------------------------------------------------------------------------------------------- *
 * test_piecewise_insert
 * --------------------------------------------------------------------------------------------- */

void test_piecewise_insert(void)
{

    print_text("\n>>>>> TEST PIECEWISE INSERTING\n\n");

    /* this code could have been used with a text file in a LONG column
       as well...  */

    /* open the app file in for reading*/

    FILE * f = fopen(OCI_SHARED_LIB, "rb");

    if (f)
    {
        unsigned char buffer[SIZE_BUF];

        fseek (f , 0 , SEEK_END);
        OCI_Int32 n = ftell(f);
        rewind (f);

        print_frmt("\n%d bytes to write\n", n);

        OCI_Long * lg = OCI_LongCreate(st, OCI_BLONG);

        /* execute query in three steps */
        OCI_StatementPrepare(st,  OTEXT("insert into test_long_raw(code, content) ")
                         OTEXT("values (1, :data)"));

        OCI_StatementBindLong(st, OTEXT(":data"), lg, n);
        OCI_StatementExecutePrepared(st);

        /* write data into table by chunks of 2048 bytes */
        while ((n = (OCI_Int32) fread(buffer, 1, sizeof(buffer), f)))
        {
            OCI_LongWrite(lg, buffer, n);
        }

        print_frmt("\n%u bytes written\n", OCI_LongGetSize(lg));
        fclose(f);

        OCI_LongFree(lg);
        OCI_ConnectionCommit(cn);
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_piecewise_fetch
 * --------------------------------------------------------------------------------------------- */

void test_piecewise_fetch(void)
{
    unsigned char buffer[SIZE_BUF];
    OCI_Int32 n;

    print_text("\n>>>>> TEST PIECEWISE FETCHING\n\n");

    /* execute query in one go */
    OCI_StatementExecute(st, OTEXT("select content from test_long_raw ")
                              OTEXT("where code = 1"));

    rs = OCI_StatementGetResultset(st);

    /* read data by chunks of 2048 bytes*/
    while (OCI_ResultsetFetchNext(rs))
    {
        OCI_Long * lg = OCI_ResultsetGetLong(rs, 0);

        while ((n = OCI_LongRead(lg, buffer, sizeof(buffer)))) {}

        print_frmt("\n%u bytes read\n", OCI_LongGetSize(lg));
    }

    print_frmt("\n%u row(s) fetched\n", OCI_ResultsetGetRowCount(rs));

    print_text("\n>>>>> TEST LONG MAPPED TO STRING\n\n");

    OCI_StatementExecute(st, OTEXT("select content from test_long_str ")
                        OTEXT("where code = 1"));
    OCI_StatementSetLongMode(st, OCI_LONG_IMPLICIT);
    rs = OCI_StatementGetResultset(st);

    while (OCI_ResultsetFetchNext(rs))
    {
        const otext *long_str = OCI_ResultsetGetString(rs, 0);

        print_ostr(long_str);
        print_frmt("\n%d bytes read\n", (OCI_Int32) ostrlen(long_str));
    }

    print_frmt("\n%u row(s) fetched\n", OCI_ResultsetGetRowCount(rs));
}

/* --------------------------------------------------------------------------------------------- *
 * test_lob
 * --------------------------------------------------------------------------------------------- */

void test_lob(void)
{

    print_text("\n>>>>> TEST LOB MANIPULATION\n\n");

    OCI_StatementExecute(st, OTEXT("select code, content from test_lob ")
                        OTEXT("where code=1 for update"));

    rs = OCI_StatementGetResultset(st);

    while (OCI_ResultsetFetchNext(rs))
    {
        OCI_UInt32 char_count, byte_count;

        OCI_Lob *lob  = OCI_ResultsetGetLob(rs, 1);

        char_count = 7;
        byte_count = 0;
        OCI_LobWrite(lob, OTEXT("today, "), &char_count, &byte_count);

        char_count = 26;
        byte_count = 0;
        OCI_LobAppend(lob, OTEXT("i'm going to the cinema ! "), &char_count, &byte_count);
        OCI_LobSeek(lob, 0, OCI_SEEK_SET);

        char_count = SIZE_STR;
        byte_count = 0;
        OCI_LobRead(lob, temp, &char_count, &byte_count);
        temp[char_count] = 0;

        print_frmt("> code    : %i", OCI_ResultsetGetInt32(rs, 0));
        print_text(", content : "); print_ostr(temp);
        print_text("\n");
    }

    OCI_ConnectionCommit(cn);

    print_frmt("\n%u row(s) fetched\n", OCI_ResultsetGetRowCount(rs));
}

/* --------------------------------------------------------------------------------------------- *
 * test_nested_table
 * --------------------------------------------------------------------------------------------- */

void test_nested_table(void)
{
    print_text("\n>>>>> TEST NESTED TABLE \n\n");

    OCI_StatementExecute(st, OTEXT("select article, cursor(select sysdate ")
                        OTEXT("from dual) from test_fetch"));

    rs = OCI_StatementGetResultset(st);

    while (OCI_ResultsetFetchNext(rs))
    {
        OCI_Statement *st2 = OCI_ResultsetGetCursor(rs, 1);
        OCI_Resultset *rs2 = OCI_StatementGetResultset(st2);

        while (OCI_ResultsetFetchNext(rs2))
        {
            print_text("> article : "); print_ostr(OCI_ResultsetGetString(rs, 0));
            print_text(", date : ");    print_ostr(OCI_ResultsetGetString(rs2, 0));
            print_text("\n");
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_ref_cursor
 * --------------------------------------------------------------------------------------------- */

void test_ref_cursor(void)
{
    OCI_Statement* st2 = OCI_StatementCreate(cn);

    print_text("\n>>>>> TEST REF CURSOR \n\n");

    OCI_StatementPrepare(st, OTEXT("begin open :c for select * from test_fetch; end;"));
    OCI_StatementBindCursor(st, OTEXT(":c"), st2);
    OCI_StatementExecutePrepared(st);

    rs = OCI_StatementGetResultset(st2);

    /* print resultset content */
    while (OCI_ResultsetFetchNext(rs))
    {
        print_frmt("> code : %i", OCI_ResultsetGetInt32(rs, 0));
        print_text(", action : "); print_ostr(OCI_ResultsetGetString(rs, 1));
        print_frmt(", price : %g", OCI_ResultsetGetDouble(rs,2));
        print_text(", date : "); print_ostr(OCI_ResultsetGetString(rs, 3));
        print_text("\n");
    }

    print_frmt("\n%u row(s) fetched\n", OCI_ResultsetGetRowCount(rs));

    OCI_StatementFree(st2);
}

/* --------------------------------------------------------------------------------------------- *
 * test_plsql
 * --------------------------------------------------------------------------------------------- */

void test_plsql(void)
{
    OCI_Int32 res = 0;
    const otext *p=NULL;

    print_text("\n>>>>> TEST PL/SQL OUTPUT BIND\n\n");

    OCI_StatementPrepare(st, OTEXT("begin :res := trunc(sysdate+1)-trunc(sysdate-1); end;"));
    OCI_StatementBindInt32(st, OTEXT(":res"), &res);
    OCI_StatementExecutePrepared(st);

    print_text("\nPL/SQL : trunc(sysdate+1)-trunc(sysdate-1)\n");
    print_frmt("\nResult : %i\n", res);

#ifndef OCI_CHARSET_MBCS

    {
        OCI_UInt32 version = OCI_EnvironmentGetOCIRuntimeVersion(env);

        /* Oracle 8i has some troubles with SERVER OUTPUT in wide builds */

        if (version < OCI_9_0) 
            return;
    }

#endif

    print_text("\n>>>>> TEST PL/SQL SERVER OUTPUT\n\n");

    /* server output */

    OCI_ConnectionEnableServerOutput(cn, 32000, 5, 255);

    OCI_StatementExecute(st, OTEXT("begin ")
                        OTEXT("   dbms_output.put_line('First  line'); ")
                        OTEXT("   dbms_output.put_line('Second line'); ")
                        OTEXT("   dbms_output.put_line('Third  line'); ")
                        OTEXT("end;")
                   );

    while ((p = OCI_ConnectionGetServerOutput(cn)))
    {
        print_ostr(p);
        print_text("\n");
    }

    OCI_ConnectionDisableServerOutput(cn);
}

/* --------------------------------------------------------------------------------------------- *
 * test_dates
 * --------------------------------------------------------------------------------------------- */

void test_dates(void)
{
    OCI_Date *date, *date2;

    print_text("\n>>>>> TEST DATETIME MANIPULATION\n\n");

    date  = OCI_DateCreate(env);
    date2 = OCI_DateCreate(env);

    OCI_DateFromText(date, OTEXT("1978-04-13 20:20:12"), OTEXT("YYYY-MM-DD HH24:MI:SS"));
    OCI_DateToText(date, OTEXT("YYYY-MM-DD HH24:MI:SS"), SIZE_STR, str);
    print_text("\nDate                     : "); print_ostr(str);

    OCI_DateSysDate(date);
    OCI_DateToText(date, OTEXT("YYYY-MM-DD HH24:MI:SS"), SIZE_STR, str);
    print_text("\nSysdate                  : "); print_ostr(str);

    OCI_DateAddDays(date, 5);
    OCI_DateAddMonths(date, 2);
    OCI_DateToText(date, OTEXT("YYYY-MM-DD HH24:MI:SS"), SIZE_STR, str);
    print_text("\nDate + 5 days + 2 months : "); print_ostr(str);

    OCI_DateAssign(date2, date);
    OCI_DateLastDay(date);
    OCI_DateToText(date, OTEXT("YYYY-MM-DD"), SIZE_STR, str);
    print_text("\nLast day of the month    : "); print_ostr(str);

    print_frmt("\nEnd of the month - date  : %i days",
               OCI_DateDaysBetween(date, date2));
    print_text("\n");

    OCI_DateFree(date);
    OCI_DateFree(date2);
}

/* --------------------------------------------------------------------------------------------- *
 * test_timestamp
 * --------------------------------------------------------------------------------------------- */

void test_timestamp(void)
{
#ifndef OCI_CHARSET_MBCS

    OCI_UInt32 version = OCI_EnvironmentGetOCIRuntimeVersion(env);

    /* Oracle 9i has some troubles with formatting Intervals/timestamps in
       an UTF16 context... */

    if ((version >= OCI_9_0) || (version < OCI_10_1))
        return;
#endif

    if (OCI_EnvironmentGetOCIRuntimeVersion(env) >= OCI_9_0)
    {

        print_text("\n>>>>> TEST TIMESTAMP\n\n");

        OCI_Timestamp * tmsp = OCI_TimestampCreate(env, OCI_TIMESTAMP);

        OCI_TimestampSysTimestamp(tmsp);

        OCI_TimestampToText(tmsp, OTEXT("YYYY-MM-DD HH24:MI:SS:FF\n"), SIZE_STR, str, 3);


        print_text("Current timestamp            : "); print_ostr(str);

        /* intervals raw oci functions have some troubles with Oracle 9i. So let's
           use it for the demo only if we're using 10g or above */

        if (OCI_EnvironmentGetOCIRuntimeVersion(env) >= OCI_10_1)
        {

            print_text("\n>>>>> TEST INTERVAL \n\n");

            OCI_Interval * itv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);

            OCI_IntervalSetDaySecond(itv, 1,1,1,1,0);

            OCI_IntervalToText(itv, 3, 3, SIZE_STR, str);

            print_text("Interval                     : "); print_ostr(str);
            print_text("\n");

            OCI_TimestampIntervalAdd(tmsp, itv);

            OCI_TimestampToText(tmsp, OTEXT("YYYY-MM-DD HH24:MI:SS:FF\n"), SIZE_STR, str, 3);

            print_text("Current timestamp + Interval : "); print_ostr(str);

            OCI_IntervalFree(itv);
        }

        OCI_TimestampFree(tmsp);
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_describe
 * --------------------------------------------------------------------------------------------- */

void test_describe(void)
{

    OCI_Int32 i, n;

    print_text("\n>>>>> TEST DESCRIBING TABLE \n\n");

    OCI_TypeInfo * tbl = OCI_TypeInfoGet(cn, OTEXT("test_fetch"), OCI_TIF_TABLE);

    if (tbl)
    {
        print_text("Column Name         NULL ?  Type                        \n");
        print_text("--------------------------------------------------------\n");

        n = OCI_TypeInfoGetColumnCount(tbl);

        for(i = 0; i < n; i++)
        {
            OCI_Column   * col = OCI_TypeInfoGetColumn(tbl, i);

            OCI_ColumnGetFullSqlType(col, str, SIZE_STR);

#if defined(OCI_CHARSET_WIDE)
  #if !defined(_WINDOWS)
            printf("%-20ls%-8ls%-30ls\n",
  #else
            wprintf(OTEXT("%-20s%-8s%-30s\n"),
  #endif
#else
            printf("%-20s%-8s%-30s\n",
#endif
                    OCI_ColumnGetName(col),
                    OCI_ColumnGetNullable(col) == TRUE ? OTEXT("Y") : OTEXT("N"),
                    str);
        }
    }

    OCI_TypeInfoFree(tbl);

    /* TEST DESCRIBING TYPE ------------------------------------------------- */

    print_text("\n>>>>> TEST DESCRIBING TYPE \n\n");

    tbl = OCI_TypeInfoGet(cn, OTEXT("test_t"), OCI_TIF_TYPE);

    if (tbl)
    {
        print_text("Column Name         Type                           \n");
        print_text("---------------------------------------------------\n");

        n = OCI_TypeInfoGetColumnCount(tbl);

        for(i = 0; i < n; i++)
        {
            OCI_Column *col = OCI_TypeInfoGetColumn(tbl, i);

            OCI_ColumnGetFullSqlType(col, str, SIZE_STR);

#if defined(OCI_CHARSET_WIDE)
  #if !defined(_WINDOWS)
            printf("%-20ls%-30ls\n",
  #else
            wprintf(OTEXT("%-20s%-30s\n"),
  #endif
#else
            printf("%-20s%-30s\n",
#endif
                    OCI_ColumnGetName(col),  str);
        }
    }

    OCI_TypeInfoFree(tbl);
}

/* --------------------------------------------------------------------------------------------- *
 * test_returning
 * --------------------------------------------------------------------------------------------- */

void test_returning(void)
{

    print_text("\n>>>>> TEST RETURNING CLAUSE \n\n");

    OCI_StatementPrepare(st, OTEXT("update test_lob set code = code + 1 returning code, content into :i, :l"));
    OCI_StatementRegisterLob(st, OTEXT(":l"), OCI_CLOB);
    OCI_StatementRegisterInt32(st, OTEXT(":i"));
    OCI_StatementExecutePrepared(st);

    rs = OCI_StatementGetResultset(st);

    while (OCI_ResultsetFetchNext(rs))
    {
        OCI_UInt32 char_count, byte_count;

        OCI_Lob * lob = OCI_ResultsetGetLob(rs, 1);

        char_count = 10;
        byte_count = 0;
        OCI_LobAppend(lob, OTEXT("(modified)"), &char_count, &byte_count);
        OCI_LobSeek(lob, 0, OCI_SEEK_SET);

        char_count = SIZE_STR;
        byte_count = 0;
        OCI_LobRead(lob, temp, &char_count, &byte_count);
        temp[char_count] = 0;

        print_frmt("> code : %i - ", OCI_ResultsetGetInt32(rs, 0));
        print_ostr(temp);
        print_text("\n");
   }

    print_frmt("\n%u row(s) fetched\n", OCI_ResultsetGetRowCount(rs));

    OCI_ConnectionCommit(cn);
}

/* --------------------------------------------------------------------------------------------- *
 * test_returning_array
 * --------------------------------------------------------------------------------------------- */

void test_returning_array(void)
{
    OCI_Lob *lob;
    OCI_Date *date;
    OCI_File *file;
    OCI_Int32 i;

    /* arrays */
    OCI_Int32       tab_int [SIZE_TAB];
    otext     tab_str [SIZE_TAB][31];
    OCI_Float     tab_flt [SIZE_TAB];
    OCI_Double    tab_dbl [SIZE_TAB];

    OCI_Date  **tab_date = OCI_DateArrayCreate(env, SIZE_TAB);
    OCI_Lob   **tab_lob  = OCI_LobArrayCreate(cn,  OCI_CLOB, SIZE_TAB);
    OCI_File  **tab_file = OCI_FileArrayCreate(cn, OCI_BFILE, SIZE_TAB);

    print_text("\n>>>>> TEST ARRAY BINDING WITH RETURNING CLAUSE \n\n");

    /* prepare SQL */

    OCI_StatementPrepare(st, OTEXT("insert into test_array ")
                    OTEXT("( ")
                    OTEXT("   val_int,  val_dbl, val_flt, val_str, val_date, ")
                    OTEXT("   val_lob, val_file ")
                    OTEXT(") " )
                    OTEXT("values ")
                    OTEXT("( ")
                    OTEXT("   :val_int, :val_dbl, :val_flt, :val_str, :val_date, ")
                    OTEXT("   :val_lob, :val_file ")
                    OTEXT(") ")
                    OTEXT("returning")
                    OTEXT("  val_int,  val_dbl, val_flt, val_str, val_date, ")
                    OTEXT("   val_lob, val_file ")
                    OTEXT("into  " )
                    OTEXT("  :out_int, :out_dbl, :out_flt,  :out_str, :out_date, ")
                    OTEXT("   :out_lob, :out_file "));

    /* Set Array Size */

    OCI_StatementBindArraySetSize(st, SIZE_TAB);

    /* bind scalar C types arrays */

    OCI_StatementBindInt32Array(st, OTEXT(":val_int"),  tab_int, 0);
    OCI_StatementBindDoubleArray(st, OTEXT(":val_dbl"), tab_dbl, 0);
    OCI_StatementBindFloatArray(st, OTEXT(":val_flt"), tab_flt, 0);
    OCI_StatementBindStringArray(st, OTEXT(":val_str"), (otext*) tab_str, 30, 0);

    /* bind oracle types arrays */

    OCI_StatementBindDateArray(st, OTEXT(":val_date"), tab_date, 0);
    OCI_StatementBindLobArray(st, OTEXT(":val_lob"), tab_lob, OCI_CLOB, 0);
    OCI_StatementBindFileArray(st, OTEXT(":val_file"), tab_file, OCI_BFILE, 0);

    /* register C scalar types output */

    OCI_StatementRegisterInt32(st, OTEXT(":out_int"));
    OCI_StatementRegisterDouble(st, OTEXT(":out_dbl"));
    OCI_StatementRegisterFloat(st, OTEXT(":out_flt"));
    OCI_StatementRegisterString(st, OTEXT(":out_str"), 30);

    /* bind oracle types outputs */

    OCI_StatementRegisterDate(st, OTEXT(":out_date"));
    OCI_StatementRegisterLob(st, OTEXT(":out_lob"), OCI_CLOB);
    OCI_StatementRegisterFile(st, OTEXT(":out_file"), OCI_BFILE);

    /* create objects and set values */

    for(i = 0; i < SIZE_TAB; i++)
    {
        /* scalar types */
        tab_int[i] = i+1;
        tab_dbl[i] = 3.14*(OCI_Double)(i+1);
        tab_flt[i] = (OCI_Float) 3.14*(OCI_Float)(i+1);
        osprintf(tab_str[i], 31, OTEXT("Name%02i"), i+1);

        /* date */
        OCI_DateSysDate(tab_date[i]);

        /* lob */
        osprintf(temp, SIZE_STR+1, OTEXT("lob value%02i"), i+1);
        {
            OCI_UInt32 char_count = (OCI_UInt32) ostrlen(temp);
            OCI_UInt32 byte_count = 0;
            OCI_LobWrite(tab_lob[i], temp, &char_count, &byte_count);
        }

        /* file */
        osprintf(str, SIZE_STR+1, OTEXT("file%02i.txt"), i+1);
        OCI_FileSetName(tab_file[i], OTEXT("mydir"), str);
    }

    OCI_StatementExecutePrepared(st);

    print_frmt("Row inserted : %u\n\n", OCI_StatementGetAffectedRows(st));

    /* free objects */

    OCI_DateArrayFree(tab_date);
    OCI_LobArrayFree(tab_lob);
    OCI_FileArrayFree(tab_file);

    /* Get back data  from the returning clause
       Obviously, each entry of the array will return a resultset containing
       1 row only (DML insert)
    */

    rs = OCI_StatementGetResultset(st);

    i = 1;

    while (rs != NULL)
    {
        while (OCI_ResultsetFetchNext(rs))
        {
            print_frmt("Row # %d-------------------\n", i);

            print_frmt(".... val_int    : %i\n", OCI_ResultsetGetInt32ByName(rs, OTEXT(":OUT_INT")));
            print_frmt(".... val_dbl    : %g\n", OCI_ResultsetGetDoubleByName(rs, OTEXT(":OUT_DBL")));
            print_frmt(".... val_flt    : %g\n", OCI_ResultsetGetFloatByName(rs, OTEXT(":OUT_FLT")));
            print_text(".... val_str    : "); print_ostr(OCI_ResultsetGetStringByName(rs, OTEXT(":OUT_STR")));
            print_text("\n");

            date = OCI_ResultsetGetDateByName(rs, OTEXT(":OUT_DATE"));
            OCI_DateToText(date, OTEXT("YYYY-MM-DD HH24:MI:SS"), SIZE_STR, str);
            print_text(".... val_date   : "); print_ostr(str);
            print_text("\n");

            lob  = OCI_ResultsetGetLobByName(rs, OTEXT(":OUT_LOB"));
            {
                OCI_UInt32 char_count = 100;
                OCI_UInt32 byte_count = 0;
                OCI_LobRead(lob, temp, &char_count, &byte_count);
                temp[char_count] = 0;
            }
            print_text(".... val_lob    : "); print_ostr(temp);
            print_text("\n");

            file = OCI_ResultsetGetFileByName(rs, OTEXT(":OUT_FILE"));
            print_text(".... val_file   : ");
            print_ostr(OCI_FileGetDirectory(file));
            print_text("/");
            print_ostr(OCI_FileGetName(file));
            print_text("\n");

            print_text("\n");
            i++;
        }

        rs = OCI_StatementGetNextResultset(st);
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_object_insert
 * --------------------------------------------------------------------------------------------- */

void test_object_insert(void)
{

    char rawbuf[11];

    strcpy(rawbuf, "0123456789");

    print_text("\n>>>>> TEST OBJECT BINDING \n\n");

    /* create types for the demo */

    OCI_Object *obj  = OCI_ObjectCreate(OCI_TypeInfoGet(cn, OTEXT("test_t"), OCI_TIF_TYPE));

    OCI_ObjectSetInt32(obj, OTEXT("VAL_INT"), 1);
    OCI_ObjectSetDouble(obj, OTEXT("VAL_DBL"), 3.14);
    OCI_ObjectSetFloat(obj, OTEXT("VAL_FLT"), (OCI_Float) 3.14);
    OCI_ObjectSetString(obj, OTEXT("VAL_STR"), OTEXT("USB KEY 2go"));
    OCI_ObjectSetRaw(obj, OTEXT("VAL_RAW"), rawbuf, 10);

    OCI_Date * date = OCI_DateCreate(env);
    OCI_DateSysDate(date);
    OCI_ObjectSetDate(obj, OTEXT("VAL_DATE"), date);

    OCI_Object * obj2 = OCI_ObjectCreate(OCI_TypeInfoGet(cn, OTEXT("TYPE_T"), OCI_TIF_TYPE));
    OCI_ObjectSetInt32(obj2, OTEXT("ID"), 1);

    OCI_ObjectSetString(obj2, OTEXT("NAME"), OTEXT("USB KEY 2go"));
    OCI_ObjectSetObject(obj, OTEXT("VAL_OBJ"), obj2);

    OCI_Lob  * lob = OCI_LobCreate(cn, OCI_CLOB);
    OCI_ObjectSetLob(obj, OTEXT("VAL_LOB"), lob);

    OCI_File * file = OCI_FileCreate(cn, OCI_BFILE);
    OCI_FileSetName(file, OTEXT("mydir"), OTEXT("myfile"));
    OCI_ObjectSetFile(obj, OTEXT("VAL_FILE"), file);

    OCI_StatementPrepare(st, OTEXT("insert into test_object values(:obj)"));
    OCI_StatementBindObject(st, OTEXT(":obj"), obj);
    OCI_StatementExecutePrepared(st);

    print_frmt("\n%u row(s) inserted\n", OCI_StatementGetAffectedRows(st));

    OCI_ConnectionCommit(cn);

    OCI_DateFree(date);
    OCI_LobFree(lob);
    OCI_FileFree(file);
    OCI_ObjectFree(obj2);
    OCI_ObjectFree(obj);
}

/* --------------------------------------------------------------------------------------------- *
 * test_object_fetch
 * --------------------------------------------------------------------------------------------- */

void test_object_fetch(void)
{
    char rawbuf[11] = "";

    print_text("\n>>>>> TEST OBJECT FETCHING \n\n");

    OCI_StatementExecute(st, OTEXT("select val from test_object for update"));

    rs = OCI_StatementGetResultset(st);

    /* print resultset content */
    while (OCI_ResultsetFetchNext(rs))
    {
        OCI_Object *obj  = OCI_ResultsetGetObject(rs, 0);

        print_frmt("val_int        : %i\n",  OCI_ObjectGetInt32(obj, OTEXT("VAL_INT")));
        print_frmt("val_dbl        : %g\n",  OCI_ObjectGetFloat(obj, OTEXT("VAL_DBL")));
        print_frmt("val_flt        : %g\n",  OCI_ObjectGetDouble(obj, OTEXT("VAL_FLT")));
        print_text("val_str        : "); print_ostr(OCI_ObjectGetString(obj, OTEXT("VAL_STR")));
        print_text("\n");

        temp[OCI_ObjectGetRaw(obj, OTEXT("VAL_RAW"), rawbuf, 10)] = 0;
        print_text("val_raw        : "); print_frmt("%s", rawbuf);
        print_text("\n");

        OCI_Date * date = OCI_ObjectGetDate(obj, OTEXT("VAL_DATE"));
        OCI_DateToText(date, OTEXT("YYYY-MM-DD HH24:MI:SS"), SIZE_STR, str);
        print_text("val_date       : "); print_ostr(str);
        print_text("\n");

        OCI_Lob * lob = OCI_ObjectGetLob(obj, OTEXT("VAL_LOB"));

        {
            OCI_UInt32 char_count = 17;
            OCI_UInt32 byte_count = 0;
            OCI_LobWrite(lob, OTEXT("Hello from Mars !"), &char_count, &byte_count);
        }
        OCI_LobSeek(lob, 0, OCI_SEEK_SET);

        {
            OCI_UInt32 char_count = SIZE_STR;
            OCI_UInt32 byte_count = 0;
            OCI_LobRead(lob, temp, &char_count, &byte_count);
            temp[char_count] = 0;
        }

        print_text("val_lob        : "); print_ostr(temp);
        print_text("\n");

        OCI_File * file = OCI_ObjectGetFile(obj, OTEXT("VAL_FILE"));
        print_text("val_file.dir   : "); print_ostr(OCI_FileGetDirectory(file));
        print_text("\n");
        print_text("val_file.name  : "); print_ostr(OCI_FileGetName(file));
        print_text("\n");

        OCI_Object * obj2 = OCI_ObjectGetObject(obj, OTEXT("VAL_OBJ"));
        print_frmt("val_obj.code   : %i\n", OCI_ObjectGetInt32(obj2, OTEXT("ID")));
        print_text("val_obj.name   : "); print_ostr(OCI_ObjectGetString(obj2, OTEXT("NAME")));
        print_text("\n");

        print_text("\n");;

    }

    OCI_ConnectionCommit(cn);
}


/* --------------------------------------------------------------------------------------------- *
 * test_object_string
 * --------------------------------------------------------------------------------------------- */

void test_object_fetch_string(void)
{
    print_text("\n>>>>> TEST OBJECT FETCHING AS STRING \n\n");

    OCI_StatementExecute(st, OTEXT("select val from test_object"));

    rs = OCI_StatementGetResultset(st);

    /* print resultset content */
    while (OCI_ResultsetFetchNext(rs))
    {
        print_ostr(OCI_ResultsetGetString(rs, 0));
        print_text("\n");
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_scrollable_cursor
 * --------------------------------------------------------------------------------------------- */

void test_scrollable_cursor(void)
{
    if (OCI_ConnectionGetVersion(cn) >= OCI_9_0)
    {
        print_text("\n>>>>> TEST SCROLLABLE CURSORS \n\n");

        OCI_StatementSetFetchMode(st, OCI_SFM_SCROLLABLE);

        OCI_StatementExecute(st, OTEXT("select table_name from user_tables where ")
                            OTEXT("table_name like 'TEST_%' order by table_name"));

        rs = OCI_StatementGetResultset(st);

        if (OCI_ResultsetFetchLast(rs))
            print_frmt("... Resultset contains %u rows\n\n", OCI_ResultsetGetRowCount(rs));

        print_text("... Go to row 1\n");
        if (OCI_ResultsetFetchFirst(rs))
        {
            print_ostr(OCI_ResultsetGetString(rs, 0));
            print_text("\n");
        }

        print_frmt("... Enumerate from row 2 to row %u\n", OCI_ResultsetGetRowCount(rs));
        while (OCI_ResultsetFetchNext(rs))
        {
            print_ostr(OCI_ResultsetGetString(rs, 0));
            print_text("\n");
        }

        print_frmt("... Enumerate from row %u back to row 1\n", OCI_ResultsetGetRowCount(rs)-1);
        while (OCI_ResultsetFetchPrev(rs))
        {
            print_ostr(OCI_ResultsetGetString(rs, 0));
            print_text("\n");
        }

        print_text("... Go to the 3th row\n");
        if (OCI_ResultsetFetchSeek(rs, OCI_SFD_ABSOLUTE,  3))
        {
            print_ostr(OCI_ResultsetGetString(rs, 0));
            print_text("\n");
        }

        print_text("... Fetch the next 2 rows\n");
        while ((OCI_ResultsetGetCurrentRow(rs) < 5) && OCI_ResultsetFetchNext(rs))
        {
            print_ostr(OCI_ResultsetGetString(rs, 0));
            print_text("\n");
        }

        OCI_StatementSetFetchMode(st, OCI_SFM_DEFAULT);
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_collection
 * --------------------------------------------------------------------------------------------- */

void test_collection(void)
{
    OCI_UInt32 i, n;

    print_text("\n>>>>> TEST VARRAY BINDING \n\n");

    OCI_TypeInfo * type = OCI_TypeInfoGet(cn, OTEXT("T_TAB1_EMP"), OCI_TIF_TYPE);
    OCI_Collection     * coll = OCI_CollectionCreate(type);

    OCI_StatementPrepare(st, OTEXT("begin")
                    OTEXT("    select employees into :tab_emp ")
                    OTEXT("    from test_coll_varray ")
                    OTEXT("    where departement = :id; ")
                    OTEXT("end;"));

    OCI_StatementBindCollection(st, OTEXT(":tab_emp"), coll);
    OCI_StatementBindUInt32(st, OTEXT(":id"), &i);

    i = 1;

    OCI_StatementExecutePrepared(st);

    OCI_Iterator     * iter = OCI_IteratorCreate(coll);
    OCI_Element     * elem = OCI_IteratorGetNext(iter);

    print_frmt("Department ID #%u\n\n", i);

    while (elem != NULL)
    {
        print_text("... Employee : ");
        print_ostr(OCI_ElementGetString(elem));
        print_text("\n");

        elem = OCI_IteratorGetNext(iter);
    }

    OCI_IteratorFree(iter);
    OCI_CollectionFree(coll);

    print_text("\n>>>>> TEST VARRAY FETCHING WITH ITERATOR \n\n");

    OCI_StatementExecute(st, OTEXT("SELECT * from test_coll_varray"));

    rs = OCI_StatementGetResultset(st);

    while (OCI_ResultsetFetchNext(rs))
    {
        coll = OCI_ResultsetGetColl(rs, 1);

        iter = OCI_IteratorCreate(coll);
        elem = OCI_IteratorGetNext(iter);

        print_frmt("\nDepartment ID #%d\n\n", OCI_ResultsetGetInt32(rs, 0));

        while (elem != NULL)
        {
            print_text("... Employee : ");
            print_ostr(OCI_ElementGetString(elem));
            print_text("\n");

            elem = OCI_IteratorGetNext(iter);
        }

        OCI_IteratorFree(iter);
    }

    print_text("\n>>>>> TEST VARRAY PRINTING \n\n");

    if (OCI_CollectionToText(coll, &n, NULL))
    {
        otext *tmp_str = (otext* ) calloc(n+1, sizeof(otext));

        if (tmp_str)
        {
            OCI_CollectionToText(coll, &n, tmp_str);
            print_ostr(tmp_str);
            print_text("\n");

            free(tmp_str);
        }
    }

    print_text("\n>>>>> TEST NESTED TABLE FETCHING WITH INDEX ACCESS \n\n");

    OCI_StatementExecute(st, OTEXT("SELECT * from test_coll_nested"));

    rs = OCI_StatementGetResultset(st);

    while (OCI_ResultsetFetchNext(rs))
    {
        coll = OCI_ResultsetGetColl(rs, 1);

        print_frmt("\nDepartment ID #%d\n\n", OCI_ResultsetGetInt32(rs, 0));

        n = OCI_CollectionGetSize(coll);

        for(i = 0; i < n; i++)
        {
            elem = OCI_CollectionGetElement(coll, i);

            print_text("... Employee : ");
            print_ostr(OCI_ElementGetString(elem));
            print_text("\n");

        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * test_ref
 * --------------------------------------------------------------------------------------------- */

void test_ref(void)
{
    OCI_Reference    *ref;

    print_text("\n>>>>> TEST REF FETCHING \n\n");

    OCI_StatementExecute(st, OTEXT("select ref(e) from test_table_obj e"));
    rs = OCI_StatementGetResultset(st);

    while (OCI_ResultsetFetchNext(rs))
    {
        ref = OCI_ResultsetGetReference(rs, 0);

        OCI_Object * obj = OCI_ReferenceGetObject(ref);

        print_frmt("%i - ", OCI_ObjectGetInt32(obj, OTEXT("ID")));
        print_ostr(OCI_ObjectGetString(obj, OTEXT("NAME")));
        print_text("\n");
    }

    print_text("\n>>>>> TEST REF PL/SQL BINDING \n\n");

    ref = OCI_ReferenceCreate(OCI_TypeInfoGet(cn, OTEXT("type_t"), OCI_TIF_TYPE));

    OCI_StatementPrepare(st, OTEXT("begin ")
                    OTEXT("  select ref(e) into :r from test_table_obj e where e.id = 1; ")
                    OTEXT("end; "));

    OCI_StatementBindReference(st, OTEXT(":r"), ref);
    OCI_StatementExecutePrepared(st);

    OCI_Object *obj = OCI_ReferenceGetObject(ref);
    print_frmt("%i - ", OCI_ObjectGetInt32(obj, OTEXT("ID")));
    print_ostr(OCI_ObjectGetString(obj, OTEXT("NAME")));
    print_text("\n");

    OCI_ReferenceFree(ref);
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

   if (OCI_VER_MAJ(OCI_EnvironmentGetOCIRuntimeVersion(env)) == OCI_ConnectionGetServerMajorVersion(cn))
   {
        OCI_Boolean res = TRUE;

        otext val1[SIZE_COL1+1] = OTEXT("");
        otext val2[SIZE_COL2+1] = OTEXT("");
        otext val3[SIZE_COL3+1] = OTEXT("");

        OCI_Int32 i = 0, j = 0, nb_rows = SIZE_ARRAY;
        OCI_Int32 state;

       /* commit any previous pending modifications */

        OCI_ConnectionCommit(cn);

        print_text("\n>>>>> TEST DIRECT PATH (10 loads of 100 rows) \n\n");

        OCI_TypeInfo * tbl = OCI_TypeInfoGet(cn, OTEXT("test_directpath"), OCI_TIF_TABLE);
        OCI_DirectPath *dp  = OCI_DirectPathCreate(tbl, NULL, NUM_COLS, nb_rows);

        /* optional attributes to set */

        res = res && OCI_DirectPathSetBufferSize(dp, 64000);
        res = res && OCI_DirectPathSetNoLog(dp, TRUE);
        res = res && OCI_DirectPathSetParallel(dp, TRUE);

        if(OCI_EnvironmentGetOCIRuntimeVersion(env) >= OCI_9_2)
        {
            res = res && OCI_DirectPathSetCacheSize(dp, 100);
        }

        /* describe the target table */

        res = res && OCI_DirectPathSetColumn(dp, 0, OTEXT("VAL_INT"),  SIZE_COL1, NULL);
        res = res && OCI_DirectPathSetColumn(dp, 1, OTEXT("VAL_STR"),  SIZE_COL2, NULL);
        res = res && OCI_DirectPathSetColumn(dp, 2, OTEXT("VAL_DATE"), SIZE_COL3, OTEXT("YYYYMMDD"));

        /* prepare the load */

        res = res && OCI_DirectPathPrepare(dp);

        if (res)
        {
            nb_rows = OCI_DirectPathGetMaxRows(dp);          
            
            for (i = 0; i < NB_LOAD ; i++)
            {
                OCI_DirectPathReset(dp);

                for (j = 0; j < nb_rows && res; j++)
                {
                    /* fill test values */

                    osprintf(val1, SIZE_COL1+1, OTEXT("%04d"), i + (i*100));
                    osprintf(val2, SIZE_COL2+1, OTEXT("value %05d"), j + (i*100));
                    osprintf(val3, SIZE_COL3+1, OTEXT("%04d%02d%02d"), (j%23)+1 + 2000,
                                                                     (j%11)+1,
                                                                     (j%23)+1);

                    res = res && OCI_DirectPathSetEntry(dp, j, 0, val1, (OCI_UInt32) ostrlen(val1), TRUE);
                    res = res && OCI_DirectPathSetEntry(dp, j, 1, val2, (OCI_UInt32) ostrlen(val2), TRUE);
                    res = res && OCI_DirectPathSetEntry(dp, j, 2, val3, (OCI_UInt32) ostrlen(val3), TRUE);
                }

               /* load data to the server */

                while (res == TRUE)
                {
                    state = OCI_DirectPathConvert(dp);

                    if ((state == OCI_DPR_FULL) || (state == OCI_DPR_COMPLETE))
                        res = OCI_DirectPathLoad(dp);

                    if (state == OCI_DPR_COMPLETE || state == OCI_DPR_ERROR)
                        break;
                }
            }

            /* commits changes */

            res = res && OCI_DirectPathFinish(dp);

            if (res)
            {
                print_frmt("%04u row(s) loaded\n", OCI_DirectPathGetRowCount(dp));
            }

            /* free direct path object */
        }

        OCI_DirectPathFree(dp);
   }
}

/* --------------------------------------------------------------------------------------------- *
* test_bigint
* --------------------------------------------------------------------------------------------- */

void test_bigint(void)
{
    OCI_Int64 value1 = 12345, value2 = 0;

    print_text("\n>>>>> TEST BINDING BIG INT \n\n");

    OCI_StatementPrepare(st, OTEXT("begin :value2 := :value1 * :value1; end;"));
    OCI_StatementBindInt64(st, OTEXT(":value1"), &value1);
    OCI_StatementBindInt64(st, OTEXT(":value2"), &value2);
    OCI_StatementExecutePrepared(st);

    printf("\n%lld * %lld = %lld \n", value1, value1, value2);
}

/* --------------------------------------------------------------------------------------------- *
* test_number
* --------------------------------------------------------------------------------------------- */

void test_number()
{
    OCI_TypeInfo   *tc = NULL;
    OCI_TypeInfo   *to = NULL;
    OCI_Number     *nm = NULL;
    OCI_Collection       *cl = NULL;
    OCI_Element       *el = NULL;
    OCI_Object     *ob = NULL;
    OCI_Number     *no = NULL;
    OCI_Number     *nt = NULL;
    OCI_Number    **ar = NULL;

    OCI_UInt32 i, n;
 
    print_text("\n>>>>> TEST ORACLE NUMBER \n\n");

    tc = OCI_TypeInfoGet(cn, OTEXT("test_num_coll_t"), OCI_TIF_TYPE);
    to = OCI_TypeInfoGet(cn, OTEXT("test_num_t"), OCI_TIF_TYPE);
    cl = OCI_CollectionCreate(tc);
    ob = OCI_ObjectCreate(to);
    el = OCI_ElementCreate(tc);
    nm = OCI_NumberCreate(env);
    ar = OCI_NumberArrayCreate(env, 5);

    // Testing fetching numbers (as number and using implicit conversion number to string)
    OCI_StatementPrepare(st, OTEXT("select value from test_number"));
    OCI_StatementExecutePrepared(st);
    rs = OCI_StatementGetResultset(st);
    while (OCI_ResultsetFetchNext(rs))
    {
        OCI_Number  *num = OCI_ResultsetGetNumber(rs, 0);
        const otext *buf = OCI_ResultsetGetString(rs, 0);

        OCI_NumberToText(num, NULL, SIZE_STR, str);

        print_ostr(str);
        print_text(" - ");
        print_ostr(buf);
        print_text("\n");
    }

    // Testing binding external single number
    OCI_StatementPrepare(st, OTEXT("begin :1 := :1 *2 ; end;"));
    OCI_StatementBindNumber(st, OTEXT(":1"), nm);
    OCI_NumberFromText(nm, OTEXT("1234.4321"), NULL);
    OCI_StatementExecutePrepared(st);
    OCI_NumberToText(nm, NULL, SIZE_STR, str);
    print_ostr(str);
    print_text("\n");

    // Testing binding internal single number
    OCI_StatementPrepare(st, OTEXT("begin :1 := :1 *2 ; end;"));
    OCI_StatementSetBindAllocation(st, OCI_BAM_INTERNAL);
    OCI_StatementBindNumber(st, OTEXT(":1"), NULL);
    nt = OCI_BindGetData(OCI_StatementGetBind(st, 0));
    OCI_NumberFromText(nt, OTEXT("1234.4321"), NULL);
    OCI_StatementExecutePrepared(st);
    OCI_NumberToText(nt, NULL, SIZE_STR, str);
    print_ostr(str);
    print_text("\n");
    OCI_StatementSetBindAllocation(st, OCI_BAM_EXTERNAL);

    // Testing registering number for a returning into statement
    OCI_StatementPrepare(st, OTEXT("update test_number set value  = value *2 returning value into :1"));
    OCI_StatementRegisterNumber(st, OTEXT(":1"));
    OCI_StatementExecutePrepared(st);
    rs = OCI_StatementGetResultset(st);
    while (OCI_ResultsetFetchNext(rs))
    {
        print_ostr(OCI_ResultsetGetString(rs, 0));
        print_text("\n");
    }

    // testing Collections
    OCI_NumberFromText(nm, OTEXT("1111.2222"), NULL);
    OCI_ElementSetNumber(el, nm);
    OCI_CollectionAppend(cl, el);
    OCI_NumberFromText(nm, OTEXT("3333.4444"), NULL);
    OCI_ElementSetNumber(el, nm);
    OCI_CollectionAppend(cl, el);

    for (i = 0, n = OCI_CollectionGetCount(cl); i < n; i++)
    {
        OCI_Element   *e = OCI_CollectionGetElement(cl, i);
        OCI_Number *num = OCI_ElementGetNumber(e);
        OCI_NumberToText(num, NULL, SIZE_STR, str);
        print_ostr(str);
        print_text("\n");
    }

    // Testing objects
    OCI_NumberFromText(nm, OTEXT("5555.6666"), NULL);
    OCI_NumberToText(nm, NULL, SIZE_STR, str);
    print_ostr(str);
    print_text("\n");
    OCI_ObjectSetNumber(ob, OTEXT("value"), nm);
    no = OCI_ObjectGetNumber(ob, OTEXT("value"));
    OCI_NumberToText(no, NULL, SIZE_STR, str);
    print_ostr(str);
    print_text("\n");

    // Testing fetching infinite values to string
    OCI_StatementPrepare(st, OTEXT("SELECT utl_raw.cast_to_number('FF65'), utl_raw.cast_to_number('00') from dual"));
    OCI_StatementExecutePrepared(st);
    rs = OCI_StatementGetResultset(st);
    while (OCI_ResultsetFetchNext(rs))
    {
        print_text("pos infinite = ");
        print_ostr(OCI_ResultsetGetString(rs, 0));
        print_text("\n");

        print_text("neg infinite = ");
        print_ostr(OCI_ResultsetGetString(rs, 1));
        print_text("\n");
    }

    // Testing formatting
    OCI_NumberFromText(nm, OTEXT("1234.4321"), NULL);
    OCI_ConnectionImmediateFormat(cn, OTEXT("select %n * 2 from dual"), nm, OCI_ARG_OF(nm));
    OCI_NumberToText(nm, NULL, SIZE_STR, str);
    print_ostr(str);
    print_text("\n");

    // Testing array of numbers
    OCI_NumberFromText(ar[0], OTEXT("1.2"), NULL);
    OCI_NumberFromText(ar[1], OTEXT("~"), NULL);
    OCI_NumberFromText(ar[2], OTEXT("5.6"), NULL);
    OCI_NumberFromText(ar[3], OTEXT("-~"), NULL);
    OCI_NumberFromText(ar[4], OTEXT("9.2"), NULL);

    OCI_NumberToText(ar[0], NULL, SIZE_STR, str);
    print_ostr(str);
    print_text("\n");

    OCI_NumberToText(ar[1], NULL, SIZE_STR, str);
    print_ostr(str);
    print_text("\n");

    OCI_NumberToText(ar[2], NULL, SIZE_STR, str);
    print_ostr(str);
    print_text("\n");

    OCI_NumberToText(ar[3], NULL, SIZE_STR, str);
    print_ostr(str);
    print_text("\n");

    OCI_NumberToText(ar[4], NULL, SIZE_STR, str);
    print_ostr(str);
    print_text("\n");

    OCI_NumberArrayFree(ar);
    OCI_NumberFree(nm);
    OCI_ElementFree(el);
    OCI_CollectionFree(cl);
    OCI_ObjectFree(ob);
}