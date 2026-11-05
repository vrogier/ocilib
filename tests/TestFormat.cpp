#include "ocilib_tests.h"

/* OCILIB uses its own %i %s %t %n %m ... format tokens, not printf — silence GCC */
#if defined(__GNUC__)
    #pragma GCC diagnostic ignored "-Wformat"
    #pragma GCC diagnostic ignored "-Wformat-extra-args"
    #pragma GCC diagnostic ignored "-Wformat-overflow"
#endif

namespace TestCApi
{
    struct FormatSetGetCase
    {
        OCI_FormatType type;
        const otext*   format;
    };

    class TestFormatSetGet : public ::testing::TestWithParam<FormatSetGetCase> {};

    TEST_P(TestFormatSetGet, SetAndGet)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ASSERT_TRUE(OCI_SetFormat(env, GetParam().type, GetParam().format));
        ASSERT_EQ(String(GetParam().format), String(OCI_GetFormat(env, GetParam().type)));
    }

    INSTANTIATE_TEST_CASE_P(FormatTypes, TestFormatSetGet,
        ::testing::Values(
            FormatSetGetCase{ OCI_FMT_NUMERIC,   OTEXT("999,999.99") },
            FormatSetGetCase{ OCI_FMT_DATE,      OTEXT("YYYY-MM-DD") },
            FormatSetGetCase{ OCI_FMT_TIMESTAMP, OTEXT("YYYY-MM-DD HH24:MI:SS.FF") }));

    /* --- shared statement fixture for token tests --- */

    class TestFormatTokens : public ::testing::Test
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

    TEST(TestFormat, ExecuteStmtFmt)
    {
        ExecDML(OTEXT("create table TestFormatTable (id int, name varchar2(50), d date, val number)"));

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        const auto date = OCI_DateCreate(env);
        const auto num = OCI_NumberCreate(env);

        ASSERT_NE(nullptr, date);
        ASSERT_NE(nullptr, num);

        OCI_Int32 n = 12345;

        ASSERT_TRUE(OCI_DateSetDateTime(date, 2020, 5, 12, 10, 20, 30));
        ASSERT_TRUE(OCI_NumberSetValue(num, OCI_NUM_INT, &n));

        ASSERT_TRUE(OCI_SetFormat(env, OCI_FMT_NUMERIC, OTEXT("FM9999999999")));

        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt,
            OTEXT("insert into TestFormatTable values (%i, %s, %t, %n)"),
            1, OTEXT("hello"), date, num));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select count(*) from TestFormatTable")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(1, OCI_GetInt(rs, 1));

        ASSERT_TRUE(OCI_NumberFree(num));
        ASSERT_TRUE(OCI_DateFree(date));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("drop table TestFormatTable"));
    }

    /* --- numeric tokens --- */

    TEST_F(TestFormatTokens, TokenSignedInt)
    {
        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select %i, %hi from dual"), -5, -7));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(-5, OCI_GetInt(rs, 1));
        ASSERT_EQ(-7, OCI_GetInt(rs, 2));
    }

    TEST_F(TestFormatTokens, TokenUnsignedInt)
    {
        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select %u, %hu from dual"), 42u, 8u));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(42, OCI_GetInt(rs, 1));
        ASSERT_EQ(8, OCI_GetInt(rs, 2));
    }

    TEST_F(TestFormatTokens, TokenBigInt)
    {
        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select %li, %lu from dual"),
            static_cast<OCI_Int64>(-1234567890123), static_cast<OCI_UInt64>(1234567890123)));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(-1234567890123, OCI_GetBigInt(rs, 1));
        ASSERT_EQ((OCI_UInt64)1234567890123, OCI_GetUnsignedBigInt(rs, 2));
    }

    TEST_F(TestFormatTokens, TokenDouble)
    {
        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select %g from dual"), 1.5));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(1.5, OCI_GetDouble(rs, 1));
    }

    /* --- string / null / literal tokens --- */

    TEST_F(TestFormatTokens, TokenStringAddsQuotes)
    {
        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select %s from dual"), OTEXT("hello")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(String(OTEXT("hello")), String(OCI_GetString(rs, 1)));
    }

    TEST_F(TestFormatTokens, TokenRawMode)
    {
        /* %m stays raw */

        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select %m from dual"), OTEXT("1+1")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(2, OCI_GetInt(rs, 1));
    }

    TEST_F(TestFormatTokens, TokenNullString)
    {
        /* NULL %s writes NULL */

        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select %s from dual"), static_cast<const otext*>(NULL)));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_TRUE(OCI_IsNull(rs, 1));
    }

    TEST_F(TestFormatTokens, TokenLiteralPercent)
    {
        /* %% emits a literal percent */

        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select 'x%%y' from dual")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(String(OTEXT("x%y")), String(OCI_GetString(rs, 1)));
    }

    /* --- handle tokens --- */

    class TestFormatHandles : public TestFormatTokens
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("create or replace type TestFormatRefType as object(id number)"));
            ExecDML(OTEXT("drop table TestFormatRefTab"));
            ExecDML(OTEXT("create table TestFormatRefTab of TestFormatRefType"));
            ExecDML(OTEXT("insert into TestFormatRefTab values (TestFormatRefType(1))"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestFormatRefTab"));
            ExecDML(OTEXT("drop type TestFormatRefType"));
        }

        void SetUp() override
        {
            TestFormatTokens::SetUp();

            const auto env = GetEnvironment(nullptr);

            date = OCI_DateCreate(env);
            tmsp = OCI_TimestampCreate(env, OCI_TIMESTAMP);
            itv  = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
            num  = OCI_NumberCreate(env);

            ASSERT_NE(nullptr, date);
            ASSERT_NE(nullptr, tmsp);
            ASSERT_NE(nullptr, itv);
            ASSERT_NE(nullptr, num);

            OCI_Int32 n = 7;
            ASSERT_TRUE(OCI_DateSetDateTime(date, 2020, 5, 12, 10, 20, 30));
            ASSERT_TRUE(OCI_TimestampConstruct(tmsp, 2021, 6, 15, 8, 30, 45, 0, nullptr));
            ASSERT_TRUE(OCI_IntervalSetDaySecond(itv, 0, 1, 2, 3, 0));
            ASSERT_TRUE(OCI_NumberSetValue(num, OCI_NUM_INT, &n));
        }

        void TearDown() override
        {
            OCI_NumberFree(num);
            OCI_IntervalFree(itv);
            OCI_TimestampFree(tmsp);
            OCI_DateFree(date);
            TestFormatTokens::TearDown();
        }

        OCI_Date*      date{ nullptr };
        OCI_Timestamp* tmsp{ nullptr };
        OCI_Interval*  itv{ nullptr };
        OCI_Number*    num{ nullptr };
    };

    TEST_F(TestFormatHandles, TokenDate)
    {
        /* %t -> to_date() */

        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select %t from dual"), date));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_FALSE(OCI_IsNull(rs, 1));
    }

    TEST_F(TestFormatHandles, TokenTimestamp)
    {
        /* %p -> to_timestamp() */

        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select %p from dual"), tmsp));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_FALSE(OCI_IsNull(rs, 1));
    }

    TEST_F(TestFormatHandles, TokenInterval)
    {
        /* %v -> interval text */

        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select to_dsinterval('%v') from dual"), itv));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_FALSE(OCI_IsNull(rs, 1));
    }

    TEST_F(TestFormatHandles, TokenRef)
    {
        /* %r -> hex refid; fetch a real REF first */

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select ref(t) from TestFormatRefTab t")));
        const auto refrs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, refrs);
        ASSERT_TRUE(OCI_FetchNext(refrs));

        const auto ref = OCI_GetRef(refrs, 1);
        ASSERT_NE(nullptr, ref);

        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select '%r' from dual"), ref));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_FALSE(OCI_IsNull(rs, 1));
    }

    TEST_F(TestFormatHandles, TokenNumber)
    {
        /* %n -> number text */

        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select %n from dual"), num));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(7, OCI_GetInt(rs, 1));
    }

    TEST_F(TestFormatHandles, TokenNullDate)
    {
        /* NULL handle arm: %t writes NULL */

        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select %t from dual"), static_cast<OCI_Date*>(NULL)));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_TRUE(OCI_IsNull(rs, 1));
    }

    TEST_F(TestFormatHandles, TokenNullTimestamp)
    {
        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select %p from dual"), static_cast<OCI_Timestamp*>(NULL)));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_TRUE(OCI_IsNull(rs, 1));
    }

    TEST_F(TestFormatHandles, TokenNullInterval)
    {
        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select %v from dual"), static_cast<OCI_Interval*>(NULL)));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_TRUE(OCI_IsNull(rs, 1));
    }

    TEST_F(TestFormatHandles, TokenNullRef)
    {
        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select %r from dual"), static_cast<OCI_Ref*>(NULL)));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_TRUE(OCI_IsNull(rs, 1));
    }

    TEST_F(TestFormatHandles, TokenJson)
    {
        /* %j -> quoted JSON text literal */

        const auto json = OCI_JsonCreate(con);
        ASSERT_NE(nullptr, json);

        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("{\"a\":1}"), 0, OCI_JTV_ALLOW_NONE, OCI_JTE_DETECT));

        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select %j from dual"), json));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_FALSE(OCI_IsNull(rs, 1));

        ASSERT_TRUE(OCI_JsonFree(json));
    }

    TEST_F(TestFormatHandles, TokenVector)
    {
        /* %V -> quoted '[...]' vector text literal */

        const auto vect = OCI_VectorCreate(con);
        ASSERT_NE(nullptr, vect);

        char values[4] = { 1, 2, 3, 4 };
        ASSERT_TRUE(OCI_VectorSetValues(vect, OCI_VEC_INT8, 4, values));

        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select to_vector(%V) from dual"), vect));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_FALSE(OCI_IsNull(rs, 1));

        ASSERT_TRUE(OCI_VectorFree(vect));
    }

    TEST_F(TestFormatHandles, TokenNullJson)
    {
        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select %j from dual"), static_cast<OCI_Json*>(NULL)));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_TRUE(OCI_IsNull(rs, 1));
    }

    TEST_F(TestFormatHandles, TokenNullVector)
    {
        ASSERT_TRUE(OCI_ExecuteStmtFmt(stmt, OTEXT("select %V from dual"), static_cast<OCI_Vector*>(NULL)));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_TRUE(OCI_IsNull(rs, 1));
    }

    /* --- invalid / unknown tokens --- */

    TEST_F(TestFormatTokens, UnknownTokenRejected)
    {
        ASSERT_FALSE(OCI_PrepareFmt(stmt, OTEXT("select %z from dual"), 0));
        ASSERT_NE(nullptr, OCI_GetLastError());
    }

    TEST_F(TestFormatTokens, UnimplementedTokenObject)
    {
        /* documented but unimplemented token hits the same arm */

        ASSERT_FALSE(OCI_PrepareFmt(stmt, OTEXT("select %o from dual"), NULL));
    }

    TEST_F(TestFormatTokens, UnimplementedTokenColl)
    {
        ASSERT_FALSE(OCI_PrepareFmt(stmt, OTEXT("select %c from dual"), NULL));
    }

    TEST_F(TestFormatTokens, TrailingPercentRejected)
    {
        ASSERT_FALSE(OCI_PrepareFmt(stmt, OTEXT("select 1 from dual where x = %")));
    }

    TEST_F(TestFormatTokens, UnknownLengthSuffixExpandsEmpty)
    {
        /* %l / %h with unknown suffix expand to nothing (no parse error),
           leaving invalid SQL that fails when actually parsed */

        ASSERT_FALSE(OCI_ParseFmt(stmt, OTEXT("select 1 from dual where dummy = %lx and 1 = 1")));
        ASSERT_FALSE(OCI_ParseFmt(stmt, OTEXT("select 1 from dual where dummy = %hx and 1 = 1")));
    }
}
