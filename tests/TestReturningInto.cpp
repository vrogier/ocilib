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

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("update TestReturningIntoRegisterInt set code = code + 10, name = name || ' updated' returning code into :code")));
        ASSERT_TRUE(OCI_RegisterInt(stmt, OTEXT(":code")));
        ASSERT_TRUE(OCI_Execute(stmt));

        auto rslt = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rslt);

        OCI_Int32 index = 0;
        while (OCI_FetchNext(rslt))
        {
            index++;
            ASSERT_EQ(index + 10, OCI_GetInt(rslt, 0));
        }

        ASSERT_EQ(2, OCI_GetAffectedRows(stmt));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select count(*) from TestReturningIntoRegisterInt where name like '%updated'")));

        rslt = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rslt);

        ASSERT_TRUE(OCI_FetchNext(rslt));
        ASSERT_EQ(2, OCI_GetInt(rslt, 0));

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
            { "RegisterBigInt", [] { return !OCI_RegisterBigInt(nullptr, OTEXT("x")); } },
            { "RegisterDate", [] { return !OCI_RegisterDate(nullptr, OTEXT("x")); } },
            { "RegisterDouble", [] { return !OCI_RegisterDouble(nullptr, OTEXT("x")); } },
            { "RegisterFile", [] { return !OCI_RegisterFile(nullptr, OTEXT("x"), 1); } },
            { "RegisterFloat", [] { return !OCI_RegisterFloat(nullptr, OTEXT("x")); } },
            { "RegisterInt", [] { return !OCI_RegisterInt(nullptr, OTEXT("x")); } },
            { "RegisterInterval", [] { return !OCI_RegisterInterval(nullptr, OTEXT("x"), 1); } },
            { "RegisterJson", [] { return !OCI_RegisterJson(nullptr, OTEXT("x")); } },
            { "RegisterLob", [] { return !OCI_RegisterLob(nullptr, OTEXT("x"), 1); } },
            { "RegisterNumber", [] { return !OCI_RegisterNumber(nullptr, OTEXT("x")); } },
            { "RegisterObject", [] { return !OCI_RegisterObject(nullptr, OTEXT("x"), nullptr); } },
            { "RegisterRaw", [] { return !OCI_RegisterRaw(nullptr, OTEXT("x"), 1); } },
            { "RegisterRef", [] { return !OCI_RegisterRef(nullptr, OTEXT("x"), nullptr); } },
            { "RegisterShort", [] { return !OCI_RegisterShort(nullptr, OTEXT("x")); } },
            { "RegisterString", [] { return !OCI_RegisterString(nullptr, OTEXT("x"), 1); } },
            { "RegisterTimestamp", [] { return !OCI_RegisterTimestamp(nullptr, OTEXT("x"), 1); } },
            { "RegisterUnsignedBigInt", [] { return !OCI_RegisterUnsignedBigInt(nullptr, OTEXT("x")); } },
            { "RegisterUnsignedInt", [] { return !OCI_RegisterUnsignedInt(nullptr, OTEXT("x")); } },
            { "RegisterUnsignedShort", [] { return !OCI_RegisterUnsignedShort(nullptr, OTEXT("x")); } },
            { "RegisterVector", [] { return !OCI_RegisterVector(nullptr, OTEXT("x")); } },
        }),
        &NullArgCaseName);

}