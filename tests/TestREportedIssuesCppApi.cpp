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

TEST(ReportedIssuesCppApi, Issue309_NoDataFoundPlsqlWithError)
{
    ExecDML(OTEXT("create table TestTableIssue309(code number, name varchar2(50))"));
    
    int errCode{};

    try
    {
        ocilib::Environment::Initialize();

        Connection con(DBS, USR, PWD);
        ocilib::Statement stmt(con);

        const ocilib::ostring cmd =
        OTEXT
        (
            R"(
            declare
                result varchar2(50);
            begin
                select name into result from TestTableIssue309 where code = 1;
            end;
            )"
        );

        stmt.Execute(cmd);
    }
    catch (ocilib::Exception e)
    {
        errCode = e.GetOracleErrorCode();
    }

    ASSERT_EQ(1403, errCode);

    Environment::Cleanup();

    ExecDML(OTEXT("drop table TestTableIssue309"));
}

TEST(ReportedIssuesCppApi, Issue309_NoDataFoundSqlWithoutError)
{
    ExecDML(OTEXT("create table TestTableIssue309(code number, name varchar2(50))"));

    auto exceptionOccured{ false };
    auto counter{ 0 };
    try
    {
        ocilib::Environment::Initialize();

        Connection con(DBS, USR, PWD);
        ocilib::Statement stmt(con);

        stmt.Execute(OTEXT(" select code, name from TestTableIssue309"));
        auto resultset = stmt.GetResultset();
        while (resultset.Next())
        {
            counter++;
        }
    }
    catch (ocilib::Exception e)
    {
        exceptionOccured = true;
    }

    ASSERT_EQ(0, counter);
    ASSERT_FALSE(exceptionOccured);

    Environment::Cleanup();

    ExecDML(OTEXT("drop table TestTableIssue309"));
}

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

    ASSERT_FALSE(exceptionOccured);

    Environment::Cleanup();
}