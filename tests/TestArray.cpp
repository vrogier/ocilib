#include "ocilib_tests.h"

namespace TestCApi
{
    TEST(TestArray, InsertExternalArray)
    {
        ExecDML(OTEXT("create table TestExternalArrayInsertArray(code int, name varchar2(50))"));
        ExecDML(OTEXT("truncate table TestExternalArrayInsertArray"));

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        OCI_Int32 tab_int[ARRAY_SIZE];
        otext tab_str[ARRAY_SIZE][STRING_SIZE + 1];

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into TestExternalArrayInsertArray values(:i, :s)")));
        ASSERT_TRUE(OCI_StatementBindArraySetSize(stmt, ARRAY_SIZE));
        ASSERT_NE(nullptr, OCI_StatementBindInt32Array(stmt, OTEXT(":i"), static_cast<OCI_Int32*>(tab_int), 0));
        ASSERT_NE(nullptr, OCI_StatementBindStringArray(stmt, OTEXT(":s"), reinterpret_cast<otext*>(tab_str), STRING_SIZE, 0));

        for (OCI_Int32 i = 0; i < ARRAY_SIZE; i++)
        {
            tab_int[i] = i + 1;
            osprintf(tab_str[i], OTEXT("Name %d"), i + 1);
        }

        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        ASSERT_EQ(ARRAY_SIZE, OCI_StatementGetAffectedRows(stmt));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestExternalArrayInsertArray"));
    }

    TEST(TestArray, InsertExternalArrayError)
    {
        ExecDML(OTEXT("create table TestExternalArrayInsertArrayError(code int NOT NULL, name varchar2(50))"));
        ExecDML(OTEXT("truncate table TestExternalArrayInsertArrayError"));

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        OCI_Int32 tab_int[ARRAY_SIZE];
        otext tab_str[ARRAY_SIZE][STRING_SIZE + 1];

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into TestExternalArrayInsertArrayError values(:i, :s)")));
        ASSERT_TRUE(OCI_StatementBindArraySetSize(stmt, ARRAY_SIZE));
        ASSERT_NE(nullptr, OCI_StatementBindInt32Array(stmt, OTEXT(":i"), static_cast<OCI_Int32*>(tab_int), 0));
        ASSERT_NE(nullptr, OCI_StatementBindStringArray(stmt, OTEXT(":s"), reinterpret_cast<otext*>(tab_str), STRING_SIZE, 0));

        for (OCI_Int32 i = 0; i < ARRAY_SIZE; i++)
        {
            tab_int[i] = i + 1;
            osprintf(tab_str[i], OTEXT("Name %d"), i + 1);
        }

        const auto bnd = OCI_StatementGetBind(stmt, 0);
        ASSERT_NE(nullptr, bnd);

        ASSERT_TRUE(OCI_BindSetNullAtPos(bnd, 3));

        ASSERT_FALSE(OCI_StatementExecutePrepared(stmt));

        const auto err = OCI_StatementGetBatchError(stmt);
        ASSERT_NE(nullptr, err);

        // Row 3 must be in error
        ASSERT_EQ(3, OCI_ErrorGetRow(err));
        // Only 1 error
        ASSERT_EQ(nullptr, OCI_StatementGetBatchError(stmt));
        // Only 1 error, ARRAY_SIZE - 1 must have been inserted
        ASSERT_EQ(ARRAY_SIZE - 1, OCI_StatementGetAffectedRows(stmt));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestExternalArrayInsertArrayError"));
    }

    /* -------------------------------------------------------------------------
       Shared fixture: internal-alloc array binds on the insert table
       ------------------------------------------------------------------------- */

    class TestArrayInt : public ::testing::Test
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_Statement* stmt = nullptr;
        OCI_Date* sysdate = nullptr;

    public:
        static void SetUpTestCase()
        {
            ExecDML(OTEXT("create table TestInternalArrayInsertArray(code int, name varchar2(50), creation date)"));
            ExecDML(OTEXT("truncate table TestInternalArrayInsertArray"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestInternalArrayInsertArray"));
        }

    protected:
        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);
            sysdate = OCI_DateCreate(GetEnvironment(nullptr));
            ASSERT_NE(nullptr, sysdate);
            ASSERT_TRUE(OCI_StatementSetBindAllocation(stmt, OCI_BAM_INTERNAL));
            ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into TestInternalArrayInsertArray values(:i, :s, :d)")));
            ASSERT_TRUE(OCI_StatementBindArraySetSize(stmt, ARRAY_SIZE));
        }

        void TearDown() override
        {
            if (sysdate) OCI_DateFree(sysdate);
            if (stmt) OCI_StatementFree(stmt);
            if (conn) OCI_ConnectionFree(conn);
            ExecDML(OTEXT("truncate table TestInternalArrayInsertArray"));
        }

        void FillArrays()
        {
            auto tab_int = static_cast<OCI_Int32*>(OCI_BindGetData(OCI_StatementGetBind(stmt, 0)));
            auto tab_str = static_cast<otext*>(OCI_BindGetData(OCI_StatementGetBind(stmt, 1)));
            auto tab_date = static_cast<OCI_Date**>(OCI_BindGetData(OCI_StatementGetBind(stmt, 2)));
            for (OCI_Int32 i = 0; i < ARRAY_SIZE; i++)
            {
                tab_int[i] = i + 1;
                osprintf(tab_str + (i * (STRING_SIZE + 1)), OTEXT("Name %d"), i + 1);
                ASSERT_TRUE(OCI_DateAssign(tab_date[i], sysdate));
            }
        }
    };

    TEST_F(TestArrayInt, BindGetDataOnInternalBinds_ReturnsWritableBuffers)
    {
        ASSERT_NE(nullptr, OCI_StatementBindInt32Array(stmt, OTEXT(":i"), NULL, 0));
        ASSERT_NE(nullptr, OCI_StatementBindStringArray(stmt, OTEXT(":s"), NULL, STRING_SIZE, 0));
        ASSERT_NE(nullptr, OCI_StatementBindDateArray(stmt, OTEXT(":d"), NULL, 0));

        EXPECT_NE(nullptr, OCI_BindGetData(OCI_StatementGetBind(stmt, 0)));
        EXPECT_NE(nullptr, OCI_BindGetData(OCI_StatementGetBind(stmt, 1)));
        EXPECT_NE(nullptr, OCI_BindGetData(OCI_StatementGetBind(stmt, 2)));
    }

    TEST_F(TestArrayInt, ExecuteInsertWithInternalArrays_InsertsAllRows)
    {
        ASSERT_NE(nullptr, OCI_StatementBindInt32Array(stmt, OTEXT(":i"), NULL, 0));
        ASSERT_NE(nullptr, OCI_StatementBindStringArray(stmt, OTEXT(":s"), NULL, STRING_SIZE, 0));
        ASSERT_NE(nullptr, OCI_StatementBindDateArray(stmt, OTEXT(":d"), NULL, 0));
        FillArrays();

        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        EXPECT_EQ(ARRAY_SIZE, OCI_StatementGetAffectedRows(stmt));
    }

    TEST_F(TestArrayInt, InsertedRows_FetchBackMatchingValues)
    {
        ASSERT_NE(nullptr, OCI_StatementBindInt32Array(stmt, OTEXT(":i"), NULL, 0));
        ASSERT_NE(nullptr, OCI_StatementBindStringArray(stmt, OTEXT(":s"), NULL, STRING_SIZE, 0));
        ASSERT_NE(nullptr, OCI_StatementBindDateArray(stmt, OTEXT(":d"), NULL, 0));
        FillArrays();
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select * from TestInternalArrayInsertArray")));

        const auto rslt = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rslt);
        OCI_Int32 count = 0;
        while (OCI_ResultsetFetchNext(rslt))
        {
            count++;
            String str = OTEXT("Name ") + TO_STRING(count);
            EXPECT_EQ(count, OCI_ResultsetGetInt32(rslt, 0));
            EXPECT_EQ(str, OCI_ResultsetGetString(rslt, 1));
            EXPECT_EQ(0, OCI_DateCompare(sysdate, OCI_ResultsetGetDate(rslt, 2)));
        }
        EXPECT_EQ(ARRAY_SIZE, count);
    }
}
