#include "ocilib_tests.h"

namespace TestCApi
{
    TEST(TestRowId, Fetch)
    {
        ExecDML(OTEXT("create table TestRowIdFetch(code int, name varchar2(50))"));
        ExecDML(OTEXT("truncate table TestRowIdFetch"));
        ExecDML(OTEXT("insert into TestRowIdFetch values (1, 'name 1')"));
        ExecDML(OTEXT("insert into TestRowIdFetch values (2, 'name 2')"));

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        otext rowid1[OCI_SIZE_ROWID + 1] = OTEXT("");
        otext rowid2[OCI_SIZE_ROWID + 1] = OTEXT("");

        ASSERT_TRUE(OCI_Immediate(conn, OTEXT("select rowid from TestRowIdFetch where code = 1"), OCI_ARG_TEXT, rowid1));
        ASSERT_TRUE(OCI_Immediate(conn, OTEXT("select rowid from TestRowIdFetch where code = 2"), OCI_ARG_TEXT, rowid2));

        ASSERT_TRUE(String(rowid1).length() > 0);
        ASSERT_TRUE(String(rowid2).length() > 0);

        ASSERT_NE(String(rowid1), String(rowid2));

        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestRowIdFetch"));
    }

    TEST(TestRowId, Bind)
    {
        ExecDML(OTEXT("create table TestRowIdBind(code int, name varchar2(50))"));
        ExecDML(OTEXT("truncate table TestRowIdBind"));
        ExecDML(OTEXT("insert into TestRowIdBind values (1, 'name 1')"));
        ExecDML(OTEXT("insert into TestRowIdBind values (2, 'name 2')"));

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        otext rowid[OCI_SIZE_ROWID + 1] = OTEXT("");

        ASSERT_TRUE(OCI_Immediate(conn, OTEXT("select rowid from TestRowIdBind where code = 2"), OCI_ARG_TEXT, rowid));

        ASSERT_TRUE(String(rowid).length() > 0);

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select code, name, rowid from TestRowIdBind where rowid = :id")));
        ASSERT_NE(nullptr, OCI_BindString(stmt, OTEXT(":id"), rowid, OCI_SIZE_ROWID));
        ASSERT_TRUE(OCI_Execute(stmt));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_TRUE(OCI_FetchNext(rs));

        ASSERT_EQ(2, OCI_GetInt(rs, 0));
        ASSERT_EQ(String(OTEXT("name 2")), OCI_GetString(rs, 1));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestRowIdBind"));
    }
}