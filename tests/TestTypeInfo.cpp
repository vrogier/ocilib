#include "ocilib_tests.h"

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

        auto col = OCI_TypeInfoGetColumn(typinf, 0);
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

    /* --- public-API error paths: NULL handles, bad describe SQL and invalid
       args hit the CHECK_PTR / CHECK_OCI arms the dropped fault tests
       covered --- */



}
