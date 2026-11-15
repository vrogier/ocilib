#include "ocilib_tests.h"
#include "TestFeatures.h"

#include <cstring>

using namespace ocilib;

namespace TestCppApi
{


    TEST(TestJsonCreateCpp, JsonCreate_ReturnsJson)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_JSON);

        Json json;
        ASSERT_TRUE(json.IsNull());

        Json json2(con);
        ASSERT_FALSE(json2.IsNull());
    }

    /* JSON requires client and server >= 21.1 — verify the exception
       contract on a real unsupported environment */

    TEST(TestJsonCreateCpp, Create_NotAvailable)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        SKIP_IF_FEATURE_AVAILABLE(env, &con, OCI_FEATURE_JSON);

        try
        {
            Json json(con);
            FAIL() << "expected ocilib::Exception";
        }
        catch (const Exception& ex)
        {
            EXPECT_EQ(Exception::OcilibError, ex.GetCategory());
            EXPECT_EQ(static_cast<UInt32>(OCI_ERR_NOT_AVAILABLE), ex.GetCode());
        }
    }

}
