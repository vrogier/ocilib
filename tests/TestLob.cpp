#include "ocilib_tests.h"
#include "test_internals.h"

namespace TestCApi
{
    class TestLob : public ::testing::TestWithParam<OCI_UInt32> {};

    static std::vector<OCI_UInt32> LobTypes{ OCI_BLOB, OCI_CLOB, OCI_NCLOB };


    void* GetBufferData()
    {
        static otext* buf = (otext*)OTEXT("123456");

        return  static_cast<void*>(buf);
    }

    OCI_UInt32 GetBufferSize(OCI_UInt32 type)
    {
        OCI_UInt32 size = static_cast<OCI_UInt32>(ostrlen(static_cast<otext*>(GetBufferData())));

        if (type == OCI_BLOB)
        {
            size *= sizeof(otext);
        }

        return size;
    }

    OCI_Int32 GetBufferOffset(OCI_UInt32 type, OCI_Int32 size)
    {
        if (type == OCI_BLOB)
        {
            size *= sizeof(otext);
        }

        return size;
    }

    TEST_P(TestLob, CreateAndFree)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto lob = OCI_LobCreate(conn, GetParam());

        ASSERT_TRUE(nullptr != lob);
        ASSERT_EQ(GetParam(), OCI_LobGetType(lob));
        ASSERT_TRUE(OCI_LobFree(lob));

        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST_P(TestLob, ArrayCreateAndFree)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto lobs = OCI_LobArrayCreate(conn, GetParam(), ARRAY_SIZE);
        ASSERT_TRUE(nullptr != lobs);

        for (OCI_Int32 i = 0; i < ARRAY_SIZE; i++)
        {
            ASSERT_TRUE(nullptr != lobs[i]);
        }
        ASSERT_TRUE(OCI_LobArrayFree(lobs));

        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }


    TEST_P(TestLob, AssignEmpty)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto lob1 = OCI_LobCreate(conn, GetParam());
        const auto lob2 = OCI_LobCreate(conn, GetParam());

        ASSERT_TRUE(OCI_LobAssign(lob2, lob1));

        ASSERT_TRUE(OCI_LobFree(lob1));
        ASSERT_TRUE(OCI_LobFree(lob2));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST_P(TestLob, AssignNotEmpty)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto lob1 = OCI_LobCreate(conn, GetParam());
        const auto lob2 = OCI_LobCreate(conn, GetParam());

        void* buffer_in = GetBufferData();
        OCI_UInt32 size_in = GetBufferSize(GetParam());
        unsigned char buffer_out[1024] = {};

        OCI_UInt32 char_count = (GetParam() == OCI_BLOB) ? 0 : size_in;
        OCI_UInt32 byte_count = (GetParam() == OCI_BLOB) ? size_in : 0;
        ASSERT_TRUE(OCI_LobWrite(lob1, buffer_in, &char_count, &byte_count));
        ASSERT_EQ(size_in, (GetParam() == OCI_BLOB) ? byte_count : char_count);
        ASSERT_EQ(size_in, OCI_LobGetLength(lob1));
        ASSERT_TRUE(OCI_LobAssign(lob2, lob1));
        ASSERT_EQ(size_in, OCI_LobGetLength(lob2));
        char_count = (GetParam() == OCI_BLOB) ? 0 : size_in;
        byte_count = (GetParam() == OCI_BLOB) ? size_in : 0;
        ASSERT_TRUE(OCI_LobRead(lob2, buffer_out, &char_count, &byte_count));
        ASSERT_EQ(size_in, (GetParam() == OCI_BLOB) ? byte_count : char_count);
        ASSERT_EQ(0, memcmp(buffer_in, buffer_out, size_in));

        ASSERT_TRUE(OCI_LobFree(lob1));
        ASSERT_TRUE(OCI_LobFree(lob2));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST_P(TestLob, SeekAndGetOffset)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto type = GetParam();

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto lob = OCI_LobCreate(conn, type);

        void* buffer = GetBufferData();
        OCI_UInt32 size = GetBufferSize(type);

        OCI_UInt32 char_count = (type == OCI_BLOB) ? 0 : size;
        OCI_UInt32 byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobWrite(lob, buffer, &char_count, &byte_count));
        ASSERT_EQ(size, (type == OCI_BLOB) ? byte_count : char_count);
        ASSERT_EQ(size, OCI_LobGetLength(lob));

        ASSERT_TRUE(OCI_LobSeek(lob, GetBufferOffset(type, 2), OCI_SEEK_SET));
        ASSERT_EQ(GetBufferOffset(type, 2), OCI_LobGetOffset(lob));

        ASSERT_TRUE(OCI_LobSeek(lob, GetBufferOffset(type, 2), OCI_SEEK_CUR));
        ASSERT_EQ(GetBufferOffset(type, 4), OCI_LobGetOffset(lob));

        ASSERT_TRUE(OCI_LobSeek(lob, GetBufferOffset(type, -3), OCI_SEEK_CUR));
        ASSERT_EQ(GetBufferOffset(type, 1), OCI_LobGetOffset(lob));

        ASSERT_TRUE(OCI_LobSeek(lob, 0, OCI_SEEK_SET));
        ASSERT_EQ(0, OCI_LobGetOffset(lob));

        ASSERT_TRUE(OCI_LobSeek(lob, 0, OCI_SEEK_END));
        ASSERT_EQ(GetBufferSize(type), OCI_LobGetOffset(lob));

        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST_P(TestLob, Truncate)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto type = GetParam();

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto lob = OCI_LobCreate(conn, type);

        void* buffer = GetBufferData();
        OCI_UInt32 size = GetBufferSize(type);
        OCI_UInt32 newSize = size + size / 2;

        OCI_UInt32 char_count = (type == OCI_BLOB) ? 0 : size;
        OCI_UInt32 byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobWrite(lob, buffer, &char_count, &byte_count));
        ASSERT_EQ(size, (type == OCI_BLOB) ? byte_count : char_count);
        ASSERT_EQ(size, OCI_LobGetLength(lob));

        ASSERT_TRUE(OCI_LobSeek(lob, size - 1, OCI_SEEK_SET));
        ASSERT_EQ(size - 1, OCI_LobGetOffset(lob));

        ASSERT_TRUE(OCI_LobTruncate(lob, size / 2));
        ASSERT_EQ(size / 2, OCI_LobGetLength(lob));
        ASSERT_EQ(size / 2, OCI_LobGetOffset(lob));

        char_count = (type == OCI_BLOB) ? 0 : size;
        byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobWrite(lob, buffer, &char_count, &byte_count));
        ASSERT_EQ(size, (type == OCI_BLOB) ? byte_count : char_count);
        ASSERT_EQ(newSize, OCI_LobGetLength(lob));

        otext newBuffer[1204] = OTEXT("");
        ASSERT_TRUE(OCI_LobSeek(lob, 0, OCI_SEEK_SET));
        char_count = (type == OCI_BLOB) ? 0 : newSize;
        byte_count = (type == OCI_BLOB) ? newSize : 0;
        ASSERT_TRUE(OCI_LobRead(lob, newBuffer, &char_count, &byte_count));
        ASSERT_EQ(newSize, (type == OCI_BLOB) ? byte_count : char_count);
        ASSERT_EQ(newSize, OCI_LobGetOffset(lob));

        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }


    TEST_P(TestLob, AppendCopyEraseAndMore)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto type = GetParam();
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto lob = OCI_LobCreate(conn, type);
        ASSERT_NE(nullptr, lob);

        void* buffer = GetBufferData();
        OCI_UInt32 size = GetBufferSize(type);

        OCI_UInt32 char_count = (type == OCI_BLOB) ? 0 : size;
        OCI_UInt32 byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobWrite(lob, buffer, &char_count, &byte_count));
        ASSERT_EQ(size, (type == OCI_BLOB) ? byte_count : char_count);
        ASSERT_EQ(size, OCI_LobGetLength(lob));

        char_count = (type == OCI_BLOB) ? 0 : size;
        byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobAppend(lob, buffer, &char_count, &byte_count));
        ASSERT_EQ(size, (type == OCI_BLOB) ? byte_count : char_count);
        ASSERT_EQ(size + size, OCI_LobGetLength(lob));

        const auto lob2 = OCI_LobCreate(conn, type);
        ASSERT_NE(nullptr, lob2);
        const auto full = OCI_LobGetLength(lob);
        ASSERT_TRUE(OCI_LobCopy(lob2, lob, 0, 0, full));
        ASSERT_EQ(full, OCI_LobGetLength(lob2));
        ASSERT_FALSE(OCI_LobIsEqual(lob, lob2));

        ASSERT_TRUE(OCI_LobIsTemporary(lob));
        ASSERT_GT(OCI_LobGetChunkSize(lob), 0u);
        ASSERT_GT(OCI_LobGetMaxSize(lob), 0u);
        ASSERT_EQ(conn, OCI_LobGetConnection(lob));
        ASSERT_FALSE(OCI_LobIsRemote(lob));

        ASSERT_EQ(size / 2, OCI_LobErase(lob, 0, size / 2));
        ASSERT_EQ(size + size, OCI_LobGetLength(lob));

        ASSERT_TRUE(OCI_LobFree(lob2));
        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST_P(TestLob, OpenCloseAndReadWrite2)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto type = GetParam();
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto lob = OCI_LobCreate(conn, type);
        ASSERT_NE(nullptr, lob);

        void* buffer = GetBufferData();
        OCI_UInt32 size = GetBufferSize(type);

        ASSERT_TRUE(OCI_LobOpen(lob, OCI_LOB_READWRITE));

        OCI_UInt32 char_count = (type == OCI_BLOB) ? 0 : size;
        OCI_UInt32 byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobWrite(lob, buffer, &char_count, &byte_count));

        ASSERT_TRUE(OCI_LobSeek(lob, 0, OCI_SEEK_SET));

        otext out[1024] = {};
        char_count = (type == OCI_BLOB) ? 0 : size;
        byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobRead(lob, out, &char_count, &byte_count));

        OCI_UInt32 compareBytes = (type == OCI_BLOB) ? size : size * static_cast<OCI_UInt32>(sizeof(otext));
        ASSERT_EQ(0, memcmp(buffer, out, compareBytes));

        ASSERT_TRUE(OCI_LobClose(lob));

        ASSERT_TRUE(OCI_LobEnableBuffering(lob, TRUE));
        ASSERT_TRUE(OCI_LobEnableBuffering(lob, FALSE));

        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestLob, NCLOBAsText)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto lob = OCI_LobCreate(conn, OCI_NCLOB);
        ASSERT_NE(nullptr, lob);
        ASSERT_EQ((OCI_UInt32) OCI_NCLOB, OCI_LobGetType(lob));

        otext text[64] = OTEXT("national characters");
        OCI_UInt32 len = static_cast<OCI_UInt32>(ostrlen(text));
        OCI_UInt32 char_count = len;
        OCI_UInt32 byte_count = 0;
        ASSERT_TRUE(OCI_LobWrite(lob, text, &char_count, &byte_count));
        ASSERT_EQ(len, char_count);
        ASSERT_EQ(len, OCI_LobGetLength(lob));
        ASSERT_GT(OCI_LobGetChunkSize(lob), 0u);

        otext read[64] = {};
        ASSERT_TRUE(OCI_LobSeek(lob, 0, OCI_SEEK_SET));
        char_count = len;
        byte_count = 0;
        ASSERT_TRUE(OCI_LobRead(lob, read, &char_count, &byte_count));
        ASSERT_EQ(len, char_count);
        ASSERT_EQ(ostring(text), ostring(read));

        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    void BuildExpectedText(void* buffer, OCI_UInt32 size, OCI_Int32 repeats, otext* out, OCI_UInt32 type)
    {
        otext* src = static_cast<otext*>(buffer);
        for (OCI_Int32 i = 0; i < repeats; i++)
        {
            for (OCI_UInt32 j = 0; j < size; j++)
            {
                out[i * size + j] = src[j];
            }
        }
        out[repeats * size] = 0;
    }

    void BuildExpectedBytes(void* buffer, OCI_UInt32 size, OCI_Int32 repeats, unsigned char* out)
    {
        for (OCI_Int32 i = 0; i < repeats; i++)
        {
            memcpy(out + i * size, buffer, size);
        }
    }

    TEST_P(TestLob, Write2WithCharAndByteCounts)
    {
        // Arrange
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto type = GetParam();
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto lob = OCI_LobCreate(conn, type);
        ASSERT_NE(nullptr, lob);

        void* buffer = GetBufferData();
        OCI_UInt32 size = GetBufferSize(type);
        otext out[128] = {};

        // Act - write using char_count and verify
        OCI_UInt32 char_count = (type == OCI_BLOB) ? 0 : size;
        OCI_UInt32 byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobWrite(lob, buffer, &char_count, &byte_count));

        // Want
        ASSERT_EQ((OCI_UInt64) size, OCI_LobGetLength(lob));

        // Act - overwrite using byte_count to exercise the alternate branch
        ASSERT_TRUE(OCI_LobSeek(lob, 0, OCI_SEEK_SET));
        char_count = 0;
        byte_count = (type == OCI_BLOB) ? size : static_cast<OCI_UInt32>(size * sizeof(otext));
        ASSERT_TRUE(OCI_LobWrite(lob, buffer, &char_count, &byte_count));

        // Want
        ASSERT_EQ((OCI_UInt64) size, OCI_LobGetLength(lob));

        ASSERT_TRUE(OCI_LobSeek(lob, 0, OCI_SEEK_SET));
        char_count = (type == OCI_BLOB) ? 0 : size;
        byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobRead(lob, out, &char_count, &byte_count));

        if (type != OCI_BLOB)
        {
            ASSERT_EQ(ostring(static_cast<otext*>(buffer)), ostring(out));
        }
        else
        {
            ASSERT_EQ(0, memcmp(buffer, out, size));
        }

        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST_P(TestLob, Append2WithCharAndByteCounts)
    {
        // Arrange
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto type = GetParam();
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto lob = OCI_LobCreate(conn, type);
        ASSERT_NE(nullptr, lob);

        void* buffer = GetBufferData();
        OCI_UInt32 size = GetBufferSize(type);
        otext out[192] = {};

        // Act - seed the LOB
        OCI_UInt32 char_count = (type == OCI_BLOB) ? 0 : size;
        OCI_UInt32 byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobWrite(lob, buffer, &char_count, &byte_count));

        // Act - append using char_count
        char_count = (type == OCI_BLOB) ? 0 : size;
        byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobAppend(lob, buffer, &char_count, &byte_count));

        // Act - append using byte_count
        char_count = 0;
        byte_count = (type == OCI_BLOB) ? size : static_cast<OCI_UInt32>(size * sizeof(otext));
        ASSERT_TRUE(OCI_LobAppend(lob, buffer, &char_count, &byte_count));

        // Want
        ASSERT_EQ((OCI_UInt64)(size * 3), OCI_LobGetLength(lob));

        ASSERT_TRUE(OCI_LobSeek(lob, 0, OCI_SEEK_SET));
        char_count = (type == OCI_BLOB) ? 0 : (size * 3);
        byte_count = (type == OCI_BLOB) ? (size * 3) : 0;
        ASSERT_TRUE(OCI_LobRead(lob, out, &char_count, &byte_count));

        if (type != OCI_BLOB)
        {
            otext expected[64] = {};
            BuildExpectedText(buffer, size, 3, expected, type);
            ASSERT_EQ(ostring(expected), ostring(out));
        }
        else
        {
            unsigned char expected[96] = {};
            BuildExpectedBytes(buffer, size, 3, expected);
            ASSERT_EQ(0, memcmp(expected, out, size * 3));
        }

        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST_P(TestLob, AppendLob)
    {
        // Arrange
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto type = GetParam();
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto lob1 = OCI_LobCreate(conn, type);
        const auto lob2 = OCI_LobCreate(conn, type);
        ASSERT_NE(nullptr, lob1);
        ASSERT_NE(nullptr, lob2);

        void* buffer = GetBufferData();
        OCI_UInt32 size = GetBufferSize(type);
        otext out[128] = {};

        // Act
        OCI_UInt32 char_count = (type == OCI_BLOB) ? 0 : size;
        OCI_UInt32 byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobWrite(lob1, buffer, &char_count, &byte_count));
        ASSERT_EQ(size, (type == OCI_BLOB) ? byte_count : char_count);
        char_count = (type == OCI_BLOB) ? 0 : size;
        byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobWrite(lob2, buffer, &char_count, &byte_count));
        ASSERT_EQ(size, (type == OCI_BLOB) ? byte_count : char_count);
        ASSERT_TRUE(OCI_LobAppendLob(lob1, lob2));

        // Want
        ASSERT_EQ((OCI_UInt64)(size * 2), OCI_LobGetLength(lob1));

        ASSERT_TRUE(OCI_LobSeek(lob1, 0, OCI_SEEK_SET));
        char_count = (type == OCI_BLOB) ? 0 : size * 2;
        byte_count = (type == OCI_BLOB) ? size * 2 : 0;
        ASSERT_TRUE(OCI_LobRead(lob1, out, &char_count, &byte_count));
        ASSERT_EQ(size * 2, (type == OCI_BLOB) ? byte_count : char_count);

        if (type != OCI_BLOB)
        {
            otext expected[64] = {};
            BuildExpectedText(buffer, size, 2, expected, type);
            ASSERT_EQ(ostring(expected), ostring(out));
        }
        else
        {
            unsigned char expected[64] = {};
            BuildExpectedBytes(buffer, size, 2, expected);
            ASSERT_EQ(0, memcmp(expected, out, size * 2));
        }

        ASSERT_TRUE(OCI_LobFree(lob2));
        ASSERT_TRUE(OCI_LobFree(lob1));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST_P(TestLob, BufferingAndFlush)
    {
        // Arrange
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto type = GetParam();
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto lob = OCI_LobCreate(conn, type);
        ASSERT_NE(nullptr, lob);

        void* buffer = GetBufferData();
        OCI_UInt32 size = GetBufferSize(type);

        // Act
        OCI_UInt32 char_count = (type == OCI_BLOB) ? 0 : size;
        OCI_UInt32 byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobWrite(lob, buffer, &char_count, &byte_count));
        ASSERT_EQ(size, (type == OCI_BLOB) ? byte_count : char_count);
        ASSERT_TRUE(OCI_LobEnableBuffering(lob, TRUE));
        char_count = (type == OCI_BLOB) ? 0 : size;
        byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobWrite(lob, buffer, &char_count, &byte_count));
        ASSERT_EQ(size, (type == OCI_BLOB) ? byte_count : char_count);
        ASSERT_TRUE(OCI_LobFlush(lob));
        ASSERT_TRUE(OCI_LobEnableBuffering(lob, FALSE));

        // Want
        ASSERT_EQ((OCI_UInt64)(size * 2), OCI_LobGetLength(lob));

        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST_P(TestLob, CopyFromFileFails)
    {
        // Arrange
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto type = GetParam();
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto lob = OCI_LobCreate(conn, type);
        ASSERT_NE(nullptr, lob);

        // Act
        const auto file = OCI_FileCreate(conn, OCI_BFILE);
        ASSERT_NE(nullptr, file);
        ASSERT_TRUE(OCI_FileSetName(file, OTEXT("FAKE_DIR"), OTEXT("fake.txt")));
        (void) OCI_LobCopyFromFile(lob, file, 0, 0, 0);

        // Want - no crash and the LOB remains usable
        ASSERT_TRUE(OCI_FileFree(file));
        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestLob, FetchedLobAndAssign)
    {
        ExecDML(OTEXT("create table TestLob_Fetch (id int, c clob)"));
        ExecDML(OTEXT("insert into TestLob_Fetch values (1, EMPTY_CLOB())"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT c FROM TestLob_Fetch WHERE id = 1")));
        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto fetched = OCI_GetLob(rs, 1);
        ASSERT_NE(nullptr, fetched);
        ASSERT_EQ((OCI_UInt32) OCI_CLOB, OCI_LobGetType(fetched));

        const auto created = OCI_LobCreate(conn, OCI_CLOB);
        ASSERT_NE(nullptr, created);
        (void) OCI_LobAssign(fetched, created);

        ASSERT_TRUE(OCI_LobFree(created));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestLob_Fetch"));
    }

    /* Exercise the ub4 (pre-LOB2) OCI calls used when the client does not
       support 64 bits LOB lengths: the fixture flips env->use_lob_ub8 off. */

    class TestLobUb4 : public ::testing::TestWithParam<OCI_UInt32>
    {
    protected:

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            type = GetParam();

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            lob  = OCI_LobCreate(conn, type);
            lob2 = OCI_LobCreate(conn, type);
            ASSERT_NE(nullptr, lob);
            ASSERT_NE(nullptr, lob2);

            size = GetBufferSize(type);

            old_ub8 = OCI_TestEnvSetUseLobUb8(conn, FALSE);
        }

        void TearDown() override
        {
            OCI_TestEnvSetUseLobUb8(conn, old_ub8);
            OCI_LobFree(lob2);
            OCI_LobFree(lob);
            OCI_ConnectionFree(conn);
        }

        OCI_UInt32 WriteBuffer()
        {
            OCI_UInt32 char_count = (type == OCI_BLOB) ? 0 : size;
            OCI_UInt32 byte_count = (type == OCI_BLOB) ? size : 0;
            EXPECT_TRUE(OCI_LobWrite(lob, GetBufferData(), &char_count, &byte_count));

            return static_cast<OCI_UInt32>(OCI_LobGetLength(lob));
        }

        OCI_Connection* conn{ nullptr };
        OCI_Lob*        lob{ nullptr };
        OCI_Lob*        lob2{ nullptr };
        OCI_UInt32    type{ 0 };
        OCI_UInt32    size{ 0 };
        OCI_Int32             old_ub8{ 0 };
    };

    TEST_P(TestLobUb4, Write)
    {
        ASSERT_EQ(size, WriteBuffer());
    }

    TEST_P(TestLobUb4, WriteByByteCount)
    {
        // write through byte_count only (char_count == 0 path)
        OCI_UInt32 char_count = 0;
        OCI_UInt32 byte_count = (type == OCI_BLOB) ? size : static_cast<OCI_UInt32>(size * sizeof(otext));
        ASSERT_TRUE(OCI_LobWrite(lob, GetBufferData(), &char_count, &byte_count));
        ASSERT_EQ(size, OCI_LobGetLength(lob));
    }

    TEST_P(TestLobUb4, Read)
    {
        ASSERT_EQ(size, WriteBuffer());

        ASSERT_TRUE(OCI_LobSeek(lob, 0, OCI_SEEK_SET));
        otext out[128] = {};
        OCI_UInt32 char_count = (type == OCI_BLOB) ? 0 : size;
        OCI_UInt32 byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobRead(lob, out, &char_count, &byte_count));
        ASSERT_EQ(size, (type == OCI_BLOB) ? byte_count : char_count);
    }

    TEST_P(TestLobUb4, Append)
    {
        ASSERT_EQ(size, WriteBuffer());

        OCI_UInt32 char_count = (type == OCI_BLOB) ? 0 : size;
        OCI_UInt32 byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobAppend(lob, GetBufferData(), &char_count, &byte_count));
        ASSERT_EQ(size * 2, OCI_LobGetLength(lob));
    }

    TEST_P(TestLobUb4, Copy)
    {
        ASSERT_EQ(size, WriteBuffer());

        ASSERT_TRUE(OCI_LobCopy(lob2, lob, 0, 0, OCI_LobGetLength(lob)));
        ASSERT_EQ(size, OCI_LobGetLength(lob2));
    }

    TEST_P(TestLobUb4, Erase)
    {
        ASSERT_EQ(size, WriteBuffer());
        ASSERT_EQ(size, OCI_LobErase(lob, 0, size));
    }

    TEST_P(TestLobUb4, Truncate)
    {
        ASSERT_EQ(size, WriteBuffer());

        OCI_UInt32 char_count = (type == OCI_BLOB) ? 0 : size;
        OCI_UInt32 byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobAppend(lob, GetBufferData(), &char_count, &byte_count));

        ASSERT_TRUE(OCI_LobTruncate(lob, size));
        ASSERT_EQ(size, OCI_LobGetLength(lob));
    }

    TEST_P(TestLobUb4, CopyFromFileFails)
    {
        // CopyFromFile on a fake directory exercises the legacy call's failure path
        const auto file = OCI_FileCreate(conn, OCI_BFILE);
        ASSERT_NE(nullptr, file);
        ASSERT_TRUE(OCI_FileSetName(file, OTEXT("FAKE_DIR"), OTEXT("fake.txt")));
        (void) OCI_LobCopyFromFile(lob, file, 0, 0, 0);
        ASSERT_TRUE(OCI_FileFree(file));
    }

    TEST_P(TestLobUb4, GetMaxSize)
    {
        ASSERT_EQ(0u, OCI_LobGetMaxSize(lob));
    }

    INSTANTIATE_TEST_CASE_P(LobTypesUb4, TestLobUb4, ::testing::ValuesIn(LobTypes));

    TEST_P(TestLob, AppendLegacyVersion)
    {
        // version_runtime < 10.1: OCI_LobAppend() uses Seek + Write
        // (Oracle bug #886191 workaround)
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto type = GetParam();
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto lob = OCI_LobCreate(conn, type);
        ASSERT_NE(nullptr, lob);

        void* buffer = GetBufferData();
        OCI_UInt32 size = GetBufferSize(type);

        OCI_UInt32 char_count = (type == OCI_BLOB) ? 0 : size;
        OCI_UInt32 byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobWrite(lob, buffer, &char_count, &byte_count));
        ASSERT_EQ(size, OCI_LobGetLength(lob));

        const auto old_ver = OCI_TestEnvSetVersionRuntime(conn, OCI_9_2);
        char_count = (type == OCI_BLOB) ? 0 : size;
        byte_count = (type == OCI_BLOB) ? size : 0;
        ASSERT_TRUE(OCI_LobAppend(lob, buffer, &char_count, &byte_count));
        OCI_TestEnvSetVersionRuntime(conn, old_ver);

        ASSERT_EQ(size * 2, OCI_LobGetLength(lob));

        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    /* flipping env->charset to ANSI exercises the single-byte (misnamed
       IS_UTF8_ENV) conversion arms of OcilibLobRead/Write/Append that a
       wide-char build never reaches. Calls may fail inside OCI; only the
       pre-call conversion paths matter here. */

    class TestLobAnsiCharset : public ::testing::Test
    {
    protected:

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            lob = OCI_LobCreate(conn, OCI_CLOB);
            ASSERT_NE(nullptr, lob);

            old_charset = OCI_TestEnvSetCharset(conn, OCI_CHAR_ANSI);
        }

        void TearDown() override
        {
            OCI_TestEnvSetCharset(conn, old_charset);
            OCI_LobFree(lob);
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
        OCI_Lob*        lob{ nullptr };
        OCI_Int32             old_charset{ 0 };
    };

    TEST_F(TestLobAnsiCharset, Write)
    {
        void* buffer = GetBufferData();
        const OCI_UInt32 size = GetBufferSize(OCI_CLOB);

        OCI_UInt32 char_count = size;
        OCI_UInt32 byte_count = 0;
        (void) OCI_LobWrite(lob, buffer, &char_count, &byte_count);
    }

    TEST_F(TestLobAnsiCharset, Read)
    {
        const OCI_UInt32 size = GetBufferSize(OCI_CLOB);

        (void) OCI_LobSeek(lob, 0, OCI_SEEK_SET);
        otext out[128] = {};
        OCI_UInt32 char_count = size;
        OCI_UInt32 byte_count = 0;
        (void) OCI_LobRead(lob, out, &char_count, &byte_count);
    }

    TEST_F(TestLobAnsiCharset, Append)
    {
        void* buffer = GetBufferData();
        const OCI_UInt32 size = GetBufferSize(OCI_CLOB);

        OCI_UInt32 char_count = size;
        OCI_UInt32 byte_count = 0;
        (void) OCI_LobAppend(lob, buffer, &char_count, &byte_count);
    }

    TEST(TestLob, CopyFromFileRealDirectory)
    {
        // Successful OCI_LobCopyFromFile() requires a DIRECTORY object that
        // exists on the DB server host and a real file inside it.
        ExecDML(OTEXT("create or replace directory TEST_LOB_DIR as 'C:\\Users\\vince\\source\\repos\\ocilib\\tests'"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto lob = OCI_LobCreate(conn, OCI_BLOB);
        ASSERT_NE(nullptr, lob);

        const auto file = OCI_FileCreate(conn, OCI_BFILE);
        ASSERT_NE(nullptr, file);
        ASSERT_TRUE(OCI_FileSetName(file, OTEXT("TEST_LOB_DIR"), OTEXT("ocilib_tests.h")));

        const auto copied = OCI_LobCopyFromFile(lob, file, 0, 0, 2000);
        if (copied)
        {
            ASSERT_GT(OCI_LobGetLength(lob), 0u);
        }

        ASSERT_TRUE(OCI_FileFree(file));
        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop directory TEST_LOB_DIR"));
    }

    INSTANTIATE_TEST_CASE_P(TestLob, TestLob, ::testing::ValuesIn(LobTypes));
}