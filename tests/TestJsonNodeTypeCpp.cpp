#include "ocilib_tests.h"

#include <cstring>

using namespace ocilib;

namespace TestCppApi
{

    TEST(TestJsonNodeTypeCpp, TypeAndNull)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json json(con);
        json.SetMutable(true);
        json.LoadText(OTEXT("null"));

        JsonNode oldRoot = json.GetRoot();
        (void)oldRoot;

        JsonNode obj = json.CreateObject();
        ASSERT_EQ(Json::NodeObject, obj.GetType());
        ASSERT_FALSE(obj.IsNull());

        JsonNode arr = json.CreateArray();
        ASSERT_EQ(Json::NodeArray, arr.GetType());
        ASSERT_FALSE(arr.IsNull());

        JsonNode nul = json.CreateNull();
        ASSERT_EQ(Json::NodeNull, nul.GetType());
        ASSERT_TRUE(nul.IsNull());

        JsonNode str = json.CreateString(OTEXT("hello"));
        ASSERT_EQ(Json::NodeString, str.GetType());

        Number num(env, 1.5);
        JsonNode number = json.CreateNumber(num);
        ASSERT_EQ(Json::NodeNumber, number.GetType());

        JsonNode boolean = json.CreateBoolean(true);
        ASSERT_EQ(Json::NodeBoolean, boolean.GetType());

        Raw bin = { 0xAB, 0xCD };
        JsonNode binary = json.CreateBinary(bin);
        ASSERT_EQ(Json::NodeBinary, binary.GetType());

        Date date(env, true);
        date.SetDate(2024, 1, 1);
        JsonNode dateNode = json.CreateDate(date);
        ASSERT_EQ(Json::NodeDate, dateNode.GetType());

        Timestamp ts(env, Timestamp::NoTimeZone);
        ts.SetDateTime(2024, 1, 1, 12, 0, 0, 0);
        JsonNode tsNode = json.CreateTimestamp(ts);
        ASSERT_EQ(Json::NodeTimestamp, tsNode.GetType());

        Interval iv(env, Interval::YearMonth);
        iv.SetYearMonth(1, 2);
        JsonNode ivNode = json.CreateInterval(iv);
        ASSERT_EQ(Json::NodeInterval, ivNode.GetType());
    }
    TEST(TestJsonNodeTypeCpp, ScalarSetters)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json json(con);
        json.SetMutable(true);
        json.LoadText(OTEXT("null"));

        JsonNode oldRoot = json.GetRoot();
        (void)oldRoot;

        JsonNode str = json.CreateString(OTEXT("hello"));
        ASSERT_EQ(ostring(OTEXT("hello")), str.GetString());

        Number num(env, 1.5);
        JsonNode number = json.CreateNumber(num);

        JsonNode boolean = json.CreateBoolean(false);
        ASSERT_FALSE(boolean.GetBoolean());

        Raw bin1 = { 0xAB, 0xCD };
        Raw bin2 = { 0x12 };
        JsonNode binary = json.CreateBinary(bin1);
        Raw got = binary.GetBinary();
        ASSERT_EQ(bin1.size(), got.size());
        ASSERT_EQ(0, memcmp(bin1.data(), got.data(), got.size()));

        Date date(env, true);
        date.SetDate(2024, 5, 15);
        JsonNode dateNode = json.CreateDate(date);

        Timestamp ts(env, Timestamp::NoTimeZone);
        ts.SetDateTime(2024, 5, 15, 10, 30, 0, 0);
        JsonNode tsNode = json.CreateTimestamp(ts);

        Interval iv(env, Interval::YearMonth);
        iv.SetYearMonth(1, 6);
        JsonNode ivNode = json.CreateInterval(iv);

    }
}
