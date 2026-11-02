#include "ocilib_tests.h"

namespace TestCApi
{
    TEST(TestError, Getters)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD_WRONG, OCI_SESSION_DEFAULT);
        ASSERT_EQ(nullptr, con);

        auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);

        ASSERT_EQ(OCI_ERR_ORACLE, OCI_ErrorGetType(err));
        ASSERT_EQ(1017, OCI_ErrorGetOCICode(err));
        ASSERT_GE(OCI_ErrorGetInternalCode(err), 0);
        ASSERT_NE(nullptr, OCI_ErrorGetString(err));
        ASSERT_NE(nullptr, OCI_ErrorGetLocation(err));
        ASSERT_NE(nullptr, OCI_ErrorGetConnection(err));
        ASSERT_EQ(nullptr, OCI_ErrorGetStatement(err));
        ASSERT_EQ(0u, OCI_ErrorGetRow(err));

    }

    TEST(TestError, SqlError)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_FALSE(OCI_ExecuteStmt(stmt, OTEXT("select * from nonexistent_table_xyz")));

        auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);

        ASSERT_EQ(OCI_ERR_ORACLE, OCI_ErrorGetType(err));
        ASSERT_NE(nullptr, OCI_ErrorGetString(err));
        ASSERT_EQ(con, OCI_ErrorGetConnection(err));
        ASSERT_EQ(stmt, OCI_ErrorGetStatement(err));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestError, InsertValueTooLong)
    {
        ExecDML(OTEXT("drop table TestErrorData"));
        ExecDML(OTEXT("create table TestErrorData (value varchar2(5))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_FALSE(OCI_ExecuteStmt(stmt, OTEXT("insert into TestErrorData values ('too long')")));

        auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);

        ASSERT_EQ(OCI_ERR_ORACLE, OCI_ErrorGetType(err));
        ASSERT_NE(nullptr, OCI_ErrorGetString(err));
        ASSERT_EQ(con, OCI_ErrorGetConnection(err));
        ASSERT_EQ(stmt, OCI_ErrorGetStatement(err));
        ASSERT_GE(OCI_ErrorGetRow(err), 0u);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("drop table TestErrorData"));
    }

    TEST(TestError, PoolCreationError)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto pool = OCI_PoolCreate(env, DBS, USR, PWD_WRONG, OCI_POOL_SESSION, OCI_ENV_DEFAULT, 0, 1, 1);
        ASSERT_NE(nullptr, pool);

        const auto con = OCI_PoolGetConnection(env, pool, nullptr);
        ASSERT_EQ(nullptr, con);

        auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);

        ASSERT_EQ(OCI_ERR_ORACLE, OCI_ErrorGetType(err));
        ASSERT_EQ(1017, OCI_ErrorGetOCICode(err));
        ASSERT_NE(nullptr, OCI_ErrorGetString(err));
        ASSERT_NE(nullptr, OCI_ErrorGetConnection(err));
        ASSERT_EQ(nullptr, OCI_ErrorGetStatement(err));
        (void) OCI_HandleGetEnvironment(err);

        ASSERT_TRUE(OCI_PoolFree(env, pool));

    }

    TEST(TestError, InternalError)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_FALSE(OCI_SetFetchMode(stmt, 9999));

        auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);

        ASSERT_EQ(OCI_ERR_OCILIB, OCI_ErrorGetType(err));
        ASSERT_EQ(0, OCI_ErrorGetOCICode(err));
        ASSERT_GT(OCI_ErrorGetInternalCode(err), 0);
        ASSERT_NE(nullptr, OCI_ErrorGetString(err));
        ASSERT_EQ(stmt, OCI_ErrorGetStatement(err));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestError, DateTimestampIntervalSource)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto date = OCI_DateCreate(env);
        const auto ts = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        const auto iv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        ASSERT_NE(nullptr, date);
        ASSERT_NE(nullptr, ts);
        ASSERT_NE(nullptr, iv);

        otext buf[64];

        ASSERT_FALSE(OCI_DateToText(date, OTEXT("BAD_FMT"), 64, buf));
        auto err = OCI_GetLastError();
        if (err != nullptr)
        {
            ASSERT_EQ(nullptr, OCI_ErrorGetConnection(err));
        }

        ASSERT_FALSE(OCI_TimestampToText(ts, OTEXT("BAD_FMT"), 64, buf, 3));
        err = OCI_GetLastError();
        if (err != nullptr)
        {
            ASSERT_EQ(nullptr, OCI_ErrorGetConnection(err));
        }

        ASSERT_FALSE(OCI_IntervalToText(iv, 2, 3, 1, buf));
        err = OCI_GetLastError();
        if (err != nullptr)
        {
            ASSERT_EQ(nullptr, OCI_ErrorGetConnection(err));
        }

        ASSERT_TRUE(OCI_DateFree(date));
        ASSERT_TRUE(OCI_TimestampFree(ts));
        ASSERT_TRUE(OCI_IntervalFree(iv));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestError, ResultsetError)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select 1 from dual")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_FALSE(OCI_FetchPrev(rs));

        auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);

        ASSERT_EQ(stmt, OCI_ErrorGetStatement(err));
        ASSERT_EQ(con, OCI_ErrorGetConnection(err));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestError, MiscSources)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select :i from dual")));

        OCI_Int32 i = 1;
        ASSERT_NE(nullptr, OCI_BindInt(stmt, OTEXT(":i"), &i));

        const auto bnd = OCI_GetBindByName(stmt, OTEXT(":i"));
        ASSERT_NE(nullptr, bnd);

        ASSERT_EQ(0u, OCI_BindGetDataSizeAtPos(bnd, 2));

        auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);
        ASSERT_EQ(con, OCI_ErrorGetConnection(err));
        ASSERT_EQ(stmt, OCI_ErrorGetStatement(err));

        const auto num = OCI_NumberCreate(env);
        ASSERT_NE(nullptr, num);

        ASSERT_FALSE(OCI_NumberFromText(num, OTEXT("abc"), OTEXT("FM999")));

        err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);
        ASSERT_EQ(nullptr, OCI_ErrorGetConnection(err));

        const auto lob = OCI_LobCreate(con, OCI_CLOB);
        ASSERT_NE(nullptr, lob);

        ASSERT_EQ(0u, OCI_LobErase(lob, 1, 0));

        err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);
        ASSERT_EQ(con, OCI_ErrorGetConnection(err));

        ASSERT_TRUE(OCI_NumberFree(num));
        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }
}
