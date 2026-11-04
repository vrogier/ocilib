#include "ocilib_tests.h"

#include <vector>

using namespace ocilib;

namespace TestCppApi
{
    TEST(TestShardingCpp, KeyLifecycle)
    {
        auto& env = GetEnvironmentCpp();

        ShardingKey skey(env);
        skey.AddColumn(ostring(OTEXT("GOLD")));
        skey.Reset();
        skey.AddColumn(ostring(OTEXT("SILVER")));

        ASSERT_FALSE(skey.ToString().empty());
    }

    TEST(TestShardingCpp, AddColumnAllTypes)
    {
        auto& env = GetEnvironmentCpp();

        ShardingKey skey(env);

        Number num(env, 42);
        Date date = Date::SysDate(env);
        Timestamp tmsp = Timestamp::SysTimestamp(env, Timestamp::NoTimeZone);

        skey.AddColumn(num);
        skey.AddColumn(date);
        skey.AddColumn(tmsp);
        skey.AddColumn(ostring(OTEXT("GOLD")));
        skey.AddColumn(std::vector<unsigned char>{ 0x01, 0x02 });

        ASSERT_FALSE(skey.ToString().empty());
    }

    TEST(TestShardingCpp, ShardedConnection)
    {
        auto& env = GetEnvironmentCpp();

        ShardingKey skey(env);
        skey.AddColumn(ostring(OTEXT("GOLD")));

        ShardingKey sskey(env);
        sskey.AddColumn(ostring(OTEXT("EUROPE")));

        Connection con(env, DBS, USR, PWD, Environment::SessionDefault, skey, sskey);
        ASSERT_TRUE(con.IsServerAlive());

    }

    TEST(TestShardingCpp, ShardedPoolGetConnection)
    {
        auto& env = GetEnvironmentCpp();

        Pool pool(env, DBS, USR, PWD, Pool::SessionPool, 1, 4, 1);

        ShardingKey skey(env);
        skey.AddColumn(ostring(OTEXT("GOLD")));
        ShardingKey sskey(env);
        sskey.AddColumn(ostring(OTEXT("EUROPE")));

        Connection con = pool.GetConnection(OTEXT(""), skey, sskey);
        ASSERT_TRUE(con.IsServerAlive());
    }
}
