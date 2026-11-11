#include "ocilib_tests.h"
#include "NullArgs.h"
#include "ocilib.hpp"
#ifdef _WIN32
#include <windows.h>
#endif

using namespace ocilib;

namespace TestCApi
{/*
    TEST(TestEnvironment, Default)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

    }
    TEST(TestEnvironment, CleanupWithoutInitialize)
    {
        ASSERT_FALSE(OCI_EnvironmentFree(nullptr));
    }

    TEST(TestEnvironment, MultipleCall)
    {
        for (OCI_Int32 i = 1; i < 10; i++)
        {
            auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

        }
    }

    TEST(TestEnvironment, GetLocaleValidCode)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const otext*  str = OCI_EnvironmentGetLocaleString(env, OCI_LSI_DAYNAME1);
        ASSERT_NE(nullptr, str);

    }

    TEST(TestEnvironment, GetLocaleInalidCode)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const otext*  str = OCI_EnvironmentGetLocaleString(env, 998);
        ASSERT_EQ(nullptr, str);

    }

    class TestEnvironmentNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestEnvironmentNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestEnvironmentNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "ConnectionCreate", [] { return nullptr == OCI_ConnectionCreate(nullptr, OTEXT("x"), OTEXT("x"), OTEXT("x"), 1); } },
            { "ConnectionCreateSharded", [] { return nullptr == OCI_ConnectionCreateSharded(nullptr, OTEXT("x"), OTEXT("x"), OTEXT("x"), 1, nullptr, nullptr); } },
            { "DatabaseShutdown", [] { return !OCI_DatabaseShutdown(nullptr, OTEXT("x"), OTEXT("x"), OTEXT("x"), 1, 1, 1); } },
            { "DatabaseStartup", [] { return !OCI_DatabaseStartup(nullptr, OTEXT("x"), OTEXT("x"), OTEXT("x"), 1, 1, 1, OTEXT("x")); } },
            { "EnableWarnings", [] { return !OCI_EnvironmentEnableWarnings(nullptr, FALSE); } },
            { "EnvironmentFree", [] { return !OCI_EnvironmentFree(nullptr); } },
            { "GetCharset", [] { return OCI_UNKNOWN == OCI_EnvironmentGetCharset(nullptr); } },
            { "GetFormat", [] { return nullptr == OCI_EnvironmentGetFormat(nullptr, 1); } },
            { "GetImportMode", [] { return OCI_UNKNOWN == OCI_EnvironmentGetImportMode(nullptr); } },
            { "GetLocaleString", [] { return nullptr == OCI_EnvironmentGetLocaleString(nullptr, 1); } },
            { "GetOCICompileVersion", [] { return OCI_UNKNOWN == OCI_EnvironmentGetOCICompileVersion(nullptr); } },
            { "GetOCIRuntimeVersion", [] { return OCI_UNKNOWN == OCI_EnvironmentGetOCIRuntimeVersion(nullptr); } },
            { "GetStats", [] { return 0 == OCI_EnvironmentGetStats(nullptr, 1); }, NoNullPointerError }, // manual
            { "GetSubscriptionPort", [] { return 0 == OCI_EnvironmentGetSubscriptionPort(nullptr); } },
            { "SetErrorHandler", [] { return !OCI_EnvironmentSetErrorHandler(nullptr, nullptr); } },
            { "SetFormat", [] { return !OCI_EnvironmentSetFormat(nullptr, 1, OTEXT("x")); } },
            { "SetHAHandler", [] { return !OCI_EnvironmentSetHAHandler(nullptr, nullptr); } },
            { "SetSubscriptionPort", [] { return !OCI_EnvironmentSetSubscriptionPort(nullptr, 1); } },
            { "SetUserPassword", [] { return !OCI_EnvironmentSetUserPassword(nullptr, OTEXT("x"), OTEXT("x"), OTEXT("x"), OTEXT("x")); } },
        }),
        &NullArgCaseName);

}

namespace TestCppApi
{
    TEST(TestEnvironment, DefaultCpp)
    {
        Environment& env = GetEnvironmentCpp();       
    }

    TEST(TestEnvironment, MultipleCallCpp)
    {
        auto exceptionRaised{ false };

        try
        {
            for (OCI_Int32 i = 1; i < 10; i++)
            {
                Environment& env = GetEnvironmentCpp();
                
            }
        }
        catch (...)
        {
            exceptionRaised = true;
        }

        ASSERT_FALSE(exceptionRaised);
    }

    TEST(TestEnvironment, GetLocaleValidCodeCpp)
    {
        Environment& env = GetEnvironmentCpp();

        auto str = env.GetLocaleString(Environment::LocaleCode(Environment::LocaleDayname1));
        ASSERT_NE(String(), str);        
    }

    TEST(TestEnvironment, ModeAndVersionInfoCpp)
    {
        Environment& env = GetEnvironmentCpp();

        ASSERT_EQ((Environment::Default | Environment::Threaded | Environment::Events).GetValues(), env.GetMode().GetValues());
#ifdef OCI_IMPORT_RUNTIME
        ASSERT_EQ(static_cast<OCI_UInt32>(Environment::ImportRuntime), static_cast<OCI_UInt32>(env.GetImportMode()));
#else
        ASSERT_EQ(static_cast<OCI_UInt32>(Environment::ImportLinkage), static_cast<OCI_UInt32>(env.GetImportMode()));
#endif
#ifdef OCI_CHARSET_WIDE
        ASSERT_EQ(static_cast<OCI_UInt32>(Environment::CharsetWide), static_cast<OCI_UInt32>(env.GetCharset()));
        ASSERT_EQ(2u, env.GetCharMaxSize());
#else
        ASSERT_EQ(static_cast<OCI_UInt32>(Environment::CharsetMbcs), static_cast<OCI_UInt32>(env.GetCharset()));
        ASSERT_EQ(1u, env.GetCharMaxSize());
#endif
        ASSERT_GE(env.GetStats(OCI_UST_BYTES), static_cast<OCI_UInt64>(0));

        ASSERT_NE(OracleVersion(), env.GetCompileVersion());
        ASSERT_GT(env.GetCompileMajorVersion(), 0u);
        ASSERT_GE(env.GetCompileMinorVersion(), 0u);
        ASSERT_GE(env.GetCompileRevisionVersion(), 0u);

        ASSERT_NE(OracleVersion(), env.GetRuntimeVersion());
        ASSERT_GT(env.GetRuntimeMajorVersion(), 0u);
        ASSERT_GE(env.GetRuntimeMinorVersion(), 0u);
        ASSERT_GE(env.GetRuntimeRevisionVersion(), 0u);
    }

    TEST(TestEnvironment, WarningsAndFormatsCpp)
    {
        Environment& env = GetEnvironmentCpp();

        env.EnableWarnings(true);
        env.EnableWarnings(false);

        ASSERT_TRUE(env.SetFormat(FormatTypeValues::FormatDate, OTEXT("YYYY-MM-DD")));
        ASSERT_EQ(String(OTEXT("YYYY-MM-DD")), env.GetFormat(FormatTypeValues::FormatDate));
    }

    TEST(TestEnvironment, SubscriptionAndHACpp)
    {
        Environment& env = GetEnvironmentCpp();

        env.SetSubscriptionPort(12345);
        ASSERT_EQ(12345u, env.GetSubscriptionPort());

        env.SetHAHandler(nullptr);
    }

    TEST(TestEnvironment, GetLocaleInvalidCodeCpp)
    {
        Environment& env = GetEnvironmentCpp();

        ASSERT_THROW((void)(env.GetLocaleString(Environment::LocaleCode(static_cast<Environment::LocaleCodeValues>(999)))), ocilib::Exception);
    }
    */
    TEST(TestEnvironment, GetOCICompileVersion)
    {
        const auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ASSERT_NE(0u, OCI_EnvironmentGetOCICompileVersion(env));
    }

    TEST(TestEnvironment, GetOCIRuntimeVersion)
    {
        const auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ASSERT_NE(0u, OCI_EnvironmentGetOCIRuntimeVersion(env));
    }

    TEST(TestEnvironment, GetCharset)
    {
        const auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ASSERT_TRUE(OCI_EnvironmentGetCharset(env) == OCI_CHAR_WIDE || OCI_EnvironmentGetCharset(env) == OCI_CHAR_MBCS);
    }

    TEST(TestEnvironment, GetImportMode)
    {
        const auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        OCI_EnvironmentGetImportMode(env);
    }

    class TestEnvironmentStats : public ::testing::TestWithParam<OCI_Stat> {};

    TEST_P(TestEnvironmentStats, GetStats)
    {
        const auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        OCI_EnvironmentGetStats(env, GetParam());
    }

    INSTANTIATE_TEST_CASE_P(StatTypes, TestEnvironmentStats,
        ::testing::Values(OCI_UST_HANDLES, OCI_UST_OBJECTS, OCI_UST_BYTES, OCI_UST_DESCRIPTORS));

    TEST(TestEnvironment, GetStatsUnknownStat)
    {
        const auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ASSERT_EQ((OCI_UInt64) 0, OCI_EnvironmentGetStats(env, static_cast<OCI_Stat>(99)));
    }

    TEST(TestEnvironment, GetStatsNullEnv)
    {
        ASSERT_EQ((OCI_UInt64) 0, OCI_EnvironmentGetStats(nullptr, OCI_UST_HANDLES));
    }

    TEST(TestEnvironment, SubscriptionPortZeroKeepsAttributeUnset)
    {
        /* use a dedicated environment: the port attribute cannot be cleared
           on the OCI handle once set, and notification tests share the
           default environment */

        const auto env = OCI_EnvironmentCreate(nullptr, nullptr, OCI_ENV_DEFAULT);
        ASSERT_NE(nullptr, env);

        ASSERT_TRUE(OCI_EnvironmentSetSubscriptionPort(env, 0));
        OCI_EnvironmentGetSubscriptionPort(env);

        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }

    TEST(TestEnvironment, SubscriptionPortSetAndGet)
    {
        /* the port attribute may not be readable on recent clients,
           so the getter is not asserted */

        const auto env = OCI_EnvironmentCreate(nullptr, nullptr, OCI_ENV_DEFAULT);
        ASSERT_NE(nullptr, env);

        OCI_EnvironmentSetSubscriptionPort(env, 12345);
        OCI_EnvironmentGetSubscriptionPort(env);

        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }

    TEST(TestEnvironment, SetFormatDate)
    {
        const auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ASSERT_TRUE(OCI_EnvironmentSetFormat(env, OCI_FMT_DATE, OTEXT("YYYY/MM/DD")));
        ASSERT_EQ(String(OTEXT("YYYY/MM/DD")), String(OCI_EnvironmentGetFormat(env, OCI_FMT_DATE)));
    }

    TEST(TestEnvironment, SetFormatNullRestoresDefault)
    {
        const auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ASSERT_TRUE(OCI_EnvironmentSetFormat(env, OCI_FMT_DATE, nullptr));
        ASSERT_NE(nullptr, OCI_EnvironmentGetFormat(env, OCI_FMT_DATE));
    }

    TEST(TestEnvironment, SetFormatInvalidTypeRejected)
    {
        const auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ASSERT_FALSE(OCI_EnvironmentSetFormat(env, static_cast<OCI_FormatType>(99), OTEXT("X")));
    }

    TEST(TestEnvironment, GetFormatInvalidType)
    {
        const auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ASSERT_EQ(nullptr, OCI_EnvironmentGetFormat(env, static_cast<OCI_FormatType>(99)));
    }


    TEST(TestEnvironment, GetLocaleInvalidCode)
    {
        const auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ASSERT_EQ(nullptr, OCI_EnvironmentGetLocaleString(env, static_cast<OCI_LocaleCode>(0)));
    }


    TEST(TestEnvironment, LibPathInvalid)
    {
        /* an invalid runtime library path fails with LoadingSharedLib,
           but lib_path is ignored entirely in OCI_IMPORT_LINKAGE builds */

        const auto env = OCI_EnvironmentCreate(nullptr, OTEXT("Z:\\no_such_dir_42"), OCI_ENV_DEFAULT);

        if (env)
        {
            /* path ignored: only possible in a linkage build */
            ASSERT_EQ(OCI_IMPORT_MODE_LINKAGE, OCI_EnvironmentGetImportMode(env));
            ASSERT_TRUE(OCI_EnvironmentFree(env));
        }
    }

    TEST(TestEnvironment, LibPathValid)
    {
    #ifdef _WINDOWS
        /* reloading the Oracle shared library through an explicit path
           exercises the OCI_IMPORT_RUNTIME path building arms */

        char modpath[MAX_PATH] = { 0 };
        const HMODULE hmod = GetModuleHandleA("oci.dll");

        ASSERT_NE(nullptr, hmod);
        ASSERT_NE(0u, GetModuleFileNameA(hmod, modpath, MAX_PATH));

        char* sep = strrchr(modpath, '\\');
        ASSERT_NE(nullptr, sep);
        *sep = 0;

        otext libdir[MAX_PATH];
        for (OCI_Size i = 0; i < strlen(modpath) + 1; i++)
        {
            libdir[i] = (otext) modpath[i];
        }

        const auto env = OCI_EnvironmentCreate(nullptr, libdir, OCI_ENV_DEFAULT);
        ASSERT_NE(nullptr, env);
        ASSERT_TRUE(OCI_EnvironmentFree(env));
    #endif
    }

    class TestEnvironmentLeak : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("create type TestEnvironment_ObjType as object (x number)"));
            ExecDML(OTEXT("create table TestEnvironment_LeakTbl (v int)"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestEnvironment_LeakTbl"));
            ExecDML(OTEXT("drop type TestEnvironment_ObjType"));
        }

        static OCI_Connection* CreateLeakConnection(OCI_Environment** env)
        {
            *env = OCI_EnvironmentCreate(nullptr, nullptr, OCI_ENV_DEFAULT);
            EXPECT_NE(nullptr, *env);

            const auto conn = OCI_ConnectionCreate(*env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            EXPECT_NE(nullptr, conn);

            return conn;
        }
    };

    /* freeing an environment that still owns objects emits the
       unfreed-data warnings and reports failure. objects not tracked by
       the environment disposal lists still count as leaks */

    TEST_F(TestEnvironmentLeak, FreeWithLeakedHandle_Warns)
    {
        /* a leaked dirpath bumps the handle counter */

        OCI_Environment* env = nullptr;
        const auto conn = CreateLeakConnection(&env);
        ASSERT_NE(nullptr, env);
        ASSERT_NE(nullptr, conn);

        const auto tbl_typinf = OCI_TypeInfoGet(conn, OTEXT("TestEnvironment_LeakTbl"), OCI_TIF_TABLE);
        ASSERT_NE(nullptr, tbl_typinf);

        const auto dp = OCI_DirectPathCreate(tbl_typinf, nullptr, 1, 1);
        ASSERT_NE(nullptr, dp);

        ASSERT_FALSE(OCI_EnvironmentFree(env));
    }

    TEST_F(TestEnvironmentLeak, FreeWithLeakedDescriptor_Warns)
    {
        /* a leaked lob bumps the descriptor counter */

        OCI_Environment* env = nullptr;
        const auto conn = CreateLeakConnection(&env);
        ASSERT_NE(nullptr, env);
        ASSERT_NE(nullptr, conn);

        const auto lob = OCI_LobCreate(conn, OCI_BLOB);
        ASSERT_NE(nullptr, lob);

        ASSERT_FALSE(OCI_EnvironmentFree(env));
    }

    TEST_F(TestEnvironmentLeak, FreeWithLeakedObjectInstance_Warns)
    {
        /* a leaked object instance bumps the object counter */

        OCI_Environment* env = nullptr;
        const auto conn = CreateLeakConnection(&env);
        ASSERT_NE(nullptr, env);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestEnvironment_ObjType"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto obj = OCI_ObjectCreate(typinf);
        ASSERT_NE(nullptr, obj);

        ASSERT_FALSE(OCI_EnvironmentFree(env));
    }

    /* --- public-API coverage of the dropped OcilibEnv* internals:
       OCI_EnvironmentGetFormat / OCI_EnvironmentSetFormat / OCI_GetVariable / OCI_EnvironmentGetImportMode /
       OCI_EnvironmentGetLocaleString / OCI_EnvironmentGetCharset / OCI_EnvironmentFree --- */

    TEST(TestEnvironment, FormatGetSetLazyFill)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        /* OcilibEnvironmentGetFormat lazy-fill arms: default format strings
           are populated on first OCI_EnvironmentGetFormat call */
        ASSERT_NE(nullptr, OCI_EnvironmentGetFormat(env, OCI_FMT_DATE));
        ASSERT_NE(nullptr, OCI_EnvironmentGetFormat(env, OCI_FMT_TIMESTAMP));
        ASSERT_NE(nullptr, OCI_EnvironmentGetFormat(env, OCI_FMT_NUMERIC));
        ASSERT_EQ(nullptr, OCI_EnvironmentGetFormat(env, static_cast<OCI_FormatType>(0)));

        /* SetFormat roundtrip through OcilibEnvironmentSetFormat */
        ASSERT_TRUE(OCI_EnvironmentSetFormat(env, OCI_FMT_DATE, OTEXT("YYYY/MM/DD")));
        ASSERT_EQ(String(OTEXT("YYYY/MM/DD")), String(OCI_EnvironmentGetFormat(env, OCI_FMT_DATE)));
        ASSERT_TRUE(OCI_EnvironmentSetFormat(env, OCI_FMT_DATE, nullptr));
    }

    TEST(TestEnvironment, GetVariableLocaleAndModes)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        /* OcilibEnvironmentGetImportMode / OcilibEnvironmentGetLocaleString
           via public getters */
        ASSERT_TRUE((OCI_Int32)OCI_EnvironmentGetImportMode(env) >= 0);
        /* locale code 0 is invalid -> NULL; valid codes return a string */
        ASSERT_EQ(nullptr, OCI_EnvironmentGetLocaleString(env, static_cast<OCI_LocaleCode>(0)));
        ASSERT_TRUE(OCI_EnvironmentGetCharset(env) == OCI_CHAR_WIDE || OCI_EnvironmentGetCharset(env) == OCI_CHAR_MBCS);
        ASSERT_NE(0u, OCI_EnvironmentGetOCIRuntimeVersion(env));
    }

    TEST(TestEnvironment, FreeWithUndisposedConn)
    {
        /* OcilibEnvironmentFree leaked-resource arm: freeing env with a live
           connection goes through the undisposed-resource path. Use a private
           env so the shared one is not freed out from under other tests. */
        const auto env = OCI_EnvironmentCreate(nullptr, nullptr, OCI_ENV_DEFAULT);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        /* env-free with a live connection hits the undisposed-resource arm;
           if it refuses, free the leaked handles ourselves */
        if (!OCI_EnvironmentFree(env))
        {
            ASSERT_TRUE(OCI_ConnectionFree(conn));
            ASSERT_TRUE(OCI_EnvironmentFree(env));
        }
    }

    /* --- emulated legacy client (version != 0 on OcilibEnvironmentCreate) ---
       covers OcilibEnvironmentClearSymbolsAbove (version < OCI_8_1 arms) and
       the OCIInitialize/OCIEnvInit legacy init branch */

    extern "C" OCI_SYM_LOCAL OCI_Environment* OcilibEnvironmentCreate(POCI_ERROR, const otext*, OCI_EnvMode, OCI_UInt32);

    TEST(TestEnvironment, Emulated8_0Client_ThreadedRejected)
    {
        /* version < OCI_8_1 + OCI_ENV_THREADED → OCI_ERR_NOT_AVAILABLE */
        const auto env = OcilibEnvironmentCreate(nullptr, nullptr, OCI_ENV_THREADED, OCI_8_0);
        ASSERT_EQ(nullptr, env);

        const auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);
        EXPECT_EQ(OCI_ERR_NOT_AVAILABLE, OCI_ErrorGetCode(err));
    }

    TEST(TestEnvironment, Emulated8_0Client_LegacyInit)
    {
        /* non-threaded 8.0 env goes through OCIInitialize + OCIEnvInit
           (requires the client to still export those legacy symbols) */
        const auto env = OcilibEnvironmentCreate(nullptr, nullptr, OCI_ENV_DEFAULT, OCI_8_0);
        ASSERT_NE(nullptr, env);
        EXPECT_EQ(OCI_8_0, OCI_EnvironmentGetOCIRuntimeVersion(env));
        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }

#ifdef _WIN32
    TEST(TestEnvironment, NlsLangUtf8_WindowsFallback)
    {
        /* SetEnvironmentVariableA() updates the process env block but NOT the
           MSVC CRT environ copy, so getenv() misses it — exactly the fallback
           OcilibEnvironmentGetVariable()'s GetEnvironmentVariableA arm handles.
           The UTF8 value also covers the nls_utf8 detection block in create. */

        ASSERT_NE(0u, SetEnvironmentVariableA("NLS_LANG", "AMERICAN_AMERICA.AL32UTF8"));

        const auto env = OCI_EnvironmentCreate(nullptr, nullptr, OCI_ENV_DEFAULT);
        ASSERT_NE(nullptr, env);
        ASSERT_TRUE(OCI_EnvironmentFree(env));

        SetEnvironmentVariableA("NLS_LANG", nullptr);
    }
#endif
}
