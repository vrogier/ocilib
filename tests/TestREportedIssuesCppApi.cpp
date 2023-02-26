#include "ocilib_tests.h"
#include "ocilib.hpp"

using namespace ocilib;

#ifdef OCI_CHARSET_WIDE

ostring ConverStringToAnsi(std::string value) noexcept
{
    if (value.empty())
    {
        return {};
    }

    otext* buffer = new (std::nothrow) otext[value.size() + 1];
    if (buffer)
    {
        const char* ptr = value.data();
       
        mbstate_t mbs;
        mbrlen(nullptr, 0, &mbs);
    
    #if defined(_MSC_VER)
        __pragma(warning(disable : 4996))
    #endif

        const size_t convLenght = mbsrtowcs(buffer, &ptr, value.size(), &mbs);

    #if defined(_MSC_VER)
        __pragma(warning(default: 4996))
    #endif   
            
        const size_t whatLenght = (static_cast<size_t>(-1) == convLenght) ? 0 : convLenght;

        buffer[whatLenght] = 0;
    }

    ostring result = buffer;

    delete[] buffer;

    return result;
}
TEST(ReportedIssuesCppApi, Issue250)
{
    std::string ansiMessage;
    std::wstring wideMessage;

    try
    {
        Environment::Initialize();

        Connection con(DBS, USR, PWD_WRONG);
    }
    catch (const ocilib::Exception& ex)
    {
        ansiMessage = ex.what();
        wideMessage = ex.GetMessage();
    }

    std::wstring convertedMessage = ConverStringToAnsi(ansiMessage);

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
    catch (const ocilib::Exception&)
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
        stmt.Bind(OTEXT(":value"), value, ocilib::BindInfo::In);
        auto bind = stmt.GetBind(1);
        bind.SetCharsetForm(CharsetFormValues::CharsetFormNational);
        stmt.ExecutePrepared();
    }
    catch (const ocilib::Exception&)
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

TEST(ReportedIssuesCppApi, Issue329)
{ 
    Environment::Initialize(Environment::Threaded);

    Connection con(DBS, USR, PWD);
    Statement st(con);

    st.SetFetchMode(Statement::FetchScrollable);
    st.Prepare(OTEXT("select count(*) from dual connect by level<=:lvl"));

    int i = 10;
    st.Bind(OTEXT(":lvl"), i, BindInfo::In);
    st.ExecutePrepared();

    auto s1 = st.GetSqlIdentifier();

    i = 100;

    st.ExecutePrepared();

    auto s2 = st.GetSqlIdentifier();

    ASSERT_EQ(s1, s2);

    Environment::Cleanup();
}

TEST(ReportedIssuesCppApi, Issue331)
{ 
    auto expectedString = ToUpper(ostring(OTEXT("<element name=\"name\" type=\"str\"></element>")));
    Environment::Initialize();
    Environment::EnableWarnings(true);

    Connection con(DBS, USR, PWD);
    Statement st(con);

    st.SetFetchMode(Statement::FetchScrollable);
    st.Execute(OTEXT("SELECT XMLELEMENT(\"element\",xmlattributes(('name') as name, (to_char(dbms_random.value(1,11))) as \"type\")) as xml1, XMLELEMENT(\"element\",xmlattributes(('name') as name, (to_char(dbms_random.value(12,16))) as \"type\")) as xml2 from dual connect by level < 3"));
 
    auto rs = st.GetResultset();

    auto col = rs.GetColumn(1);
    ASSERT_EQ(ostring(OTEXT("XMLTYPE")), col.GetSQLType());
    ASSERT_EQ(ostring(OTEXT("XMLTYPE")), col.GetFullSQLType());
    ASSERT_EQ(DataTypeValues::TypeXmlType, col.GetType());


    Environment::Cleanup();
}
