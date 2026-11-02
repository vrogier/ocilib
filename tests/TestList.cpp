#include "ocilib_tests.h"

namespace TestCApi
{
    TEST(TestList, ConnectionLists)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt1 = OCI_StatementCreate(con);
        const auto stmt2 = OCI_StatementCreate(con);
        const auto stmt3 = OCI_StatementCreate(con);

        ASSERT_NE(nullptr, stmt1);
        ASSERT_NE(nullptr, stmt2);
        ASSERT_NE(nullptr, stmt3);

        ASSERT_TRUE(OCI_StatementFree(stmt2));
        ASSERT_TRUE(OCI_StatementFree(stmt1));
        ASSERT_TRUE(OCI_StatementFree(stmt3));

        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestList, TypeInfoCache)
    {
        ExecDML(OTEXT("create table TestListTable (id int)"));

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto ti1 = OCI_TypeInfoGet(con, OTEXT("TestListTable"), OCI_TIF_TABLE);
        ASSERT_NE(nullptr, ti1);

        const auto ti2 = OCI_TypeInfoGet(con, OTEXT("TestListTable"), OCI_TIF_TABLE);
        ASSERT_NE(nullptr, ti2);

        ASSERT_EQ(ti1, ti2);

        ASSERT_TRUE(OCI_TypeInfoFree(ti1));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("drop table TestListTable"));
    }
}
