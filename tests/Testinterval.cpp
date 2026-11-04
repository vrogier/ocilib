#include "ocilib_tests.h"

namespace TestCApi
{
    static Context context;

    static inline void AddError(OCI_Error* err)
    {
        context.Errs.emplace_back(Error{ OCI_ErrorGetCode(err), OCI_ErrorGetCategory(err) });

        auto str = OCI_ErrorGetString(err);

        if (str == nullptr)
        {
            str = nullptr;
        }
    }
    class TestInterval : public ::testing::TestWithParam<OCI_UInt32> {};

    static std::vector<OCI_UInt32> IntervalTypes{ OCI_INTERVAL_DS, OCI_INTERVAL_YM };

    TEST_P(TestInterval, CreateAndFree)
    {
        auto env = GetEnvironment(AddError);
        ASSERT_NE(nullptr, env);

        const auto itv = OCI_IntervalCreate(env, GetParam());
        ASSERT_EQ(GetParam(), OCI_IntervalGetType(itv));
        ASSERT_TRUE(OCI_IntervalFree(itv));

    }

    TEST_P(TestInterval, ArrayCreateAndFree)
    {
        auto env = GetEnvironment(AddError);
        ASSERT_NE(nullptr, env);

        const auto itvs = OCI_IntervalArrayCreate(env, GetParam(), ARRAY_SIZE);
        ASSERT_TRUE(nullptr != itvs);

        for (OCI_Int32 i = 0; i < ARRAY_SIZE; i++)
        {
            ASSERT_TRUE(nullptr != itvs[i]);
            ASSERT_EQ(GetParam(), OCI_IntervalGetType(itvs[i]));
        }
        ASSERT_TRUE(OCI_IntervalArrayFree(itvs));

    }

    TEST(TestInterval, AssignDaySecond)
    {
        auto env = GetEnvironment(AddError);

        ASSERT_NE(nullptr, env);

        const auto itv1 = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        const auto itv2 = OCI_IntervalCreate(env, OCI_INTERVAL_DS);

        ASSERT_TRUE(OCI_IntervalSetDaySecond(itv1, 1, 1, 1, 1, 1));
        ASSERT_TRUE(OCI_IntervalAssign(itv2, itv1));
        ASSERT_EQ(0, OCI_IntervalCompare(itv1, itv2));

        ASSERT_TRUE(OCI_IntervalFree(itv1));
        ASSERT_TRUE(OCI_IntervalFree(itv2));

    }

    TEST(TestInterval, AssignYearMonth)
    {
        auto env = GetEnvironment(AddError);

        ASSERT_NE(nullptr, env);

        const auto itv1 = OCI_IntervalCreate(env, OCI_INTERVAL_YM);
        const auto itv2 = OCI_IntervalCreate(env, OCI_INTERVAL_YM);

        ASSERT_TRUE(OCI_IntervalSetYearMonth(itv1, 1, 1));
        ASSERT_TRUE(OCI_IntervalAssign(itv2, itv1));
        ASSERT_EQ(0, OCI_IntervalCompare(itv1, itv2));

        ASSERT_TRUE(OCI_IntervalFree(itv1));
        ASSERT_TRUE(OCI_IntervalFree(itv2));

    }

    TEST(TestInterval, CompareDaySecondInferiorSuperior)
    {
        auto env = GetEnvironment(AddError);

        ASSERT_NE(nullptr, env);

        const auto itv1 = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        const auto itv2 = OCI_IntervalCreate(env, OCI_INTERVAL_DS);

        ASSERT_TRUE(OCI_IntervalSetDaySecond(itv1, 1, 1, 1, 1, 1));
        ASSERT_TRUE(OCI_IntervalSetDaySecond(itv2, 2, 1, 1, 1, 1));
        ASSERT_EQ(-1, OCI_IntervalCompare(itv1, itv2));
        ASSERT_EQ(1, OCI_IntervalCompare(itv2, itv1));

        ASSERT_TRUE(OCI_IntervalFree(itv1));
        ASSERT_TRUE(OCI_IntervalFree(itv2));

    }

    TEST(TestInterval, CompareYearMonthInferiorSuperior)
    {
        auto env = GetEnvironment(AddError);

        ASSERT_NE(nullptr, env);

        const auto itv1 = OCI_IntervalCreate(env, OCI_INTERVAL_YM);
        const auto itv2 = OCI_IntervalCreate(env, OCI_INTERVAL_YM);

        ASSERT_TRUE(OCI_IntervalSetYearMonth(itv1, 1, 1));
        ASSERT_TRUE(OCI_IntervalSetYearMonth(itv2, 2, 1));
        ASSERT_EQ(-1, OCI_IntervalCompare(itv1, itv2));
        ASSERT_EQ(1, OCI_IntervalCompare(itv2, itv1));

        ASSERT_TRUE(OCI_IntervalFree(itv1));
        ASSERT_TRUE(OCI_IntervalFree(itv2));

    }


    TEST(TestInterval, CompareDaySecondEqual)
    {
        auto env = GetEnvironment(AddError);

        ASSERT_NE(nullptr, env);

        const auto itv1 = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        const auto itv2 = OCI_IntervalCreate(env, OCI_INTERVAL_DS);

        ASSERT_TRUE(OCI_IntervalSetDaySecond(itv1, 1, 1, 1, 1, 1));
        ASSERT_TRUE(OCI_IntervalSetDaySecond(itv2, 1, 1, 1, 1, 1));
        ASSERT_EQ(0, OCI_IntervalCompare(itv1, itv2));

        ASSERT_TRUE(OCI_IntervalFree(itv1));
        ASSERT_TRUE(OCI_IntervalFree(itv2));

    }

    TEST(TestInterval, CompareYearMonthEqual)
    {
        auto env = GetEnvironment(AddError);

        ASSERT_NE(nullptr, env);

        const auto itv1 = OCI_IntervalCreate(env, OCI_INTERVAL_YM);
        const auto itv2 = OCI_IntervalCreate(env, OCI_INTERVAL_YM);

        ASSERT_TRUE(OCI_IntervalSetYearMonth(itv1, 1, 1));
        ASSERT_TRUE(OCI_IntervalSetYearMonth(itv2, 1, 1));
        ASSERT_EQ(0, OCI_IntervalCompare(itv1, itv2));

        ASSERT_TRUE(OCI_IntervalFree(itv1));
        ASSERT_TRUE(OCI_IntervalFree(itv2));

    }

    TEST(TestInterval, FromTextDaySecond)
    {
        auto env = GetEnvironment(AddError);

        ASSERT_NE(nullptr, env);

        const auto itv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        OCI_Int32 day = 0, hour = 0, min = 0, sec = 0, msec = 0;

        ASSERT_TRUE(OCI_IntervalFromText(itv, OTEXT("+001 02:03:04.000000567")));
        ASSERT_TRUE(OCI_IntervalGetDaySecond(itv, &day, &hour, &min, &sec, &msec));

        ASSERT_EQ(1, day);
        ASSERT_EQ(2, hour);
        ASSERT_EQ(3, min);
        ASSERT_EQ(4, sec);
        ASSERT_EQ(567, msec);

        ASSERT_TRUE(OCI_IntervalFree(itv));

    }

    TEST(TestInterval, FromTextYearMonth)
    {
        auto env = GetEnvironment(AddError);

        ASSERT_NE(nullptr, env);

        const auto itv = OCI_IntervalCreate(env, OCI_INTERVAL_YM);
        OCI_Int32 year = 0, month = 0;

        ASSERT_TRUE(OCI_IntervalFromText(itv, OTEXT("+101-09")));
        ASSERT_TRUE(OCI_IntervalGetYearMonth(itv, &year, &month));

        ASSERT_EQ(101, year);
        ASSERT_EQ(9, month);

        ASSERT_TRUE(OCI_IntervalFree(itv));

    }

    TEST(TestInterval, ToTextDaySecond)
    {
        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");

        auto env = GetEnvironment(AddError);

        ASSERT_NE(nullptr, env);

        const auto itv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);

        ASSERT_TRUE(OCI_IntervalSetDaySecond(itv, 1, 2, 3, 4, 567));
        ASSERT_TRUE(OCI_IntervalToText(itv, 3, 9, 512, buffer));

        ASSERT_EQ(ostring(OTEXT("+001 02:03:04.000000567")), ostring(buffer));

        ASSERT_TRUE(OCI_IntervalFree(itv));

    }

    TEST(TestInterval, ToTextDayYearMonth)
    {
        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");

        auto env = GetEnvironment(AddError);

        ASSERT_NE(nullptr, env);

        const auto itv = OCI_IntervalCreate(env, OCI_INTERVAL_YM);

        ASSERT_TRUE(OCI_IntervalSetYearMonth(itv, 101, 9));
        ASSERT_TRUE(OCI_IntervalToText(itv, 3, 3, 512, buffer));

        ASSERT_EQ(ostring(OTEXT("+101-09")), ostring(buffer));

        ASSERT_TRUE(OCI_IntervalFree(itv));

    }

    TEST(TestInterval, AddDaySecond)
    {
        auto env = GetEnvironment(AddError);

        ASSERT_NE(nullptr, env);

        OCI_Int32 day = 0, hour = 0, min = 0, sec = 0, msec = 0;

        const auto itv1 = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        const auto itv2 = OCI_IntervalCreate(env, OCI_INTERVAL_DS);

        ASSERT_TRUE(OCI_IntervalSetDaySecond(itv1, 1, 2, 3, 4, 200));
        ASSERT_TRUE(OCI_IntervalSetDaySecond(itv2, 2, 3, 4, 5, 300));
        ASSERT_TRUE(OCI_IntervalAdd(itv1, itv2));

        ASSERT_TRUE(OCI_IntervalGetDaySecond(itv1, &day, &hour, &min, &sec, &msec));

        ASSERT_EQ(3, day);
        ASSERT_EQ(5, hour);
        ASSERT_EQ(7, min);
        ASSERT_EQ(9, sec);
        ASSERT_EQ(500, msec);

        ASSERT_TRUE(OCI_IntervalFree(itv1));
        ASSERT_TRUE(OCI_IntervalFree(itv2));

    }

    TEST(TestInterval, SubstractDaySecond)
    {
        auto env = GetEnvironment(AddError);

        ASSERT_NE(nullptr, env);

        OCI_Int32 day = 0, hour = 0, min = 0, sec = 0, msec = 0;

        const auto itv1 = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        const auto itv2 = OCI_IntervalCreate(env, OCI_INTERVAL_DS);

        ASSERT_TRUE(OCI_IntervalSetDaySecond(itv1, 1, 2, 3, 4, 200));
        ASSERT_TRUE(OCI_IntervalSetDaySecond(itv2, 2, 3, 4, 5, 300));
        ASSERT_TRUE(OCI_IntervalSubtract(itv2, itv1));

        ASSERT_TRUE(OCI_IntervalGetDaySecond(itv2, &day, &hour, &min, &sec, &msec));

        ASSERT_EQ(1, day);
        ASSERT_EQ(1, hour);
        ASSERT_EQ(1, min);
        ASSERT_EQ(1, sec);
        ASSERT_EQ(100, msec);

        ASSERT_TRUE(OCI_IntervalFree(itv1));
        ASSERT_TRUE(OCI_IntervalFree(itv2));

    }


    TEST(TestInterval, AdYearMonth)
    {
        auto env = GetEnvironment(AddError);

        ASSERT_NE(nullptr, env);

        OCI_Int32 year = 0, month = 0;

        const auto itv1 = OCI_IntervalCreate(env, OCI_INTERVAL_YM);
        const auto itv2 = OCI_IntervalCreate(env, OCI_INTERVAL_YM);

        ASSERT_TRUE(OCI_IntervalSetYearMonth(itv1, 1, 2));
        ASSERT_TRUE(OCI_IntervalSetYearMonth(itv2, 3, 4));
        ASSERT_TRUE(OCI_IntervalAdd(itv1, itv2));

        ASSERT_TRUE(OCI_IntervalGetYearMonth(itv1, &year, &month));

        ASSERT_EQ(4, year);
        ASSERT_EQ(6, month);

        ASSERT_TRUE(OCI_IntervalFree(itv1));
        ASSERT_TRUE(OCI_IntervalFree(itv2));

    }

    TEST(TestInterval, SubstractYearMonth)
    {
        auto env = GetEnvironment(AddError);

        ASSERT_NE(nullptr, env);

        OCI_Int32 year = 0, month = 0;

        const auto itv1 = OCI_IntervalCreate(env, OCI_INTERVAL_YM);
        const auto itv2 = OCI_IntervalCreate(env, OCI_INTERVAL_YM);

        ASSERT_TRUE(OCI_IntervalSetYearMonth(itv1, 1, 2));
        ASSERT_TRUE(OCI_IntervalSetYearMonth(itv2, 3, 4));
        ASSERT_TRUE(OCI_IntervalSubtract(itv2, itv1));

        ASSERT_TRUE(OCI_IntervalGetYearMonth(itv2, &year, &month));

        ASSERT_EQ(2, year);
        ASSERT_EQ(2, month);

        ASSERT_TRUE(OCI_IntervalFree(itv1));
        ASSERT_TRUE(OCI_IntervalFree(itv2));

    }

    TEST(TestInterval, IntervalFetch)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select to_dsinterval('1 02:03:04') from dual")));

        const auto rslt = OCI_GetResultset(stmt);
        ASSERT_TRUE(OCI_FetchNext(rslt));

        const auto itv = OCI_GetInterval(rslt, 1);

        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
        ASSERT_TRUE(OCI_IntervalToText(itv, 3, 9, 512, buffer));

        ASSERT_EQ(ostring(OTEXT("+001 02:03:04.000000000")), ostring(buffer));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestInterval, IntervalBindInOut)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto itv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);

        ASSERT_TRUE(OCI_IntervalFromText(itv, OTEXT("+001 02:03:04.000000004")));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("begin :itv := :itv * 2; end;")));
        ASSERT_NE(nullptr, OCI_BindInterval(stmt, OTEXT(":itv"), itv));
        ASSERT_TRUE(OCI_BindSetDirection(OCI_GetBind(stmt, 1), OCI_BDM_IN_OUT));
        ASSERT_TRUE(OCI_Execute(stmt));

        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
        ASSERT_TRUE(OCI_IntervalToText(itv, 3, 9, 512, buffer));

        ASSERT_EQ(ostring(OTEXT("+002 04:06:08.000000008")), ostring(buffer));

        ASSERT_TRUE(OCI_IntervalFree(itv));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestInterval, IntervalBindRegister)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);

        OCI_ExecuteStmt(stmt, OTEXT("drop table test_interval_register"));
        OCI_ExecuteStmt(stmt, OTEXT("create table test_interval_register (value interval day to second)"));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into test_interval_register values(to_dsinterval('1 02:03:04')) returning value into :1")));
        ASSERT_TRUE(OCI_RegisterInterval(stmt, OTEXT(":1"), OCI_INTERVAL_DS));
        ASSERT_TRUE(OCI_Execute(stmt));

        const auto rslt = OCI_GetResultset(stmt);
        ASSERT_TRUE(OCI_FetchNext(rslt));

        const auto itv = OCI_GetInterval(rslt, 1);

        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
        ASSERT_TRUE(OCI_IntervalToText(itv, 3, 9, 512, buffer));

        ASSERT_EQ(ostring(OTEXT("+001 02:03:04.000000000")), ostring(buffer));

        OCI_ExecuteStmt(stmt, OTEXT("drop table test_interval_register"));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    INSTANTIATE_TEST_CASE_P(TestInterval, TestInterval, ::testing::ValuesIn(IntervalTypes));

    TEST(TestInterval, Check)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto itv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        ASSERT_NE(nullptr, itv);

        ASSERT_EQ(0, OCI_IntervalCheck(itv));

        ASSERT_TRUE(OCI_IntervalFree(itv));
    }

    TEST(TestInterval, FromTimeZone)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto itv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        ASSERT_NE(nullptr, itv);

        ASSERT_TRUE(OCI_IntervalFromTimeZone(itv, OTEXT("-05:00")));

        OCI_Int32 day = 0, hour = 0, minute = 0, second = 0, fsecond = 0;
        ASSERT_TRUE(OCI_IntervalGetDaySecond(itv, &day, &hour, &minute, &second, &fsecond));
        ASSERT_EQ(-5, hour);

        ASSERT_TRUE(OCI_IntervalFree(itv));
    }
}
