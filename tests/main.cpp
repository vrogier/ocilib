#include "ocilib_tests.h"
#include "TestFeatures.h"
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

OCI_Environment* GetSpecificEnvironment(POCI_ERROR err_handler, OCI_UInt32 version, OCI_EnvMode mode)
{
    return OcilibEnvironmentCreate(err_handler, HOME, mode, version);
}

static TestCaps g_caps;

const TestCaps& GetTestCaps()
{
    return g_caps;
}

void SkipTest(const char* reason)
{
    std::printf("    [SKIPPED] %s\n", reason);
    ::testing::Test::RecordProperty("skipped", reason);
}

/* mirrors the CHECK_FEATURE / CHECK_ENV_FEATURE version matrix
   (src/macros.h) — must stay in sync with it */

struct FeatureGate
{
    int         feature;
    OCI_UInt32  min_ver;
    bool        check_server;
};

static const FeatureGate FeatureGates[] =
{
    { OCI_FEATURE_WIDE_USERDATA,       OCI_9_0,  false },
    { OCI_FEATURE_TIMESTAMP,           OCI_9_0,  false },
    { OCI_FEATURE_DIRPATH_DATE_CACHE,  OCI_9_2,  false },
    { OCI_FEATURE_STATEMENT_CACHING,   OCI_9_2,  false },
    { OCI_FEATURE_SCROLLABLE_CURSOR,   OCI_9_0,  true  },
    { OCI_FEATURE_DATABASE_NOTIFY,     OCI_10_2, false },
    { OCI_FEATURE_REMOTE_DBS_CONTROL,  OCI_10_2, false },
    { OCI_FEATURE_HIGH_AVAILABILITY,   OCI_10_2, false },
    { OCI_FEATURE_EXTENDED_PLSQLTYPES, OCI_12_1, true  },
    { OCI_FEATURE_VECTOR,              OCI_23_4, true  },
    { OCI_FEATURE_SODA,                OCI_18_1, true  },
    { OCI_FEATURE_JSON,                OCI_21_1, true  },
    { OCI_FEATURE_SECURE_NOTIFICATION, OCI_19_4, true  },
    { OCI_FEATURE_SHARDING,            OCI_12_2, false },
    { OCI_FEATURE_SPARSE_VECTOR,       OCI_23_7, true  },
    { OCI_FEATURE_SQL_BOOLEAN,         OCI_23_1, true  },
    { OCI_FEATURE_COLL_KEY,            OCI_23_6, false },
};

bool FeatureAvailable(OCI_Environment* env, OCI_Connection* con, int feature)
{
    for (const auto& gate : FeatureGates)
    {
        if (gate.feature == feature)
        {
            if (!env || OCI_EnvironmentGetOCIRuntimeVersion(env) < gate.min_ver)
            {
                return false;
            }

            /* OCI_ConnectionGetVersion() = min(client, server) — the env
               check above already covered the client side, so this is the
               effective server check */

            if (gate.check_server && (!con || OCI_ConnectionGetVersion(con) < gate.min_ver))
            {
                return false;
            }

            return true;
        }
    }

    return true;
}

bool FeatureAvailable(ocilib::Environment& env, ocilib::Connection* con, int feature)
{
    for (const auto& gate : FeatureGates)
    {
        if (gate.feature == feature)
        {
            if (static_cast<OCI_UInt32>(env.GetRuntimeVersion()) < gate.min_ver)
            {
                return false;
            }

            /* Connection::GetVersion() = min(client, server) */

            if (gate.check_server && (!con || static_cast<OCI_UInt32>(con->GetVersion()) < gate.min_ver))
            {
                return false;
            }

            return true;
        }
    }

    return true;
}

bool FeatureAvailableCaps(int feature)
{
    for (const auto& gate : FeatureGates)
    {
        if (gate.feature == feature)
        {
            if (g_caps.client_ver < gate.min_ver ||
                (gate.check_server && g_caps.server_ver < gate.min_ver))
            {
                return false;
            }

            return true;
        }
    }

    return true;
}

static bool ProbeTestCaps()
{
    auto env = GetSpecificEnvironment(nullptr, OCI_UNKNOWN);

    if (!env)
    {
        std::fprintf(stderr, "error: capability probe failed to create OCI environment\n");
        return false;
    }

    auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);

    if (!con)
    {
        std::fprintf(stderr, "error: capability probe failed to connect to database\n");
        OCI_EnvironmentFree(env);
        return false;
    }

    g_caps.client_ver = OCI_EnvironmentGetOCIRuntimeVersion(env);
    g_caps.server_ver = OCI_VER_MAKE(OCI_ConnectionGetServerMajorVersion(con),
                                     OCI_ConnectionGetServerMinorVersion(con),
                                     OCI_ConnectionGetServerRevisionVersion(con));

    /* instant client 12.2.0.x SEH-faults inside
       OCIDescriptorFree(OCI_DTYPE_SHARDING_KEY) — verified with a raw-OCI
       repro (alloc + free per the documented sequence, no OCILIB calls);
       Oracle client defect. Probing it would crash this process, so gate
       on the version */

    g_caps.shardingkey_ops = (g_caps.client_ver != OCI_12_2);

    OCI_Int32 n = 0;
    g_caps.ctxsys = OCI_ConnectionImmediate(con,
        OTEXT("select count(*) from all_indextypes where owner='CTXSYS' and indextype_name='CONTEXT'"),
        OCI_ARG_INT32, &n) && n > 0;

    std::printf("[ CAPS ] client=%u.%u.%u server=%u.%u.%u ctxsys=%d shardingkey_ops=%d\n",
        OCI_VER_MAJ(g_caps.client_ver), OCI_VER_MIN(g_caps.client_ver), OCI_VER_REV(g_caps.client_ver),
        OCI_VER_MAJ(g_caps.server_ver), OCI_VER_MIN(g_caps.server_ver), OCI_VER_REV(g_caps.server_ver),
        g_caps.ctxsys ? 1 : 0, g_caps.shardingkey_ops ? 1 : 0);

    OCI_ConnectionFree(con);
    OCI_EnvironmentFree(env);
    return true;
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

    if (!ProbeTestCaps())
    {
        return 1;
    }

    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new LeakListener(GetEnvironmentManager()));

    auto res = RUN_ALL_TESTS();

    return res;
}
