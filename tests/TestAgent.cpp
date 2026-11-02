#include "ocilib_tests.h"

namespace TestCApi
{
    TEST(TestAgent, CreateAndSet)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto agent = OCI_AgentCreate(conn, OTEXT("TestAgent_Name"), OTEXT("TestAgent_Address"));
        ASSERT_NE(nullptr, agent);

        ASSERT_EQ(ostring(OTEXT("TestAgent_Name")), ostring(OCI_AgentGetName(agent)));
        ASSERT_EQ(ostring(OTEXT("TestAgent_Address")), ostring(OCI_AgentGetAddress(agent)));

        ASSERT_TRUE(OCI_AgentSetName(agent, OTEXT("TestAgent_NewName")));
        ASSERT_TRUE(OCI_AgentSetAddress(agent, OTEXT("TestAgent_NewAddress")));

        ASSERT_EQ(ostring(OTEXT("TestAgent_NewName")), ostring(OCI_AgentGetName(agent)));
        ASSERT_EQ(ostring(OTEXT("TestAgent_NewAddress")), ostring(OCI_AgentGetAddress(agent)));

        ASSERT_TRUE(OCI_AgentFree(agent));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }
}