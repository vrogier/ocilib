#include "ocilib_tests.h"
#include "NullArgs.h"

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

    /* The NULL-constructor and NULL-source mega-tests that previously lived
       here (NullEnvironmentConstructors / NullParentConstructors /
       NullSourceCalls) are superseded by the per-type <Suite>NullArgs
       parameterized suites generated into each Test*.cpp file - every call
       they exercised is now a named, individually reported case. */

    class TestHandleNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestHandleNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestHandleNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "GetUserData", [] { return nullptr == OCI_GetUserData(nullptr); }, NoNullPointerError }, // handle.c
            { "HandleGetEnvironment", [] { return nullptr == OCI_HandleGetEnvironment(nullptr); }, NoNullPointerError }, // manual
            { "SetUserData", [] { return !OCI_SetUserData(nullptr, NullArgBuffer()); }, NoNullPointerError }, // handle.c
        }),
        &NullArgCaseName);

}
