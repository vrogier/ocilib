#include "ocilib_tests.h"

namespace TestCApi
{
    TEST(TestArray, InsertExternalArray)
    {
        ExecDML(OTEXT("create table TestExternalArrayInsertArray(code int, name varchar2(50))"));
        ExecDML(OTEXT("truncate table TestExternalArrayInsertArray"));

        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        int tab_int[ARRAY_SIZE];
        otext tab_str[ARRAY_SIZE][STRING_SIZE + 1];

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into TestExternalArrayInsertArray values(:i, :s)")));
        ASSERT_TRUE(OCI_BindArraySetSize(stmt, ARRAY_SIZE));
        ASSERT_TRUE(OCI_BindArrayOfInts(stmt, OTEXT(":i"), static_cast<int*>(tab_int), 0));
        ASSERT_TRUE(OCI_BindArrayOfStrings(stmt, OTEXT(":s"), reinterpret_cast<otext*>(tab_str), STRING_SIZE, 0));

        for (int i = 0; i < ARRAY_SIZE; i++)
        {
            tab_int[i] = i + 1;
            osprintf(tab_str[i], OTEXT("Name %d"), i + 1);
        }

        ASSERT_TRUE(OCI_Execute(stmt));

        ASSERT_EQ(ARRAY_SIZE, OCI_GetAffectedRows(stmt));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_Cleanup());

        ExecDML(OTEXT("drop table TestExternalArrayInsertArray"));
    }

    TEST(TestArray, InsertExternalArrayError)
    {
        ExecDML(OTEXT("create table TestExternalArrayInsertArrayError(code int NOT NULL, name varchar2(50))"));
        ExecDML(OTEXT("truncate table TestExternalArrayInsertArrayError"));

        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        int tab_int[ARRAY_SIZE];
        otext tab_str[ARRAY_SIZE][STRING_SIZE + 1];

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into TestExternalArrayInsertArrayError values(:i, :s)")));
        ASSERT_TRUE(OCI_BindArraySetSize(stmt, ARRAY_SIZE));
        ASSERT_TRUE(OCI_BindArrayOfInts(stmt, OTEXT(":i"), static_cast<int*>(tab_int), 0));
        ASSERT_TRUE(OCI_BindArrayOfStrings(stmt, OTEXT(":s"), reinterpret_cast<otext*>(tab_str), STRING_SIZE, 0));

        for (int i = 0; i < ARRAY_SIZE; i++)
        {
            tab_int[i] = i + 1;
            osprintf(tab_str[i], OTEXT("Name %d"), i + 1);
        }

        const auto bnd = OCI_GetBind(stmt, 1);
        ASSERT_NE(nullptr, bnd);

        ASSERT_TRUE(OCI_BindSetNullAtPos(bnd, 3));

        ASSERT_FALSE(OCI_Execute(stmt));

        const auto err = OCI_GetBatchError(stmt);
        ASSERT_NE(nullptr, err);

        // Row 3 must be in error
        ASSERT_EQ(3, OCI_ErrorGetRow(err));
        // Only 1 error
        ASSERT_EQ(nullptr, OCI_GetBatchError(stmt));
        // Only 1 error, ARRAY_SIZE - 1 must have been inserted
        ASSERT_EQ(ARRAY_SIZE - 1, OCI_GetAffectedRows(stmt));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_Cleanup());

        ExecDML(OTEXT("drop table TestExternalArrayInsertArrayError"));
    }

    TEST(TestArray, InsertInternalArray)
    {
        ExecDML(OTEXT("create table TestInternalArrayInsertArray(code int, name varchar2(50), creation date)"));
        ExecDML(OTEXT("truncate table TestInternalArrayInsertArray"));

        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_SetBindAllocation(stmt, OCI_BAM_INTERNAL));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into TestInternalArrayInsertArray values(:i, :s, :d)")));
        ASSERT_TRUE(OCI_BindArraySetSize(stmt, ARRAY_SIZE));
        ASSERT_TRUE(OCI_BindArrayOfInts(stmt, OTEXT(":i"), NULL, 0));
        ASSERT_TRUE(OCI_BindArrayOfStrings(stmt, OTEXT(":s"), NULL, STRING_SIZE, 0));
        ASSERT_TRUE(OCI_BindArrayOfDates(stmt, OTEXT(":d"), NULL, 0));

        auto sysdate = OCI_DateCreate(nullptr);
        ASSERT_NE(nullptr, sysdate);

        auto tab_int = static_cast<int*>(OCI_BindGetData(OCI_GetBind(stmt, 1)));
        ASSERT_NE(nullptr, tab_int);
        auto tab_str = static_cast<otext*>(OCI_BindGetData(OCI_GetBind(stmt, 2)));
        ASSERT_NE(nullptr, tab_str);
        auto tab_date = static_cast<OCI_Date**>(OCI_BindGetData(OCI_GetBind(stmt, 3)));
        ASSERT_NE(nullptr, tab_date);

        for (int i = 0; i < ARRAY_SIZE; i++)
        {
            tab_int[i] = i + 1;
            osprintf(tab_str + (i * (STRING_SIZE + 1)), OTEXT("Name %d"), i + 1);
            ASSERT_TRUE(OCI_DateAssign(tab_date[i], sysdate));
        }

        ASSERT_TRUE(OCI_Execute(stmt));

        ASSERT_EQ(ARRAY_SIZE, OCI_GetAffectedRows(stmt));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select * from TestInternalArrayInsertArray")));

        auto rslt = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rslt);

        int count = 0;
        while (OCI_FetchNext(rslt))
        {
            count++;
            ostring str = OTEXT("Name ") + TO_STRING(count);
            ASSERT_EQ(count, OCI_GetInt(rslt, 1));
            ASSERT_EQ(str, OCI_GetString(rslt, 2));
            ASSERT_EQ(0, OCI_DateCompare(sysdate, OCI_GetDate(rslt, 3)));
        }

        ASSERT_EQ(ARRAY_SIZE, count);

        ASSERT_TRUE(OCI_DateFree(sysdate));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_Cleanup());

        ExecDML(OTEXT("drop table TestInternalArrayInsertArray"));
    }
}