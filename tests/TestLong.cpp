#include "ocilib_tests.h"

using namespace ocilib;

namespace TestCppApi
{

    void TestLongFetchCustom
    (
        OCI_UInt32 longSize,
        OCI_UInt32 pieceSize
    )
    {
        ExecDML(OTEXT("create table TestLongFetchCustom (code int, content long raw)"));

        Environment& env = GetEnvironmentCpp();
        env.EnableWarnings(true);

        Raw longContent;

        for (auto i = 1u; i <= longSize; i++)
        {
            longContent.push_back('A' + (i % CHAR_MAX));
        }

        Connection con(env, DBS, USR, PWD);
        Statement st(con);

        Blong lng(st);

        st.SetPieceSize(pieceSize);

        st.Prepare(OTEXT("insert into TestLongFetchCustom(code, content) values (1, :data)"));
        st.Bind<Blong>(OTEXT(":data"), lng, longSize, BindInfo::In);
        st.ExecutePrepared();

        auto written = 0u;

        while (written < longSize)
        {
            auto remaining = (std::min)(longSize - written, longSize);
            Raw bytes(std::begin(longContent) + written, std::begin(longContent) + written + remaining);
            written += lng.Write(bytes);
        }

        con.Commit();

        st.Execute(OTEXT("select * from TestLongFetchCustom"));
        auto rs = st.GetResultset();
        rs.Next();

        lng = rs.Get<Blong>(1);

        auto selectLongContent = lng.GetContent();

        ASSERT_EQ(longContent.size(), selectLongContent.size());

        unsigned char readBuf[16] = {};
        ASSERT_EQ(OCI_BLONG, OCI_LongGetType(lng));
        ASSERT_GT(OCI_LongRead(lng, readBuf, 4), 0u);

        ExecDML(OTEXT("drop table TestLongFetchCustom"));
    }

    TEST(TestLongCpp, InsertAndFetchSmallBinaryBigLongSize)
    {
        TestLongFetchCustom(50, OCI_SIZE_PIECE_DYNAMIC_FETCH);
    }

    TEST(TestLongCpp, InsertAndFetchSmallBinarySmallLongSize)
    {
        TestLongFetchCustom(50, 20);
    }

    TEST(TestLongCpp, InsertAndFetchBigBinarySmallLongSize)
    {
        TestLongFetchCustom(5000, 200);
    }

    TEST(TestLongCpp, EmptyWriteReturnsZero)
    {
        Blong lng;
        Raw empty;
        ASSERT_EQ(0u, lng.Write(empty));
    }
}

namespace TestCApi
{
    class TestClong : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("drop table TestClongData"));
            ExecDML(OTEXT("create table TestClongData (code int, content long)"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestClongData"));
        }

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);
        }

        void TearDown() override
        {
            OCI_StatementFree(stmt);
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
        OCI_Statement*  stmt{ nullptr };
    };

    TEST_F(TestClong, ClongCreate_ReturnsLongType)
    {
        const auto lg = OCI_LongCreate(stmt, OCI_CLONG);
        ASSERT_NE(nullptr, lg);
        ASSERT_EQ(OCI_CLONG, OCI_LongGetType(lg));

        ASSERT_TRUE(OCI_LongFree(lg));
    }

    TEST_F(TestClong, SetLongModeAndPieceSize_RoundTrips)
    {
        ASSERT_TRUE(OCI_SetLongMode(stmt, OCI_LONG_EXPLICIT));
        ASSERT_TRUE(OCI_SetPieceSize(stmt, 8));
    }

    TEST_F(TestClong, WritePieces_WritesLongData)
    {
        /* write two pieces then terminate with an empty write
           (OCI_LAST_PIECE arm of OcilibLongWrite) */

        const auto lg = OCI_LongCreate(stmt, OCI_CLONG);
        ASSERT_NE(nullptr, lg);

        ASSERT_TRUE(OCI_SetLongMode(stmt, OCI_LONG_EXPLICIT));
        ASSERT_TRUE(OCI_SetPieceSize(stmt, 8));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into TestClongData(code, content) values (1, :data)")));
        ASSERT_NE(nullptr, OCI_BindLong(stmt, OTEXT(":data"), lg, 40));
        ASSERT_TRUE(OCI_Execute(stmt));

        otext content[40] = OTEXT("0123456789abcdefghij");

        ASSERT_EQ(8u, OCI_LongWrite(lg, content, 8));
        ASSERT_EQ(4u, OCI_LongWrite(lg, content + 8, 4));
        ASSERT_EQ(0u, OCI_LongWrite(lg, content, 0));

        ASSERT_TRUE(OCI_Commit(conn));
        ASSERT_TRUE(OCI_LongFree(lg));
    }

    TEST_F(TestClong, ReadPieces_ReadsLongData)
    {
        /* fetch a character LONG and read it back with clamped reads */

        ExecDML(OTEXT("delete from TestClongData"));
        ExecDML(OTEXT("insert into TestClongData values (2, '0123456789ab')"));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select content from TestClongData")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto lg = OCI_GetLong(rs, 0);
        ASSERT_NE(nullptr, lg);
        ASSERT_EQ(OCI_CLONG, OCI_LongGetType(lg));
        ASSERT_NE(nullptr, OCI_LongGetBuffer(lg));
        ASSERT_EQ(12u, OCI_LongGetSize(lg));

        otext buf[64] = {};

        OCI_UInt32 total = 0;
        OCI_UInt32 n     = 0;

        while (0 != (n = OCI_LongRead(lg, buf, 4)))
        {
            total += n;
        }

        ASSERT_EQ(12u, total);
        ASSERT_EQ(String(OTEXT("0123456789ab")),
                  String(static_cast<const otext*>(OCI_LongGetBuffer(lg)), 12));
    }
}




