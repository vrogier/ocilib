#pragma once

#include "ocilib_tests.h"

#include <functional>
#include <vector>
#include <ctime>

/*
 * NULL-handle contract tests (test-quality-plan item 1).
 *
 * Every public OCI_* API taking a handle as first argument must reject NULL
 * gracefully: return its documented failure value and raise
 * OCI_ERR_NULL_POINTER (category OCI_ERR_CAT_LIB) — never crash.
 *
 * The failure value asserted by each case is derived from the implementation's
 * ENTER_FUNC(...) default (the value returned when CHECK_PTR fails).
 * APIs that reject NULL *silently* by design (error.c getters, handle.c
 * user-data accessors, ...) set `raises` to NoNullPointerError.
 */

constexpr bool NoNullPointerError = false;

struct NullArgCase
{
    const char* Api;                 // public API name without the OCI_ prefix
    std::function<bool()> Invoke;    // returns true when the failure contract holds
    bool raises = true;              // true when OCI_ERR_NULL_POINTER must be raised
};

using NullArgCases = std::vector<NullArgCase>;

inline void RunNullArgCase(const NullArgCase& c)
{
    const auto env = GetEnvironment(nullptr);
    ASSERT_NE(nullptr, env);

    EXPECT_TRUE(c.Invoke()) << c.Api << " must return its documented failure value on NULL";

    const auto err = OCI_GetLastError();

    if (c.raises)
    {
        ASSERT_NE(nullptr, err) << c.Api << " must raise OCI_ERR_NULL_POINTER on NULL";
        EXPECT_EQ(OCI_ERR_CAT_LIB, static_cast<OCI_Int32>(OCI_ErrorGetCategory(err)));
        EXPECT_EQ(OCI_ERR_NULL_POINTER, static_cast<OCI_Int32>(OCI_ErrorGetCode(err)));
    }
    else
    {
        /* silent rejection is part of the contract for these APIs:
           any public call resets the last error on entry, so nothing
           may be recorded */
        EXPECT_EQ(nullptr, err) << c.Api << " must reject NULL silently";
    }
}

inline std::string NullArgCaseName(const ::testing::TestParamInfo<NullArgCase>& info)
{
    return info.param.Api;
}

/* plausible non-NULL filler arguments for calls under test */

inline otext* NullArgText()
{
    static otext buffer[64] = {};
    return buffer;
}

inline void* NullArgBuffer()
{
    static unsigned char buffer[256] = {};
    return buffer;
}

inline unsigned char* NullArgRaw()
{
    static unsigned char buffer[64] = {};
    return buffer;
}

inline tm* NullArgTm()
{
    static tm value = {};
    return &value;
}

inline time_t* NullArgTime()
{
    static time_t value = 0;
    return &value;
}

template <typename T>
inline T* NullArgOut()
{
    static T value{};
    return &value;
}

/* Usage per test file, at the end of namespace TestCApi:

    class Test<Feature>NullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(Test<Feature>NullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, Test<Feature>NullArgs,
        ::testing::ValuesIn(NullArgCases{ ... }),
        &NullArgCaseName);
*/
