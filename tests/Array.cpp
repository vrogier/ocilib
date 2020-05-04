#include "ocilib_tests.h"

// requires DDL from /demo/product.sql

TEST(TestArray, InsertArray)
{
    ExecDML(OTEXT("create table TestArrayInsertArray(code int, name varchar2(50))"));
    ExecDML(OTEXT("truncate table TestArrayInsertArray(code int, name varchar2(50))"));

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto stmt = OCI_StatementCreate(conn);
    ASSERT_NE(nullptr, stmt);

    int tab_int[ARRAY_SIZE];
    char tab_str[ARRAY_SIZE][STRING_SIZE + 1];

    ASSERT_TRUE(OCI_Prepare(stmt, "insert into TestArrayInsertArray values(:i, :s)"));
    ASSERT_TRUE(OCI_BindArraySetSize(stmt, ARRAY_SIZE));
    ASSERT_TRUE(OCI_BindArrayOfInts(stmt, ":i", static_cast<int*>(tab_int), 0));
    ASSERT_TRUE(OCI_BindArrayOfStrings(stmt, ":s", reinterpret_cast<char*>(tab_str), STRING_SIZE, 0));

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        tab_int[i] = i + 1;
        osprintf(tab_str[i], STRING_SIZE, OTEXT("Name %d"), i + 1);
    }

    ASSERT_TRUE(OCI_Execute(stmt));

    ASSERT_EQ(ARRAY_SIZE, OCI_GetAffectedRows(stmt));

    ASSERT_TRUE(OCI_StatementFree(stmt));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("drop table TestArrayInsertArray"));
}

TEST(TestArray, InsertArrayError)
{
    ExecDML(OTEXT("create table TestArrayInsertArrayError(code int NOT NULL, name varchar2(50))"));
    ExecDML(OTEXT("truncate table TestArrayInsertArrayError(code int, name varchar2(50))"));

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto stmt = OCI_StatementCreate(conn);
    ASSERT_NE(nullptr, stmt);

    int tab_int[ARRAY_SIZE];
    char tab_str[ARRAY_SIZE][STRING_SIZE + 1];

    ASSERT_TRUE(OCI_Prepare(stmt, "insert into TestArrayInsertArrayError values(:i, :s)"));
    ASSERT_TRUE(OCI_BindArraySetSize(stmt, ARRAY_SIZE));
    ASSERT_TRUE(OCI_BindArrayOfInts(stmt, ":i", static_cast<int*>(tab_int), 0));
    ASSERT_TRUE(OCI_BindArrayOfStrings(stmt, ":s", reinterpret_cast<char*>(tab_str), STRING_SIZE, 0));

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        tab_int[i] = i + 1;
        osprintf(tab_str[i], STRING_SIZE, OTEXT("Name %d"), i + 1);
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
    ASSERT_EQ(ARRAY_SIZE -1, OCI_GetAffectedRows(stmt));

    ASSERT_TRUE(OCI_StatementFree(stmt));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("drop table TestArrayInsertArrayError"));
}
