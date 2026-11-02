#include "ocilib_tests.h"

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestAgentCpp, CreateAndGettersSettersCpp)
    {
        Environment& env = GetEnvironmentCpp();
        Connection conn(env, DBS, USR, PWD);

        Agent agent(conn, OTEXT("TestAgent_Name"), OTEXT("TestAgent_Address"));
        ASSERT_EQ(ostring(OTEXT("TestAgent_Name")), agent.GetName());
        ASSERT_EQ(ostring(OTEXT("TestAgent_Address")), agent.GetAddress());

        agent.SetName(OTEXT("TestAgent_NewName"));
        agent.SetAddress(OTEXT("TestAgent_NewAddress"));

        ASSERT_EQ(ostring(OTEXT("TestAgent_NewName")), agent.GetName());
        ASSERT_EQ(ostring(OTEXT("TestAgent_NewAddress")), agent.GetAddress());
    }
}
