#include "ocilib_tests.h"

namespace TestCApi
{
    TEST(TestIterator, ForwardAndBackward)
    {
        ExecDML(OTEXT("create or replace type TestIterator_VarArr as varray(10) of number"));

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select TestIterator_VarArr(10, 20, 30) from dual")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 0);
        ASSERT_NE(nullptr, coll);

        const auto iter = OCI_IterCreate(coll);
        ASSERT_NE(nullptr, iter);

        const auto e1 = OCI_IterGetNext(iter);
        ASSERT_NE(nullptr, e1);
        ASSERT_EQ(10, OCI_ElemGetInt(e1));

        const auto e2 = OCI_IterGetNext(iter);
        ASSERT_NE(nullptr, e2);
        ASSERT_EQ(20, OCI_ElemGetInt(e2));

        const auto cur = OCI_IterGetCurrent(iter);
        ASSERT_NE(nullptr, cur);
        ASSERT_EQ(20, OCI_ElemGetInt(cur));

        const auto e3 = OCI_IterGetNext(iter);
        ASSERT_NE(nullptr, e3);
        ASSERT_EQ(30, OCI_ElemGetInt(e3));

        const auto prev = OCI_IterGetPrev(iter);
        ASSERT_NE(nullptr, prev);
        ASSERT_EQ(20, OCI_ElemGetInt(prev));

        ASSERT_TRUE(OCI_IterFree(iter));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("drop type TestIterator_VarArr"));
    }
}
