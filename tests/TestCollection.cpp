#include "ocilib_tests.h"

using namespace ocilib;

namespace TestCApi
{
    void SelectCollection(String sql)
    {
        const OCI_Int32 expectedRows = 2;
        const OCI_Int32 expectedItems = 2;

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, sql.data()));

        const auto rset = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rset);

        OCI_Int32 nbRows = 0;
        OCI_Int32 itemIndex = 0;

        while (OCI_FetchNext(rset))
        {
            nbRows++;

            String category = OTEXT("category ") + TO_STRING(nbRows);
            ASSERT_EQ(category, String(OCI_GetString(rset, 1)));

            const auto coll = OCI_GetColl(rset, 2);
            ASSERT_NE(nullptr, coll);

            ASSERT_EQ(expectedItems, OCI_CollGetCount(coll));
            ASSERT_EQ(expectedItems, OCI_CollGetSize(coll));

            const auto iter = OCI_IterCreate(coll);
            ASSERT_NE(nullptr, iter);

            auto elem = OCI_IterGetNext(iter);
            ASSERT_NE(nullptr, elem);

            while (elem)
            {
                itemIndex++;

                const auto obj = OCI_ElemGetObject(elem);
                ASSERT_NE(nullptr, obj);

                String itemName = OTEXT("name ") + TO_STRING(itemIndex);

                ASSERT_EQ(itemIndex, OCI_ObjectGetInt(obj, OTEXT("code")));
                ASSERT_EQ(itemName, OCI_ObjectGetString(obj, OTEXT("name")));
                elem = OCI_IterGetNext(iter);
            }
            
            ASSERT_TRUE(OCI_IterFree(iter));
        }

        ASSERT_EQ(expectedRows, nbRows);
        ASSERT_EQ(expectedRows * expectedItems, itemIndex);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    void BindOutCollection(String type, String sql)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        const auto coll = OCI_CollCreate(OCI_TypeInfoGet(conn, type.data(), OCI_TIF_TYPE));
        ASSERT_NE(nullptr, coll);

        ASSERT_TRUE(OCI_Prepare(stmt, sql.data()));
        ASSERT_NE(nullptr, OCI_BindColl(stmt, OTEXT(":coll"), coll));
        ASSERT_TRUE(OCI_Execute(stmt));

        const OCI_Int32 expectedItems = 3;

        OCI_Int32 itemIndex = 0;

        ASSERT_EQ(expectedItems, OCI_CollGetCount(coll));
        ASSERT_EQ(expectedItems, OCI_CollGetSize(coll));

        const auto iter = OCI_IterCreate(coll);
        ASSERT_NE(nullptr, iter);

        auto elem = OCI_IterGetNext(iter);
        ASSERT_NE(nullptr, elem);

        while (elem)
        {
            itemIndex++;

            const auto obj = OCI_ElemGetObject(elem);
            ASSERT_NE(nullptr, obj);

            String itemName = OTEXT("name ") + TO_STRING(itemIndex);

            ASSERT_EQ(itemIndex, OCI_ObjectGetInt(obj, OTEXT("code")));
            ASSERT_EQ(itemName, OCI_ObjectGetString(obj, OTEXT("name")));
            elem = OCI_IterGetNext(iter);
        }

        ASSERT_EQ(expectedItems, itemIndex);

        ASSERT_TRUE(OCI_IterFree(iter));
        ASSERT_TRUE(OCI_CollFree(coll));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestCollection, SelectVarray)
    {
        ExecDML(OTEXT("create type SelectVarray_Type as object(code int, name varchar2(50))"));
        ExecDML(OTEXT("create type SelectVarray_Coll as varray(2) of SelectVarray_Type"));
        ExecDML(OTEXT("create table SelectVarray_Table(category varchar2(50), products SelectVarray_Coll)"));
        ExecDML(OTEXT("truncate table SelectVarray_Table"));
        ExecDML(OTEXT("insert into SelectVarray_Table values('category 1', SelectVarray_Coll(SelectVarray_Type(1, 'name 1'), SelectVarray_Type(2, 'name 2')))"));
        ExecDML(OTEXT("insert into SelectVarray_Table values('category 2', SelectVarray_Coll(SelectVarray_Type(3, 'name 3'), SelectVarray_Type(4, 'name 4')))"));

        SelectCollection(OTEXT("SELECT * from SelectVarray_Table order by category"));

        ExecDML(OTEXT("drop table SelectVarray_Table"));
        ExecDML(OTEXT("drop type SelectVarray_Coll"));
        ExecDML(OTEXT("drop type SelectVarray_Type"));
    }

    TEST(TestCollection, SelectNestedTable)
    {
        ExecDML(OTEXT("create type SelectNestedTable_Type as object(code int, name varchar2(50))"));
        ExecDML(OTEXT("create type SelectNestedTable_Coll as table of SelectNestedTable_Type"));
        ExecDML(OTEXT("create table SelectNestedTable_Table(category varchar2(50), products SelectNestedTable_Coll) nested table products store as SelectNestedTable_Storage"));
        ExecDML(OTEXT("truncate table SelectNestedTable_Table"));
        ExecDML(OTEXT("insert into SelectNestedTable_Table values('category 1', SelectNestedTable_Coll(SelectNestedTable_Type(1, 'name 1'), SelectNestedTable_Type(2, 'name 2')))"));
        ExecDML(OTEXT("insert into SelectNestedTable_Table values('category 2', SelectNestedTable_Coll(SelectNestedTable_Type(3, 'name 3'), SelectNestedTable_Type(4, 'name 4')))"));

        SelectCollection(OTEXT("SELECT * from SelectNestedTable_Table order by category"));

        ExecDML(OTEXT("drop table SelectNestedTable_Table"));
        ExecDML(OTEXT("drop type SelectNestedTable_Coll"));
        ExecDML(OTEXT("drop type SelectNestedTable_Type"));
    }

    TEST(TestCollection, BindOutVarray)
    {
        ExecDML(OTEXT("create type BindOutVarray_Type as object(code int, name varchar2(50))"));
        ExecDML(OTEXT("create type BindOutVarray_Coll as varray(3) of BindOutVarray_Type"));

        BindOutCollection
        (
            OTEXT("BindOutVarray_Coll"),
            OTEXT("begin                                            ")
            OTEXT("  :coll := BindOutVarray_Coll                        ")
            OTEXT("            (                                        ")
            OTEXT("                 BindOutVarray_Type(1, 'name 1'),    ")
            OTEXT("                 BindOutVarray_Type(2, 'name 2'),    ")
            OTEXT("                 BindOutVarray_Type(3, 'name 3')     ")
            OTEXT("            );                                       ")
            OTEXT("end;                                                 ")
        );

        ExecDML(OTEXT("drop type BindOutVarray_Coll"));
        ExecDML(OTEXT("drop type BindOutVarray_Type"));
    }
    TEST(TestCollection, BindOutNestedTable)
    {
        ExecDML(OTEXT("create type BindOutNestedTable_Type as object(code int, name varchar2(50))"));
        ExecDML(OTEXT("create type BindOutNestedTable_Coll as table of BindOutNestedTable_Type"));


        BindOutCollection
        (
            OTEXT("BindOutNestedTable_Coll"),
            OTEXT("begin                                                ")
            OTEXT("  :coll := BindOutNestedTable_Coll                       ")
            OTEXT("            (                                            ")
            OTEXT("                 BindOutNestedTable_Type(1, 'name 1'),   ")
            OTEXT("                 BindOutNestedTable_Type(2, 'name 2'),   ")
            OTEXT("                 BindOutNestedTable_Type(3, 'name 3')    ")
            OTEXT("            );                                           ")
            OTEXT("end;                                                     ")
        );

        ExecDML(OTEXT("drop type BindOutNestedTable_Coll"));
        ExecDML(OTEXT("drop type BindOutNestedTable_Type"));
    }

    TEST(TestCollection, NumberVarrayAccess)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCApiCovVarray AS VARRAY(10) OF NUMBER"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestCApiCovVarray(1, 2, 3) FROM DUAL")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 1);
        ASSERT_NE(nullptr, coll);

        ASSERT_EQ(3u, OCI_CollGetSize(coll));

        auto iter = OCI_IterCreate(coll);
        ASSERT_NE(nullptr, iter);

        OCI_Int32 expected = 1;
        for (auto elem = OCI_IterGetNext(iter); elem != nullptr; elem = OCI_IterGetNext(iter), ++expected)
        {
            ASSERT_EQ(expected, OCI_ElemGetInt(elem));
        }
        ASSERT_EQ(4, expected);

        ASSERT_TRUE(OCI_IterFree(iter));

        auto direct = OCI_CollGetElem(coll, 2);
        ASSERT_NE(nullptr, direct);
        ASSERT_EQ(2, OCI_ElemGetInt(direct));
        ASSERT_FALSE(OCI_ElemIsNull(direct));

        otext collText[512];
        OCI_UInt32 collTextSize = 512;
        ASSERT_TRUE(OCI_CollToText(coll, &collTextSize, collText));
        ASSERT_GT(collTextSize, 0u);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TYPE TestCApiCovVarray"));
    }

    TEST(TestCollection, StringVarrayAccess)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCApiCovStrVarray AS VARRAY(10) OF VARCHAR2(50)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestCApiCovStrVarray('first', 'second') FROM DUAL")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 1);
        ASSERT_NE(nullptr, coll);

        otext collText[512];
        OCI_UInt32 collTextSize = 512;
        ASSERT_TRUE(OCI_CollToText(coll, &collTextSize, collText));
        ASSERT_GT(collTextSize, 0u);

        auto elem = OCI_CollGetElem(coll, 1);
        ASSERT_NE(nullptr, elem);
        const otext* str = OCI_ElemGetString(elem);
        ASSERT_NE(nullptr, str);
        ASSERT_EQ(String(OTEXT("first")), String(str));

        auto typinf = OCI_TypeInfoGet(con, OTEXT("TestCApiCovStrVarray"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);
        auto local = OCI_ElemCreate(typinf);
        ASSERT_NE(nullptr, local);
        ASSERT_TRUE(OCI_ElemSetString(local, OTEXT("local value")));
        ASSERT_EQ(String(OTEXT("local value")), String(OCI_ElemGetString(local)));
        ASSERT_TRUE(OCI_ElemSetNull(local));
        ASSERT_TRUE(OCI_ElemIsNull(local));
        ASSERT_TRUE(OCI_ElemFree(local));

        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TYPE TestCApiCovStrVarray"));
    }

    TEST(TestCollection, NumberVarrayCrud)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCollCrud AS VARRAY(10) OF NUMBER"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto typinf = OCI_TypeInfoGet(con, OTEXT("TestCollCrud"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto coll = OCI_CollCreate(typinf);
        ASSERT_NE(nullptr, coll);

        ASSERT_EQ((OCI_UInt32) OCI_COLL_VARRAY, OCI_CollGetType(coll));
        ASSERT_EQ(10u, OCI_CollGetMax(coll));
        ASSERT_EQ(0u, OCI_CollGetSize(coll));
        ASSERT_EQ(0u, OCI_CollGetCount(coll));
        ASSERT_EQ(typinf, OCI_CollGetTypeInfo(coll));

        const auto elem = OCI_ElemCreate(typinf);
        ASSERT_NE(nullptr, elem);

        ASSERT_TRUE(OCI_ElemSetInt(elem, 10));
        ASSERT_TRUE(OCI_CollAppend(coll, elem));
        ASSERT_EQ(1u, OCI_CollGetSize(coll));
        ASSERT_EQ(1u, OCI_CollGetCount(coll));

        ASSERT_TRUE(OCI_ElemSetInt(elem, 20));
        ASSERT_TRUE(OCI_CollAppend(coll, elem));
        ASSERT_EQ(2u, OCI_CollGetSize(coll));
        ASSERT_EQ(2u, OCI_CollGetCount(coll));

        auto e1 = OCI_CollGetElem(coll, 1);
        ASSERT_NE(nullptr, e1);
        ASSERT_EQ(10, OCI_ElemGetInt(e1));

        auto e2 = OCI_CollGetElem(coll, 2);
        ASSERT_NE(nullptr, e2);
        ASSERT_EQ(20, OCI_ElemGetInt(e2));

        ASSERT_TRUE(OCI_ElemSetInt(elem, 30));
        ASSERT_TRUE(OCI_CollSetElem(coll, 1, elem));
        e1 = OCI_CollGetElem(coll, 1);
        ASSERT_EQ(30, OCI_ElemGetInt(e1));

        auto elemRef = OCI_CollGetElem(coll, 1);
        ASSERT_NE(nullptr, elemRef);
        elemRef = OCI_CollGetElem(coll, 2);
        ASSERT_EQ(20, OCI_ElemGetInt(elemRef));

        const auto coll2 = OCI_CollCreate(typinf);
        ASSERT_NE(nullptr, coll2);
        ASSERT_TRUE(OCI_CollAssign(coll2, coll));
        ASSERT_EQ(2u, OCI_CollGetCount(coll2));
        ASSERT_EQ(20, OCI_ElemGetInt(OCI_CollGetElem(coll2, 2)));

        otext text[512];
        OCI_UInt32 size = 512;
        ASSERT_TRUE(OCI_CollToText(coll, &size, text));
        ASSERT_GT(size, 0u);

        ASSERT_TRUE(OCI_CollDeleteElem(coll, 1));
        ASSERT_EQ(2u, OCI_CollGetCount(coll));

        ASSERT_TRUE(OCI_CollTrim(coll, 1));
        ASSERT_EQ(1u, OCI_CollGetCount(coll));

        ASSERT_TRUE(OCI_CollClear(coll));
        ASSERT_EQ(0u, OCI_CollGetCount(coll));

        const auto colls = OCI_CollArrayCreate(typinf, 2);
        ASSERT_NE(nullptr, colls);
        ASSERT_NE(nullptr, colls[0]);
        ASSERT_NE(nullptr, colls[1]);
        ASSERT_TRUE(OCI_CollArrayFree(colls));

        ASSERT_TRUE(OCI_CollFree(coll2));
        ASSERT_TRUE(OCI_ElemFree(elem));
        ASSERT_TRUE(OCI_CollFree(coll));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TYPE TestCollCrud"));
    }

    TEST(TestCollection, NestedTableCrud)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCollNested AS TABLE OF NUMBER"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto typinf = OCI_TypeInfoGet(con, OTEXT("TestCollNested"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto coll = OCI_CollCreate(typinf);
        ASSERT_NE(nullptr, coll);

        ASSERT_EQ((OCI_UInt32) OCI_COLL_NESTED_TABLE, OCI_CollGetType(coll));

        const auto elem = OCI_ElemCreate(typinf);
        ASSERT_NE(nullptr, elem);

        ASSERT_TRUE(OCI_ElemSetInt(elem, 1));
        ASSERT_TRUE(OCI_CollAppend(coll, elem));
        ASSERT_TRUE(OCI_ElemSetInt(elem, 2));
        ASSERT_TRUE(OCI_CollAppend(coll, elem));
        ASSERT_TRUE(OCI_ElemSetInt(elem, 3));
        ASSERT_TRUE(OCI_CollAppend(coll, elem));

        ASSERT_EQ(3u, OCI_CollGetCount(coll));

        ASSERT_TRUE(OCI_CollDeleteElem(coll, 2));
        ASSERT_LT(OCI_CollGetCount(coll), 3u);

        ASSERT_TRUE(OCI_CollTrim(coll, 1));
        ASSERT_GT(OCI_CollGetCount(coll), 0u);

        ASSERT_TRUE(OCI_CollClear(coll));
        ASSERT_EQ(0u, OCI_CollGetCount(coll));

        ASSERT_TRUE(OCI_ElemFree(elem));
        ASSERT_TRUE(OCI_CollFree(coll));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TYPE TestCollNested"));
    }

    TEST(TestCollection, NullElementToText)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCollNull AS VARRAY(2) OF NUMBER"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestCollNull(NULL, 42) FROM DUAL")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 1);
        ASSERT_NE(nullptr, coll);

        otext text[512];
        OCI_UInt32 size = 512;
        ASSERT_TRUE(OCI_CollToText(coll, &size, text));
        ASSERT_GT(size, 0u);
        ASSERT_NE(String::npos, String(text).find(OTEXT("NULL")));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TYPE TestCollNull"));
    }

    TEST(TestCollection, ToTextWithNullString)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCollNullStr AS VARRAY(2) OF VARCHAR2(50)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestCollNullStr('first', NULL) FROM DUAL")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 1);
        ASSERT_NE(nullptr, coll);

        otext text[512];
        OCI_UInt32 size = 512;
        ASSERT_TRUE(OCI_CollToText(coll, &size, text));
        ASSERT_GT(size, 0u);
        ASSERT_NE(String::npos, String(text).find(OTEXT("NULL")));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TYPE TestCollNullStr"));
    }

    TEST(TestCollection, ToTextWithRaw)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCollRaw AS VARRAY(2) OF RAW(10)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestCollRaw(HEXTORAW('AABBCC'), HEXTORAW('DDEEFF')) FROM DUAL")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 1);
        ASSERT_NE(nullptr, coll);

        otext text[512];
        OCI_UInt32 size = 512;
        ASSERT_TRUE(OCI_CollToText(coll, &size, text));
        ASSERT_GT(size, 0u);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TYPE TestCollRaw"));
    }

    TEST(TestCollection, ToTextWithDate)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCollDate AS VARRAY(2) OF DATE"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestCollDate(TO_DATE('2023-01-01','YYYY-MM-DD'), TO_DATE('2023-12-31','YYYY-MM-DD')) FROM DUAL")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 1);
        ASSERT_NE(nullptr, coll);

        otext text[512];
        OCI_UInt32 size = 512;
        ASSERT_TRUE(OCI_CollToText(coll, &size, text));
        ASSERT_GT(size, 0u);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TYPE TestCollDate"));
    }

    TEST(TestCollection, ToTextWithTimestamp)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCollTs AS VARRAY(2) OF TIMESTAMP"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestCollTs(TO_TIMESTAMP('2023-01-01 12:34:56','YYYY-MM-DD HH24:MI:SS'), TO_TIMESTAMP('2023-12-31 23:59:59','YYYY-MM-DD HH24:MI:SS')) FROM DUAL")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 1);
        ASSERT_NE(nullptr, coll);

        otext text[512];
        OCI_UInt32 size = 512;
        ASSERT_TRUE(OCI_CollToText(coll, &size, text));
        ASSERT_GT(size, 0u);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TYPE TestCollTs"));
    }

    TEST(TestCollection, ToTextWithInterval)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCollItv AS VARRAY(2) OF INTERVAL DAY TO SECOND"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestCollItv(INTERVAL '1 12:30:00' DAY TO SECOND, INTERVAL '2 00:00:00' DAY TO SECOND) FROM DUAL")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 1);
        ASSERT_NE(nullptr, coll);

        otext text[512];
        OCI_UInt32 size = 512;
        ASSERT_TRUE(OCI_CollToText(coll, &size, text));
        ASSERT_GT(size, 0u);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TYPE TestCollItv"));
    }

    struct CollToTextCase
    {
        const otext* sql;
    };

    class TestCollectionToText : public ::testing::TestWithParam<CollToTextCase>
    {
    public:

        /* public: TEST_P fixtures require access in gtest 1.8 */

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCollObj AS OBJECT(code int)"));
            ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCollInner AS TABLE OF NUMBER"));
            ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCollLobC AS VARRAY(2) OF CLOB"));
            ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCollFileC AS VARRAY(2) OF BFILE"));
            ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCollRefC AS VARRAY(2) OF REF TestCollObj"));
            ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCollNestC AS VARRAY(2) OF TestCollInner"));
            ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCollBoolC AS VARRAY(2) OF BOOLEAN"));
            ExecDML(OTEXT("CREATE TABLE TestCollObjTab OF TestCollObj"));
            ExecDML(OTEXT("INSERT INTO TestCollObjTab VALUES (1)"));
            ExecDML(OTEXT("COMMIT"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("DROP TABLE TestCollObjTab"));
            ExecDML(OTEXT("DROP TYPE TestCollBoolC"));
            ExecDML(OTEXT("DROP TYPE TestCollNestC"));
            ExecDML(OTEXT("DROP TYPE TestCollRefC"));
            ExecDML(OTEXT("DROP TYPE TestCollFileC"));
            ExecDML(OTEXT("DROP TYPE TestCollLobC"));
            ExecDML(OTEXT("DROP TYPE TestCollInner"));
            ExecDML(OTEXT("DROP TYPE TestCollObj FORCE"));
        }
    };

    TEST_P(TestCollectionToText, RendersElementType)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        if (OCI_ExecuteStmt(stmt, GetParam().sql))
        {
            const auto rs = OCI_GetResultset(stmt);
            ASSERT_NE(nullptr, rs);
            ASSERT_TRUE(OCI_FetchNext(rs));

            const auto coll = OCI_GetColl(rs, 1);
            ASSERT_NE(nullptr, coll);

            otext text[1024];
            OCI_UInt32 size = sizeof(text) / sizeof(otext);
            ASSERT_TRUE(OCI_CollToText(coll, &size, text));
            ASSERT_GT(size, 0u);
        }

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));
    }

    INSTANTIATE_TEST_CASE_P(ElementTypes, TestCollectionToText,
        ::testing::Values(
            CollToTextCase{ OTEXT("SELECT TestCollLobC(TO_CLOB('x'), EMPTY_CLOB()) FROM DUAL") },
            CollToTextCase{ OTEXT("SELECT TestCollFileC(BFILENAME('FAKE_DIR', 'a.txt'), NULL) FROM DUAL") },
            CollToTextCase{ OTEXT("SELECT TestCollRefC(REF(o), NULL) FROM TestCollObjTab o") },
            CollToTextCase{ OTEXT("SELECT TestCollNestC(TestCollInner(1, 2), NULL) FROM DUAL") },
            CollToTextCase{ OTEXT("SELECT TestCollBoolC(TRUE, FALSE) FROM DUAL") },
            CollToTextCase{ OTEXT("SELECT TestCollInner(NULL) FROM DUAL") }));

    /* element flagged not null but without data goes through the
       null literal arm of the element value rendering */

    TEST_F(TestCollectionToText, RendersNullDataElement)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto typinf = OCI_TypeInfoGet(con, OTEXT("TestCollInner"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto coll = OCI_CollCreate(typinf);
        ASSERT_NE(nullptr, coll);

        const auto elem = OCI_ElemCreate(typinf);
        ASSERT_NE(nullptr, elem);

        ASSERT_TRUE(OCI_ElemSetNull(elem));
        ASSERT_TRUE(OCI_CollAppend(coll, elem));

        otext text[512];
        OCI_UInt32 size = sizeof(text) / sizeof(otext);
        ASSERT_TRUE(OCI_CollToText(coll, &size, text));
        ASSERT_NE(String(text).find(OTEXT("NULL")), String::npos);

        ASSERT_TRUE(OCI_ElemFree(elem));
        ASSERT_TRUE(OCI_CollFree(coll));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(con));
    }
}