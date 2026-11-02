#include "ocilib_tests.h"

namespace TestCppApi
{
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

                const OCI_Size convLenght = mbsrtowcs(buffer, &ptr, value.size(), &mbs);

#if defined(_MSC_VER)
        __pragma(warning(default: 4996))
#endif   

            const OCI_Size whatLenght = (static_cast<OCI_Size>(-1) == convLenght) ? 0 : convLenght;

                                buffer[whatLenght] = 0;
        }

        ostring result = buffer;

        delete[] buffer;

        return result;
    }
    TEST(ReportedIssuesCpp, Issue250)
    {

        std::string ansiMessage;
        std::wstring wideMessage;

        try
        {
            Environment& env = GetEnvironmentCpp();

            Connection con(env, DBS, USR, PWD_WRONG);
        }
        catch (const Exception& ex)
        {
            ansiMessage = ex.what();
            wideMessage = ex.GetMessage();
        }

        std::wstring convertedMessage = ConverStringToAnsi(ansiMessage);

        ASSERT_FALSE(ansiMessage.empty());
        ASSERT_FALSE(wideMessage.empty());
        ASSERT_EQ(convertedMessage, wideMessage);

        
    }

#endif

    TEST(ReportedIssuesCpp, Issue309_NoDataFoundPlsqlWithError)
    {
        ExecDML(OTEXT("create table TestTableIssue309_1(code number, name varchar2(50))"));

        OCI_Int32 errCode{};

        try
        {
            Environment& env = GetEnvironmentCpp();

            Connection con(env, DBS, USR, PWD);
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
        catch (const Exception& e)
        {
            errCode = e.GetOracleErrorCode();
        }

        ASSERT_EQ(1403, errCode);

        

        ExecDML(OTEXT("drop table TestTableIssue309_1"));
    }

    TEST(ReportedIssuesCpp, Issue309_NoDataFoundSqlCallingPlsqlWithoutError)
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
            Environment& env = GetEnvironmentCpp();

            Connection con(env, DBS, USR, PWD);
            Statement stmt(con);

            stmt.Execute(OTEXT("select TestPackageIssue309.ReturnNumber() from dual"));
            auto resultset = stmt.GetResultset();
            while (resultset.Next())
            {
                isNull = resultset.IsColumnNull(1);
                counter++;
            }
        }
        catch (const Exception&)
        {
            exceptionOccured = true;
        }

        ASSERT_EQ(1, counter);
        ASSERT_FALSE(exceptionOccured);
        ASSERT_TRUE(isNull);

        

        ExecDML(OTEXT("drop package TestPackageIssue309"));
        ExecDML(OTEXT("drop table TestTableIssue309_2"));
    }

    TEST(ReportedIssuesCpp, Issue309_NoDataFoundSqlWithoutError)
    {
        ExecDML(OTEXT("create table TestTableIssue309_3(code number, name varchar2(50))"));

        auto exceptionOccured{ false };
        auto counter{ 0 };
        try
        {
            Environment& env = GetEnvironmentCpp();

            Connection con(env, DBS, USR, PWD);
            Statement stmt(con);

            stmt.Execute(OTEXT(" select code, name from TestTableIssue309_3"));
            auto resultset = stmt.GetResultset();
            while (resultset.Next())
            {
                counter++;
            }
        }
        catch (const Exception&)
        {
            exceptionOccured = true;
        }

        ASSERT_EQ(0, counter);
        ASSERT_FALSE(exceptionOccured);

        

        ExecDML(OTEXT("drop table TestTableIssue309_3"));
    }

    TEST(ReportedIssuesCpp, Issue314)
    {
        auto exceptionOccured{ false };

        try
        {
            Environment& env = GetEnvironmentCpp();

            Connection con(env, DBS, USR, PWD);
            Statement stmt(con);

            OCI_Int32 value = 2;

            stmt.Prepare(OTEXT("select * from dual where 1 > :value"));
            stmt.Bind( OTEXT(":value"), value, BindInfo::In);
            auto bind = stmt.GetBind(1);
            bind.SetCharsetForm(CharsetFormValues::CharsetFormNational);
            stmt.ExecutePrepared();
        }
        catch (const Exception&)
        {
            exceptionOccured = true;
        }

        ASSERT_FALSE(exceptionOccured);

        
    }

    TEST(ReportedIssuesCpp, Issue325)
    {
        Environment& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);

        Clob clob(con);

        // to test it compiles
        auto clobConn = clob.GetConnection();

        

    }

    TEST(ReportedIssuesCpp, Issue329)
    {
        Environment& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Statement st(con);

        st.SetFetchMode(Statement::FetchScrollable);
        st.Prepare(OTEXT("select count(*) from dual connect by level<=:lvl"));

        OCI_Int32 i = 10;
        st.Bind( OTEXT(":lvl"), i, BindInfo::In);
        st.ExecutePrepared();

        auto s1 = st.GetSqlIdentifier();

        i = 100;

        st.ExecutePrepared();

        auto s2 = st.GetSqlIdentifier();

        ASSERT_EQ(s1, s2);

        
    }

    TEST(ReportedIssuesCpp, Issue331)
    {
        auto expectedString = ToUpper(ostring(OTEXT("<element name=\"name\" type=\"str\"></element>")));
        Environment& env = GetEnvironmentCpp();
        env.EnableWarnings(true);

        Connection con(env, DBS, USR, PWD);
        Statement st(con);

        st.SetFetchMode(Statement::FetchScrollable);
        st.Execute(OTEXT("SELECT XMLELEMENT(\"element\",xmlattributes(('name') as name, (to_char(dbms_random.value(1,11))) as \"type\")) as xml1, XMLELEMENT(\"element\",xmlattributes(('name') as name, (to_char(dbms_random.value(12,16))) as \"type\")) as xml2 from dual connect by level < 3"));

        auto rs = st.GetResultset();

        auto col = rs.GetColumn(1);
        ASSERT_EQ(ostring(OTEXT("XMLTYPE")), col.GetSQLType());
        ASSERT_EQ(ostring(OTEXT("XMLTYPE")), col.GetFullSQLType());
        ASSERT_EQ(DataTypeValues::TypeXmlType, col.GetType());

        
    }

    TEST(ReportedIssuesCpp, Issue355)
    {
        Environment& env = GetEnvironmentCpp();

        Pool pool(env, DBS, USR, PWD, Pool::SessionPool, 1, 2);

        for (OCI_Int32 i = 0; i < 3; ++i)
        {
            Connection connection = pool.GetConnection();
        }

        
    }

    TEST(ReportedIssuesCpp, Issue351_Date)
    {
        Environment& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);

        auto dt1 = Date::SysDate(env);
        auto dt2 = dt1;

        dt1.AddDays(7);

        auto str1 = dt1.ToString();
        auto str2 = dt2.ToString();

        ASSERT_NE(str1, str2);

        
    }

    TEST(ReportedIssuesCpp, Issue351_Number)
    {
        Environment& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);

        Number nm1(env, 1);
        Number nm2 = nm1;

        nm1 += 5;

        OCI_Int32 int1 = nm1;
        OCI_Int32 int2 = nm2;

        ASSERT_EQ(int1, 6);
        ASSERT_EQ(int2, 1);

        
    }

    TEST(ReportedIssuesCpp, Issue351_Timestamp)
    {
        Environment& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);

        auto ts1 = Timestamp::SysTimestamp(env);
        auto ts2 = ts1;

        Interval itv(env, Interval::YearMonth);
        itv.SetYearMonth(1, 1);

        ts1 += itv;

        auto str1 = ts1.ToString();
        auto str2 = ts2.ToString();

        ASSERT_NE(str1, str2);

        
    }

    TEST(ReportedIssuesCpp, Issue351_Interval)
    {
        Environment& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);

        Interval itv1(env, Interval::YearMonth);
        itv1.SetYearMonth(1970, 1);

        auto itv2 = itv1;

        itv1.SetYearMonth(2025, 10);

        auto str1 = itv1.ToString();
        auto str2 = itv2.ToString();

        ASSERT_NE(str1, str2);

        
    }

    TEST(ReportedIssuesCpp, Issue377)
    {
        ExecDML(OTEXT("create table TestIssue377(value number(18))"));

        ostring errMessage;
        OCI_Int32 errCode{};
        OCI_Size expectedCount{ 10 };
        OCI_Size insertedCount{};

        try
        {
            Environment& env = GetEnvironmentCpp();
            Connection conn(env, DBS, USR, PWD);

            static std::vector<OCI_Int64> values;
            for (OCI_Int32 i = 1; i <= expectedCount; ++i)
            {
                values.push_back(1234567890000 + i);
            }

            ostring sql = OTEXT("begin insert into TestIssue377(value) values (:0); end;");
            Statement stmt(conn);
            stmt.Prepare(sql);
            stmt.SetBindArraySize(static_cast<OCI_UInt32>(values.size()));
            stmt.Bind( OTEXT(":0"), values, BindInfo::In);
            stmt.ExecutePrepared();

            stmt.Execute(OTEXT("select value from TestIssue377"));
            auto rs = stmt.GetResultset();
            while (rs++)
            {
                ASSERT_EQ(values[insertedCount], rs.Get<OCI_Int64>(1));
                insertedCount++;
            }
        }
        catch (const Exception& ex)
        {
            errMessage = ex.GetMessage();
            errCode = ex.GetOracleErrorCode();
        }

        ASSERT_EQ(true, errMessage.empty());
        ASSERT_EQ(0, errCode);
        ASSERT_EQ(expectedCount, insertedCount);

        

        ExecDML(OTEXT("drop table TestIssue377"));
    }

    TEST(ReportedIssuesCpp, Issue379_Collection)
    {
        ExecDML(OTEXT("create or replace type Issue379Type is table of varchar2(255)"));

        Environment& env = GetEnvironmentCpp();

        auto pool = Pool(env, DBS, USR, PWD, Pool::ConnectionPool, 0, 1);
        auto conn = pool.GetConnection();
        auto type = TypeInfo(conn, OTEXT("Issue379Type"), ocilib::TypeInfo::Type);
        auto coll = Collection<ostring>(type);
        
        ASSERT_EQ(0, coll.GetSize());

        coll.Append(OTEXT("Value1"));

        ASSERT_EQ(1, coll.GetSize());

        
    
        ExecDML(OTEXT("drop type Issue379Type"));
    }

    TEST(ReportedIssuesCpp, Issue379_Lob)
    {
        Environment& env = GetEnvironmentCpp();

        auto pool = Pool(env, DBS, USR, PWD, Pool::ConnectionPool, 0, 1);
        auto conn = pool.GetConnection();
        auto lob = Clob(conn);

        {
            auto localConn = lob.GetConnection();
            ASSERT_EQ(true, localConn.IsServerAlive());
        }

        ASSERT_EQ(false, lob.IsNull());
        ASSERT_EQ(true, conn.IsServerAlive());

        
    }

    TEST(ReportedIssuesCpp, Issue379_File)
    {
        Environment& env = GetEnvironmentCpp();

        auto pool = Pool(env, DBS, USR, PWD, Pool::ConnectionPool, 0, 1);
        auto conn = pool.GetConnection();
        auto file = File(conn);

        {
            auto localConn = file.GetConnection();
            ASSERT_EQ(true, localConn.IsServerAlive());
        }

        ASSERT_EQ(false, file.IsNull());
        ASSERT_EQ(true, conn.IsServerAlive());

        
    }

    TEST(ReportedIssuesCpp, Issue379_Statement)
    {
        Environment& env = GetEnvironmentCpp();

        auto pool = Pool(env, DBS, USR, PWD, Pool::ConnectionPool, 0, 1);
        auto conn = pool.GetConnection();
        auto stmt = Statement(conn);

        {
            auto localConn = stmt.GetConnection();
            ASSERT_EQ(true, localConn.IsServerAlive());
        }

        ASSERT_EQ(false, stmt.IsNull());
        ASSERT_EQ(true, conn.IsServerAlive());

        
    }

    TEST(ReportedIssuesCpp, Issue381_BindByName)
    {
        Environment& env = GetEnvironmentCpp();

        auto conn = Connection(env, DBS, USR, PWD);
        auto stmt = Statement(conn);

        stmt.Prepare(OTEXT("SELECT 1 FROM dual WHERE 1 = :abc and 2 = :def and 3 = :def"));

        auto bindInfo = stmt.GetParseBindNames();

        ASSERT_EQ(2, bindInfo.size());
        ASSERT_EQ(ostring(OTEXT("ABC")), bindInfo[0]);
        ASSERT_EQ(ostring(OTEXT("DEF")), bindInfo[1]);

        
    }

    TEST(ReportedIssuesCpp, Issue381_BindByPos)
    {
        Environment& env = GetEnvironmentCpp();

        auto conn = Connection(env, DBS, USR, PWD);
        auto stmt = Statement(conn);

        stmt.SetBindMode(Statement::BindByPosition);
        stmt.Prepare(OTEXT("SELECT 1 FROM dual WHERE 1 = :abc and 2 = :def and 3 = :def"));

        auto bindInfo = stmt.GetParseBindNames();

        ASSERT_EQ(3, bindInfo.size());
        ASSERT_EQ(ostring(OTEXT("ABC")), bindInfo[0]);
        ASSERT_EQ(ostring(OTEXT("DEF")), bindInfo[1]);
        ASSERT_EQ(ostring(OTEXT("DEF")), bindInfo[2]);

        
    }

    TEST(ReportedIssuesCpp, Issue382)
    {
        ExecDML(OTEXT("drop user UserIssue382"));
        ExecDML(OTEXT("drop profile ProfileIssue382"));

        ExecDML(OTEXT("create profile ProfileIssue382 limit password_life_time 0.00001158 password_grace_time 1"));
        ExecDML(OTEXT("create user UserIssue382 identified by UserIssue382"));
        ExecDML(OTEXT("grant create session to UserIssue382"));
        ExecDML(OTEXT("alter user UserIssue382 profile ProfileIssue382"));

        Sleep(2000);

        ostring errMessage;
        OCI_Int32 errCode{};

        try
        {
            Environment& env = GetEnvironmentCpp();
            Connection conn(env, DBS, OTEXT("UserIssue382"), OTEXT("UserIssue382"));
        }
        catch (const Exception& ex)
        {
            errMessage = ex.GetMessage();
            errCode = ex.GetOracleErrorCode();
        }

        ASSERT_EQ(true, errMessage.empty());
        ASSERT_EQ(0, errCode);

        try
        {
            Environment& env = GetEnvironmentCpp();
            env.EnableWarnings(true);
            Connection conn(env, DBS, OTEXT("UserIssue382"), OTEXT("UserIssue382"));
        }
        catch (const Exception& ex)
        {
            errMessage = ex.GetMessage();
            errCode = ex.GetOracleErrorCode();
        }

        ASSERT_EQ(false, errMessage.empty());
        ASSERT_EQ(28098, errCode);


        

        ExecDML(OTEXT("drop user UserIssue382"));
        ExecDML(OTEXT("drop profile ProfileIssue382"));
    }

    TEST(ReportedIssuesCpp, Issue387)
    { 
        const auto msgTypeName = OTEXT("SYS.RAW");
        
        Environment& env = GetEnvironmentCpp();

        auto conn = Connection(env, DBS, USR, PWD);   
        auto msgType = TypeInfo(conn, msgTypeName, TypeInfo::Type);
        auto msgRaw = Message(msgType);
      
        auto emptyRaw = msgRaw.GetPayload<Raw>();
        ASSERT_EQ(0, emptyRaw.size());

        msgRaw.SetPayload(Raw{ 0x01, 0x02, 0x03 });

        auto validRaw = msgRaw.GetPayload<Raw>();
        ASSERT_EQ(3, validRaw.size());

        
    }
}


