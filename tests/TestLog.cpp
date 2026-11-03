#include "ocilib_tests.h"
#include "test_internals.h"

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

    /* Drains the logger reference count, applies the env vars, and re-parses
       them through OcilibLogInitialize(). */

    static void ReinitLogger(const char* level, const char* categories, const char* output)
    {
        OCI_TestLogReset();
        SetLogEnv(level, categories, output);
        ASSERT_TRUE(OCI_TestLogInit());
    }

    static void ShutdownLogger()
    {
        OCI_TestLogSetHandler(nullptr);
        ReinitLogger(nullptr, nullptr, nullptr);
    }

    /* --- level parsing --- */

    TEST(TestLog, DisabledWithoutEnvVar)
    {
        /* no OCILIB_LOG variable -> early return, logging stays disabled */

        ReinitLogger(nullptr, nullptr, nullptr);
        ASSERT_FALSE(OCI_TestLogIsEnabled(OCI_LOG_CAT_ALL, OCI_LOG_DEBUG));

        ShutdownLogger();
    }

    TEST(TestLog, InvalidLevelName)
    {
        /* invalid level name -> treated like disabled */

        ReinitLogger("BOGUS", nullptr, nullptr);
        ASSERT_FALSE(OCI_TestLogIsEnabled(OCI_LOG_CAT_ALL, OCI_LOG_ERROR));

        ShutdownLogger();
    }

    struct LogLevelCase
    {
        const char*    name;
        OCI_LogLevel   level;
    };

    class TestLogLevel : public ::testing::TestWithParam<LogLevelCase>
    {
    protected:

        void SetUp() override
        {
            ReinitLogger(GetParam().name, nullptr, "STDERR");
        }

        void TearDown() override
        {
            ShutdownLogger();
        }
    };

    TEST_P(TestLogLevel, GatesEnabledAndWrite)
    {
        /* each level name exercises a comparison arm and gates writes */

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ASSERT_TRUE(OCI_TestLogIsEnabled(OCI_LOG_CAT_LIB, GetParam().level));
        OCI_TestLogWrite(env, OCI_LOG_CAT_LIB, GetParam().level, "TestLogLevel", OTEXT("level test"));
    }

    INSTANTIATE_TEST_CASE_P(Levels, TestLogLevel,
        ::testing::Values(
            LogLevelCase{ "ERROR", OCI_LOG_ERROR },
            LogLevelCase{ "WARN",  OCI_LOG_WARN },
            LogLevelCase{ "INFO",  OCI_LOG_INFO },
            LogLevelCase{ "TRACE", OCI_LOG_TRACE },
            LogLevelCase{ "DEBUG", OCI_LOG_DEBUG }));

    /* --- category parsing --- */

    struct LogCategoryCase
    {
        const char* categories;
        boolean     libEnabled;
        boolean     ociEnabled;
    };

    class TestLogCategories : public ::testing::TestWithParam<LogCategoryCase>
    {
    protected:

        void SetUp() override
        {
            ReinitLogger("DEBUG", GetParam().categories, "STDERR");
        }

        void TearDown() override
        {
            ShutdownLogger();
        }
    };

    TEST_P(TestLogCategories, GatesEnabled)
    {
        ASSERT_EQ(GetParam().libEnabled != FALSE, OCI_TestLogIsEnabled(OCI_LOG_CAT_LIB, OCI_LOG_DEBUG));
        ASSERT_EQ(GetParam().ociEnabled != FALSE, OCI_TestLogIsEnabled(OCI_LOG_CAT_OCI, OCI_LOG_DEBUG));
    }

    INSTANTIATE_TEST_CASE_P(Categories, TestLogCategories,
        ::testing::Values(
            LogCategoryCase{ "LIB", TRUE,  FALSE },
            LogCategoryCase{ "OCI", FALSE, TRUE  }));

    /* --- output parsing --- */

    TEST(TestLog, OutputDefaultsToStderr)
    {
        /* no output variable -> defaults to stderr */

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ReinitLogger("DEBUG", nullptr, nullptr);
        OCI_TestLogWrite(env, OCI_LOG_CAT_ALL, OCI_LOG_ERROR, "TestLog", OTEXT("default output"));

        ShutdownLogger();
    }

    TEST(TestLog, OutputToFile)
    {
        /* file output exercises OcilibLogOpenFile(); the reset
           closes the owned file in OcilibLogCleanup() */

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ReinitLogger("DEBUG", nullptr, "ocilib_test_log.txt");
        OCI_TestLogWrite(env, OCI_LOG_CAT_LIB, OCI_LOG_DEBUG, "TestLog", OTEXT("file output"));
        OCI_TestLogReset();

        std::remove("ocilib_test_log.txt");

        ShutdownLogger();
    }

    TEST(TestLog, OutputToStdout)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ReinitLogger("DEBUG", "OCI", "STDOUT");
        OCI_TestLogWrite(env, OCI_LOG_CAT_OCI, OCI_LOG_WARN, "TestLog", OTEXT("stdout output"));

        ShutdownLogger();
    }

    /* --- message formatting: the file-backed fixture reinitializes the
       logger once per test --- */

    class TestLogWrite : public ::testing::Test
    {
    protected:

        void SetUp() override
        {
            env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            ReinitLogger("DEBUG", nullptr, "ocilib_test_log.txt");
        }

        void TearDown() override
        {
            OCI_TestLogReset();
            std::remove("ocilib_test_log.txt");
            ShutdownLogger();
        }

        OCI_Environment* env{ nullptr };
    };

    TEST_F(TestLogWrite, WritesEveryLevelName)
    {
        for (OCI_UInt32 lvl = OCI_LOG_ERROR; lvl <= OCI_LOG_DEBUG; lvl++)
        {
            OCI_TestLogWrite(env, OCI_LOG_CAT_LIB, lvl, "TestLogWrite", OTEXT("level name"));
        }
    }

    TEST_F(TestLogWrite, WritesLibCategoryName)
    {
        OCI_TestLogWrite(env, OCI_LOG_CAT_LIB, OCI_LOG_INFO, "TestLogWrite", OTEXT("cat lib"));
    }

    TEST_F(TestLogWrite, WritesOciCategoryName)
    {
        OCI_TestLogWrite(env, OCI_LOG_CAT_OCI, OCI_LOG_INFO, "TestLogWrite", OTEXT("cat oci"));
    }

    TEST_F(TestLogWrite, WritesAllCategoryName)
    {
        OCI_TestLogWrite(env, OCI_LOG_CAT_ALL, OCI_LOG_INFO, "TestLogWrite", OTEXT("cat all"));
    }

    TEST_F(TestLogWrite, WritesNullFunctionName)
    {
        /* NULL function name arm */

        OCI_TestLogWrite(env, OCI_LOG_CAT_LIB, OCI_LOG_INFO, nullptr, OTEXT("no function"));
    }

    TEST_F(TestLogWrite, WritesNonAsciiContent)
    {
        /* non ASCII content exercises the native -> UTF-8 conversion,
           including a surrogate pair (non BMP code point).
           In ANSI builds otext is char based: only single byte escapes
           are allowed and the conversion is a plain copy. */

#ifdef OCI_CHARSET_ANSI

        OCI_TestLogWrite(env, OCI_LOG_CAT_LIB, OCI_LOG_INFO, "TestLogWrite",
                         OTEXT("accent \xe9\xe8 \xa7\xa5"));

#else

        OCI_TestLogWrite(env, OCI_LOG_CAT_LIB, OCI_LOG_INFO, "TestLogWrite",
                         OTEXT("accent e\x0301 \x00e9\x00e8 \x4e2d\x6587 \xd83d\xde00"));

#endif
    }

    TEST_F(TestLogWrite, WritesTruncatedMessage)
    {
        /* truncated message -> vsnprintf failure arm */

        otext huge[5000];
        for (OCI_Int32 i = 0; i < 4999; i++) { huge[i] = OTEXT('x'); }
        huge[4999] = 0;

        OCI_TestLogWrite(env, OCI_LOG_CAT_LIB, OCI_LOG_INFO, "TestLogWrite", huge);
    }

    TEST_F(TestLogWrite, InvokesHandler)
    {
        /* user handler invoked outside the logger lock */

        static OCI_Int32 handler_calls = 0;
        handler_calls = 0;

        OCI_TestLogSetHandler([](OCI_Environment*, OCI_LogLevel, OCI_LogCategory, const char*) { handler_calls++; });
        OCI_TestLogWrite(env, OCI_LOG_CAT_LIB, OCI_LOG_ERROR, "TestLogWrite", OTEXT("handler test"));
        OCI_TestLogSetHandler(nullptr);

        ASSERT_EQ(1, handler_calls);
    }

    /* --- OcilibLogCodepointToUTF8() arms, including undersized buffers --- */

    struct LogCodepointCase
    {
        OCI_UInt32 codepoint;
        OCI_Int32          bufferSize;
        OCI_Int32          expected;
    };

    class TestLogCodepoint : public ::testing::TestWithParam<LogCodepointCase> {};

    TEST_P(TestLogCodepoint, Converts)
    {
        char dst[8];
        ASSERT_EQ(GetParam().expected, OCI_TestLogCodepointToUTF8(GetParam().codepoint, dst, GetParam().bufferSize));
    }

    INSTANTIATE_TEST_CASE_P(Ranges, TestLogCodepoint,
        ::testing::Values(
            LogCodepointCase{ 0x41,     8, 1 },
            LogCodepointCase{ 0x7FF,    8, 2 },
            LogCodepointCase{ 0xFFFF,   8, 3 },
            LogCodepointCase{ 0x10FFFF, 8, 4 },
            LogCodepointCase{ 0x110000, 8, 0 },
            LogCodepointCase{ 0x41,     0, 0 },
            LogCodepointCase{ 0x7FF,    1, 0 },
            LogCodepointCase{ 0xFFFF,   2, 0 },
            LogCodepointCase{ 0x10FFFF, 3, 0 }));

    /* --- OcilibLogNativeToUTF8() edges --- */

    TEST(TestLog, NativeToUTF8NullSource)
    {
        char dst[8];
        ASSERT_EQ(0, OCI_TestLogNativeToUTF8(nullptr, dst, (OCI_Int32) sizeof(dst)));
    }

    TEST(TestLog, NativeToUTF8NullDest)
    {
        ASSERT_EQ(0, OCI_TestLogNativeToUTF8(OTEXT("abc"), nullptr, 8));
    }

    TEST(TestLog, NativeToUTF8ZeroSize)
    {
        char dst[8];
        ASSERT_EQ(0, OCI_TestLogNativeToUTF8(OTEXT("abc"), dst, 0));
    }

    TEST(TestLog, NativeToUTF8UndersizedBuffer)
    {
        char dst[8];
        ASSERT_EQ(0, OCI_TestLogNativeToUTF8(OTEXT("abc"), dst, 2));
    }

    TEST(TestLog, NativeToUTF8Roundtrip)
    {
        char dst[8];
        ASSERT_EQ(2, OCI_TestLogNativeToUTF8(OTEXT("ok"), dst, (OCI_Int32) sizeof(dst)));
        ASSERT_EQ(0, std::strcmp(dst, "ok"));
    }

    /* --- OcilibLogStrEquals() edges --- */

    struct LogStrEqualsCase
    {
        const char* left;
        const char* right;
        boolean     expected;
    };

    class TestLogStrEquals : public ::testing::TestWithParam<LogStrEqualsCase> {};

    TEST_P(TestLogStrEquals, Compares)
    {
        ASSERT_EQ(GetParam().expected != FALSE, OCI_TestLogStrEquals(GetParam().left, GetParam().right));
    }

    INSTANTIATE_TEST_CASE_P(Pairs, TestLogStrEquals,
        ::testing::Values(
            LogStrEqualsCase{ nullptr,   "DEBUG", FALSE },
            LogStrEqualsCase{ "DEBUG",   nullptr, FALSE },
            LogStrEqualsCase{ "debug",   "DEBUG", TRUE  },
            LogStrEqualsCase{ "DEB",     "DEBUG", FALSE },
            LogStrEqualsCase{ "DEBUGGER", "DEBUG", FALSE }));

    /* --- name lookups, including the fallback arms --- */

    struct LogLevelNameCase
    {
        OCI_UInt32 level;
        const char*  name;
    };

    class TestLogLevelName : public ::testing::TestWithParam<LogLevelNameCase> {};

    TEST_P(TestLogLevelName, Resolves)
    {
        ASSERT_EQ(0, std::strcmp(OCI_TestLogLevelName(GetParam().level), GetParam().name));
    }

    INSTANTIATE_TEST_CASE_P(Levels, TestLogLevelName,
        ::testing::Values(
            LogLevelNameCase{ OCI_LOG_NONE,  "NONE" },
            LogLevelNameCase{ OCI_LOG_ERROR, "ERROR" },
            LogLevelNameCase{ OCI_LOG_WARN,  "WARN" },
            LogLevelNameCase{ OCI_LOG_INFO,  "INFO" },
            LogLevelNameCase{ OCI_LOG_TRACE, "TRACE" },
            LogLevelNameCase{ OCI_LOG_DEBUG, "DEBUG" },
            LogLevelNameCase{ 99,            "?" }));

    struct LogCategoryNameCase
    {
        OCI_UInt32 category;
        const char*  name;
    };

    class TestLogCategoryName : public ::testing::TestWithParam<LogCategoryNameCase> {};

    TEST_P(TestLogCategoryName, Resolves)
    {
        ASSERT_EQ(0, std::strcmp(OCI_TestLogCategoryName(GetParam().category), GetParam().name));
    }

    INSTANTIATE_TEST_CASE_P(Categories, TestLogCategoryName,
        ::testing::Values(
            LogCategoryNameCase{ OCI_LOG_CAT_LIB, "LIB" },
            LogCategoryNameCase{ OCI_LOG_CAT_OCI, "OCI" },
            LogCategoryNameCase{ OCI_LOG_CAT_ALL, "ALL" },
            LogCategoryNameCase{ 0x40,            "?" }));
}
