#include "ocilib_tests.h"
#include "test_internals.h"

#include <chrono>
#include <cstdio>

namespace TestCApi
{
    /* generates a unique XID per call: suspended global branches persist
       server-side, so reusing a fixed XID across runs hits ORA-24757 */

    static void MakeXid(OCI_XID* xid)
    {
        static OCI_UInt32 seq = 0;

        char buf[32] = {};
        snprintf(buf, sizeof(buf), "X%08lx%03x",
                 static_cast<unsigned long>(
                     std::chrono::steady_clock::now().time_since_epoch().count() & 0xffffffff),
                 ++seq & 0xfff);

        memset(xid, 0, sizeof(*xid));
        xid->formatID     = 0x1234;
        xid->gtrid_length = 8;
        xid->bqual_length = 8;
        memcpy(xid->data, buf, 16);
    }

    TEST(TestTransaction, CreateAndGetters)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        auto trans = OCI_TransactionCreate(con, 60, OCI_TRS_NEW | OCI_TRS_READWRITE, nullptr);
        ASSERT_NE(nullptr, trans);

        ASSERT_EQ(OCI_TRS_NEW | OCI_TRS_READWRITE, OCI_TransactionGetMode(trans));
        ASSERT_EQ(60u, OCI_TransactionGetTimeout(trans));

        (void) OCI_GetTransaction(con);
        (void) OCI_TransactionStart(trans);
        (void) OCI_TransactionStop(trans);

        ASSERT_TRUE(OCI_TransactionFree(trans));

        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestTransaction, LocalAutoCommitTransaction)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        ASSERT_TRUE(OCI_SetAutoCommit(con, TRUE));

        auto trans = OCI_TransactionCreate(con, 60, OCI_TRS_NEW | OCI_TRS_READWRITE, nullptr);
        ASSERT_NE(nullptr, trans);

        ASSERT_EQ(60u, OCI_TransactionGetTimeout(trans));
        ASSERT_EQ(OCI_TRS_NEW | OCI_TRS_READWRITE, OCI_TransactionGetMode(trans));

        (void) OCI_TransactionStart(trans);
        (void) OCI_TransactionStop(trans);

        ASSERT_TRUE(OCI_TransactionFree(trans));

        ASSERT_TRUE(OCI_SetAutoCommit(con, FALSE));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestTransaction, GlobalTransactionLifecycle)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        OCI_XID xid = {};
        MakeXid(&xid);

        auto trans = OCI_TransactionCreate(con, 30, OCI_TRS_NEW | OCI_TRS_READWRITE, &xid);
        ASSERT_NE(nullptr, trans);

        ASSERT_EQ(30u, OCI_TransactionGetTimeout(trans));
        ASSERT_EQ(OCI_TRS_NEW | OCI_TRS_READWRITE, OCI_TransactionGetMode(trans));

        (void) OCI_TransactionStart(trans);
        (void) OCI_TransactionPrepare(trans);
        (void) OCI_TransactionForget(trans);
        (void) OCI_TransactionResume(trans);
        (void) OCI_TransactionStop(trans);

        ASSERT_TRUE(OCI_TransactionFree(trans));

        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestTransaction, GlobalTransactionWithWork)
    {
        ExecDML(OTEXT("drop table TestXaWork"));
        ExecDML(OTEXT("create table TestXaWork(code int)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        OCI_XID xid = {};
        MakeXid(&xid);

        const auto trans = OCI_TransactionCreate(con, 60, OCI_TRS_NEW, &xid);
        ASSERT_NE(nullptr, trans);

        /* full XA flow on one branch: stop suspends (detach), resume
           re-attaches, then prepare + forget resolve the branch */

        ASSERT_TRUE(OCI_TransactionStart(trans));
        ASSERT_TRUE(OCI_Immediate(con, OTEXT("insert into TestXaWork values (1)")));
        ASSERT_TRUE(OCI_TransactionStop(trans));

        ASSERT_TRUE(OCI_TransactionResume(trans));
        ASSERT_TRUE(OCI_Immediate(con, OTEXT("insert into TestXaWork values (2)")));
        ASSERT_TRUE(OCI_TransactionPrepare(trans));

        /* roll back the prepared branch to resolve it (a two-phase commit
           is the transaction manager's job and is not exposed), then
           exercise forget - it only succeeds on heuristically completed
           branches, which need a DBA commit/rollback force */

        (void) OCI_Rollback(con);
        (void) OCI_TransactionForget(trans);

        ASSERT_TRUE(OCI_TransactionFree(trans));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("drop table TestXaWork"));
    }

    /* each armed fault makes the next matching OCI call fail, covering the
       error arm of the enclosing CHECK_ATTRIB_* / dispatch */

    class TestTransactionErr : public ::testing::Test
    {
    protected:

        void SetUp() override
        {
            env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, con);

            MakeXid(&xid);
        }

        void TearDown() override
        {
            OCI_TestApiRestore(env);
            /* resolve any still-active branch so it does not linger
               server-side as a suspended global transaction */
            (void) OCI_Rollback(con);
            OCI_ConnectionFree(con);
        }

        OCI_Transaction* CreateGlobal()
        {
            const auto trans = OCI_TransactionCreate(con, 60, OCI_TRS_NEW, &xid);
            EXPECT_NE(nullptr, trans);
            return trans;
        }

        OCI_Environment* env{ nullptr };
        OCI_Connection*  con{ nullptr };
        OCI_XID          xid{};
    };

    TEST_F(TestTransactionErr, CreateFailsOnXidAttach)
    {
        /* failure to attach the XID runs the create error arm */

        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_SET, 1);
        ASSERT_EQ(nullptr, OCI_TransactionCreate(con, 60, OCI_TRS_NEW, &xid));
    }

    TEST_F(TestTransactionErr, StartLocalTransFails)
    {
        /* local transaction: OCITransStart failure arm */

        const auto trans = OCI_TransactionCreate(con, 60, OCI_TRS_NEW, nullptr);
        ASSERT_NE(nullptr, trans);

        OCI_TestFailApiCall(env, OCI_TEST_API_TRANS_START, 1);
        ASSERT_FALSE(OCI_TransactionStart(trans));
        ASSERT_TRUE(OCI_TransactionFree(trans));
    }

    TEST_F(TestTransactionErr, StartFailsOnXidAttach)
    {
        /* global transaction: XID attach failure on start */

        const auto trans = CreateGlobal();
        ASSERT_NE(nullptr, trans);

        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_SET, 1);
        ASSERT_FALSE(OCI_TransactionStart(trans));
        ASSERT_TRUE(OCI_TransactionFree(trans));
    }

    TEST_F(TestTransactionErr, StartGlobalTransFails)
    {
        /* OCITransStart failure arm */

        const auto trans = CreateGlobal();
        ASSERT_NE(nullptr, trans);

        OCI_TestFailApiCall(env, OCI_TEST_API_TRANS_START, 1);
        ASSERT_FALSE(OCI_TransactionStart(trans));
        ASSERT_TRUE(OCI_TransactionFree(trans));
    }

    TEST_F(TestTransactionErr, ResumeFailsOnXidAttach)
    {
        const auto trans = CreateGlobal();
        ASSERT_NE(nullptr, trans);
        ASSERT_TRUE(OCI_TransactionStart(trans));

        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_SET, 1);
        ASSERT_FALSE(OCI_TransactionResume(trans));

        ASSERT_TRUE(OCI_TransactionFree(trans));
    }

    TEST_F(TestTransactionErr, ResumeGlobalTransFails)
    {
        const auto trans = CreateGlobal();
        ASSERT_NE(nullptr, trans);
        ASSERT_TRUE(OCI_TransactionStart(trans));

        OCI_TestFailApiCall(env, OCI_TEST_API_TRANS_START, 1);
        ASSERT_FALSE(OCI_TransactionResume(trans));

        ASSERT_TRUE(OCI_TransactionFree(trans));
    }

    TEST_F(TestTransactionErr, StopFailsOnDetach)
    {
        /* OCITransDetach failure arm */

        const auto trans = CreateGlobal();
        ASSERT_NE(nullptr, trans);
        ASSERT_TRUE(OCI_TransactionStart(trans));

        OCI_TestFailApiCall(env, OCI_TEST_API_TRANS_DETACH, 1);
        ASSERT_FALSE(OCI_TransactionStop(trans));

        ASSERT_TRUE(OCI_TransactionFree(trans));
    }
}
