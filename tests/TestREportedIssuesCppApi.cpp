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


TEST(ReportedIssuesCppApi, Issue314)
{
    auto exceptionOccured{ false };

    try
    {
        ocilib::Environment::Initialize();

        Connection con(DBS, USR, PWD);
        ocilib::Statement stmt(con);

        int value = 2;

        stmt.Prepare(OTEXT("select * from dual where 1 > :value"));
        stmt.Bind(":value", value, ocilib::BindInfo::In);
        auto bind = stmt.GetBind(1);
        bind.SetCharsetForm(CharsetFormValues::CharsetFormNational);
        stmt.ExecutePrepared();
    }
    catch (ocilib::Exception e)
    {
        exceptionOccured = true;
    }

    ASSERT_FALSE(0, exceptionOccured);


    Environment::Cleanup();
}