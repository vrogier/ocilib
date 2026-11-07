#include "ocilib_tests.h"

#include <cstring>

using namespace ocilib;

namespace TestCppApi
{

    static Json MakeJson(const Connection& con, const String& text)
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
        ASSERT_EQ(String(OTEXT("name")), verify.GetFieldName(0));

        JsonNode nameField = verify.GetFieldByName(OTEXT("name"));
        ASSERT_EQ(Json::NodeString, nameField.GetType());
        ASSERT_EQ(String(OTEXT("test")), nameField.GetString());

        JsonNode nameField2 = verify.GetField(OTEXT("name"));
        ASSERT_EQ(String(OTEXT("test")), nameField2.GetString());
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

        String fieldName = root.GetFieldName(0);
        ASSERT_FALSE(fieldName.empty());

        JsonNode name = root.GetFieldByName(OTEXT("name"));
        ASSERT_EQ(Json::NodeString, name.GetType());
        ASSERT_EQ(String(OTEXT("test")), name.GetString());

        JsonNode name2 = root.GetField(OTEXT("name"));
        ASSERT_EQ(String(OTEXT("test")), name2.GetString());

        JsonNode name3 = root[OTEXT("name")];
        ASSERT_EQ(String(OTEXT("test")), name3.GetString());

        JsonNode items = root.GetFieldByIndex(1);
        ASSERT_EQ(Json::NodeArray, items.GetType());
        ASSERT_EQ(3u, items.GetItemCount());

        JsonNode item2 = items.GetItem(1);
        ASSERT_EQ(Json::NodeNumber, item2.GetType());
        ASSERT_DOUBLE_EQ(2.0, static_cast<OCI_Double>(item2.GetNumeric<Number>()));

        JsonNode item3 = items[2];
        ASSERT_EQ(Json::NodeNumber, item3.GetType());
    }
    TEST(TestJsonDomCpp, JsonNodeIterator)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json json = MakeJson(con, OTEXT("{\"name\":\"test\",\"items\":[1,2,3]}"));
        json.SetMutable(true);

        JsonNode root = json.GetRoot();

        /* iterating an object node yields its fields in order */

        std::vector<String> fieldNames;
        OCI_UInt32 index = 0;
        for (const JsonNode& field : root)
        {
            fieldNames.push_back(root.GetFieldName(index++));
        }
        ASSERT_EQ(2u, fieldNames.size());
        ASSERT_EQ(String(OTEXT("name")), fieldNames[0]);
        ASSERT_EQ(String(OTEXT("items")), fieldNames[1]);

        /* iterating an array node yields its items in order */

        JsonNode items = root.GetFieldByName(OTEXT("items"));
        std::vector<Json::JsonType> itemTypes;
        for (const JsonNode& item : items)
        {
            itemTypes.push_back(item.GetType());
        }
        ASSERT_EQ(3u, itemTypes.size());
        ASSERT_TRUE(std::all_of(itemTypes.begin(), itemTypes.end(),
                                [](Json::JsonType t) { return t == Json::NodeNumber; }));

        /* scalar and null nodes produce an empty range */

        ASSERT_EQ(0u, std::distance(root.GetFieldByName(OTEXT("name")).begin(),
                                    root.GetFieldByName(OTEXT("name")).end()));
    }
    /* -------------------------------------------------------------------------
       Parameterized: typed node create + set/get round-trip per node type
       ------------------------------------------------------------------------- */

    struct JsonCppTypeCase
    {
        const char* name;
        void (*run)(Environment&, Json&, JsonNode&);
    };

    void PrintTo(const JsonCppTypeCase& c, std::ostream* os) { *os << c.name; }

    static void CppNodeString(Environment&, Json& json, JsonNode& root)
    {
        JsonNode n = json.CreateString(OTEXT("old"));
        n.SetString(OTEXT("new"));
        EXPECT_EQ(String(OTEXT("new")), n.GetString());
        root.SetField(OTEXT("str"), n);
    }
    static void CppNodeNumber(Environment& env, Json& json, JsonNode& root)
    {
        Number num(env, 3.14);
        JsonNode n = json.CreateNumber(num);
        EXPECT_EQ(Json::NodeNumber, n.GetType());
        EXPECT_DOUBLE_EQ(3.14, static_cast<OCI_Double>(n.GetNumeric<Number>()));
        num = 2.718;
        n.SetNumeric(num);
        EXPECT_DOUBLE_EQ(2.718, static_cast<OCI_Double>(n.GetNumeric<Number>()));
        root.SetField(OTEXT("num"), n);
    }
    static void CppNodeBoolean(Environment&, Json& json, JsonNode& root)
    {
        JsonNode n = json.CreateBoolean(true);
        ASSERT_TRUE(n.GetBoolean());
        n.SetBoolean(false);
        EXPECT_FALSE(n.GetBoolean());
        root.SetField(OTEXT("bool"), n);
    }
    static void CppNodeNull(Environment&, Json& json, JsonNode& root)
    {
        JsonNode n = json.CreateNull();
        ASSERT_TRUE(n.IsNull());
        n.SetNull();
        EXPECT_TRUE(n.IsNull());
        root.SetField(OTEXT("nil"), n);
    }
    static void CppNodeBinary(Environment&, Json& json, JsonNode& root)
    {
        Raw binData = { 0xAB, 0xCD, 0xEF };
        JsonNode n = json.CreateBinary(binData);
        Raw binVal = n.GetBinary();
        ASSERT_EQ(binData.size(), binVal.size());
        for (OCI_Size i = 0; i < binData.size(); ++i) { EXPECT_EQ(binData[i], binVal[i]); }
        Raw binData2 = { 0x12, 0x34 };
        n.SetBinary(binData2);
        binVal = n.GetBinary();
        ASSERT_EQ(binData2.size(), binVal.size());
        for (OCI_Size i = 0; i < binData2.size(); ++i) { EXPECT_EQ(binData2[i], binVal[i]); }
        root.SetField(OTEXT("bin"), n);
    }
    static void CppNodeDate(Environment& env, Json& json, JsonNode& root)
    {
        Date date(env, true);
        date.SetDate(2024, 5, 15);
        JsonNode n = json.CreateDate(date);
        EXPECT_EQ(Json::NodeDate, n.GetType());
        Date d2 = n.GetDate();
        OCI_Int32 y = 0, m = 0, d = 0;
        d2.GetDate(y, m, d);
        EXPECT_EQ(2024, y); EXPECT_EQ(5, m); EXPECT_EQ(15, d);
        Date date3(env, true);
        date3.SetDate(2025, 6, 20);
        n.SetDate(date3);
        d2 = n.GetDate();
        d2.GetDate(y, m, d);
        EXPECT_EQ(2025, y); EXPECT_EQ(6, m); EXPECT_EQ(20, d);
        root.SetField(OTEXT("date"), n);
    }
    static void CppNodeTimestamp(Environment& env, Json& json, JsonNode& root)
    {
        Timestamp ts(env, Timestamp::NoTimeZone);
        ts.SetDateTime(2024, 5, 15, 10, 30, 0, 0);
        JsonNode n = json.CreateTimestamp(ts);
        EXPECT_EQ(Json::NodeTimestamp, n.GetType());
        Timestamp ts2 = n.GetTimestamp();
        OCI_Int32 y = 0, m = 0, d = 0, h = 0, mn = 0, s = 0, f = 0;
        ts2.GetDateTime(y, m, d, h, mn, s, f);
        EXPECT_EQ(2024, y); EXPECT_EQ(5, m); EXPECT_EQ(15, d); EXPECT_EQ(10, h); EXPECT_EQ(30, mn);
        Timestamp ts3(env, Timestamp::NoTimeZone);
        ts3.SetDateTime(2025, 6, 20, 14, 45, 0, 0);
        n.SetTimestamp(ts3);
        ts2 = n.GetTimestamp();
        ts2.GetDateTime(y, m, d, h, mn, s, f);
        EXPECT_EQ(2025, y); EXPECT_EQ(6, m); EXPECT_EQ(20, d); EXPECT_EQ(14, h); EXPECT_EQ(45, mn);
        root.SetField(OTEXT("ts"), n);
    }
    static void CppNodeInterval(Environment& env, Json& json, JsonNode& root)
    {
        Interval iv(env, Interval::YearMonth);
        iv.SetYearMonth(1, 6);
        JsonNode n = json.CreateInterval(iv);
        EXPECT_EQ(Json::NodeInterval, n.GetType());
        Interval iv2 = n.GetInterval();
        OCI_Int32 y = 0, m = 0;
        iv2.GetYearMonth(y, m);
        EXPECT_EQ(1, y); EXPECT_EQ(6, m);
        Interval iv3(env, Interval::YearMonth);
        iv3.SetYearMonth(2, 3);
        n.SetInterval(iv3);
        iv2 = n.GetInterval();
        iv2.GetYearMonth(y, m);
        EXPECT_EQ(2, y); EXPECT_EQ(3, m);
        root.SetField(OTEXT("iv"), n);
    }

    class TestJsonDomCppCreate : public ::testing::TestWithParam<JsonCppTypeCase>
    {
    protected:
        Connection con;
        Json json;
        JsonNode root;

        void SetUp() override
        {
            con = Connection(GetEnvironmentCpp(), DBS, USR, PWD);
            json = Json(con);
            json.SetMutable(true);
            json.LoadText(OTEXT("null"));
            root = json.CreateObject();
        }
    };

    TEST_P(TestJsonDomCppCreate, CreateSetGetAndAttach_RoundTripsValue)
    {
        GetParam().run(GetEnvironmentCpp(), json, root);

        EXPECT_EQ(1u, root.GetFieldCount());
    }

    INSTANTIATE_TEST_CASE_P(NodeKind, TestJsonDomCppCreate,
        ::testing::Values(
            JsonCppTypeCase{ "String",    CppNodeString },
            JsonCppTypeCase{ "Number",    CppNodeNumber },
            JsonCppTypeCase{ "Boolean",   CppNodeBoolean },
            JsonCppTypeCase{ "Null",      CppNodeNull },
            JsonCppTypeCase{ "Binary",    CppNodeBinary },
            JsonCppTypeCase{ "Date",      CppNodeDate },
            JsonCppTypeCase{ "Timestamp", CppNodeTimestamp },
            JsonCppTypeCase{ "Interval",  CppNodeInterval }),
        ::testing::PrintToStringParamName());

    TEST_F(TestJsonDomCppCreate, CreateGenericNodeWithStringType_ReturnsStringNode)
    {
        JsonNode generic = json.CreateNode(Json::NodeString);

        EXPECT_EQ(Json::NodeString, generic.GetType());
        generic.SetString(OTEXT("generic"));
        EXPECT_EQ(String(OTEXT("generic")), generic.GetString());

        root.SetField(OTEXT("generic"), generic);
    }

    TEST_F(TestJsonDomCppCreate, ArrayOpsOnCreatedArray_ManageItems)
    {
        JsonNode arr = json.CreateArray();
        EXPECT_EQ(Json::NodeArray, arr.GetType());
        Number n1(GetEnvironmentCpp(), 1.0);
        Number n2(GetEnvironmentCpp(), 2.0);
        JsonNode item1 = json.CreateNumber(n1);
        JsonNode item2 = json.CreateNumber(n2);
        JsonNode item3 = json.CreateString(OTEXT("three"));

        arr.AppendItem(item1);
        arr.AppendItem(item2);
        arr.SetItem(1, item3);

        EXPECT_EQ(3u, arr.GetItemCount());
        JsonNode chk = arr.GetItem(1);
        EXPECT_EQ(Json::NodeString, chk.GetType());
        EXPECT_EQ(String(OTEXT("three")), chk.GetString());

        root.SetField(OTEXT("arr"), arr);
    }

    TEST_F(TestJsonDomCppCreate, SetRootThenGetRoot_ReturnsAttachedDoc)
    {
        json.SetRoot(root);

        EXPECT_NE(JsonNode(), json.GetRoot());
    }

    /* -------------------------------------------------------------------------
       Shared fixture: mutable {} document for DOM mutation ops
       ------------------------------------------------------------------------- */

    class TestJsonDomCppOps : public ::testing::Test
    {
    protected:
        Connection con;
        Json json;

        void SetUp() override
        {
            con = Connection(GetEnvironmentCpp(), DBS, USR, PWD);
            json = Json(con);
            json.SetMutable(true);
            json.LoadText(OTEXT("{}"));
        }
    };

    TEST_F(TestJsonDomCppOps, SetFieldOnObject_AddsFieldWithValue)
    {
        JsonNode obj = json.CreateObject();
        JsonNode name = json.CreateString(OTEXT("test"));

        obj.SetField(OTEXT("name"), name);

        EXPECT_EQ(1u, obj.GetFieldCount());
        EXPECT_EQ(String(OTEXT("test")), obj.GetField(OTEXT("name")).GetString());
    }

    TEST_F(TestJsonDomCppOps, RemoveFieldOnObject_RemovesField)
    {
        JsonNode obj = json.CreateObject();
        obj.SetField(OTEXT("name"), json.CreateString(OTEXT("test")));

        obj.RemoveField(OTEXT("name"));

        EXPECT_EQ(0u, obj.GetFieldCount());
    }

    TEST_F(TestJsonDomCppOps, AppendItemOnArray_AddsItemAtEnd)
    {
        JsonNode arr = json.CreateArray();
        arr.AppendItem(json.CreateString(OTEXT("one")));

        arr.AppendItem(json.CreateString(OTEXT("three")));

        EXPECT_EQ(2u, arr.GetItemCount());
    }

    TEST_F(TestJsonDomCppOps, InsertItemOnArray_InsertsItemAtIndex)
    {
        JsonNode arr = json.CreateArray();
        arr.AppendItem(json.CreateString(OTEXT("one")));
        arr.AppendItem(json.CreateString(OTEXT("three")));

        arr.InsertItem(1, json.CreateString(OTEXT("two")));

        EXPECT_EQ(3u, arr.GetItemCount());
        EXPECT_EQ(String(OTEXT("two")), arr.GetItem(1).GetString());
    }

    TEST_F(TestJsonDomCppOps, RemoveItemOnArray_RemovesItemAtIndex)
    {
        JsonNode arr = json.CreateArray();
        arr.AppendItem(json.CreateString(OTEXT("one")));
        arr.AppendItem(json.CreateString(OTEXT("two")));
        arr.AppendItem(json.CreateString(OTEXT("three")));

        arr.RemoveItem(0);

        EXPECT_EQ(2u, arr.GetItemCount());
        EXPECT_EQ(String(OTEXT("two")), arr.GetItem(0).GetString());
    }

    TEST_F(TestJsonDomCppOps, SetNumericIntOnNumberNode_RoundTripsValue)
    {
        JsonNode num = json.CreateNode(Json::NodeNumber);

        num.SetNumeric(Int32(-7));

        EXPECT_EQ(-7, num.GetNumeric<Int32>());
    }

    TEST_F(TestJsonDomCppOps, SetNumericDoubleOnNumberNode_RoundTripsValue)
    {
        JsonNode num = json.CreateNode(Json::NodeNumber);

        num.SetNumeric(2.5);

        EXPECT_DOUBLE_EQ(2.5, num.GetNumeric<Double>());
    }

    TEST_F(TestJsonDomCppOps, ClearOnArray_RemovesAllItems)
    {
        JsonNode arr = json.CreateArray();
        arr.AppendItem(json.CreateString(OTEXT("one")));

        arr.Clear();

        EXPECT_EQ(0u, arr.GetItemCount());
    }

    TEST_F(TestJsonDomCppOps, ClearOnObject_RemovesAllFields)
    {
        JsonNode obj = json.CreateObject();
        obj.SetField(OTEXT("name"), json.CreateString(OTEXT("test")));

        obj.Clear();

        EXPECT_EQ(0u, obj.GetFieldCount());
    }

    TEST(TestJsonDomCpp, Validate)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json schema(con);
        schema.LoadText(OTEXT("{\"type\":\"object\",\"properties\":{\"a\":{\"type\":\"number\"}},\"required\":[\"a\"]}"));

        Json ok(con);
        ok.LoadText(OTEXT("{\"a\":1}"));

        ASSERT_TRUE(ok.Validate(schema));

        Json errors;
        ASSERT_TRUE(ok.Validate(schema, errors));

        Json bad(con);
        bad.LoadText(OTEXT("{\"a\":\"notanum\"}"));

        ASSERT_FALSE(bad.Validate(schema));
        ASSERT_FALSE(bad.Validate(schema, errors));
        ASSERT_FALSE(errors.ToString().empty());
    }
}