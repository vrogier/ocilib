#include "ocilib_tests.h"

#include <cstring>

using namespace ocilib;

namespace TestCppApi
{

    TEST(TestJsonNodeArrayCpp, ArrayAccess)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json json(con);
        json.SetMutable(true);
        json.LoadText(OTEXT("null"));

        JsonNode arr = json.CreateArray();

        JsonNode item1 = json.CreateString(OTEXT("first"));
        JsonNode item2 = json.CreateString(OTEXT("second"));

        arr.AppendItem(item1);
        arr.AppendItem(item2);

        ASSERT_EQ(2u, arr.GetItemCount());

        JsonNode i1 = arr.GetItem(1);
        ASSERT_EQ(Json::NodeString, i1.GetType());
        ASSERT_EQ(String(OTEXT("first")), i1.GetString());

        JsonNode i2 = arr.GetItem(2);
        ASSERT_EQ(Json::NodeString, i2.GetType());
        ASSERT_EQ(String(OTEXT("second")), i2.GetString());

        JsonNode v1 = arr.GetItem(1);
        ASSERT_EQ(Json::NodeString, v1.GetType());
        ASSERT_EQ(String(OTEXT("first")), v1.GetString());

        json.SetRoot(arr);

        JsonNode verify = json.GetRoot();
        ASSERT_EQ(2u, verify.GetItemCount());
    }
    TEST(TestJsonNodeArrayCpp, BadPaths)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json json(con);
        json.SetMutable(true);
        json.LoadText(OTEXT("null"));

        JsonNode scalar = json.CreateString(OTEXT("hello"));
        JsonNode obj = json.CreateObject();
        JsonNode arr = json.CreateArray();
        JsonNode child = json.CreateString(OTEXT("child"));

        ASSERT_THROW((void) scalar.GetFieldByName(OTEXT("x")), Exception);
        ASSERT_THROW((void) scalar.GetFieldByIndex(1), Exception);
        ASSERT_THROW((void) scalar.GetFieldName(1), Exception);
        ASSERT_THROW((void) scalar.SetField(OTEXT("x"), child), Exception);

        ASSERT_THROW((void) obj.GetItem(1), Exception);
        ASSERT_THROW((void) obj.AppendItem(child), Exception);
        ASSERT_THROW((void) obj.SetItem(1, child), Exception);

        ASSERT_THROW((void) arr.SetField(OTEXT("x"), child), Exception);
        ASSERT_THROW((void) arr.GetFieldByName(OTEXT("x")), Exception);
        ASSERT_THROW((void) arr.GetFieldByIndex(1), Exception);

        JsonNode number = json.CreateNumber(Number(env, 1.0));
        ASSERT_THROW((void) number.GetString(), Exception);
        ASSERT_THROW((void) scalar.GetNumeric<Number>(), Exception);
        ASSERT_THROW((void) number.GetBoolean(), Exception);
        ASSERT_THROW((void) number.GetBinary(), Exception);
        ASSERT_THROW((void) number.GetDate(), Exception);
        ASSERT_THROW((void) number.GetTimestamp(), Exception);
        ASSERT_THROW((void) number.GetInterval(), Exception);
        ASSERT_THROW((void) number.SetString(OTEXT("x")), Exception);

        JsonNode arr2 = json.CreateArray();
        JsonNode one = json.CreateString(OTEXT("only"));
        arr2.AppendItem(one);

        ASSERT_THROW((void) arr2.GetItem(100), Exception);
        ASSERT_THROW((void) arr2.SetItem(100, child), Exception);

        JsonNode obj2 = json.CreateObject();
        obj2.SetField(OTEXT("f"), child);

        ASSERT_THROW((void) obj2.GetFieldByIndex(100), Exception);
        ASSERT_THROW((void) obj2.GetFieldName(100), Exception);
    }
}
