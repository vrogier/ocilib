#include "ocilib_tests.h"
#include "ocilib.hpp"

using namespace ocilib;

namespace TestCApi
{
	TEST(TestEnvironment, Default)
	{
		ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

		ASSERT_TRUE(OCI_Cleanup());
	}
	TEST(TestEnvironment, CleanupWithInitialize)
	{
		ASSERT_FALSE(OCI_Cleanup());
	}

	TEST(TestEnvironment, MultipleCall)
	{
		for (int i = 1; i < 10; i++)
		{
			ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));
			ASSERT_TRUE(OCI_Cleanup());
		}
	}

	TEST(TestEnvironment, GetLocaleValidCode)
	{
		ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

		const otext*  str = OCI_GetLocaleString(OCI_LSI_DAYNAME1);
		ASSERT_NE(nullptr, str);

		ASSERT_TRUE(OCI_Cleanup());
	}

	TEST(TestEnvironment, GetLocaleInalidCode)
	{
		ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

		const otext*  str = OCI_GetLocaleString(999);
		ASSERT_EQ(nullptr, str);

		ASSERT_TRUE(OCI_Cleanup());
	}
}

namespace TestCppApi
{
	TEST(TestEnvironment, DefaultCpp)
	{
		Environment::Initialize();

		Environment::Cleanup();
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
		catch (...)
		{
			exceptionRaised = true;
		}

		ASSERT_FALSE(exceptionRaised);
	}

	TEST(TestEnvironment, GetLocaleValidCodeCpp)
	{
		Environment::Initialize();

		auto str = Environment::GetLocaleString(Environment::LocaleDayname1);
		ASSERT_NE(ostring(), str);

		Environment::Cleanup();
	}

}
