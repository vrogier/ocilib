#include "ocilib_tests.h"

#include <cstring>

using namespace ocilib;

namespace
{
    OCI_Number* MakeDoubleNumber(OCI_Environment* env, OCI_Double value)
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
    /* -------------------------------------------------------------------------
       Parameterized: OCI_JsonNodeCreateX reports the right node type per kind
       ------------------------------------------------------------------------- */

    struct JntCase
    {
        const char* name;
        void (*run)(OCI_Environment*, OCI_Json*);
    };

    void PrintTo(const JntCase& c, std::ostream* os) { *os << c.name; }

    static void JntObject(OCI_Environment*, OCI_Json* json)
    {
        const auto node = OCI_JsonNodeCreateObject(json);
        ASSERT_NE(nullptr, node);
        EXPECT_EQ(OCI_JNT_OBJECT, OCI_JsonNodeGetType(node));
        EXPECT_FALSE(OCI_JsonNodeIsNull(node));
        OCI_JsonNodeFree(node);
    }
    static void JntArray(OCI_Environment*, OCI_Json* json)
    {
        const auto node = OCI_JsonNodeCreateArray(json);
        ASSERT_NE(nullptr, node);
        EXPECT_EQ(OCI_JNT_ARRAY, OCI_JsonNodeGetType(node));
        EXPECT_FALSE(OCI_JsonNodeIsNull(node));
        OCI_JsonNodeFree(node);
    }
    static void JntNull(OCI_Environment*, OCI_Json* json)
    {
        const auto node = OCI_JsonNodeCreateNull(json);
        ASSERT_NE(nullptr, node);
        EXPECT_EQ(OCI_JNT_NULL, OCI_JsonNodeGetType(node));
        EXPECT_TRUE(OCI_JsonNodeIsNull(node));
        OCI_JsonNodeFree(node);
    }
    static void JntBoolean(OCI_Environment*, OCI_Json* json)
    {
        const auto node = OCI_JsonNodeCreateBoolean(json, TRUE);
        ASSERT_NE(nullptr, node);
        EXPECT_EQ(OCI_JNT_BOOLEAN, OCI_JsonNodeGetType(node));
        EXPECT_FALSE(OCI_JsonNodeIsNull(node));
        OCI_JsonNodeFree(node);
    }
    static void JntString(OCI_Environment*, OCI_Json* json)
    {
        const auto node = OCI_JsonNodeCreateString(json, OTEXT("hello"), 5);
        ASSERT_NE(nullptr, node);
        EXPECT_EQ(OCI_JNT_STRING, OCI_JsonNodeGetType(node));
        OCI_JsonNodeFree(node);
    }
    static void JntNumber(OCI_Environment* env, OCI_Json* json)
    {
        OCI_Number* num = MakeDoubleNumber(env, 1.5);
        ASSERT_NE(nullptr, num);
        const auto node = OCI_JsonNodeCreateNumber(json, num);
        ASSERT_NE(nullptr, node);
        EXPECT_EQ(OCI_JNT_NUMBER, OCI_JsonNodeGetType(node));
        OCI_JsonNodeFree(node);
        OCI_NumberFree(num);
    }
    static void JntBinary(OCI_Environment*, OCI_Json* json)
    {
        unsigned char bin[] = { 0xAB, 0xCD };
        const auto node = OCI_JsonNodeCreateBinary(json, bin, 2);
        ASSERT_NE(nullptr, node);
        EXPECT_EQ(OCI_JNT_BINARY, OCI_JsonNodeGetType(node));
        OCI_JsonNodeFree(node);
    }
    static void JntDate(OCI_Environment* env, OCI_Json* json)
    {
        OCI_Date* date = OCI_DateCreate(env);
        ASSERT_NE(nullptr, date);
        OCI_DateSetDate(date, 2024, 1, 1);
        const auto node = OCI_JsonNodeCreateDate(json, date);
        ASSERT_NE(nullptr, node);
        EXPECT_EQ(OCI_JNT_DATE, OCI_JsonNodeGetType(node));
        OCI_JsonNodeFree(node);
        OCI_DateFree(date);
    }
    static void JntTimestamp(OCI_Environment* env, OCI_Json* json)
    {
        OCI_Timestamp* ts = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        ASSERT_NE(nullptr, ts);
        OCI_TimestampConstruct(ts, 2024, 1, 1, 12, 0, 0, 0, nullptr);
        const auto node = OCI_JsonNodeCreateTimestamp(json, ts);
        ASSERT_NE(nullptr, node);
        EXPECT_EQ(OCI_JNT_TIMESTAMP, OCI_JsonNodeGetType(node));
        OCI_JsonNodeFree(node);
        OCI_TimestampFree(ts);
    }
    static void JntInterval(OCI_Environment* env, OCI_Json* json)
    {
        OCI_Interval* itv = OCI_IntervalCreate(env, OCI_INTERVAL_YM);
        ASSERT_NE(nullptr, itv);
        OCI_IntervalSetYearMonth(itv, 1, 2);
        const auto node = OCI_JsonNodeCreateInterval(json, itv);
        ASSERT_NE(nullptr, node);
        EXPECT_EQ(OCI_JNT_INTERVAL, OCI_JsonNodeGetType(node));
        OCI_JsonNodeFree(node);
        OCI_IntervalFree(itv);
    }

    class TestJntCreate : public ::testing::TestWithParam<JntCase>
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_Json* json = nullptr;

        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            json = OCI_JsonCreate(conn);
            ASSERT_NE(nullptr, json);
            ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
            ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));
        }

        void TearDown() override
        {
            if (json) OCI_JsonFree(json);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_P(TestJntCreate, CreateNode_ReportsExpectedType)
    {
        GetParam().run(GetEnvironment(nullptr), json);
    }

    INSTANTIATE_TEST_CASE_P(NodeType, TestJntCreate,
        ::testing::Values(
            JntCase{ "Object",    JntObject },
            JntCase{ "Array",     JntArray },
            JntCase{ "Null",      JntNull },
            JntCase{ "Boolean",   JntBoolean },
            JntCase{ "String",    JntString },
            JntCase{ "Number",    JntNumber },
            JntCase{ "Binary",    JntBinary },
            JntCase{ "Date",      JntDate },
            JntCase{ "Timestamp", JntTimestamp },
            JntCase{ "Interval",  JntInterval }),
        ::testing::PrintToStringParamName());
    /* -------------------------------------------------------------------------
       Parameterized: timestamp-with-timezone node round-trips per offset
       ------------------------------------------------------------------------- */

    struct TsTzCase
    {
        const char* name;
        OCI_Int32 y, mo, d, h, mn;
        const otext* tz;
        OCI_Int32 tzh, tzm;
    };

    void PrintTo(const TsTzCase& c, std::ostream* os) { *os << c.name; }

    class TestJntTsTz : public ::testing::TestWithParam<TsTzCase>
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_Json* json = nullptr;

        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            json = OCI_JsonCreate(conn);
            ASSERT_NE(nullptr, json);
            ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
            ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));
        }

        void TearDown() override
        {
            if (json) OCI_JsonFree(json);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_P(TestJntTsTz, CreateTimestampNode_RoundTripsDateTimeAndOffset)
    {
        const auto& p = GetParam();
        auto env = GetEnvironment(nullptr);

        OCI_Timestamp* ts = OCI_TimestampCreate(env, OCI_TIMESTAMP_TZ);
        ASSERT_NE(nullptr, ts);
        ASSERT_TRUE(OCI_TimestampConstruct(ts, p.y, p.mo, p.d, p.h, p.mn, 30, 0, p.tz));

        OCI_JsonNode* node = OCI_JsonNodeCreateTimestamp(json, ts);
        ASSERT_NE(nullptr, node);
        EXPECT_EQ(OCI_JNT_TIMESTAMP, OCI_JsonNodeGetType(node));

        OCI_Timestamp* out = OCI_JsonNodeGetTimestamp(node);
        ASSERT_NE(nullptr, out);
        EXPECT_EQ(OCI_TIMESTAMP_TZ, OCI_TimestampGetType(out));

        OCI_Int32 y = 0, m = 0, d = 0, h = 0, mn = 0, s = 0, f = 0;
        ASSERT_TRUE(OCI_TimestampGetDateTime(out, &y, &m, &d, &h, &mn, &s, &f));
        EXPECT_EQ(p.y, y);
        EXPECT_EQ(p.mo, m);
        EXPECT_EQ(p.d, d);
        EXPECT_EQ(p.h, h);
        EXPECT_EQ(p.mn, mn);

        OCI_Int32 tzh = 0, tzm = 0;
        ASSERT_TRUE(OCI_TimestampGetTimeZoneOffset(out, &tzh, &tzm));
        EXPECT_EQ(p.tzh, tzh);
        EXPECT_EQ(p.tzm, tzm);

        OCI_TimestampFree(out);
        OCI_JsonNodeFree(node);
        OCI_TimestampFree(ts);
    }

    INSTANTIATE_TEST_CASE_P(TsOffset, TestJntTsTz,
        ::testing::Values(
            TsTzCase{ "Positive", 2024, 3, 1,  8, 15, OTEXT("+02:00"), 2,  0 },
            TsTzCase{ "Negative", 2024, 6, 20, 23, 45, OTEXT("-05:30"), -5, -30 }),
        ::testing::PrintToStringParamName());
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

        const auto json = OCI_GetJson(rs, 0);
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
        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));

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

        const auto json = OCI_GetJson(rs, 0);
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
    TEST(TestJsonNodeType, GenericCreateC)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);
        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));

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
        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));

        /* repeated GetString releases the previously cached string */

        OCI_JsonNode* str = OCI_JsonNodeCreateString(json, OTEXT("hello"), 0);
        ASSERT_NE(nullptr, str);
        ASSERT_NE(nullptr, OCI_JsonNodeGetString(str, nullptr));
        ASSERT_EQ(String(OTEXT("hello")), String(OCI_JsonNodeGetString(str, nullptr)));

        /* repeated GetFieldName releases the previously cached field name */

        OCI_JsonNode* root = OCI_JsonNodeCreateObject(json);
        ASSERT_NE(nullptr, root);
        ASSERT_TRUE(OCI_JsonNodeSetField(root, OTEXT("name"), str));

        ASSERT_EQ(String(OTEXT("name")), String(OCI_JsonNodeGetFieldName(root, 0)));
        ASSERT_EQ(String(OTEXT("name")), String(OCI_JsonNodeGetFieldName(root, 0)));

        OCI_JsonNodeFree(str);
        OCI_JsonNodeFree(root);

        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
    /* -------------------------------------------------------------------------
       Parameterized: typed node create + getter round-trip
       ------------------------------------------------------------------------- */

    struct JsonScalarCase
    {
        const char* name;
        void (*run)(OCI_Environment*, OCI_Json*);
    };

    void PrintTo(const JsonScalarCase& c, std::ostream* os) { *os << c.name; }

    static void ScalarGetString(OCI_Environment*, OCI_Json* json)
    {
        OCI_JsonNode* str = OCI_JsonNodeCreateString(json, OTEXT("hello"), 0);
        ASSERT_NE(nullptr, str);
        EXPECT_EQ(String(OTEXT("hello")), String(OCI_JsonNodeGetString(str, nullptr)));
        OCI_JsonNodeFree(str);
    }
    static void ScalarGetNumber(OCI_Environment* env, OCI_Json* json)
    {
        OCI_Number* num = MakeDoubleNumber(env, 1.5);
        ASSERT_NE(nullptr, num);
        OCI_JsonNode* number = OCI_JsonNodeCreateNumber(json, num);
        OCI_NumberFree(num);
        ASSERT_NE(nullptr, number);
        OCI_Number* out = OCI_JsonNodeGetNumber(number);
        ASSERT_NE(nullptr, out);
        OCI_Double value = 0.0;
        ASSERT_TRUE(OCI_NumberGetValue(out, OCI_NUM_DOUBLE, &value));
        EXPECT_DOUBLE_EQ(1.5, value);
        OCI_NumberFree(out);
        OCI_JsonNodeFree(number);
    }
    static void ScalarGetBoolean(OCI_Environment*, OCI_Json* json)
    {
        OCI_JsonNode* b = OCI_JsonNodeCreateBoolean(json, FALSE);
        ASSERT_NE(nullptr, b);
        EXPECT_FALSE(OCI_JsonNodeGetBoolean(b));
        OCI_JsonNodeFree(b);
    }
    static void ScalarGetBinary(OCI_Environment*, OCI_Json* json)
    {
        unsigned char d1[] = { 0xAB, 0xCD };
        OCI_JsonNode* binary = OCI_JsonNodeCreateBinary(json, d1, 2);
        ASSERT_NE(nullptr, binary);
        OCI_UInt32 len = 0;
        const void* data = OCI_JsonNodeGetBinary(binary, &len);
        EXPECT_EQ(2u, len);
        EXPECT_EQ(0, memcmp(data, d1, 2));
        OCI_JsonNodeFree(binary);
    }
    static void ScalarGetDate(OCI_Environment* env, OCI_Json* json)
    {
        OCI_Date* date = OCI_DateCreate(env);
        OCI_DateSetDate(date, 2024, 5, 15);
        OCI_JsonNode* node = OCI_JsonNodeCreateDate(json, date);
        ASSERT_NE(nullptr, node);
        OCI_Date* out = OCI_JsonNodeGetDate(node);
        ASSERT_NE(nullptr, out);
        OCI_Int32 y = 0, m = 0, d = 0;
        OCI_DateGetDate(out, &y, &m, &d);
        EXPECT_EQ(2024, y); EXPECT_EQ(5, m); EXPECT_EQ(15, d);
        OCI_DateFree(out);
        OCI_DateFree(date);
        OCI_JsonNodeFree(node);
    }
    static void ScalarGetTimestamp(OCI_Environment* env, OCI_Json* json)
    {
        OCI_Timestamp* ts = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        OCI_TimestampConstruct(ts, 2024, 5, 15, 10, 30, 0, 0, nullptr);
        OCI_JsonNode* node = OCI_JsonNodeCreateTimestamp(json, ts);
        ASSERT_NE(nullptr, node);
        OCI_Timestamp* out = OCI_JsonNodeGetTimestamp(node);
        ASSERT_NE(nullptr, out);
        OCI_Int32 y = 0, m = 0, d = 0, h = 0, mn = 0, s = 0, f = 0;
        OCI_TimestampGetDateTime(out, &y, &m, &d, &h, &mn, &s, &f);
        EXPECT_EQ(2024, y); EXPECT_EQ(5, m); EXPECT_EQ(15, d);
        EXPECT_EQ(10, h); EXPECT_EQ(30, mn);
        OCI_TimestampFree(out);
        OCI_TimestampFree(ts);
        OCI_JsonNodeFree(node);
    }
    static void ScalarGetInterval(OCI_Environment* env, OCI_Json* json)
    {
        OCI_Interval* itv = OCI_IntervalCreate(env, OCI_INTERVAL_YM);
        OCI_IntervalSetYearMonth(itv, 1, 6);
        OCI_JsonNode* node = OCI_JsonNodeCreateInterval(json, itv);
        ASSERT_NE(nullptr, node);
        OCI_Interval* out = OCI_JsonNodeGetInterval(node);
        ASSERT_NE(nullptr, out);
        OCI_Int32 y = 0, m = 0;
        OCI_IntervalGetYearMonth(out, &y, &m);
        EXPECT_EQ(1, y); EXPECT_EQ(6, m);
        OCI_IntervalFree(out);
        OCI_IntervalFree(itv);
        OCI_JsonNodeFree(node);
    }

    class TestJsonScalarGet : public ::testing::TestWithParam<JsonScalarCase>
    {
    protected:
        OCI_Environment* env = nullptr;
        OCI_Connection* conn = nullptr;
        OCI_Json* json = nullptr;

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
        }

        void TearDown() override
        {
            if (json) OCI_JsonFree(json);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_P(TestJsonScalarGet, CreateThenGet_RoundTripsValue)
    {
        GetParam().run(env, json);
    }

    INSTANTIATE_TEST_CASE_P(ScalarType, TestJsonScalarGet,
        ::testing::Values(
            JsonScalarCase{ "String",    ScalarGetString },
            JsonScalarCase{ "Number",    ScalarGetNumber },
            JsonScalarCase{ "Boolean",   ScalarGetBoolean },
            JsonScalarCase{ "Binary",    ScalarGetBinary },
            JsonScalarCase{ "Date",      ScalarGetDate },
            JsonScalarCase{ "Timestamp", ScalarGetTimestamp },
            JsonScalarCase{ "Interval",  ScalarGetInterval }),
        ::testing::PrintToStringParamName());

    /* -------------------------------------------------------------------------
       Parameterized: created node reports its declared type
       ------------------------------------------------------------------------- */

    struct JsonTypeCase
    {
        const char* name;
        void (*run)(OCI_Environment*, OCI_Json*);
    };

    void PrintTo(const JsonTypeCase& c, std::ostream* os) { *os << c.name; }

    static void TypeBoolean(OCI_Environment*, OCI_Json* json)
    {
        const auto n = OCI_JsonNodeCreateBoolean(json, TRUE);
        ASSERT_NE(nullptr, n);
        EXPECT_EQ(OCI_JNT_BOOLEAN, OCI_JsonNodeGetType(n));
        EXPECT_EQ(TRUE, OCI_JsonNodeGetBoolean(n));
        EXPECT_FALSE(OCI_JsonNodeIsNull(n));
        ASSERT_TRUE(OCI_JsonNodeFree(n));
    }
    static void TypeBinary(OCI_Environment*, OCI_Json* json)
    {
        const auto n = OCI_JsonNodeCreateBinary(json, OTEXT("01"), 2);
        ASSERT_NE(nullptr, n);
        EXPECT_EQ(OCI_JNT_BINARY, OCI_JsonNodeGetType(n));
        ASSERT_TRUE(OCI_JsonNodeFree(n));
    }
    static void TypeDate(OCI_Environment* env, OCI_Json* json)
    {
        const auto dt = OCI_DateCreate(env);
        ASSERT_NE(nullptr, dt);
        ASSERT_TRUE(OCI_DateSetDate(dt, 2024, 1, 1));
        const auto n = OCI_JsonNodeCreateDate(json, dt);
        ASSERT_NE(nullptr, n);
        EXPECT_EQ(OCI_JNT_DATE, OCI_JsonNodeGetType(n));
        const auto out = OCI_JsonNodeGetDate(n);
        ASSERT_NE(nullptr, out);
        ASSERT_TRUE(OCI_DateFree(out));
        ASSERT_TRUE(OCI_JsonNodeFree(n));
        ASSERT_TRUE(OCI_DateFree(dt));
    }
    static void TypeTimestamp(OCI_Environment* env, OCI_Json* json)
    {
        const auto ts = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        ASSERT_NE(nullptr, ts);
        ASSERT_TRUE(OCI_TimestampConstruct(ts, 2024, 1, 1, 12, 0, 0, 0, nullptr));
        const auto n = OCI_JsonNodeCreateTimestamp(json, ts);
        ASSERT_NE(nullptr, n);
        EXPECT_EQ(OCI_JNT_TIMESTAMP, OCI_JsonNodeGetType(n));
        const auto out = OCI_JsonNodeGetTimestamp(n);
        ASSERT_NE(nullptr, out);
        ASSERT_TRUE(OCI_TimestampFree(out));
        ASSERT_TRUE(OCI_JsonNodeFree(n));
        ASSERT_TRUE(OCI_TimestampFree(ts));
    }
    static void TypeInterval(OCI_Environment* env, OCI_Json* json)
    {
        const auto itv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        ASSERT_NE(nullptr, itv);
        ASSERT_TRUE(OCI_IntervalSetDaySecond(itv, 1, 2, 3, 4, 0));
        const auto n = OCI_JsonNodeCreateInterval(json, itv);
        ASSERT_NE(nullptr, n);
        EXPECT_EQ(OCI_JNT_INTERVAL, OCI_JsonNodeGetType(n));
        const auto out = OCI_JsonNodeGetInterval(n);
        ASSERT_NE(nullptr, out);
        ASSERT_TRUE(OCI_IntervalFree(out));
        ASSERT_TRUE(OCI_JsonNodeFree(n));
        ASSERT_TRUE(OCI_IntervalFree(itv));
    }
    static void TypeNull(OCI_Environment*, OCI_Json* json)
    {
        const auto n = OCI_JsonNodeCreateNull(json);
        ASSERT_NE(nullptr, n);
        EXPECT_EQ(OCI_JNT_NULL, OCI_JsonNodeGetType(n));
        EXPECT_TRUE(OCI_JsonNodeIsNull(n));
        ASSERT_TRUE(OCI_JsonNodeFree(n));
    }
    static void TypeString(OCI_Environment*, OCI_Json* json)
    {
        const auto n = OCI_JsonNodeCreateString(json, OTEXT("abc"), 0);
        ASSERT_NE(nullptr, n);
        EXPECT_EQ(OCI_JNT_STRING, OCI_JsonNodeGetType(n));
        OCI_UInt32 sLen = 0;
        EXPECT_EQ(String(OTEXT("abc")), String(OCI_JsonNodeGetString(n, &sLen)));
        EXPECT_FALSE(OCI_JsonNodeSetNumber(n, nullptr));
        ASSERT_TRUE(OCI_JsonNodeFree(n));
    }

    class TestJsonScalarType : public ::testing::TestWithParam<JsonTypeCase>
    {
    protected:
        OCI_Environment* env = nullptr;
        OCI_Connection* conn = nullptr;
        OCI_Json* json = nullptr;

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
        }

        void TearDown() override
        {
            if (json) OCI_JsonFree(json);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_P(TestJsonScalarType, CreatedNode_ReportsDeclaredType)
    {
        GetParam().run(env, json);
    }

    INSTANTIATE_TEST_CASE_P(NodeKind, TestJsonScalarType,
        ::testing::Values(
            JsonTypeCase{ "Boolean",   TypeBoolean },
            JsonTypeCase{ "Binary",    TypeBinary },
            JsonTypeCase{ "Date",      TypeDate },
            JsonTypeCase{ "Timestamp", TypeTimestamp },
            JsonTypeCase{ "Interval",  TypeInterval },
            JsonTypeCase{ "Null",      TypeNull },
            JsonTypeCase{ "String",    TypeString }),
        ::testing::PrintToStringParamName());

    TEST_F(TestJsonScalarType, SetBooleanOnNumberNode_IsRejectedAndTypeUnchanged)
    {
        const auto n = OCI_JsonNodeCreate(json, OCI_JNT_NUMBER);
        ASSERT_NE(nullptr, n);

        EXPECT_FALSE(OCI_JsonNodeSetBoolean(n, TRUE));
        EXPECT_EQ(OCI_JNT_NUMBER, OCI_JsonNodeGetType(n));

        ASSERT_TRUE(OCI_JsonNodeFree(n));
    }

    TEST_F(TestJsonScalarType, SetNullOnNumberNode_ConvertsToNull)
    {
        const auto n = OCI_JsonNodeCreate(json, OCI_JNT_NUMBER);
        ASSERT_NE(nullptr, n);

        ASSERT_TRUE(OCI_JsonNodeSetNull(n));

        EXPECT_EQ(OCI_JNT_NULL, OCI_JsonNodeGetType(n));
        EXPECT_TRUE(OCI_JsonNodeIsNull(n));

        ASSERT_TRUE(OCI_JsonNodeFree(n));
    }
}
