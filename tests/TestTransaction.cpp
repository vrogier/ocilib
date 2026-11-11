#include "ocilib_tests.h"
#include "NullArgs.h"

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

        (void) OCI_ConnectionGetTransaction(con);
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

        ASSERT_TRUE(OCI_ConnectionSetAutoCommit(con, TRUE));

        auto trans = OCI_TransactionCreate(con, 60, OCI_TRS_NEW | OCI_TRS_READWRITE, nullptr);
        ASSERT_NE(nullptr, trans);

        ASSERT_EQ(60u, OCI_TransactionGetTimeout(trans));
        ASSERT_EQ(OCI_TRS_NEW | OCI_TRS_READWRITE, OCI_TransactionGetMode(trans));

        (void) OCI_TransactionStart(trans);
        (void) OCI_TransactionStop(trans);

        ASSERT_TRUE(OCI_TransactionFree(trans));

        ASSERT_TRUE(OCI_ConnectionSetAutoCommit(con, FALSE));
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
        ASSERT_TRUE(OCI_ConnectionImmediate(con, OTEXT("insert into TestXaWork values (1)")));
        ASSERT_TRUE(OCI_TransactionStop(trans));

        ASSERT_TRUE(OCI_TransactionResume(trans));
        ASSERT_TRUE(OCI_ConnectionImmediate(con, OTEXT("insert into TestXaWork values (2)")));
        ASSERT_TRUE(OCI_TransactionPrepare(trans));

        /* roll back the prepared branch to resolve it (a two-phase commit
           is the transaction manager's job and is not exposed), then
           exercise forget - it only succeeds on heuristically completed
           branches, which need a DBA commit/rollback force */

        (void) OCI_ConnectionRollback(con);
        (void) OCI_TransactionForget(trans);

        ASSERT_TRUE(OCI_TransactionFree(trans));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("drop table TestXaWork"));
    }

    /* --- public-API error paths: NULL transaction handles hit the CHECK_PTR
       arms the dropped fault tests covered --- */

    TEST(TestTransaction, NullTransactionErrors)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ASSERT_FALSE(OCI_TransactionFree(nullptr));
        ASSERT_FALSE(OCI_TransactionStart(nullptr));
        ASSERT_FALSE(OCI_TransactionStop(nullptr));
        ASSERT_FALSE(OCI_TransactionResume(nullptr));
        ASSERT_FALSE(OCI_TransactionPrepare(nullptr));
        ASSERT_FALSE(OCI_TransactionForget(nullptr));
        ASSERT_EQ(OCI_UNKNOWN, (OCI_Int32)OCI_TransactionGetMode(nullptr));
        ASSERT_EQ(0u, OCI_TransactionGetTimeout(nullptr));
        ASSERT_EQ(nullptr, OCI_TransactionCreate(nullptr, 0, OCI_TRS_NEW, nullptr));
    }

    class TestTransactionNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestTransactionNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestTransactionNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "TransactionCreate", [] { return nullptr == OCI_TransactionCreate(nullptr, 1, 1, nullptr /* OCI_XID * */); } },
            { "TransactionForget", [] { return !OCI_TransactionForget(nullptr); } },
            { "TransactionFree", [] { return !OCI_TransactionFree(nullptr); } },
            { "TransactionGetMode", [] { return OCI_UNKNOWN == OCI_TransactionGetMode(nullptr); } },
            { "TransactionGetTimeout", [] { return 0 == OCI_TransactionGetTimeout(nullptr); } },
            { "TransactionPrepare", [] { return !OCI_TransactionPrepare(nullptr); } },
            { "TransactionResume", [] { return !OCI_TransactionResume(nullptr); } },
            { "TransactionStart", [] { return !OCI_TransactionStart(nullptr); } },
            { "TransactionStop", [] { return !OCI_TransactionStop(nullptr); } },
        }),
        &NullArgCaseName);

}
