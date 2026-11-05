#include <atomic>
#include <thread>
#include <vector>

#include "ocilib_tests.h"

const OCI_Size MaxThread = 10;

static std::atomic<OCI_Int32> ConnCreatedCount{ 0 };

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestPoolCpp, GettersAndSettersCpp)
    {
        Environment& env = GetEnvironmentCpp();
        Pool pool(env, DBS, USR, PWD, Pool::SessionPool, 1, 5, 1);

        ASSERT_EQ(0u, pool.GetBusyConnectionsCount());
        ASSERT_GE(pool.GetOpenedConnectionsCount(), 0u);
        ASSERT_EQ(1u, pool.GetMinSize());
        ASSERT_EQ(5u, pool.GetMaxSize());
        ASSERT_EQ(1u, pool.GetIncrement());
        ASSERT_EQ(20u, pool.GetStatementCacheSize());

        pool.SetTimeout(10);
        ASSERT_EQ(10u, pool.GetTimeout());

        pool.SetNoWait(true);
        ASSERT_TRUE(pool.GetNoWait());
        pool.SetNoWait(false);
        ASSERT_FALSE(pool.GetNoWait());

        pool.SetStatementCacheSize(10);
        ASSERT_EQ(10u, pool.GetStatementCacheSize());

        Connection conn = pool.GetConnection();
        ASSERT_FALSE(conn.IsNull());
        ASSERT_EQ(1u, pool.GetBusyConnectionsCount());
        conn.Close();
        ASSERT_EQ(0u, pool.GetBusyConnectionsCount());
    }

    TEST(TestPoolCpp, GetConnectionWithTagCpp)
    {
        Environment& env = GetEnvironmentCpp();
        Pool pool(env, DBS, USR, PWD, Pool::SessionPool, 1, 3, 1);

        Connection conn = pool.GetConnection(OTEXT("mytag"));
        ASSERT_FALSE(conn.IsNull());
        conn.SetSessionTag(OTEXT("mytag"));
        ASSERT_EQ(String(OTEXT("mytag")), conn.GetSessionTag());
        conn.Close();

        Connection conn2 = pool.GetConnection(OTEXT("mytag"));
        ASSERT_FALSE(conn2.IsNull());
        ASSERT_EQ(String(OTEXT("mytag")), conn2.GetSessionTag());
    }

    TEST(TestPoolCpp, OpenCloseCpp)
    {
        Environment& env = GetEnvironmentCpp();
        Pool pool;
        ASSERT_TRUE(pool.IsNull());
        pool.Open(env, DBS, USR, PWD, Pool::SessionPool, 0, 5, 1);
        ASSERT_FALSE(pool.IsNull());
        pool.Close();
        ASSERT_TRUE(pool.IsNull());
    }
}