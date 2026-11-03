#include "ocilib_tests.h"

namespace TestCApi
{
    TEST(TestStringutils, GetStringConversions)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_SetFormat(env, OCI_FMT_NUMERIC, OTEXT("FM999999")));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select 'hello' as str, 123 as num from dual")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        ASSERT_EQ(ostring(OTEXT("hello")), ostring(OCI_GetString(rs, 1)));
        ASSERT_EQ(ostring(OTEXT("hello")), ostring(OCI_GetStringByName(rs, OTEXT("STR"))));

        ASSERT_EQ(ostring(OTEXT("123")), ostring(OCI_GetString(rs, 2)));
        ASSERT_EQ(ostring(OTEXT("123")), ostring(OCI_GetStringByName(rs, OTEXT("NUM"))));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestStringutils, NumberAndDateToString)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select 123456 as n, to_date('2020-05-12', 'YYYY-MM-DD') as d, 3.14 as f from dual")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        ASSERT_EQ(ostring(OTEXT("123456")), ostring(OCI_GetString(rs, 1)));
        ASSERT_EQ(ostring(OTEXT("2020-05-12")), ostring(OCI_GetString(rs, 2)));
        ASSERT_NE(nullptr, OCI_GetString(rs, 3));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestStringutils, UnicodeAndRawConversions)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select 'hello' as s, UTL_RAW.CAST_TO_RAW('world') as r, cast('test' as nvarchar2(50)) as n from dual")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        ASSERT_EQ(ostring(OTEXT("hello")), ostring(OCI_GetString(rs, 1)));
        ASSERT_TRUE(nullptr != OCI_GetString(rs, 2));
        ASSERT_EQ(ostring(OTEXT("test")), ostring(OCI_GetString(rs, 3)));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestStringutils, GetStringForNumericSubtypes)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select cast(12 as number(4)) as s, cast(12345 as number(10)) as i, cast(3.14 as binary_float) as f, cast(3.14 as binary_double) as d from dual")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        ASSERT_EQ(ostring(OTEXT("12")), ostring(OCI_GetString(rs, 1)));
        ASSERT_EQ(ostring(OTEXT("12345")), ostring(OCI_GetString(rs, 2)));
        ASSERT_NE(nullptr, OCI_GetString(rs, 3));
        ASSERT_NE(nullptr, OCI_GetString(rs, 4));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestStringutils, BindAndGetString)
    {
        ExecDML(OTEXT("drop table TestStringutilsData"));
        ExecDML(OTEXT("create table TestStringutilsData (value varchar2(50))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto insertStmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, insertStmt);

        otext value[50] = OTEXT("bound value");
        ASSERT_TRUE(OCI_Prepare(insertStmt, OTEXT("insert into TestStringutilsData values (:1)")));
        ASSERT_NE(nullptr, OCI_BindString(insertStmt, OTEXT(":1"), value, 49));
        ASSERT_TRUE(OCI_Execute(insertStmt));

        ASSERT_TRUE(OCI_StatementFree(insertStmt));

        const auto selectStmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, selectStmt);

        ASSERT_TRUE(OCI_ExecuteStmt(selectStmt, OTEXT("select value from TestStringutilsData")));

        const auto rs = OCI_GetResultset(selectStmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        ASSERT_EQ(ostring(OTEXT("bound value")), ostring(OCI_GetString(rs, 1)));

        ASSERT_TRUE(OCI_StatementFree(selectStmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestStringutilsData"));
    }

    /* --- public-API coverage of the dropped OcilibString* internals:
       OCI_ParseFmt / OCI_GetString / OCI_ColumnGetSQLType /
       OCI_ColumnGetFullSQLType exercise OcilibStringAddToBuffer /
       OcilibStringRequestBuffer / OcilibStringGetTypeName /
       OcilibStringGetFullTypeName / OcilibStringTranslate ---
       through the public surface --- */

    TEST(TestStringutils, ParseFmtStringAppendBuffer)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        /* OCI_ParseFmt formats SQL values -> OcilibParseFmt +
           OcilibStringAddToBuffer / OcilibStringRequestBuffer paths */
        ASSERT_TRUE(OCI_ParseFmt(stmt, OTEXT("select %i as n, %s as s from dual"), 42, OTEXT("test")));
        ASSERT_TRUE(OCI_Execute(stmt));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(42, OCI_GetInt(rs, 1));
        ASSERT_EQ(ostring(OTEXT("test")), ostring(OCI_GetString(rs, 2)));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestStringutils, ColumnGetSqlTypeNames)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ExecDML(OTEXT("drop table TestStrTypeTbl"));
        ExecDML(OTEXT("create table TestStrTypeTbl (num number, str varchar2(30), rawc raw(8), dt date)"));

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);
        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select * from TestStrTypeTbl")));
        ASSERT_TRUE(OCI_Execute(stmt));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        /* OcilibStringGetTypeName / OcilibStringGetFullTypeName via public
           column getters */
        OCI_Column* colNum = OCI_GetColumn(rs, 1);
        OCI_Column* colStr = OCI_GetColumn(rs, 2);
        ASSERT_NE(nullptr, colNum);
        ASSERT_NE(nullptr, colStr);
        ASSERT_NE(nullptr, OCI_ColumnGetSQLType(colNum));
        ASSERT_NE(nullptr, OCI_ColumnGetSQLType(colStr));

        otext fullBuf[128] = { 0 };
        ASSERT_NE(0u, OCI_ColumnGetFullSQLType(colNum, fullBuf, (OCI_UInt32)sizeof(fullBuf) / sizeof(otext)));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ExecDML(OTEXT("drop table TestStrTypeTbl"));
    }
}
