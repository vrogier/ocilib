#include "ocilib_tests.h"
#include "ocilib.hpp"

using namespace ocilib;

#ifdef OCI_CHARSET_WIDE

TEST(ReportedIssuesCppApi, Issue250)
{
    std::string ansiMessage;
    std::wstring wideMessage;

    try
    {
        Environment::Initialize();

        Connection con(DBS, USR, PWD_WRONG);
    }
    catch (Exception& ex)
    {
        ansiMessage = ex.what();
        wideMessage = ex.GetMessage();
    }

    std::wstring convertedMessage(ansiMessage.begin(), ansiMessage.end());

    ASSERT_FALSE(ansiMessage.empty());
    ASSERT_FALSE(wideMessage.empty());
    ASSERT_EQ(convertedMessage, wideMessage);

    Environment::Cleanup();
}

#endif
