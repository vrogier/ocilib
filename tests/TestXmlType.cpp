#include "ocilib_tests.h"
#include "NullArgs.h"

using namespace ocilib;

namespace TestCApi
{
    TEST(TestXmlType, GetTypeInfo)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("SYS.XMLTYPE"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        ASSERT_EQ(String(OTEXT("XMLTYPE")), String(OCI_TypeInfoGetName(typinf)));
        ASSERT_EQ(0, OCI_TypeInfoGetColumnCount(typinf));

        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestXmlType, Describe)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementDescribe(stmt, OTEXT("SELECT XMLELEMENT(\"person\",xmlattributes(('john') as \"firstname\", ('doe') as \"lastname\")) as xml from dual")));

        const auto rst = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rst);

        const auto col = OCI_ResultsetGetColumn(rst, 0);
        ASSERT_NE(nullptr, col);

        ASSERT_EQ(String(OTEXT("XML")), String(OCI_ColumnGetName(col)));
        ASSERT_EQ(String(OTEXT("XMLTYPE")), String(OCI_ColumnGetSqlType(col)));
        ASSERT_EQ(OCI_CDT_XMLTYPE, OCI_ColumnGetType(col));
        ASSERT_EQ(0, OCI_ColumnGetSubType(col));
        ASSERT_EQ(sizeof(OCI_XmlType*), OCI_ColumnGetSize(col));

        const auto typinf = OCI_ColumnGetTypeInfo(col);
        ASSERT_NE(nullptr, typinf);

        ASSERT_EQ(String(OTEXT("XMLTYPE")), String(OCI_TypeInfoGetName(typinf)));
        ASSERT_EQ(0, OCI_TypeInfoGetColumnCount(typinf));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestXmlType, Fetch)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("SELECT XMLELEMENT(\"person\",xmlattributes(('john') as \"firstname\", ('doe') as \"lastname\")) as xml1, XMLELEMENT(\"person\",xmlattributes(('vince') as \"firstname\", ('rogier') as \"lastname\")) as xml2 from dual")));

        const auto rst = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rst);

        ASSERT_TRUE(OCI_ResultsetFetchNext(rst));

        auto expected1 = String(OTEXT("<person firstname=\"john\" lastname=\"doe\"></person>"));
        auto expected2 = String(OTEXT("<person firstname=\"vince\" lastname=\"rogier\"></person>"));

        auto xmlType1 = OCI_ResultsetGetXmlType(rst, 0);
        ASSERT_NE(nullptr, xmlType1);

        ASSERT_EQ(expected1.size(), OCI_XmlTypeGetContentSize(xmlType1));
        ASSERT_EQ(expected1, String(OCI_XmlTypeGetContent(xmlType1)));

        ASSERT_EQ(expected1, String(OCI_ResultsetGetString(rst, 0)));

        auto xmlType2 = OCI_ResultsetGetXmlType(rst, 1);
        ASSERT_NE(nullptr, xmlType2);

        ASSERT_EQ(expected2.size(), OCI_XmlTypeGetContentSize(xmlType2));
        ASSERT_EQ(expected2, String(OCI_XmlTypeGetContent(xmlType2)));

        ASSERT_EQ(expected2, String(OCI_ResultsetGetString(rst, 1)));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestXmlType, FetchScrollable)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementSetFetchMode(stmt, OCI_SFM_SCROLLABLE));

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("SELECT XMLELEMENT(\"person\",xmlattributes(to_char(ROWNUM) as \"index\")) as xml from dual connect by level <= 2")));

        const auto rst = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rst);

        auto expectedFirst = String(OTEXT("<person index=\"1\"></person>"));
        auto expectedLast = String(OTEXT("<person index=\"2\"></person>"));

        ASSERT_TRUE(OCI_ResultsetFetchLast(rst));
        ASSERT_EQ(expectedLast, String(OCI_ResultsetGetString(rst, 0)));

        ASSERT_TRUE(OCI_ResultsetFetchFirst(rst));
        ASSERT_EQ(expectedFirst, String(OCI_ResultsetGetString(rst, 0)));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    /* --- public-API error paths: NULL xml-type handles hit the CHECK_PTR
       arms (OcilibXmlType* internals are exercised transitively by the
       Select* XML-column fetch tests) --- */

    TEST(TestXmlType, NullXmlTypeErrors)
    {
        ASSERT_EQ(nullptr, OCI_XmlTypeGetContent(nullptr));
        ASSERT_EQ(0u, OCI_XmlTypeGetContentSize(nullptr));
        ASSERT_EQ(nullptr, OCI_ResultsetGetXmlType(nullptr, 0));
        ASSERT_EQ(nullptr, OCI_ResultsetGetXmlTypeByName(nullptr, nullptr));
    }

    class TestXmlTypeNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestXmlTypeNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestXmlTypeNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "XmlTypeGetContent", [] { return nullptr == OCI_XmlTypeGetContent(nullptr); } },
            { "XmlTypeGetContentSize", [] { return 0 == OCI_XmlTypeGetContentSize(nullptr); } },
        }),
        &NullArgCaseName);

}
