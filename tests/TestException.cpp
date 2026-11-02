#include "ocilib_tests.h"

namespace TestCApi
{
    TEST(TestException, BadConnection)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD_WRONG, OCI_SESSION_DEFAULT);
        ASSERT_EQ(nullptr, conn);

        const auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);

        ASSERT_NE(nullptr, OCI_ErrorGetString(err));
        ASSERT_NE(0u, (OCI_UInt32) OCI_ErrorGetType(err));
        ASSERT_NE(0, OCI_ErrorGetOCICode(err));

    }

    TEST(TestException, SqlError)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_FALSE(OCI_ExecuteStmt(stmt, OTEXT("select * from non_existent_table_for_exception_test")));

        const auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);

        ASSERT_NE(nullptr, OCI_ErrorGetString(err));
        ASSERT_EQ(conn, OCI_ErrorGetConnection(err));
        ASSERT_EQ(env, OCI_HandleGetEnvironment(err));
        ASSERT_EQ(stmt, OCI_ErrorGetStatement(err));
        ASSERT_NE(nullptr, OCI_ErrorGetLocation(err));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }
}