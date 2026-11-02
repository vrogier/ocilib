#include "ocilib_tests.h"

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestTransactionCpp, CreateAndGettersCpp)
    {
        Environment& env = GetEnvironmentCpp();
        Connection conn(env, DBS, USR, PWD);

        Transaction trans(conn, 60, Transaction::TransactionFlags(Transaction::New | Transaction::ReadWrite));
        ASSERT_EQ(60u, trans.GetTimeout());
        ASSERT_EQ(Transaction::TransactionFlags(Transaction::New | Transaction::ReadWrite), trans.GetFlags());
    }

    TEST(TestTransactionCpp, StartStopCpp)
    {
        Environment& env = GetEnvironmentCpp();
        Connection conn(env, DBS, USR, PWD);

        Transaction trans(conn, 60, Transaction::TransactionFlags(Transaction::New | Transaction::ReadWrite));
        conn.SetTransaction(trans);
        trans.Start();
        trans.Stop();
    }

    TEST(TestTransactionCpp, ConnectionGetSetTransactionCpp)
    {
        Environment& env = GetEnvironmentCpp();
        Connection conn(env, DBS, USR, PWD);

        Transaction trans(conn, 60, Transaction::TransactionFlags(Transaction::New | Transaction::ReadWrite));
        conn.SetTransaction(trans);

        Transaction current = conn.GetTransaction();
        ASSERT_FALSE(current.IsNull());
    }

    TEST(TestTransactionCpp, GlobalTransactionCreateAndGettersCpp)
    {
        Environment& env = GetEnvironmentCpp();
        Connection conn(env, DBS, USR, PWD);

        OCI_XID xid = {};
        xid.formatID = 1;
        xid.gtrid_length = 6;
        xid.bqual_length = 5;
        memcpy(xid.data, "gtridBQUAL", 11);

        Transaction trans(conn, 30, Transaction::TransactionFlags(Transaction::New | Transaction::ReadWrite), &xid);
        ASSERT_EQ(30u, trans.GetTimeout());
        ASSERT_EQ(Transaction::TransactionFlags(Transaction::New | Transaction::ReadWrite), trans.GetFlags());
    }
}
