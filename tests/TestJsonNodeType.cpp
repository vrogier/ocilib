#include "ocilib_tests.h"
#include "test_internals.h"

#include <cstring>

using namespace ocilib;

namespace
{
    OCI_Number* MakeDoubleNumber(OCI_Environment* env, double value)
    {
        OCI_Number* num = OCI_NumberCreate(env);
        if (num)
        {
            OCI_NumberSetValue(num, OCI_NUM_DOUBLE, &value);
        }
        return num;
    }
}

namespace TestCApi
{
    TEST(TestJsonNodeType, TypeAndNullC)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);
        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JV_ALLOW_ALL, OCI_JE_DETECT));

        OCI_JsonNode* oldRoot = OCI_JsonGetRoot(json);
        ASSERT_NE(nullptr, oldRoot);
        OCI_JsonNodeFree(oldRoot);

        OCI_JsonNode* node = nullptr;

        node = OCI_JsonNodeCreateObject(json);
        ASSERT_EQ(OCI_JNT_OBJECT, OCI_JsonNodeGetType(node));
        ASSERT_FALSE(OCI_JsonNodeIsNull(node));
        OCI_JsonNodeFree(node);

        node = OCI_JsonNodeCreateArray(json);
        ASSERT_EQ(OCI_JNT_ARRAY, OCI_JsonNodeGetType(node));
        ASSERT_FALSE(OCI_JsonNodeIsNull(node));
        OCI_JsonNodeFree(node);

        node = OCI_JsonNodeCreateNull(json);
        ASSERT_EQ(OCI_JNT_NULL, OCI_JsonNodeGetType(node));
        ASSERT_TRUE(OCI_JsonNodeIsNull(node));
        OCI_JsonNodeFree(node);

        node = OCI_JsonNodeCreateBoolean(json, TRUE);
        ASSERT_EQ(OCI_JNT_BOOLEAN, OCI_JsonNodeGetType(node));
        ASSERT_FALSE(OCI_JsonNodeIsNull(node));
        OCI_JsonNodeFree(node);

        node = OCI_JsonNodeCreateString(json, OTEXT("hello"), 5);
        ASSERT_EQ(OCI_JNT_STRING, OCI_JsonNodeGetType(node));
        OCI_JsonNodeFree(node);

        OCI_Number* num = MakeDoubleNumber(env, 1.5);
        ASSERT_NE(nullptr, num);
        node = OCI_JsonNodeCreateNumber(json, num);
        ASSERT_EQ(OCI_JNT_NUMBER, OCI_JsonNodeGetType(node));
        OCI_JsonNodeFree(node);
        OCI_NumberFree(num);

        unsigned char bin[] = { 0xAB, 0xCD };
        node = OCI_JsonNodeCreateBinary(json, bin, 2);
        ASSERT_EQ(OCI_JNT_BINARY, OCI_JsonNodeGetType(node));
        OCI_JsonNodeFree(node);

        OCI_Date* date = OCI_DateCreate(env);
        ASSERT_NE(nullptr, date);
        OCI_DateSetDate(date, 2024, 1, 1);
        node = OCI_JsonNodeCreateDate(json, date);
        ASSERT_EQ(OCI_JNT_DATE, OCI_JsonNodeGetType(node));
        OCI_JsonNodeFree(node);
        OCI_DateFree(date);

        OCI_Timestamp* ts = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        ASSERT_NE(nullptr, ts);
        OCI_TimestampConstruct(ts, 2024, 1, 1, 12, 0, 0, 0, nullptr);
        node = OCI_JsonNodeCreateTimestamp(json, ts);
        ASSERT_EQ(OCI_JNT_TIMESTAMP, OCI_JsonNodeGetType(node));
        OCI_JsonNodeFree(node);
        OCI_TimestampFree(ts);

        OCI_Interval* itv = OCI_IntervalCreate(env, OCI_INTERVAL_YM);
        ASSERT_NE(nullptr, itv);
        OCI_IntervalSetYearMonth(itv, 1, 2);
        node = OCI_JsonNodeCreateInterval(json, itv);
        ASSERT_EQ(OCI_JNT_INTERVAL, OCI_JsonNodeGetType(node));
        OCI_JsonNodeFree(node);
        OCI_IntervalFree(itv);

        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
    TEST(TestJsonNodeType, TimestampWithTimeZoneC)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);
        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JV_ALLOW_ALL, OCI_JE_DETECT));

        OCI_JsonNode* oldRoot = OCI_JsonGetRoot(json);
        ASSERT_NE(nullptr, oldRoot);
        OCI_JsonNodeFree(oldRoot);

        /* positive offset */

        OCI_Timestamp* ts = OCI_TimestampCreate(env, OCI_TIMESTAMP_TZ);
        ASSERT_NE(nullptr, ts);
        ASSERT_TRUE(OCI_TimestampConstruct(ts, 2024, 3, 1, 8, 15, 30, 0, OTEXT("+02:00")));

        OCI_JsonNode* node = OCI_JsonNodeCreateTimestamp(json, ts);
        ASSERT_NE(nullptr, node);
        ASSERT_EQ(OCI_JNT_TIMESTAMP, OCI_JsonNodeGetType(node));

        OCI_Timestamp* out = OCI_JsonNodeGetTimestamp(node);
        ASSERT_NE(nullptr, out);
        ASSERT_EQ(OCI_TIMESTAMP_TZ, OCI_TimestampGetType(out));

        OCI_Int32 y = 0, m = 0, d = 0, h = 0, mn = 0, s = 0, f = 0;
        ASSERT_TRUE(OCI_TimestampGetDateTime(out, &y, &m, &d, &h, &mn, &s, &f));
        ASSERT_EQ(2024, y);
        ASSERT_EQ(3, m);
        ASSERT_EQ(1, d);
        ASSERT_EQ(8, h);
        ASSERT_EQ(15, mn);

        OCI_Int32 tzh = 0, tzm = 0;
        ASSERT_TRUE(OCI_TimestampGetTimeZoneOffset(out, &tzh, &tzm));
        ASSERT_EQ(2, tzh);
        ASSERT_EQ(0, tzm);

        OCI_TimestampFree(out);
        OCI_JsonNodeFree(node);
        OCI_TimestampFree(ts);

        /* negative offset with minutes */

        ts = OCI_TimestampCreate(env, OCI_TIMESTAMP_TZ);
        ASSERT_NE(nullptr, ts);
        ASSERT_TRUE(OCI_TimestampConstruct(ts, 2024, 6, 20, 23, 45, 10, 0, OTEXT("-05:30")));

        node = OCI_JsonNodeCreateTimestamp(json, ts);
        ASSERT_NE(nullptr, node);
        ASSERT_EQ(OCI_JNT_TIMESTAMP, OCI_JsonNodeGetType(node));

        out = OCI_JsonNodeGetTimestamp(node);
        ASSERT_NE(nullptr, out);
        ASSERT_EQ(OCI_TIMESTAMP_TZ, OCI_TimestampGetType(out));
        ASSERT_TRUE(OCI_TimestampGetTimeZoneOffset(out, &tzh, &tzm));
        ASSERT_EQ(-5, tzh);
        ASSERT_EQ(-30, tzm);

        OCI_TimestampFree(out);
        OCI_JsonNodeFree(node);
        OCI_TimestampFree(ts);

        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
    TEST(TestJsonNodeType, TimestampTzFetchedFieldC)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);
        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("SELECT JSON_OBJECT('z' VALUE TIMESTAMP '2024-01-15 10:30:00 +02:00' RETURNING JSON) FROM dual")));
        ASSERT_TRUE(OCI_Execute(stmt));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto json = OCI_GetJson(rs, 1);
        ASSERT_NE(nullptr, json);

        OCI_JsonNode* root = OCI_JsonGetRoot(json);
        ASSERT_NE(nullptr, root);

        OCI_JsonNode* field = OCI_JsonNodeGetFieldByName(root, OTEXT("z"));
        ASSERT_NE(nullptr, field);
        ASSERT_EQ(OCI_JNT_TIMESTAMP, OCI_JsonNodeGetType(field));

        OCI_Timestamp* out = OCI_JsonNodeGetTimestamp(field);
        ASSERT_NE(nullptr, out);
        ASSERT_EQ(OCI_TIMESTAMP_TZ, OCI_TimestampGetType(out));

        OCI_Int32 tzh = 0, tzm = 0;
        ASSERT_TRUE(OCI_TimestampGetTimeZoneOffset(out, &tzh, &tzm));
        ASSERT_EQ(2, tzh);
        ASSERT_EQ(0, tzm);

        OCI_TimestampFree(out);
        OCI_JsonNodeFree(field);
        OCI_JsonNodeFree(root);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
    TEST(TestJsonNodeType, IntervalDaySecondC)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);
        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JV_ALLOW_ALL, OCI_JE_DETECT));

        OCI_JsonNode* oldRoot = OCI_JsonGetRoot(json);
        ASSERT_NE(nullptr, oldRoot);
        OCI_JsonNodeFree(oldRoot);

        OCI_Interval* itv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        ASSERT_NE(nullptr, itv);
        ASSERT_TRUE(OCI_IntervalSetDaySecond(itv, 2, 3, 4, 5, 0));

        OCI_JsonNode* node = OCI_JsonNodeCreateInterval(json, itv);
        ASSERT_NE(nullptr, node);
        ASSERT_EQ(OCI_JNT_INTERVAL, OCI_JsonNodeGetType(node));

        OCI_Interval* out = OCI_JsonNodeGetInterval(node);
        ASSERT_NE(nullptr, out);
        ASSERT_EQ(OCI_INTERVAL_DS, OCI_IntervalGetType(out));

        OCI_Int32 d = 0, h = 0, mn = 0, s = 0, ns = 0;
        ASSERT_TRUE(OCI_IntervalGetDaySecond(out, &d, &h, &mn, &s, &ns));
        ASSERT_EQ(2, d);
        ASSERT_EQ(3, h);
        ASSERT_EQ(4, mn);
        ASSERT_EQ(5, s);

        OCI_IntervalFree(out);
        OCI_JsonNodeFree(node);
        OCI_IntervalFree(itv);

        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
    TEST(TestJsonNodeType, IntervalDaySecondFetchedFieldC)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);
        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("SELECT JSON_OBJECT('i' VALUE INTERVAL '2 03:04:05' DAY TO SECOND RETURNING JSON) FROM dual")));
        ASSERT_TRUE(OCI_Execute(stmt));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto json = OCI_GetJson(rs, 1);
        ASSERT_NE(nullptr, json);

        OCI_JsonNode* root = OCI_JsonGetRoot(json);
        ASSERT_NE(nullptr, root);

        OCI_JsonNode* field = OCI_JsonNodeGetFieldByName(root, OTEXT("i"));
        ASSERT_NE(nullptr, field);
        ASSERT_EQ(OCI_JNT_INTERVAL, OCI_JsonNodeGetType(field));

        OCI_Interval* out = OCI_JsonNodeGetInterval(field);
        ASSERT_NE(nullptr, out);
        ASSERT_EQ(OCI_INTERVAL_DS, OCI_IntervalGetType(out));

        OCI_IntervalFree(out);
        OCI_JsonNodeFree(field);
        OCI_JsonNodeFree(root);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
    TEST(TestJsonNodeType, OraBooleanScalarC)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);
        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JV_ALLOW_ALL, OCI_JE_DETECT));

        OCI_JsonNode* oldRoot = OCI_JsonGetRoot(json);
        ASSERT_NE(nullptr, oldRoot);
        OCI_JsonNodeFree(oldRoot);

        /* JZNVAL_ORA_BOOLEAN is an Oracle extended scalar that no public
           constructor produces; see test_internals.c for how it is built */

        OCI_JsonNode* node = OCI_TestJsonNodeCreateOraBoolean(json, TRUE);
        ASSERT_NE(nullptr, node);
        ASSERT_EQ(OCI_JNT_BOOLEAN, OCI_JsonNodeGetType(node));
        ASSERT_TRUE(OCI_JsonNodeGetBoolean(node));
        OCI_JsonNodeFree(node);

        node = OCI_TestJsonNodeCreateOraBoolean(json, FALSE);
        ASSERT_NE(nullptr, node);
        ASSERT_EQ(OCI_JNT_BOOLEAN, OCI_JsonNodeGetType(node));
        ASSERT_FALSE(OCI_JsonNodeGetBoolean(node));
        OCI_JsonNodeFree(node);

        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
    TEST(TestJsonNodeType, OpaqueScalarTypeC)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);
        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JV_ALLOW_ALL, OCI_JE_DETECT));

        OCI_JsonNode* oldRoot = OCI_JsonGetRoot(json);
        ASSERT_NE(nullptr, oldRoot);
        OCI_JsonNodeFree(oldRoot);

        /* a scalar whose jzn type has no public mapping resolves to OCI_JNT_NULL */

        OCI_JsonNode* node = OCI_TestJsonNodeCreateOpaqueScalar(json);
        ASSERT_NE(nullptr, node);
        ASSERT_EQ(OCI_JNT_NULL, OCI_JsonNodeGetType(node));
        OCI_JsonNodeFree(node);

        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
    TEST(TestJsonNodeType, LazyTypeResolutionC)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);
        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JV_ALLOW_ALL, OCI_JE_DETECT));

        OCI_JsonNode* oldRoot = OCI_JsonGetRoot(json);
        ASSERT_NE(nullptr, oldRoot);
        OCI_JsonNodeFree(oldRoot);

        /* force the lazy type resolution path in OCI_JsonNodeGetType */

        OCI_JsonNode* node = OCI_JsonNodeCreateObject(json);
        ASSERT_NE(nullptr, node);
        OCI_TestJsonNodeResetType(node);
        ASSERT_EQ(OCI_JNT_OBJECT, OCI_JsonNodeGetType(node));
        OCI_JsonNodeFree(node);

        node = OCI_JsonNodeCreateArray(json);
        ASSERT_NE(nullptr, node);
        OCI_TestJsonNodeResetType(node);
        ASSERT_EQ(OCI_JNT_ARRAY, OCI_JsonNodeGetType(node));
        OCI_JsonNodeFree(node);

        /* node without a DOM handle resolves to OCI_JNT_NULL */

        node = OCI_TestJsonNodeCreateNullHandle(json);
        ASSERT_NE(nullptr, node);
        ASSERT_EQ(OCI_JNT_NULL, OCI_JsonNodeGetType(node));
        OCI_JsonNodeFree(node);

        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
    TEST(TestJsonNodeType, NumberFromTextScalarC)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);
        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JV_ALLOW_ALL, OCI_JE_DETECT));

        OCI_JsonNode* oldRoot = OCI_JsonGetRoot(json);
        ASSERT_NE(nullptr, oldRoot);
        OCI_JsonNodeFree(oldRoot);

        /* JZNVAL_NUMBER scalars store their value as text and are converted
           through OCINumberFromText (see test_internals.c) */

        OCI_JsonNode* node = OCI_TestJsonNodeCreateNumberText(json, OTEXT("42.5"));
        ASSERT_NE(nullptr, node);
        ASSERT_EQ(OCI_JNT_NUMBER, OCI_JsonNodeGetType(node));

        OCI_Number* num = OCI_JsonNodeGetNumber(node);
        ASSERT_NE(nullptr, num);

        double d = 0.0;
        ASSERT_TRUE(OCI_NumberGetValue(num, OCI_NUM_DOUBLE, &d));
        ASSERT_DOUBLE_EQ(42.5, d);

        OCI_NumberFree(num);
        OCI_JsonNodeFree(node);

        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
    TEST(TestJsonNodeType, GenericCreateC)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);
        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JV_ALLOW_ALL, OCI_JE_DETECT));

        OCI_JsonNode* oldRoot = OCI_JsonGetRoot(json);
        ASSERT_NE(nullptr, oldRoot);
        OCI_JsonNodeFree(oldRoot);

        const OCI_JsonType types[] =
        {
            OCI_JNT_NULL, OCI_JNT_OBJECT, OCI_JNT_ARRAY, OCI_JNT_STRING,
            OCI_JNT_NUMBER, OCI_JNT_BOOLEAN, OCI_JNT_BINARY, OCI_JNT_DATE,
            OCI_JNT_TIMESTAMP, OCI_JNT_INTERVAL
        };

        for (OCI_Int32 i = 0; i < 10; i++)
        {
            OCI_JsonNode* node = OCI_JsonNodeCreate(json, types[i]);
            ASSERT_NE(nullptr, node);
            ASSERT_EQ(types[i], OCI_JsonNodeGetType(node));
            OCI_JsonNodeFree(node);
        }

        ASSERT_TRUE(OCI_JsonNodeCreate(json, (OCI_JsonType) -1) == NULL);

        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
    TEST(TestJsonNodeType, CachedStringReuseC)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);
        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JV_ALLOW_ALL, OCI_JE_DETECT));

        OCI_JsonNode* oldRoot = OCI_JsonGetRoot(json);
        ASSERT_NE(nullptr, oldRoot);
        OCI_JsonNodeFree(oldRoot);

        /* repeated GetString releases the previously cached string */

        OCI_JsonNode* str = OCI_JsonNodeCreateString(json, OTEXT("hello"), 0);
        ASSERT_NE(nullptr, str);
        ASSERT_NE(nullptr, OCI_JsonNodeGetString(str, nullptr));
        ASSERT_EQ(ostring(OTEXT("hello")), ostring(OCI_JsonNodeGetString(str, nullptr)));

        /* repeated GetFieldName releases the previously cached field name */

        OCI_JsonNode* root = OCI_JsonNodeCreateObject(json);
        ASSERT_NE(nullptr, root);
        ASSERT_TRUE(OCI_JsonNodeSetField(root, OTEXT("name"), str));

        ASSERT_EQ(ostring(OTEXT("name")), ostring(OCI_JsonNodeGetFieldName(root, 1)));
        ASSERT_EQ(ostring(OTEXT("name")), ostring(OCI_JsonNodeGetFieldName(root, 1)));

        OCI_JsonNodeFree(str);
        OCI_JsonNodeFree(root);

        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
    TEST(TestJsonNodeType, ScalarSettersC)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);
        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JV_ALLOW_ALL, OCI_JE_DETECT));

        OCI_JsonNode* oldRoot = OCI_JsonGetRoot(json);
        ASSERT_NE(nullptr, oldRoot);
        OCI_JsonNodeFree(oldRoot);

        OCI_JsonNode* str = OCI_JsonNodeCreateString(json, OTEXT("hello"), 0);
        ASSERT_EQ(ostring(OTEXT("hello")), ostring(OCI_JsonNodeGetString(str, nullptr)));
        OCI_JsonNodeFree(str);

        double v = 1.5;
        OCI_Number* num = MakeDoubleNumber(env, v);
        ASSERT_NE(nullptr, num);
        OCI_JsonNode* number = OCI_JsonNodeCreateNumber(json, num);
        OCI_NumberFree(num);

        OCI_Number* out = OCI_JsonNodeGetNumber(number);
        ASSERT_NE(nullptr, out);
        double value = 0.0;
        ASSERT_TRUE(OCI_NumberGetValue(out, OCI_NUM_DOUBLE, &value));
        ASSERT_DOUBLE_EQ(1.5, value);
        OCI_NumberFree(out);

        OCI_JsonNodeFree(number);

        OCI_JsonNode* boolean = OCI_JsonNodeCreateBoolean(json, FALSE);
        ASSERT_FALSE(OCI_JsonNodeGetBoolean(boolean));
        OCI_JsonNodeFree(boolean);

        unsigned char d1[] = { 0xAB, 0xCD };
        unsigned char d2[] = { 0x12 };
        OCI_JsonNode* binary = OCI_JsonNodeCreateBinary(json, d1, 2);
        OCI_UInt32 len = 0;
        const void* data = OCI_JsonNodeGetBinary(binary, &len);
        ASSERT_EQ(2u, len);
        ASSERT_EQ(0, memcmp(data, d1, 2));
        OCI_JsonNodeFree(binary);

        OCI_Date* date = OCI_DateCreate(env);
        OCI_DateSetDate(date, 2024, 5, 15);
        OCI_JsonNode* dateNode = OCI_JsonNodeCreateDate(json, date);
        OCI_Date* dateOut = OCI_JsonNodeGetDate(dateNode);
        ASSERT_NE(nullptr, dateOut);
        OCI_Int32 y = 0, m = 0, d = 0;
        OCI_DateGetDate(dateOut, &y, &m, &d);
        ASSERT_EQ(2024, y); ASSERT_EQ(5, m); ASSERT_EQ(15, d);
        OCI_DateFree(dateOut);

        OCI_DateFree(date);
        OCI_JsonNodeFree(dateNode);

        OCI_Timestamp* ts = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        OCI_TimestampConstruct(ts, 2024, 5, 15, 10, 30, 0, 0, nullptr);
        OCI_JsonNode* tsNode = OCI_JsonNodeCreateTimestamp(json, ts);
        OCI_Timestamp* tsOut = OCI_JsonNodeGetTimestamp(tsNode);
        ASSERT_NE(nullptr, tsOut);
        OCI_Int32 h = 0, mn = 0, s = 0, f = 0;
        OCI_TimestampGetDateTime(tsOut, &y, &m, &d, &h, &mn, &s, &f);
        ASSERT_EQ(2024, y); ASSERT_EQ(5, m); ASSERT_EQ(15, d);
        ASSERT_EQ(10, h); ASSERT_EQ(30, mn);
        OCI_TimestampFree(tsOut);

        OCI_TimestampFree(ts);
        OCI_JsonNodeFree(tsNode);

        OCI_Interval* itv = OCI_IntervalCreate(env, OCI_INTERVAL_YM);
        OCI_IntervalSetYearMonth(itv, 1, 6);
        OCI_JsonNode* itvNode = OCI_JsonNodeCreateInterval(json, itv);
        OCI_Interval* itvOut = OCI_JsonNodeGetInterval(itvNode);
        ASSERT_NE(nullptr, itvOut);
        OCI_IntervalGetYearMonth(itvOut, &y, &m);
        ASSERT_EQ(1, y); ASSERT_EQ(6, m);
        OCI_IntervalFree(itvOut);

        OCI_IntervalFree(itv);
        OCI_JsonNodeFree(itvNode);

        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
}
