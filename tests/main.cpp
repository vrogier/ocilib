#include "ocilib_tests.h"
#include "LeakListener.h"
#include "EnvironmentManager.h"

static EnvironmentManager environmentManager;

/* internal test hook (src/environment.h) — version != 0 makes the env
   emulate an older client so backward-compat paths can be exercised on
   the current client */
extern "C" OCI_SYM_LOCAL OCI_Environment* OcilibEnvironmentCreate(POCI_ERROR, const otext*, OCI_EnvMode, OCI_UInt32);

OCI_Environment* GetEnvironment(POCI_ERROR err_handler)
{
    auto env = environmentManager.GetEnvironmentCApi();
    OCI_EnvironmentSetErrorHandler(env, err_handler);
    return env;
}

ocilib::Environment& GetEnvironmentCpp()
{
    return environmentManager.GetEnvironmentCppApi();
}

OCI_Environment* GetSpecificEnvironment(POCI_ERROR err_handler, OCI_UInt32 version)
{
    return OcilibEnvironmentCreate(err_handler, nullptr, OCI_ENV_DEFAULT, version);
}

void ExecDML(String dml)
{
    const auto env = environmentManager.GetEnvironmentCApi();
    const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
    OCI_ConnectionImmediate(con, dml.data());
    OCI_ConnectionCommit(con);
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
