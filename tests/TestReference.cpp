#include "ocilib_tests.h"

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
        const auto ref = OCI_RefCreate(tif);

        ASSERT_TRUE(nullptr != ref);
        ASSERT_TRUE(OCI_RefFree(ref));

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
        const auto refs = OCI_RefArrayCreate(tif, ARRAY_SIZE);
        ASSERT_TRUE(nullptr != refs);

        for (OCI_Int32 i = 0; i < ARRAY_SIZE; i++)
        {
            ASSERT_TRUE(nullptr != refs[i]);
        }
        ASSERT_TRUE(OCI_RefArrayFree(refs));

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
        const auto ref2 = OCI_RefCreate(tif);

        OCI_ExecuteStmt(stmt, OTEXT("select ref(v) from vendors v"));
        const auto rslt = OCI_GetResultset(stmt);

        while (OCI_FetchNext(rslt))
        {
            const auto ref1 = OCI_GetRef(rslt, 0);
            ASSERT_TRUE(OCI_RefAssign(ref2, ref1));
        }

        ASSERT_TRUE(OCI_RefFree(ref2));
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
        const auto ref = OCI_RefCreate(tif);

        ASSERT_EQ(OCI_TypeInfoGet(conn, OTEXT("vendor_t"), OCI_TIF_TYPE), OCI_RefGetTypeInfo((ref)));
        ASSERT_TRUE(OCI_RefFree(ref));
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

        OCI_ExecuteStmt(stmt, OTEXT("select ref(v) from vendors v"));
        const auto rslt = OCI_GetResultset(stmt);

        while (OCI_FetchNext(rslt))
        {
            const auto ref = OCI_GetRef(rslt, 0);
            const auto obj = OCI_RefGetObject(ref);
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
        const auto ref = OCI_RefCreate(tif);

        ASSERT_TRUE(nullptr != ref);
        ASSERT_TRUE(nullptr == OCI_RefGetObject(ref));
        ASSERT_TRUE(OCI_RefFree(ref));

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
        const auto ref = OCI_RefCreate(tif);

        ASSERT_TRUE(nullptr != ref);
        ASSERT_TRUE(OCI_RefIsNull(ref));
        ASSERT_TRUE(OCI_RefFree(ref));

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

        OCI_ExecuteStmt(stmt, OTEXT("select ref(v) from vendors v"));
        const auto rslt = OCI_GetResultset(stmt);

        while (OCI_FetchNext(rslt))
        {
            const auto ref = OCI_GetRef(rslt, 0);
            ASSERT_FALSE(OCI_RefIsNull(ref));
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

        OCI_ExecuteStmt(stmt, OTEXT("select ref(v) from vendors v"));
        const auto rslt = OCI_GetResultset(stmt);

        while (OCI_FetchNext(rslt))
        {
            const auto ref = OCI_GetRef(rslt, 0);
            const auto length = OCI_RefGetHexSize(ref);
            ASSERT_TRUE(length > 0);
            otext buffer[128] = {};
            OCI_RefToText(ref, length, buffer);
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
        const auto ref = OCI_RefCreate(tif);
        const auto length = OCI_RefGetHexSize(ref);
        ASSERT_TRUE(length == 0);
        otext buffer[128] = {};
        OCI_RefToText(ref, length, buffer);
        String value = buffer;
        ASSERT_EQ(length, value.size());

        ASSERT_TRUE(OCI_RefFree(ref));
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

        OCI_ExecuteStmt(stmt, OTEXT("select ref(v) from vendors v"));
        const auto rslt = OCI_GetResultset(stmt);

        while (OCI_FetchNext(rslt))
        {
            const auto ref = OCI_GetRef(rslt, 0);
            ASSERT_FALSE(OCI_RefIsNull(ref));
            ASSERT_TRUE(OCI_RefSetNull(ref));
            ASSERT_TRUE(OCI_RefIsNull(ref));
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

        OCI_ExecuteStmt(stmt, OTEXT("select ref(v) from vendors v"));
        const auto rslt = OCI_GetResultset(stmt);

        while (OCI_FetchNext(rslt))
        {
            const auto ref = OCI_GetRef(rslt, 0);
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

        OCI_ExecuteStmt(stmt, OTEXT("select ref(v) from vendors v"));
        const auto rslt = OCI_GetResultset(stmt);

        while (OCI_FetchNext(rslt))
        {
            const auto ref = OCI_GetRef(rslt, 0);
            ASSERT_TRUE(nullptr != ref);

            const auto stmt2 = OCI_StatementCreate(conn);
            ASSERT_TRUE(OCI_Prepare(stmt2, OTEXT("select count(*) from vendors v where ref(v) = :ref")));
            ASSERT_NE(nullptr, OCI_BindRef(stmt2, OTEXT(":ref"), ref));
            ASSERT_TRUE(OCI_Execute(stmt2));
            const auto rslt2 = OCI_GetResultset(stmt2);
            ASSERT_TRUE(OCI_FetchNext(rslt2));
            ASSERT_EQ(1, OCI_GetInt(rslt2, 0));
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

        OCI_ExecuteStmt(stmt, OTEXT("drop table test_ref_register"));
        OCI_ExecuteStmt(stmt, OTEXT("create table test_ref_register of vendor_t"));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into test_ref_register v values (vendor_t(1, 'Earth')) returning ref(v) into :1")));
        ASSERT_TRUE(OCI_RegisterRef(stmt, OTEXT(":1"), OCI_TypeInfoGet(conn, OTEXT("vendor_t"), OCI_TIF_TYPE)));
        ASSERT_TRUE(OCI_Execute(stmt));

        const auto rslt = OCI_GetResultset(stmt);
        ASSERT_TRUE(OCI_FetchNext(rslt));

        const auto ref = OCI_GetRef(rslt, 0);
        ASSERT_TRUE(nullptr != ref);
        const auto obj = OCI_RefGetObject(ref);
        ASSERT_TRUE(nullptr != obj);

        ASSERT_EQ(1, OCI_ObjectGetInt(obj, OTEXT("code")));
        ASSERT_EQ(String(OTEXT("Earth")), String(OCI_ObjectGetString(obj, OTEXT("name"))));

        OCI_ExecuteStmt(stmt, OTEXT("drop table test_ref_register"));

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

        OCI_ExecuteStmt(stmt, OTEXT("select ref(v) from vendors v"));
        const auto rslt = OCI_GetResultset(stmt);

        while (OCI_FetchNext(rslt))
        {
            const auto ref = OCI_GetRef(rslt, 0);
            const auto obj = OCI_RefGetObject(ref);
            ASSERT_TRUE(nullptr != obj);
            ASSERT_FALSE(OCI_RefIsNull(ref));
            ASSERT_TRUE(OCI_RefSetNull(ref));
            ASSERT_TRUE(OCI_RefIsNull(ref));
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
        const auto ref = OCI_RefCreate(tif);

        ASSERT_TRUE(OCI_RefIsNull(ref));
        ASSERT_TRUE(OCI_RefSetNull(ref));
        ASSERT_TRUE(OCI_RefIsNull(ref));

        ASSERT_TRUE(OCI_RefFree(ref));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        CleanupTestRef();
    }
}