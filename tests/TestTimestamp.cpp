#include "ocilib_tests.h"
#include "../include/ocilib.h"

class TestTimestamp : public ::testing::TestWithParam<OCI_UInt32>{};

static std::vector<OCI_UInt32> TimestampTypes{ OCI_TIMESTAMP, OCI_TIMESTAMP_TZ, OCI_TIMESTAMP_LTZ };

namespace TestCApi
{

    TEST_P(TestTimestamp, CreateAndFree)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto tmsp = OCI_TimestampCreate(env, GetParam());
        ASSERT_EQ(GetParam(), OCI_TimestampGetType(tmsp));
        ASSERT_TRUE(OCI_TimestampFree(tmsp));

    }

    TEST_P(TestTimestamp, ArrayCreateAndFree)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto tmsps = OCI_TimestampArrayCreate(env, GetParam(), ARRAY_SIZE);
        ASSERT_TRUE(nullptr != tmsps);

        for (OCI_Int32 i = 0; i < ARRAY_SIZE; i++)
        {
            ASSERT_TRUE(nullptr != tmsps[i]);
            ASSERT_EQ(GetParam(), OCI_TimestampGetType(tmsps[i]));
        }
        ASSERT_TRUE(OCI_TimestampArrayFree(tmsps));

    }

    TEST_P(TestTimestamp, Assign)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto tmsp1 = OCI_TimestampCreate(env, GetParam());
        const auto tmsp2 = OCI_TimestampCreate(env, GetParam());

        ASSERT_TRUE(OCI_TimestampSysTimestamp(tmsp1));
        ASSERT_TRUE(OCI_TimestampAssign(tmsp2, tmsp1));
        ASSERT_EQ(0, OCI_TimestampCompare(tmsp1, tmsp2));

        ASSERT_TRUE(OCI_TimestampFree(tmsp1));
        ASSERT_TRUE(OCI_TimestampFree(tmsp2));

    }

    TEST_P(TestTimestamp, CompareInferiorSuperior)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto tmsp1 = OCI_TimestampCreate(env, GetParam());
        const auto tmsp2 = OCI_TimestampCreate(env, GetParam());

        ASSERT_TRUE(OCI_TimestampSysTimestamp(tmsp1));
        Sleep(100);
        ASSERT_TRUE(OCI_TimestampSysTimestamp(tmsp2));
        ASSERT_EQ(-1, OCI_TimestampCompare(tmsp1, tmsp2));
        ASSERT_EQ(1, OCI_TimestampCompare(tmsp2, tmsp1));

        ASSERT_TRUE(OCI_TimestampFree(tmsp1));
        ASSERT_TRUE(OCI_TimestampFree(tmsp2));

    }

    TEST_P(TestTimestamp, CompareEqual)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto tmsp1 = OCI_TimestampCreate(env, GetParam());
        const auto tmsp2 = OCI_TimestampCreate(env, GetParam());

        ASSERT_TRUE(OCI_TimestampSysTimestamp(tmsp1));
        ASSERT_TRUE(OCI_TimestampAssign(tmsp2, tmsp1));
        ASSERT_EQ(0, OCI_TimestampCompare(tmsp1, tmsp2));

        ASSERT_TRUE(OCI_TimestampFree(tmsp1));
        ASSERT_TRUE(OCI_TimestampFree(tmsp2));

    }

    TEST_P(TestTimestamp, FromTextDefaultFormat)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto tmsp = OCI_TimestampCreate(env, GetParam());
        OCI_Int32 year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0, msec = 0;

        ASSERT_TRUE(OCI_TimestampFromText(tmsp, OTEXT("1978-04-13 20:30:15.000000300"), OCI_STRING_FORMAT_TIMESTAMP));
        ASSERT_TRUE(OCI_TimestampGetDateTime(tmsp, &year, &month, &day, &hour, &min, &sec, &msec));

        ASSERT_EQ(1978, year);
        ASSERT_EQ(4, month);
        ASSERT_EQ(13, day);
        ASSERT_EQ(20, hour);
        ASSERT_EQ(30, min);
        ASSERT_EQ(15, sec);
        ASSERT_EQ(300, msec);

        ASSERT_TRUE(OCI_TimestampFree(tmsp));

    }

    TEST_P(TestTimestamp, ToTextDefaultFormat)
    {
        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto tmsp = OCI_TimestampCreate(env, GetParam());

        ASSERT_TRUE(OCI_TimestampFromText(tmsp, OTEXT("1978-04-13 20:30:15.000000300"), OCI_STRING_FORMAT_TIMESTAMP));
        ASSERT_TRUE(OCI_TimestampToText(tmsp, OCI_STRING_FORMAT_TIMESTAMP, 512, buffer, 9));

        ASSERT_EQ(ostring(OTEXT("1978-04-13 20:30:15.000000300")), ostring(buffer));

        ASSERT_TRUE(OCI_TimestampFree(tmsp));

    }

    TEST_P(TestTimestamp, AddInterval)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto tmsp = OCI_TimestampCreate(env, GetParam());
        const auto itv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        OCI_Int32 year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0, msec = 0;

        ASSERT_TRUE(OCI_TimestampFromText(tmsp, OTEXT("1978-04-13 20:30:15.000000300"), OCI_STRING_FORMAT_TIMESTAMP));
        ASSERT_TRUE(OCI_IntervalSetDaySecond(itv, 0, 1, 0, 0, 0));
        ASSERT_TRUE(OCI_TimestampIntervalAdd(tmsp, itv));
        ASSERT_TRUE(OCI_TimestampGetDateTime(tmsp, &year, &month, &day, &hour, &min, &sec, &msec));

        ASSERT_EQ(1978, year);
        ASSERT_EQ(4, month);
        ASSERT_EQ(13, day);
        ASSERT_EQ(21, hour);
        ASSERT_EQ(30, min);
        ASSERT_EQ(15, sec);
        ASSERT_EQ(300, msec);

        ASSERT_TRUE(OCI_TimestampFree(tmsp));
        ASSERT_TRUE(OCI_IntervalFree(itv));

    }

    TEST_P(TestTimestamp, SubInterval)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto tmsp = OCI_TimestampCreate(env, GetParam());
        const auto itv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        OCI_Int32 year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0, msec = 0;

        ASSERT_TRUE(OCI_TimestampFromText(tmsp, OTEXT("1978-04-13 20:30:15.000000300"), OCI_STRING_FORMAT_TIMESTAMP));
        ASSERT_TRUE(OCI_IntervalSetDaySecond(itv, 0, 1, 0, 0, 0));
        ASSERT_TRUE(OCI_TimestampIntervalSub(tmsp, itv));
        ASSERT_TRUE(OCI_TimestampGetDateTime(tmsp, &year, &month, &day, &hour, &min, &sec, &msec));

        ASSERT_EQ(1978, year);
        ASSERT_EQ(4, month);
        ASSERT_EQ(13, day);
        ASSERT_EQ(19, hour);
        ASSERT_EQ(30, min);
        ASSERT_EQ(15, sec);
        ASSERT_EQ(300, msec);

        ASSERT_TRUE(OCI_TimestampFree(tmsp));
        ASSERT_TRUE(OCI_IntervalFree(itv));

    }

    TEST(TestTimestamp, TimestampFetch)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select to_timestamp('1978-04-13 20:30:15.000000300', 'YYYY-MM-DD HH24:MI:SS.FF') from dual")));

        const auto rslt = OCI_GetResultset(stmt);
        ASSERT_TRUE(OCI_FetchNext(rslt));

        const auto tmsp = OCI_GetTimestamp(rslt, 1);

        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
        ASSERT_TRUE(OCI_TimestampToText(tmsp, OCI_STRING_FORMAT_TIMESTAMP, 512, buffer, 9));

        ASSERT_EQ(ostring(OTEXT("1978-04-13 20:30:15.000000300")), ostring(buffer));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST_P(TestTimestamp, TimestampBindInOut)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto tmsp = OCI_TimestampCreate(env, GetParam());
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);

        ASSERT_TRUE(OCI_TimestampFromText(tmsp, OTEXT("1978-04-13 19:30:15.000000300"), OCI_STRING_FORMAT_TIMESTAMP));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("begin :tmsp := :tmsp + to_dsinterval('1 01:00:00'); end;")));
        ASSERT_NE(nullptr, OCI_BindTimestamp(stmt, OTEXT(":tmsp"), tmsp));
        ASSERT_TRUE(OCI_BindSetDirection(OCI_GetBind(stmt, 1), OCI_BDM_IN_OUT));
        ASSERT_TRUE(OCI_Execute(stmt));

        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
        ASSERT_TRUE(OCI_TimestampToText(tmsp, OCI_STRING_FORMAT_TIMESTAMP, 512, buffer, 9));

        ASSERT_EQ(ostring(OTEXT("1978-04-14 20:30:15.000000300")), ostring(buffer));

        ASSERT_TRUE(OCI_TimestampFree(tmsp));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestTimestamp, TimestampBindRegister)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);

        OCI_ExecuteStmt(stmt, OTEXT("drop table test_tmsp_register"));
        OCI_ExecuteStmt(stmt, OTEXT("create table test_tmsp_register (value timestamp)"));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into test_tmsp_register values (to_timestamp('1978-04-13 20:30:15.000300', 'YYYY-MM-DD HH24:MI:SS.FF6')) returning value into :1")));
        ASSERT_TRUE(OCI_RegisterTimestamp(stmt, OTEXT(":1"), OCI_TIMESTAMP));
        ASSERT_TRUE(OCI_Execute(stmt));

        const auto rslt = OCI_GetResultset(stmt);
        ASSERT_TRUE(OCI_FetchNext(rslt));

        const auto tmsp = OCI_GetTimestamp(rslt, 1);

        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
        ASSERT_TRUE(OCI_TimestampToText(tmsp, OCI_STRING_FORMAT_TIMESTAMP, 512, buffer, 6));

        ASSERT_EQ(ostring(OTEXT("1978-04-13 20:30:15.000300")), ostring(buffer));

        OCI_ExecuteStmt(stmt, OTEXT("drop table test_tmsp_register"));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestTimestamp, FormatWithSetGet)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        ASSERT_TRUE(OCI_SetFormat(env, OCI_FMT_TIMESTAMP, OTEXT("YYYY-MM-DD HH24:MI:SS")));
        ASSERT_EQ(ostring(OTEXT("YYYY-MM-DD HH24:MI:SS")), ostring(OCI_GetFormat(env, OCI_FMT_TIMESTAMP)));

        auto ts = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        ASSERT_NE(nullptr, ts);
        ASSERT_TRUE(OCI_TimestampFromText(ts, OTEXT("2020-05-12 10:30:00"), OTEXT("YYYY-MM-DD HH24:MI:SS")));

        otext buffer[64];
        ASSERT_TRUE(OCI_TimestampToText(ts, OTEXT("YYYY-MM-DD HH24:MI:SS"), 64, buffer, 0));
        ASSERT_EQ(ostring(OTEXT("2020-05-12 10:30:00")), ostring(buffer));

        ASSERT_TRUE(OCI_TimestampFree(ts));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST_P(TestTimestamp, GetDateAndTime)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto tmsp = OCI_TimestampCreate(env, GetParam());
        ASSERT_NE(nullptr, tmsp);

        ASSERT_TRUE(OCI_TimestampFromText(tmsp, OTEXT("1978-04-13 20:30:15.000000300"), OCI_STRING_FORMAT_TIMESTAMP));

        OCI_Int32 year = 0, month = 0, day = 0;
        ASSERT_TRUE(OCI_TimestampGetDate(tmsp, &year, &month, &day));
        ASSERT_EQ(1978, year);
        ASSERT_EQ(4, month);
        ASSERT_EQ(13, day);

        OCI_Int32 hour = 0, min = 0, sec = 0, fsec = 0;
        ASSERT_TRUE(OCI_TimestampGetTime(tmsp, &hour, &min, &sec, &fsec));
        ASSERT_EQ(20, hour);
        ASSERT_EQ(30, min);
        ASSERT_EQ(15, sec);
        ASSERT_EQ(300, fsec);

        ASSERT_TRUE(OCI_TimestampFree(tmsp));

    }

    TEST_P(TestTimestamp, TimeZoneAccessors)
    {
        if (OCI_TIMESTAMP == GetParam())
        {
            return;
        }

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto tmsp = OCI_TimestampCreate(env, GetParam());
        ASSERT_NE(nullptr, tmsp);

        ASSERT_TRUE(OCI_TimestampFromText(tmsp, OTEXT("2020-05-12 10:30:00.000000"), OCI_STRING_FORMAT_TIMESTAMP));

        otext tzName[64] = {};
        ASSERT_TRUE(OCI_TimestampGetTimeZoneName(tmsp, 64, tzName));

        OCI_Int32 tzHour = 0, tzMin = 0;
        ASSERT_TRUE(OCI_TimestampGetTimeZoneOffset(tmsp, &tzHour, &tzMin));

        ASSERT_TRUE(OCI_TimestampFree(tmsp));

    }

    TEST_P(TestTimestamp, ToTextWithNullFormat)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto tmsp = OCI_TimestampCreate(env, GetParam());
        ASSERT_NE(nullptr, tmsp);

        ASSERT_TRUE(OCI_TimestampFromText(tmsp, OTEXT("1978-04-13 20:30:15.000000300"), OCI_STRING_FORMAT_TIMESTAMP));

        otext buffer[OCI_SIZE_BUFFER] = {};
        ASSERT_TRUE(OCI_TimestampToText(tmsp, nullptr, OCI_SIZE_BUFFER, buffer, 9));

        ASSERT_TRUE(OCI_TimestampFree(tmsp));

    }

    TEST_P(TestTimestamp, FromTextWithNullFormat)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto tmsp = OCI_TimestampCreate(env, GetParam());
        ASSERT_NE(nullptr, tmsp);

        ASSERT_TRUE(OCI_TimestampFromText(tmsp, OTEXT("2020-05-12 10:30:00.000000"), OCI_STRING_FORMAT_TIMESTAMP));

        otext buffer[OCI_SIZE_BUFFER] = {};
        ASSERT_TRUE(OCI_TimestampToText(tmsp, nullptr, OCI_SIZE_BUFFER, buffer, 0));
        ASSERT_TRUE(OCI_TimestampFromText(tmsp, buffer, nullptr));

        ASSERT_TRUE(OCI_TimestampFree(tmsp));

    }

    TEST_P(TestTimestamp, ToFromCTime)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto tmsp = OCI_TimestampCreate(env, GetParam());
        ASSERT_NE(nullptr, tmsp);

        struct tm ptm = {};
        ptm.tm_year = 120;
        ptm.tm_mon = 3;
        ptm.tm_mday = 13;
        ptm.tm_hour = 20;
        ptm.tm_min = 30;
        ptm.tm_sec = 15;

        time_t t = mktime(&ptm);

        ASSERT_TRUE(OCI_TimestampFromCTime(tmsp, &ptm, t));

        struct tm ptm2 = {};
        time_t t2 = 0;
        ASSERT_TRUE(OCI_TimestampToCTime(tmsp, &ptm2, &t2));

        ASSERT_TRUE(OCI_TimestampFree(tmsp));

    }

    TEST_P(TestTimestamp, ConstructAndConvert)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto tmsp = OCI_TimestampCreate(env, GetParam());
        ASSERT_NE(nullptr, tmsp);

        ASSERT_TRUE(OCI_TimestampConstruct(tmsp, 2020, 5, 12, 10, 30, 0, 0, nullptr));

        const auto tmsp2 = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        ASSERT_NE(nullptr, tmsp2);
        ASSERT_TRUE(OCI_TimestampConvert(tmsp2, tmsp));

        ASSERT_TRUE(OCI_TimestampFree(tmsp));
        ASSERT_TRUE(OCI_TimestampFree(tmsp2));

    }

    TEST_P(TestTimestamp, SubtractAndCheck)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto tmsp1 = OCI_TimestampCreate(env, GetParam());
        const auto tmsp2 = OCI_TimestampCreate(env, GetParam());
        const auto itv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        ASSERT_NE(nullptr, tmsp1);
        ASSERT_NE(nullptr, tmsp2);
        ASSERT_NE(nullptr, itv);

        ASSERT_TRUE(OCI_TimestampFromText(tmsp1, OTEXT("1978-04-13 20:30:15.000000300"), OCI_STRING_FORMAT_TIMESTAMP));
        ASSERT_TRUE(OCI_TimestampFromText(tmsp2, OTEXT("1978-04-13 19:30:15.000000300"), OCI_STRING_FORMAT_TIMESTAMP));

        ASSERT_TRUE(OCI_TimestampSubtract(tmsp1, tmsp2, itv));
        ASSERT_TRUE(OCI_TimestampCheck(tmsp1) >= 0);

        ASSERT_TRUE(OCI_TimestampFree(tmsp1));
        ASSERT_TRUE(OCI_TimestampFree(tmsp2));
        ASSERT_TRUE(OCI_IntervalFree(itv));

    }

    INSTANTIATE_TEST_CASE_P(TestTimestamp, TestTimestamp, ::testing::ValuesIn(TimestampTypes));
}