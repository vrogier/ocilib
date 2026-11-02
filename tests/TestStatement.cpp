#include "ocilib_tests.h"
#include "test_internals.h"

namespace TestCApi
{
    TEST(TestStatement, Metadata)
    {
        ExecDML(OTEXT("CREATE TABLE TestCApiCovMeta(id NUMBER)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        const otext* sql = OTEXT("INSERT INTO TestCApiCovMeta(id) VALUES(1)");
        ASSERT_TRUE(OCI_ExecuteStmt(stmt, sql));

        const otext* gotSql = OCI_GetSql(stmt);
        ASSERT_NE(nullptr, gotSql);
        ASSERT_EQ(ostring(sql), ostring(gotSql));

        ASSERT_EQ(OCI_CST_INSERT, OCI_GetStatementType(stmt));
        ASSERT_EQ(1u, OCI_GetAffectedRows(stmt));

        (void) OCI_GetSqlIdentifier(stmt);
        ASSERT_EQ(nullptr, OCI_GetBatchError(stmt));
        ASSERT_EQ(0u, OCI_GetBatchErrorCount(stmt));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TABLE TestCApiCovMeta"));
    }

    TEST(TestStatement, SettersAndGetters)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_SetFetchMode(stmt, OCI_SFM_DEFAULT));
        ASSERT_EQ((OCI_UInt32) OCI_SFM_DEFAULT, OCI_GetFetchMode(stmt));
        ASSERT_TRUE(OCI_SetBindMode(stmt, OCI_BIND_BY_NAME));
        ASSERT_EQ((OCI_UInt32) OCI_BIND_BY_NAME, OCI_GetBindMode(stmt));
        ASSERT_TRUE(OCI_SetBindAllocation(stmt, OCI_BAM_EXTERNAL));
        ASSERT_EQ((OCI_UInt32) OCI_BAM_EXTERNAL, OCI_GetBindAllocation(stmt));
        ASSERT_TRUE(OCI_SetFetchSize(stmt, 10));
        ASSERT_EQ(10u, OCI_GetFetchSize(stmt));
        ASSERT_TRUE(OCI_SetPrefetchSize(stmt, 100));
        ASSERT_EQ(100u, OCI_GetPrefetchSize(stmt));
        ASSERT_TRUE(OCI_SetPrefetchMemory(stmt, 1024));
        ASSERT_EQ(1024u, OCI_GetPrefetchMemory(stmt));
        ASSERT_TRUE(OCI_SetPieceSize(stmt, 32000));
        ASSERT_EQ(32000u, OCI_GetPieceSize(stmt));
        ASSERT_TRUE(OCI_SetLongMode(stmt, OCI_LONG_IMPLICIT));
        ASSERT_EQ((OCI_UInt32) OCI_LONG_IMPLICIT, OCI_GetLongMode(stmt));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select 1 from dual")));
        ASSERT_TRUE(OCI_BindArraySetSize(stmt, 1));
        ASSERT_EQ(1u, OCI_BindArrayGetSize(stmt));

        ASSERT_TRUE(OCI_AllowRebinding(stmt, TRUE));
        ASSERT_TRUE(OCI_IsRebindingAllowed(stmt));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestStatement, ParseAndDescribe)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select :id as id, :name as name from dual")));

        OCI_UInt32 count = 0;
        const otext** names = nullptr;
        ASSERT_TRUE(OCI_GetParseBindNames(stmt, &count, &names));
        ASSERT_EQ(2u, count);
        ASSERT_NE(nullptr, names);

        ASSERT_TRUE(OCI_Parse(stmt, OTEXT("select 1 from dual")));
        ASSERT_TRUE(OCI_Describe(stmt, OTEXT("select 1 as c from dual")));

        const auto stmt2 = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt2);
        ASSERT_FALSE(OCI_ExecuteStmt(stmt2, OTEXT("select * from")));
        ASSERT_GT(OCI_GetSqlErrorPos(stmt2), 0u);
        ASSERT_TRUE(OCI_StatementFree(stmt2));

        const auto stmt3 = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt3);
        ASSERT_TRUE(OCI_ExecuteStmt(stmt3, OTEXT("select 1 from dual")));
        ASSERT_EQ((OCI_UInt32) OCI_CST_SELECT, OCI_GetStatementType(stmt3));
        ASSERT_TRUE(OCI_StatementFree(stmt3));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestStatement, ExecuteStmtFmt)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select %i as c from dual"), 123));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(123, OCI_GetInt(rs, 1));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestStatement, ParseAndDescribeFmt)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ParseFmt(stmt, OTEXT("select %i from dual"), 1));
        ASSERT_TRUE(OCI_DescribeFmt(stmt, OTEXT("select %i as c from dual"), 1));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestStatement, BindAllTypes)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        const auto num = OCI_NumberCreate(env);
        const auto date = OCI_DateCreate(env);
        const auto ts = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        const auto iv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        ASSERT_NE(nullptr, num);
        ASSERT_NE(nullptr, date);
        ASSERT_NE(nullptr, ts);
        ASSERT_NE(nullptr, iv);

        OCI_Int32 i = 1;
        OCI_UInt32 u = 2;
        OCI_Int16 s = 3;
        OCI_UInt16 us = 4;
        OCI_Int64 bi = 5;
        OCI_UInt64 bui = 6;
        OCI_Float f = 7.5f;
        OCI_Double db = 8.5;
        otext str[64] = OTEXT("text");
        unsigned char raw[20] = { 0x01, 0x02, 0x03 };

        ASSERT_TRUE(OCI_NumberSetValue(num, OCI_NUM_INT, &i));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select :i as i, :u as u, :s as s, :us as us, :bi as bi, :bui as bui, :f as f, :db as db, :n as n, :str as str, :r as r, :d as d, :ts as ts, :iv as iv from dual")));

        ASSERT_NE(nullptr, OCI_BindInt(stmt, OTEXT(":i"), &i));
        ASSERT_NE(nullptr, OCI_BindUnsignedInt(stmt, OTEXT(":u"), &u));
        ASSERT_NE(nullptr, OCI_BindShort(stmt, OTEXT(":s"), &s));
        ASSERT_NE(nullptr, OCI_BindUnsignedShort(stmt, OTEXT(":us"), &us));
        ASSERT_NE(nullptr, OCI_BindBigInt(stmt, OTEXT(":bi"), &bi));
        ASSERT_NE(nullptr, OCI_BindUnsignedBigInt(stmt, OTEXT(":bui"), &bui));
        ASSERT_NE(nullptr, OCI_BindFloat(stmt, OTEXT(":f"), &f));
        ASSERT_NE(nullptr, OCI_BindDouble(stmt, OTEXT(":db"), &db));
        ASSERT_NE(nullptr, OCI_BindNumber(stmt, OTEXT(":n"), num));
        ASSERT_NE(nullptr, OCI_BindString(stmt, OTEXT(":str"), str, 50));
        ASSERT_NE(nullptr, OCI_BindRaw(stmt, OTEXT(":r"), raw, 3));
        ASSERT_NE(nullptr, OCI_BindDate(stmt, OTEXT(":d"), date));
        ASSERT_NE(nullptr, OCI_BindTimestamp(stmt, OTEXT(":ts"), ts));
        ASSERT_NE(nullptr, OCI_BindInterval(stmt, OTEXT(":iv"), iv));

        ASSERT_EQ(14u, OCI_GetBindCount(stmt));
        ASSERT_NE(nullptr, OCI_GetBind(stmt, 1));
        ASSERT_NE(nullptr, OCI_GetBindByName(stmt, OTEXT(":i")));
        ASSERT_EQ(1u, OCI_GetBindIndex(stmt, OTEXT(":i")));

        ASSERT_TRUE(OCI_NumberFree(num));
        ASSERT_TRUE(OCI_DateFree(date));
        ASSERT_TRUE(OCI_TimestampFree(ts));
        ASSERT_TRUE(OCI_IntervalFree(iv));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestStatement, BindArrays)
    {
        ExecDML(OTEXT("create table TestStatementArrays (i number, u number, sh number, us number, bi number, bui number, f number, db number, n number, str varchar2(50), raw_col raw(20), d date, ts timestamp, iv interval day to second)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        const auto num = OCI_NumberCreate(env);
        const auto date = OCI_DateCreate(env);
        const auto ts = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        const auto iv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        ASSERT_NE(nullptr, num);
        ASSERT_NE(nullptr, date);
        ASSERT_NE(nullptr, ts);
        ASSERT_NE(nullptr, iv);

        OCI_Int32 i_arr[1] = { 1 };
        OCI_UInt32 u_arr[1] = { 2 };
        OCI_Int16 sh_arr[1] = { 3 };
        OCI_UInt16 us_arr[1] = { 4 };
        OCI_Int64 bi_arr[1] = { 5 };
        OCI_UInt64 bui_arr[1] = { 6 };
        OCI_Float f_arr[1] = { 7.5f };
        OCI_Double db_arr[1] = { 8.5 };
        OCI_Number* n_arr[1] = { num };
        OCI_Date* d_arr[1] = { date };
        OCI_Timestamp* ts_arr[1] = { ts };
        OCI_Interval* iv_arr[1] = { iv };
        otext str_arr[1][64] = { { OTEXT("text") } };
        unsigned char raw_arr[1][20] = { { 0x01, 0x02, 0x03 } };


        OCI_Int32 n = 1;
        ASSERT_TRUE(OCI_NumberSetValue(num, OCI_NUM_INT, &n));

        ASSERT_TRUE(OCI_Prepare(stmt,
            OTEXT("insert into TestStatementArrays (i, u, sh, us, bi, bui, f, db, n, str, raw_col, d, ts, iv) values (:i, :u, :sh, :us, :bi, :bui, :f, :db, :n, :str, :raw_col, :d, :ts, :iv)")));

        ASSERT_TRUE(OCI_BindArraySetSize(stmt, 1));

        ASSERT_NE(nullptr, OCI_BindArrayOfInts(stmt, OTEXT(":i"), i_arr, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfUnsignedInts(stmt, OTEXT(":u"), u_arr, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfShorts(stmt, OTEXT(":sh"), sh_arr, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfUnsignedShorts(stmt, OTEXT(":us"), us_arr, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfBigInts(stmt, OTEXT(":bi"), bi_arr, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfUnsignedBigInts(stmt, OTEXT(":bui"), bui_arr, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfDoubles(stmt, OTEXT(":db"), db_arr, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfFloats(stmt, OTEXT(":f"), f_arr, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfNumbers(stmt, OTEXT(":n"), n_arr, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfStrings(stmt, OTEXT(":str"), str_arr[0], 64, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfRaws(stmt, OTEXT(":raw_col"), raw_arr[0], 20, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfDates(stmt, OTEXT(":d"), d_arr, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfTimestamps(stmt, OTEXT(":ts"), ts_arr, OCI_TIMESTAMP, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfIntervals(stmt, OTEXT(":iv"), iv_arr, OCI_INTERVAL_DS, 0));

        const auto executed = OCI_Execute(stmt);
        if (!executed)
        {
            const auto err = OCI_GetLastError();
            std::wcout << OTEXT("Execute failed: ") << (err ? OCI_ErrorGetString(err) : OTEXT("unknown")) << std::endl;
        }
        ASSERT_TRUE(executed);
        ASSERT_EQ(1u, OCI_GetAffectedRows(stmt));

        ASSERT_TRUE(OCI_NumberFree(num));
        ASSERT_TRUE(OCI_DateFree(date));
        ASSERT_TRUE(OCI_TimestampFree(ts));
        ASSERT_TRUE(OCI_IntervalFree(iv));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("drop table TestStatementArrays"));
    }

    TEST(TestStatement, RegisterAllTypes)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_Prepare(stmt,
            OTEXT("BEGIN :i := 1; :u := 1; :sh := 1; :us := 1; :bi := 1; :db := 1.5; :f := 1.5; :n := 1; :ss := 'x'; :d := sysdate; :ts := systimestamp; :iv := interval '1' day; END;")));

        ASSERT_TRUE(OCI_RegisterInt(stmt, OTEXT(":i")));
        ASSERT_TRUE(OCI_RegisterUnsignedInt(stmt, OTEXT(":u")));
        ASSERT_TRUE(OCI_RegisterShort(stmt, OTEXT(":sh")));
        ASSERT_TRUE(OCI_RegisterUnsignedShort(stmt, OTEXT(":us")));
        ASSERT_TRUE(OCI_RegisterBigInt(stmt, OTEXT(":bi")));
        ASSERT_TRUE(OCI_RegisterDouble(stmt, OTEXT(":db")));
        ASSERT_TRUE(OCI_RegisterFloat(stmt, OTEXT(":f")));
        ASSERT_TRUE(OCI_RegisterNumber(stmt, OTEXT(":n")));
        ASSERT_TRUE(OCI_RegisterString(stmt, OTEXT(":ss"), 20));
        ASSERT_TRUE(OCI_RegisterDate(stmt, OTEXT(":d")));
        ASSERT_TRUE(OCI_RegisterTimestamp(stmt, OTEXT(":ts"), OCI_TIMESTAMP));
        ASSERT_TRUE(OCI_RegisterInterval(stmt, OTEXT(":iv"), OCI_INTERVAL_DS));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestStatement, ExecuteAndGetResultset)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select 1 as n from dual")));

        const auto rs = OCI_ExecuteAndGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(1, OCI_GetInt(rs, 1));

        // re-executing a statement that holds a resultset releases it first
        const auto rs2 = OCI_ExecuteStmtAndGetResultset(stmt, OTEXT("select 2 as n from dual"));
        ASSERT_NE(nullptr, rs2);
        ASSERT_TRUE(OCI_FetchNext(rs2));
        ASSERT_EQ(2, OCI_GetInt(rs2, 1));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestStatement, PrepareFmt)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_PrepareFmt(stmt, OTEXT("select %i as n, %s as s from dual"), 7, OTEXT("txt")));
        ASSERT_TRUE(OCI_Execute(stmt));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(7, OCI_GetInt(rs, 1));
        ASSERT_EQ(ostring(OTEXT("txt")), ostring(OCI_GetString(rs, 2)));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestStatement, SqlCommandAndVerb)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        // not executed yet: getter must fail
        ASSERT_EQ(OCI_UNKNOWN, (OCI_Int32) OCI_GetSQLCommand(stmt));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select 1 from dual")));
        ASSERT_EQ(OCI_SFC_SELECT, OCI_GetSQLCommand(stmt));
        ASSERT_EQ(ostring(OTEXT("SELECT")), ostring(OCI_GetSQLVerb(stmt)));

        ExecDML(OTEXT("drop table TestStatementCmd"));
        ExecDML(OTEXT("create table TestStatementCmd (id int)"));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("insert into TestStatementCmd values (1)")));
        ASSERT_EQ(OCI_SFC_INSERT, OCI_GetSQLCommand(stmt));
        ASSERT_EQ(ostring(OTEXT("INSERT")), ostring(OCI_GetSQLVerb(stmt)));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("update TestStatementCmd set id = 2")));
        ASSERT_EQ(ostring(OTEXT("UPDATE")), ostring(OCI_GetSQLVerb(stmt)));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("drop table TestStatementCmd")));
        ASSERT_EQ(ostring(OTEXT("DROP TABLE")), ostring(OCI_GetSQLVerb(stmt)));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    /* --- OCI_Immediate argument types --- */

    class TestStatementImmediate : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("drop table TestImmediateTbl"));
            ExecDML(OTEXT("drop type TestImmediateColl"));
            ExecDML(OTEXT("drop type TestImmediateObj"));
            ExecDML(OTEXT("create type TestImmediateObj as object(code int, name varchar2(30))"));
            ExecDML(OTEXT("create type TestImmediateColl as varray(4) of number"));
            ExecDML(OTEXT("create table TestImmediateTbl of TestImmediateObj"));
            ExecDML(OTEXT("insert into TestImmediateTbl values (TestImmediateObj(7, 'seven'))"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestImmediateTbl"));
            ExecDML(OTEXT("drop type TestImmediateColl"));
            ExecDML(OTEXT("drop type TestImmediateObj"));
        }

        void SetUp() override
        {
            env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, con);
        }

        void TearDown() override
        {
            OCI_ConnectionFree(con);
        }

        OCI_Environment* env{ nullptr };
        OCI_Connection*  con{ nullptr };
    };

    TEST_F(TestStatementImmediate, AllArgTypes)
    {
        const auto typinf_obj  = OCI_TypeInfoGet(con, OTEXT("TestImmediateObj"), OCI_TIF_TYPE);
        const auto typinf_coll = OCI_TypeInfoGet(con, OTEXT("TestImmediateColl"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf_obj);
        ASSERT_NE(nullptr, typinf_coll);

        otext  txt[64] = {};
        OCI_Int16  sh  = 0;
        OCI_UInt16 ush = 0;
        OCI_Int32    i   = 0;
        OCI_UInt32 ui = 0;
        OCI_Int64  bi  = 0;
        OCI_UInt64 bui = 0;
        OCI_Double   dbl = 0.0;
        OCI_Float    flt = 0.0f;
        unsigned char raw[64] = {};

        const auto num  = OCI_NumberCreate(env);
        const auto dt   = OCI_DateCreate(env);
        const auto lob  = OCI_LobCreate(con, OCI_CLOB);
        const auto file = OCI_FileCreate(con, OCI_BFILE);
        const auto ts   = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        const auto iv   = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        const auto obj  = OCI_ObjectCreate(typinf_obj);
        const auto coll = OCI_CollCreate(typinf_coll);
        const auto ref  = OCI_RefCreate(typinf_obj);
        ASSERT_NE(nullptr, num);
        ASSERT_NE(nullptr, dt);
        ASSERT_NE(nullptr, lob);
        ASSERT_NE(nullptr, file);
        ASSERT_NE(nullptr, ts);
        ASSERT_NE(nullptr, iv);
        ASSERT_NE(nullptr, obj);
        ASSERT_NE(nullptr, coll);
        ASSERT_NE(nullptr, ref);

        ASSERT_TRUE(OCI_Immediate(con,
            OTEXT("select 'abc', cast(1 as number(4)), cast(2 as number(4)), "
                  "cast(3 as number(9)), cast(4 as number(9)), "
                  "cast(5 as number(19)), cast(6 as number(19)), "
                  "cast(1.5 as binary_double), cast(2.5 as binary_float), "
                  "cast(9 as number), sysdate, hextoraw('AABB'), "
                  "to_clob('lobdata'), bfilename('FAKE_DIR', 'f.txt'), "
                  "systimestamp, interval '1 02:03:04' day to second, "
                  "TestImmediateObj(1, 'o'), TestImmediateColl(1, 2), ref(t) "
                  "from dual, TestImmediateTbl t"),
            OCI_ARG_TEXT, txt,
            OCI_ARG_SHORT, &sh,
            OCI_ARG_USHORT, &ush,
            OCI_ARG_INT, &i,
            OCI_ARG_UINT, &ui,
            OCI_ARG_BIGINT, &bi,
            OCI_ARG_BIGUINT, &bui,
            OCI_ARG_DOUBLE, &dbl,
            OCI_ARG_FLOAT, &flt,
            OCI_ARG_NUMBER, num,
            OCI_ARG_DATETIME, dt,
            OCI_ARG_RAW, raw,
            OCI_ARG_LOB, lob,
            OCI_ARG_FILE, file,
            OCI_ARG_TIMESTAMP, ts,
            OCI_ARG_INTERVAL, iv,
            OCI_ARG_OBJECT, obj,
            OCI_ARG_COLLECTION, coll,
            OCI_ARG_REF, ref));

        ASSERT_EQ(ostring(OTEXT("abc")), ostring(txt));
        ASSERT_EQ(1, sh);
        ASSERT_EQ(2, ush);
        ASSERT_EQ(3, i);
        ASSERT_EQ(4u, ui);
        ASSERT_EQ(5, bi);
        ASSERT_EQ(6u, bui);
        ASSERT_GT(dbl, 0.0);
        ASSERT_GT(flt, 0.0f);
        ASSERT_GT(OCI_LobGetLength(lob), 0u);

        ASSERT_TRUE(OCI_NumberFree(num));
        ASSERT_TRUE(OCI_DateFree(dt));
        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_FileFree(file));
        ASSERT_TRUE(OCI_TimestampFree(ts));
        ASSERT_TRUE(OCI_IntervalFree(iv));
        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_CollFree(coll));
        ASSERT_TRUE(OCI_RefFree(ref));
    }

    TEST_F(TestStatementImmediate, TextWithSizeArg)
    {
        otext txtn[8] = {};

        // size-carrying text arg
        ASSERT_TRUE(OCI_Immediate(con, OTEXT("select 'bounded' from dual"),
                                  OCI_ARG_TEXT_N, txtn, (OCI_UInt32) 8));
        ASSERT_EQ(ostring(OTEXT("bounded")), ostring(txtn));
    }

    TEST_F(TestStatementImmediate, JsonArg)
    {
        const auto json = OCI_JsonCreate(con);
        ASSERT_NE(nullptr, json);

        ASSERT_TRUE(OCI_Immediate(con,
            OTEXT("select json('{\"a\":1}') from dual"),
            OCI_ARG_JSON, json));

        OCI_UInt32 jsize = 0;
        ASSERT_TRUE(OCI_JsonToText(json, nullptr, &jsize, 0, FALSE));
        ASSERT_GT(jsize, 0u);

        ASSERT_TRUE(OCI_JsonFree(json));
    }

    TEST_F(TestStatementImmediate, VectorArg)
    {
        const auto vect = OCI_VectorCreate(con);
        ASSERT_NE(nullptr, vect);

        ASSERT_TRUE(OCI_Immediate(con,
            OTEXT("select vector('[1.5,2.5,3.5]', 3, float32) from dual"),
            OCI_ARG_VECTOR, vect));

        OCI_UInt32 dimensions = 0;
        OCI_UInt32 format     = OCI_VEC_FLEX;
        ASSERT_TRUE(OCI_VectorGetInfo(vect, &format, &dimensions));
        ASSERT_EQ(3u, dimensions);
        ASSERT_EQ(OCI_VEC_FLOAT32, format);

        ASSERT_TRUE(OCI_VectorFree(vect));
    }

    TEST_F(TestStatementImmediate, UnknownArgTag)
    {
        OCI_Int32 dummy = 0;

        // unknown arg tag -> mapping error
        ASSERT_FALSE(OCI_Immediate(con, OTEXT("select 1 from dual"), 4242, &dummy));
    }

    /* --- PL/SQL boolean and cursor binds --- */

    class TestStatementPlsqlBinds : public ::testing::Test
    {
    protected:

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, con);

            stmt = OCI_StatementCreate(con);
            ASSERT_NE(nullptr, stmt);

            sub = OCI_StatementCreate(con);
            ASSERT_NE(nullptr, sub);

            ASSERT_TRUE(OCI_Prepare(stmt,
                OTEXT("begin :b := true; open :c for select 1 as n from dual; end;")));
            ASSERT_NE(nullptr, OCI_BindBoolean(stmt, OTEXT(":b"), &b));
            ASSERT_NE(nullptr, OCI_BindStatement(stmt, OTEXT(":c"), sub));
        }

        void TearDown() override
        {
            OCI_StatementFree(sub);
            OCI_StatementFree(stmt);
            OCI_ConnectionFree(con);
        }

        OCI_Connection* con{ nullptr };
        OCI_Statement*  stmt{ nullptr };
        OCI_Statement*  sub{ nullptr };
        OCI_Boolean     b{ FALSE };
    };

    TEST_F(TestStatementPlsqlBinds, BooleanAndCursorBinds)
    {
        ASSERT_TRUE(OCI_Execute(stmt));
        ASSERT_TRUE(b);

        const auto rs = OCI_GetResultset(sub);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(1, OCI_GetInt(rs, 1));
    }

    TEST_F(TestStatementPlsqlBinds, ReexecuteResetsBoundStatement)
    {
        // second execute resets the bound statement (ALLOCATED_BIND_STMT path)
        ASSERT_TRUE(OCI_Execute(stmt));
        ASSERT_TRUE(OCI_Execute(stmt));
    }

    /* --- complex bind arrays (file/ref/coll/vector) --- */

    class TestStatementComplexArrays : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("drop table TestStmtCArrTbl"));
            ExecDML(OTEXT("drop type TestStmtCArrColl"));
            ExecDML(OTEXT("drop type TestStmtCArrObj"));
            ExecDML(OTEXT("create type TestStmtCArrObj as object(code int)"));
            ExecDML(OTEXT("create type TestStmtCArrColl as varray(4) of number"));
            ExecDML(OTEXT("create table TestStmtCArrTbl (id int primary key, o TestStmtCArrObj)"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestStmtCArrTbl"));
            ExecDML(OTEXT("drop type TestStmtCArrColl"));
            ExecDML(OTEXT("drop type TestStmtCArrObj"));
        }

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, con);

            stmt = OCI_StatementCreate(con);
            ASSERT_NE(nullptr, stmt);
        }

        void TearDown() override
        {
            OCI_StatementFree(stmt);
            OCI_ConnectionFree(con);
        }

        void PrepareArrayBind(const otext* sql)
        {
            ASSERT_TRUE(OCI_Prepare(stmt, sql));
            ASSERT_TRUE(OCI_BindArraySetSize(stmt, 2));
        }

        OCI_Connection* con{ nullptr };
        OCI_Statement*  stmt{ nullptr };
    };

    TEST_F(TestStatementComplexArrays, BindFile)
    {
        PrepareArrayBind(OTEXT("begin :f := :f; end;"));

        const auto file = OCI_FileCreate(con, OCI_BFILE);
        ASSERT_NE(nullptr, file);
        ASSERT_NE(nullptr, OCI_BindFile(stmt, OTEXT(":f"), file));

        ASSERT_TRUE(OCI_FileFree(file));
    }

    TEST_F(TestStatementComplexArrays, BindArrayOfFiles)
    {
        PrepareArrayBind(OTEXT("begin :fa := :fa; end;"));

        const auto files = OCI_FileArrayCreate(con, OCI_BFILE, 2);
        ASSERT_NE(nullptr, files);
        ASSERT_NE(nullptr, OCI_BindArrayOfFiles(stmt, OTEXT(":fa"), files, OCI_BFILE, 0));

        ASSERT_TRUE(OCI_FileArrayFree(files));
    }

    TEST_F(TestStatementComplexArrays, BindArrayOfRefs)
    {
        PrepareArrayBind(OTEXT("begin :r := :r; end;"));

        const auto typinf = OCI_TypeInfoGet(con, OTEXT("TestStmtCArrObj"), OCI_TIF_TYPE);
        const auto refs   = OCI_RefArrayCreate(typinf, 2);
        ASSERT_NE(nullptr, refs);
        ASSERT_NE(nullptr, OCI_BindArrayOfRefs(stmt, OTEXT(":r"), refs, typinf, 0));

        ASSERT_TRUE(OCI_RefArrayFree(refs));
    }

    TEST_F(TestStatementComplexArrays, BindArrayOfColls)
    {
        PrepareArrayBind(OTEXT("begin :c := :c; end;"));

        const auto typinf = OCI_TypeInfoGet(con, OTEXT("TestStmtCArrColl"), OCI_TIF_TYPE);
        const auto colls  = OCI_CollArrayCreate(typinf, 2);
        ASSERT_NE(nullptr, colls);
        ASSERT_NE(nullptr, OCI_BindArrayOfColls(stmt, OTEXT(":c"), colls, typinf, 0));

        ASSERT_TRUE(OCI_CollArrayFree(colls));
    }

    TEST_F(TestStatementComplexArrays, BindArrayOfVectors)
    {
        PrepareArrayBind(OTEXT("begin :v := :v; end;"));

        const auto vects = OCI_VectorArrayCreate(con, 2);
        ASSERT_NE(nullptr, vects);
        ASSERT_NE(nullptr, OCI_BindArrayOfVectors(stmt, OTEXT(":v"), vects, 0));

        ASSERT_TRUE(OCI_VectorArrayFree(vects));
    }

    /* --- bind error paths --- */

    class TestStatementBindErrors : public ::testing::Test
    {
    protected:

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, con);

            stmt = OCI_StatementCreate(con);
            ASSERT_NE(nullptr, stmt);
        }

        void TearDown() override
        {
            OCI_StatementFree(stmt);
            OCI_ConnectionFree(con);
        }

        OCI_Connection* con{ nullptr };
        OCI_Statement*  stmt{ nullptr };
    };

    TEST_F(TestStatementBindErrors, ArraySizeCannotGrowWithExistingBinds)
    {
        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into t values (:x)")));

        ASSERT_TRUE(OCI_BindArraySetSize(stmt, 5));
        OCI_Int32 i = 1;
        ASSERT_NE(nullptr, OCI_BindInt(stmt, OTEXT(":x"), &i));
        ASSERT_FALSE(OCI_BindArraySetSize(stmt, 10));
    }

    TEST_F(TestStatementBindErrors, NullStringDataWithInternalAlloc)
    {
        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into t values (:s)")));

        // NULL data with internal allocation and no usable length -> error
        ASSERT_TRUE(OCI_SetBindAllocation(stmt, OCI_BAM_INTERNAL));
        ASSERT_EQ(nullptr, OCI_BindString(stmt, OTEXT(":s"), NULL, 0));
    }

    TEST_F(TestStatementBindErrors, NullRawDataWithInternalAlloc)
    {
        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into t values (:r)")));

        ASSERT_TRUE(OCI_SetBindAllocation(stmt, OCI_BAM_INTERNAL));
        ASSERT_EQ(nullptr, OCI_BindRaw(stmt, OTEXT(":r"), NULL, 0));
    }

    TEST_F(TestStatementBindErrors, GetBindByNameUnknown)
    {
        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into t values (:x)")));
        ASSERT_EQ(nullptr, OCI_GetBindByName(stmt, OTEXT(":nope")));
    }

    /* --- fetch mode on a legacy (9iR1) server version --- */

    class TestStatementLegacyFetchMode : public ::testing::Test
    {
    protected:

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, con);

            stmt = OCI_StatementCreate(con);
            ASSERT_NE(nullptr, stmt);

            // con->ver_num == 9iR1 toggles prefetch handling in SetFetchMode()
            old_ver = OCI_TestConSetServerVersion(con, OCI_9_0);
        }

        void TearDown() override
        {
            OCI_TestConSetServerVersion(con, old_ver);
            OCI_StatementFree(stmt);
            OCI_ConnectionFree(con);
        }

        OCI_Connection* con{ nullptr };
        OCI_Statement*  stmt{ nullptr };
        OCI_UInt32    old_ver{ 0 };
    };

    TEST_F(TestStatementLegacyFetchMode, FetchModeTogglesPrefetch)
    {
        ASSERT_TRUE(OCI_SetFetchMode(stmt, OCI_SFM_SCROLLABLE));
        ASSERT_EQ(0u, OCI_GetPrefetchSize(stmt));
        ASSERT_TRUE(OCI_SetFetchMode(stmt, OCI_SFM_DEFAULT));
        ASSERT_EQ((OCI_UInt32) OCI_PREFETCH_SIZE, OCI_GetPrefetchSize(stmt));
    }

    TEST_F(TestStatementLegacyFetchMode, ScrollableForcesPrefetchOff)
    {
        // scrollable + prefetch on 9iR1 forces prefetch off
        ASSERT_TRUE(OCI_SetFetchMode(stmt, OCI_SFM_SCROLLABLE));
        ASSERT_TRUE(OCI_SetPrefetchSize(stmt, 50));
        ASSERT_EQ(0u, OCI_GetPrefetchSize(stmt));
    }

    TEST(TestStatement, LegacyClientPrepareAndExecute)
    {
        // env->version_runtime < 9.2: statement handle is allocated and
        // OCIStmtPrepare() (not Prepare2) is used; OCIStmtRelease skipped.
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        const auto old_ver = OCI_TestEnvSetVersionRuntime(con, OCI_9_0);

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select 1 as n from dual")));
        ASSERT_TRUE(OCI_Execute(stmt));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(1, OCI_GetInt(rs, 1));

        // freeing under < 9.2 uses OcilibMemoryFreeHandle instead of OCIStmtRelease
        ASSERT_TRUE(OCI_StatementFree(stmt));

        OCI_TestEnvSetVersionRuntime(con, old_ver);

        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    /* --- autocommit and batch errors --- */

    class TestStatementBatch : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("drop table TestStatementBatch"));
            ExecDML(OTEXT("create table TestStatementBatch (id int not null)"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestStatementBatch"));
        }

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, con);

            stmt = OCI_StatementCreate(con);
            ASSERT_NE(nullptr, stmt);
        }

        void TearDown() override
        {
            OCI_StatementFree(stmt);
            OCI_ConnectionFree(con);
        }

        OCI_Connection* con{ nullptr };
        OCI_Statement*  stmt{ nullptr };
    };

    TEST_F(TestStatementBatch, AutoCommitOnExecute)
    {
        // autocommit path on DML execute
        ASSERT_TRUE(OCI_SetAutoCommit(con, TRUE));
        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("insert into TestStatementBatch values (1)")));
        ASSERT_TRUE(OCI_SetAutoCommit(con, FALSE));
    }

    TEST_F(TestStatementBatch, BatchErrorList)
    {
        // array DML with one failing row -> batch error list
        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into TestStatementBatch values (:x)")));

        ASSERT_TRUE(OCI_BindArraySetSize(stmt, 3));
        OCI_Int32 vals[3] = { 2, 0, 3 };
        ASSERT_NE(nullptr, OCI_BindArrayOfInts(stmt, OTEXT(":x"), vals, 0));
        // NULL indicator on the second element
        ASSERT_TRUE(OCI_BindSetNullAtPos(OCI_GetBind(stmt, 1), 1));

        ASSERT_FALSE(OCI_Execute(stmt));
        ASSERT_GT(OCI_GetBatchErrorCount(stmt), 0u);

        const auto err = OCI_GetBatchError(stmt);
        ASSERT_NE(nullptr, err);
        ASSERT_EQ(nullptr, OCI_GetBatchError(stmt));
    }

    TEST(TestStatement, RegisterOutBindsFetch)
    {
        ExecDML(OTEXT("drop table TestStatementRet"));
        ExecDML(OTEXT("create table TestStatementRet (id int, v varchar2(50), d date, l nclob)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_Prepare(stmt,
            OTEXT("insert into TestStatementRet values (1, 'hello', sysdate, to_nclob('nval')) "
                  "returning v, id, d, l, id into :v, :i, :d, :l, :u")));

        ASSERT_TRUE(OCI_RegisterString(stmt, OTEXT(":v"), 50));
        ASSERT_TRUE(OCI_RegisterInt(stmt, OTEXT(":i")));

        // server < 10.2 registers DATE as SQLT_DAT (Oracle bug #3269146)
        const auto old_ver = OCI_TestEnvSetVersionRuntime(con, OCI_10_1);
        ASSERT_TRUE(OCI_RegisterDate(stmt, OTEXT(":d")));
        OCI_TestEnvSetVersionRuntime(con, old_ver);

        ASSERT_TRUE(OCI_RegisterLob(stmt, OTEXT(":l"), OCI_NCLOB));
        ASSERT_TRUE(OCI_RegisterUnsignedBigInt(stmt, OTEXT(":u")));

        ASSERT_TRUE(OCI_Execute(stmt));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        ASSERT_EQ(ostring(OTEXT("hello")), ostring(OCI_GetString(rs, 1)));
        ASSERT_EQ(1, OCI_GetInt(rs, 2));
        ASSERT_NE(nullptr, OCI_GetDate(rs, 3));
        ASSERT_NE(nullptr, OCI_GetLob(rs, 4));
        ASSERT_EQ(1u, OCI_GetUnsignedBigInt(rs, 5));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("drop table TestStatementRet"));
    }

    TEST(TestStatement, PrefetchMemoryOnHandle)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select 1 from dual")));

        // stmt->handle is allocated after prepare: hits the OCI_ATTR_PREFETCH_MEMORY set
        ASSERT_TRUE(OCI_SetPrefetchMemory(stmt, 4096));
        ASSERT_EQ(4096u, OCI_GetPrefetchMemory(stmt));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }
}
