#include "ocilib_tests.h"
#include "test_internals.h"

namespace TestCApi
{
    TEST(TestTypeInfo, Metadata)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCApiCovType AS OBJECT(code NUMBER, name VARCHAR2(50))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        auto typinf = OCI_TypeInfoGet(con, OTEXT("TestCApiCovType"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        ASSERT_EQ(OCI_TIF_TYPE, OCI_TypeInfoGetType(typinf));
        ASSERT_EQ(con, OCI_TypeInfoGetConnection(typinf));
        ASSERT_NE(nullptr, OCI_TypeInfoGetName(typinf));
        ASSERT_EQ(2u, OCI_TypeInfoGetColumnCount(typinf));
        ASSERT_TRUE(OCI_TypeInfoIsFinalType(typinf));

        auto col = OCI_TypeInfoGetColumn(typinf, 1);
        ASSERT_NE(nullptr, col);
        ASSERT_NE(nullptr, OCI_ColumnGetName(col));

        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TYPE TestCApiCovType"));
    }

    TEST(TestTypeInfo, CollectionType)
    {
        /* named collection types hit the SQLT_NCO arm of OcilibTypeInfoGet() */

        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCApiCovColl AS TABLE OF NUMBER"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto typinf = OCI_TypeInfoGet(con, OTEXT("TestCApiCovColl"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);
        ASSERT_EQ(OCI_TIF_TYPE, OCI_TypeInfoGetType(typinf));

        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TYPE TestCApiCovColl"));
    }

    TEST(TestTypeInfo, SynonymResolution)
    {
        /* describing a synonym walks the OCI_PTYPE_SYN arm and resolves
           the underlying object */

        ExecDML(OTEXT("drop table TestCApiCovSynTab"));
        ExecDML(OTEXT("create table TestCApiCovSynTab(code number, name varchar2(30))"));
        ExecDML(OTEXT("create or replace synonym TestCApiCovSyn for TestCApiCovSynTab"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto typinf = OCI_TypeInfoGet(con, OTEXT("TestCApiCovSyn"), OCI_TIF_TABLE);
        ASSERT_NE(nullptr, typinf);
        ASSERT_EQ(OCI_TIF_TABLE, OCI_TypeInfoGetType(typinf));
        ASSERT_EQ(2u, OCI_TypeInfoGetColumnCount(typinf));

        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("drop synonym TestCApiCovSyn"));
        ExecDML(OTEXT("drop table TestCApiCovSynTab"));
    }

    /* a type UNDER another covers the supertype resolution arm;
       asking a table for OCI_TIF_TYPE covers the wrong-type error */

    class TestTypeInfoSub : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCApiCovSuper AS OBJECT(a NUMBER) NOT FINAL"));
            ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCApiCovSub UNDER TestCApiCovSuper (b NUMBER)"));
            ExecDML(OTEXT("drop table TestCApiCovWrongTab"));
            ExecDML(OTEXT("create table TestCApiCovWrongTab(code number)"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestCApiCovWrongTab"));
            ExecDML(OTEXT("DROP TYPE TestCApiCovSub"));
            ExecDML(OTEXT("DROP TYPE TestCApiCovSuper"));
        }

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, con);
        }

        void TearDown() override
        {
            OCI_ConnectionFree(con);
        }

        OCI_Connection* con{ nullptr };
    };

    TEST_F(TestTypeInfoSub, SubtypeResolvesColumns)
    {
        const auto typinf = OCI_TypeInfoGet(con, OTEXT("TestCApiCovSub"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);
        ASSERT_EQ(OCI_TIF_TYPE, OCI_TypeInfoGetType(typinf));
        ASSERT_EQ(2u, OCI_TypeInfoGetColumnCount(typinf));

        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
    }

    TEST_F(TestTypeInfoSub, SupertypeIsNotFinal)
    {
        /* the supertype is NOT FINAL (a leaf subtype is FINAL by default) */

        const auto super = OCI_TypeInfoGet(con, OTEXT("TestCApiCovSuper"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, super);
        ASSERT_FALSE(OCI_TypeInfoIsFinalType(super));

        ASSERT_TRUE(OCI_TypeInfoFree(super));
    }

    TEST_F(TestTypeInfoSub, WrongTypeFails)
    {
        /* describing a table as a type fails */

        ASSERT_EQ(nullptr, OCI_TypeInfoGet(con, OTEXT("TestCApiCovWrongTab"), OCI_TIF_TYPE));
    }

    /* armed faults make the Nth subsequent OCI call fail, covering the
       error arm of the CHECK_* macro at that position in the describe
       sequence */

    class TestTypeInfoErr : public ::testing::Test
    {
    public:

        /* public: parameterized subclasses require access in gtest 1.8 */

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("drop table TestTiErrTab"));
            ExecDML(OTEXT("create table TestTiErrTab(id number, name varchar2(20))"));
            ExecDML(OTEXT("create or replace type TestTiErrTyp as object(a number, b varchar2(20))"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop type TestTiErrTyp"));
            ExecDML(OTEXT("drop table TestTiErrTab"));
        }

    protected:

        void SetUp() override
        {
            env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, con);
        }

        void TearDown() override
        {
            OCI_TestApiRestore(env);
            OCI_ConnectionFree(con);
        }

        OCI_Environment* env{ nullptr };
        OCI_Connection*  con{ nullptr };
    };

    TEST_F(TestTypeInfoErr, DescribeFailsOnAttrSet)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_SET, 1);
        ASSERT_EQ(nullptr, OCI_TypeInfoGet(con, OTEXT("TestTiErrTab"), OCI_TIF_TABLE));
    }

    /* a failed describe leaves no cached entry, so each position can be
       swept independently on the same object */

    struct TypeInfoAttrFailCase
    {
        const otext* name;
        OCI_TypeInfoType type;
        OCI_Int32          position;
    };

    class TestTypeInfoAttrFail : public TestTypeInfoErr,
                                 public ::testing::WithParamInterface<TypeInfoAttrFailCase> {};

    TEST_P(TestTypeInfoAttrFail, Fails)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_GET, GetParam().position);
        ASSERT_EQ(nullptr, OCI_TypeInfoGet(con, GetParam().name, GetParam().type));
    }

    static TypeInfoAttrFailCase MakeTableCase(OCI_Int32 pos)
    {
        return TypeInfoAttrFailCase{ OTEXT("TestTiErrTab"), OCI_TIF_TABLE, pos };
    }

    static TypeInfoAttrFailCase MakeTypeCase(OCI_Int32 pos)
    {
        return TypeInfoAttrFailCase{ OTEXT("TestTiErrTyp"), OCI_TIF_TYPE, pos };
    }

    static const TypeInfoAttrFailCase TypeInfoAttrFailCases[] =
    {
        MakeTableCase(1), MakeTableCase(2), MakeTableCase(3), MakeTableCase(4),
        MakeTableCase(5), MakeTableCase(6), MakeTableCase(7), MakeTableCase(8),
        MakeTypeCase(1),  MakeTypeCase(2),  MakeTypeCase(3),  MakeTypeCase(4),
        MakeTypeCase(5),  MakeTypeCase(6),  MakeTypeCase(7),  MakeTypeCase(8),
        MakeTypeCase(9),  MakeTypeCase(10)
    };

    INSTANTIATE_TEST_CASE_P(AttrGetPositions, TestTypeInfoAttrFail,
        ::testing::ValuesIn(TypeInfoAttrFailCases));

    TEST_F(TestTypeInfoErr, DescribeFailsOnTypeByName)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_TYPE_BY_NAME, 1);
        ASSERT_EQ(nullptr, OCI_TypeInfoGet(con, OTEXT("TestTiErrTyp"), OCI_TIF_TYPE));
    }

    TEST_F(TestTypeInfoErr, DescribeTypeFailsOnDescribeAny)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_DESCRIBE_ANY, 1);
        ASSERT_EQ(nullptr, OCI_TypeInfoGet(con, OTEXT("TestTiErrTyp"), OCI_TIF_TYPE));
    }

    TEST_F(TestTypeInfoErr, DescribeTableFailsOnDescribeAny)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_DESCRIBE_ANY, 1);
        ASSERT_EQ(nullptr, OCI_TypeInfoGet(con, OTEXT("TestTiErrTab"), OCI_TIF_TABLE));
    }

    TEST_F(TestTypeInfoErr, DescribeFailsOnParamGet)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_PARAM_GET, 1);
        ASSERT_EQ(nullptr, OCI_TypeInfoGet(con, OTEXT("TestTiErrTyp"), OCI_TIF_TYPE));
    }

    TEST_F(TestTypeInfoErr, WrongTypeAfterDescribe)
    {
        /* wrong-type sanity check after a successful describe */

        ASSERT_EQ(nullptr, OCI_TypeInfoGet(con, OTEXT("TestTiErrTab"), OCI_TIF_TYPE));
    }

    TEST_F(TestTypeInfoErr, UnsupportedSystemType)
    {
        /* an opaque non-XMLTYPE system type hits the unsupported arm */

        ASSERT_EQ(nullptr, OCI_TypeInfoGet(con, OTEXT("SYS.ANYDATA"), OCI_TIF_TYPE));
    }
}
