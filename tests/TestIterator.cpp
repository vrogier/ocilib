#include "ocilib_tests.h"
#include "NullArgs.h"

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

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select TestIterator_VarArr(10, 20, 30) from dual")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        const auto coll = OCI_ResultsetGetColl(rs, 0);
        ASSERT_NE(nullptr, coll);

        const auto iter = OCI_IteratorCreate(coll);
        ASSERT_NE(nullptr, iter);

        const auto e1 = OCI_IteratorGetNext(iter);
        ASSERT_NE(nullptr, e1);
        ASSERT_EQ(10, OCI_ElementGetInt32(e1));

        const auto e2 = OCI_IteratorGetNext(iter);
        ASSERT_NE(nullptr, e2);
        ASSERT_EQ(20, OCI_ElementGetInt32(e2));

        const auto cur = OCI_IteratorGetCurrent(iter);
        ASSERT_NE(nullptr, cur);
        ASSERT_EQ(20, OCI_ElementGetInt32(cur));

        const auto e3 = OCI_IteratorGetNext(iter);
        ASSERT_NE(nullptr, e3);
        ASSERT_EQ(30, OCI_ElementGetInt32(e3));

        const auto prev = OCI_IteratorGetPrev(iter);
        ASSERT_NE(nullptr, prev);
        ASSERT_EQ(20, OCI_ElementGetInt32(prev));

        ASSERT_TRUE(OCI_IteratorFree(iter));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("drop type TestIterator_VarArr"));
    }

    class TestIteratorNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestIteratorNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestIteratorNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "IterCreate", [] { return nullptr == OCI_IteratorCreate(nullptr); } },
            { "IterFree", [] { return !OCI_IteratorFree(nullptr); } },
            { "IterGetCurrent", [] { return nullptr == OCI_IteratorGetCurrent(nullptr); } },
            { "IterGetNext", [] { return nullptr == OCI_IteratorGetNext(nullptr); } },
            { "IterGetPrev", [] { return nullptr == OCI_IteratorGetPrev(nullptr); } },
        }),
        &NullArgCaseName);

}
