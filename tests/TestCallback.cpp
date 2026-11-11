#include "ocilib_tests.h"

namespace TestCApi
{
    static bool s_errorHandlerCalled = false;

    static void ErrorHandler(OCI_Error* err)
    {
        s_errorHandlerCalled = true;
    }

    static OCI_UInt32 TAFHandler(OCI_Connection* con, OCI_UInt32 type, OCI_UInt32 event)
    {
        return OCI_FOC_OK;
    }

    static void HAHandler(OCI_Connection* con, OCI_UInt32 source, OCI_UInt32 event, OCI_Timestamp* time)
    {
    }

    TEST(TestCallback, SetAndTriggerErrorHandler)
    {
        s_errorHandlerCalled = false;

        auto env = GetEnvironment(ErrorHandler);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_EnvironmentSetErrorHandler(env, ErrorHandler));
        ASSERT_FALSE(OCI_StatementExecute(stmt, OTEXT("select * from TestCallback_NoSuchTable")));
        ASSERT_TRUE(s_errorHandlerCalled);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestCallback, SetTAFHandlerOnCapableConnection)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        if (OCI_ConnectionIsTAFCapable(conn))
        {
            EXPECT_TRUE(OCI_ConnectionSetTAFHandler(conn, TAFHandler));
        }

        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestCallback, SetHAHandlerOnEnvironment)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ASSERT_TRUE(OCI_EnvironmentSetHAHandler(env, HAHandler));
    }
}
