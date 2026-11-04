#include "ocilib_tests.h"

#include <cstring>

using namespace ocilib;

namespace TestCppApi
{

    static Json MakeJson(const Connection& con, const ostring& text)
    {
        Json json(con);
        json.LoadText(text);
        return json;
    }

    TEST(TestJsonDomCpp, MutableAndRoot)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json json(con);
        json.SetMutable(true);
        ASSERT_TRUE(json.IsMutable());

        json.LoadText(OTEXT("null"));
        JsonNode oldRoot = json.GetRoot();
        ASSERT_EQ(Json::NodeNull, oldRoot.GetType());
        ASSERT_TRUE(oldRoot.IsNull());

        JsonNode root = json.CreateObject();
        JsonNode name = json.CreateString(OTEXT("test"));
        root.SetField(OTEXT("name"), name);
        json.SetRoot(root);

        JsonNode verify = json.GetRoot();
        ASSERT_EQ(Json::NodeObject, verify.GetType());
        ASSERT_EQ(1u, verify.GetFieldCount());
        ASSERT_EQ(ostring(OTEXT("name")), verify.GetFieldName(1));

        JsonNode nameField = verify.GetFieldByName(OTEXT("name"));
        ASSERT_EQ(Json::NodeString, nameField.GetType());
        ASSERT_EQ(ostring(OTEXT("test")), nameField.GetString());

        JsonNode nameField2 = verify.GetField(OTEXT("name"));
        ASSERT_EQ(ostring(OTEXT("test")), nameField2.GetString());
    }
    TEST(TestJsonDomCpp, DomNavigation)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json json = MakeJson(con, OTEXT("{\"name\":\"test\",\"items\":[1,2,3]}"));
        json.SetMutable(true);

        JsonNode root = json.GetRoot();
        ASSERT_EQ(Json::NodeObject, root.GetType());
        ASSERT_EQ(2u, root.GetFieldCount());

        ostring fieldName = root.GetFieldName(1);
        ASSERT_FALSE(fieldName.empty());

        JsonNode name = root.GetFieldByName(OTEXT("name"));
        ASSERT_EQ(Json::NodeString, name.GetType());
        ASSERT_EQ(ostring(OTEXT("test")), name.GetString());

        JsonNode name2 = root.GetField(OTEXT("name"));
        ASSERT_EQ(ostring(OTEXT("test")), name2.GetString());

        JsonNode name3 = root[OTEXT("name")];
        ASSERT_EQ(ostring(OTEXT("test")), name3.GetString());

        JsonNode items = root.GetFieldByIndex(1);
        ASSERT_EQ(Json::NodeArray, items.GetType());
        ASSERT_EQ(3u, items.GetItemCount());

        JsonNode item2 = items.GetItem(1);
        ASSERT_EQ(Json::NodeNumber, item2.GetType());
        ASSERT_DOUBLE_EQ(2.0, static_cast<OCI_Double>(item2.GetNumeric<Number>()));

        JsonNode item3 = items[2];
        ASSERT_EQ(Json::NodeNumber, item3.GetType());
    }
    TEST(TestJsonDomCpp, NodeCreationAndSetters)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json json(con);
        json.SetMutable(true);
        json.LoadText(OTEXT("null"));

        JsonNode root = json.CreateObject();

        JsonNode generic = json.CreateNode(Json::NodeString);
        ASSERT_EQ(Json::NodeString, generic.GetType());
        generic.SetString(OTEXT("generic"));
        ASSERT_EQ(ostring(OTEXT("generic")), generic.GetString());
        root.SetField(OTEXT("generic"), generic);

        JsonNode str = json.CreateString(OTEXT("old"));
        str.SetString(OTEXT("new"));
        ASSERT_EQ(ostring(OTEXT("new")), str.GetString());
        root.SetField(OTEXT("str"), str);

        Number num(env, 3.14);
        JsonNode numNode = json.CreateNumber(num);
        ASSERT_EQ(Json::NodeNumber, numNode.GetType());
        ASSERT_DOUBLE_EQ(3.14, static_cast<OCI_Double>(numNode.GetNumeric<Number>()));

        num = 2.718;
        numNode.SetNumeric(num);
        ASSERT_DOUBLE_EQ(2.718, static_cast<OCI_Double>(numNode.GetNumeric<Number>()));
        root.SetField(OTEXT("num"), numNode);

        JsonNode OCI_Boolean = json.CreateBoolean(true);
        ASSERT_TRUE(OCI_Boolean.GetBoolean());
        OCI_Boolean.SetBoolean(false);
        ASSERT_FALSE(OCI_Boolean.GetBoolean());
        root.SetField(OTEXT("bool"), OCI_Boolean);

        JsonNode nullNode = json.CreateNull();
        ASSERT_TRUE(nullNode.IsNull());
        nullNode.SetNull();
        ASSERT_TRUE(nullNode.IsNull());
        root.SetField(OTEXT("nil"), nullNode);

        Raw binData = { 0xAB, 0xCD, 0xEF };
        JsonNode binary = json.CreateBinary(binData);
        Raw binVal = binary.GetBinary();
        ASSERT_EQ(binData.size(), binVal.size());
        for (OCI_Size i = 0; i < binData.size(); ++i)
        {
            ASSERT_EQ(binData[i], binVal[i]);
        }

        Raw binData2 = { 0x12, 0x34 };
        binary.SetBinary(binData2);
        binVal = binary.GetBinary();
        ASSERT_EQ(binData2.size(), binVal.size());
        for (OCI_Size i = 0; i < binData2.size(); ++i)
        {
            ASSERT_EQ(binData2[i], binVal[i]);
        }
        root.SetField(OTEXT("bin"), binary);

        Date date(env, true);
        date.SetDate(2024, 5, 15);
        JsonNode dateNode = json.CreateDate(date);
        ASSERT_EQ(Json::NodeDate, dateNode.GetType());

        Date date2 = dateNode.GetDate();
        OCI_Int32 y = 0, m = 0, d = 0;
        date2.GetDate(y, m, d);
        ASSERT_EQ(2024, y);
        ASSERT_EQ(5, m);
        ASSERT_EQ(15, d);

        Date date3(env, true);
        date3.SetDate(2025, 6, 20);
        dateNode.SetDate(date3);
        date2 = dateNode.GetDate();
        date2.GetDate(y, m, d);
        ASSERT_EQ(2025, y);
        ASSERT_EQ(6, m);
        ASSERT_EQ(20, d);
        root.SetField(OTEXT("date"), dateNode);

        Timestamp ts(env, Timestamp::NoTimeZone);
        ts.SetDateTime(2024, 5, 15, 10, 30, 0, 0);
        JsonNode tsNode = json.CreateTimestamp(ts);
        ASSERT_EQ(Json::NodeTimestamp, tsNode.GetType());

        Timestamp ts2 = tsNode.GetTimestamp();
        OCI_Int32 h = 0, mn = 0, s = 0, f = 0;
        ts2.GetDateTime(y, m, d, h, mn, s, f);
        ASSERT_EQ(2024, y);
        ASSERT_EQ(5, m);
        ASSERT_EQ(15, d);
        ASSERT_EQ(10, h);
        ASSERT_EQ(30, mn);

        Timestamp ts3(env, Timestamp::NoTimeZone);
        ts3.SetDateTime(2025, 6, 20, 14, 45, 0, 0);
        tsNode.SetTimestamp(ts3);
        ts2 = tsNode.GetTimestamp();
        ts2.GetDateTime(y, m, d, h, mn, s, f);
        ASSERT_EQ(2025, y);
        ASSERT_EQ(6, m);
        ASSERT_EQ(20, d);
        ASSERT_EQ(14, h);
        ASSERT_EQ(45, mn);
        root.SetField(OTEXT("ts"), tsNode);

        Interval iv(env, Interval::YearMonth);
        iv.SetYearMonth(1, 6);
        JsonNode ivNode = json.CreateInterval(iv);
        ASSERT_EQ(Json::NodeInterval, ivNode.GetType());

        Interval iv2 = ivNode.GetInterval();
        iv2.GetYearMonth(y, m);
        ASSERT_EQ(1, y);
        ASSERT_EQ(6, m);

        Interval iv3(env, Interval::YearMonth);
        iv3.SetYearMonth(2, 3);
        ivNode.SetInterval(iv3);
        iv2 = ivNode.GetInterval();
        iv2.GetYearMonth(y, m);
        ASSERT_EQ(2, y);
        ASSERT_EQ(3, m);
        root.SetField(OTEXT("iv"), ivNode);

        JsonNode arr = json.CreateArray();
        ASSERT_EQ(Json::NodeArray, arr.GetType());

        Number n1(env, 1.0);
        JsonNode item1 = json.CreateNumber(n1);
        Number n2(env, 2.0);
        JsonNode item2 = json.CreateNumber(n2);
        arr.AppendItem(item1);
        arr.AppendItem(item2);

        JsonNode item3 = json.CreateString(OTEXT("three"));
        arr.SetItem(1, item3);
        ASSERT_EQ(3u, arr.GetItemCount());

        JsonNode verifyItem = arr.GetItem(2);
        ASSERT_EQ(Json::NodeString, verifyItem.GetType());
        ASSERT_EQ(ostring(OTEXT("three")), verifyItem.GetString());

        root.SetField(OTEXT("arr"), arr);

        ASSERT_EQ(10u, root.GetFieldCount());

        json.SetRoot(root);

        JsonNode verify = json.GetRoot();
        ASSERT_EQ(3u, verify.GetFieldByName(OTEXT("arr")).GetItemCount());
    }

    TEST(TestJsonDomCpp, DomMutationOps)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json json(con);
        json.SetMutable(true);
        json.LoadText(OTEXT("{}"));

        /* object ops on a detached node */
        JsonNode obj = json.CreateObject();
        JsonNode name = json.CreateString(OTEXT("test"));
        obj.SetField(OTEXT("name"), name);
        ASSERT_EQ(1u, obj.GetFieldCount());
        JsonNode back = obj.GetField(OTEXT("name"));
        ASSERT_EQ(ostring(OTEXT("test")), back.GetString());
        obj.RemoveField(OTEXT("name"));
        ASSERT_EQ(0u, obj.GetFieldCount());

        /* array ops on a detached node */
        JsonNode arr = json.CreateArray();
        JsonNode s1  = json.CreateString(OTEXT("one"));
        JsonNode s3  = json.CreateString(OTEXT("three"));
        arr.AppendItem(s1);
        arr.AppendItem(s3);
        ASSERT_EQ(2u, arr.GetItemCount());

        JsonNode s2 = json.CreateString(OTEXT("two"));
        arr.InsertItem(1, s2);
        ASSERT_EQ(3u, arr.GetItemCount());
        ASSERT_EQ(ostring(OTEXT("two")), arr.GetItem(1).GetString());
        arr.RemoveItem(1);
        ASSERT_EQ(2u, arr.GetItemCount());

        /* typed accessors */
        JsonNode num = json.CreateNode(Json::NodeNumber);
        num.SetNumeric(Int32(-7));
        ASSERT_EQ(-7, num.GetNumeric<Int32>());
        num.SetNumeric(2.5);
        ASSERT_DOUBLE_EQ(2.5, num.GetNumeric<Double>());

        /* Clear */
        arr.Clear();
        ASSERT_EQ(0u, arr.GetItemCount());
        obj.Clear();
        ASSERT_EQ(0u, obj.GetFieldCount());
    }
}