#include "ocilib_tests.h"


TEST(TestReturningInto, RegisterInt)
{
    ExecDML(OTEXT("create table TestReturningIntoRegisterInt(code int, name varchar2(50))"));
    ExecDML(OTEXT("truncate table TestReturningIntoRegisterInt"));
    ExecDML(OTEXT("insert into TestReturningIntoRegisterInt values (1, 'name 1')"));
    ExecDML(OTEXT("insert into TestReturningIntoRegisterInt values (2, 'name 2')"));

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto stmt = OCI_StatementCreate(conn);
    ASSERT_NE(nullptr, stmt);

    ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("update TestReturningIntoRegisterInt set code = code + 10, name = name || ' updated' returning code into :code")));
    ASSERT_TRUE(OCI_RegisterInt(stmt, OTEXT(":code")));
    ASSERT_TRUE(OCI_Execute(stmt));

    auto rslt = OCI_GetResultset(stmt);
    ASSERT_NE(nullptr, rslt);

    int index = 0;
    while (OCI_FetchNext(rslt))
    {
        index++;
        ASSERT_EQ(index + 10, OCI_GetInt(rslt, 1));
    }

    ASSERT_EQ(2, OCI_GetAffectedRows(stmt));

    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select count(*) from TestReturningIntoRegisterInt where name like '%updated'")));

    rslt = OCI_GetResultset(stmt);
    ASSERT_NE(nullptr, rslt);

    ASSERT_TRUE(OCI_FetchNext(rslt));
    ASSERT_EQ(2, OCI_GetInt(rslt, 1));
 
    ASSERT_TRUE(OCI_StatementFree(stmt));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("drop table TestReturningIntoRegisterInt"));
}