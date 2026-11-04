#include "ocilib_tests.h"

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
        ASSERT_EQ(ostring(OTEXT("test")), ostring(OCI_JsonNodeGetString(nameField, nullptr)));

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
        ASSERT_FALSE(ostring(fieldName).empty());

        OCI_JsonNode* name = OCI_JsonNodeGetFieldByName(root, OTEXT("name"));
        ASSERT_NE(nullptr, name);
        ASSERT_EQ(OCI_JNT_STRING, OCI_JsonNodeGetType(name));
        ASSERT_EQ(ostring(OTEXT("test")), ostring(OCI_JsonNodeGetString(name, nullptr)));

        OCI_JsonNode* items = OCI_JsonNodeGetFieldByIndex(root, 2);
        ASSERT_NE(nullptr, items);
        ASSERT_EQ(OCI_JNT_ARRAY, OCI_JsonNodeGetType(items));
        ASSERT_EQ(3u, OCI_JsonNodeGetItemCount(items));

        OCI_JsonNode* item2 = OCI_JsonNodeGetItem(items, 2);
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

    TEST(TestJsonDom, NodeCreationAndSetters)
    {
        auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);
        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));

        OCI_JsonNode* root = OCI_JsonNodeCreateObject(json);
        ASSERT_NE(nullptr, root);

        OCI_JsonNode* generic = OCI_JsonNodeCreate(json, OCI_JNT_STRING);
        ASSERT_NE(nullptr, generic);
        ASSERT_EQ(OCI_JNT_STRING, OCI_JsonNodeGetType(generic));
        ASSERT_TRUE(OCI_JsonNodeSetString(generic, OTEXT("generic"), 0));
        ASSERT_EQ(ostring(OTEXT("generic")), ostring(OCI_JsonNodeGetString(generic, nullptr)));
        OCI_JsonNodeSetField(root, OTEXT("generic"), generic);
        OCI_JsonNodeFree(generic);

        OCI_JsonNode* str = OCI_JsonNodeCreateString(json, OTEXT("old"), 0);
        ASSERT_TRUE(OCI_JsonNodeSetString(str, OTEXT("new"), 0));
        ASSERT_EQ(ostring(OTEXT("new")), ostring(OCI_JsonNodeGetString(str, nullptr)));
        OCI_JsonNodeSetField(root, OTEXT("str"), str);
        OCI_JsonNodeFree(str);

        OCI_Number* num = OCI_NumberCreate(env);
        ASSERT_NE(nullptr, num);
        OCI_Double v = 3.14;
        ASSERT_TRUE(OCI_NumberSetValue(num, OCI_NUM_DOUBLE, &v));
        OCI_JsonNode* numNode = OCI_JsonNodeCreateNumber(json, num);
        ASSERT_EQ(OCI_JNT_NUMBER, OCI_JsonNodeGetType(numNode));

        OCI_Number* numOut = OCI_JsonNodeGetNumber(numNode);
        ASSERT_NE(nullptr, numOut);
        OCI_Double vOut = 0.0;
        ASSERT_TRUE(OCI_NumberGetValue(numOut, OCI_NUM_DOUBLE, &vOut));
        ASSERT_DOUBLE_EQ(3.14, vOut);
        ASSERT_TRUE(OCI_NumberFree(numOut));

        OCI_Number* num2 = OCI_NumberCreate(env);
        ASSERT_NE(nullptr, num2);
        v = 2.718;
        ASSERT_TRUE(OCI_NumberSetValue(num2, OCI_NUM_DOUBLE, &v));
        ASSERT_TRUE(OCI_JsonNodeSetNumber(numNode, num2));
        ASSERT_TRUE(OCI_NumberFree(num2));

        numOut = OCI_JsonNodeGetNumber(numNode);
        ASSERT_TRUE(OCI_NumberGetValue(numOut, OCI_NUM_DOUBLE, &vOut));
        ASSERT_DOUBLE_EQ(2.718, vOut);
        ASSERT_TRUE(OCI_NumberFree(numOut));
        ASSERT_TRUE(OCI_NumberFree(num));

        OCI_JsonNodeSetField(root, OTEXT("num"), numNode);
        OCI_JsonNodeFree(numNode);

        OCI_JsonNode* OCI_Boolean = OCI_JsonNodeCreateBoolean(json, TRUE);
        ASSERT_TRUE(OCI_JsonNodeGetBoolean(OCI_Boolean));
        ASSERT_TRUE(OCI_JsonNodeSetBoolean(OCI_Boolean, FALSE));
        ASSERT_FALSE(OCI_JsonNodeGetBoolean(OCI_Boolean));
        OCI_JsonNodeSetField(root, OTEXT("bool"), OCI_Boolean);
        OCI_JsonNodeFree(OCI_Boolean);

        OCI_JsonNode* nullNode = OCI_JsonNodeCreateNull(json);
        ASSERT_TRUE(OCI_JsonNodeIsNull(nullNode));
        ASSERT_TRUE(OCI_JsonNodeSetNull(nullNode));
        ASSERT_TRUE(OCI_JsonNodeIsNull(nullNode));
        OCI_JsonNodeSetField(root, OTEXT("nil"), nullNode);
        OCI_JsonNodeFree(nullNode);

        unsigned char binData[] = { 0xAB, 0xCD, 0xEF };
        OCI_JsonNode* binary = OCI_JsonNodeCreateBinary(json, binData, 3);
        OCI_UInt32 binLen = 0;
        const void* binVal = OCI_JsonNodeGetBinary(binary, &binLen);
        ASSERT_EQ(3u, binLen);
        ASSERT_EQ(0, memcmp(binVal, binData, 3));

        unsigned char binData2[] = { 0x12, 0x34 };
        ASSERT_TRUE(OCI_JsonNodeSetBinary(binary, binData2, 2));
        binVal = OCI_JsonNodeGetBinary(binary, &binLen);
        ASSERT_EQ(2u, binLen);
        ASSERT_EQ(0, memcmp(binVal, binData2, 2));
        OCI_JsonNodeSetField(root, OTEXT("bin"), binary);
        OCI_JsonNodeFree(binary);

        OCI_Date* date = OCI_DateCreate(env);
        ASSERT_NE(nullptr, date);
        ASSERT_TRUE(OCI_DateSetDate(date, 2024, 5, 15));
        OCI_JsonNode* dateNode = OCI_JsonNodeCreateDate(json, date);
        ASSERT_EQ(OCI_JNT_DATE, OCI_JsonNodeGetType(dateNode));

        OCI_Date* date2 = OCI_JsonNodeGetDate(dateNode);
        ASSERT_NE(nullptr, date2);
        OCI_Int32 y = 0, m = 0, d = 0;
        ASSERT_TRUE(OCI_DateGetDate(date2, &y, &m, &d));
        ASSERT_EQ(2024, y);
        ASSERT_EQ(5, m);
        ASSERT_EQ(15, d);
        ASSERT_TRUE(OCI_DateFree(date2));

        OCI_Date* date3 = OCI_DateCreate(env);
        ASSERT_TRUE(OCI_DateSetDate(date3, 2025, 6, 20));
        ASSERT_TRUE(OCI_JsonNodeSetDate(dateNode, date3));
        ASSERT_TRUE(OCI_DateFree(date3));

        date2 = OCI_JsonNodeGetDate(dateNode);
        ASSERT_TRUE(OCI_DateGetDate(date2, &y, &m, &d));
        ASSERT_EQ(2025, y);
        ASSERT_EQ(6, m);
        ASSERT_EQ(20, d);
        ASSERT_TRUE(OCI_DateFree(date2));
        ASSERT_TRUE(OCI_DateFree(date));

        OCI_JsonNodeSetField(root, OTEXT("date"), dateNode);
        OCI_JsonNodeFree(dateNode);

        OCI_Timestamp* ts = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        ASSERT_NE(nullptr, ts);
        ASSERT_TRUE(OCI_TimestampConstruct(ts, 2024, 5, 15, 10, 30, 0, 0, nullptr));
        OCI_JsonNode* tsNode = OCI_JsonNodeCreateTimestamp(json, ts);
        ASSERT_EQ(OCI_JNT_TIMESTAMP, OCI_JsonNodeGetType(tsNode));

        OCI_Timestamp* ts2 = OCI_JsonNodeGetTimestamp(tsNode);
        ASSERT_NE(nullptr, ts2);
        OCI_Int32 h = 0, mn = 0, s = 0, f = 0;
        ASSERT_TRUE(OCI_TimestampGetDateTime(ts2, &y, &m, &d, &h, &mn, &s, &f));
        ASSERT_EQ(2024, y);
        ASSERT_EQ(5, m);
        ASSERT_EQ(15, d);
        ASSERT_EQ(10, h);
        ASSERT_EQ(30, mn);
        ASSERT_TRUE(OCI_TimestampFree(ts2));

        OCI_Timestamp* ts3 = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        ASSERT_TRUE(OCI_TimestampConstruct(ts3, 2025, 6, 20, 14, 45, 0, 0, nullptr));
        ASSERT_TRUE(OCI_JsonNodeSetTimestamp(tsNode, ts3));
        ASSERT_TRUE(OCI_TimestampFree(ts3));

        ts2 = OCI_JsonNodeGetTimestamp(tsNode);
        ASSERT_TRUE(OCI_TimestampGetDateTime(ts2, &y, &m, &d, &h, &mn, &s, &f));
        ASSERT_EQ(2025, y);
        ASSERT_EQ(6, m);
        ASSERT_EQ(20, d);
        ASSERT_EQ(14, h);
        ASSERT_EQ(45, mn);
        ASSERT_TRUE(OCI_TimestampFree(ts2));
        ASSERT_TRUE(OCI_TimestampFree(ts));

        OCI_JsonNodeSetField(root, OTEXT("ts"), tsNode);
        OCI_JsonNodeFree(tsNode);

        OCI_Interval* itv = OCI_IntervalCreate(env, OCI_INTERVAL_YM);
        ASSERT_NE(nullptr, itv);
        ASSERT_TRUE(OCI_IntervalSetYearMonth(itv, 1, 6));
        OCI_JsonNode* itvNode = OCI_JsonNodeCreateInterval(json, itv);
        ASSERT_EQ(OCI_JNT_INTERVAL, OCI_JsonNodeGetType(itvNode));

        OCI_Interval* itv2 = OCI_JsonNodeGetInterval(itvNode);
        ASSERT_NE(nullptr, itv2);
        ASSERT_TRUE(OCI_IntervalGetYearMonth(itv2, &y, &m));
        ASSERT_EQ(1, y);
        ASSERT_EQ(6, m);
        ASSERT_TRUE(OCI_IntervalFree(itv2));

        OCI_Interval* itv3 = OCI_IntervalCreate(env, OCI_INTERVAL_YM);
        ASSERT_TRUE(OCI_IntervalSetYearMonth(itv3, 2, 3));
        ASSERT_TRUE(OCI_JsonNodeSetInterval(itvNode, itv3));
        ASSERT_TRUE(OCI_IntervalFree(itv3));

        itv2 = OCI_JsonNodeGetInterval(itvNode);
        ASSERT_TRUE(OCI_IntervalGetYearMonth(itv2, &y, &m));
        ASSERT_EQ(2, y);
        ASSERT_EQ(3, m);
        ASSERT_TRUE(OCI_IntervalFree(itv2));
        ASSERT_TRUE(OCI_IntervalFree(itv));

        OCI_JsonNodeSetField(root, OTEXT("iv"), itvNode);
        OCI_JsonNodeFree(itvNode);

        OCI_JsonNode* arr = OCI_JsonNodeCreateArray(json);
        ASSERT_EQ(OCI_JNT_ARRAY, OCI_JsonNodeGetType(arr));

        OCI_Number* numA = OCI_NumberCreate(env);
        v = 1.0;
        ASSERT_TRUE(OCI_NumberSetValue(numA, OCI_NUM_DOUBLE, &v));
        OCI_JsonNode* item1 = OCI_JsonNodeCreateNumber(json, numA);
        ASSERT_TRUE(OCI_NumberFree(numA));

        OCI_Number* numB = OCI_NumberCreate(env);
        v = 2.0;
        ASSERT_TRUE(OCI_NumberSetValue(numB, OCI_NUM_DOUBLE, &v));
        OCI_JsonNode* item2 = OCI_JsonNodeCreateNumber(json, numB);
        ASSERT_TRUE(OCI_JsonNodeAppendItem(arr, item1));
        ASSERT_TRUE(OCI_JsonNodeAppendItem(arr, item2));
        ASSERT_TRUE(OCI_NumberFree(numB));

        OCI_JsonNode* item3str = OCI_JsonNodeCreateString(json, OTEXT("three"), 0);
        ASSERT_TRUE(OCI_JsonNodeSetItem(arr, 1, item3str));
        ASSERT_EQ(3u, OCI_JsonNodeGetItemCount(arr));

        OCI_JsonNode* verifyItem = OCI_JsonNodeGetItem(arr, 3);
        ASSERT_EQ(OCI_JNT_STRING, OCI_JsonNodeGetType(verifyItem));
        ASSERT_EQ(ostring(OTEXT("three")), ostring(OCI_JsonNodeGetString(verifyItem, nullptr)));
        OCI_JsonNodeFree(verifyItem);

        OCI_JsonNodeFree(item1);
        OCI_JsonNodeFree(item2);
        OCI_JsonNodeFree(item3str);

        OCI_JsonNodeSetField(root, OTEXT("arr"), arr);
        OCI_JsonNodeFree(arr);

        ASSERT_EQ(10u, OCI_JsonNodeGetFieldCount(root));

        ASSERT_TRUE(OCI_JsonSetRoot(json, root));
        OCI_JsonNodeFree(root);

        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestJsonDom, DomMutationOps)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);

        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("{}"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));

        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("{\"name\":\"test\",\"arr\":[\"one\",\"two\"],\"num\":123}"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));

        OCI_JsonNode* obj = OCI_JsonGetRoot(json);
        ASSERT_NE(nullptr, obj);

        /* HasField / GetFieldByName / RemoveField / SetField on root */
        ASSERT_TRUE(OCI_JsonNodeHasField(obj, OTEXT("name")));
        OCI_JsonNode* name = OCI_JsonNodeGetFieldByName(obj, OTEXT("name"));
        ASSERT_NE(nullptr, name);
        ASSERT_EQ(ostring(OTEXT("test")), ostring(OCI_JsonNodeGetString(name, nullptr)));
        OCI_JsonNodeFree(name);
        ASSERT_FALSE(OCI_JsonNodeHasField(obj, OTEXT("missing")));
        ASSERT_TRUE(OCI_JsonNodeRemoveField(obj, OTEXT("name")));
        ASSERT_FALSE(OCI_JsonNodeHasField(obj, OTEXT("name")));

        /* GetByPath on root */
        OCI_JsonNode* pathNode = OCI_JsonNodeGetByPath(obj, OTEXT("$.arr[0]"));
        ASSERT_NE(nullptr, pathNode);
        ASSERT_EQ(ostring(OTEXT("one")), ostring(OCI_JsonNodeGetString(pathNode, nullptr)));
        OCI_JsonNodeFree(pathNode);
        ASSERT_EQ(nullptr, OCI_JsonNodeGetByPath(obj, OTEXT("$.arr[5]")));
        ASSERT_EQ(nullptr, OCI_JsonNodeGetByPath(obj, OTEXT("$.missing")));
        ASSERT_EQ(nullptr, OCI_GetLastError());

        /* array ops on root.arr */
        OCI_JsonNode* arr = OCI_JsonNodeGetFieldByName(obj, OTEXT("arr"));
        ASSERT_NE(nullptr, arr);
        ASSERT_EQ(2u, OCI_JsonNodeGetItemCount(arr));
        OCI_JsonNode* s3 = OCI_JsonNodeCreateString(json, OTEXT("three"), 0);
        ASSERT_TRUE(OCI_JsonNodeAppendItem(arr, s3));
        OCI_JsonNodeFree(s3);
        OCI_JsonNode* s2 = OCI_JsonNodeCreateString(json, OTEXT("inserted"), 0);
        ASSERT_TRUE(OCI_JsonNodeInsertItem(arr, 1, s2));
        OCI_JsonNodeFree(s2);
        ASSERT_EQ(4u, OCI_JsonNodeGetItemCount(arr));
        OCI_JsonNode* chk = OCI_JsonNodeGetItem(arr, 2);
        ASSERT_EQ(ostring(OTEXT("inserted")), ostring(OCI_JsonNodeGetString(chk, nullptr)));
        OCI_JsonNodeFree(chk);
        ASSERT_TRUE(OCI_JsonNodeRemoveItem(arr, 2));
        ASSERT_EQ(3u, OCI_JsonNodeGetItemCount(arr));

        /* GetParent */
        OCI_JsonNode* it = OCI_JsonNodeGetItem(arr, 1);
        ASSERT_NE(nullptr, it);
        OCI_JsonNode* par = OCI_JsonNodeGetParent(it);
        ASSERT_NE(nullptr, par);
        ASSERT_EQ(OCI_JNT_ARRAY, OCI_JsonNodeGetType(par));
        ASSERT_EQ(nullptr, OCI_JsonNodeGetParent(obj));
        OCI_JsonNodeFree(par);
        OCI_JsonNodeFree(it);

        /* typed accessors on root.num */
        OCI_JsonNode* num = OCI_JsonNodeGetFieldByName(obj, OTEXT("num"));
        ASSERT_NE(nullptr, num);
        ASSERT_TRUE(OCI_JsonNodeSetInt(num, 456));
        OCI_Int32 iv = 0;
        ASSERT_TRUE(OCI_JsonNodeGetInt(num, &iv));
        ASSERT_EQ(456, iv);
        ASSERT_TRUE(OCI_JsonNodeSetDouble(num, 2.5));
        OCI_Double dv = 0;
        ASSERT_TRUE(OCI_JsonNodeGetDouble(num, &dv));
        ASSERT_EQ(2.5, dv);
        OCI_Int16  v1=0; OCI_UInt16 v2=0; OCI_UInt32 v4=0; OCI_Int64 v5=0; OCI_UInt64 v6=0; OCI_Float v7=0.f;
        ASSERT_TRUE(OCI_JsonNodeSetShort(num, (OCI_Int16)-11));
        ASSERT_TRUE(OCI_JsonNodeGetShort(num, &v1)); ASSERT_EQ(-11, (OCI_Int16)v1);
        ASSERT_TRUE(OCI_JsonNodeSetUShort(num, (OCI_UInt16)12));
        ASSERT_TRUE(OCI_JsonNodeGetUShort(num, &v2)); ASSERT_EQ(12, (OCI_UInt16)v2);
        ASSERT_TRUE(OCI_JsonNodeSetUInt(num, 5678u));
        ASSERT_TRUE(OCI_JsonNodeGetUInt(num, &v4)); ASSERT_EQ(5678u, (OCI_UInt32)v4);
        ASSERT_TRUE(OCI_JsonNodeSetBigInt(num, -123456789012345LL));
        ASSERT_TRUE(OCI_JsonNodeGetBigInt(num, &v5)); ASSERT_EQ(-123456789012345LL, (OCI_Int64)v5);
        ASSERT_TRUE(OCI_JsonNodeSetBigUInt(num, 123456789012345ULL));
        ASSERT_TRUE(OCI_JsonNodeGetBigUInt(num, &v6)); ASSERT_EQ(123456789012345ULL, (OCI_UInt64)v6);
        ASSERT_TRUE(OCI_JsonNodeSetFloat(num, 1.5f));
        ASSERT_TRUE(OCI_JsonNodeGetFloat(num, &v7)); ASSERT_NEAR(1.5f, (OCI_Float)v7, 0.001f);
        OCI_JsonNodeFree(num);

        /* SetString / SetBoolean / SetNull / GetType round-trip on a scalar node */
        OCI_JsonNode* strN = OCI_JsonNodeCreateString(json, OTEXT("v"), 0);
        ASSERT_TRUE(OCI_JsonNodeSetField(obj, OTEXT("s"), strN));
        OCI_JsonNodeFree(strN);
        
        /* Clear */
        ASSERT_TRUE(OCI_JsonNodeClear(arr));
        ASSERT_EQ(0u, OCI_JsonNodeGetItemCount(arr));
        OCI_JsonNodeFree(arr);
        ASSERT_TRUE(OCI_JsonNodeClear(obj));
        ASSERT_EQ(0u, OCI_JsonNodeGetFieldCount(obj));

        OCI_JsonNodeFree(obj);
        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
}