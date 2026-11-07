#include "ocilib_tests.h"

using namespace ocilib;

namespace TestCppApi
{
    TEST(TestXmlTypeCpp, GetTypeInfoCpp)
    {
        Environment& env = GetEnvironmentCpp();

        Connection conn(env, DBS, USR, PWD);
        TypeInfo typinf(conn, OTEXT("SYS.XMLTYPE"), TypeInfo::Type);

        ASSERT_EQ(String(OTEXT("XMLTYPE")), typinf.GetName());
        ASSERT_EQ(0, typinf.GetColumnCount());

        
    }

    TEST(TestXmlTypeCpp, DescribeCpp)
    {
        Environment& env = GetEnvironmentCpp();

        Connection conn(env, DBS, USR, PWD);
        Statement stmt(conn);

        stmt.Describe(OTEXT("SELECT XMLELEMENT(\"person\",xmlattributes(('john') as \"firstname\", ('doe') as \"lastname\")) as xml from dual"));

        auto rst = stmt.GetResultset();
        auto col = rst.GetColumn(0);

        ASSERT_EQ(String(OTEXT("XML")), col.GetName());
        ASSERT_EQ(String(OTEXT("XMLTYPE")), col.GetSQLType());
        ASSERT_EQ(DataTypeValues::TypeXmlType, col.GetType());
        ASSERT_EQ(0, col.GetSubType());
        ASSERT_EQ(sizeof(OCI_XmlType*), col.GetSize());

        auto typinf = col.GetTypeInfo();
        ASSERT_EQ(String(OTEXT("XMLTYPE")), typinf.GetName());
        ASSERT_EQ(0, typinf.GetColumnCount());

        
    }


    TEST(TestXmlTypeCpp, FetchCpp)
    {
        Environment& env = GetEnvironmentCpp();

        Connection conn(env, DBS, USR, PWD);
        Statement stmt(conn);

        stmt.Execute(OTEXT("SELECT XMLELEMENT(\"person\",xmlattributes(('john') as \"firstname\", ('doe') as \"lastname\")) as xml1, XMLELEMENT(\"person\",xmlattributes(('vince') as \"firstname\", ('rogier') as \"lastname\")) as xml2 from dual"));

        auto rst = stmt.GetResultset();

        ASSERT_TRUE(rst.Next());

        auto expected1 = String(OTEXT("<person firstname=\"john\" lastname=\"doe\"></person>"));
        auto expected2 = String(OTEXT("<person firstname=\"vince\" lastname=\"rogier\"></person>"));

        auto xmlType1 = rst.Get<XmlType>(0);
        ASSERT_EQ(expected1, xmlType1.ToString());
        ASSERT_EQ(expected1, rst.Get<String>(0));

        auto xmlType2 = rst.Get<XmlType>(1);
        ASSERT_EQ(expected2, xmlType2.ToString());
        ASSERT_EQ(expected2, rst.Get<String>(1));

        
    }

    struct XmlRow
    {
        OCI_Int32 Level;
        String Xml;
    };

    void TestXmlTypeFetchCustom
    (
        OCI_UInt32 fetchSize,
        OCI_UInt32 pieceSize,
        Statement::FetchModeValues mode
    )
    {
        Environment& env = GetEnvironmentCpp();
        env.EnableWarnings(true);

        std::vector<XmlRow> xmlRowValues;

        Connection con(env, DBS, USR, PWD);
        Statement st(con);

        st.SetFetchSize(fetchSize);
        st.SetPieceSize(pieceSize);
        st.SetFetchMode(mode);

        st.Prepare(OTEXT("SELECT level lvl, XMLELEMENT(\"element\",xmlattributes((to_char(level)) as name)) as xml from dual connect by level<=100"));
        st.ExecutePrepared();

        auto rs = st.GetResultset();
        while (rs++)
        {
            xmlRowValues.push_back({ rs.Get<OCI_Int32>(0), ToUpper(rs.Get<String>(1)) });
        }

        const auto expectedRows = 100u;
        const auto rows = rs.GetCount();
        const auto xmlrows = static_cast<decltype(expectedRows)>(xmlRowValues.size());
        ASSERT_EQ(expectedRows, rows);
        ASSERT_EQ(expectedRows, xmlrows);

        OCI_Int32 i = 1;
        for (auto& row : xmlRowValues)
        {
            ASSERT_EQ(i, row.Level);
            auto expectedString = ToUpper(String(OTEXT("<element name=\"")) + TO_STRING(i) + String(OTEXT("\"></element>")));
            ASSERT_EQ(expectedString, row.Xml);
            i++;
        }

        
    }


    TEST(TestXmlTypeCpp, FetchSmallBufferFetchSizeOneCpp)
    {
        TestXmlTypeFetchCustom(1, 20, Statement::FetchForward);
    }

    TEST(TestXmlTypeCpp, FetchSmallBufferFetchSizeTwentyCpp)
    {
        TestXmlTypeFetchCustom(20, 20, Statement::FetchForward);
    }

    TEST(TestXmlTypeCpp, FetchDefaultBufferFetchSizeOneCpp)
    {
        TestXmlTypeFetchCustom(1, OCI_SIZE_PIECE_DYNAMIC_FETCH, Statement::FetchForward);
    }

    TEST(TestXmlTypeCpp, FetchDefaultBufferFetchSizeTwentyCpp)
    {
        TestXmlTypeFetchCustom(20, OCI_SIZE_PIECE_DYNAMIC_FETCH, Statement::FetchForward);

    }

    TEST(TestXmlTypeCpp, FetchSmallBufferFetchSizeOneScrollableCpp)
    {
        TestXmlTypeFetchCustom(1, 20, Statement::FetchForward);
    }

    TEST(TestXmlTypeCpp, FetchSmallBufferFetchSizeTwentyscrollableCpp)
    {
        TestXmlTypeFetchCustom(20, 20, Statement::FetchForward);
    }

    TEST(TestXmlTypeCpp, FetchDefaultBufferFetchSizeOneScrollableCpp)
    {
        TestXmlTypeFetchCustom(1, OCI_SIZE_PIECE_DYNAMIC_FETCH, Statement::FetchForward);
    }

    TEST(TestXmlTypeCpp, FetchDefaultBufferFetchSizeTwentyScrollableCpp)
    {
        TestXmlTypeFetchCustom(20, OCI_SIZE_PIECE_DYNAMIC_FETCH, Statement::FetchForward);
    }

    TEST(TestXmlTypeCpp, FetchScrollableCpp)
    {
        Environment& env = GetEnvironmentCpp();

        Connection conn(env, DBS, USR, PWD);
        Statement stmt(conn);

        stmt.SetFetchMode(Statement::FetchScrollable);
        stmt.Execute(OTEXT("SELECT XMLELEMENT(\"person\",xmlattributes(to_char(ROWNUM) as \"index\")) as xml from dual connect by level <= 2"));

        auto rst = stmt.GetResultset();

        auto expectedFirst = String(OTEXT("<person index=\"1\"></person>"));
        auto expectedLast = String(OTEXT("<person index=\"2\"></person>"));

        rst.Last();
        auto xmlType1 = rst.Get<XmlType>(0);
        ASSERT_EQ(expectedLast, xmlType1.ToString());
        ASSERT_EQ(expectedLast, rst.Get<String>(0));

        rst.First();
        auto xmlType2 = rst.Get<XmlType>(0);
        ASSERT_EQ(expectedFirst, xmlType2.ToString());
        ASSERT_EQ(expectedFirst, rst.Get<String>(0));


        
    }

    TEST(TestXmlTypeCpp, NullToString)
    {
        ExecDML(OTEXT("CREATE TABLE TestXmlTypeNull (x XMLTYPE)"));

        Environment& env = GetEnvironmentCpp();
        Connection conn(env, DBS, USR, PWD);
        Statement stmt(conn);

        stmt.Execute(OTEXT("INSERT INTO TestXmlTypeNull VALUES (NULL)"));
        stmt.Execute(OTEXT("SELECT x FROM TestXmlTypeNull"));

        auto rst = stmt.GetResultset();
        ASSERT_TRUE(rst.Next());

        XmlType xml = rst.Get<XmlType>(0);
        ASSERT_EQ(String(), xml.ToString());

        ExecDML(OTEXT("DROP TABLE TestXmlTypeNull"));
    }
}





