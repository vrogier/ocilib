#include "ocilib_tests.h"

namespace TestCApi
{
    TEST(TestHandle, HandleGetEnvironment)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_EQ(env, OCI_HandleGetEnvironment(env));
        ASSERT_EQ(env, OCI_HandleGetEnvironment(con));
        ASSERT_EQ(env, OCI_HandleGetEnvironment(stmt));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestHandle, UserData)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        OCI_Int32 value = 0x12345678;
        void* data = &value;

        ASSERT_TRUE(OCI_SetUserData(con, data));
        ASSERT_EQ(data, OCI_GetUserData(con));

        ASSERT_TRUE(OCI_ConnectionFree(con));

    }
}
