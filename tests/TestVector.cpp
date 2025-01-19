#include "ocilib_tests.h"

using namespace ocilib;

namespace TestCApi
{
	void CheckColumnFullType
    (
        int index,
        OCI_TypeInfo* typinf,
        const ostring& name,
        const ostring& typeName,
        unsigned int type,
        unsigned int subtype
    )
    {
        const auto col = OCI_TypeInfoGetColumn(typinf, index);
        ASSERT_NE(nullptr, col);

        ASSERT_EQ(type, OCI_GetColumnType(col));
        ASSERT_EQ(name, ostring(OCI_ColumnGetName(col)));
        ASSERT_EQ(subtype, OCI_ColumnGetSubType(col));

        otext buffer[256] = OTEXT("");

        OCI_ColumnGetFullSQLType(col, buffer, 256);

        ASSERT_EQ(typeName, ostring(buffer));  
	}

	TEST(TestVector, DescribeTableVector)
    {
        ExecDML(OTEXT("DROP TABLE TestDescribeVector"));
        ExecDML
        (
            OTEXT("CREATE TABLE TestDescribeVector")
            OTEXT("(")
            OTEXT("    v1 VECTOR,")
            OTEXT("    v2 VECTOR(8, *),")
            OTEXT("    v3 VECTOR(*, INT8),")
            OTEXT("    v4 VECTOR(8, INT8),")
            OTEXT("    v5 VECTOR(32, FLOAT32),")
            OTEXT("    v6 VECTOR(64, FLOAT64),")
            OTEXT("    v7 VECTOR(64, BINARY)")
            OTEXT(")")
           );
     
        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestDescribeVector"), OCI_TIF_TABLE);
        ASSERT_NE(nullptr, typinf);

        CheckColumnFullType(1, typinf, OTEXT("V1"), OTEXT("VECTOR(*, *)"), OCI_CDT_VECTOR, OCI_VEC_FLEX);
        CheckColumnFullType(2, typinf, OTEXT("V2"), OTEXT("VECTOR(8, *)"), OCI_CDT_VECTOR, OCI_VEC_FLEX);
        CheckColumnFullType(3, typinf, OTEXT("V3"), OTEXT("VECTOR(*, INT8)"), OCI_CDT_VECTOR, OCI_VEC_INT8);
        CheckColumnFullType(4, typinf, OTEXT("V4"), OTEXT("VECTOR(8, INT8)"), OCI_CDT_VECTOR, OCI_VEC_INT8);
        CheckColumnFullType(5, typinf, OTEXT("V5"), OTEXT("VECTOR(32, FLOAT32)"), OCI_CDT_VECTOR, OCI_VEC_FLOAT32);
        CheckColumnFullType(6, typinf, OTEXT("V6"), OTEXT("VECTOR(64, FLOAT64)"), OCI_CDT_VECTOR, OCI_VEC_FLOAT64);
        CheckColumnFullType(7, typinf, OTEXT("V7"), OTEXT("VECTOR(64, BINARY)"), OCI_CDT_VECTOR, OCI_VEC_BINARY);

        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_Cleanup());
   
        ExecDML(OTEXT("DROP TABLE TestDescribeVector"));
    }

	TEST(TestVector, Create)
	{
		ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

		const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
		ASSERT_NE(nullptr, conn);

		const auto vect = OCI_VectorCreate(conn);
		ASSERT_NE(nullptr, vect);

		ASSERT_TRUE(OCI_VectorFree(vect));

		ASSERT_TRUE(OCI_Cleanup());
	}

	TEST(TestVector, SetValue)
	{
		ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

		const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
		ASSERT_NE(nullptr, conn);

		const auto vect = OCI_VectorCreate(conn);
		ASSERT_NE(nullptr, vect);

		char values1[4] = { 1, 3, 2, 4 };
		char values2[4] = { 0, 0, 0, 0 };

		unsigned int dimensions1 = 4;
		unsigned int format1 = OCI_VEC_INT8;

		ASSERT_TRUE(OCI_VectorSetValues(vect, format1, dimensions1, values1));

		unsigned int dimensions2 = 0;
		unsigned int format2 = OCI_VEC_FLEX;

		ASSERT_TRUE(OCI_VectorGetInfo(vect, &format2, &dimensions2));

		ASSERT_EQ(dimensions1, dimensions2);
		ASSERT_EQ(format1, format2);

		ASSERT_TRUE(OCI_VectorGetValues(vect, values2));

		for (int i = 0; i < 4; i++)
		{
			ASSERT_EQ(values1[i], values2[i]);
		}

		ASSERT_TRUE(OCI_VectorFree(vect));

		ASSERT_TRUE(OCI_Cleanup());
	}

	TEST(TestVector, ToText)
	{
		ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

		const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
		ASSERT_NE(nullptr, conn);

		const auto vect = OCI_VectorCreate(conn);
		ASSERT_NE(nullptr, vect);

		char values[4] = { 1, 3, 2, 4 };

		ASSERT_TRUE(OCI_VectorSetValues(vect, OCI_VEC_INT8, 4, values));

		unsigned int size = 0;
		ASSERT_TRUE(OCI_VectorToText(vect, &size, NULL));

		auto buffer = new otext[size+1];
		ASSERT_TRUE(OCI_VectorToText(vect, &size, buffer));

		ASSERT_EQ(ostring(OTEXT("[1,3,2,4]")), ostring(buffer));

		delete[] buffer;
	
		ASSERT_TRUE(OCI_VectorFree(vect));

		ASSERT_TRUE(OCI_Cleanup());
	}

	TEST(TestVector, FromText)
	{
		ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

		const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
		ASSERT_NE(nullptr, conn);

		const auto vect = OCI_VectorCreate(conn);
		ASSERT_NE(nullptr, vect);

		ASSERT_TRUE(OCI_VectorFromText(vect, OTEXT("[1.1,2.2,3.3,4.4]"), 0,  OCI_VEC_FLOAT32,  4));
		
		unsigned int dimensions = 0;
		unsigned int format = OCI_VEC_FLEX;

		ASSERT_TRUE(OCI_VectorGetInfo(vect, &format, &dimensions));

		ASSERT_EQ(4, dimensions);
		ASSERT_EQ(OCI_VEC_FLOAT32, format);

		float values[4] = { 1.1f,2.2f,3.3f,4.4f };
		ASSERT_TRUE(OCI_VectorGetValues(vect, values));

		ASSERT_FLOAT_EQ(1.1f, values[0]);		
		ASSERT_FLOAT_EQ(2.2f, values[1]);
		ASSERT_FLOAT_EQ(3.3f, values[2]);
		ASSERT_FLOAT_EQ(4.4f, values[3]);


		ASSERT_TRUE(OCI_VectorFree(vect));

		ASSERT_TRUE(OCI_Cleanup());
	}

	TEST(TestVector, Bind)
	{
		ExecDML(OTEXT("CREATE TABLE TestBindVector (value VECTOR(*, FLOAT32))"));

		ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

		const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
		ASSERT_NE(nullptr, conn);

		auto vect = OCI_VectorCreate(conn);
		ASSERT_NE(nullptr, vect);

		char values1[4] = { 1, 3, 2, 4 };

		ASSERT_TRUE(OCI_VectorSetValues(vect, OCI_VEC_INT8, 4, values1));

		const auto stmt = OCI_StatementCreate(conn);
		ASSERT_NE(nullptr, stmt);

		ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("INSERT INTO TestBindVector values(:v)")));
        ASSERT_TRUE(OCI_BindVector(stmt, OTEXT(":v"), vect));
		ASSERT_TRUE(OCI_Execute(stmt));
	
		ASSERT_TRUE(OCI_VectorFree(vect));

		ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select value from TestBindVector")));

		const auto rs = OCI_GetResultset(stmt);
		ASSERT_NE(nullptr, rs);

		ASSERT_TRUE(OCI_FetchNext(rs));

		vect = OCI_GetVector(rs, 1);
		ASSERT_NE(nullptr, vect);

		unsigned int dimensions = 0;
		unsigned int format = OCI_VEC_FLEX;

		ASSERT_TRUE(OCI_VectorGetInfo(vect, &format, &dimensions));
		ASSERT_EQ(OCI_VEC_FLOAT32, format);
		ASSERT_EQ(4, dimensions);
	
		float values2[4];

		ASSERT_TRUE(OCI_VectorGetValues(vect, values2));
	
		ASSERT_FLOAT_EQ(values1[0], values2[0]);		
		ASSERT_FLOAT_EQ(values1[1], values2[1]);
		ASSERT_FLOAT_EQ(values1[2], values2[2]);
		ASSERT_FLOAT_EQ(values1[3], values2[3]);
		
		ASSERT_TRUE(OCI_Cleanup());

		ExecDML(OTEXT("DROP TABLE  TestBindVector"));
	}

	TEST(TestVector, Fetch)
	{
		ExecDML(OTEXT("CREATE TABLE TestFetchVector (value VECTOR(*, FLOAT32))"));
		ExecDML(OTEXT("INSERT INTO TestFetchVector values('[1.1, 2.2, 3.3, 4.4]')"));

		ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

		const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
		ASSERT_NE(nullptr, conn);

		const auto stmt = OCI_StatementCreate(conn);
		ASSERT_NE(nullptr, stmt);

		ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select value from TestFetchVector")));

		const auto rs = OCI_GetResultset(stmt);
		ASSERT_NE(nullptr, rs);

		ASSERT_TRUE(OCI_FetchNext(rs));

		auto vect = OCI_GetVector(rs, 1);
		ASSERT_NE(nullptr, vect);

		unsigned int dimensions = 0;
		unsigned int format = OCI_VEC_FLEX;

		ASSERT_TRUE(OCI_VectorGetInfo(vect, &format, &dimensions));
		ASSERT_EQ(OCI_VEC_FLOAT32, format);
		ASSERT_EQ(4, dimensions);
	
		float values[4];

		ASSERT_TRUE(OCI_VectorGetValues(vect, values));
	
		ASSERT_FLOAT_EQ(1.1f, values[0]);		
		ASSERT_FLOAT_EQ(2.2f, values[1]);
		ASSERT_FLOAT_EQ(3.3f, values[2]);
		ASSERT_FLOAT_EQ(4.4f, values[3]);
		
		ASSERT_TRUE(OCI_Cleanup());

		ExecDML(OTEXT("DROP TABLE  TestFetchVector"));
	}

	TEST(TestVector, FetchAsString)
	{
		ExecDML(OTEXT("CREATE TABLE TestFetchVectorAsString (value VECTOR(*, INT8))"));
		ExecDML(OTEXT("INSERT INTO TestFetchVectorAsString values('[1, 2, 3, 4]')"));

		ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

		const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
		ASSERT_NE(nullptr, conn);

		const auto stmt = OCI_StatementCreate(conn);
		ASSERT_NE(nullptr, stmt);

		ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select value from TestFetchVectorAsString")));

		const auto rs = OCI_GetResultset(stmt);
		ASSERT_NE(nullptr, rs);

		ASSERT_TRUE(OCI_FetchNext(rs));

		auto str = OCI_GetString(rs, 1);

		ASSERT_EQ(ostring(OTEXT("[1,2,3,4]")), str);
	
		ASSERT_TRUE(OCI_Cleanup());

		ExecDML(OTEXT("DROP TABLE  TestFetchVectorAsString"));
	}

	TEST(TestVector, RegisterVector)
    {
 		ExecDML(OTEXT("CREATE TABLE TestRegisterVector (code int, value VECTOR(*, INT8))"));
        ExecDML(OTEXT("TRUNCATE TABLE TestRegisterVector"));
		ExecDML(OTEXT("INSERT INTO TestRegisterVector values(1, '[1, 2, 3, 4]')"));

        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("update TestRegisterVector set code = 2 returning value into :value")));
        ASSERT_TRUE(OCI_RegisterVector(stmt, OTEXT(":value")));
        ASSERT_TRUE(OCI_Execute(stmt));

        auto rslt = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rslt);

  		ASSERT_TRUE(OCI_FetchNext(rslt));

		auto str = OCI_GetString(rslt, 1);

		ASSERT_EQ(ostring(OTEXT("[1,2,3,4]")), str);
	
        ASSERT_TRUE(OCI_Cleanup());

        ExecDML(OTEXT("DROP TABLE TestRegisterVector"));
    }
}

namespace TestCppApi
{
		TEST(TestVector, CreateCpp)
	{
        Environment::Initialize();

        Connection conn(DBS, USR, PWD);
		Vector vect(conn);

		std::vector<char> arr1 { 1,2,3,4 };

		vect.Set(arr1);

		auto str = vect.ToString();

		auto arr2 = vect.Get<char>();

		str = str;

	    
		Environment::Cleanup();
	}

}





