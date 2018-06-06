#include "ocilib_tests.h"

class TestLob : public ::testing::TestWithParam<unsigned int> {};

std::vector<unsigned int> LobTypes{ OCI_BLOB, OCI_CLOB, OCI_NCLOB };


void* GetBufferData()
{
    static otext* buf = OTEXT("123456");

    return  static_cast<void*>(buf);
}

unsigned int GetBufferSize(unsigned int type)
{
    unsigned int size = static_cast<unsigned int>(ostrlen(static_cast<otext*>(GetBufferData())));

    if (type == OCI_BLOB)
    {
        size *= sizeof(otext);
    }

    return size;
}


 int GetBufferOffset(unsigned int type, int size)
{
    if (type == OCI_BLOB)
    {
        size *= sizeof(otext);
    }

    return size;
}

TEST_P(TestLob, CreateAndFree)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    const auto lob = OCI_LobCreate(conn, GetParam());

    ASSERT_TRUE(nullptr != lob);
    ASSERT_EQ(GetParam(), OCI_LobGetType(lob));
    ASSERT_TRUE(OCI_LobFree(lob));

    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());
}

TEST_P(TestLob, ArrayCreateAndFree)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    const auto lobs = OCI_LobArrayCreate(conn, GetParam(), ARRAY_SIZE);
    ASSERT_TRUE(nullptr != lobs);

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        ASSERT_TRUE(nullptr != lobs[i]);
    }
    ASSERT_TRUE(OCI_LobArrayFree(lobs));

    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());
}


TEST_P(TestLob, AssignEmpty)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    const auto lob1 = OCI_LobCreate(conn, GetParam());
    const auto lob2 = OCI_LobCreate(conn, GetParam());

    ASSERT_TRUE(OCI_LobAssign(lob2, lob1));
    
    ASSERT_TRUE(OCI_LobFree(lob1));
    ASSERT_TRUE(OCI_LobFree(lob2));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());
}

TEST_P(TestLob, AssignNotEmpty)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    const auto lob1 = OCI_LobCreate(conn, GetParam());
    const auto lob2 = OCI_LobCreate(conn, GetParam());
    
    void *buffer_in = GetBufferData();
    unsigned int size_in = GetBufferSize(GetParam());
    unsigned char buffer_out[1024] = {};

    ASSERT_EQ(size_in, OCI_LobWrite(lob1, buffer_in, size_in));
    ASSERT_EQ(size_in, OCI_LobGetLength(lob1));
    ASSERT_TRUE(OCI_LobAssign(lob2, lob1));
    ASSERT_EQ(size_in, OCI_LobGetLength(lob2));
    ASSERT_EQ(size_in, OCI_LobRead(lob2, buffer_out, size_in));
    ASSERT_EQ(0, memcmp(buffer_in, buffer_out, size_in));

    ASSERT_TRUE(OCI_LobFree(lob1));
    ASSERT_TRUE(OCI_LobFree(lob2));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());
}

TEST_P(TestLob, SeekAndGetOffset)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    auto type = GetParam();

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    const auto lob = OCI_LobCreate(conn, type);
   
    void *buffer = GetBufferData();
    unsigned int size = GetBufferSize(type);

    ASSERT_EQ(size, OCI_LobWrite(lob, buffer, size));
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
    ASSERT_EQ(GetBufferOffset(type, size), OCI_LobGetOffset(lob));

    ASSERT_TRUE(OCI_LobFree(lob));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());
}

TEST_P(TestLob, Truncate)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, nullptr, OCI_ENV_DEFAULT));

    auto type = GetParam();

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    const auto lob = OCI_LobCreate(conn, type);

    void *buffer = GetBufferData();
    unsigned int size = GetBufferSize(type);
    unsigned int newSize = size + size / 2;

    ASSERT_EQ(size, OCI_LobWrite(lob, buffer, size));
    ASSERT_EQ(size, OCI_LobGetLength(lob));

    ASSERT_TRUE(OCI_LobSeek(lob, size-1, OCI_SEEK_SET));
    ASSERT_EQ(size - 1, OCI_LobGetOffset(lob));

    ASSERT_TRUE(OCI_LobTruncate(lob, size / 2));
    ASSERT_EQ(size / 2, OCI_LobGetLength(lob));
    ASSERT_EQ(size / 2, OCI_LobGetOffset(lob));

    ASSERT_EQ(size, OCI_LobWrite(lob, buffer, size));
    ASSERT_EQ(newSize, OCI_LobGetLength(lob));

    otext newBuffer[1204] = OTEXT("");
    ASSERT_TRUE(OCI_LobSeek(lob, 0, OCI_SEEK_SET));
    ASSERT_EQ(newSize, OCI_LobRead(lob, newBuffer, newSize));
    ASSERT_EQ(newSize, OCI_LobGetOffset(lob));

    ASSERT_TRUE(OCI_LobFree(lob));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());
}


INSTANTIATE_TEST_CASE_P(TestLob, TestLob, ::testing::ValuesIn(LobTypes));
