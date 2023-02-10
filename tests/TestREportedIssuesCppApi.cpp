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
    catch (const ocilib::Exception& e)
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
    ExecDML(OTEXT("create table TestTableIssue309_1(code number, name varchar2(50))"));
    
    int errCode{};

    try
    {
        Environment::Initialize();

        Connection con(DBS, USR, PWD);
        Statement stmt(con);

        const ostring cmd =
        OTEXT
        (
            R"(
            declare
                result varchar2(50);
            begin
                select name into result from TestTableIssue309_1 where code = 1;
            end;
            )"
        );

        stmt.Execute(cmd);
    }
    catch (const ocilib::Exception& e)
    {
        errCode = e.GetOracleErrorCode();
    }

    ASSERT_EQ(1403, errCode);

    Environment::Cleanup();

    ExecDML(OTEXT("drop table TestTableIssue309_1"));
}

TEST(ReportedIssuesCppApi, Issue309_NoDataFoundSqlCallingPlsqlWithoutError)
{
    ExecDML(OTEXT("create table TestTableIssue309_2(code number)"));
    ExecDML(OTEXT(
        R"(
            CREATE OR REPLACE PACKAGE TestPackageIssue309 IS
                FUNCTION ReturnNumber return number;
            END;
            )"
        ));
    ExecDML(OTEXT(
        R"(
            CREATE OR REPLACE PACKAGE BODY TestPackageIssue309 IS
                FUNCTION ReturnNumber return number
	            AS
		            result number;
	            BEGIN
	                select code into result from TestTableIssue309_2 where code = 1;
	                return result;
	            END;
            END;
            )"
    ));

    auto exceptionOccured{ false };
    auto counter{ 0 };
    auto isNull{ false };
    try
    {
        Environment::Initialize();

        Connection con(DBS, USR, PWD);
        Statement stmt(con);

        stmt.Execute(OTEXT("select TestPackageIssue309.ReturnNumber() from dual"));
        auto resultset = stmt.GetResultset();
        while (resultset.Next())
        {
            isNull = resultset.IsColumnNull(1);
            counter++;
        }
    }
    catch (const ocilib::Exception&)
    {
        exceptionOccured = true;
    }

    ASSERT_EQ(1, counter);
    ASSERT_FALSE(exceptionOccured);
    ASSERT_TRUE(isNull);

    Environment::Cleanup();

    ExecDML(OTEXT("drop package TestPackageIssue309"));
    ExecDML(OTEXT("drop table TestTableIssue309_2"));
}

TEST(ReportedIssuesCppApi, Issue309_NoDataFoundSqlWithoutError)
{
    ExecDML(OTEXT("create table TestTableIssue309_3(code number, name varchar2(50))"));

    auto exceptionOccured{ false };
    auto counter{ 0 };
    try
    {
        Environment::Initialize();

        Connection con(DBS, USR, PWD);
        Statement stmt(con);

        stmt.Execute(OTEXT(" select code, name from TestTableIssue309_3"));
        auto resultset = stmt.GetResultset();
        while (resultset.Next())
        {
            counter++;
        }
    }
    catch (const ocilib::Exception& e)
    {
        exceptionOccured = true;
    }

    ASSERT_EQ(0, counter);
    ASSERT_FALSE(exceptionOccured);

    Environment::Cleanup();

    ExecDML(OTEXT("drop table TestTableIssue309_3"));
}

TEST(ReportedIssuesCppApi, Issue314)
{
    auto exceptionOccured{ false };

    try
    {
        Environment::Initialize();

        Connection con(DBS, USR, PWD);
        Statement stmt(con);

        int value = 2;

        stmt.Prepare(OTEXT("select * from dual where 1 > :value"));
        stmt.Bind(":value", value, ocilib::BindInfo::In);
        auto bind = stmt.GetBind(1);
        bind.SetCharsetForm(CharsetFormValues::CharsetFormNational);
        stmt.ExecutePrepared();
    }
    catch (const ocilib::Exception& e)
    {
        exceptionOccured = true;
    }

    ASSERT_FALSE(exceptionOccured);

    Environment::Cleanup();
}

TEST(ReportedIssuesCppApi, Issue325)
{
    Environment::Initialize();

    Connection con(DBS, USR, PWD);
 
    Clob clob(con);

    // to test it compiles
    auto clobConn = clob.GetConnection();

    Environment::Cleanup();

}