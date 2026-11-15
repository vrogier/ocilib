#include "ocilib_tests.h"
#include "LeakListener.h"
#include "EnvironmentManager.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cwchar>

/* Test configuration backing storage — populated by LoadTestConfig() */

static String g_dbs, g_usr, g_pwd, g_sys_usr, g_sys_pwd, g_home, g_subscr_addr;

const otext* DBS         = nullptr;
const otext* USR         = nullptr;
const otext* PWD         = nullptr;
const otext* SYS_USR     = nullptr;
const otext* SYS_PWD     = nullptr;
const otext* HOME        = nullptr;
const otext* SUBSCR_ADDR = nullptr;

/* the manager owns the shared environments; it must be constructed lazily,
   after LoadTestConfig() has populated HOME */
static EnvironmentManager& GetEnvironmentManager()
{
    static EnvironmentManager environmentManager;
    return environmentManager;
}

/* internal test hook (src/environment.h) — version != 0 makes the env
   emulate an older client so backward-compat paths can be exercised on
   the current client */
extern "C" OCI_SYM_LOCAL OCI_Environment* OcilibEnvironmentCreate(POCI_ERROR, const otext*, OCI_EnvMode, OCI_UInt32);

static String ToOtext(const char* str)
{
#ifdef OCI_CHARSET_WIDE

    String result;
    result.resize(std::strlen(str));

    mbstate_t mbs{};
    const char* ptr = str;

#if defined(_MSC_VER)
    __pragma(warning(disable : 4996))
#endif

    const size_t length = std::mbsrtowcs(&result[0], &ptr, result.size(), &mbs);

#if defined(_MSC_VER)
    __pragma(warning(default: 4996))
#endif

    result.resize((static_cast<size_t>(-1) == length) ? 0 : length);

    return result;

#else

    return String(str);

#endif
}

static bool LoadTestConfig()
{
    struct TestVar
    {
        const char* envName;
        String*     storage;
        const otext** target;
        bool        allowEmpty;
    };

    const TestVar vars[] =
    {
        { "OCILIB_TEST_DB",      &g_dbs,     &DBS,     false },
        { "OCILIB_TEST_USR",     &g_usr,     &USR,     false },
        { "OCILIB_TEST_PWD",     &g_pwd,     &PWD,     false },
        { "OCILIB_TEST_SYS_USR", &g_sys_usr, &SYS_USR, false },
        { "OCILIB_TEST_SYS_PWD", &g_sys_pwd, &SYS_PWD, false },
        { "OCILIB_TEST_HOME",    &g_home,    &HOME,    true  },
        { "OCILIB_TEST_SUBSCR_ADDR", &g_subscr_addr, &SUBSCR_ADDR, true  },
    };

    bool ok = true;

    for (const auto& var : vars)
    {
        const char* value = std::getenv(var.envName);

        if (nullptr == value || 0 == *value)
        {
            if (!var.allowEmpty)
            {
                std::fprintf(stderr, "error: required environment variable %s is not set\n", var.envName);
                ok = false;
            }
            continue;
        }

        *var.storage = ToOtext(value);
        *var.target  = var.storage->data();
    }

    return ok;
}

OCI_Environment* GetEnvironment(POCI_ERROR err_handler)
{
    auto env = GetEnvironmentManager().GetEnvironmentCApi();
    OCI_EnvironmentSetErrorHandler(env, err_handler);
    return env;
}

ocilib::Environment& GetEnvironmentCpp()
{
    return GetEnvironmentManager().GetEnvironmentCppApi();
}

OCI_Environment* GetSpecificEnvironment(POCI_ERROR err_handler, OCI_UInt32 version)
{
    return OcilibEnvironmentCreate(err_handler, HOME, OCI_ENV_DEFAULT, version);
}

void ExecDML(String dml)
{
    const auto env = GetEnvironmentManager().GetEnvironmentCApi();
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

    /* test discovery must work without the test environment variables
       (VS Test Explorer runs the binary with --gtest_list_tests)      */

    if (::testing::GTEST_FLAG(list_tests))
    {
        return RUN_ALL_TESTS();
    }

    if (!LoadTestConfig())
    {
        return 1;
    }

    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new LeakListener(GetEnvironmentManager()));

    auto res = RUN_ALL_TESTS();

    return res;
}
