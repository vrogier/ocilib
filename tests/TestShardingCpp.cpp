#include "ocilib_tests.h"
#include "TestFeatures.h"

#include <vector>

using namespace ocilib;

namespace TestCppApi
{
    TEST(TestShardingCpp, ShardingKeyCreateSetColumnAndFree_Works)
    {
        SKIP_UNLESS(GetTestCaps().shardingkey_ops,
                    "Oracle client 12.2.0.x crashes in OCIDescriptorFree(OCI_DTYPE_SHARDING_KEY) (Oracle defect)");

        auto& env = GetEnvironmentCpp();

        SKIP_IF_FEATURE_UNAVAILABLE(env, nullptr, OCI_FEATURE_SHARDING);
        ShardingKey skey(env);
        skey.AddColumn(String(OTEXT("GOLD")));
        skey.Reset();
        skey.AddColumn(String(OTEXT("SILVER")));

        ASSERT_FALSE(skey.ToString().empty());
    }

    TEST(TestShardingCpp, AddColumnsOfAllSupportedTypes_StoresColumns)
    {
        SKIP_UNLESS(GetTestCaps().shardingkey_ops,
                    "Oracle client 12.2.0.x crashes in OCIDescriptorFree(OCI_DTYPE_SHARDING_KEY) (Oracle defect)");

        auto& env = GetEnvironmentCpp();

        SKIP_IF_FEATURE_UNAVAILABLE(env, nullptr, OCI_FEATURE_SHARDING);
        ShardingKey skey(env);

        Number num(env, 42);
        Date date = Date::SysDate(env);
        Timestamp tmsp = Timestamp::SysTimestamp(env, Timestamp::NoTimeZone);

        skey.AddColumn(num);
        skey.AddColumn(date);
        skey.AddColumn(tmsp);
        skey.AddColumn(String(OTEXT("GOLD")));
        skey.AddColumn(std::vector<unsigned char>{ 0x01, 0x02 });

        ASSERT_FALSE(skey.ToString().empty());
    }

    TEST(TestShardingCpp, ConnectionCreateShardedWithKey_Connects)
    {
        SKIP_UNLESS(GetTestCaps().shardingkey_ops,
                    "Oracle client 12.2.0.x crashes in OCIDescriptorFree(OCI_DTYPE_SHARDING_KEY) (Oracle defect)");

        auto& env = GetEnvironmentCpp();

        SKIP_IF_FEATURE_UNAVAILABLE(env, nullptr, OCI_FEATURE_SHARDING);
        ShardingKey skey(env);
        skey.AddColumn(String(OTEXT("GOLD")));

        ShardingKey sskey(env);
        sskey.AddColumn(String(OTEXT("EUROPE")));

        Connection con(env, DBS, USR, PWD, Environment::SessionDefault, skey, sskey);
        ASSERT_TRUE(con.IsServerAlive());

    }

    TEST(TestShardingCpp, PoolGetShardedConnection_ReturnsConnection)
    {
        SKIP_UNLESS(GetTestCaps().shardingkey_ops,
                    "Oracle client 12.2.0.x crashes in OCIDescriptorFree(OCI_DTYPE_SHARDING_KEY) (Oracle defect)");

        auto& env = GetEnvironmentCpp();

        SKIP_IF_FEATURE_UNAVAILABLE(env, nullptr, OCI_FEATURE_SHARDING);
        Pool pool(env, DBS, USR, PWD, Pool::SessionPool, 1, 4, 1);

        ShardingKey skey(env);
        skey.AddColumn(String(OTEXT("GOLD")));
        ShardingKey sskey(env);
        sskey.AddColumn(String(OTEXT("EUROPE")));

        Connection con = pool.GetConnection(OTEXT(""), skey, sskey);
        ASSERT_TRUE(con.IsServerAlive());
    }

    /* SHARDING requires a 12.2+ client — verify the exception contract
       on an unsupported client (env-only gate, always skips when the
       client supports sharding) */

    TEST(TestShardingCpp, Create_NotAvailable)
    {
        Environment& env = GetEnvironmentCpp();

        SKIP_IF_FEATURE_AVAILABLE(env, nullptr, OCI_FEATURE_SHARDING);

        try
        {
            ShardingKey skey(env);
            FAIL() << "expected ocilib::Exception";
        }
        catch (const Exception& ex)
        {
            EXPECT_EQ(Exception::OcilibError, ex.GetCategory());
            EXPECT_EQ(static_cast<UInt32>(OCI_ERR_NOT_AVAILABLE), ex.GetCode());
        }
    }

}
