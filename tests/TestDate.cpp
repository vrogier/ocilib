#include "ocilib_tests.h"
#include "NullArgs.h"

namespace TestCApi
{
    TEST(TestDate, CreateAndFree)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto date = OCI_DateCreate(env);
        ASSERT_TRUE(nullptr != date);
        ASSERT_TRUE(OCI_DateFree(date));

    }

    TEST(TestDate, ArrayCreateAndFree)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto dates = OCI_DateArrayCreate(env, ARRAY_SIZE);
        ASSERT_TRUE(nullptr != dates);

        for (OCI_Int32 i = 0; i < ARRAY_SIZE; i++)
        {
            ASSERT_TRUE(nullptr != dates[i]);
        }
        ASSERT_TRUE(OCI_DateArrayFree(dates));

    }

    TEST(TestDate, Assign)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto date1 = OCI_DateCreate(env);
        const auto date2 = OCI_DateCreate(env);

        ASSERT_TRUE(OCI_DateSysDate(date1));
        ASSERT_TRUE(OCI_DateAssign(date2, date1));
        ASSERT_EQ(0, OCI_DateCompare(date1, date2));

        ASSERT_TRUE(OCI_DateFree(date1));
        ASSERT_TRUE(OCI_DateFree(date2));

    }

    TEST(TestDate, AddDays)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto date1 = OCI_DateCreate(env);
        const auto date2 = OCI_DateCreate(env);

        ASSERT_TRUE(OCI_DateSysDate(date1));
        ASSERT_TRUE(OCI_DateAssign(date2, date1));
        ASSERT_TRUE(OCI_DateAddDays(date2, 3));
        ASSERT_TRUE(OCI_DateAddDays(date2, -3));
        ASSERT_EQ(0, OCI_DateCompare(date1, date2));

        ASSERT_TRUE(OCI_DateFree(date1));
        ASSERT_TRUE(OCI_DateFree(date2));

    }

    TEST(TestDate, AddMonths)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto date1 = OCI_DateCreate(env);
        const auto date2 = OCI_DateCreate(env);

        ASSERT_TRUE(OCI_DateSysDate(date1));
        ASSERT_TRUE(OCI_DateAssign(date2, date1));
        ASSERT_TRUE(OCI_DateAddMonths(date2, 5));
        ASSERT_TRUE(OCI_DateAddMonths(date2, -5));
        ASSERT_EQ(0, OCI_DateCompare(date1, date2));

        ASSERT_TRUE(OCI_DateFree(date1));
        ASSERT_TRUE(OCI_DateFree(date2));

    }

    TEST(TestDate, CompareInferior)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto date1 = OCI_DateCreate(env);
        const auto date2 = OCI_DateCreate(env);

        ASSERT_TRUE(OCI_DateSysDate(date1));
        ASSERT_TRUE(OCI_DateAssign(date2, date1));
        ASSERT_TRUE(OCI_DateAddDays(date2, 3));
        ASSERT_EQ(-1, OCI_DateCompare(date1, date2));

        ASSERT_TRUE(OCI_DateFree(date1));
        ASSERT_TRUE(OCI_DateFree(date2));

    }

    TEST(TestDate, CompareSuperior)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto date1 = OCI_DateCreate(env);
        const auto date2 = OCI_DateCreate(env);

        ASSERT_TRUE(OCI_DateSysDate(date1));
        ASSERT_TRUE(OCI_DateAssign(date2, date1));
        ASSERT_TRUE(OCI_DateAddDays(date2, -3));
        ASSERT_EQ(1, OCI_DateCompare(date1, date2));

        ASSERT_TRUE(OCI_DateFree(date1));
        ASSERT_TRUE(OCI_DateFree(date2));

    }

    TEST(TestDate, CompareEqual)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto date1 = OCI_DateCreate(env);
        const auto date2 = OCI_DateCreate(env);

        ASSERT_TRUE(OCI_DateSysDate(date1));
        ASSERT_TRUE(OCI_DateAssign(date2, date1));
        ASSERT_EQ(0, OCI_DateCompare(date1, date2));

        ASSERT_TRUE(OCI_DateFree(date1));
        ASSERT_TRUE(OCI_DateFree(date2));

    }

    TEST(TestDate, DaysBetween)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto date1 = OCI_DateCreate(env);
        const auto date2 = OCI_DateCreate(env);

        ASSERT_TRUE(OCI_DateSysDate(date1));
        ASSERT_TRUE(OCI_DateAssign(date2, date1));
        ASSERT_TRUE(OCI_DateAddDays(date2, 14));
        ASSERT_EQ(14, OCI_DateDaysBetween(date2, date1));

        ASSERT_TRUE(OCI_DateFree(date1));
        ASSERT_TRUE(OCI_DateFree(date2));

    }

    TEST(TestDate, FromTextDefaultFormat)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto date = OCI_DateCreate(env);
        tm cdate{};

        ASSERT_TRUE(OCI_DateFromText(date, OTEXT("1978-04-13 20:30:15"), OCI_STRING_FORMAT_DATETIME));
        ASSERT_TRUE(OCI_DateToCTime(date, &cdate, NULL));

        ASSERT_EQ(1978, (1900 + cdate.tm_year));
        ASSERT_EQ(4, cdate.tm_mon + 1);
        ASSERT_EQ(13, cdate.tm_mday);
        ASSERT_EQ(20, cdate.tm_hour);
        ASSERT_EQ(30, cdate.tm_min);
        ASSERT_EQ(15, cdate.tm_sec);

        ASSERT_TRUE(OCI_DateFree(date));

    }

    TEST(TestDate, ToTextDefaultFormat)
    {
        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
        const OCI_Int32 year = 1978, month = 4, day = 13, hour = 20, min = 30, sec = 15;

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto date = OCI_DateCreate(env);

        ASSERT_TRUE(OCI_DateSetDateTime(date, year, month, day, hour, min, sec));
        ASSERT_TRUE(OCI_DateToText(date, OCI_STRING_FORMAT_DATETIME, 512, buffer));

        ASSERT_EQ(String(OTEXT("1978-04-13 20:30:15")), String(buffer));

        ASSERT_TRUE(OCI_DateFree(date));

    }

    TEST(TestDate, GetLastDay)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto date = OCI_DateCreate(env);
        OCI_Int32 year = 0, month = 0, day = 0;

        ASSERT_TRUE(OCI_DateFromText(date, OTEXT("1978-04-13 20:30:15"), OCI_STRING_FORMAT_DATETIME));
        ASSERT_TRUE(OCI_DateLastDay(date));
        ASSERT_TRUE(OCI_DateGetDate(date, &year, &month, &day));
        ASSERT_EQ(1978, year);
        ASSERT_EQ(4, month);
        ASSERT_EQ(30, day);

        ASSERT_TRUE(OCI_DateFree(date));

    }
    /*
    TEST(TestDate, GetNextDay)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto date = OCI_DateCreate(env);
        OCI_Int32 year = 0, month = 0, day = 0;

        ASSERT_TRUE(OCI_DateFromText(date, OTEXT("1978-04-13 20:30:15"), OCI_STRING_FORMAT_DATETIME));

        const otext* sunday = OCI_EnvironmentGetLocaleString(env, OCI_LSI_DAYNAME7);
        ASSERT_NE(nullptr, sunday);
        ASSERT_TRUE(OCI_DateNextDay(date, sunday));

        ASSERT_TRUE(OCI_DateGetDate(date, &year, &month, &day));
        ASSERT_EQ(1978, year);
        ASSERT_EQ(4, month);
        ASSERT_EQ(16, day);

        ASSERT_TRUE(OCI_DateFree(date));

    }*/

    TEST(TestDate, SetGetDateTime)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto date = OCI_DateCreate(env);
        OCI_Int32 year = 1978, month = 4, day = 13, hour = 20, min = 30, sec = 15;

        ASSERT_TRUE(OCI_DateFromText(date, OTEXT("1978-04-13 20:30:15"), OCI_STRING_FORMAT_DATETIME));
        ASSERT_TRUE(OCI_DateSetDateTime(date, year, month, day, hour, min, sec));
        ASSERT_TRUE(OCI_DateGetDateTime(date, &year, &month, &day, &hour, &min, &sec));

        ASSERT_EQ(1978, year);
        ASSERT_EQ(4, month);
        ASSERT_EQ(13, day);
        ASSERT_EQ(20, hour);
        ASSERT_EQ(30, min);
        ASSERT_EQ(15, sec);

        ASSERT_TRUE(OCI_DateFree(date));

    }


    TEST(TestDate, DateFetch)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select to_date('1978-04-13 20:30:15', 'YYYY-MM-DD HH24:MI:SS') from dual")));

        const auto rslt = OCI_StatementGetResultset(stmt);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rslt));

        const auto date = OCI_ResultsetGetDate(rslt, 0);

        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
        ASSERT_TRUE(OCI_DateToText(date, OCI_STRING_FORMAT_DATETIME, 512, buffer));

        ASSERT_EQ(String(OTEXT("1978-04-13 20:30:15")), String(buffer));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestDate, DateBindInOut)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto date = OCI_DateCreate(env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);

        ASSERT_TRUE(OCI_DateFromText(date, OTEXT("1978-04-12 20:30:15"), OCI_STRING_FORMAT_DATETIME));
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("begin :date := :date + 1; end;")));
        ASSERT_NE(nullptr, OCI_StatementBindDate(stmt, OTEXT(":date"), date));
        ASSERT_TRUE(OCI_BindSetDirection(OCI_StatementGetBind(stmt, 0), OCI_BDM_IN_OUT));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
        ASSERT_TRUE(OCI_DateToText(date, OCI_STRING_FORMAT_DATETIME, 512, buffer));

        ASSERT_EQ(String(OTEXT("1978-04-13 20:30:15")), String(buffer));

        ASSERT_TRUE(OCI_DateFree(date));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestDate, DateBindRegister)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);

        OCI_StatementExecute(stmt, OTEXT("drop table test_date_register"));
        OCI_StatementExecute(stmt, OTEXT("create table test_date_register (value date)"));

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into test_date_register values (to_date('1978-04-13 20:30:15', 'YYYY-MM-DD HH24:MI:SS')) returning value into :1")));
        ASSERT_TRUE(OCI_StatementRegisterDate(stmt, OTEXT(":1")));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        const auto rslt = OCI_StatementGetResultset(stmt);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rslt));

        const auto date = OCI_ResultsetGetDate(rslt, 0);

        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
        ASSERT_TRUE(OCI_DateToText(date, OCI_STRING_FORMAT_DATETIME, 512, buffer));

        ASSERT_EQ(String(OTEXT("1978-04-13 20:30:15")), String(buffer));

        OCI_StatementExecute(stmt, OTEXT("drop table test_date_register"));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestDate, FormatWithSetGet)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        ASSERT_TRUE(OCI_EnvironmentSetFormat(env, OCI_FMT_DATE, OTEXT("DD/MM/YYYY")));
        ASSERT_EQ(String(OTEXT("DD/MM/YYYY")), String(OCI_EnvironmentGetFormat(env, OCI_FMT_DATE)));

        auto date = OCI_DateCreate(env);
        ASSERT_NE(nullptr, date);
        ASSERT_TRUE(OCI_DateFromText(date, OTEXT("12/05/2020"), OTEXT("DD/MM/YYYY")));

        otext buffer[64];
        ASSERT_TRUE(OCI_DateToText(date, OTEXT("DD/MM/YYYY"), 64, buffer));
        ASSERT_EQ(String(OTEXT("12/05/2020")), String(buffer));

        ASSERT_TRUE(OCI_DateFree(date));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestDate, FromCTimeAndNullFormat)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto date = OCI_DateCreate(env);
        time_t t = 0;
        struct tm ptm = {};

        ASSERT_TRUE(OCI_DateFromText(date, OTEXT("1978-04-13 20:30:15"), OCI_STRING_FORMAT_DATETIME));

        ASSERT_TRUE(OCI_DateToCTime(date, &ptm, &t));

        ASSERT_TRUE(OCI_DateFromCTime(date, &ptm, t));

        otext buffer[OCI_SIZE_BUFFER] = {};
        ASSERT_TRUE(OCI_DateToText(date, nullptr, OCI_SIZE_BUFFER, buffer));
        ASSERT_TRUE(OCI_DateFromText(date, buffer, nullptr));

        ASSERT_TRUE(OCI_DateFree(date));

    }

    /*
    TEST(TestDate, NextDay)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto date = OCI_DateCreate(env);
        ASSERT_NE(nullptr, date);

        ASSERT_TRUE(OCI_DateFromText(date, OTEXT("1978-04-13 20:30:15"), OCI_STRING_FORMAT_DATETIME));
        ASSERT_TRUE(OCI_DateNextDay(date, OTEXT("MONDAY")));

        OCI_Int32 year = 0, month = 0, day = 0;
        ASSERT_TRUE(OCI_DateGetDate(date, &year, &month, &day));
        ASSERT_EQ(1978, year);
        ASSERT_EQ(4, month);
        ASSERT_EQ(17, day);

        ASSERT_TRUE(OCI_DateFree(date));
    }
    */

    TEST(TestDate, ZoneToZone)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto date = OCI_DateCreate(env);
        ASSERT_NE(nullptr, date);

        ASSERT_TRUE(OCI_DateSysDate(date));
        ASSERT_TRUE(OCI_DateZoneToZone(date, OTEXT("GMT"), OTEXT("EST")));

        ASSERT_TRUE(OCI_DateFree(date));
    }

    TEST(TestDate, FromCTimeNullTmZeroTime)
    {
        /* NULL tm with a zero time is rejected */

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto date = OCI_DateCreate(env);
        ASSERT_NE(nullptr, date);

        ASSERT_FALSE(OCI_DateFromCTime(date, nullptr, (time_t) 0));

        ASSERT_TRUE(OCI_DateFree(date));
    }

    TEST(TestDate, FromCTimeNullTmValidTime)
    {
        /* NULL tm with a valid time goes through localtime() */

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto date = OCI_DateCreate(env);
        ASSERT_NE(nullptr, date);

        ASSERT_TRUE(OCI_DateFromCTime(date, nullptr, time(nullptr)));

        ASSERT_TRUE(OCI_DateFree(date));
    }

    TEST(TestDate, FromCTimeNegative)
    {
        /* a negative time_t fails where localtime() rejects it (MSVC) */

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto date = OCI_DateCreate(env);
        ASSERT_NE(nullptr, date);

        (void) OCI_DateFromCTime(date, nullptr, (time_t) -1);

        ASSERT_TRUE(OCI_DateFree(date));
    }

    class TestDateNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestDateNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestDateNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "DateAddDays", [] { return !OCI_DateAddDays(nullptr, 1); } },
            { "DateAddMonths", [] { return !OCI_DateAddMonths(nullptr, 1); } },
            { "DateArrayCreate", [] { return nullptr == OCI_DateArrayCreate(nullptr, 1); } },
            { "DateArrayFree", [] { return !OCI_DateArrayFree(nullptr); } },
            { "DateAssign", [] { return !OCI_DateAssign(nullptr, nullptr); } },
            { "DateCheck", [] { return -1 == OCI_DateCheck(nullptr); } },
            { "DateCompare", [] { return 0 == OCI_DateCompare(nullptr, nullptr); } },
            { "DateCreate", [] { return nullptr == OCI_DateCreate(nullptr); } },
            { "DateDaysBetween", [] { return 0 == OCI_DateDaysBetween(nullptr, nullptr); } },
            { "DateFree", [] { return !OCI_DateFree(nullptr); } },
            { "DateFromCTime", [] { return !OCI_DateFromCTime(nullptr, NullArgTm(), 1); } },
            { "DateFromText", [] { return !OCI_DateFromText(nullptr, OTEXT("x"), OTEXT("x")); } },
            { "DateGetDate", [] { return !OCI_DateGetDate(nullptr, NullArgOut<OCI_Int32>(), NullArgOut<OCI_Int32>(), NullArgOut<OCI_Int32>()); } },
            { "DateGetDateTime", [] { return !OCI_DateGetDateTime(nullptr, NullArgOut<OCI_Int32>(), NullArgOut<OCI_Int32>(), NullArgOut<OCI_Int32>(), NullArgOut<OCI_Int32>(), NullArgOut<OCI_Int32>(), NullArgOut<OCI_Int32>()); } },
            { "DateGetTime", [] { return !OCI_DateGetTime(nullptr, NullArgOut<OCI_Int32>(), NullArgOut<OCI_Int32>(), NullArgOut<OCI_Int32>()); } },
            { "DateLastDay", [] { return !OCI_DateLastDay(nullptr); } },
            { "DateNextDay", [] { return !OCI_DateNextDay(nullptr, OTEXT("x")); } },
            { "DateSetDate", [] { return !OCI_DateSetDate(nullptr, 1, 1, 1); } },
            { "DateSetDateTime", [] { return !OCI_DateSetDateTime(nullptr, 1, 1, 1, 1, 1, 1); } },
            { "DateSetTime", [] { return !OCI_DateSetTime(nullptr, 1, 1, 1); } },
            { "DateSysDate", [] { return !OCI_DateSysDate(nullptr); } },
            { "DateToCTime", [] { return !OCI_DateToCTime(nullptr, NullArgTm(), NullArgTime()); } },
            { "DateToText", [] { return !OCI_DateToText(nullptr, OTEXT("x"), 1, NullArgText()); } },
            { "DateZoneToZone", [] { return !OCI_DateZoneToZone(nullptr, OTEXT("x"), OTEXT("x")); } },
        }),
        &NullArgCaseName);

}
