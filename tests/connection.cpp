#include "ocilib_tests.h"

static Context context;

static inline void AddError(OCI_Error *err)
{
    context.Errs.emplace_back(Error{ OCI_ErrorGetInternalCode(err), OCI_ErrorGetOCICode(err) });
}

TEST(TestConnection, ValidCredentials)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);

    ASSERT_NE(nullptr, conn);

    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestConnection, InvalidUser)
{
    Guard guard(context.Lock);
    context.Errs.clear();

    ASSERT_TRUE(OCI_Initialize(AddError, nullptr, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD_WRONG, OCI_SESSION_DEFAULT);

    ASSERT_EQ(nullptr, conn);

    ASSERT_EQ(1, context.Errs.size());
    ASSERT_EQ(0, context.Errs[0].LibCode);
    ASSERT_EQ(1017, context.Errs[0].OCICode);

    ASSERT_FALSE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());
}


TEST(TestConnection, InvalidPassword)
{
    Guard guard(context.Lock);
    context.Errs.clear();

    ASSERT_TRUE(OCI_Initialize(AddError, nullptr, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD_WRONG, OCI_SESSION_DEFAULT);

    ASSERT_EQ(nullptr, conn);

    ASSERT_EQ(1, context.Errs.size());
    ASSERT_EQ(0, context.Errs[0].LibCode);
    ASSERT_EQ(1017, context.Errs[0].OCICode);

    ASSERT_FALSE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());
}