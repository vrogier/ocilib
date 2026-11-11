#include "ocilib_tests.h"
#include "NullArgs.h"

namespace TestCApi
{
    TEST(TestReturningInto, RegisterInt)
    {
        ExecDML(OTEXT("create table TestReturningIntoRegisterInt(code int, name varchar2(50))"));
        ExecDML(OTEXT("truncate table TestReturningIntoRegisterInt"));
        ExecDML(OTEXT("insert into TestReturningIntoRegisterInt values (1, 'name 1')"));
        ExecDML(OTEXT("insert into TestReturningIntoRegisterInt values (2, 'name 2')"));

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("update TestReturningIntoRegisterInt set code = code + 10, name = name || ' updated' returning code into :code")));
        ASSERT_TRUE(OCI_StatementRegisterInt32(stmt, OTEXT(":code")));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        auto rslt = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rslt);

        OCI_Int32 index = 0;
        while (OCI_ResultsetFetchNext(rslt))
        {
            index++;
            ASSERT_EQ(index + 10, OCI_ResultsetGetInt32(rslt, 0));
        }

        ASSERT_EQ(2, OCI_StatementGetAffectedRows(stmt));

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select count(*) from TestReturningIntoRegisterInt where name like '%updated'")));

        rslt = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rslt);

        ASSERT_TRUE(OCI_ResultsetFetchNext(rslt));
        ASSERT_EQ(2, OCI_ResultsetGetInt32(rslt, 0));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestReturningIntoRegisterInt"));
    }

    class TestReturningIntoNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestReturningIntoNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestReturningIntoNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "RegisterBigInt", [] { return !OCI_StatementRegisterInt64(nullptr, OTEXT("x")); } },
            { "RegisterDate", [] { return !OCI_StatementRegisterDate(nullptr, OTEXT("x")); } },
            { "RegisterDouble", [] { return !OCI_StatementRegisterDouble(nullptr, OTEXT("x")); } },
            { "RegisterFile", [] { return !OCI_StatementRegisterFile(nullptr, OTEXT("x"), 1); } },
            { "RegisterFloat", [] { return !OCI_StatementRegisterFloat(nullptr, OTEXT("x")); } },
            { "RegisterInt", [] { return !OCI_StatementRegisterInt32(nullptr, OTEXT("x")); } },
            { "RegisterInterval", [] { return !OCI_StatementRegisterInterval(nullptr, OTEXT("x"), 1); } },
            { "RegisterJson", [] { return !OCI_StatementRegisterJson(nullptr, OTEXT("x")); } },
            { "RegisterLob", [] { return !OCI_StatementRegisterLob(nullptr, OTEXT("x"), 1); } },
            { "RegisterNumber", [] { return !OCI_StatementRegisterNumber(nullptr, OTEXT("x")); } },
            { "RegisterObject", [] { return !OCI_StatementRegisterObject(nullptr, OTEXT("x"), nullptr); } },
            { "RegisterRaw", [] { return !OCI_StatementRegisterRaw(nullptr, OTEXT("x"), 1); } },
            { "RegisterRef", [] { return !OCI_StatementRegisterReference(nullptr, OTEXT("x"), nullptr); } },
            { "RegisterShort", [] { return !OCI_StatementRegisterInt16(nullptr, OTEXT("x")); } },
            { "RegisterString", [] { return !OCI_StatementRegisterString(nullptr, OTEXT("x"), 1); } },
            { "RegisterTimestamp", [] { return !OCI_StatementRegisterTimestamp(nullptr, OTEXT("x"), 1); } },
            { "RegisterUnsignedBigInt", [] { return !OCI_StatementRegisterUInt64(nullptr, OTEXT("x")); } },
            { "RegisterUnsignedInt", [] { return !OCI_StatementRegisterUInt32(nullptr, OTEXT("x")); } },
            { "RegisterUnsignedShort", [] { return !OCI_StatementRegisterUInt16(nullptr, OTEXT("x")); } },
            { "RegisterVector", [] { return !OCI_StatementRegisterVector(nullptr, OTEXT("x")); } },
        }),
        &NullArgCaseName);

}