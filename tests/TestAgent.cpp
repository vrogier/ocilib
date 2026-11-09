#include "ocilib_tests.h"
#include "NullArgs.h"

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

        ASSERT_EQ(String(OTEXT("TestAgent_Name")), String(OCI_AgentGetName(agent)));
        ASSERT_EQ(String(OTEXT("TestAgent_Address")), String(OCI_AgentGetAddress(agent)));

        ASSERT_TRUE(OCI_AgentSetName(agent, OTEXT("TestAgent_NewName")));
        ASSERT_TRUE(OCI_AgentSetAddress(agent, OTEXT("TestAgent_NewAddress")));

        ASSERT_EQ(String(OTEXT("TestAgent_NewName")), String(OCI_AgentGetName(agent)));
        ASSERT_EQ(String(OTEXT("TestAgent_NewAddress")), String(OCI_AgentGetAddress(agent)));

        ASSERT_TRUE(OCI_AgentFree(agent));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    class TestAgentNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestAgentNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestAgentNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "AgentCreate", [] { return nullptr == OCI_AgentCreate(nullptr, OTEXT("x"), OTEXT("x")); } },
            { "AgentFree", [] { return !OCI_AgentFree(nullptr); } },
            { "AgentGetAddress", [] { return nullptr == OCI_AgentGetAddress(nullptr); } },
            { "AgentGetName", [] { return nullptr == OCI_AgentGetName(nullptr); } },
            { "AgentSetAddress", [] { return !OCI_AgentSetAddress(nullptr, OTEXT("x")); } },
            { "AgentSetName", [] { return !OCI_AgentSetName(nullptr, OTEXT("x")); } },
        }),
        &NullArgCaseName);

}