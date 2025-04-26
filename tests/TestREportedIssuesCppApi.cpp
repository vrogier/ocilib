#include "ocilib_tests.h"
#include "ocilib.hpp"

using namespace ocilib;

namespace TestCppApi
{

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

    TEST(ReportedIssuesCppApi, Issue355)
    {  
        Environment::Initialize(ocilib::Environment::Threaded);

        Pool pool(DBS, USR, PWD, Pool::SessionPool, 1, 2);

        for (int i = 0; i < 3; ++i)
        {
            ocilib::Connection connection = pool.GetConnection();
        }

        Environment::Cleanup();
    }

    TEST(ReportedIssuesCppApi, Issue351_Date)
    { 
        Environment::Initialize();

        Connection con(DBS, USR, PWD);

        auto dt1 = Date::SysDate();
        auto dt2 = dt1;

        dt1.AddDays(7);

        auto str1 = dt1.ToString();
        auto str2 = dt2.ToString();

        ASSERT_NE(str1, str2);

        Environment::Cleanup();
   }

    TEST(ReportedIssuesCppApi, Issue351_Number)
    {
        Environment::Initialize();

        Connection con(DBS, USR, PWD);

        Number nm1 = 1;
        Number nm2 = nm1;

        nm1 += 5;

        int int1 = nm1;
        int int2 = nm2;

        ASSERT_EQ(int1, 6);
        ASSERT_EQ(int2, 1);

        Environment::Cleanup();
    }

    TEST(ReportedIssuesCppApi, Issue351_Timestamp)
    {
        Environment::Initialize();

        Connection con(DBS, USR, PWD);

        auto ts1 = Timestamp::SysTimestamp();
        auto ts2 = ts1;

        Interval itv(Interval::YearMonth);
        itv.SetYearMonth(1, 1);

        ts1 += itv;

        auto str1 = ts1.ToString();
        auto str2 = ts2.ToString();

        ASSERT_NE(str1, str2);

        Environment::Cleanup();
    }

    TEST(ReportedIssuesCppApi, Issue351_Interval)
    {
        Environment::Initialize();

        Connection con(DBS, USR, PWD);

        Interval itv1(Interval::YearMonth);
        itv1.SetYearMonth(1970, 1);

        auto itv2 = itv1;

        itv1.SetYearMonth(2025, 10);

        auto str1 = itv1.ToString();
        auto str2 = itv2.ToString();

        ASSERT_NE(str1, str2);

        Environment::Cleanup();
    }

    TEST(ReportedIssuesCppApi, Issue370)
    {
        int errCode = 0;
        int errType = 0;
        try
        {
            auto inititialzed = Environment::Initialized();
        }
        catch (const ocilib::Exception& ex)
        {
            errCode = ex.GetInternalErrorCode();
            errType = ex.GetType();
        }

        ASSERT_EQ(OCI_ERR_NOT_INITIALIZED, errCode);
        ASSERT_EQ(Exception::ExceptionTypeValues::OcilibError, errType);
    }

    TEST(ReportedIssuesCppApi, Issue377)
    {
        ExecDML(OTEXT("create table TestIssue377(value number(18))"));

        ostring errMessage;
        int errCode{};
        size_t expectedCount{10};
        size_t insertedCount{};

        try
        {
            ocilib::Environment::Initialize(ocilib::Environment::Default | ocilib::Environment::Threaded);
            Connection conn(DBS, USR, PWD);

            std::vector<big_int> values;
            for (int i = 1; i <= expectedCount; ++i)
            {
                values.push_back(1234567890000 + i);
            }

            ostring sql = OTEXT("begin insert into TestIssue377(value) values (:0); end;");
            ocilib::Statement stmt(conn);
            stmt.Prepare(sql);
            stmt.SetBindArraySize(static_cast<unsigned int>(values.size()));
            stmt.Bind(OTEXT(":0"), values, ocilib::BindInfo::In);
            stmt.ExecutePrepared();

            stmt.Execute(OTEXT("select value from TestIssue377"));
            auto rs = stmt.GetResultset();
            while (rs++)
            {
                ASSERT_EQ(values[insertedCount],rs.Get<big_int>(1));
                insertedCount++;
            }
        }
        catch (const ocilib::Exception& ex) 
        {
            errMessage = ex.GetMessage();
            errCode = ex.GetOracleErrorCode();
        }

        ASSERT_EQ(true, errMessage.empty());
        ASSERT_EQ(0, errCode);
        ASSERT_EQ(expectedCount, insertedCount);

        ocilib::Environment::Cleanup();
     
        ExecDML(OTEXT("drop table TestIssue377"));
    }
}

