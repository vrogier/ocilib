#include "ocilib_tests.h"
#include "../include/ocilib.h"

class TestTimestamp : public ::testing::TestWithParam<unsigned int>{};

std::vector<unsigned int> TimestampTypes{ OCI_TIMESTAMP, OCI_TIMESTAMP_TZ, OCI_TIMESTAMP_LTZ };


TEST_P(TestTimestamp, CreateAndFree)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto tmsp = OCI_TimestampCreate(nullptr, GetParam());
    ASSERT_EQ(GetParam(), OCI_TimestampGetType(tmsp));
    ASSERT_TRUE(OCI_TimestampFree(tmsp));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST_P(TestTimestamp, ArrayCreateAndFree)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto tmsps = OCI_TimestampArrayCreate(nullptr, GetParam(), ARRAY_SIZE);
    ASSERT_TRUE(nullptr != tmsps);

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        ASSERT_TRUE(nullptr != tmsps[i]);
        ASSERT_EQ(GetParam(), OCI_TimestampGetType(tmsps[i]));
    }
    ASSERT_TRUE(OCI_TimestampArrayFree(tmsps));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST_P(TestTimestamp, Assign)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto tmsp1 = OCI_TimestampCreate(nullptr, GetParam());
    const auto tmsp2 = OCI_TimestampCreate(nullptr, GetParam());

    ASSERT_TRUE(OCI_TimestampSysTimestamp(tmsp1));
    ASSERT_TRUE(OCI_TimestampAssign(tmsp2, tmsp1));
    ASSERT_EQ(0, OCI_TimestampCompare(tmsp1, tmsp2));

    ASSERT_TRUE(OCI_TimestampFree(tmsp1));
    ASSERT_TRUE(OCI_TimestampFree(tmsp2));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST_P(TestTimestamp, CompareInferiorSuperior)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto tmsp1 = OCI_TimestampCreate(nullptr, GetParam());
    const auto tmsp2 = OCI_TimestampCreate(nullptr, GetParam());

    ASSERT_TRUE(OCI_TimestampSysTimestamp(tmsp1));
    Sleep(100);
    ASSERT_TRUE(OCI_TimestampSysTimestamp(tmsp2));
    ASSERT_EQ(-1, OCI_TimestampCompare(tmsp1, tmsp2));
    ASSERT_EQ(1, OCI_TimestampCompare(tmsp2, tmsp1));

    ASSERT_TRUE(OCI_TimestampFree(tmsp1));
    ASSERT_TRUE(OCI_TimestampFree(tmsp2));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST_P(TestTimestamp, CompareEqual)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto tmsp1 = OCI_TimestampCreate(nullptr, GetParam());
    const auto tmsp2 = OCI_TimestampCreate(nullptr, GetParam());

    ASSERT_TRUE(OCI_TimestampSysTimestamp(tmsp1));
    ASSERT_TRUE(OCI_TimestampAssign(tmsp2, tmsp1));
    ASSERT_EQ(0, OCI_TimestampCompare(tmsp1, tmsp2));

    ASSERT_TRUE(OCI_TimestampFree(tmsp1));
    ASSERT_TRUE(OCI_TimestampFree(tmsp2));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST_P(TestTimestamp, FromTextDefaultFormat)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto tmsp = OCI_TimestampCreate(nullptr, GetParam());
    int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0, msec = 0;

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

    ASSERT_TRUE(OCI_Cleanup());
}

TEST_P(TestTimestamp, ToTextDefaultFormat)
{
    otext buffer[OCI_SIZE_BUFFER] = OTEXT("");

    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto tmsp = OCI_TimestampCreate(nullptr, GetParam());

    ASSERT_TRUE(OCI_TimestampFromText(tmsp, OTEXT("1978-04-13 20:30:15.000000300"), OCI_STRING_FORMAT_TIMESTAMP));
    ASSERT_TRUE(OCI_TimestampToText(tmsp, OCI_STRING_FORMAT_TIMESTAMP, 512, buffer, 9));

    ASSERT_EQ(ostring(OTEXT("1978-04-13 20:30:15.000000300")), ostring(buffer));

    ASSERT_TRUE(OCI_TimestampFree(tmsp));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST_P(TestTimestamp, AddInterval)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto tmsp = OCI_TimestampCreate(nullptr, GetParam());
    const auto itv  = OCI_IntervalCreate(nullptr, OCI_INTERVAL_DS);
    int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0, msec = 0;

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

    ASSERT_TRUE(OCI_Cleanup());
}

TEST_P(TestTimestamp, SubInterval)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto tmsp = OCI_TimestampCreate(nullptr, GetParam());
    const auto itv = OCI_IntervalCreate(nullptr, OCI_INTERVAL_DS);
    int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0, msec = 0;

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

    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestTimestamp, TimestampFetch)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
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

    ASSERT_TRUE(OCI_Cleanup());
}

TEST_P(TestTimestamp, TimestampBindInOut)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto tmsp = OCI_TimestampCreate(nullptr, GetParam());
    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    const auto stmt = OCI_StatementCreate(conn);

    ASSERT_TRUE(OCI_TimestampFromText(tmsp, OTEXT("1978-04-13 19:30:15.000000300"), OCI_STRING_FORMAT_TIMESTAMP));
 
    ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("begin :tmsp := :tmsp + to_dsinterval('1 01:00:00'); end;")));
    ASSERT_TRUE(OCI_BindTimestamp(stmt, OTEXT(":tmsp"), tmsp));
    ASSERT_TRUE(OCI_BindSetDirection(OCI_GetBind(stmt, 1), OCI_BDM_IN_OUT));
    ASSERT_TRUE(OCI_Execute(stmt));

    otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
    ASSERT_TRUE(OCI_TimestampToText(tmsp, OCI_STRING_FORMAT_TIMESTAMP, 512, buffer, 9));

    ASSERT_EQ(ostring(OTEXT("1978-04-14 20:30:15.000000300")), ostring(buffer));

    ASSERT_TRUE(OCI_TimestampFree(tmsp));
    ASSERT_TRUE(OCI_StatementFree(stmt));
    ASSERT_TRUE(OCI_ConnectionFree(conn));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestTimestamp, TimestampBindRegister)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
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

    ASSERT_TRUE(OCI_Cleanup());
}


INSTANTIATE_TEST_CASE_P(TestTimestamp, TestTimestamp, ::testing::ValuesIn(TimestampTypes));
