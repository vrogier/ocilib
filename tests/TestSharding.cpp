#include "ocilib_tests.h"
#include "NullArgs.h"

namespace TestCApi
{
    TEST(TestSharding, KeyLifecycle)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        OCI_ShardingKey* skey = OCI_ShardingKeyCreate(env);
        ASSERT_NE(nullptr, skey);
        ASSERT_TRUE(OCI_ShardingKeyFree(skey));
    }

    TEST(TestSharding, KeyReset)
    {
        auto env = GetEnvironment(nullptr);

        OCI_ShardingKey* skey = OCI_ShardingKeyCreate(env);
        ASSERT_NE(nullptr, skey);

        ASSERT_TRUE(OCI_ShardingKeyAddString(skey, OTEXT("GOLD")));
        ASSERT_TRUE(OCI_ShardingKeyReset(skey));
        ASSERT_TRUE(OCI_ShardingKeyAddString(skey, OTEXT("SILVER")));

        ASSERT_TRUE(OCI_ShardingKeyFree(skey));
    }

    TEST(TestSharding, AddColumns)
    {
        auto env = GetEnvironment(nullptr);

        OCI_ShardingKey* skey = OCI_ShardingKeyCreate(env);
        ASSERT_NE(nullptr, skey);

        ASSERT_TRUE(OCI_ShardingKeyAddString(skey, OTEXT("GOLD")));

        OCI_Number* num = OCI_NumberCreate(env);
        ASSERT_NE(nullptr, num);
        int val = 42;
        ASSERT_TRUE(OCI_NumberSetValue(num, OCI_NUM_INT32, &val));
        ASSERT_TRUE(OCI_ShardingKeyAddNumber(skey, num));

        OCI_Date* date = OCI_DateCreate(env);
        ASSERT_NE(nullptr, date);
        ASSERT_TRUE(OCI_DateSysDate(date));
        ASSERT_TRUE(OCI_ShardingKeyAddDate(skey, date));

        OCI_Timestamp* tmsp = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        ASSERT_NE(nullptr, tmsp);
        ASSERT_TRUE(OCI_TimestampSysTimestamp(tmsp));
        ASSERT_TRUE(OCI_ShardingKeyAddTimestamp(skey, tmsp));

        unsigned char raw[4] = { 0x01, 0x02, 0x03, 0x04 };
        ASSERT_TRUE(OCI_ShardingKeyAddRaw(skey, raw, sizeof(raw)));

        OCI_TimestampFree(tmsp);
        OCI_DateFree(date);
        OCI_NumberFree(num);
        ASSERT_TRUE(OCI_ShardingKeyFree(skey));
    }

    TEST(TestSharding, ToText)
    {
        auto env = GetEnvironment(nullptr);

        OCI_ShardingKey* skey = OCI_ShardingKeyCreate(env);
        ASSERT_NE(nullptr, skey);

        ASSERT_TRUE(OCI_ShardingKeyAddString(skey, OTEXT("GOLD")));

        const otext* text = OCI_ShardingKeyToText(skey);
        ASSERT_NE(nullptr, text);
        ASSERT_TRUE(ostrlen(text) > 0);

        ASSERT_TRUE(OCI_ShardingKeyFree(skey));
    }

    TEST(TestSharding, NullKeyConnectionCreate)
    {
        auto env = GetEnvironment(nullptr);

        /* NULL keys must behave like OCI_ConnectionCreate() */

        OCI_Connection* con = OCI_ConnectionCreateSharded(env, DBS, USR, PWD, OCI_SESSION_DEFAULT, NULL, NULL);
        ASSERT_NE(nullptr, con);
        ASSERT_TRUE(OCI_ConnectionFree(con));
    }

    TEST(TestSharding, ShardedConnection)
    {
        auto env = GetEnvironment(nullptr);

        OCI_ShardingKey* skey = OCI_ShardingKeyCreate(env);
        ASSERT_NE(nullptr, skey);
        ASSERT_TRUE(OCI_ShardingKeyAddString(skey, OTEXT("GOLD")));

        /* on a non-sharded database, sharding attributes are accepted
           and a regular connection is established */

        OCI_Connection* con = OCI_ConnectionCreateSharded(env, DBS, USR, PWD, OCI_SESSION_DEFAULT, skey, NULL);
        ASSERT_NE(nullptr, con);


        ASSERT_TRUE(OCI_ConnectionFree(con));
        ASSERT_TRUE(OCI_ShardingKeyFree(skey));
    }

    TEST(TestSharding, ShardedPoolGetConnection)
    {
        auto env = GetEnvironment(nullptr);

        OCI_Pool* pool = OCI_PoolCreate(env, DBS, USR, PWD, OCI_POOL_SESSION, OCI_SESSION_DEFAULT, 1, 4, 1);
        ASSERT_NE(nullptr, pool);

        OCI_ShardingKey* skey = OCI_ShardingKeyCreate(env);
        ASSERT_NE(nullptr, skey);
        ASSERT_TRUE(OCI_ShardingKeyAddString(skey, OTEXT("GOLD")));

        OCI_Connection* con = OCI_PoolGetConnectionSharded(env, pool, NULL, skey, NULL);
        ASSERT_NE(nullptr, con);
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ASSERT_TRUE(OCI_ShardingKeyFree(skey));
        ASSERT_TRUE(OCI_PoolFree(env, pool));
    }

    TEST(TestSharding, ErrorPaths)
    {
        ASSERT_FALSE(OCI_ShardingKeyFree(NULL));
        ASSERT_FALSE(OCI_ShardingKeyReset(NULL));
        ASSERT_FALSE(OCI_ShardingKeyAddString(NULL, OTEXT("x")));

        auto env = GetEnvironment(nullptr);
        OCI_ShardingKey* skey = OCI_ShardingKeyCreate(env);
        ASSERT_NE(nullptr, skey);

        ASSERT_FALSE(OCI_ShardingKeyAddString(skey, NULL));
        ASSERT_FALSE(OCI_ShardingKeyAddRaw(skey, NULL, 10));

        ASSERT_TRUE(OCI_ShardingKeyFree(skey));
    }

    TEST(TestSharding, CreateKey_PreClient12_2_Fails)
    {
        /* sharding keys are a 12.2 feature */

        const auto env = GetSpecificEnvironment(nullptr, OCI_12_1);
        ASSERT_NE(nullptr, env);
        ASSERT_EQ(nullptr, OCI_ShardingKeyCreate(env));
        ASSERT_NE(nullptr, OCI_GetLastError());

        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }


    class TestShardingNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestShardingNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestShardingNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "ShardingKeyAddDate", [] { return !OCI_ShardingKeyAddDate(nullptr, nullptr); } },
            { "ShardingKeyAddNumber", [] { return !OCI_ShardingKeyAddNumber(nullptr, nullptr); } },
            { "ShardingKeyAddRaw", [] { return !OCI_ShardingKeyAddRaw(nullptr, NullArgBuffer(), 1); } },
            { "ShardingKeyAddString", [] { return !OCI_ShardingKeyAddString(nullptr, OTEXT("x")); } },
            { "ShardingKeyAddTimestamp", [] { return !OCI_ShardingKeyAddTimestamp(nullptr, nullptr); } },
            { "ShardingKeyCreate", [] { return nullptr == OCI_ShardingKeyCreate(nullptr); } },
            { "ShardingKeyFree", [] { return !OCI_ShardingKeyFree(nullptr); } },
            { "ShardingKeyReset", [] { return !OCI_ShardingKeyReset(nullptr); } },
            { "ShardingKeyToText", [] { return nullptr == OCI_ShardingKeyToText(nullptr); } },
        }),
        &NullArgCaseName);

}
