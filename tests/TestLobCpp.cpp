#include "ocilib_tests.h"

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestLobCpp, BlobReadWriteAndGettersCpp)
    {
        Environment& env = GetEnvironmentCpp();
        Connection conn(env, DBS, USR, PWD);

        Blob lob(conn);
        ASSERT_TRUE(lob.GetType() == LobBinary);
        ASSERT_TRUE(lob.IsTemporary());
        ASSERT_FALSE(lob.IsRemote());
        ASSERT_TRUE(lob.GetConnection());

        Raw content{ 'a', 'b', 'c' };
        ASSERT_EQ(static_cast<OCI_UInt32>(content.size()), lob.Write(content));
        ASSERT_EQ(content.size(), lob.GetLength());
        ASSERT_GT(lob.GetMaxSize(), 0u);
        ASSERT_GT(lob.GetChunkSize(), 0u);

        ASSERT_TRUE(lob.Seek(SeekMode(SeekSet), 0));
        ASSERT_EQ(0u, lob.GetOffset());

        Raw read = lob.Read(static_cast<OCI_UInt32>(content.size()));
        ASSERT_EQ(content, read);

        Blob lob2 = lob.Clone();
        ASSERT_TRUE(lob == lob2);
        ASSERT_FALSE(lob != lob2);

        lob2 += lob;
        ASSERT_EQ(content.size() * 2, lob2.GetLength());

        lob.Truncate(1);
        ASSERT_EQ(1u, lob.GetLength());

        ASSERT_EQ(1u, lob.Erase(0, 1));
        ASSERT_EQ(1u, lob.GetLength());

        lob2.Copy(lob, 0, 0, 1);

        lob.Open(OpenMode(ReadWrite));
        lob.Flush();
        lob.Close();

        lob.EnableBuffering(false);
    }

    TEST(TestLobCpp, ClobReadWriteAndGettersCpp)
    {
        Environment& env = GetEnvironmentCpp();
        Connection conn(env, DBS, USR, PWD);

        Clob lob(conn);
        ASSERT_TRUE(lob.GetType() == LobCharacter);
        ASSERT_TRUE(lob.IsTemporary());
        ASSERT_FALSE(lob.IsRemote());

        ostring content = OTEXT("abc");
        ASSERT_EQ(static_cast<OCI_UInt32>(content.size()), lob.Write(content));
        ASSERT_EQ(content.size(), lob.GetLength());
        ASSERT_GT(lob.GetMaxSize(), 0u);
        ASSERT_GT(lob.GetChunkSize(), 0u);

        ASSERT_TRUE(lob.Seek(SeekMode(SeekSet), 0));
        ASSERT_EQ(0u, lob.GetOffset());

        ostring read = lob.Read(static_cast<OCI_UInt32>(content.size()));
        ASSERT_EQ(content, read);

        Clob lob2 = lob.Clone();
        ASSERT_TRUE(lob == lob2);

        lob2 += lob;
        ASSERT_EQ(content.size() * 2, lob2.GetLength());

        lob.Truncate(1);
        ASSERT_EQ(1u, lob.GetLength());

        ASSERT_EQ(1u, lob.Erase(0, 1));
        ASSERT_EQ(1u, lob.GetLength());

        lob2.Copy(lob, 0, 0, 1);

        lob.Open(OpenMode(ReadWrite));
        lob.Flush();
        lob.Close();

        lob.EnableBuffering(false);
    }
}