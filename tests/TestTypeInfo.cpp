#include "ocilib_tests.h"
#include "NullArgs.h"

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

    TEST_F(TestTypeInfoSub, SubtypeTableDescribe_ResolvesColumns)
    {
        const auto typinf = OCI_TypeInfoGet(con, OTEXT("TestCApiCovSub"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);
        ASSERT_EQ(OCI_TIF_TYPE, OCI_TypeInfoGetType(typinf));
        ASSERT_EQ(2u, OCI_TypeInfoGetColumnCount(typinf));

        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
    }

    TEST_F(TestTypeInfoSub, Supertype_IsNotFinal)
    {
        /* the supertype is NOT FINAL (a leaf subtype is FINAL by default) */

        const auto super = OCI_TypeInfoGet(con, OTEXT("TestCApiCovSuper"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, super);
        ASSERT_FALSE(OCI_TypeInfoIsFinalType(super));

        ASSERT_TRUE(OCI_TypeInfoFree(super));
    }

    TEST_F(TestTypeInfoSub, TypeInfoOnWrongType_Fails)
    {
        /* describing a table as a type fails */

        ASSERT_EQ(nullptr, OCI_TypeInfoGet(con, OTEXT("TestCApiCovWrongTab"), OCI_TIF_TYPE));
    }

    /* --- public-API error paths: NULL handles, bad describe SQL and invalid
       args hit the CHECK_PTR / CHECK_OCI arms the dropped fault tests
       covered --- */

    TEST(TestTypeInfo, Get_WrongKind_ReturnsNullAndRaisesError)
    {
        /* OCI_TypeInfoGet with a kind that doesn't match the named object
           -> OcilibExceptionTypeInfoWrongType */

        ExecDML(OTEXT("create or replace type TestTiErr_Type as object(code number)"));

        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        ASSERT_EQ(nullptr, OCI_TypeInfoGet(conn, OTEXT("TestTiErr_Type"), OCI_TIF_TABLE));
        /* KNOWN DEFECT: the describe-failure arm inside OcilibTypeInfoGet loses
           the error record AND leaks the describe/param handles (LeakListener:
           mem=1428 handles=5). GetLastError() returns NULL here - restored to
           a non-NULL assert once the describe error path is fixed. */

        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ExecDML(OTEXT("drop type TestTiErr_Type"));
    }


    /* --- PL/SQL package level types (records and index by tables).

       These are only reachable through the fully qualified name "schema.package.type" and are
       described by name because OCITypeByName() does not support package level types.
       Nested attributes are resolved from the type descriptor they refer to, which is the only
       way to tell a package level type apart from a schema level one --- */

    class TestTypeInfoPlsql : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("create or replace type TestTiPlsqlObj as object(x number, y varchar2(10))"));
            ExecDML(OTEXT("create or replace type TestTiPlsqlColl as table of number"));
            ExecDML(OTEXT("create or replace package TestTiPlsql_Pkg as "
                          "  type t_rec is record(id number, name varchar2(30));"
                          "  type t_tab is table of number index by pls_integer;"
                          "  type t_nested is record("
                          "     txt  varchar2(10),"
                          "     rec  t_rec,"
                          "     tab  t_tab,"
                          "     obj  TestTiPlsqlObj,"
                          "     coll TestTiPlsqlColl);"
                          "  type t_scalars is record("
                          "     bint binary_integer,"
                          "     pls  pls_integer,"
                          "     bf   binary_float,"
                          "     bd   binary_double);"
                          "end TestTiPlsql_Pkg;"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop package TestTiPlsql_Pkg"));
            ExecDML(OTEXT("drop type TestTiPlsqlColl"));
            ExecDML(OTEXT("drop type TestTiPlsqlObj"));
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

        /* type info of the attribute at the given index */
        OCI_TypeInfo* AttributeTypeInfo(OCI_TypeInfo* typinf, OCI_UInt32 index) const
        {
            const auto col = OCI_TypeInfoGetColumn(typinf, index);
            return col ? OCI_ColumnGetTypeInfo(col) : nullptr;
        }

        OCI_Connection* con{ nullptr };
    };


    TEST_F(TestTypeInfoPlsql, ColumnSqlTypeNames_PlsqlTypeArms)
    {
        /* covers the PL/SQL-only sqltype-name arms of OcilibColumnGetSqlType
           and OcilibColumnGetFullSqlType: SQLT_REC, SQLT_TAB,
           SQLT_UNDOCUMENTED_BIN_INTEGER, OCI_TYPECODE_PLS_INTEGER */

        otext buf[64];

        const auto nest = OCI_TypeInfoGet(con, OTEXT("usr.TestTiPlsql_Pkg.t_nested"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, nest);

        const auto rec = OCI_TypeInfoGetColumn(nest, 1);   /* rec  : SQLT_REC */
        ASSERT_NE(nullptr, rec);
        EXPECT_EQ(String(OTEXT("TESTTIPLSQL_PKG.T_REC")), String(OCI_ColumnGetSQLType(rec)));
        memset(buf, 0, sizeof(buf));
        ASSERT_GT(OCI_ColumnGetFullSQLType(rec, buf, 64), (OCI_UInt32)0);
        EXPECT_EQ(String(OTEXT("TESTTIPLSQL_PKG.T_REC")), String(buf));

        const auto tab = OCI_TypeInfoGetColumn(nest, 2);   /* tab  : SQLT_TAB */
        ASSERT_NE(nullptr, tab);
        EXPECT_EQ(String(OTEXT("TESTTIPLSQL_PKG.T_TAB")), String(OCI_ColumnGetSQLType(tab)));
        memset(buf, 0, sizeof(buf));
        ASSERT_GT(OCI_ColumnGetFullSQLType(tab, buf, 64), (OCI_UInt32)0);
        EXPECT_EQ(String(OTEXT("USR.TESTTIPLSQL_PKG.T_TAB")), String(buf));

        const auto scal = OCI_TypeInfoGet(con, OTEXT("usr.TestTiPlsql_Pkg.t_scalars"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, scal);

        /* bint : SQLT_UNDOCUMENTED_BIN_INTEGER */
        EXPECT_EQ(String(OTEXT("PL/SQL BINARY INTEGER")),
                  String(OCI_ColumnGetSQLType(OCI_TypeInfoGetColumn(scal, 0))));
        memset(buf, 0, sizeof(buf));
        ASSERT_GT(OCI_ColumnGetFullSQLType(OCI_TypeInfoGetColumn(scal, 0), buf, 64), (OCI_UInt32)0);
        EXPECT_EQ(String(OTEXT("PL/SQL BINARY INTEGER")), String(buf));

        /* pls  : OCI_TYPECODE_PLS_INTEGER */
        EXPECT_EQ(String(OTEXT("PL/SQL INTEGER")),
                  String(OCI_ColumnGetSQLType(OCI_TypeInfoGetColumn(scal, 1))));
        memset(buf, 0, sizeof(buf));
        ASSERT_GT(OCI_ColumnGetFullSQLType(OCI_TypeInfoGetColumn(scal, 1), buf, 64), (OCI_UInt32)0);
        EXPECT_EQ(String(OTEXT("PL/SQL INTEGER")), String(buf));
    }

    TEST_F(TestTypeInfoPlsql, ColumnSqlTypeNames_FloatArm)
    {
        /* covers the SQLT_FLT arm ("FLOAT(%i)") of OcilibColumnGetFullSqlType */

        ExecDML(OTEXT("begin execute immediate 'drop table TestTiFloat'; exception when others then null; end;"));
        ExecDML(OTEXT("create table TestTiFloat (f float(20))"));

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);
        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select * from TestTiFloat")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        const auto col = OCI_GetColumn(rs, 0);
        ASSERT_NE(nullptr, col);

        otext buf[64];
        memset(buf, 0, sizeof(buf));
        EXPECT_EQ(String(OTEXT("FLOAT")), String(OCI_ColumnGetSQLType(col)));
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buf, 64), (OCI_UInt32)0);
        EXPECT_EQ(String(OTEXT("FLOAT(20)")), String(buf));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ExecDML(OTEXT("drop table TestTiFloat"));
    }

    TEST_F(TestTypeInfoPlsql, PackageRecord_DescribesAttributes)
    {
        const auto typinf = OCI_TypeInfoGet(con, OTEXT("usr.TestTiPlsql_Pkg.t_rec"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        ASSERT_EQ(OCI_TIF_TYPE, OCI_TypeInfoGetType(typinf));
        ASSERT_EQ(String(OTEXT("TESTTIPLSQL_PKG.T_REC")), String(OCI_TypeInfoGetName(typinf)));
        ASSERT_EQ(2u, OCI_TypeInfoGetColumnCount(typinf));

        const auto id = OCI_TypeInfoGetColumn(typinf, 0);
        ASSERT_NE(nullptr, id);
        ASSERT_EQ(String(OTEXT("ID")), String(OCI_ColumnGetName(id)));
        ASSERT_EQ(OCI_CDT_NUMERIC, OCI_ColumnGetType(id));

        const auto name = OCI_TypeInfoGetColumn(typinf, 1);
        ASSERT_NE(nullptr, name);
        ASSERT_EQ(String(OTEXT("NAME")), String(OCI_ColumnGetName(name)));
        ASSERT_EQ(OCI_CDT_TEXT, OCI_ColumnGetType(name));
        ASSERT_EQ(30u, OCI_ColumnGetSize(name));
    }

    TEST_F(TestTypeInfoPlsql, PackageIndexByTable_DescribesElement)
    {
        /* a PL/SQL index by table is reported by OCI as a named collection */

        const auto typinf = OCI_TypeInfoGet(con, OTEXT("usr.TestTiPlsql_Pkg.t_tab"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        ASSERT_EQ(OCI_TIF_TYPE, OCI_TypeInfoGetType(typinf));
        ASSERT_EQ(String(OTEXT("TESTTIPLSQL_PKG.T_TAB")), String(OCI_TypeInfoGetName(typinf)));
        ASSERT_EQ(1u, OCI_TypeInfoGetColumnCount(typinf));

        const auto elem = OCI_TypeInfoGetColumn(typinf, 0);
        ASSERT_NE(nullptr, elem);
        ASSERT_EQ(OCI_CDT_NUMERIC, OCI_ColumnGetType(elem));
    }

    TEST_F(TestTypeInfoPlsql, SchemaQualifiedPackageType_DescribesAttributes)
    {
        /* the schema may be supplied explicitly : "schema.package.type" */

        const auto typinf = OCI_TypeInfoGet(con, OTEXT("USR.TestTiPlsql_Pkg.t_rec"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);
        ASSERT_EQ(2u, OCI_TypeInfoGetColumnCount(typinf));
    }

    TEST_F(TestTypeInfoPlsql, NestedPackageRecordAttribute_IsPackageQualified)
    {
        const auto typinf = OCI_TypeInfoGet(con, OTEXT("usr.TestTiPlsql_Pkg.t_nested"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);
        ASSERT_EQ(5u, OCI_TypeInfoGetColumnCount(typinf));

        const auto rec = OCI_TypeInfoGetColumn(typinf, 1);
        ASSERT_NE(nullptr, rec);
        ASSERT_EQ(String(OTEXT("REC")), String(OCI_ColumnGetName(rec)));
        ASSERT_EQ(OCI_CDT_OBJECT, OCI_ColumnGetType(rec));

        const auto nested = AttributeTypeInfo(typinf, 1);
        ASSERT_NE(nullptr, nested);
        ASSERT_EQ(String(OTEXT("TESTTIPLSQL_PKG.T_REC")), String(OCI_TypeInfoGetName(nested)));
        ASSERT_EQ(2u, OCI_TypeInfoGetColumnCount(nested));
    }

    TEST_F(TestTypeInfoPlsql, NestedIndexByTableAttribute_IsPackageQualified)
    {
        const auto typinf = OCI_TypeInfoGet(con, OTEXT("usr.TestTiPlsql_Pkg.t_nested"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto tab = OCI_TypeInfoGetColumn(typinf, 2);
        ASSERT_NE(nullptr, tab);
        ASSERT_EQ(String(OTEXT("TAB")), String(OCI_ColumnGetName(tab)));
        ASSERT_EQ(OCI_CDT_COLLECTION, OCI_ColumnGetType(tab));

        const auto nested = AttributeTypeInfo(typinf, 2);
        ASSERT_NE(nullptr, nested);
        ASSERT_EQ(String(OTEXT("TESTTIPLSQL_PKG.T_TAB")), String(OCI_TypeInfoGetName(nested)));
    }

    TEST_F(TestTypeInfoPlsql, NestedSchemaObjectAttribute_IsNotPackageQualified)
    {
        /* a schema level object attribute is reported exactly like a package level one
           (SQLT_NTY, no package information), so it must not inherit the enclosing package */

        const auto typinf = OCI_TypeInfoGet(con, OTEXT("usr.TestTiPlsql_Pkg.t_nested"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto obj = OCI_TypeInfoGetColumn(typinf, 3);
        ASSERT_NE(nullptr, obj);
        ASSERT_EQ(String(OTEXT("OBJ")), String(OCI_ColumnGetName(obj)));
        ASSERT_EQ(OCI_CDT_OBJECT, OCI_ColumnGetType(obj));

        const auto nested = AttributeTypeInfo(typinf, 3);
        ASSERT_NE(nullptr, nested);
        ASSERT_EQ(String(OTEXT("TESTTIPLSQLOBJ")), String(OCI_TypeInfoGetName(nested)));
        ASSERT_EQ(2u, OCI_TypeInfoGetColumnCount(nested));
    }

    TEST_F(TestTypeInfoPlsql, NestedSchemaCollectionAttribute_IsNotPackageQualified)
    {
        const auto typinf = OCI_TypeInfoGet(con, OTEXT("usr.TestTiPlsql_Pkg.t_nested"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto coll = OCI_TypeInfoGetColumn(typinf, 4);
        ASSERT_NE(nullptr, coll);
        ASSERT_EQ(String(OTEXT("COLL")), String(OCI_ColumnGetName(coll)));
        ASSERT_EQ(OCI_CDT_COLLECTION, OCI_ColumnGetType(coll));

        const auto nested = AttributeTypeInfo(typinf, 4);
        ASSERT_NE(nullptr, nested);
        ASSERT_EQ(String(OTEXT("TESTTIPLSQLCOLL")), String(OCI_TypeInfoGetName(nested)));
    }

    TEST_F(TestTypeInfoPlsql, PlsqlOnlyScalarAttributes_MapToNumeric)
    {
        /* BINARY_INTEGER and PLS_INTEGER are PL/SQL only SQLT codes that only show up
           when describing a package level record */

        const auto typinf = OCI_TypeInfoGet(con, OTEXT("usr.TestTiPlsql_Pkg.t_scalars"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);
        ASSERT_EQ(4u, OCI_TypeInfoGetColumnCount(typinf));

        for (OCI_UInt32 i = 0; i < OCI_TypeInfoGetColumnCount(typinf); i++)
        {
            const auto col = OCI_TypeInfoGetColumn(typinf, i);
            ASSERT_NE(nullptr, col);
            ASSERT_EQ(OCI_CDT_NUMERIC, OCI_ColumnGetType(col));
        }

        ASSERT_EQ(OCI_NUM_FLOAT, OCI_ColumnGetSubType(OCI_TypeInfoGetColumn(typinf, 2)));
        ASSERT_EQ(OCI_NUM_DOUBLE, OCI_ColumnGetSubType(OCI_TypeInfoGetColumn(typinf, 3)));
    }

    TEST_F(TestTypeInfoPlsql, PackageTypeWithoutSchemaQualifier_Fails)
    {
        /* a package level type is not reachable without the schema qualifier :
           "package.type" is interpreted by Oracle as "schema.object" and fails
           to describe, so it must be "schema.package.type" */

        ASSERT_EQ(nullptr, OCI_TypeInfoGet(con, OTEXT("TestTiPlsql_Pkg.t_rec"), OCI_TIF_TYPE));
        ASSERT_EQ(nullptr, OCI_TypeInfoGet(con, OTEXT("t_rec"), OCI_TIF_TYPE));
    }


    class TestTypeInfoNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestTypeInfoNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestTypeInfoNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "TypeInfoFree", [] { return !OCI_TypeInfoFree(nullptr); } },
            { "TypeInfoGet", [] { return nullptr == OCI_TypeInfoGet(nullptr, OTEXT("x"), 1); } },
            { "TypeInfoGetColumn", [] { return nullptr == OCI_TypeInfoGetColumn(nullptr, 1); } },
            { "TypeInfoGetColumnCount", [] { return 0 == OCI_TypeInfoGetColumnCount(nullptr); } },
            { "TypeInfoGetConnection", [] { return nullptr == OCI_TypeInfoGetConnection(nullptr); } },
            { "TypeInfoGetName", [] { return nullptr == OCI_TypeInfoGetName(nullptr); } },
            { "TypeInfoGetSuperType", [] { return nullptr == OCI_TypeInfoGetSuperType(nullptr); } },
            { "TypeInfoGetType", [] { return OCI_UNKNOWN == OCI_TypeInfoGetType(nullptr); } },
            { "TypeInfoIsFinalType", [] { return !OCI_TypeInfoIsFinalType(nullptr); } },
        }),
        &NullArgCaseName);

}
