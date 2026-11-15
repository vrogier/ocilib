#include "ocilib_tests.h"
#include "TestFeatures.h"
#include "NullArgs.h"

#include <cstring>

using namespace ocilib;

namespace TestCApi
{
    TEST(TestJsonCreate, JsonCreate_ReturnsJson)
    {
        SKIP_IF_FEATURE_UNSUPPORTED(OCI_FEATURE_JSON);
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);
        ASSERT_EQ(conn, OCI_JsonGetConnection(json));
        ASSERT_EQ(env, OCI_HandleGetEnvironment(json));

        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
    TEST(TestJsonCreate, JsonArrayCreate_ReturnsJsonArray)
    {
        SKIP_IF_FEATURE_UNSUPPORTED(OCI_FEATURE_JSON);
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        OCI_Json** jsons = OCI_JsonArrayCreate(conn, ARRAY_SIZE);
        ASSERT_NE(nullptr, jsons);

        for (OCI_Int32 i = 0; i < ARRAY_SIZE; ++i)
        {
            ASSERT_NE(nullptr, jsons[i]);
        }

        ASSERT_TRUE(OCI_JsonArrayFree(jsons));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestJsonCreate, Create_PreClient21_Fails)
    {
        /* JSON text parse client API is 21.1+ */

        const auto env = GetSpecificEnvironment(nullptr, OCI_12_1);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        ASSERT_EQ(nullptr, OCI_JsonCreate(conn));

        const auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);
        EXPECT_EQ(OCI_ERR_NOT_AVAILABLE, OCI_ErrorGetCode(err));

        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }

    /* same contract through the real server-version path — runs only on
       servers older than 21.1 */

    TEST(TestJsonCreate, Create_RealServer_NotAvailable)
    {
        SKIP_IF_FEATURE_SUPPORTED(OCI_FEATURE_JSON);

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        ASSERT_EQ(nullptr, OCI_JsonCreate(conn));

        const auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);
        EXPECT_EQ(OCI_ERR_NOT_AVAILABLE, OCI_ErrorGetCode(err));

        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }


    class TestJsonCreateNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestJsonCreateNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestJsonCreateNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "JsonArrayCreate", [] { return nullptr == OCI_JsonArrayCreate(nullptr, 1); } },
            { "JsonArrayFree", [] { return !OCI_JsonArrayFree(nullptr); } },
            { "JsonClone", [] { return !OCI_JsonClone(nullptr, nullptr); } },
            { "JsonCreate", [] { return nullptr == OCI_JsonCreate(nullptr); } },
            { "JsonFree", [] { return !OCI_JsonFree(nullptr); } },
            { "JsonGetConnection", [] { return nullptr == OCI_JsonGetConnection(nullptr); } },
            { "JsonGetRoot", [] { return nullptr == OCI_JsonGetRoot(nullptr); } },
            { "JsonIsMutable", [] { return !OCI_JsonIsMutable(nullptr); } },
            { "JsonSetMutable", [] { return !OCI_JsonSetMutable(nullptr, FALSE); } },
            { "JsonSetRoot", [] { return !OCI_JsonSetRoot(nullptr, nullptr); } },
        }));

}
