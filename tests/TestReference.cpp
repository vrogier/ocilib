#include "ocilib_tests.h"
#include "NullArgs.h"

using namespace ocilib;

static void IniTestRef()
{
    ExecDML(OTEXT("create type vendor_t as object(code  number, name  varchar2(30))"));
    ExecDML(OTEXT("create table vendors of vendor_t"));
    ExecDML(OTEXT("insert into vendors values (vendor_t(1, 'John Doe'))"));
}

static void CleanupTestRef()
{
    ExecDML(OTEXT("drop table vendors"));
}
namespace TestCApi
{
    TEST(TestRef, CreateAndFree)
    {
        IniTestRef();

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto tif = OCI_TypeInfoGet(conn, OTEXT("vendor_t"), OCI_TIF_TYPE);
        const auto ref = OCI_ReferenceCreate(tif);

        ASSERT_TRUE(nullptr != ref);
        ASSERT_TRUE(OCI_ReferenceFree(ref));

        ASSERT_TRUE(OCI_ConnectionFree(conn));

        CleanupTestRef();
    }

    TEST(TestRef, ArrayCreateAndFree)
    {
        IniTestRef();

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto tif = OCI_TypeInfoGet(conn, OTEXT("vendor_t"), OCI_TIF_TYPE);
        const auto refs = OCI_ReferenceArrayCreate(tif, ARRAY_SIZE);
        ASSERT_TRUE(nullptr != refs);

        for (OCI_Int32 i = 0; i < ARRAY_SIZE; i++)
        {
            ASSERT_TRUE(nullptr != refs[i]);
        }
        ASSERT_TRUE(OCI_ReferenceArrayFree(refs));

        ASSERT_TRUE(OCI_ConnectionFree(conn));

        CleanupTestRef();
    }

    TEST(TestRef, Assign)
    {
        IniTestRef();

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);
        const auto tif = OCI_TypeInfoGet(conn, OTEXT("vendor_t"), OCI_TIF_TYPE);
        const auto ref2 = OCI_ReferenceCreate(tif);

        OCI_StatementExecute(stmt, OTEXT("select ref(v) from vendors v"));
        const auto rslt = OCI_StatementGetResultset(stmt);

        while (OCI_ResultsetFetchNext(rslt))
        {
            const auto ref1 = OCI_ResultsetGetReference(rslt, 0);
            ASSERT_TRUE(OCI_ReferenceAssign(ref2, ref1));
        }

        ASSERT_TRUE(OCI_ReferenceFree(ref2));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        CleanupTestRef();
    }

    TEST(TestRef, GetTypeInfo)
    {
        IniTestRef();

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto tif = OCI_TypeInfoGet(conn, OTEXT("vendor_t"), OCI_TIF_TYPE);
        const auto ref = OCI_ReferenceCreate(tif);

        ASSERT_EQ(OCI_TypeInfoGet(conn, OTEXT("vendor_t"), OCI_TIF_TYPE), OCI_ReferenceGetTypeInfo((ref)));
        ASSERT_TRUE(OCI_ReferenceFree(ref));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        CleanupTestRef();
    }

    TEST(TestRef, GetObjectFetch)
    {
        IniTestRef();

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);

        OCI_StatementExecute(stmt, OTEXT("select ref(v) from vendors v"));
        const auto rslt = OCI_StatementGetResultset(stmt);

        while (OCI_ResultsetFetchNext(rslt))
        {
            const auto ref = OCI_ResultsetGetReference(rslt, 0);
            const auto obj = OCI_ReferenceGetObject(ref);
            ASSERT_TRUE(nullptr != obj);
        }

        ASSERT_TRUE(OCI_ConnectionFree(conn));

        CleanupTestRef();
    }

    TEST(TestRef, GetObjectNull)
    {
        IniTestRef();

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto tif = OCI_TypeInfoGet(conn, OTEXT("vendor_t"), OCI_TIF_TYPE);
        const auto ref = OCI_ReferenceCreate(tif);

        ASSERT_TRUE(nullptr != ref);
        ASSERT_TRUE(nullptr == OCI_ReferenceGetObject(ref));
        ASSERT_TRUE(OCI_ReferenceFree(ref));

        ASSERT_TRUE(OCI_ConnectionFree(conn));

        CleanupTestRef();
    }

    TEST(TestRef, IsNullTrue)
    {
        IniTestRef();

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto tif = OCI_TypeInfoGet(conn, OTEXT("vendor_t"), OCI_TIF_TYPE);
        const auto ref = OCI_ReferenceCreate(tif);

        ASSERT_TRUE(nullptr != ref);
        ASSERT_TRUE(OCI_ReferenceIsNull(ref));
        ASSERT_TRUE(OCI_ReferenceFree(ref));

        ASSERT_TRUE(OCI_ConnectionFree(conn));

        CleanupTestRef();
    }

    TEST(TestRef, IsNullFalse)
    {
        IniTestRef();

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);

        OCI_StatementExecute(stmt, OTEXT("select ref(v) from vendors v"));
        const auto rslt = OCI_StatementGetResultset(stmt);

        while (OCI_ResultsetFetchNext(rslt))
        {
            const auto ref = OCI_ResultsetGetReference(rslt, 0);
            ASSERT_FALSE(OCI_ReferenceIsNull(ref));
        }

        ASSERT_TRUE(OCI_ConnectionFree(conn));

        CleanupTestRef();
    }

    TEST(TestRef, ToTextNotNull)
    {
        IniTestRef();

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);

        OCI_StatementExecute(stmt, OTEXT("select ref(v) from vendors v"));
        const auto rslt = OCI_StatementGetResultset(stmt);

        while (OCI_ResultsetFetchNext(rslt))
        {
            const auto ref = OCI_ResultsetGetReference(rslt, 0);
            const auto length = OCI_ReferenceGetHexSize(ref);
            ASSERT_TRUE(length > 0);
            otext buffer[128] = {};
            OCI_ReferenceToText(ref, length, buffer);
            String value = buffer;
            ASSERT_EQ(length, value.size());
        }

        ASSERT_TRUE(OCI_ConnectionFree(conn));

        CleanupTestRef();
    }

    TEST(TestRef, ToTextNull)
    {
        IniTestRef();

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto tif = OCI_TypeInfoGet(conn, OTEXT("vendor_t"), OCI_TIF_TYPE);
        const auto ref = OCI_ReferenceCreate(tif);
        const auto length = OCI_ReferenceGetHexSize(ref);
        ASSERT_TRUE(length == 0);
        otext buffer[128] = {};
        OCI_ReferenceToText(ref, length, buffer);
        String value = buffer;
        ASSERT_EQ(length, value.size());

        ASSERT_TRUE(OCI_ReferenceFree(ref));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        CleanupTestRef();
    }

    TEST(TestRef, SetNullWhenNotNullWithoutPinningOject)
    {
        IniTestRef();

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);

        OCI_StatementExecute(stmt, OTEXT("select ref(v) from vendors v"));
        const auto rslt = OCI_StatementGetResultset(stmt);

        while (OCI_ResultsetFetchNext(rslt))
        {
            const auto ref = OCI_ResultsetGetReference(rslt, 0);
            ASSERT_FALSE(OCI_ReferenceIsNull(ref));
            ASSERT_TRUE(OCI_ReferenceSetNull(ref));
            ASSERT_TRUE(OCI_ReferenceIsNull(ref));
        }

        ASSERT_TRUE(OCI_ConnectionFree(conn));

        CleanupTestRef();
    }

    TEST(TestRef, RefFetch)
    {
        IniTestRef();

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);

        OCI_StatementExecute(stmt, OTEXT("select ref(v) from vendors v"));
        const auto rslt = OCI_StatementGetResultset(stmt);

        while (OCI_ResultsetFetchNext(rslt))
        {
            const auto ref = OCI_ResultsetGetReference(rslt, 0);
            ASSERT_TRUE(nullptr != ref);
        }

        ASSERT_TRUE(OCI_ConnectionFree(conn));

        CleanupTestRef();
    }

    TEST(TestRef, RefBindInOut)
    {
        IniTestRef();

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);

        OCI_StatementExecute(stmt, OTEXT("select ref(v) from vendors v"));
        const auto rslt = OCI_StatementGetResultset(stmt);

        while (OCI_ResultsetFetchNext(rslt))
        {
            const auto ref = OCI_ResultsetGetReference(rslt, 0);
            ASSERT_TRUE(nullptr != ref);

            const auto stmt2 = OCI_StatementCreate(conn);
            ASSERT_TRUE(OCI_StatementPrepare(stmt2, OTEXT("select count(*) from vendors v where ref(v) = :ref")));
            ASSERT_NE(nullptr, OCI_StatementBindReference(stmt2, OTEXT(":ref"), ref));
            ASSERT_TRUE(OCI_StatementExecutePrepared(stmt2));
            const auto rslt2 = OCI_StatementGetResultset(stmt2);
            ASSERT_TRUE(OCI_ResultsetFetchNext(rslt2));
            ASSERT_EQ(1, OCI_ResultsetGetInt32(rslt2, 0));
        }

        ASSERT_TRUE(OCI_ConnectionFree(conn));

        CleanupTestRef();
    }

    TEST(TestRef, RefRegister)
    {
        IniTestRef();

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);

        OCI_StatementExecute(stmt, OTEXT("drop table test_ref_register"));
        OCI_StatementExecute(stmt, OTEXT("create table test_ref_register of vendor_t"));

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into test_ref_register v values (vendor_t(1, 'Earth')) returning ref(v) into :1")));
        ASSERT_TRUE(OCI_StatementRegisterReference(stmt, OTEXT(":1"), OCI_TypeInfoGet(conn, OTEXT("vendor_t"), OCI_TIF_TYPE)));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        const auto rslt = OCI_StatementGetResultset(stmt);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rslt));

        const auto ref = OCI_ResultsetGetReference(rslt, 0);
        ASSERT_TRUE(nullptr != ref);
        const auto obj = OCI_ReferenceGetObject(ref);
        ASSERT_TRUE(nullptr != obj);

        ASSERT_EQ(1, OCI_ObjectGetInt32(obj, OTEXT("code")));
        ASSERT_EQ(String(OTEXT("Earth")), String(OCI_ObjectGetString(obj, OTEXT("name"))));

        OCI_StatementExecute(stmt, OTEXT("drop table test_ref_register"));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        CleanupTestRef();
    }

    TEST(TestRef, SetNullWhenNotNullWithPinningOject)
    {
        IniTestRef();

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);

        OCI_StatementExecute(stmt, OTEXT("select ref(v) from vendors v"));
        const auto rslt = OCI_StatementGetResultset(stmt);

        while (OCI_ResultsetFetchNext(rslt))
        {
            const auto ref = OCI_ResultsetGetReference(rslt, 0);
            const auto obj = OCI_ReferenceGetObject(ref);
            ASSERT_TRUE(nullptr != obj);
            ASSERT_FALSE(OCI_ReferenceIsNull(ref));
            ASSERT_TRUE(OCI_ReferenceSetNull(ref));
            ASSERT_TRUE(OCI_ReferenceIsNull(ref));
        }

        ASSERT_TRUE(OCI_ConnectionFree(conn));

        CleanupTestRef();
    }

    TEST(TestRef, SetNullWhenNull)
    {
        IniTestRef();

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto tif = OCI_TypeInfoGet(conn, OTEXT("vendor_t"), OCI_TIF_TYPE);
        const auto ref = OCI_ReferenceCreate(tif);

        ASSERT_TRUE(OCI_ReferenceIsNull(ref));
        ASSERT_TRUE(OCI_ReferenceSetNull(ref));
        ASSERT_TRUE(OCI_ReferenceIsNull(ref));

        ASSERT_TRUE(OCI_ReferenceFree(ref));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        CleanupTestRef();
    }

    class TestReferenceNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestReferenceNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestReferenceNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "RefArrayCreate", [] { return nullptr == OCI_ReferenceArrayCreate(nullptr, 1); } },
            { "RefArrayFree", [] { return !OCI_ReferenceArrayFree(nullptr); } },
            { "RefAssign", [] { return !OCI_ReferenceAssign(nullptr, nullptr); } },
            { "RefCreate", [] { return nullptr == OCI_ReferenceCreate(nullptr); } },
            { "RefFree", [] { return !OCI_ReferenceFree(nullptr); } },
            { "RefGetHexSize", [] { return 0 == OCI_ReferenceGetHexSize(nullptr); } },
            { "RefGetObject", [] { return nullptr == OCI_ReferenceGetObject(nullptr); } },
            { "RefGetTypeInfo", [] { return nullptr == OCI_ReferenceGetTypeInfo(nullptr); } },
            { "RefIsNull", [] { return !OCI_ReferenceIsNull(nullptr); } },
            { "RefSetNull", [] { return !OCI_ReferenceSetNull(nullptr); } },
            { "RefToText", [] { return !OCI_ReferenceToText(nullptr, 1, NullArgText()); } },
        }),
        &NullArgCaseName);

}