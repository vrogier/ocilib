#include "ocilib_tests.h"

using namespace ocilib;

namespace TestCApi
{
    void SelectCollection(ostring sql)
    {
        const int expectedRows = 2;
        const int expectedItems = 2;

        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, sql.data()));

        const auto rset = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rset);

        int nbRows = 0;
        int itemIndex = 0;

        while (OCI_FetchNext(rset))
        {
            nbRows++;

            ostring category = OTEXT("category ") + TO_STRING(nbRows);
            ASSERT_EQ(category, ostring(OCI_GetString(rset, 1)));

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

                ostring itemName = OTEXT("name ") + TO_STRING(itemIndex);

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
        ASSERT_TRUE(OCI_Cleanup());
    }

    void BindOutCollection(ostring type, ostring sql)
    {
        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        const auto coll = OCI_CollCreate(OCI_TypeInfoGet(conn, type.data(), OCI_TIF_TYPE));
        ASSERT_NE(nullptr, coll);

        ASSERT_TRUE(OCI_Prepare(stmt, sql.data()));
        ASSERT_TRUE(OCI_BindColl(stmt, OTEXT(":coll"), coll));
        ASSERT_TRUE(OCI_Execute(stmt));

        const int expectedItems = 3;

        int itemIndex = 0;

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

            ostring itemName = OTEXT("name ") + TO_STRING(itemIndex);

            ASSERT_EQ(itemIndex, OCI_ObjectGetInt(obj, OTEXT("code")));
            ASSERT_EQ(itemName, OCI_ObjectGetString(obj, OTEXT("name")));
            elem = OCI_IterGetNext(iter);
        }

        ASSERT_EQ(expectedItems, itemIndex);

        ASSERT_TRUE(OCI_IterFree(iter));
        ASSERT_TRUE(OCI_CollFree(coll));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_Cleanup());
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
}

namespace TestCppApi
{
    TEST(TestCollection, ToStringCpp)
    {
        ExecDML(OTEXT("create type CollToStringCpp_Type as object(code int, name varchar2(50))"));
        ExecDML(OTEXT("create type CollToStringCpp_Coll as varray(2) of CollToStringCpp_Type"));

        Environment::Initialize();

        Connection conn(DBS, USR, PWD);
        TypeInfo typinfColl(conn, OTEXT("CollToStringCpp_Coll"), TypeInfo::Type);
        TypeInfo typinfObj(conn, OTEXT("CollToStringCpp_Type"), TypeInfo::Type);
        Collection<Object> coll(typinfColl);

        Object obj1(typinfObj);
        obj1.Set<int>(OTEXT("code"), 1);
        obj1.Set<ostring>(OTEXT("name"), OTEXT("computer"));
        coll.Append(obj1);

        Object obj2(typinfObj);
        obj2.Set<int>(OTEXT("code"), 2);
        obj2.Set<ostring>(OTEXT("name"), OTEXT("book"));
        coll.Append(obj2);

        ostring expected = OTEXT("CollToStringCpp_Coll(CollToStringCpp_Type(1, 'computer'), CollToStringCpp_Type(2, 'book'))");

        ASSERT_EQ(ToUpper(expected), ToUpper(coll.ToString()));

        Environment::Cleanup();

        ExecDML(OTEXT("drop type CollToStringCpp_Coll"));
        ExecDML(OTEXT("drop type CollToStringCpp_Type"));
    }
}

