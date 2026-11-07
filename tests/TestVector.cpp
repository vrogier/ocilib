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

namespace TestCApi
{
    TEST(TestVector, DescribeTableVector)
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
     
        auto env = GetEnvironment(nullptr);
     
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestDescribeVector"), OCI_TIF_TABLE);
        ASSERT_NE(nullptr, typinf);

        CheckColumnFullType(0, typinf, OTEXT("V1"), OTEXT("VECTOR(*, *)"), OCI_CDT_VECTOR, OCI_VEC_FLEX, 0);
        CheckColumnFullType(1, typinf, OTEXT("V2"), OTEXT("VECTOR(64, *)"), OCI_CDT_VECTOR, OCI_VEC_FLEX, 64);
        CheckColumnFullType(2, typinf, OTEXT("V3"), OTEXT("VECTOR(*, INT8)"), OCI_CDT_VECTOR, OCI_VEC_INT8, 0);
        CheckColumnFullType(3, typinf, OTEXT("V4"), OTEXT("VECTOR(128, INT8)"), OCI_CDT_VECTOR, OCI_VEC_INT8, 128);
        CheckColumnFullType(4, typinf, OTEXT("V5"), OTEXT("VECTOR(256, FLOAT32)"), OCI_CDT_VECTOR, OCI_VEC_FLOAT32, 256);
        CheckColumnFullType(5, typinf, OTEXT("V6"), OTEXT("VECTOR(256, FLOAT64)"), OCI_CDT_VECTOR, OCI_VEC_FLOAT64, 256);
        CheckColumnFullType(6, typinf, OTEXT("V7"), OTEXT("VECTOR(512, BINARY)"), OCI_CDT_VECTOR, OCI_VEC_BINARY, 512);

        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TABLE TestDescribeVector"));
    }

    TEST(TestVector, Create)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto vect = OCI_VectorCreate(conn);
        ASSERT_NE(nullptr, vect);

        ASSERT_TRUE(OCI_VectorFree(vect));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestVector, ArrayCreateAndFree)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

         const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto vects = OCI_VectorArrayCreate(conn, ARRAY_SIZE);
        ASSERT_TRUE(nullptr != vects);

        for (OCI_Int32 i = 0; i < ARRAY_SIZE; i++)
        {
            ASSERT_TRUE(nullptr != vects[i]);
        }
        ASSERT_TRUE(OCI_VectorArrayFree(vects));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestVector, SetValue)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto vect = OCI_VectorCreate(conn);
        ASSERT_NE(nullptr, vect);

        char values1[4] = { 1, 3, 2, 4 };
        char values2[4] = { 0, 0, 0, 0 };

        OCI_UInt32 dimensions1 = 4;
        OCI_UInt32 format1 = OCI_VEC_INT8;

        ASSERT_TRUE(OCI_VectorSetValues(vect, format1, dimensions1, values1));

        OCI_UInt32 dimensions2 = 0;
        OCI_UInt32 format2 = OCI_VEC_FLEX;

        ASSERT_TRUE(OCI_VectorGetInfo(vect, &format2, &dimensions2));

        ASSERT_EQ(dimensions1, dimensions2);
        ASSERT_EQ(format1, format2);

        ASSERT_TRUE(OCI_VectorGetValues(vect, values2));

        for (OCI_Int32 i = 0; i < 4; i++)
        {
            ASSERT_EQ(values1[i], values2[i]);
        }

        ASSERT_TRUE(OCI_VectorFree(vect));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestVector, ToText)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto vect = OCI_VectorCreate(conn);
        ASSERT_NE(nullptr, vect);

        char values[4] = { 1, 3, 2, 4 };

        ASSERT_TRUE(OCI_VectorSetValues(vect, OCI_VEC_INT8, 4, values));

        OCI_UInt32 size = 0;
        ASSERT_TRUE(OCI_VectorToText(vect, &size, NULL));

        auto buffer = new otext[size+1];
        ASSERT_TRUE(OCI_VectorToText(vect, &size, buffer));

        ASSERT_EQ(String(OTEXT("[1,3,2,4]")), String(buffer));

        delete[] buffer;
    
        ASSERT_TRUE(OCI_VectorFree(vect));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestVector, FromText)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto vect = OCI_VectorCreate(conn);
        ASSERT_NE(nullptr, vect);

        ASSERT_TRUE(OCI_VectorFromText(vect, OTEXT("[1.1,2.2,3.3,4.4]"), 0,  OCI_VEC_FLOAT32,  4));
        
        OCI_UInt32 dimensions = 0;
        OCI_UInt32 format = OCI_VEC_FLEX;

        ASSERT_TRUE(OCI_VectorGetInfo(vect, &format, &dimensions));

        ASSERT_EQ(4, dimensions);
        ASSERT_EQ(OCI_VEC_FLOAT32, format);

        OCI_Float values[4] = { 1.1f,2.2f,3.3f,4.4f };
        ASSERT_TRUE(OCI_VectorGetValues(vect, values));

        ASSERT_FLOAT_EQ(1.1f, values[0]);        
        ASSERT_FLOAT_EQ(2.2f, values[1]);
        ASSERT_FLOAT_EQ(3.3f, values[2]);
        ASSERT_FLOAT_EQ(4.4f, values[3]);


        ASSERT_TRUE(OCI_VectorFree(vect));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestVector, Bind)
    {
        ExecDML(OTEXT("CREATE TABLE TestBindVector (value VECTOR(*, FLOAT32))"));

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        auto vect = OCI_VectorCreate(conn);
        ASSERT_NE(nullptr, vect);

        char values1[4] = { 1, 3, 2, 4 };

        ASSERT_TRUE(OCI_VectorSetValues(vect, OCI_VEC_INT8, 4, values1));

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("INSERT INTO TestBindVector values(:v)")));
        ASSERT_NE(nullptr, OCI_BindVector(stmt, OTEXT(":v"), vect));
        ASSERT_TRUE(OCI_Execute(stmt));
    
        ASSERT_TRUE(OCI_VectorFree(vect));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select value from TestBindVector")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_TRUE(OCI_FetchNext(rs));

        vect = OCI_GetVector(rs, 0);
        ASSERT_NE(nullptr, vect);

        OCI_UInt32 dimensions = 0;
        OCI_UInt32 format = OCI_VEC_FLEX;

        ASSERT_TRUE(OCI_VectorGetInfo(vect, &format, &dimensions));
        ASSERT_EQ(OCI_VEC_FLOAT32, format);
        ASSERT_EQ(4, dimensions);
    
        OCI_Float values2[4];

        ASSERT_TRUE(OCI_VectorGetValues(vect, values2));
    
        ASSERT_FLOAT_EQ(values1[0], values2[0]);        
        ASSERT_FLOAT_EQ(values1[1], values2[1]);
        ASSERT_FLOAT_EQ(values1[2], values2[2]);
        ASSERT_FLOAT_EQ(values1[3], values2[3]);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TABLE  TestBindVector"));
    }

    TEST(TestVector, Fetch)
    {
        ExecDML(OTEXT("CREATE TABLE TestFetchVector (value VECTOR(*, FLOAT32))"));
        ExecDML(OTEXT("INSERT INTO TestFetchVector values('[1.1, 2.2, 3.3, 4.4]')"));

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select value from TestFetchVector")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_TRUE(OCI_FetchNext(rs));

        auto vect = OCI_GetVector(rs, 0);
        ASSERT_NE(nullptr, vect);

        OCI_UInt32 dimensions = 0;
        OCI_UInt32 format = OCI_VEC_FLEX;

        ASSERT_TRUE(OCI_VectorGetInfo(vect, &format, &dimensions));
        ASSERT_EQ(OCI_VEC_FLOAT32, format);
        ASSERT_EQ(4, dimensions);
    
        OCI_Float values[4];

        ASSERT_TRUE(OCI_VectorGetValues(vect, values));
    
        ASSERT_FLOAT_EQ(1.1f, values[0]);        
        ASSERT_FLOAT_EQ(2.2f, values[1]);
        ASSERT_FLOAT_EQ(3.3f, values[2]);
        ASSERT_FLOAT_EQ(4.4f, values[3]);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TABLE  TestFetchVector"));
    }

    TEST(TestVector, FetchAsString)
    {
        ExecDML(OTEXT("CREATE TABLE TestFetchVectorAsString (value VECTOR(*, INT8))"));
        ExecDML(OTEXT("INSERT INTO TestFetchVectorAsString values('[1, 2, 3, 4]')"));

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select value from TestFetchVectorAsString")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_TRUE(OCI_FetchNext(rs));

        auto str = OCI_GetString(rs, 0);

        ASSERT_EQ(String(OTEXT("[1,2,3,4]")), String(str));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TABLE  TestFetchVectorAsString"));
    }

    TEST(TestVector, RegisterVector)
    {
         ExecDML(OTEXT("CREATE TABLE TestRegisterVector (code int, value VECTOR(*, INT8))"));
        ExecDML(OTEXT("TRUNCATE TABLE TestRegisterVector"));
        ExecDML(OTEXT("INSERT INTO TestRegisterVector values(1, '[1, 2, 3, 4]')"));

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("update TestRegisterVector set code = 2 returning value into :value")));
        ASSERT_TRUE(OCI_RegisterVector(stmt, OTEXT(":value")));
        ASSERT_TRUE(OCI_Execute(stmt));

        auto rslt = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rslt);

          ASSERT_TRUE(OCI_FetchNext(rslt));

        auto str = OCI_GetString(rslt, 0);

        ASSERT_EQ(String(OTEXT("[1,2,3,4]")), str);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TABLE TestRegisterVector"));
    }
}