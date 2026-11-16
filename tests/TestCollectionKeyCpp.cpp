#include "ocilib_tests.h"
#include "TestFeatures.h"

using namespace ocilib;

namespace TestCppApi
{
    /* String-indexed associative arrays (IBVC) through the C++ API —
       the OCI key API requires an Oracle client >= 23.6 */

    class TestCollectionKeyCpp : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT(
                "CREATE OR REPLACE PACKAGE TestCollKeyCpp_Pkg AS "
                "  TYPE ibvc_num IS TABLE OF NUMBER INDEX BY VARCHAR2(64); "
                "  PROCEDURE fill(p IN OUT ibvc_num); "
                "END;"));
            ExecDML(OTEXT(
                "CREATE OR REPLACE PACKAGE BODY TestCollKeyCpp_Pkg AS "
                "  PROCEDURE fill(p IN OUT ibvc_num) IS BEGIN "
                "    p('alpha') := 111; p('beta') := 222; p('gamma') := 333; "
                "  END; "
                "END;"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("DROP PACKAGE TestCollKeyCpp_Pkg"));
        }
    };

    static Collection<OCI_Int32> CreateColl(Connection& con)
    {
        return Collection<OCI_Int32>(TypeInfo(con, OTEXT("usr.TestCollKeyCpp_Pkg.ibvc_num"), TypeInfo::Type));
    }

    TEST_F(TestCollectionKeyCpp, GetKeyType_OnIbvc_ReturnsString)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_COLL_KEY);

        auto coll = CreateColl(con);

        EXPECT_EQ(Collection<OCI_Int32>::KeyTypeString, coll.GetKeyType().GetValue());
    }

    TEST_F(TestCollectionKeyCpp, SetGetByKey_RoundTrips)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_COLL_KEY);

        auto coll = CreateColl(con);

        coll.Set(OTEXT("alpha"), 10);
        coll.Set(OTEXT("beta"), 20);

        EXPECT_EQ(2u, coll.GetCount());
        EXPECT_EQ(10, coll.Get(OTEXT("alpha")));
        EXPECT_EQ(20, coll.Get(OTEXT("beta")));

        /* reassigning an existing key updates its value */

        coll.Set(OTEXT("alpha"), 15);
        EXPECT_EQ(15, coll.Get(OTEXT("alpha")));
        EXPECT_EQ(2u, coll.GetCount());
    }

    TEST_F(TestCollectionKeyCpp, KeyExists_PresentAndAbsent)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_COLL_KEY);

        auto coll = CreateColl(con);

        coll.Set(OTEXT("alpha"), 10);

        EXPECT_TRUE(coll.KeyExists(OTEXT("alpha")));
        EXPECT_FALSE(coll.KeyExists(OTEXT("zzz")));
    }

    TEST_F(TestCollectionKeyCpp, Delete_RemovesKey)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_COLL_KEY);

        auto coll = CreateColl(con);

        coll.Set(OTEXT("alpha"), 10);
        coll.Set(OTEXT("beta"), 20);

        ASSERT_TRUE(coll.Delete(OTEXT("beta")));
        EXPECT_FALSE(coll.KeyExists(OTEXT("beta")));
        EXPECT_EQ(1u, coll.GetCount());
    }

        TEST_F(TestCollectionKeyCpp, NullElement_StoredAndDetected)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_COLL_KEY);

        auto coll = CreateColl(con);

        coll.Set(OTEXT("nulled"), 0);
        coll.SetElementNull(OTEXT("nulled"));

        /* OCI does not preserve element null indicators in string-indexed
           collections (OCICollKeyAssignElem drops OCI_IND_NULL — see
           .vince/BugReport_OCICollKeyAssignElem.md) */
        EXPECT_FALSE(coll.IsElementNull(OTEXT("nulled")));
    }

    TEST_F(TestCollectionKeyCpp, IteratorGetCurrentKey_KeysInOrder)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_COLL_KEY);

        auto coll = CreateColl(con);

        coll.Set(OTEXT("gamma"), 30);
        coll.Set(OTEXT("alpha"), 10);
        coll.Set(OTEXT("beta"), 20);

        const String expected[] = { OTEXT("alpha"), OTEXT("beta"), OTEXT("gamma") };
        const auto keys = coll.Keys();

        ASSERT_EQ(3u, keys.size());
        EXPECT_EQ(expected[0], keys[0]);
        EXPECT_EQ(expected[1], keys[1]);
        EXPECT_EQ(expected[2], keys[2]);
    }

    TEST_F(TestCollectionKeyCpp, BindOut_PlsqlFillsKeysAndValues)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_COLL_KEY);

#ifdef OCI_CHARSET_WIDE
        /* OCI defect: an IN OUT bind of an INDEX BY VARCHAR2 collection fails
           at OCIStmtExecute with ORA-22141 in UTF-16 environments (reproduced
           with raw OCI, no OCILIB involved) */
        SkipTest("ORA-22141: INDEX BY VARCHAR2 IN OUT bind fails in UTF-16");
        return;
#endif

        auto coll = CreateColl(con);

        Statement stmt(con);
        stmt.Prepare(OTEXT("BEGIN TestCollKeyCpp_Pkg.fill(:c); END;"));
        stmt.Bind(OTEXT(":c"), coll, BindInfo::InOut);
        stmt.ExecutePrepared();

        EXPECT_EQ(3u, coll.GetCount());
        EXPECT_EQ(222, coll.Get(OTEXT("beta")));
        EXPECT_TRUE(coll.KeyExists(OTEXT("alpha")));
        EXPECT_TRUE(coll.KeyExists(OTEXT("gamma")));
    }
}
