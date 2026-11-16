#pragma once

#include "ocilib_tests.h"

/* Oracle feature ids — re-declared from src/defs.h (internal header, not
   installed); names and values must stay identical to the library's */
enum
{
    OCI_FEATURE_WIDE_USERDATA        = 1,
    OCI_FEATURE_TIMESTAMP            = 2,
    OCI_FEATURE_DIRPATH_DATE_CACHE   = 3,
    OCI_FEATURE_STATEMENT_CACHING    = 4,
    OCI_FEATURE_SCROLLABLE_CURSOR    = 5,
    OCI_FEATURE_DATABASE_NOTIFY      = 6,
    OCI_FEATURE_REMOTE_DBS_CONTROL   = 7,
    OCI_FEATURE_HIGH_AVAILABILITY    = 8,
    OCI_FEATURE_EXTENDED_PLSQLTYPES  = 9,
    OCI_FEATURE_MULTITHREADING      = 10,
    OCI_FEATURE_VECTOR              = 11,
    OCI_FEATURE_SODA                = 12,
    OCI_FEATURE_JSON                = 13,
    OCI_FEATURE_SECURE_NOTIFICATION = 14,
    OCI_FEATURE_SHARDING            = 15,
    OCI_FEATURE_SPARSE_VECTOR       = 16,
    OCI_FEATURE_SQL_BOOLEAN         = 17,
    OCI_FEATURE_COLL_KEY            = 18
};

/* runtime capabilities of the configured environment — probed once in
   main() after LoadTestConfig (run aborts when the probe fails) */
struct TestCaps
{
    OCI_UInt32 client_ver      = OCI_UNKNOWN;   /* OCI client runtime version */
    OCI_UInt32 server_ver      = OCI_UNKNOWN;   /* Oracle server version */
    bool       ctxsys          = false;         /* CTXSYS.CONTEXT indextype present */
    /* true unless a known Oracle client defect makes sharding-key ops
       unusable (instant client 12.2.0.x SEH-faults in
       OCIDescriptorFree(OCI_DTYPE_SHARDING_KEY) — verified with a raw-OCI
       repro, no OCILIB calls) */
    bool       shardingkey_ops = true;
};

const TestCaps& GetTestCaps();

/* true when env (and con when the feature checks the server) satisfy the
   feature's minimum versions — mirrors CHECK_FEATURE/CHECK_ENV_FEATURE;
   the C++ overload probes through the C++ API on purpose so both
   surfaces stay independently exercised */
bool FeatureAvailable(OCI_Environment* env, OCI_Connection* con, int feature);
bool FeatureAvailable(ocilib::Environment& env, ocilib::Connection* con, int feature);
/* caps-based variant — uses the probed TestCaps, no live handles needed;
   use it for gates placed before any handle is created (no leak risk) */
bool FeatureAvailableCaps(int feature);

/* prints a skip marker and records it for the test report
   (gtest 1.8 has no GTEST_SKIP) */
void SkipTest(const char* reason);

#define SKIP_IF_FEATURE_UNAVAILABLE(env, con, feature)                             \
    do                                                                             \
    {                                                                              \
        if (!FeatureAvailable((env), (con), (feature)))                            \
        {                                                                          \
            SkipTest("unsupported client/server version: " #feature);              \
            return;                                                                \
        }                                                                          \
    }                                                                              \
    while (0)

#define SKIP_IF_FEATURE_AVAILABLE(env, con, feature)                               \
    do                                                                             \
    {                                                                              \
        if (FeatureAvailable((env), (con), (feature)))                             \
        {                                                                          \
            SkipTest("feature supported: " #feature);                              \
            return;                                                                \
        }                                                                          \
    }                                                                              \
    while (0)

#define SKIP_IF_FEATURE_UNSUPPORTED(feature)                                       \
    do                                                                             \
    {                                                                              \
        if (!FeatureAvailableCaps(feature))                                        \
        {                                                                          \
            SkipTest("unsupported client/server version: " #feature);              \
            return;                                                                \
        }                                                                          \
    }                                                                              \
    while (0)

#define SKIP_IF_FEATURE_SUPPORTED(feature)                                         \
    do                                                                             \
    {                                                                              \
        if (FeatureAvailableCaps(feature))                                         \
        {                                                                          \
            SkipTest("feature supported on this environment: " #feature);          \
            return;                                                                \
        }                                                                          \
    }                                                                              \
    while (0)

#define SKIP_UNLESS(cond, reason)                                                  \
    do                                                                             \
    {                                                                              \
        if (!(cond))                                                               \
        {                                                                          \
            SkipTest(reason);                                                      \
            return;                                                                \
        }                                                                          \
    }                                                                              \
    while (0)
