#include "ocilib_tests.h"

#include <cstring>

using namespace ocilib;

namespace TestCApi
{
    TEST(TestJsonCreate, CreateAndFree)
    {
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
    TEST(TestJsonCreate, ArrayCreateAndFree)
    {
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
}
