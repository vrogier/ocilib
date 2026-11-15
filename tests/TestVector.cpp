#include "ocilib_tests.h"
#include "NullArgs.h"

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

    OCI_ColumnGetFullSqlType(col, buffer, 256);

    ASSERT_EQ(typeName, String(buffer));  
}

namespace TestCApi
{
    TEST(TestVector, DescribeTableVectorColumn_ReturnsVectorInfo)
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

    TEST(TestVector, CreateVector_ReturnsTypedVector)
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

    TEST(TestVector, ArrayCreate_ReturnsVectorArray)
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

    TEST(TestVector, SetValueOnVector_StoresValues)
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

    TEST(TestVector, ToTextOnVector_RendersValues)
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

    TEST(TestVector, FromTextOnVector_ParsesValues)
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

    TEST(TestVector, BindVectorOnInsert_BindsValues)
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

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("INSERT INTO TestBindVector values(:v)")));
        ASSERT_NE(nullptr, OCI_StatementBindVector(stmt, OTEXT(":v"), vect));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));
    
        ASSERT_TRUE(OCI_VectorFree(vect));

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select value from TestBindVector")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        vect = OCI_ResultsetGetVector(rs, 0);
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

    TEST(TestVector, FetchVectorColumn_ReturnsVector)
    {
        ExecDML(OTEXT("CREATE TABLE TestFetchVector (value VECTOR(*, FLOAT32))"));
        ExecDML(OTEXT("INSERT INTO TestFetchVector values('[1.1, 2.2, 3.3, 4.4]')"));

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select value from TestFetchVector")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        auto vect = OCI_ResultsetGetVector(rs, 0);
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

    TEST(TestVector, FetchVectorAsString_ReturnsText)
    {
        ExecDML(OTEXT("CREATE TABLE TestFetchVectorAsString (value VECTOR(*, INT8))"));
        ExecDML(OTEXT("INSERT INTO TestFetchVectorAsString values('[1, 2, 3, 4]')"));

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select value from TestFetchVectorAsString")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        auto str = OCI_ResultsetGetString(rs, 0);

        ASSERT_EQ(String(OTEXT("[1,2,3,4]")), String(str));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TABLE  TestFetchVectorAsString"));
    }

    TEST(TestVector, RegisterVectorOnReturningInto_ReturnsVector)
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

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("update TestRegisterVector set code = 2 returning value into :value")));
        ASSERT_TRUE(OCI_StatementRegisterVector(stmt, OTEXT(":value")));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        auto rslt = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rslt);

          ASSERT_TRUE(OCI_ResultsetFetchNext(rslt));

        auto str = OCI_ResultsetGetString(rslt, 0);

        ASSERT_EQ(String(OTEXT("[1,2,3,4]")), str);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TABLE TestRegisterVector"));
    }

    TEST(TestVector, Create_PreClient23_4_Fails)
    {
        /* vectors are a 23ai feature */

        const auto env = GetSpecificEnvironment(nullptr, OCI_21_3);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        ASSERT_EQ(nullptr, OCI_VectorCreate(conn));
        ASSERT_NE(nullptr, OCI_GetLastError());

        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }

    /* sparse vectors require both client and server >= Oracle 23.7 */

    static bool IsSparseVectorSupported(OCI_Environment* env, OCI_Connection* conn)
    {
        return OCI_EnvironmentGetOCIRuntimeVersion(env) >= OCI_23_7 &&
               OCI_ConnectionGetVersion(conn) >= OCI_23_7;
    }

    TEST(TestVector, IsSparseOnDenseVector_ReturnsFalse)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto vect = OCI_VectorCreate(conn);
        ASSERT_NE(nullptr, vect);

        char values[4] = { 1, 3, 2, 4 };

        ASSERT_TRUE(OCI_VectorSetValues(vect, OCI_VEC_INT8, 4, values));

        EXPECT_FALSE(OCI_VectorIsSparse(vect));

        ASSERT_TRUE(OCI_VectorFree(vect));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestVector, SetSparseValuesThenIsSparse_ReturnsTrue)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        if (!IsSparseVectorSupported(env, conn))
        {
            ASSERT_TRUE(OCI_ConnectionFree(conn));
            return; /* sparse vectors require Oracle 23.7+ */
        }

        const auto vect = OCI_VectorCreate(conn);
        ASSERT_NE(nullptr, vect);

        OCI_UInt32 indices[2] = { 2, 4 };
        OCI_Float  values[2]  = { 1.5f, 2.5f };

        ASSERT_TRUE(OCI_VectorSetSparseValues(vect, OCI_VEC_FLOAT32, 5, 2, indices, values));

        EXPECT_TRUE(OCI_VectorIsSparse(vect));

        OCI_UInt32 dimensions = 0;
        OCI_UInt32 format     = OCI_VEC_FLEX;

        ASSERT_TRUE(OCI_VectorGetInfo(vect, &format, &dimensions));
        EXPECT_EQ(5u, dimensions);
        EXPECT_EQ(static_cast<OCI_UInt32>(OCI_VEC_FLOAT32), format);

        ASSERT_TRUE(OCI_VectorFree(vect));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestVector, GetSparseValuesOnSparseVector_ReturnsNonZeroEntries)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        if (!IsSparseVectorSupported(env, conn))
        {
            ASSERT_TRUE(OCI_ConnectionFree(conn));
            return; /* sparse vectors require Oracle 23.7+ */
        }

        const auto vect = OCI_VectorCreate(conn);
        ASSERT_NE(nullptr, vect);

        OCI_UInt32 indices_in[2] = { 2, 4 };
        OCI_Float  values_in[2]  = { 1.5f, 2.5f };

        ASSERT_TRUE(OCI_VectorSetSparseValues(vect, OCI_VEC_FLOAT32, 5, 2, indices_in, values_in));

        OCI_UInt32 indices_out[5] = { 0 };
        OCI_Float  values_out[5]  = { 0.0f };
        OCI_UInt32 count          = 5;

        ASSERT_TRUE(OCI_VectorGetSparseValues(vect, indices_out, values_out, &count));

        EXPECT_EQ(2u, count);
        EXPECT_EQ(2u, indices_out[0]);
        EXPECT_EQ(4u, indices_out[1]);
        EXPECT_FLOAT_EQ(1.5f, values_out[0]);
        EXPECT_FLOAT_EQ(2.5f, values_out[1]);

        ASSERT_TRUE(OCI_VectorFree(vect));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestVector, GetSparseValuesWithInsufficientCapacity_Fails)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        if (!IsSparseVectorSupported(env, conn))
        {
            ASSERT_TRUE(OCI_ConnectionFree(conn));
            return; /* sparse vectors require Oracle 23.7+ */
        }

        const auto vect = OCI_VectorCreate(conn);
        ASSERT_NE(nullptr, vect);

        OCI_UInt32 indices_in[2] = { 2, 4 };
        OCI_Float  values_in[2]  = { 1.5f, 2.5f };

        ASSERT_TRUE(OCI_VectorSetSparseValues(vect, OCI_VEC_FLOAT32, 5, 2, indices_in, values_in));

        OCI_UInt32 indices_out[1] = { 0 };
        OCI_Float  values_out[1]  = { 0.0f };
        OCI_UInt32 count          = 1; /* capacity < actual non-zero count */

        EXPECT_FALSE(OCI_VectorGetSparseValues(vect, indices_out, values_out, &count));

        ASSERT_TRUE(OCI_VectorFree(vect));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestVector, SetSparseValuesWithFloat16Format_Fails)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        if (!IsSparseVectorSupported(env, conn))
        {
            ASSERT_TRUE(OCI_ConnectionFree(conn));
            return; /* sparse vectors require Oracle 23.7+ */
        }

        const auto vect = OCI_VectorCreate(conn);
        ASSERT_NE(nullptr, vect);

        OCI_UInt32 indices[1] = { 1 };
        OCI_Float  values[1]  = { 1.5f };

        /* FLOAT16 is not a valid format for sparse vectors */

        EXPECT_FALSE(OCI_VectorSetSparseValues(vect, OCI_VEC_FLOAT16, 1, 1, indices, values));

        ASSERT_TRUE(OCI_VectorFree(vect));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestVector, DescribeSparseVectorColumn_ReportsSparseFlag)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        if (!IsSparseVectorSupported(env, conn))
        {
            ASSERT_TRUE(OCI_ConnectionFree(conn));
            return; /* sparse vectors require Oracle 23.7+ */
        }

        ExecDML(OTEXT("CREATE TABLE TestDescribeSparseVector (value VECTOR(8, FLOAT32, SPARSE))"));

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestDescribeSparseVector"), OCI_TIF_TABLE);
        ASSERT_NE(nullptr, typinf);

        CheckColumnFullType(0, typinf, OTEXT("VALUE"), OTEXT("VECTOR(8, FLOAT32, SPARSE)"),
                            OCI_CDT_VECTOR, OCI_VEC_FLOAT32, 8);

        const auto col = OCI_TypeInfoGetColumn(typinf, 0);
        ASSERT_NE(nullptr, col);

        EXPECT_TRUE(OCI_ColumnIsVectorSparse(col));

        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TABLE TestDescribeSparseVector"));
    }

    TEST(TestVector, BindSparseVectorOnInsert_PreservesSparseStorage)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        if (!IsSparseVectorSupported(env, conn))
        {
            ASSERT_TRUE(OCI_ConnectionFree(conn));
            return; /* sparse vectors require Oracle 23.7+ */
        }

        ExecDML(OTEXT("CREATE TABLE TestBindSparseVector (value VECTOR(8, FLOAT32, SPARSE))"));

        auto vect = OCI_VectorCreate(conn);
        ASSERT_NE(nullptr, vect);

        OCI_UInt32 indices[2] = { 2, 5 };
        OCI_Float  values[2]  = { 1.5f, 2.5f };

        ASSERT_TRUE(OCI_VectorSetSparseValues(vect, OCI_VEC_FLOAT32, 8, 2, indices, values));

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("INSERT INTO TestBindSparseVector values(:v)")));
        ASSERT_NE(nullptr, OCI_StatementBindVector(stmt, OTEXT(":v"), vect));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        ASSERT_TRUE(OCI_VectorFree(vect));

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select value from TestBindSparseVector")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        vect = OCI_ResultsetGetVector(rs, 0);
        ASSERT_NE(nullptr, vect);

        EXPECT_TRUE(OCI_VectorIsSparse(vect));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TABLE TestBindSparseVector"));
    }

    TEST(TestVector, ImmediateWithSparseVectorArg_PreservesSparsity)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        if (!IsSparseVectorSupported(env, conn))
        {
            ASSERT_TRUE(OCI_ConnectionFree(conn));
            return; /* sparse vectors require Oracle 23.7+ */
        }

        ExecDML(OTEXT("CREATE TABLE TestAssignSparseVector (value VECTOR(8, FLOAT32, SPARSE))"));
        ExecDML(OTEXT("INSERT INTO TestAssignSparseVector values('[8,[2,5],[1.5,2.5]]')"));

        const auto vect = OCI_VectorCreate(conn);
        ASSERT_NE(nullptr, vect);

        /* OCI_ARG_VECTOR fetch goes through OcilibVectorAssign */

        ASSERT_TRUE(OCI_ConnectionImmediate(conn,
            OTEXT("select value from TestAssignSparseVector"),
            OCI_ARG_VECTOR, vect));

        EXPECT_TRUE(OCI_VectorIsSparse(vect));

        ASSERT_TRUE(OCI_VectorFree(vect));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TABLE TestAssignSparseVector"));
    }

    TEST(TestVector, SetSparseValues_PreClient23_7_Fails)
    {
        /* sparse vectors are a 23.7 feature */

        const auto env = GetSpecificEnvironment(nullptr, OCI_23_4);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto vect = OCI_VectorCreate(conn);
        ASSERT_NE(nullptr, vect);

        OCI_UInt32 indices[1] = { 1 };
        OCI_Float  values[1]  = { 1.5f };

        EXPECT_FALSE(OCI_VectorSetSparseValues(vect, OCI_VEC_FLOAT32, 1, 1, indices, values));
        EXPECT_NE(nullptr, OCI_GetLastError());

        ASSERT_TRUE(OCI_VectorFree(vect));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }

    TEST(TestVector, GetSparseValues_PreClient23_7_Fails)
    {
        /* sparse vectors are a 23.7 feature */

        const auto env = GetSpecificEnvironment(nullptr, OCI_23_4);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto vect = OCI_VectorCreate(conn);
        ASSERT_NE(nullptr, vect);

        OCI_UInt32 indices[1] = { 0 };
        OCI_Float  values[1]  = { 0.0f };
        OCI_UInt32 count      = 1;

        EXPECT_FALSE(OCI_VectorGetSparseValues(vect, indices, values, &count));
        EXPECT_NE(nullptr, OCI_GetLastError());

        ASSERT_TRUE(OCI_VectorFree(vect));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }


    class TestVectorNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestVectorNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestVectorNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "VectorArrayCreate", [] { return nullptr == OCI_VectorArrayCreate(nullptr, 1); } },
            { "VectorArrayFree", [] { return !OCI_VectorArrayFree(nullptr); } },
            { "VectorCreate", [] { return nullptr == OCI_VectorCreate(nullptr); } },
            { "VectorFree", [] { return !OCI_VectorFree(nullptr); } },
            { "VectorFromText", [] { return !OCI_VectorFromText(nullptr, OTEXT("x"), 1, 1, 1); } },
            { "VectorGetInfo", [] { return !OCI_VectorGetInfo(nullptr, nullptr /* OCI_VectorFormat * */, NullArgOut<OCI_UInt32>()); } },
            { "VectorGetSparseValues", [] { return !OCI_VectorGetSparseValues(nullptr, NullArgOut<OCI_UInt32>(), NullArgBuffer(), NullArgOut<OCI_UInt32>()); } },
            { "VectorGetValues", [] { return !OCI_VectorGetValues(nullptr, NullArgBuffer()); } },
            { "VectorIsSparse", [] { return !OCI_VectorIsSparse(nullptr); } },
            { "VectorSetSparseValues", [] { return !OCI_VectorSetSparseValues(nullptr, 1, 1, 1, NullArgOut<OCI_UInt32>(), NullArgBuffer()); } },
            { "VectorSetValues", [] { return !OCI_VectorSetValues(nullptr, 1, 1, NullArgBuffer()); } },
            { "VectorToText", [] { return !OCI_VectorToText(nullptr, NullArgOut<OCI_UInt32>(), NullArgText()); } },
        }));

}