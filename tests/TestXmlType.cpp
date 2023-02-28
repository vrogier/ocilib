#include "ocilib_tests.h"

using namespace ocilib;

TEST(TestXmlType, GetTypeInfo)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto typinf = OCI_TypeInfoGet(conn, OTEXT("SYS.XMLTYPE"), OCI_TIF_TYPE);
    ASSERT_NE(nullptr, typinf);

    ASSERT_EQ(ostring(OTEXT("XMLTYPE")), ostring(OCI_TypeInfoGetName(typinf)));
    ASSERT_EQ(0, OCI_TypeInfoGetColumnCount(typinf));

    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestXmlType, GetTypeInfoCpp)
{
    Environment::Initialize();

    Connection conn(DBS, USR, PWD);
    TypeInfo typinf(conn, OTEXT("SYS.XMLTYPE"), TypeInfo::Type);

    ASSERT_EQ(ostring(OTEXT("XMLTYPE")), typinf.GetName());
    ASSERT_EQ(0, typinf.GetColumnCount());

    Environment::Cleanup();
}

TEST(TestXmlType, Describe)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto stmt = OCI_StatementCreate(conn);
    ASSERT_NE(nullptr, stmt);

    ASSERT_TRUE(OCI_Describe(stmt, OTEXT("SELECT XMLELEMENT(\"person\",xmlattributes(('john') as \"firstname\", ('doe') as \"lastname\")) as xml from dual")));
 
    const auto rst = OCI_GetResultset(stmt);
    ASSERT_NE(nullptr, rst);

    const auto col = OCI_GetColumn(rst, 1);
    ASSERT_NE(nullptr, col);

    ASSERT_EQ(ostring(OTEXT("XML")), ostring(OCI_ColumnGetName(col)));
    ASSERT_EQ(ostring(OTEXT("XMLTYPE")), ostring(OCI_ColumnGetSQLType(col)));
    ASSERT_EQ(OCI_CDT_XMLTYPE, OCI_ColumnGetType(col));
    ASSERT_EQ(0, OCI_ColumnGetSubType(col));
    ASSERT_EQ(sizeof(OCI_XmlType*), OCI_ColumnGetSize(col));

    const auto typinf = OCI_ColumnGetTypeInfo(col);
    ASSERT_NE(nullptr, typinf);

    ASSERT_EQ(ostring(OTEXT("XMLTYPE")), ostring(OCI_TypeInfoGetName(typinf)));
    ASSERT_EQ(0, OCI_TypeInfoGetColumnCount(typinf));

    ASSERT_TRUE(OCI_Cleanup());
}


TEST(TestXmlType, DescribeCpp)
{
    Environment::Initialize();

    Connection conn(DBS, USR, PWD);
    Statement stmt (conn);

    stmt.Describe(OTEXT("SELECT XMLELEMENT(\"person\",xmlattributes(('john') as \"firstname\", ('doe') as \"lastname\")) as xml from dual"));
 
    auto rst = stmt.GetResultset();
    auto col = rst.GetColumn( 1);

    ASSERT_EQ(ostring(OTEXT("XML")), col.GetName());
    ASSERT_EQ(ostring(OTEXT("XMLTYPE")), col.GetSQLType());
    ASSERT_EQ(DataTypeValues::TypeXmlType, col.GetType());
    ASSERT_EQ(0, col.GetSubType());
    ASSERT_EQ(sizeof(OCI_XmlType*), col.GetSize());

    auto typinf = col.GetTypeInfo();
    ASSERT_EQ(ostring(OTEXT("XMLTYPE")), typinf.GetName());
    ASSERT_EQ(0, typinf.GetColumnCount());

    Environment::Cleanup();
}

TEST(TestXmlType, Fetch)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto stmt = OCI_StatementCreate(conn);
    ASSERT_NE(nullptr, stmt);

    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT XMLELEMENT(\"person\",xmlattributes(('john') as \"firstname\", ('doe') as \"lastname\")) as xml1, XMLELEMENT(\"person\",xmlattributes(('vince') as \"firstname\", ('rogier') as \"lastname\")) as xml2 from dual")));
 
    const auto rst = OCI_GetResultset(stmt);
    ASSERT_NE(nullptr, rst);

    ASSERT_TRUE(OCI_FetchNext(rst));

    auto expected1 = ostring(OTEXT("<person firstname=\"john\" lastname=\"doe\"></person>"));
    auto expected2 = ostring(OTEXT("<person firstname=\"vince\" lastname=\"rogier\"></person>"));

    auto xmlType1 = OCI_GetXmlType(rst, 1);
    ASSERT_NE(nullptr, xmlType1);

    ASSERT_EQ(expected1.size(), OCI_XmlTypeGetContentSize(xmlType1));
    ASSERT_EQ(expected1, ostring(OCI_XmlTypeGetContent(xmlType1)));

    ASSERT_EQ(expected1, ostring(OCI_GetString(rst, 1)));

    auto xmlType2 = OCI_GetXmlType(rst, 2);
    ASSERT_NE(nullptr, xmlType2);

    ASSERT_EQ(expected2.size(), OCI_XmlTypeGetContentSize(xmlType2));
    ASSERT_EQ(expected2, ostring(OCI_XmlTypeGetContent(xmlType2)));

    ASSERT_EQ(expected2, ostring(OCI_GetString(rst, 2)));
  
    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestXmlType, FetchCpp)
{
    Environment::Initialize();

    Connection conn(DBS, USR, PWD);
    Statement stmt (conn);

    stmt.Execute(OTEXT("SELECT XMLELEMENT(\"person\",xmlattributes(('john') as \"firstname\", ('doe') as \"lastname\")) as xml1, XMLELEMENT(\"person\",xmlattributes(('vince') as \"firstname\", ('rogier') as \"lastname\")) as xml2 from dual"));
 
    auto rst = stmt.GetResultset();

    ASSERT_TRUE(rst.Next());

    auto expected1 = ostring(OTEXT("<person firstname=\"john\" lastname=\"doe\"></person>"));
    auto expected2 = ostring(OTEXT("<person firstname=\"vince\" lastname=\"rogier\"></person>"));

    auto xmlType1 = rst.Get<XmlType>(1);
    ASSERT_EQ(expected1, xmlType1.ToString());
    ASSERT_EQ(expected1, rst.Get<ostring>(1));

    auto xmlType2 = rst.Get<XmlType>(2);
    ASSERT_EQ(expected2, xmlType2.ToString());
    ASSERT_EQ(expected2, rst.Get<ostring>(2));

  
    Environment::Cleanup();
}

TEST(TestXmlType, FetchScrollable)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto stmt = OCI_StatementCreate(conn);
    ASSERT_NE(nullptr, stmt);

    ASSERT_TRUE(OCI_SetFetchMode(stmt, OCI_SFM_SCROLLABLE));

    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT XMLELEMENT(\"person\",xmlattributes(to_char(ROWNUM) as \"index\")) as xml from dual connect by level <= 2")));
 
    const auto rst = OCI_GetResultset(stmt);
    ASSERT_NE(nullptr, rst);

    auto expectedFirst = ostring(OTEXT("<person index=\"1\"></person>"));
    auto expectedLast = ostring(OTEXT("<person index=\"2\"></person>"));

    ASSERT_TRUE(OCI_FetchLast(rst));
    ASSERT_EQ(expectedLast, ostring(OCI_GetString(rst, 1)));
 
    ASSERT_TRUE(OCI_FetchFirst(rst));
    ASSERT_EQ(expectedFirst, ostring(OCI_GetString(rst, 1)));
  
    ASSERT_TRUE(OCI_Cleanup());
}

struct XmlRow
{
    int Level;
    ostring Xml;
};

void TestXmlTypeFetchCustom
(
    unsigned int fetchSize, 
    unsigned int pieceSize, 
    Statement::FetchModeValues mode
)
{ 
    Environment::Initialize();
    Environment::EnableWarnings(true);

    std::vector<XmlRow> xmlRowValues;

    Connection con(DBS, USR, PWD);
    Statement st(con);
    
    st.SetFetchSize(fetchSize);
    st.SetPieceSize(pieceSize);
    st.SetFetchMode(mode);

    st.Prepare(OTEXT("SELECT level lvl, XMLELEMENT(\"element\",xmlattributes((to_char(level)) as name)) as xml from dual connect by level<=100"));
    st.ExecutePrepared();

    auto rs = st.GetResultset();
    while (rs++) 
    {
        xmlRowValues.push_back({ rs.Get<int>(1), ToUpper(rs.Get<ostring>(2)) });
    }

    const auto expectedRows = 100u;
    ASSERT_EQ(expectedRows, rs.GetCount());
    ASSERT_EQ(expectedRows, static_cast<decltype(expectedRows)>(xmlRowValues.size()));

    int i = 1;
    for(auto & row : xmlRowValues)
    {
        ASSERT_EQ(i, row.Level);
        auto expectedString = ToUpper(ostring(OTEXT("<element name=\"")) + TO_STRING(i) + ostring(OTEXT("\"></element>")));
        ASSERT_EQ(expectedString, row.Xml);
        i++;
    }

    Environment::Cleanup();
}


TEST(TestXmlType, FetchSmallBufferFetchSizeOneCpp)
{ 
    TestXmlTypeFetchCustom(1, 20, Statement::FetchForward);
}

TEST(TestXmlType, FetchSmallBufferFetchSizeTwentyCpp)
{ 
    TestXmlTypeFetchCustom(20, 20, Statement::FetchForward);
}

TEST(TestXmlType, FetchDefaultBufferFetchSizeOneCpp)
{ 
     TestXmlTypeFetchCustom(1, OCI_SIZE_PIECE_DYNAMIC_FETCH, Statement::FetchForward);
}

TEST(TestXmlType, FetchDefaultBufferFetchSizeTwentyCpp)
{ 
     TestXmlTypeFetchCustom(20, OCI_SIZE_PIECE_DYNAMIC_FETCH, Statement::FetchForward);

}

TEST(TestXmlType, FetchSmallBufferFetchSizeOneScrollableCpp)
{ 
    TestXmlTypeFetchCustom(1, 20, Statement::FetchForward);
}

TEST(TestXmlType, FetchSmallBufferFetchSizeTwentyscrollableCpp)
{ 
    TestXmlTypeFetchCustom(20, 20, Statement::FetchForward);
}

TEST(TestXmlType, FetchDefaultBufferFetchSizeOneScrollableCpp)
{ 
     TestXmlTypeFetchCustom(1, OCI_SIZE_PIECE_DYNAMIC_FETCH, Statement::FetchForward);
}

TEST(TestXmlType, FetchDefaultBufferFetchSizeTwentyScrollableCpp)
{ 
     TestXmlTypeFetchCustom(20, OCI_SIZE_PIECE_DYNAMIC_FETCH, Statement::FetchForward);
}

TEST(TestXmlType, FetchScrollableCpp)
{
    Environment::Initialize();

    Connection conn(DBS, USR, PWD);
    Statement stmt (conn);

    stmt.SetFetchMode(Statement::FetchScrollable);
    stmt.Execute( OTEXT("SELECT XMLELEMENT(\"person\",xmlattributes(to_char(ROWNUM) as \"index\")) as xml from dual connect by level <= 2"));
  
    auto rst = stmt.GetResultset();

    auto expectedFirst = ostring(OTEXT("<person index=\"1\"></person>"));
    auto expectedLast = ostring(OTEXT("<person index=\"2\"></person>"));

    rst.Last();
    auto xmlType1 = rst.Get<XmlType>(1);
    ASSERT_EQ(expectedLast, xmlType1.ToString());
    ASSERT_EQ(expectedLast, rst.Get<ostring>(1));

    rst.First();
    auto xmlType2 = rst.Get<XmlType>(1);
    ASSERT_EQ(expectedFirst, xmlType2.ToString());
    ASSERT_EQ(expectedFirst, rst.Get<ostring>(1));


    Environment::Cleanup();
}


