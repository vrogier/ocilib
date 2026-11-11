#include "ocilib_tests.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>

namespace TestCApi
{
    static void SetLogEnv(const char* level, const char* categories, const char* output)
    {
    #ifdef _WINDOWS
        _putenv(level      ? "OCILIB_LOG=" : "OCILIB_LOG=");
        _putenv(categories ? "OCILIB_LOG_CATEGORIES=" : "OCILIB_LOG_CATEGORIES=");
        _putenv(output     ? "OCILIB_LOG_OUTPUT=" : "OCILIB_LOG_OUTPUT=");

        if (level)      _putenv((std::string("OCILIB_LOG=") + level).c_str());
        if (categories) _putenv((std::string("OCILIB_LOG_CATEGORIES=") + categories).c_str());
        if (output)     _putenv((std::string("OCILIB_LOG_OUTPUT=") + output).c_str());
    #else
        if (level)      setenv("OCILIB_LOG", level, 1);           else unsetenv("OCILIB_LOG");
        if (categories) setenv("OCILIB_LOG_CATEGORIES", categories, 1); else unsetenv("OCILIB_LOG_CATEGORIES");
        if (output)     setenv("OCILIB_LOG_OUTPUT", output, 1);   else unsetenv("OCILIB_LOG_OUTPUT");
    #endif
    }

    /* OCILIB_LOG* vars are parsed inside OCI_EnvironmentCreate() via
       OcilibLogInitialize(); freeing the env drains the logger through
       OcilibLogReset() -> OcilibLogCleanup(). */

    static void ReinitLogger(const char* level, const char* categories, const char* output)
    {
        SetLogEnv(level, categories, output);
        OCI_Environment* env = OCI_EnvironmentCreate(nullptr, HOME, OCI_ENV_DEFAULT | OCI_ENV_THREADED | OCI_ENV_EVENTS);
        ASSERT_NE(nullptr, env);
        OCI_EnvironmentFree(env);
    }

    static void ShutdownLogger()
    {
        SetLogEnv(nullptr, nullptr, nullptr);
    }

    /* --- level / env-var parsing --- */

    TEST(TestLog, DisabledWithoutEnvVar)
    {
        /* no OCILIB_LOG variable -> logging stays disabled */

        ReinitLogger(nullptr, nullptr, nullptr);
        ShutdownLogger();
    }

    TEST(TestLog, InvalidLevelName)
    {
        /* invalid level name -> treated like disabled */

        ReinitLogger("BOGUS", nullptr, nullptr);
        ShutdownLogger();
    }

    TEST(TestLog, ValidLevelEnablesLogging)
    {
        ReinitLogger("DEBUG", nullptr, "STDERR");
        ShutdownLogger();
    }

    /* --- output routing (OcilibLogOpenFile / env-var parsing) --- */

    TEST(TestLog, OutputToFile)
    {
        /* file output exercises OcilibLogOpenFile(); env cleanup closes
           the owned file in OcilibLogCleanup() */

        ReinitLogger("DEBUG", nullptr, "ocilib_test_log.txt");
        std::remove("ocilib_test_log.txt");

        ShutdownLogger();
    }

    TEST(TestLog, OutputToStdout)
    {
        ReinitLogger("DEBUG", "OCI", "STDOUT");
        ShutdownLogger();
    }

    TEST(TestLog, OutputToStderr)
    {
        ReinitLogger("DEBUG", nullptr, "STDERR");
        ShutdownLogger();
    }

    /* --- public-API coverage of the dropped OcilibLog* internals:
       OCI_Trace / OCI_GetLastError / OCILIB_LOG* env vars drive
       OcilibLogWrite / OcilibLogOpenFile /
       OcilibLogLevelName / OcilibLogCategoryName
       through the public surface --- */

    TEST(TestLog, TraceWritesToLog)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ReinitLogger("DEBUG", nullptr, "STDERR");

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        /* OCI_Trace -> OcilibLogWrite / OcilibLogLevelName /
           OcilibLogCategoryName / OcilibLogNativeToUTF8 arms */
        ASSERT_TRUE(OCI_ConnectionSetTrace(conn, OCI_TRC_IDENTITY, OTEXT("TestLogIdentity")));
        ASSERT_TRUE(OCI_ConnectionSetTrace(conn, OCI_TRC_MODULE, OTEXT("TestLogModule")));
        ASSERT_TRUE(OCI_ConnectionSetTrace(conn, OCI_TRC_ACTION, OTEXT("TestLogAction")));
        ASSERT_TRUE(OCI_ConnectionSetTrace(conn, OCI_TRC_DETAIL, OTEXT("TestLogDetail")));
        ASSERT_TRUE(OCI_ConnectionSetTrace(conn, OCI_TRC_OPERATION, OTEXT("TestLogOp")));

        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ShutdownLogger();
    }

    TEST(TestLog, ErrorWritesToLog)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ReinitLogger("DEBUG", nullptr, "STDERR");

        /* an error + OCI_GetLastError -> OcilibLogWrite arms */
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD_WRONG, OCI_SESSION_DEFAULT);
        ASSERT_EQ(nullptr, conn);
        ASSERT_NE(nullptr, OCI_GetLastError());

        ShutdownLogger();
    }
}
