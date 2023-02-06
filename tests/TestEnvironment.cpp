#include "ocilib_tests.h"

TEST(TestEnvironment, Default)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    ASSERT_TRUE(OCI_Cleanup());
}
TEST(TestEnvironment, CleanupWithInitialize)
{
    ASSERT_FALSE(OCI_Cleanup());
}
