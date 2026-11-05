#include "ocilib_tests.h"
#include "LeakListener.h"
#include "EnvironmentManager.h"

static EnvironmentManager environmentManager;

OCI_Environment* GetEnvironment(POCI_ERROR err_handler)
{
    auto env = environmentManager.GetEnvironmentCApi();
    OCI_SetErrorHandler(env, err_handler);
    return env;
}

ocilib::Environment& GetEnvironmentCpp()
{
    return environmentManager.GetEnvironmentCppApi();
}

void ExecDML(String dml)
{
    const auto env = environmentManager.GetEnvironmentCApi();
    const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
    OCI_Immediate(con, dml.data());
    OCI_Commit(con);
    OCI_ConnectionFree(con);
}

String ToUpper(String str)
{
    std::for_each(std::begin(str), std::end(str), [](otext& c) { c = static_cast<otext>(::towupper(c)); });

    return str;
}

int main(OCI_Int32 argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new LeakListener(environmentManager));

    auto res = RUN_ALL_TESTS();

    return res;
}
