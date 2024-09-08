#include "ocilib_tests.h"
#include "ocilib.hpp"

using namespace ocilib;
TEST(TestEnvironment, Default)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    ASSERT_TRUE(OCI_Cleanup());
}
TEST(TestEnvironment, CleanupWithInitialize)
{
    ASSERT_FALSE(OCI_Cleanup());
}

TEST(TestEnvironment, DefaultCpp)
{  
      Environment::Initialize();

      Environment::Cleanup();
}

TEST(TestEnvironment, MultipleCall)
{
    for (int i = 1; i < 10; i++)
    {
        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));
        ASSERT_TRUE(OCI_Cleanup());
    }
}

TEST(TestEnvironment, CleanupWithInitializeCpp)
{
    auto exceptionRaised{ false };
    try
    {
        Environment::Cleanup();
    }
    catch (...)
    {
        exceptionRaised = true;
    }

    ASSERT_FALSE(exceptionRaised);
}

TEST(TestEnvironment, MultipleCallCpp)
{
    auto exceptionRaised{ false };

    try
    {
        for (int i = 1; i < 10; i++)
        {
            Environment::Initialize();
            Environment::Cleanup();
        }
    }
    catch(...)
    {
        exceptionRaised = true;
    }

    ASSERT_FALSE(exceptionRaised);
}

