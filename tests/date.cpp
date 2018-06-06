#include "ocilib_tests.h"

TEST(TestDate, CreateAndFree)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto date = OCI_DateCreate(nullptr);
    ASSERT_TRUE(nullptr != date);
    ASSERT_TRUE(OCI_DateFree(date));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestDate, ArrayCreateAndFree)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto dates = OCI_DateArrayCreate(nullptr, ARRAY_SIZE);
    ASSERT_TRUE(nullptr != dates);

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        ASSERT_TRUE(nullptr != dates[i]);
    }
    ASSERT_TRUE(OCI_DateArrayFree(dates));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestDate, Assign)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto date1 = OCI_DateCreate(nullptr);
    const auto date2 = OCI_DateCreate(nullptr);

    ASSERT_TRUE(OCI_DateSysDate(date1));
    ASSERT_TRUE(OCI_DateAssign(date2, date1));
    ASSERT_EQ(0, OCI_DateCompare(date1, date2));

    ASSERT_TRUE(OCI_DateFree(date1));
    ASSERT_TRUE(OCI_DateFree(date2));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestDate, AddDays)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto date1 = OCI_DateCreate(nullptr);
    const auto date2 = OCI_DateCreate(nullptr);

    ASSERT_TRUE(OCI_DateSysDate(date1));
    ASSERT_TRUE(OCI_DateAssign(date2, date1));
    ASSERT_TRUE(OCI_DateAddDays(date2, 3));
    ASSERT_TRUE(OCI_DateAddDays(date2, -3));
    ASSERT_EQ(0, OCI_DateCompare(date1, date2));

    ASSERT_TRUE(OCI_DateFree(date1));
    ASSERT_TRUE(OCI_DateFree(date2));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestDate, AddMonths)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto date1 = OCI_DateCreate(nullptr);
    const auto date2 = OCI_DateCreate(nullptr);

    ASSERT_TRUE(OCI_DateSysDate(date1));
    ASSERT_TRUE(OCI_DateAssign(date2, date1));
    ASSERT_TRUE(OCI_DateAddMonths(date2, 5));
    ASSERT_TRUE(OCI_DateAddMonths(date2, -5));
    ASSERT_EQ(0, OCI_DateCompare(date1, date2));

    ASSERT_TRUE(OCI_DateFree(date1));
    ASSERT_TRUE(OCI_DateFree(date2));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestDate, CompareInferior)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto date1 = OCI_DateCreate(nullptr);
    const auto date2 = OCI_DateCreate(nullptr);

    ASSERT_TRUE(OCI_DateSysDate(date1));
    ASSERT_TRUE(OCI_DateAssign(date2, date1));
    ASSERT_TRUE(OCI_DateAddDays(date2, 3));
    ASSERT_EQ(-1, OCI_DateCompare(date1, date2));

    ASSERT_TRUE(OCI_DateFree(date1));
    ASSERT_TRUE(OCI_DateFree(date2));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestDate, CompareSuperior)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto date1 = OCI_DateCreate(nullptr);
    const auto date2 = OCI_DateCreate(nullptr);

    ASSERT_TRUE(OCI_DateSysDate(date1));
    ASSERT_TRUE(OCI_DateAssign(date2, date1));
    ASSERT_TRUE(OCI_DateAddDays(date2, -3));
    ASSERT_EQ(1, OCI_DateCompare(date1, date2));

    ASSERT_TRUE(OCI_DateFree(date1));
    ASSERT_TRUE(OCI_DateFree(date2));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestDate, CompareEqual)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto date1 = OCI_DateCreate(nullptr);
    const auto date2 = OCI_DateCreate(nullptr);

    ASSERT_TRUE(OCI_DateSysDate(date1));
    ASSERT_TRUE(OCI_DateAssign(date2, date1));
    ASSERT_EQ(0, OCI_DateCompare(date1, date2));

    ASSERT_TRUE(OCI_DateFree(date1));
    ASSERT_TRUE(OCI_DateFree(date2));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestDate, DaysBetween)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto date1 = OCI_DateCreate(nullptr);
    const auto date2 = OCI_DateCreate(nullptr);

    ASSERT_TRUE(OCI_DateSysDate(date1));
    ASSERT_TRUE(OCI_DateAssign(date2, date1));
    ASSERT_TRUE(OCI_DateAddDays(date2, 14));
    ASSERT_EQ(14, OCI_DateDaysBetween(date2, date1));

    ASSERT_TRUE(OCI_DateFree(date1));
    ASSERT_TRUE(OCI_DateFree(date2));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestDate, FromTextDefaultFormat)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto date = OCI_DateCreate(nullptr);
    tm cdate {};

    ASSERT_TRUE(OCI_DateFromText(date, OTEXT("1978-04-13 20:30:15"), OCI_STRING_FORMAT_DATETIME));
    ASSERT_TRUE(OCI_DateToCTime(date, &cdate, NULL));    

    ASSERT_EQ(1978, (1900+cdate.tm_year));
    ASSERT_EQ(4, cdate.tm_mon+1);
    ASSERT_EQ(13, cdate.tm_mday);
    ASSERT_EQ(20, cdate.tm_hour);
    ASSERT_EQ(30, cdate.tm_min);
    ASSERT_EQ(15, cdate.tm_sec);

    ASSERT_TRUE(OCI_DateFree(date));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestDate, ToTextDefaultFormat)
{
    otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
    const int year = 1978, month = 4, day = 13, hour = 20, min = 30, sec = 15;

    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto date = OCI_DateCreate(nullptr);

    ASSERT_TRUE(OCI_DateSetDateTime(date, year, month, day, hour, min, sec));
    ASSERT_TRUE(OCI_DateToText(date, OCI_STRING_FORMAT_DATETIME, 512, buffer));

    ASSERT_EQ(ostring(OTEXT("1978-04-13 20:30:15")), ostring(buffer));

    ASSERT_TRUE(OCI_DateFree(date));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestDate, GetLastDay)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto date = OCI_DateCreate(nullptr);
    int year = 0, month = 0, day = 0;

    ASSERT_TRUE(OCI_DateFromText(date, OTEXT("1978-04-13 20:30:15"), OCI_STRING_FORMAT_DATETIME));
    ASSERT_TRUE(OCI_DateLastDay(date));
    ASSERT_TRUE(OCI_DateGetDate(date, &year, &month, &day));
    ASSERT_EQ(1978, year);
    ASSERT_EQ(4, month);
    ASSERT_EQ(30, day);

    ASSERT_TRUE(OCI_DateFree(date));

    ASSERT_TRUE(OCI_Cleanup());
}

#ifndef OCI_CHAR_WIDE

TEST(TestDate, GetNextDay)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto date = OCI_DateCreate(nullptr);
    int year = 0, month = 0, day = 0;

    ASSERT_TRUE(OCI_DateFromText(date, OTEXT("1978-04-13 20:30:15"), OCI_STRING_FORMAT_DATETIME));
    ASSERT_TRUE(OCI_DateNextDay(date, NLS_LANGUAGE_SUNDAY_NAME));
    ASSERT_TRUE(OCI_DateGetDate(date, &year, &month, &day));
    ASSERT_EQ(1978, year);
    ASSERT_EQ(4, month);
    ASSERT_EQ(16, day);

    ASSERT_TRUE(OCI_DateFree(date));

    ASSERT_TRUE(OCI_Cleanup());
}
#endif

TEST(TestDate, SetGetDateTime)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto date = OCI_DateCreate(nullptr);
    int year = 1978, month = 4, day = 13, hour = 20, min = 30, sec = 15;

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

    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestDate, DateFetch)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    const auto stmt = OCI_StatementCreate(conn);

    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select to_date('1978-04-13 20:30:15', 'YYYY-MM-DD HH24:MI:SS') from dual")));
    
    const auto rslt = OCI_GetResultset(stmt);
    ASSERT_TRUE(OCI_FetchNext(rslt));

    const auto date = OCI_GetDate(rslt, 1);
   
    otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
    ASSERT_TRUE(OCI_DateToText(date, OCI_STRING_FORMAT_DATETIME, 512, buffer));

    ASSERT_EQ(ostring(OTEXT("1978-04-13 20:30:15")), ostring(buffer));

    ASSERT_TRUE(OCI_StatementFree(stmt));
    ASSERT_TRUE(OCI_ConnectionFree(conn));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestDate, DateBindInOut)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto date = OCI_DateCreate(nullptr);
    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    const auto stmt = OCI_StatementCreate(conn);

    ASSERT_TRUE(OCI_DateFromText(date, OTEXT("1978-04-12 20:30:15"), OCI_STRING_FORMAT_DATETIME));
    ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("begin :date := :date + 1; end;")));
    ASSERT_TRUE(OCI_BindDate(stmt, OTEXT(":date"), date));
    ASSERT_TRUE(OCI_BindSetDirection(OCI_GetBind(stmt, 1), OCI_BDM_IN_OUT));
    ASSERT_TRUE(OCI_Execute(stmt));
 
    otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
    ASSERT_TRUE(OCI_DateToText(date, OCI_STRING_FORMAT_DATETIME, 512, buffer));

    ASSERT_EQ(ostring(OTEXT("1978-04-13 20:30:15")), ostring(buffer));

    ASSERT_TRUE(OCI_DateFree(date));
    ASSERT_TRUE(OCI_StatementFree(stmt));
    ASSERT_TRUE(OCI_ConnectionFree(conn));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestDate, DateBindRegister)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    const auto stmt = OCI_StatementCreate(conn);

    OCI_ExecuteStmt(stmt, OTEXT("drop table test_date_register"));
    OCI_ExecuteStmt(stmt, OTEXT("create table test_date_register (value date)"));

    ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into test_date_register values (to_date('1978-04-13 20:30:15', 'YYYY-MM-DD HH24:MI:SS')) returning value into :1")));
    ASSERT_TRUE(OCI_RegisterDate(stmt, OTEXT(":1")));
    ASSERT_TRUE(OCI_Execute(stmt));

    const auto rslt = OCI_GetResultset(stmt);
    ASSERT_TRUE(OCI_FetchNext(rslt));

    const auto date = OCI_GetDate(rslt, 1);

    otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
    ASSERT_TRUE(OCI_DateToText(date, OCI_STRING_FORMAT_DATETIME, 512, buffer));

    ASSERT_EQ(ostring(OTEXT("1978-04-13 20:30:15")), ostring(buffer));

    OCI_ExecuteStmt(stmt, OTEXT("drop table test_date_register"));

    ASSERT_TRUE(OCI_StatementFree(stmt));
    ASSERT_TRUE(OCI_ConnectionFree(conn));

    ASSERT_TRUE(OCI_Cleanup());
}