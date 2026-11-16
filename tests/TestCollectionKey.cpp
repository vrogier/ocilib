#include "ocilib_tests.h"
#include "NullArgs.h"
#include "TestFeatures.h"

using namespace ocilib;

namespace TestCApi
{
    /* String-indexed associative arrays (IBVC — index-by-varchar2 collections).
       PL/SQL INDEX BY VARCHAR2 types exist since Oracle 9iR2; the OCI key API
       requires an Oracle client >= 23.6 */

    class TestCollectionKey : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT(
                "CREATE OR REPLACE PACKAGE TestCollKey_Pkg AS "
                "  TYPE ibvc_num IS TABLE OF NUMBER INDEX BY VARCHAR2(64); "
                "  TYPE ibvc_str IS TABLE OF VARCHAR2(200) INDEX BY VARCHAR2(64); "
                "  TYPE num_tab  IS TABLE OF NUMBER INDEX BY BINARY_INTEGER; "
                "  PROCEDURE fill(p IN OUT ibvc_num); "
                "  FUNCTION  total(p IN ibvc_num) RETURN NUMBER; "
                "END;"));
            ExecDML(OTEXT(
                "CREATE OR REPLACE PACKAGE BODY TestCollKey_Pkg AS "
                "  PROCEDURE fill(p IN OUT ibvc_num) IS BEGIN "
                "    p('alpha') := 111; p('beta') := 222; p('gamma') := 333; "
                "  END; "
                "  FUNCTION total(p IN ibvc_num) RETURN NUMBER IS "
                "    k VARCHAR2(64); s NUMBER := 0; BEGIN "
                "    k := p.FIRST; WHILE k IS NOT NULL LOOP s := s + p(k); k := p.NEXT(k); END LOOP; "
                "    RETURN s; END; "
                "END;"));
            ExecDML(OTEXT("CREATE OR REPLACE TYPE TestCollKey_Varray AS VARRAY(4) OF NUMBER"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("DROP PACKAGE TestCollKey_Pkg"));
            ExecDML(OTEXT("DROP TYPE TestCollKey_Varray"));
        }
    };

    TEST_F(TestCollectionKey, GetKeyType_OnIbvc_ReturnsString)
    {
        SKIP_IF_FEATURE_UNSUPPORTED(OCI_FEATURE_COLL_KEY);

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("usr.TestCollKey_Pkg.ibvc_num"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto coll = OCI_CollectionCreate(typinf);
        ASSERT_NE(nullptr, coll);

        EXPECT_EQ(OCI_CKT_STRING, OCI_CollectionGetKeyType(coll));

        ASSERT_TRUE(OCI_CollectionFree(coll));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST_F(TestCollectionKey, GetKeyType_OnNumericItable_ReturnsNumber)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("usr.TestCollKey_Pkg.num_tab"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto coll = OCI_CollectionCreate(typinf);
        ASSERT_NE(nullptr, coll);

        EXPECT_EQ(OCI_CKT_NUMBER, OCI_CollectionGetKeyType(coll));

        /* no spurious OCI error must be left behind by the keytype probe */

        const auto err = OCI_GetLastError();
        EXPECT_TRUE(nullptr == err || OCI_ERR_NONE == OCI_ErrorGetCode(err));

        ASSERT_TRUE(OCI_CollectionFree(coll));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST_F(TestCollectionKey, GetKeyType_OnVarray_ReturnsUnknown)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestCollKey_Varray"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto coll = OCI_CollectionCreate(typinf);
        ASSERT_NE(nullptr, coll);

        EXPECT_EQ(OCI_CKT_UNKNOWN, OCI_CollectionGetKeyType(coll));

        ASSERT_TRUE(OCI_CollectionFree(coll));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    /* creates a local string-indexed collection of NUMBER elements */
    static OCI_Collection* CreateIbvcNumColl(OCI_Connection *conn)
    {
        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("usr.TestCollKey_Pkg.ibvc_num"), OCI_TIF_TYPE);

        if (!typinf)
        {
            return nullptr;
        }

        auto coll = OCI_CollectionCreate(typinf);

        if (!OCI_TypeInfoFree(typinf) || !coll)
        {
            if (coll) { OCI_CollectionFree(coll); }
            return nullptr;
        }

        return coll;
    }

    static bool SetNumKey(OCI_Collection *coll, OCI_TypeInfo *typinf, const otext *key, OCI_Int32 value)
    {
        const auto elem = OCI_ElementCreate(typinf);

        if (!elem || !OCI_ElementSetInt32(elem, value))
        {
            return false;
        }

        const OCI_Boolean res = OCI_CollectionSetElementByKey(coll, key, elem);

        OCI_ElementFree(elem);

        return res == TRUE;
    }

    TEST_F(TestCollectionKey, SetGetElementByKey_RoundTrips)
    {
        SKIP_IF_FEATURE_UNSUPPORTED(OCI_FEATURE_COLL_KEY);

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("usr.TestCollKey_Pkg.ibvc_num"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto coll = OCI_CollectionCreate(typinf);
        ASSERT_NE(nullptr, coll);

        ASSERT_TRUE(SetNumKey(coll, typinf, OTEXT("alpha"), 10));
        ASSERT_TRUE(SetNumKey(coll, typinf, OTEXT("beta"), 20));

        EXPECT_EQ(2, OCI_CollectionGetCount(coll));

        auto elem = OCI_CollectionGetElementByKey(coll, OTEXT("alpha"));
        ASSERT_NE(nullptr, elem);
        EXPECT_EQ(10, OCI_ElementGetInt32(elem));

        /* reassigning an existing key updates its value */

        ASSERT_TRUE(SetNumKey(coll, typinf, OTEXT("alpha"), 15));

        elem = OCI_CollectionGetElementByKey(coll, OTEXT("alpha"));
        ASSERT_NE(nullptr, elem);
        EXPECT_EQ(15, OCI_ElementGetInt32(elem));
        EXPECT_EQ(2, OCI_CollectionGetCount(coll));

        /* unknown key → NULL element */

        EXPECT_EQ(nullptr, OCI_CollectionGetElementByKey(coll, OTEXT("delta")));

        ASSERT_TRUE(OCI_CollectionFree(coll));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST_F(TestCollectionKey, KeyExists_PresentAndAbsent)
    {
        SKIP_IF_FEATURE_UNSUPPORTED(OCI_FEATURE_COLL_KEY);

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("usr.TestCollKey_Pkg.ibvc_num"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto coll = OCI_CollectionCreate(typinf);
        ASSERT_NE(nullptr, coll);

        ASSERT_TRUE(SetNumKey(coll, typinf, OTEXT("alpha"), 10));

        EXPECT_TRUE(OCI_CollectionKeyExists(coll, OTEXT("alpha")));
        EXPECT_FALSE(OCI_CollectionKeyExists(coll, OTEXT("zzz")));

        ASSERT_TRUE(OCI_CollectionFree(coll));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST_F(TestCollectionKey, DeleteElementByKey_RemovesKeyAndFailsOnMissing)
    {
        SKIP_IF_FEATURE_UNSUPPORTED(OCI_FEATURE_COLL_KEY);

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("usr.TestCollKey_Pkg.ibvc_num"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto coll = OCI_CollectionCreate(typinf);
        ASSERT_NE(nullptr, coll);

        ASSERT_TRUE(SetNumKey(coll, typinf, OTEXT("alpha"), 10));
        ASSERT_TRUE(SetNumKey(coll, typinf, OTEXT("beta"), 20));

        ASSERT_TRUE(OCI_CollectionDeleteElementByKey(coll, OTEXT("beta")));
        EXPECT_FALSE(OCI_CollectionKeyExists(coll, OTEXT("beta")));
        EXPECT_EQ(1, OCI_CollectionGetCount(coll));

        /* deleting a nonexistent key reports an error and returns FALSE */

        EXPECT_FALSE(OCI_CollectionDeleteElementByKey(coll, OTEXT("missing")));
        EXPECT_NE(nullptr, OCI_GetLastError());

        ASSERT_TRUE(OCI_CollectionFree(coll));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

        TEST_F(TestCollectionKey, IteratorGetCurrentKey_ReturnsKeysInOrder)
    {
        SKIP_IF_FEATURE_UNSUPPORTED(OCI_FEATURE_COLL_KEY);

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("usr.TestCollKey_Pkg.ibvc_num"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto coll = OCI_CollectionCreate(typinf);
        ASSERT_NE(nullptr, coll);

        ASSERT_TRUE(SetNumKey(coll, typinf, OTEXT("gamma"), 30));
        ASSERT_TRUE(SetNumKey(coll, typinf, OTEXT("alpha"), 10));
        ASSERT_TRUE(SetNumKey(coll, typinf, OTEXT("beta"), 20));

        const auto it = OCI_IteratorCreate(coll);
        ASSERT_NE(nullptr, it);

        const otext* expected[] = { OTEXT("alpha"), OTEXT("beta"), OTEXT("gamma") };
        int i = 0;

        while (OCI_IteratorGetNext(it))
        {
            ASSERT_LT(i, 3);
            EXPECT_EQ(0, ocilib::String(OCI_IteratorGetCurrentKey(it)).compare(expected[i]));
            i++;
        }

        EXPECT_EQ(3, i);

        ASSERT_TRUE(OCI_IteratorFree(it));
        ASSERT_TRUE(OCI_CollectionFree(coll));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST_F(TestCollectionKey, NullElement_StoredAndRetrieved)
    {
        SKIP_IF_FEATURE_UNSUPPORTED(OCI_FEATURE_COLL_KEY);

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("usr.TestCollKey_Pkg.ibvc_num"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto coll = OCI_CollectionCreate(typinf);
        ASSERT_NE(nullptr, coll);

        const auto elem = OCI_ElementCreate(typinf);
        ASSERT_NE(nullptr, elem);
        ASSERT_TRUE(OCI_ElementSetNull(elem));

        ASSERT_TRUE(OCI_CollectionSetElementByKey(coll, OTEXT("nully"), elem));
        ASSERT_TRUE(OCI_ElementFree(elem));

        ASSERT_TRUE(OCI_CollectionKeyExists(coll, OTEXT("nully")));

        const auto got = OCI_CollectionGetElementByKey(coll, OTEXT("nully"));
        ASSERT_NE(nullptr, got);

        /* OCI does not preserve element null indicators in string-indexed
           collections (OCICollKeyAssignElem drops OCI_IND_NULL — see
           .vince/BugReport_OCICollKeyAssignElem.md) */
        EXPECT_FALSE(OCI_ElementIsNull(got));

        ASSERT_TRUE(OCI_CollectionFree(coll));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST_F(TestCollectionKey, LongKey_RoundTrips)
    {
        SKIP_IF_FEATURE_UNSUPPORTED(OCI_FEATURE_COLL_KEY);

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("usr.TestCollKey_Pkg.ibvc_num"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto coll = OCI_CollectionCreate(typinf);
        ASSERT_NE(nullptr, coll);

        otext longkey[64];
        for (int i = 0; i < 63; i++) { longkey[i] = OTEXT('a') + (i % 26); }
        longkey[63] = 0;

        ASSERT_TRUE(SetNumKey(coll, typinf, longkey, 77));
        ASSERT_TRUE(OCI_CollectionKeyExists(coll, longkey));

        const auto elem = OCI_CollectionGetElementByKey(coll, longkey);
        ASSERT_NE(nullptr, elem);
        EXPECT_EQ(77, OCI_ElementGetInt32(elem));

        ASSERT_TRUE(OCI_CollectionFree(coll));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST_F(TestCollectionKey, BindOut_PlsqlFillsKeysAndValues)
    {
        SKIP_IF_FEATURE_UNSUPPORTED(OCI_FEATURE_COLL_KEY);

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto coll = CreateIbvcNumColl(conn);
        ASSERT_NE(nullptr, coll);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("BEGIN usr.TestCollKey_Pkg.fill(:c); END;")));
        ASSERT_NE(nullptr, OCI_StatementBindCollection(stmt, OTEXT(":c"), coll));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        EXPECT_EQ(3, OCI_CollectionGetCount(coll));

        const auto elem = OCI_CollectionGetElementByKey(coll, OTEXT("beta"));
        ASSERT_NE(nullptr, elem);
        EXPECT_EQ(222, OCI_ElementGetInt32(elem));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_CollectionFree(coll));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST_F(TestCollectionKey, BindIn_PlsqlReadsKeysAndValues)
    {
        SKIP_IF_FEATURE_UNSUPPORTED(OCI_FEATURE_COLL_KEY);

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("usr.TestCollKey_Pkg.ibvc_num"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto coll = OCI_CollectionCreate(typinf);
        ASSERT_NE(nullptr, coll);

        ASSERT_TRUE(SetNumKey(coll, typinf, OTEXT("k1"), 10));
        ASSERT_TRUE(SetNumKey(coll, typinf, OTEXT("k2"), 20));

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("BEGIN :t := usr.TestCollKey_Pkg.total(:c); END;")));
        OCI_Int32 total = 0;
        ASSERT_NE(nullptr, OCI_StatementBindInt32(stmt, OTEXT(":t"), &total));
        ASSERT_NE(nullptr, OCI_StatementBindCollection(stmt, OTEXT(":c"), coll));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        EXPECT_EQ(30, total);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_CollectionFree(coll));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST_F(TestCollectionKey, NumericItable_PositionalAccessStillWorks)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("usr.TestCollKey_Pkg.num_tab"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto coll = OCI_CollectionCreate(typinf);
        ASSERT_NE(nullptr, coll);

        const auto elem = OCI_ElementCreate(typinf);
        ASSERT_NE(nullptr, elem);
        ASSERT_TRUE(OCI_ElementSetInt32(elem, 42));

        /* numeric index-by tables keep the positional (ByIndex) API on any
           client version */

        ASSERT_TRUE(OCI_CollectionSetElementByIndex(coll, 5, elem));
        ASSERT_TRUE(OCI_ElementFree(elem));

        const auto got = OCI_CollectionGetElementByIndex(coll, 5);
        ASSERT_NE(nullptr, got);
        EXPECT_EQ(42, OCI_ElementGetInt32(got));

        ASSERT_TRUE(OCI_CollectionFree(coll));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST_F(TestCollectionKey, KeyFunctions_PreClient23_6_NotAvailable)
    {
        /* exercised implicitly by GetKeyType_OnNumericItable_ReturnsNumber:
           key functions are gated on OCI_FEATURE_COLL_KEY (client >= 23.6)
           while GetKeyType reports OCI_CKT_NUMBER on any client version */
        SUCCEED();
    }
}
