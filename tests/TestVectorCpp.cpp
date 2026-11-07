#include "ocilib_tests.h"

using namespace ocilib;

static void CheckColumnFullType
(
    OCI_Int32 index,
    OCI_TypeInfo* typinf,
    const String& name,
    const String& typeName,
    OCI_UInt32 type,
    OCI_UInt32 subtype,
    OCI_UInt32 dimension
)
{
    const auto col = OCI_TypeInfoGetColumn(typinf, index);
    ASSERT_NE(nullptr, col);

    ASSERT_EQ(type, OCI_ColumnGetType(col));
    ASSERT_EQ(name, String(OCI_ColumnGetName(col)));
    ASSERT_EQ(subtype, OCI_ColumnGetSubType(col));
    ASSERT_EQ(dimension, OCI_ColumnGetDimension(col));

    otext buffer[256] = OTEXT("");

    OCI_ColumnGetFullSQLType(col, buffer, 256);

    ASSERT_EQ(typeName, String(buffer));  
}

namespace TestCppApi
{
    TEST(TestVectorCpp, DescribeTableVectorCpp)
    {
        ExecDML(OTEXT("DROP TABLE TestDescribeVector"));
        ExecDML
        (
            OTEXT("CREATE TABLE TestDescribeVector")
            OTEXT("(")
            OTEXT("    v1 VECTOR,")
            OTEXT("    v2 VECTOR(64, *),")
            OTEXT("    v3 VECTOR(*, INT8),")
            OTEXT("    v4 VECTOR(128, INT8),")
            OTEXT("    v5 VECTOR(256, FLOAT32),")
            OTEXT("    v6 VECTOR(256, FLOAT64),")
            OTEXT("    v7 VECTOR(512, BINARY)")
            OTEXT(")")
           );
     
        Environment& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        TypeInfo tif(con,  OTEXT("TestDescribeVector"), TypeInfo::Table);

        CheckColumnFullType(0, tif, OTEXT("V1"), OTEXT("VECTOR(*, *)"), TypeVector, Vector::Flex, 0);
        CheckColumnFullType(1, tif, OTEXT("V2"), OTEXT("VECTOR(64, *)"), TypeVector, Vector::Flex,64);
        CheckColumnFullType(2, tif, OTEXT("V3"), OTEXT("VECTOR(*, INT8)"), TypeVector, Vector::Int8, 0);
        CheckColumnFullType(3, tif, OTEXT("V4"), OTEXT("VECTOR(128, INT8)"), TypeVector, Vector::Int8, 128);
        CheckColumnFullType(4, tif, OTEXT("V5"), OTEXT("VECTOR(256, FLOAT32)"), TypeVector, Vector::Float32, 256);
        CheckColumnFullType(5, tif, OTEXT("V6"), OTEXT("VECTOR(256, FLOAT64)"), TypeVector, Vector::Float64, 256);
        CheckColumnFullType(6, tif, OTEXT("V7"), OTEXT("VECTOR(512, BINARY)"), TypeVector, Vector::Binary, 512);

        

        ExecDML(OTEXT("DROP TABLE TestDescribeVector"));
    }

    TEST(TestVectorCpp, CreateCpp)
    {
        Environment& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Vector vect(con);

        
    }

    TEST(TestVectorCpp, SetValueArrayCpp)
    {
        Environment& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Vector vect(con);

        std::array<char, 4> values1 = { 1, 3, 2, 4 };
        std::array<char, 4> values2 = { 0, 0, 0, 0 };


        vect.Set(values1.data(), static_cast<OCI_UInt32>(values1.size()));

        const OCI_UInt32 dimensions1 = 4;
        const Vector::VectorFormat format1 = Vector::Int8;

        ASSERT_EQ(dimensions1, vect.GetDimensionCount());
        ASSERT_EQ(format1, vect.GetFormat());

        vect.Get(values2.data());

        for (OCI_Int32 i = 0; i < 4; i++)
        {
            ASSERT_EQ(values1[i], values2[i]);
        }

        
    }

    TEST(TestVectorCpp, SetValueVectorCpp)
    {
        Environment& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Vector vect(con);

        static std::vector<char> values1 = { 1, 3, 2, 4 };


        vect.Set(values1);

        auto values2 = vect.Get<char>();
        ASSERT_EQ(values1.size(), values2.size());

        for (OCI_Int32 i = 0; i < 4; i++)
        {
            ASSERT_EQ(values1[i], values2[i]);
        }

        
    }

    TEST(TestVectorCpp, ToStringCpp)
    {
        Environment& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Vector vect(con);

        static std::vector<char> values1 = { 1, 3, 2, 4 };
    
        vect.Set(values1);

        ASSERT_EQ(String(OTEXT("[1,3,2,4]")), vect.ToString());
    
        
    }

    TEST(TestVectorCpp, FromTextCpp)
    {
        Environment& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Vector vect(con);

        vect.FromString(OTEXT("[1.1,2.2,3.3,4.4]"), Vector::Float32, 4);

        ASSERT_EQ(4, vect.GetDimensionCount());
        ASSERT_EQ(Vector::Float32, vect.GetFormat());

        auto values = vect.Get<OCI_Float>();

        ASSERT_FLOAT_EQ(1.1f, values[0]);        
        ASSERT_FLOAT_EQ(2.2f, values[1]);
        ASSERT_FLOAT_EQ(3.3f, values[2]);
        ASSERT_FLOAT_EQ(4.4f, values[3]);        
    }

    TEST(TestVectorCpp, BindCpp)
    {
        ExecDML(OTEXT("CREATE TABLE TestBindVector (value VECTOR(*, FLOAT32))"));

        Environment& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Vector vect(con);

        static std::vector<char> values1 = { 1, 3, 2, 4 };
        vect.Set(values1);

        Statement stmt(con);
        stmt.Prepare(OTEXT("INSERT INTO TestBindVector values(:v)"));
        stmt.Bind( OTEXT(":v"), vect, BindInfo::In);
        stmt.ExecutePrepared();
    
        stmt.Execute(OTEXT("select value from TestBindVector"));

        auto rs = stmt.GetResultset();
        rs.Next();
        vect = rs.Get<Vector>(0);

        ASSERT_EQ(4, vect.GetDimensionCount());
        ASSERT_EQ(Vector::Float32, vect.GetFormat());
            
        auto values2 = vect.Get<OCI_Float>();
        ASSERT_EQ(4, values2.size());
    
        ASSERT_FLOAT_EQ(values1[0], values2[0]);        
        ASSERT_FLOAT_EQ(values1[1], values2[1]);
        ASSERT_FLOAT_EQ(values1[2], values2[2]);
        ASSERT_FLOAT_EQ(values1[3], values2[3]);  

        ExecDML(OTEXT("DROP TABLE  TestBindVector"));
    }

    TEST(TestVectorCpp, FetchCpp)
    {
        ExecDML(OTEXT("CREATE TABLE TestFetchVector (value VECTOR(*, FLOAT32))"));
        ExecDML(OTEXT("INSERT INTO TestFetchVector values('[1.1, 2.2, 3.3, 4.4]')"));

        ExecDML(OTEXT("CREATE TABLE TestBindVector (value VECTOR(*, FLOAT32))"));

        Environment& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Statement stmt(con);

        stmt.Execute(OTEXT("select value from TestFetchVector"));

        auto rs = stmt.GetResultset();
        rs.Next();
        auto vect = rs.Get<Vector>(0);

        ASSERT_EQ(4, vect.GetDimensionCount());
        ASSERT_EQ(Vector::Float32, vect.GetFormat());
    
        auto values = vect.Get<OCI_Float>();    
        ASSERT_EQ(4, values.size());
    
        ASSERT_FLOAT_EQ(1.1f, values[0]);        
        ASSERT_FLOAT_EQ(2.2f, values[1]);
        ASSERT_FLOAT_EQ(3.3f, values[2]);
        ASSERT_FLOAT_EQ(4.4f, values[3]);

        ExecDML(OTEXT("DROP TABLE  TestFetchVector"));
    }

    TEST(TestVectorCpp, FetchAsStringCpp)
    {
        ExecDML(OTEXT("CREATE TABLE TestFetchVectorAsString (value VECTOR(*, INT8))"));
        ExecDML(OTEXT("INSERT INTO TestFetchVectorAsString values('[1, 2, 3, 4]')"));

        Environment& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Statement stmt(con);

        stmt.Execute(OTEXT("select value from TestFetchVectorAsString"));

        auto rs = stmt.GetResultset();
        rs.Next();

        ASSERT_EQ(String(OTEXT("[1,2,3,4]")), rs.Get<String>(0));

        ExecDML(OTEXT("DROP TABLE  TestFetchVectorAsString"));
    }

    TEST(TestVectorCpp, RegisterVectorCpp)
    {
        ExecDML(OTEXT("CREATE TABLE TestRegisterVector (code int, value VECTOR(*, INT8))"));
        ExecDML(OTEXT("TRUNCATE TABLE TestRegisterVector"));
        ExecDML(OTEXT("INSERT INTO TestRegisterVector values(1, '[1, 2, 3, 4]')"));

        Environment& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Statement stmt(con);
        stmt.Prepare(OTEXT("update TestRegisterVector set code = 2 returning value into :value"));
        stmt.Register<Vector>(OTEXT(":value"));
        stmt.ExecutePrepared();

        auto rs = stmt.GetResultset();
        rs.Next();

        ASSERT_EQ(String(OTEXT("[1,2,3,4]")),  rs.Get<String>(0));

        ExecDML(OTEXT("DROP TABLE TestRegisterVector"));
    }

    TEST(TestVectorCpp, NullToString)
    {
        Vector v;
        ASSERT_EQ(String(OTEXT("NULL")), v.ToString());
    }
}






