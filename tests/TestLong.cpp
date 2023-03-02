#include "ocilib_tests.h"

using namespace ocilib;

template<class T, class U>
void TestLongInsertAndFetch
(
    unsigned int longSize,
    unsigned int pieceSize,
    ostring tableName,
    ostring sqlType
)
{ 
    ExecDML(OTEXT("create table ") +  tableName + OTEXT("(code int, content ") + sqlType + OTEXT(")"));

    Environment::Initialize();
    Environment::EnableWarnings(true);

    U longContent;

    for (auto i = 1u; i <= longSize; i++)
    {           
        longContent.push_back('A' + (i % CHAR_MAX));
    }

    Connection con(DBS, USR, PWD);
    Statement st(con);

    T lng(st);
    
    st.SetPieceSize(pieceSize);

    st.Prepare(OTEXT("insert into ") +  tableName + OTEXT("(code, content) values(1, :data)"));
    st.Bind<T>(OTEXT(":data"), lng, longSize, BindInfo::In);
    st.ExecutePrepared();

    auto written = 0u;

    while (written < longSize)
    {
        auto remaining = (std::min)(longSize - written, longSize);
        U str (std::begin(longContent) + written, std::begin(longContent) + written + remaining);
        written += lng.Write(str);
    }

    con.Commit();

    st.Execute(OTEXT("select * from ") +  tableName);
    auto rs = st.GetResultset();
    rs.Next();

    lng = rs.Get<T>(2);

    auto selectLongContent = lng.GetContent();
   
    ASSERT_EQ(longContent.size(), selectLongContent.size());

    Environment::Cleanup();
  
    ExecDML(OTEXT("drop table ") +  tableName); 
}

TEST(TestLong, BinaryInsertAndFetchSmallLongSizeBig)
{ 
     TestLongInsertAndFetch<Blong, Raw>
     (
         50,
         OCI_SIZE_PIECE_DYNAMIC_FETCH,
         OTEXT("TestLongInsertAndFetchBinary"),
         OTEXT("long raw")
     );
}

TEST(TestLong, BinaryInsertAndFetchSmallLongSizeSmall)
{ 
     TestLongInsertAndFetch<Blong, Raw>
     (
         50,
         20,
         OTEXT("TestLongInsertAndFetchBinary"),
         OTEXT("long raw")
     );}

TEST(TestLong, BinaryInsertAndFetchBigLongSizeSmall)
{ 
     TestLongInsertAndFetch<Blong, Raw>
     (
         5000,
         200,
         OTEXT("TestLongInsertAndFetchBinary"),
         OTEXT("long raw")
     );}


TEST(TestLong, StringInsertAndFetchSmallLongSizeBig)
{ 
     TestLongInsertAndFetch<Clong, ostring>
     (
         50,
         OCI_SIZE_PIECE_DYNAMIC_FETCH,
         OTEXT("TestLongInsertAndFetchString"),
         OTEXT("long")
     );
}

TEST(TestLong, StringInsertAndFetchSmallongSizeSmallL)
{ 
     TestLongInsertAndFetch<Clong, ostring>
     (
         50,
         20,
         OTEXT("TestLongInsertAndFetchString"),
         OTEXT("long")
     );
}

TEST(TestLong, StringInsertAndFetchBigLongSizeSmall)
{ 
     TestLongInsertAndFetch<Clong, ostring>
     (
         5000,
         200,
         OTEXT("TestLongInsertAndFetchString"),
         OTEXT("long")
     );
}






