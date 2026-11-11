#include "ocilib_tests.h"
#include "NullArgs.h"

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
        ASSERT_EQ(String(text), String(read));

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
            ASSERT_EQ(String(static_cast<otext*>(buffer)), String(out));
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
            ASSERT_EQ(String(expected), String(out));
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
            ASSERT_EQ(String(expected), String(out));
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

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("SELECT c FROM TestLob_Fetch WHERE id = 1")));
        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        const auto fetched = OCI_ResultsetGetLob(rs, 0);
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













    /* flipping env->charset to OCI_CHAR_MBCS exercises the single-byte (misnamed
       IS_UTF8_ENV) conversion arms of OcilibLobRead/Write/Append that a
       wide-char build never reaches. Calls may fail inside OCI; only the
       pre-call conversion paths matter here. */





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

    /* --- public-API error paths: NULL lob handles hit the CHECK_PTR arms
       (covers the OcilibLob* guards the dropped ub4/charset internals
       tests used) --- */

    TEST(TestLob, Append_PreClient10_UsesSeekWriteFallback)
    {
        /* pre-10.1 clients have no OCILobWriteAppend -> append falls back to
           Seek+Write. Lowered env exercises that branch without an old client. */

        const auto env = GetSpecificEnvironment(nullptr, OCI_9_2);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);
        const auto lob = OCI_LobCreate(conn, OCI_CLOB);
        ASSERT_NE(nullptr, lob);
        const auto text = OTEXT("abc");

        ASSERT_EQ(OCI_9_2, OCI_ConnectionGetVersion(conn));

        OCI_UInt32 cc = 3, bc = 0;
        ASSERT_TRUE(OCI_LobWrite(lob, (void*)text, &cc, &bc));
        cc = 3; bc = 0;
        ASSERT_TRUE(OCI_LobAppend(lob, (void*)text, &cc, &bc));
        ASSERT_EQ((OCI_UInt64)6, OCI_LobGetLength(lob));

        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }

    /* --- coverage: OcilibLobWrite non-UTF8 count-derivation arms 
       the default test env is not UTF8 on either build, so the
       zero-count input gets derived from the other --- */

    TEST(TestLob, Write_CharCountOnly_DerivesByteCount)
    {
        const auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);
        const auto lob = OCI_LobCreate(conn, OCI_CLOB);
        ASSERT_NE(nullptr, lob);
        const auto text = OTEXT("abc");

        OCI_UInt32 cc = 3, bc = 0;
        ASSERT_TRUE(OCI_LobWrite(lob, (void*)text, &cc, &bc));
        EXPECT_EQ(3u * static_cast<OCI_UInt32>(sizeof(otext)), bc);
        ASSERT_EQ((OCI_UInt64)3, OCI_LobGetLength(lob));

        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestLob, Write_ByteCountOnly_DerivesCharCount)
    {
        const auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);
        const auto lob = OCI_LobCreate(conn, OCI_CLOB);
        ASSERT_NE(nullptr, lob);
        const auto text = OTEXT("abc");

        OCI_UInt32 cc = 0, bc = 3u * static_cast<OCI_UInt32>(sizeof(otext));
        ASSERT_TRUE(OCI_LobWrite(lob, (void*)text, &cc, &bc));
        EXPECT_EQ(3u, cc);
        ASSERT_EQ((OCI_UInt64)3, OCI_LobGetLength(lob));

        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    /* --- coverage: OcilibLobWrite / OcilibLobErase legacy ub4 arms
       pre-10.1 clients have no OCILobWrite2/OCILobErase2,
       so a lowered env exercises the non-ub8 branches --- */

    TEST(TestLob, Write_PreClient10_UsesLegacyLobWrite)
    {
        const auto env = GetSpecificEnvironment(nullptr, OCI_9_2);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);
        const auto lob = OCI_LobCreate(conn, OCI_CLOB);
        ASSERT_NE(nullptr, lob);
        const auto text = OTEXT("abc");

        ASSERT_EQ(OCI_9_2, OCI_ConnectionGetVersion(conn));

        OCI_UInt32 cc = 3, bc = 0;
        ASSERT_TRUE(OCI_LobWrite(lob, (void*)text, &cc, &bc));
        ASSERT_EQ((OCI_UInt64)3, OCI_LobGetLength(lob));

        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }

    TEST(TestLob, Erase_PreClient10_UsesLegacyLobErase)
    {
        const auto env = GetSpecificEnvironment(nullptr, OCI_9_2);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);
        const auto lob = OCI_LobCreate(conn, OCI_CLOB);
        ASSERT_NE(nullptr, lob);
        const auto text = OTEXT("abcd");

        OCI_UInt32 cc = 4, bc = 0;
        ASSERT_TRUE(OCI_LobWrite(lob, (void*)text, &cc, &bc));

        ASSERT_EQ((OCI_UInt64)2, OCI_LobErase(lob, 0, 2));

        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }

    /* --- coverage: OcilibMemoryAllocDescriptorArray /
       OcilibMemoryFreeDescriptorArray legacy per-descriptor loops
       (memory.c) — pre-11.1 clients have no OCIArrayDescriptorAlloc/Free,
       so a lowered env exercises the OCIDescriptorAlloc/Free loops --- */

    TEST(TestLob, ArrayCreateFree_PreClient11_UsesDescriptorLoop)
    {
        const auto env = GetSpecificEnvironment(nullptr, OCI_10_1);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);
        ASSERT_EQ(OCI_10_1, OCI_ConnectionGetVersion(conn));

        const auto arr = OCI_LobArrayCreate(conn, OCI_CLOB, 2);
        ASSERT_NE(nullptr, arr);
        ASSERT_TRUE(OCI_LobArrayFree(arr));

        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }

    /* --- coverage: OcilibLobTruncate offset reset arm 
       truncating below the current write offset resets it to
       OcilibLobGetLength() + 1 --- */

    TEST(TestLob, Truncate_BelowOffset_ResetsOffset)
    {
        const auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);
        const auto lob = OCI_LobCreate(conn, OCI_CLOB);
        ASSERT_NE(nullptr, lob);
        const auto text = OTEXT("abcd");

        OCI_UInt32 cc = 4, bc = 0;
        ASSERT_TRUE(OCI_LobWrite(lob, (void*)text, &cc, &bc));
        ASSERT_EQ((OCI_UInt64)4, OCI_LobGetOffset(lob));

        ASSERT_TRUE(OCI_LobTruncate(lob, 2));
        EXPECT_LE(OCI_LobGetOffset(lob), (OCI_UInt64)2);

        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }


    class TestLobNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestLobNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestLobNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "LobAppend", [] { return !OCI_LobAppend(nullptr, NullArgBuffer(), NullArgOut<OCI_UInt32>(), NullArgOut<OCI_UInt32>()); } },
            { "LobAppendLob", [] { return !OCI_LobAppendLob(nullptr, nullptr); } },
            { "LobArrayCreate", [] { return nullptr == OCI_LobArrayCreate(nullptr, 1, 1); } },
            { "LobArrayFree", [] { return !OCI_LobArrayFree(nullptr); } },
            { "LobAssign", [] { return !OCI_LobAssign(nullptr, nullptr); } },
            { "LobClose", [] { return !OCI_LobClose(nullptr); } },
            { "LobCopy", [] { return !OCI_LobCopy(nullptr, nullptr, 1, 1, 1); } },
            { "LobCopyFromFile", [] { return !OCI_LobCopyFromFile(nullptr, nullptr, 1, 1, 1); } },
            { "LobCreate", [] { return nullptr == OCI_LobCreate(nullptr, 1); } },
            { "LobEnableBuffering", [] { return !OCI_LobEnableBuffering(nullptr, FALSE); } },
            { "LobErase", [] { return 0 == OCI_LobErase(nullptr, 1, 1); } },
            { "LobFlush", [] { return !OCI_LobFlush(nullptr); } },
            { "LobFree", [] { return !OCI_LobFree(nullptr); } },
            { "LobGetChunkSize", [] { return 0 == OCI_LobGetChunkSize(nullptr); } },
            { "LobGetConnection", [] { return nullptr == OCI_LobGetConnection(nullptr); } },
            { "LobGetLength", [] { return 0 == OCI_LobGetLength(nullptr); } },
            { "LobGetMaxSize", [] { return 0 == OCI_LobGetMaxSize(nullptr); } },
            { "LobGetOffset", [] { return 0 == OCI_LobGetOffset(nullptr); } },
            { "LobGetType", [] { return OCI_UNKNOWN == OCI_LobGetType(nullptr); } },
            { "LobIsEqual", [] { return !OCI_LobIsEqual(nullptr, nullptr); } },
            { "LobIsRemote", [] { return !OCI_LobIsRemote(nullptr); } },
            { "LobIsTemporary", [] { return !OCI_LobIsTemporary(nullptr); } },
            { "LobOpen", [] { return !OCI_LobOpen(nullptr, 1); } },
            { "LobRead", [] { return !OCI_LobRead(nullptr, NullArgBuffer(), NullArgOut<OCI_UInt32>(), NullArgOut<OCI_UInt32>()); } },
            { "LobSeek", [] { return !OCI_LobSeek(nullptr, 1, 1); } },
            { "LobTruncate", [] { return !OCI_LobTruncate(nullptr, 1); } },
            { "LobWrite", [] { return !OCI_LobWrite(nullptr, NullArgBuffer(), NullArgOut<OCI_UInt32>(), NullArgOut<OCI_UInt32>()); } },
        }),
        &NullArgCaseName);

}