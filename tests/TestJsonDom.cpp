#include "ocilib_tests.h"
#include "NullArgs.h"

#include <cstring>

using namespace ocilib;

namespace
{
    OCI_Json* JsonFromText(OCI_Connection* conn, const otext* text)
    {
        OCI_Json* json = OCI_JsonCreate(conn);
        if (!json) return nullptr;
        if (!OCI_JsonLoadText(json, text, 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT))
        {
            OCI_JsonFree(json);
            return nullptr;
        }
        return json;
    }

}

namespace TestCApi
{
    TEST(TestJsonDom, MutableAndRoot)
    {
        auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);
        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
        ASSERT_TRUE(OCI_JsonIsMutable(json));

        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));

        OCI_JsonNode* oldRoot = OCI_JsonGetRoot(json);
        ASSERT_NE(nullptr, oldRoot);
        ASSERT_EQ(OCI_JNT_NULL, OCI_JsonNodeGetType(oldRoot));
        ASSERT_TRUE(OCI_JsonNodeIsNull(oldRoot));
        OCI_JsonNodeFree(oldRoot);

        OCI_JsonNode* root = OCI_JsonNodeCreateObject(json);
        ASSERT_NE(nullptr, root);

        OCI_JsonNode* name = OCI_JsonNodeCreateString(json, OTEXT("test"), 0);
        ASSERT_NE(nullptr, name);
        ASSERT_TRUE(OCI_JsonNodeSetField(root, OTEXT("name"), name));

        ASSERT_TRUE(OCI_JsonSetRoot(json, root));

        OCI_JsonNodeFree(name);
        OCI_JsonNodeFree(root);

        OCI_JsonNode* verify = OCI_JsonGetRoot(json);
        ASSERT_NE(nullptr, verify);
        ASSERT_EQ(OCI_JNT_OBJECT, OCI_JsonNodeGetType(verify));
        ASSERT_EQ(1u, OCI_JsonNodeGetFieldCount(verify));

        OCI_JsonNode* nameField = OCI_JsonNodeGetFieldByName(verify, OTEXT("name"));
        ASSERT_NE(nullptr, nameField);
        ASSERT_EQ(OCI_JNT_STRING, OCI_JsonNodeGetType(nameField));
        ASSERT_EQ(String(OTEXT("test")), String(OCI_JsonNodeGetString(nameField, nullptr)));

        OCI_JsonNodeFree(nameField);
        OCI_JsonNodeFree(verify);
        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestJsonDom, DomNavigation)
    {
        auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = JsonFromText(conn, OTEXT("{\"name\":\"test\",\"items\":[1,2,3]}"));
        ASSERT_NE(nullptr, json);

        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));

        OCI_JsonNode* root = OCI_JsonGetRoot(json);
        ASSERT_NE(nullptr, root);
        ASSERT_EQ(OCI_JNT_OBJECT, OCI_JsonNodeGetType(root));
        ASSERT_EQ(2u, OCI_JsonNodeGetFieldCount(root));

        const otext* fieldName = OCI_JsonNodeGetFieldName(root, 1);
        ASSERT_NE(nullptr, fieldName);
        ASSERT_FALSE(String(fieldName).empty());

        OCI_JsonNode* name = OCI_JsonNodeGetFieldByName(root, OTEXT("name"));
        ASSERT_NE(nullptr, name);
        ASSERT_EQ(OCI_JNT_STRING, OCI_JsonNodeGetType(name));
        ASSERT_EQ(String(OTEXT("test")), String(OCI_JsonNodeGetString(name, nullptr)));

        OCI_JsonNode* items = OCI_JsonNodeGetFieldByIndex(root, 1);
        ASSERT_NE(nullptr, items);
        ASSERT_EQ(OCI_JNT_ARRAY, OCI_JsonNodeGetType(items));
        ASSERT_EQ(3u, OCI_JsonNodeGetItemCount(items));

        OCI_JsonNode* item2 = OCI_JsonNodeGetItem(items, 1);
        ASSERT_NE(nullptr, item2);
        ASSERT_EQ(OCI_JNT_NUMBER, OCI_JsonNodeGetType(item2));

        OCI_Number* num = OCI_JsonNodeGetNumber(item2);
        ASSERT_NE(nullptr, num);
        OCI_Double val = 0.0;
        ASSERT_TRUE(OCI_NumberGetValue(num, OCI_NUM_DOUBLE, &val));
        ASSERT_DOUBLE_EQ(2.0, val);
        ASSERT_TRUE(OCI_NumberFree(num));

        OCI_JsonNodeFree(item2);
        OCI_JsonNodeFree(items);
        OCI_JsonNodeFree(name);
        OCI_JsonNodeFree(root);
        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    /* -------------------------------------------------------------------------
       Parameterized: create/get/set round-trip per JSON node type
       ------------------------------------------------------------------------- */

    struct JsonNodeTypeCase
    {
        const char* name;
        void (*run)(OCI_Environment*, OCI_Json*, OCI_JsonNode*);
    };

    void PrintTo(const JsonNodeTypeCase& c, std::ostream* os) { *os << c.name; }

    static void NodeString(OCI_Environment*, OCI_Json* json, OCI_JsonNode* root)
    {
        OCI_JsonNode* n = OCI_JsonNodeCreateString(json, OTEXT("old"), 0);
        ASSERT_NE(nullptr, n);
        ASSERT_TRUE(OCI_JsonNodeSetString(n, OTEXT("new"), 0));
        EXPECT_EQ(String(OTEXT("new")), String(OCI_JsonNodeGetString(n, nullptr)));
        OCI_JsonNodeSetField(root, OTEXT("str"), n);
        OCI_JsonNodeFree(n);
    }
    static void NodeNumber(OCI_Environment* env, OCI_Json* json, OCI_JsonNode* root)
    {
        OCI_Number* num = OCI_NumberCreate(env);
        ASSERT_NE(nullptr, num);
        OCI_Double v = 3.14;
        ASSERT_TRUE(OCI_NumberSetValue(num, OCI_NUM_DOUBLE, &v));
        OCI_JsonNode* n = OCI_JsonNodeCreateNumber(json, num);
        ASSERT_NE(nullptr, n);
        ASSERT_EQ(OCI_JNT_NUMBER, OCI_JsonNodeGetType(n));
        ASSERT_TRUE(OCI_NumberFree(num));
        OCI_Number* out = OCI_JsonNodeGetNumber(n);
        ASSERT_NE(nullptr, out);
        OCI_Double vOut = 0.0;
        ASSERT_TRUE(OCI_NumberGetValue(out, OCI_NUM_DOUBLE, &vOut));
        EXPECT_DOUBLE_EQ(3.14, vOut);
        ASSERT_TRUE(OCI_NumberFree(out));
        OCI_Number* num2 = OCI_NumberCreate(env);
        v = 2.718;
        ASSERT_TRUE(OCI_NumberSetValue(num2, OCI_NUM_DOUBLE, &v));
        ASSERT_TRUE(OCI_JsonNodeSetNumber(n, num2));
        ASSERT_TRUE(OCI_NumberFree(num2));
        out = OCI_JsonNodeGetNumber(n);
        ASSERT_TRUE(OCI_NumberGetValue(out, OCI_NUM_DOUBLE, &vOut));
        EXPECT_DOUBLE_EQ(2.718, vOut);
        ASSERT_TRUE(OCI_NumberFree(out));
        OCI_JsonNodeSetField(root, OTEXT("num"), n);
        OCI_JsonNodeFree(n);
    }
    static void NodeBoolean(OCI_Environment*, OCI_Json* json, OCI_JsonNode* root)
    {
        OCI_JsonNode* n = OCI_JsonNodeCreateBoolean(json, TRUE);
        ASSERT_NE(nullptr, n);
        ASSERT_TRUE(OCI_JsonNodeGetBoolean(n));
        ASSERT_TRUE(OCI_JsonNodeSetBoolean(n, FALSE));
        EXPECT_FALSE(OCI_JsonNodeGetBoolean(n));
        OCI_JsonNodeSetField(root, OTEXT("bool"), n);
        OCI_JsonNodeFree(n);
    }
    static void NodeNull(OCI_Environment*, OCI_Json* json, OCI_JsonNode* root)
    {
        OCI_JsonNode* n = OCI_JsonNodeCreateNull(json);
        ASSERT_NE(nullptr, n);
        ASSERT_TRUE(OCI_JsonNodeIsNull(n));
        ASSERT_TRUE(OCI_JsonNodeSetNull(n));
        EXPECT_TRUE(OCI_JsonNodeIsNull(n));
        OCI_JsonNodeSetField(root, OTEXT("nil"), n);
        OCI_JsonNodeFree(n);
    }
    static void NodeBinary(OCI_Environment*, OCI_Json* json, OCI_JsonNode* root)
    {
        unsigned char d1[] = { 0xAB, 0xCD, 0xEF };
        unsigned char d2[] = { 0x12, 0x34 };
        OCI_JsonNode* n = OCI_JsonNodeCreateBinary(json, d1, 3);
        ASSERT_NE(nullptr, n);
        OCI_UInt32 len = 0;
        const void* val = OCI_JsonNodeGetBinary(n, &len);
        ASSERT_EQ(3u, len);
        ASSERT_EQ(0, memcmp(val, d1, 3));
        ASSERT_TRUE(OCI_JsonNodeSetBinary(n, d2, 2));
        val = OCI_JsonNodeGetBinary(n, &len);
        EXPECT_EQ(2u, len);
        EXPECT_EQ(0, memcmp(val, d2, 2));
        OCI_JsonNodeSetField(root, OTEXT("bin"), n);
        OCI_JsonNodeFree(n);
    }
    static void NodeDate(OCI_Environment* env, OCI_Json* json, OCI_JsonNode* root)
    {
        OCI_Date* date = OCI_DateCreate(env);
        ASSERT_NE(nullptr, date);
        ASSERT_TRUE(OCI_DateSetDate(date, 2024, 5, 15));
        OCI_JsonNode* n = OCI_JsonNodeCreateDate(json, date);
        ASSERT_NE(nullptr, n);
        ASSERT_EQ(OCI_JNT_DATE, OCI_JsonNodeGetType(n));
        ASSERT_TRUE(OCI_DateFree(date));
        OCI_Date* out = OCI_JsonNodeGetDate(n);
        ASSERT_NE(nullptr, out);
        OCI_Int32 y = 0, m = 0, d = 0;
        ASSERT_TRUE(OCI_DateGetDate(out, &y, &m, &d));
        EXPECT_EQ(2024, y); EXPECT_EQ(5, m); EXPECT_EQ(15, d);
        ASSERT_TRUE(OCI_DateFree(out));
        OCI_Date* d2 = OCI_DateCreate(env);
        ASSERT_TRUE(OCI_DateSetDate(d2, 2025, 6, 20));
        ASSERT_TRUE(OCI_JsonNodeSetDate(n, d2));
        ASSERT_TRUE(OCI_DateFree(d2));
        out = OCI_JsonNodeGetDate(n);
        ASSERT_TRUE(OCI_DateGetDate(out, &y, &m, &d));
        EXPECT_EQ(2025, y); EXPECT_EQ(6, m); EXPECT_EQ(20, d);
        ASSERT_TRUE(OCI_DateFree(out));
        OCI_JsonNodeSetField(root, OTEXT("date"), n);
        OCI_JsonNodeFree(n);
    }
    static void NodeTimestamp(OCI_Environment* env, OCI_Json* json, OCI_JsonNode* root)
    {
        OCI_Timestamp* ts = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        ASSERT_NE(nullptr, ts);
        ASSERT_TRUE(OCI_TimestampConstruct(ts, 2024, 5, 15, 10, 30, 0, 0, nullptr));
        OCI_JsonNode* n = OCI_JsonNodeCreateTimestamp(json, ts);
        ASSERT_NE(nullptr, n);
        ASSERT_EQ(OCI_JNT_TIMESTAMP, OCI_JsonNodeGetType(n));
        ASSERT_TRUE(OCI_TimestampFree(ts));
        OCI_Timestamp* out = OCI_JsonNodeGetTimestamp(n);
        ASSERT_NE(nullptr, out);
        OCI_Int32 y = 0, m = 0, d = 0, h = 0, mn = 0, s = 0, f = 0;
        ASSERT_TRUE(OCI_TimestampGetDateTime(out, &y, &m, &d, &h, &mn, &s, &f));
        EXPECT_EQ(2024, y); EXPECT_EQ(5, m); EXPECT_EQ(15, d); EXPECT_EQ(10, h); EXPECT_EQ(30, mn);
        ASSERT_TRUE(OCI_TimestampFree(out));
        OCI_Timestamp* t2 = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        ASSERT_TRUE(OCI_TimestampConstruct(t2, 2025, 6, 20, 14, 45, 0, 0, nullptr));
        ASSERT_TRUE(OCI_JsonNodeSetTimestamp(n, t2));
        ASSERT_TRUE(OCI_TimestampFree(t2));
        out = OCI_JsonNodeGetTimestamp(n);
        ASSERT_TRUE(OCI_TimestampGetDateTime(out, &y, &m, &d, &h, &mn, &s, &f));
        EXPECT_EQ(2025, y); EXPECT_EQ(6, m); EXPECT_EQ(20, d); EXPECT_EQ(14, h); EXPECT_EQ(45, mn);
        ASSERT_TRUE(OCI_TimestampFree(out));
        OCI_JsonNodeSetField(root, OTEXT("ts"), n);
        OCI_JsonNodeFree(n);
    }
    static void NodeInterval(OCI_Environment* env, OCI_Json* json, OCI_JsonNode* root)
    {
        OCI_Interval* itv = OCI_IntervalCreate(env, OCI_INTERVAL_YM);
        ASSERT_NE(nullptr, itv);
        ASSERT_TRUE(OCI_IntervalSetYearMonth(itv, 1, 6));
        OCI_JsonNode* n = OCI_JsonNodeCreateInterval(json, itv);
        ASSERT_NE(nullptr, n);
        ASSERT_EQ(OCI_JNT_INTERVAL, OCI_JsonNodeGetType(n));
        ASSERT_TRUE(OCI_IntervalFree(itv));
        OCI_Interval* out = OCI_JsonNodeGetInterval(n);
        ASSERT_NE(nullptr, out);
        OCI_Int32 y = 0, m = 0;
        ASSERT_TRUE(OCI_IntervalGetYearMonth(out, &y, &m));
        EXPECT_EQ(1, y); EXPECT_EQ(6, m);
        ASSERT_TRUE(OCI_IntervalFree(out));
        OCI_Interval* i2 = OCI_IntervalCreate(env, OCI_INTERVAL_YM);
        ASSERT_TRUE(OCI_IntervalSetYearMonth(i2, 2, 3));
        ASSERT_TRUE(OCI_JsonNodeSetInterval(n, i2));
        ASSERT_TRUE(OCI_IntervalFree(i2));
        out = OCI_JsonNodeGetInterval(n);
        ASSERT_TRUE(OCI_IntervalGetYearMonth(out, &y, &m));
        EXPECT_EQ(2, y); EXPECT_EQ(3, m);
        ASSERT_TRUE(OCI_IntervalFree(out));
        OCI_JsonNodeSetField(root, OTEXT("iv"), n);
        OCI_JsonNodeFree(n);
    }

    class TestJsonNodeCreate : public ::testing::TestWithParam<JsonNodeTypeCase>
    {
    protected:
        OCI_Environment* env = nullptr;
        OCI_Connection* conn = nullptr;
        OCI_Json* json = nullptr;
        OCI_JsonNode* root = nullptr;

        void SetUp() override
        {
            env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);
            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            json = OCI_JsonCreate(conn);
            ASSERT_NE(nullptr, json);
            ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
            ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));
            root = OCI_JsonNodeCreateObject(json);
            ASSERT_NE(nullptr, root);
        }

        void TearDown() override
        {
            if (root) OCI_JsonNodeFree(root);
            if (json) OCI_JsonFree(json);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_P(TestJsonNodeCreate, CreateSetGetAndAttach_RoundTripsValue)
    {
        GetParam().run(env, json, root);

        EXPECT_EQ(1u, OCI_JsonNodeGetFieldCount(root));
    }

    INSTANTIATE_TEST_CASE_P(NodeType, TestJsonNodeCreate,
        ::testing::Values(
            JsonNodeTypeCase{ "String",    NodeString },
            JsonNodeTypeCase{ "Number",    NodeNumber },
            JsonNodeTypeCase{ "Boolean",   NodeBoolean },
            JsonNodeTypeCase{ "Null",      NodeNull },
            JsonNodeTypeCase{ "Binary",    NodeBinary },
            JsonNodeTypeCase{ "Date",      NodeDate },
            JsonNodeTypeCase{ "Timestamp", NodeTimestamp },
            JsonNodeTypeCase{ "Interval",  NodeInterval }),
        ::testing::PrintToStringParamName());

    class TestJsonDomCreate : public ::testing::Test
    {
    protected:
        OCI_Environment* env = nullptr;
        OCI_Connection* conn = nullptr;
        OCI_Json* json = nullptr;
        OCI_JsonNode* root = nullptr;

        void SetUp() override
        {
            env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);
            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            json = OCI_JsonCreate(conn);
            ASSERT_NE(nullptr, json);
            ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
            ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));
            root = OCI_JsonNodeCreateObject(json);
            ASSERT_NE(nullptr, root);
        }

        void TearDown() override
        {
            if (root) OCI_JsonNodeFree(root);
            if (json) OCI_JsonFree(json);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_F(TestJsonDomCreate, CreateNodeWithStringType_ReturnsStringNode)
    {
        OCI_JsonNode* generic = OCI_JsonNodeCreate(json, OCI_JNT_STRING);
        ASSERT_NE(nullptr, generic);
        EXPECT_EQ(OCI_JNT_STRING, OCI_JsonNodeGetType(generic));
        ASSERT_TRUE(OCI_JsonNodeSetString(generic, OTEXT("generic"), 0));
        EXPECT_EQ(String(OTEXT("generic")), String(OCI_JsonNodeGetString(generic, nullptr)));
        OCI_JsonNodeSetField(root, OTEXT("generic"), generic);
        OCI_JsonNodeFree(generic);
    }

    TEST_F(TestJsonDomCreate, ArrayOpsOnCreatedArray_ManageItems)
    {
        OCI_JsonNode* arr = OCI_JsonNodeCreateArray(json);
        ASSERT_NE(nullptr, arr);
        EXPECT_EQ(OCI_JNT_ARRAY, OCI_JsonNodeGetType(arr));

        OCI_Number* nA = OCI_NumberCreate(env);
        OCI_Number* nB = OCI_NumberCreate(env);
        OCI_Double v = 1.0;
        ASSERT_TRUE(OCI_NumberSetValue(nA, OCI_NUM_DOUBLE, &v));
        v = 2.0;
        ASSERT_TRUE(OCI_NumberSetValue(nB, OCI_NUM_DOUBLE, &v));
        OCI_JsonNode* item1 = OCI_JsonNodeCreateNumber(json, nA);
        OCI_JsonNode* item2 = OCI_JsonNodeCreateNumber(json, nB);
        ASSERT_TRUE(OCI_NumberFree(nA));
        ASSERT_TRUE(OCI_NumberFree(nB));
        OCI_JsonNode* item3 = OCI_JsonNodeCreateString(json, OTEXT("three"), 0);

        ASSERT_TRUE(OCI_JsonNodeAppendItem(arr, item1));
        ASSERT_TRUE(OCI_JsonNodeAppendItem(arr, item2));
        ASSERT_TRUE(OCI_JsonNodeSetItem(arr, 1, item3));

        EXPECT_EQ(3u, OCI_JsonNodeGetItemCount(arr));
        OCI_JsonNode* chk = OCI_JsonNodeGetItem(arr, 1);
        ASSERT_NE(nullptr, chk);
        EXPECT_EQ(OCI_JNT_STRING, OCI_JsonNodeGetType(chk));
        EXPECT_EQ(String(OTEXT("three")), String(OCI_JsonNodeGetString(chk, nullptr)));

        OCI_JsonNodeFree(chk);
        OCI_JsonNodeFree(item1);
        OCI_JsonNodeFree(item2);
        OCI_JsonNodeFree(item3);
        OCI_JsonNodeSetField(root, OTEXT("arr"), arr);
        OCI_JsonNodeFree(arr);
    }

    TEST_F(TestJsonDomCreate, SetRootOnJson_AttachesObjectAsRoot)
    {
        ASSERT_TRUE(OCI_JsonSetRoot(json, root));

        OCI_JsonNode* fetched = OCI_JsonGetRoot(json);
        EXPECT_NE(nullptr, fetched);
        OCI_JsonNodeFree(fetched);
    }

    /* -------------------------------------------------------------------------
       Shared DOM fixture: mutable {"name":"test","arr":["one","two"],"num":123}
       ------------------------------------------------------------------------- */

    class TestJsonDomOps : public ::testing::Test
    {
    protected:
        OCI_Environment* env = nullptr;
        OCI_Connection* conn = nullptr;
        OCI_Json* json = nullptr;
        OCI_JsonNode* obj = nullptr;

        void SetUp() override
        {
            env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);
            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            json = OCI_JsonCreate(conn);
            ASSERT_NE(nullptr, json);
            ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
            ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("{\"name\":\"test\",\"arr\":[\"one\",\"two\"],\"num\":123}"),
                                         0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));
            obj = OCI_JsonGetRoot(json);
            ASSERT_NE(nullptr, obj);
        }

        void TearDown() override
        {
            if (obj) OCI_JsonNodeFree(obj);
            if (json) OCI_JsonFree(json);
            if (conn) OCI_ConnectionFree(conn);
        }

        OCI_JsonNode* GetArr()
        {
            return OCI_JsonNodeGetFieldByName(obj, OTEXT("arr"));
        }
    };

    TEST_F(TestJsonDomOps, GetFieldByNameOnExistingField_ReturnsFieldValue)
    {
        ASSERT_TRUE(OCI_JsonNodeHasField(obj, OTEXT("name")));

        OCI_JsonNode* name = OCI_JsonNodeGetFieldByName(obj, OTEXT("name"));

        ASSERT_NE(nullptr, name);
        EXPECT_EQ(String(OTEXT("test")), String(OCI_JsonNodeGetString(name, nullptr)));

        OCI_JsonNodeFree(name);
    }

    TEST_F(TestJsonDomOps, HasFieldOnUnknownField_ReturnsFalse)
    {
        EXPECT_FALSE(OCI_JsonNodeHasField(obj, OTEXT("missing")));
    }

    TEST_F(TestJsonDomOps, RemoveFieldOnExistingField_RemovesIt)
    {
        ASSERT_TRUE(OCI_JsonNodeRemoveField(obj, OTEXT("name")));

        EXPECT_FALSE(OCI_JsonNodeHasField(obj, OTEXT("name")));
    }

    TEST_F(TestJsonDomOps, GetByPathOnArrayItem_ReturnsItem)
    {
        OCI_JsonNode* node = OCI_JsonNodeGetByPath(obj, OTEXT("$.arr[0]"));

        ASSERT_NE(nullptr, node);
        EXPECT_EQ(String(OTEXT("one")), String(OCI_JsonNodeGetString(node, nullptr)));

        OCI_JsonNodeFree(node);
    }

    TEST_F(TestJsonDomOps, GetByPathOnOutOfRangeIndex_ReturnsNull)
    {
        EXPECT_EQ(nullptr, OCI_JsonNodeGetByPath(obj, OTEXT("$.arr[5]")));
        EXPECT_EQ(nullptr, OCI_GetLastError());
    }

    TEST_F(TestJsonDomOps, SetStringOnAttachedField_UpdatesDocument)
    {
        OCI_JsonNode* name = OCI_JsonNodeGetFieldByName(obj, OTEXT("name"));

        ASSERT_NE(nullptr, name);
        ASSERT_TRUE(OCI_JsonNodeSetString(name, OTEXT("changed"), 0));

        otext      buffer[512];
        OCI_UInt32 length = (OCI_UInt32) sizeof(buffer) - 1;

        ASSERT_TRUE(OCI_JsonToText(json, buffer, &length, 0));
        EXPECT_NE(String::npos, String(buffer).find(OTEXT("\"name\":\"changed\"")));

        OCI_JsonNodeFree(name);
    }

    TEST_F(TestJsonDomOps, SetStringOnAttachedArrayItem_ReplacesElement)
    {
        OCI_JsonNode* arr  = GetArr();
        OCI_JsonNode* item = OCI_JsonNodeGetItem(arr, 0);

        ASSERT_NE(nullptr, arr);
        ASSERT_NE(nullptr, item);
        ASSERT_TRUE(OCI_JsonNodeSetString(item, OTEXT("REPL"), 0));

        otext      buffer[512];
        OCI_UInt32 length = (OCI_UInt32) sizeof(buffer) - 1;

        ASSERT_TRUE(OCI_JsonToText(json, buffer, &length, 0));
        EXPECT_NE(String::npos, String(buffer).find(OTEXT("\"arr\":[\"REPL\",\"two\"]")));

        OCI_JsonNodeFree(item);
        OCI_JsonNodeFree(arr);
    }

    TEST_F(TestJsonDomOps, SetStringOnDetachedNode_StillWorks)
    {
        OCI_JsonNode* node = OCI_JsonNodeCreateString(json, OTEXT("before"), 0);

        ASSERT_NE(nullptr, node);
        ASSERT_TRUE(OCI_JsonNodeSetString(node, OTEXT("after"), 0));

        OCI_UInt32   length = 0;
        const otext *str   = OCI_JsonNodeGetString(node, &length);

        ASSERT_NE(nullptr, str);
        EXPECT_EQ(5u, length);
        EXPECT_EQ(0, memcmp(str, OTEXT("after"), 5 * sizeof(otext)));

        OCI_JsonNodeFree(node);
    }

    TEST_F(TestJsonDomOps, GetByPathOnUnknownField_ReturnsNull)
    {
        EXPECT_EQ(nullptr, OCI_JsonNodeGetByPath(obj, OTEXT("$.missing")));
        EXPECT_EQ(nullptr, OCI_GetLastError());
    }

    TEST_F(TestJsonDomOps, AppendItemOnArray_AddsItemAtEnd)
    {
        OCI_JsonNode* arr = GetArr();
        ASSERT_NE(nullptr, arr);
        ASSERT_EQ(2u, OCI_JsonNodeGetItemCount(arr));
        OCI_JsonNode* s3 = OCI_JsonNodeCreateString(json, OTEXT("three"), 0);
        ASSERT_NE(nullptr, s3);

        ASSERT_TRUE(OCI_JsonNodeAppendItem(arr, s3));

        EXPECT_EQ(3u, OCI_JsonNodeGetItemCount(arr));

        OCI_JsonNodeFree(s3);
        OCI_JsonNodeFree(arr);
    }

    TEST_F(TestJsonDomOps, InsertItemOnArray_InsertsItemAtIndex)
    {
        OCI_JsonNode* arr = GetArr();
        ASSERT_NE(nullptr, arr);
        OCI_JsonNode* s2 = OCI_JsonNodeCreateString(json, OTEXT("inserted"), 0);
        ASSERT_NE(nullptr, s2);

        ASSERT_TRUE(OCI_JsonNodeInsertItem(arr, 1, s2));

        EXPECT_EQ(3u, OCI_JsonNodeGetItemCount(arr));
        OCI_JsonNode* chk = OCI_JsonNodeGetItem(arr, 1);
        ASSERT_NE(nullptr, chk);
        EXPECT_EQ(String(OTEXT("inserted")), String(OCI_JsonNodeGetString(chk, nullptr)));

        OCI_JsonNodeFree(chk);
        OCI_JsonNodeFree(s2);
        OCI_JsonNodeFree(arr);
    }

    TEST_F(TestJsonDomOps, RemoveItemOnArray_RemovesItemAtIndex)
    {
        OCI_JsonNode* arr = GetArr();
        ASSERT_NE(nullptr, arr);
        ASSERT_EQ(2u, OCI_JsonNodeGetItemCount(arr));

        ASSERT_TRUE(OCI_JsonNodeRemoveItem(arr, 1));

        EXPECT_EQ(1u, OCI_JsonNodeGetItemCount(arr));

        OCI_JsonNodeFree(arr);
    }

    TEST_F(TestJsonDomOps, GetParentOnArrayItem_ReturnsArrayNode)
    {
        OCI_JsonNode* arr = GetArr();
        ASSERT_NE(nullptr, arr);
        OCI_JsonNode* it = OCI_JsonNodeGetItem(arr, 0);
        ASSERT_NE(nullptr, it);

        OCI_JsonNode* par = OCI_JsonNodeGetParent(it);

        ASSERT_NE(nullptr, par);
        EXPECT_EQ(OCI_JNT_ARRAY, OCI_JsonNodeGetType(par));

        OCI_JsonNodeFree(par);
        OCI_JsonNodeFree(it);
        OCI_JsonNodeFree(arr);
    }

    TEST_F(TestJsonDomOps, GetParentOnRootNode_ReturnsNull)
    {
        EXPECT_EQ(nullptr, OCI_JsonNodeGetParent(obj));
    }

    TEST_F(TestJsonDomOps, SetFieldOnObject_AddsNewField)
    {
        OCI_JsonNode* s = OCI_JsonNodeCreateString(json, OTEXT("v"), 0);
        ASSERT_NE(nullptr, s);

        ASSERT_TRUE(OCI_JsonNodeSetField(obj, OTEXT("s"), s));

        EXPECT_TRUE(OCI_JsonNodeHasField(obj, OTEXT("s")));

        OCI_JsonNodeFree(s);
    }

    TEST_F(TestJsonDomOps, ClearOnArray_RemovesAllItems)
    {
        OCI_JsonNode* arr = GetArr();
        ASSERT_NE(nullptr, arr);

        ASSERT_TRUE(OCI_JsonNodeClear(arr));

        EXPECT_EQ(0u, OCI_JsonNodeGetItemCount(arr));

        OCI_JsonNodeFree(arr);
    }

    TEST_F(TestJsonDomOps, ClearOnObject_RemovesAllFields)
    {
        ASSERT_TRUE(OCI_JsonNodeClear(obj));

        EXPECT_EQ(0u, OCI_JsonNodeGetFieldCount(obj));
    }

    /* -------------------------------------------------------------------------
       Parameterized: every numeric setter/getter pair round-trips its value
       ------------------------------------------------------------------------- */

    struct JsonNumCase
    {
        const char* name;
        void (*run)(OCI_JsonNode*);
    };

    void PrintTo(const JsonNumCase& c, std::ostream* os) { *os << c.name; }

    static void NumInt(OCI_JsonNode* n)
    {
        OCI_Int32 v = 0;
        ASSERT_TRUE(OCI_JsonNodeSetInt32(n, 456));
        ASSERT_TRUE(OCI_JsonNodeGetInt32(n, &v));
        EXPECT_EQ(456, v);
    }
    static void NumShort(OCI_JsonNode* n)
    {
        OCI_Int16 v = 0;
        ASSERT_TRUE(OCI_JsonNodeSetInt16(n, (OCI_Int16)-11));
        ASSERT_TRUE(OCI_JsonNodeGetInt16(n, &v));
        EXPECT_EQ((OCI_Int16)-11, v);
    }
    static void NumUShort(OCI_JsonNode* n)
    {
        OCI_UInt16 v = 0;
        ASSERT_TRUE(OCI_JsonNodeSetUInt16(n, (OCI_UInt16)12));
        ASSERT_TRUE(OCI_JsonNodeGetUInt16(n, &v));
        EXPECT_EQ((OCI_UInt16)12, v);
    }
    static void NumUInt(OCI_JsonNode* n)
    {
        OCI_UInt32 v = 0;
        ASSERT_TRUE(OCI_JsonNodeSetUInt32(n, 5678u));
        ASSERT_TRUE(OCI_JsonNodeGetUInt32(n, &v));
        EXPECT_EQ(5678u, v);
    }
    static void NumBigInt(OCI_JsonNode* n)
    {
        OCI_Int64 v = 0;
        ASSERT_TRUE(OCI_JsonNodeSetInt64(n, -123456789012345LL));
        ASSERT_TRUE(OCI_JsonNodeGetInt64(n, &v));
        EXPECT_EQ(-123456789012345LL, v);
    }
    static void NumBigUInt(OCI_JsonNode* n)
    {
        OCI_UInt64 v = 0;
        ASSERT_TRUE(OCI_JsonNodeSetUInt64(n, 123456789012345ULL));
        ASSERT_TRUE(OCI_JsonNodeGetUInt64(n, &v));
        EXPECT_EQ(123456789012345ULL, v);
    }
    static void NumFloat(OCI_JsonNode* n)
    {
        OCI_Float v = 0.f;
        ASSERT_TRUE(OCI_JsonNodeSetFloat(n, 1.5f));
        ASSERT_TRUE(OCI_JsonNodeGetFloat(n, &v));
        EXPECT_NEAR(1.5f, v, 0.001f);
    }
    static void NumDouble(OCI_JsonNode* n)
    {
        OCI_Double v = 0;
        ASSERT_TRUE(OCI_JsonNodeSetDouble(n, 2.5));
        ASSERT_TRUE(OCI_JsonNodeGetDouble(n, &v));
        EXPECT_EQ(2.5, v);
    }

    class TestJsonNumScalar : public ::testing::TestWithParam<JsonNumCase>
    {
    protected:
        OCI_Environment* env = nullptr;
        OCI_Connection* conn = nullptr;
        OCI_Json* json = nullptr;
        OCI_JsonNode* node = nullptr;

        void SetUp() override
        {
            env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);
            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            json = OCI_JsonCreate(conn);
            ASSERT_NE(nullptr, json);
            ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
            ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("{\"num\":123}"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));
            OCI_JsonNode* root = OCI_JsonGetRoot(json);
            ASSERT_NE(nullptr, root);
            node = OCI_JsonNodeGetFieldByName(root, OTEXT("num"));
            ASSERT_NE(nullptr, node);
            OCI_JsonNodeFree(root);
        }

        void TearDown() override
        {
            if (node) OCI_JsonNodeFree(node);
            if (json) OCI_JsonFree(json);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_P(TestJsonNumScalar, SetThenGet_RoundTripsValue)
    {
        GetParam().run(node);
    }

    INSTANTIATE_TEST_CASE_P(NumType, TestJsonNumScalar,
        ::testing::Values(
            JsonNumCase{ "Int",     NumInt },
            JsonNumCase{ "Short",   NumShort },
            JsonNumCase{ "UShort",  NumUShort },
            JsonNumCase{ "UInt",    NumUInt },
            JsonNumCase{ "BigInt",  NumBigInt },
            JsonNumCase{ "BigUInt", NumBigUInt },
            JsonNumCase{ "Float",   NumFloat },
            JsonNumCase{ "Double",  NumDouble }),
        ::testing::PrintToStringParamName());

    TEST(TestJsonDom, ValidateSchema)
    {
        auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);

        if (OCI_EnvironmentGetOCIRuntimeVersion(env) < OCI_VER_MAKE(23, 4, 0))
        {
            return; /* OCIJsonSchemaValidate requires 23ai+ */
        }

        auto schema = JsonFromText(conn, OTEXT("{\"type\":\"object\",\"properties\":{\"a\":{\"type\":\"number\"}},\"required\":[\"a\"]}"));
        ASSERT_TRUE(NULL != schema);

        auto ok = JsonFromText(conn, OTEXT("{\"a\":1}"));
        ASSERT_TRUE(NULL != ok);

        OCI_Json* errors = NULL;

        EXPECT_TRUE(OCI_JsonValidate(ok, schema, NULL));
        EXPECT_TRUE(OCI_JsonValidate(ok, schema, &errors));

        OCI_JsonFree(errors);
        errors = NULL;

        auto bad = JsonFromText(conn, OTEXT("{\"a\":\"notanum\"}"));
        ASSERT_TRUE(NULL != bad);

        EXPECT_FALSE(OCI_JsonValidate(bad, schema, NULL));

        EXPECT_FALSE(OCI_JsonValidate(bad, schema, &errors));
        ASSERT_TRUE(NULL != errors);
        {
            /* error doc is populated — verify via the DOM (portable: the
               server-generated error text may not be serializable to the
               client charset — JZN-00010 on localized messages) */

            OCI_JsonNode* errroot = OCI_JsonGetRoot(errors);

            ASSERT_TRUE(NULL != errroot);
            EXPECT_TRUE(OCI_JsonNodeGetFieldCount(errroot) > 0);

            OCI_JsonNodeFree(errroot);
        }
        OCI_JsonFree(errors);
        errors = NULL;

        /* invalid schema — parses as JSON but isn't a valid JSON Schema */
        auto badschema = JsonFromText(conn, OTEXT("{\"type\":\"bogus\"}"));
        ASSERT_TRUE(NULL != badschema);

        EXPECT_FALSE(OCI_JsonValidate(ok, badschema, &errors));

        if (NULL != errors) { OCI_JsonFree(errors); errors = NULL; }
        OCI_JsonFree(badschema);

        if (NULL != errors) { OCI_JsonFree(errors); }
        if (NULL != bad)    { OCI_JsonFree(bad); }
        if (NULL != ok)     { OCI_JsonFree(ok); }
        OCI_JsonFree(schema);
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestJsonDom, NodeOps)
    {
        auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);

        auto json = JsonFromText(conn, OTEXT("{\"a\":1,\"b\":2,\"arr\":[10,20,30,40]}"));
        ASSERT_TRUE(NULL != json);
        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));

        OCI_JsonNode* root = OCI_JsonGetRoot(json);
        ASSERT_TRUE(NULL != root);
        ASSERT_EQ(OCI_JsonNodeGetJson(root), json);

        ASSERT_TRUE(OCI_JsonNodeRenameField(root, OTEXT("a"), OTEXT("c")));
        ASSERT_TRUE(OCI_JsonNodeHasField(root, OTEXT("c")));
        ASSERT_FALSE(OCI_JsonNodeHasField(root, OTEXT("a")));

        OCI_JsonNode* copy = OCI_JsonNodeCreate(json, OCI_JNT_OBJECT);
        ASSERT_TRUE(NULL != copy);
        ASSERT_TRUE(OCI_JsonNodeAssign(copy, root));
        ASSERT_TRUE(OCI_JsonNodeEquals(copy, root));

        OCI_JsonNode* arr = OCI_JsonNodeGetFieldByName(root, OTEXT("arr"));
        ASSERT_TRUE(NULL != arr);
        ASSERT_EQ(4u, OCI_JsonNodeGetItemCount(arr));
        ASSERT_EQ(2u, OCI_JsonNodeRemoveItems(arr, 2, 2));
        ASSERT_EQ(2u, OCI_JsonNodeGetItemCount(arr));

        OCI_JsonNodeFree(arr);
        OCI_JsonNodeFree(copy);
        OCI_JsonNodeFree(root);
        OCI_JsonFree(json);
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestJsonDom, NodeRemoveItemsOutOfRange)
    {
        auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);

        auto json = JsonFromText(conn, OTEXT("[10,20,30]"));
        ASSERT_TRUE(NULL != json);
        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));

        OCI_JsonNode* root = OCI_JsonGetRoot(json);
        ASSERT_TRUE(NULL != root);
        ASSERT_EQ(3u, OCI_JsonNodeGetItemCount(root));

        /* start past the end */
        ASSERT_EQ(0u, OCI_JsonNodeRemoveItems(root, 4, 1));

        /* count overruns the end */
        ASSERT_EQ(0u, OCI_JsonNodeRemoveItems(root, 2, 3));

        /* zero start / zero count */
        ASSERT_EQ(0u, OCI_JsonNodeRemoveItems(root, 0, 1));
        ASSERT_EQ(0u, OCI_JsonNodeRemoveItems(root, 1, 0));

        /* the array is unchanged after the failures */
        ASSERT_EQ(3u, OCI_JsonNodeGetItemCount(root));

        OCI_JsonNodeFree(root);
        OCI_JsonFree(json);
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestJsonDom, Lifetime_NodeCycles_StatsBounded)
    {
        /* C1-class guard: repeated Json/node create->use->destroy must not grow env stats */

        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        auto cycle = [&]
        {
            const auto json = OCI_JsonCreate(conn);
            EXPECT_NE(nullptr, json);
            EXPECT_TRUE(OCI_JsonLoadText(json, OTEXT("{\"a\":1}"), 7, OCI_JTV_ALLOW_NONE, OCI_JTE_DETECT));
            const auto root = OCI_JsonGetRoot(json);
            EXPECT_NE(nullptr, root);
            OCI_JsonNodeFree(root);
            EXPECT_TRUE(OCI_JsonFree(json));
        };

        for (int i = 0; i < 20; i++) cycle();

        const auto handles = OCI_EnvironmentGetStats(env, OCI_UST_HANDLES);
        const auto objs    = OCI_EnvironmentGetStats(env, OCI_UST_OBJECTS);
        const auto bytes   = OCI_EnvironmentGetStats(env, OCI_UST_BYTES);

        for (int i = 0; i < 50; i++) cycle();

        ASSERT_EQ(handles, OCI_EnvironmentGetStats(env, OCI_UST_HANDLES));
        ASSERT_EQ(objs,    OCI_EnvironmentGetStats(env, OCI_UST_OBJECTS));
        ASSERT_LE(OCI_EnvironmentGetStats(env, OCI_UST_BYTES), bytes + 64 * 1024);

        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }


    TEST(TestJsonDom, GetRoot_OnEmptyJson_ReturnsNullAndRaisesError)
    {
        /* CHECK_JSON arm: DOM has no root -> OcilibExceptionJson */

        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);
        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);

        ASSERT_EQ(nullptr, OCI_JsonGetRoot(json));
        ASSERT_NE(nullptr, OCI_GetLastError());

        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestJsonDom, NodeGetFieldByName_MissingField_RaisesJznError)
    {
        /* JsonDomGetFieldByName returns NULL for a missing field ->
           CHECK_JSON -> OcilibExceptionJson (OCI_ERR_CAT_JZN) */

        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);
        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);

        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("{\"a\":1}"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));
        const auto root = OCI_JsonGetRoot(json);
        ASSERT_NE(nullptr, root);
        ASSERT_EQ(OCI_JNT_OBJECT, OCI_JsonNodeGetType(root));

        /* legit miss: NULL, no error, and the doc's stale JZN error is drained
           so the next mutating call is not poisoned */
        ASSERT_EQ(nullptr, OCI_JsonNodeGetFieldByName(root, OTEXT("no_such_field_xyz")));
        ASSERT_EQ(nullptr, OCI_GetLastError());

        /* mutating the loaded (immutable) DOM fails at JZN level ->
           CHECK_JSON -> OcilibExceptionJson -> real JZN error */
        const auto val = OCI_JsonNodeCreateNull(json);
        ASSERT_NE(nullptr, val);
        ASSERT_FALSE(OCI_JsonNodeSetField(root, OTEXT("b"), val));
        const auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);
        ASSERT_EQ(OCI_ERR_CAT_JZN, OCI_ErrorGetCategory(err));

        ASSERT_TRUE(OCI_JsonNodeFree(val));
        ASSERT_TRUE(OCI_JsonNodeFree(root));
        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    class TestJsonDomNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestJsonDomNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestJsonDomNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "JsonNodeClear", [] { return !OCI_JsonNodeClear(nullptr); } },
            { "JsonNodeFree", [] { return !OCI_JsonNodeFree(nullptr); } },
            { "JsonNodeGetByPath", [] { return nullptr == OCI_JsonNodeGetByPath(nullptr, OTEXT("x")); } },
            { "JsonNodeGetParent", [] { return nullptr == OCI_JsonNodeGetParent(nullptr); } },
            { "JsonNodeGetType", [] { return OCI_JNT_NULL == OCI_JsonNodeGetType(nullptr); } },
            { "JsonNodeIsNull", [] { return !OCI_JsonNodeIsNull(nullptr); } },
        }),
        &NullArgCaseName);

}