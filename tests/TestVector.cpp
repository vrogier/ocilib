#include "ocilib_tests.h"

using namespace ocilib;

namespace TestCApi
{
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
}

namespace TestCppApi
{
	TEST(TestVector, CreateCpp)
	{

	}

}






