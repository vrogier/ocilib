#include "ocilib_tests.h"

using namespace ocilib;

namespace TestCppApi
{

    void TestLongFetchCustom
    (
        unsigned int longSize,
        unsigned int pieceSize
    )
    {
        ExecDML(OTEXT("create table TestLongFetchCustom (code int, content long raw)"));

        Environment::Initialize();
        Environment::EnableWarnings(true);

        Raw longContent;

        for (auto i = 1u; i <= longSize; i++)
        {
            longContent.push_back('A' + (i % CHAR_MAX));
        }

        Connection con(DBS, USR, PWD);
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

        lng = rs.Get<Blong>(2);

        auto selectLongContent = lng.GetContent();

        ASSERT_EQ(longContent.size(), selectLongContent.size());

        Environment::Cleanup();

        ExecDML(OTEXT("drop table TestLongFetchCustom"));
    }


    TEST(TestLong, InsertAndFetchSmallBinaryBigLongSize)
    {
        TestLongFetchCustom(50, OCI_SIZE_PIECE_DYNAMIC_FETCH);
    }

    TEST(TestLong, InsertAndFetchSmallBinarySmallLongSize)
    {
        TestLongFetchCustom(50, 20);
    }

    TEST(TestLong, InsertAndFetchBigBinarySmallLongSize)
    {
        TestLongFetchCustom(5000, 200);
    }
}




