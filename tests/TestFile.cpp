#include "ocilib_tests.h"
#include "NullArgs.h"

namespace TestCApi
{
    TEST(TestFile, CreateAndGetters)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto file = OCI_FileCreate(conn, OCI_BFILE);
        ASSERT_NE(nullptr, file);

        ASSERT_EQ(OCI_BFILE, OCI_FileGetType(file));
        ASSERT_EQ(conn, OCI_FileGetConnection(file));

        ASSERT_TRUE(OCI_FileSetName(file, OTEXT("TEST_DIR"), OTEXT("test.txt")));

        ASSERT_EQ(String(OTEXT("TEST_DIR")), String(OCI_FileGetDirectory(file)));
        ASSERT_EQ(String(OTEXT("test.txt")), String(OCI_FileGetName(file)));

        ASSERT_EQ(0u, OCI_FileGetOffset(file));

        ASSERT_TRUE(OCI_FileFree(file));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestFile, ArrayCreateAndFree)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto files = OCI_FileArrayCreate(conn, OCI_BFILE, ARRAY_SIZE);
        ASSERT_NE(nullptr, files);

        for (OCI_Int32 i = 0; i < ARRAY_SIZE; i++)
        {
            ASSERT_NE(nullptr, files[i]);
        }

        ASSERT_TRUE(OCI_FileArrayFree(files));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestFile, AssignAndEquality)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto file1 = OCI_FileCreate(conn, OCI_BFILE);
        ASSERT_NE(nullptr, file1);

        const auto file2 = OCI_FileCreate(conn, OCI_BFILE);
        ASSERT_NE(nullptr, file2);

        const auto file3 = OCI_FileCreate(conn, OCI_BFILE);
        ASSERT_NE(nullptr, file3);

        ASSERT_TRUE(OCI_FileSetName(file1, OTEXT("TEST_DIR"), OTEXT("test.txt")));
        ASSERT_TRUE(OCI_FileSetName(file2, OTEXT("TEST_DIR"), OTEXT("test.txt")));

        ASSERT_TRUE(OCI_FileIsEqual(file1, file2));

        ASSERT_TRUE(OCI_FileAssign(file3, file1));
        ASSERT_EQ(String(OTEXT("test.txt")), String(OCI_FileGetName(file3)));
        ASSERT_TRUE(OCI_FileIsEqual(file1, file3));

        ASSERT_TRUE(OCI_FileFree(file1));
        ASSERT_TRUE(OCI_FileFree(file2));
        ASSERT_TRUE(OCI_FileFree(file3));

        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }
#ifdef _WINDOWS
    /* real BFILE I/O needs a DIRECTORY that the DB service account can
       read: the Oracle home bin directory holding the loaded oci.dll is
       guaranteed to exist and be readable on this local database */

    class TestFileBfile : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            char modpath[MAX_PATH] = { 0 };
            const HMODULE hmod = GetModuleHandleA("oci.dll");
            ASSERT_NE(nullptr, hmod);
            ASSERT_NE(0u, GetModuleFileNameA(hmod, modpath, MAX_PATH));

            char* sep = strrchr(modpath, '\\');
            ASSERT_NE(nullptr, sep);
            *sep = 0;

            otext dir[MAX_PATH];
            for (OCI_Size i = 0; i < strlen(modpath) + 1; i++)
            {
                dir[i] = (otext) modpath[i];
            }

            otext ddl[1024];
            osprintf(ddl, OTEXT("create or replace directory TEST_FILE_DIR as '%s'"), dir);
            ExecDML(ddl);
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop directory TEST_FILE_DIR"));
        }

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            file = OCI_FileCreate(conn, OCI_BFILE);
            ASSERT_NE(nullptr, file);

            ASSERT_TRUE(OCI_FileSetName(file, OTEXT("TEST_FILE_DIR"), OTEXT("oci.dll")));
        }

        void TearDown() override
        {
            OCI_FileFree(file);
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
        OCI_File*       file{ nullptr };
    };

    TEST_F(TestFileBfile, ExistsOnBfile_ReturnsTrue)
    {
        ASSERT_TRUE(OCI_FileExists(file));
    }

    TEST_F(TestFileBfile, GetInfoOnUnnamedLocator_Fails)
    {
        /* an unnamed locator goes through OcilibFileGetInfo() */

        const auto unnamed = OCI_FileCreate(conn, OCI_BFILE);
        ASSERT_NE(nullptr, unnamed);

        OCI_FileGetName(unnamed);
        OCI_FileGetDirectory(unnamed);

        ASSERT_TRUE(OCI_FileFree(unnamed));
    }

    TEST_F(TestFileBfile, GetSizeOnBfile_ReturnsSize)
    {
        ASSERT_GT(OCI_FileGetSize(file), 0u);
    }

    TEST_F(TestFileBfile, OpenOnBfile_Succeeds)
    {
        ASSERT_FALSE(OCI_FileIsOpen(file));
        ASSERT_TRUE(OCI_FileOpen(file));
        ASSERT_TRUE(OCI_FileIsOpen(file));
    }

    TEST_F(TestFileBfile, CloseOnOpenBfile_Succeeds)
    {
        ASSERT_TRUE(OCI_FileOpen(file));
        ASSERT_TRUE(OCI_FileClose(file));
        ASSERT_FALSE(OCI_FileIsOpen(file));
    }

    TEST_F(TestFileBfile, SeekAbsoluteOnBfile_Succeeds)
    {
        /* offsets are 0-based */

        ASSERT_TRUE(OCI_FileOpen(file));
        ASSERT_TRUE(OCI_FileSeek(file, 0, OCI_SEEK_SET));
        ASSERT_EQ((OCI_UInt64) 0, OCI_FileGetOffset(file));
    }

    TEST_F(TestFileBfile, SeekRelativeOnBfile_Succeeds)
    {
        ASSERT_TRUE(OCI_FileOpen(file));
        ASSERT_TRUE(OCI_FileSeek(file, 0, OCI_SEEK_SET));
        ASSERT_TRUE(OCI_FileSeek(file, 4, OCI_SEEK_CUR));
        ASSERT_EQ((OCI_UInt64) 4, OCI_FileGetOffset(file));
    }

    TEST_F(TestFileBfile, SeekFromEndOnBfile_Succeeds)
    {
        const auto size = OCI_FileGetSize(file);
        ASSERT_GT(size, 0u);

        ASSERT_TRUE(OCI_FileOpen(file));
        ASSERT_TRUE(OCI_FileSeek(file, 1, OCI_SEEK_END));
        ASSERT_EQ(size - 1, OCI_FileGetOffset(file));
    }

    TEST_F(TestFileBfile, SeekBeyondEnd_Fails)
    {
        const auto size = OCI_FileGetSize(file);
        ASSERT_GT(size, 0u);

        ASSERT_TRUE(OCI_FileOpen(file));
        ASSERT_FALSE(OCI_FileSeek(file, size + 1000, OCI_SEEK_SET));
        ASSERT_FALSE(OCI_FileSeek(file, size + 1000, OCI_SEEK_END));
    }

    TEST_F(TestFileBfile, SeekWithInvalidMode_Fails)
    {
        ASSERT_TRUE(OCI_FileOpen(file));
        ASSERT_FALSE(OCI_FileSeek(file, 1, static_cast<OCI_SeekMode>(99)));
    }

    TEST_F(TestFileBfile, ReadOnBfile_ReturnsData)
    {
        ASSERT_TRUE(OCI_FileOpen(file));
        ASSERT_TRUE(OCI_FileSeek(file, 0, OCI_SEEK_SET));

        unsigned char buf[2048];
        memset(buf, 0, sizeof(buf));

        const OCI_UInt32 n1 = OCI_FileRead(file, buf, 1024);
        ASSERT_EQ(1024u, n1);
        ASSERT_EQ((OCI_UInt64) 1024, OCI_FileGetOffset(file));

        const OCI_UInt32 n2 = OCI_FileRead(file, buf, 1024);
        ASSERT_EQ(1024u, n2);
    }
#endif

    TEST(TestFile, Read_WithoutOpen_ReturnsZero)
    {
        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);
        const auto file = OCI_FileCreate(conn, OCI_BFILE);
        ASSERT_NE(nullptr, file);
        unsigned char buffer[64];

        ASSERT_EQ(0u, OCI_FileRead(file, buffer, sizeof(buffer)));

        ASSERT_TRUE(OCI_FileFree(file));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
    class TestFileNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestFileNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestFileNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "FileArrayCreate", [] { return nullptr == OCI_FileArrayCreate(nullptr, 1, 1); } },
            { "FileArrayFree", [] { return !OCI_FileArrayFree(nullptr); } },
            { "FileAssign", [] { return !OCI_FileAssign(nullptr, nullptr); } },
            { "FileClose", [] { return !OCI_FileClose(nullptr); } },
            { "FileCreate", [] { return nullptr == OCI_FileCreate(nullptr, 1); } },
            { "FileExists", [] { return !OCI_FileExists(nullptr); } },
            { "FileFree", [] { return !OCI_FileFree(nullptr); } },
            { "FileGetConnection", [] { return nullptr == OCI_FileGetConnection(nullptr); } },
            { "FileGetDirectory", [] { return nullptr == OCI_FileGetDirectory(nullptr); } },
            { "FileGetName", [] { return nullptr == OCI_FileGetName(nullptr); } },
            { "FileGetOffset", [] { return 0 == OCI_FileGetOffset(nullptr); } },
            { "FileGetSize", [] { return 0 == OCI_FileGetSize(nullptr); } },
            { "FileGetType", [] { return OCI_UNKNOWN == OCI_FileGetType(nullptr); } },
            { "FileIsEqual", [] { return !OCI_FileIsEqual(nullptr, nullptr); } },
            { "FileIsOpen", [] { return !OCI_FileIsOpen(nullptr); } },
            { "FileOpen", [] { return !OCI_FileOpen(nullptr); } },
            { "FileRead", [] { return 0 == OCI_FileRead(nullptr, NullArgBuffer(), 1); } },
            { "FileSeek", [] { return !OCI_FileSeek(nullptr, 1, 1); } },
            { "FileSetName", [] { return !OCI_FileSetName(nullptr, OTEXT("x"), OTEXT("x")); } },
        }),
        &NullArgCaseName);

}
