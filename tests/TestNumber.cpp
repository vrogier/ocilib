#include "ocilib_tests.h"
#include "test_internals.h"

namespace TestCApi
{
    template<typename T, OCI_Int32 C>
    void SetAndGetValue(T value)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto number = OCI_NumberCreate(env);
        T value_in = value, value_out{};

        ASSERT_TRUE(OCI_NumberSetValue(number, C, &value_in));
        ASSERT_TRUE(OCI_NumberGetValue(number, C, &value_out));
        ASSERT_EQ(value_in, value_out);

        ASSERT_TRUE(OCI_NumberFree(number));

    }

    template<typename T, OCI_Int32 C, typename F>
    void NumberOperation(T value1, T value2, T expected, F func)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto number = OCI_NumberCreate(env);
        T value{};

        ASSERT_TRUE(OCI_NumberSetValue(number, C, &value1));
        ASSERT_TRUE(func(number, C, &value2));
        ASSERT_TRUE(OCI_NumberGetValue(number, C, &value));
        ASSERT_EQ(expected, value);

        ASSERT_TRUE(OCI_NumberFree(number));

    }


    template<typename T, OCI_Int32 C>
    void AddValue(T value1, T value2, T expected)
    {
        NumberOperation<T, C, decltype(OCI_NumberAdd)>(value1, value2, expected, OCI_NumberAdd);
    }

    template<typename T, OCI_Int32 C>
    void SubValue(T value1, T value2, T expected)
    {
        NumberOperation<T, C, decltype(OCI_NumberSub)>(value1, value2, expected, OCI_NumberSub);
    }

    template<typename T, OCI_Int32 C>
    void MultiplyValue(T value1, T value2, T expected)
    {
        NumberOperation<T, C, decltype(OCI_NumberMultiply)>(value1, value2, expected, OCI_NumberMultiply);
    }

    template<typename T, OCI_Int32 C>
    void DivideValue(T value1, T value2, T expected)
    {
        NumberOperation<T, C, decltype(OCI_NumberDivide)>(value1, value2, expected, OCI_NumberDivide);
    }

    TEST(TestNumber, CreateAndFree)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto number = OCI_NumberCreate(env);
        ASSERT_TRUE(nullptr != number);
        ASSERT_TRUE(OCI_NumberFree(number));

    }

    TEST(TestNumber, ArrayCreateAndFree)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto numbers = OCI_NumberArrayCreate(env, ARRAY_SIZE);
        ASSERT_TRUE(nullptr != numbers);

        for (OCI_Int32 i = 0; i < ARRAY_SIZE; i++)
        {
            ASSERT_TRUE(nullptr != numbers[i]);
        }
        ASSERT_TRUE(OCI_NumberArrayFree(numbers));

    }

    TEST(TestNumber, SetAndGetValueUnsignedShort)
    {
        SetAndGetValue<OCI_UInt16, OCI_NUM_USHORT>(13);
    }

    TEST(TestNumber, SetAndGetValueSignedShort)
    {
        SetAndGetValue<OCI_Int16, OCI_NUM_SHORT>(-13);
    }

    TEST(TestNumber, SetAndGetValueUnsignedInt)
    {
        SetAndGetValue<OCI_UInt32, OCI_NUM_UINT>(99123456);
    }

    TEST(TestNumber, SetAndGetValueSignedInt)
    {
        SetAndGetValue<OCI_Int32, OCI_NUM_INT>(-99123456);
    }

    TEST(TestNumber, SetAndGetValueUnsignedBigInt)
    {
        SetAndGetValue<OCI_UInt64, OCI_NUM_BIGUINT>(81234123412341234);
    }

    TEST(TestNumber, SetAndGetValueSignedBigInt)
    {
        SetAndGetValue<OCI_Int64, OCI_NUM_BIGINT>(-34123412341234);
    }

    TEST(TestNumber, SetAndGetValueFloat)
    {
        SetAndGetValue<float, OCI_NUM_FLOAT>(3.14f);
    }

    TEST(TestNumber, SetAndGetValueDouble)
    {
        SetAndGetValue<double, OCI_NUM_DOUBLE>(3.14);
    }

    TEST(TestNumber, SetAndGetValueNumber)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto number1 = OCI_NumberCreate(env);
        const auto number2 = OCI_NumberCreate(env);
        OCI_Int32 value_in = 13, value_out = 0;

        ASSERT_TRUE(OCI_NumberSetValue(number1, OCI_NUM_INT, &value_in));
        ASSERT_TRUE(OCI_NumberSetValue(number2, OCI_NUM_NUMBER, OCI_NumberGetContent(number1)));
        ASSERT_TRUE(OCI_NumberGetValue(number2, OCI_NUM_INT, &value_out));
        ASSERT_EQ(value_in, value_out);

        ASSERT_TRUE(OCI_NumberFree(number1));
        ASSERT_TRUE(OCI_NumberFree(number2));

    }

    TEST(TestNumber, SetContent)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto number1 = OCI_NumberCreate(env);
        const auto number2 = OCI_NumberCreate(env);
        OCI_Int32 value_in = 13, value_out = 0;

        ASSERT_TRUE(OCI_NumberSetValue(number1, OCI_NUM_INT, &value_in));
        ASSERT_TRUE(OCI_NumberSetContent(number2, OCI_NumberGetContent(number1)));
        ASSERT_TRUE(OCI_NumberGetValue(number2, OCI_NUM_INT, &value_out));
        ASSERT_EQ(value_in, value_out);

        ASSERT_TRUE(OCI_NumberFree(number1));
        ASSERT_TRUE(OCI_NumberFree(number2));

    }

    TEST(TestNumber, Assign)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto number1 = OCI_NumberCreate(env);
        const auto number2 = OCI_NumberCreate(env);
        OCI_Int32 value_in = 13, value_out = 0;

        ASSERT_TRUE(OCI_NumberSetValue(number1, OCI_NUM_INT, &value_in));
        ASSERT_TRUE(OCI_NumberAssign(number2, number1));
        ASSERT_TRUE(OCI_NumberGetValue(number2, OCI_NUM_INT, &value_out));
        ASSERT_EQ(value_in, value_out);

        ASSERT_TRUE(OCI_NumberFree(number1));
        ASSERT_TRUE(OCI_NumberFree(number2));

    }

    TEST(TestNumber, FromTextDefaultFormat)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto number = OCI_NumberCreate(env);
        OCI_Int32 value_test = 123456789, value_out = 0;

        ASSERT_TRUE(OCI_NumberFromText(number, OTEXT("123456789"), OCI_STRING_FORMAT_NUM));
        ASSERT_TRUE(OCI_NumberGetValue(number, OCI_NUM_INT, &value_out));

        ASSERT_EQ(value_test, value_out);

        ASSERT_TRUE(OCI_NumberFree(number));

    }

    TEST(TestNumber, ToTextDefaultFormat)
    {
        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
        OCI_Int32 value_test = 123456789;

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto number = OCI_NumberCreate(env);

        ASSERT_TRUE(OCI_NumberSetValue(number, OCI_NUM_INT, &value_test));
        ASSERT_TRUE(OCI_NumberToText(number, OCI_STRING_FORMAT_NUM, 512, buffer));

        ASSERT_EQ(ostring(OTEXT("123456789")), ostring(buffer));

        ASSERT_TRUE(OCI_NumberFree(number));

    }

    TEST(TestNumber, ToTextWithNullFormat)
    {
        otext buffer[OCI_SIZE_BUFFER] = {};
        OCI_Int32 value_test = 123456789;

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto number = OCI_NumberCreate(env);

        ASSERT_TRUE(OCI_NumberSetValue(number, OCI_NUM_INT, &value_test));
        ASSERT_TRUE(OCI_NumberToText(number, nullptr, 512, buffer));

        ASSERT_EQ(ostring(OTEXT("123456789")), ostring(buffer));

        ASSERT_TRUE(OCI_NumberFree(number));

    }

    TEST(TestNumber, FromTextWithNullFormat)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto number = OCI_NumberCreate(env);

        ASSERT_TRUE(OCI_NumberFromText(number, OTEXT("123456789"), nullptr));

        OCI_Int32 value_out = 0;
        ASSERT_TRUE(OCI_NumberGetValue(number, OCI_NUM_INT, &value_out));
        ASSERT_EQ(123456789, value_out);

        ASSERT_TRUE(OCI_NumberFree(number));

    }

    TEST(TestNumber, AddValueUnsignedShort)
    {
        AddValue<OCI_UInt16, OCI_NUM_USHORT>(13, 14, 27);
    }

    TEST(TestNumber, AddValueSignedShort)
    {
        AddValue<OCI_Int16, OCI_NUM_SHORT>(-13, 4, -9);
    }

    TEST(TestNumber, AddValueUnsignedInt)
    {
        AddValue<OCI_UInt32, OCI_NUM_UINT>(1876543210, 1876543210, 3753086420);
    }

    TEST(TestNumber, AddValueSignedInt)
    {
        AddValue<OCI_Int32, OCI_NUM_INT>(-938271605, 1876543210, 938271605);
    }

    TEST(TestNumber, AddValueUnsignedBigInt)
    {
        AddValue<OCI_UInt64, OCI_NUM_BIGUINT>(18765432100, 18765432100, 37530864200);
    }

    TEST(TestNumber, AddValueSignedBigInt)
    {
        AddValue<OCI_Int64, OCI_NUM_BIGINT>(-18765432100, 37530864200, 18765432100);
    }

    TEST(TestNumber, AddValueFloat)
    {
        AddValue<float, OCI_NUM_FLOAT>(3.14f, 6.28f, 9.42f);
    }

    TEST(TestNumber, AddValueDouble)
    {
        AddValue<double, OCI_NUM_DOUBLE>(3.14, 6.28, 9.42);
    }

    TEST(TestNumber, AddValueNumber)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto number1 = OCI_NumberCreate(env);
        const auto number2 = OCI_NumberCreate(env);
        OCI_Int32 value_in_1 = 13, value_in_2 = 15, value_out = 0;

        ASSERT_TRUE(OCI_NumberSetValue(number1, OCI_NUM_INT, &value_in_1));
        ASSERT_TRUE(OCI_NumberSetValue(number2, OCI_NUM_INT, &value_in_2));
        ASSERT_TRUE(OCI_NumberAdd(number1, OCI_NUM_NUMBER, OCI_NumberGetContent(number2)));
        ASSERT_TRUE(OCI_NumberGetValue(number1, OCI_NUM_INT, &value_out));
        ASSERT_EQ(28, value_out);

        ASSERT_TRUE(OCI_NumberFree(number1));
        ASSERT_TRUE(OCI_NumberFree(number2));

    }

    TEST(TestNumber, SubValueUnsignedShort)
    {
        SubValue<OCI_UInt16, OCI_NUM_USHORT>(14, 13, 1);
    }

    TEST(TestNumber, SubValueSignedShort)
    {
        SubValue<OCI_Int16, OCI_NUM_SHORT>(-13, 4, -17);
    }

    TEST(TestNumber, SubValueUnsignedInt)
    {
        SubValue<OCI_UInt32, OCI_NUM_UINT>(3753086420, 1876543210, 1876543210);
    }

    TEST(TestNumber, SubValueSignedInt)
    {
        SubValue<OCI_Int32, OCI_NUM_INT>(-938271605, 938271605, -1876543210);
    }

    TEST(TestNumber, SubValueUnsignedBigInt)
    {
        SubValue<OCI_UInt64, OCI_NUM_BIGUINT>(37530864200, 18765432100, 18765432100);
    }

    TEST(TestNumber, SubValueSignedBigInt)
    {
        SubValue<OCI_Int64, OCI_NUM_BIGINT>(-18765432100, 18765432100, -37530864200);
    }

    TEST(TestNumber, SubValueFloat)
    {
        SubValue<float, OCI_NUM_FLOAT>(9.42f, 3.14f, 6.28f);
    }

    TEST(TestNumber, SubValueDouble)
    {
        SubValue<double, OCI_NUM_DOUBLE>(9.42, 3.14, 6.28);
    }

    TEST(TestNumber, SubValueNumber)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto number1 = OCI_NumberCreate(env);
        const auto number2 = OCI_NumberCreate(env);
        OCI_Int32 value_in_1 = 13, value_in_2 = 15, value_out = 0;

        ASSERT_TRUE(OCI_NumberSetValue(number1, OCI_NUM_INT, &value_in_1));
        ASSERT_TRUE(OCI_NumberSetValue(number2, OCI_NUM_INT, &value_in_2));
        ASSERT_TRUE(OCI_NumberSub(number1, OCI_NUM_NUMBER, OCI_NumberGetContent(number2)));
        ASSERT_TRUE(OCI_NumberGetValue(number1, OCI_NUM_INT, &value_out));
        ASSERT_EQ(-2, value_out);

        ASSERT_TRUE(OCI_NumberFree(number1));
        ASSERT_TRUE(OCI_NumberFree(number2));

    }

    TEST(TestNumber, MultiplyValueUnsignedShort)
    {
        MultiplyValue<OCI_UInt16, OCI_NUM_USHORT>(13, 2, 26);
    }

    TEST(TestNumber, MultiplyValueSignedShort)
    {
        MultiplyValue<OCI_Int16, OCI_NUM_SHORT>(-13, 2, -26);
    }

    TEST(TestNumber, MultiplyValueUnsignedInt)
    {
        MultiplyValue<OCI_UInt32, OCI_NUM_UINT>(1876543210, 2, 3753086420);
    }

    TEST(TestNumber, MultiplyValueSignedInt)
    {
        MultiplyValue<OCI_Int32, OCI_NUM_INT>(-938271605, 2, -1876543210);
    }

    TEST(TestNumber, MultiplyValueUnsignedBigInt)
    {
        MultiplyValue<OCI_UInt64, OCI_NUM_BIGUINT>(18765432100, 2, 37530864200);
    }

    TEST(TestNumber, MultiplyValueSignedBigInt)
    {
        MultiplyValue<OCI_Int64, OCI_NUM_BIGINT>(-18765432100, 2, -37530864200);
    }

    TEST(TestNumber, MultiplyValueFloat)
    {
        MultiplyValue<float, OCI_NUM_FLOAT>(3.14f, 2.0f, 6.28f);
    }

    TEST(TestNumber, MultiplyValueDouble)
    {
        MultiplyValue<double, OCI_NUM_DOUBLE>(3.14, 2.0, 6.28);
    }

    TEST(TestNumber, MultiplyValueNumber)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto number1 = OCI_NumberCreate(env);
        const auto number2 = OCI_NumberCreate(env);
        OCI_Int32 value_in_1 = 13, value_in_2 = 2, value_out = 0;

        ASSERT_TRUE(OCI_NumberSetValue(number1, OCI_NUM_INT, &value_in_1));
        ASSERT_TRUE(OCI_NumberSetValue(number2, OCI_NUM_INT, &value_in_2));
        ASSERT_TRUE(OCI_NumberMultiply(number1, OCI_NUM_NUMBER, OCI_NumberGetContent(number2)));
        ASSERT_TRUE(OCI_NumberGetValue(number1, OCI_NUM_INT, &value_out));
        ASSERT_EQ(26, value_out);

        ASSERT_TRUE(OCI_NumberFree(number1));
        ASSERT_TRUE(OCI_NumberFree(number2));

    }

    TEST(TestNumber, DivideValueUnsignedShort)
    {
        DivideValue<OCI_UInt16, OCI_NUM_USHORT>(26, 2, 13);
    }

    TEST(TestNumber, DivideValueSignedShort)
    {
        DivideValue<OCI_Int16, OCI_NUM_SHORT>(-26, 2, -13);
    }

    TEST(TestNumber, DivideValueUnsignedInt)
    {
        DivideValue<OCI_UInt32, OCI_NUM_UINT>(3753086420, 2, 1876543210);
    }

    TEST(TestNumber, DivideValueSignedInt)
    {
        DivideValue<OCI_Int32, OCI_NUM_INT>(-1876543210, 2, -938271605);
    }

    TEST(TestNumber, DivideValueUnsignedBigInt)
    {
        DivideValue<OCI_UInt64, OCI_NUM_BIGUINT>(37530864200, 2, 18765432100);
    }

    TEST(TestNumber, DivideValueSignedBigInt)
    {
        DivideValue<OCI_Int64, OCI_NUM_BIGINT>(-37530864200, 2, -18765432100);
    }

    TEST(TestNumber, DivideValueFloat)
    {
        DivideValue<float, OCI_NUM_FLOAT>(6.28f, 2.0f, 3.14f);
    }

    TEST(TestNumber, DivideValueDouble)
    {
        DivideValue<double, OCI_NUM_DOUBLE>(6.28, 2.0, 3.14);
    }

    TEST(TestNumber, DivideValueNumber)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto number1 = OCI_NumberCreate(env);
        const auto number2 = OCI_NumberCreate(env);
        OCI_Int32 value_in_1 = 13, value_in_2 = 2;
        float value_out = 0;

        ASSERT_TRUE(OCI_NumberSetValue(number1, OCI_NUM_INT, &value_in_1));
        ASSERT_TRUE(OCI_NumberSetValue(number2, OCI_NUM_INT, &value_in_2));
        ASSERT_TRUE(OCI_NumberDivide(number1, OCI_NUM_NUMBER, OCI_NumberGetContent(number2)));
        ASSERT_TRUE(OCI_NumberGetValue(number1, OCI_NUM_FLOAT, &value_out));
        ASSERT_EQ(6.5f, value_out);

        ASSERT_TRUE(OCI_NumberFree(number1));
        ASSERT_TRUE(OCI_NumberFree(number2));

    }

    TEST(TestNumber, TestPositiveInfinity)
    {
        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto number = OCI_NumberCreate(env);

        ASSERT_TRUE(OCI_NumberFromText(number, OTEXT("~"), OCI_STRING_FORMAT_NUM));
        ASSERT_TRUE(OCI_NumberToText(number, OCI_STRING_FORMAT_NUM, 512, buffer));

        ASSERT_EQ(ostring(OTEXT("~")), ostring(buffer));

        ASSERT_TRUE(OCI_NumberFree(number));

    }

    TEST(TestNumber, TestNegativeInfinity)
    {
        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto number = OCI_NumberCreate(env);

        ASSERT_TRUE(OCI_NumberFromText(number, OTEXT("-~"), OCI_STRING_FORMAT_NUM));
        ASSERT_TRUE(OCI_NumberToText(number, OCI_STRING_FORMAT_NUM, 512, buffer));

        ASSERT_EQ(ostring(OTEXT("-~")), ostring(buffer));

        ASSERT_TRUE(OCI_NumberFree(number));

    }

    TEST(TestNumber, FormatWithSetGet)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        ASSERT_TRUE(OCI_SetFormat(env, OCI_FMT_NUMERIC, OTEXT("999999")));
        const otext* fmt = OCI_GetFormat(env, OCI_FMT_NUMERIC);
        ASSERT_NE(nullptr, fmt);
        ASSERT_EQ(ostring(OTEXT("999999")), ostring(fmt));

        auto num = OCI_NumberCreate(env);
        ASSERT_NE(nullptr, num);

        OCI_Int32 value = 12345;
        ASSERT_TRUE(OCI_NumberSetValue(num, OCI_NUM_INT, &value));

        otext buffer[64];
        ASSERT_TRUE(OCI_NumberToText(num, OTEXT("999999"), 64, buffer));
        const otext* trimmed = buffer;
        while (*trimmed == OTEXT(' ')) ++trimmed;
        ASSERT_EQ(ostring(OTEXT("12345")), ostring(trimmed));

        ASSERT_TRUE(OCI_NumberFromText(num, OTEXT("42"), OTEXT("99")));
        ASSERT_TRUE(OCI_NumberToText(num, OTEXT("999"), 64, buffer));
        trimmed = buffer;
        while (*trimmed == OTEXT(' ')) ++trimmed;
        ASSERT_EQ(ostring(OTEXT("42")), ostring(trimmed));

        ASSERT_TRUE(OCI_NumberFree(num));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    /* --- resultset getters converting non-native column types --- */

    class TestNumberCvt : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("create table testnumcvt (n number, bf binary_float, bd binary_double, t varchar2(30))"));
            ExecDML(OTEXT("insert into testnumcvt values (42, 2.5, 7.25, '17')"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table testnumcvt"));
        }

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);

            ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select n, bf, bd, t from testnumcvt")));

            rs = OCI_GetResultset(stmt);
            ASSERT_NE(nullptr, rs);
            ASSERT_TRUE(OCI_FetchNext(rs));
        }

        void TearDown() override
        {
            OCI_StatementFree(stmt);
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
        OCI_Statement*  stmt{ nullptr };
        OCI_Resultset*  rs{ nullptr };
    };

    TEST_F(TestNumberCvt, NumberColumnToInt)
    {
        ASSERT_EQ(42, OCI_GetInt(rs, 1));
    }

    TEST_F(TestNumberCvt, NumberColumnToShort)
    {
        ASSERT_EQ(42, OCI_GetShort(rs, 1));
    }

    TEST_F(TestNumberCvt, NumberColumnToBigInt)
    {
        ASSERT_EQ(42, OCI_GetBigInt(rs, 1));
    }

    TEST_F(TestNumberCvt, NumberColumnToDouble)
    {
        ASSERT_NEAR(42.0, OCI_GetDouble(rs, 1), 0.001);
    }

    TEST_F(TestNumberCvt, NumberColumnToFloat)
    {
        ASSERT_NEAR(42.0f, OCI_GetFloat(rs, 1), 0.001f);
    }

    TEST_F(TestNumberCvt, BinaryFloatColumnToDouble)
    {
        ASSERT_NEAR(2.5, OCI_GetDouble(rs, 2), 0.001);
    }

    TEST_F(TestNumberCvt, BinaryFloatColumnToInt)
    {
        ASSERT_EQ(2, OCI_GetInt(rs, 2));
    }

    TEST_F(TestNumberCvt, BinaryDoubleColumnToFloat)
    {
        ASSERT_NEAR(7.25f, OCI_GetFloat(rs, 3), 0.001f);
    }

    TEST_F(TestNumberCvt, BinaryDoubleColumnToInt)
    {
        ASSERT_EQ(7, OCI_GetInt(rs, 3));
    }

    TEST_F(TestNumberCvt, VarcharColumnToShort)
    {
        /* VARCHAR2 column parsed to binary numeric types */

        ASSERT_EQ(17, OCI_GetShort(rs, 4));
    }

    TEST_F(TestNumberCvt, VarcharColumnToInt)
    {
        ASSERT_EQ(17, OCI_GetInt(rs, 4));
    }

    TEST_F(TestNumberCvt, VarcharColumnToBigInt)
    {
        ASSERT_EQ(17, OCI_GetBigInt(rs, 4));
    }

    TEST_F(TestNumberCvt, VarcharColumnToDouble)
    {
        ASSERT_NEAR(17.0, OCI_GetDouble(rs, 4), 0.001);
    }

    TEST_F(TestNumberCvt, VarcharColumnToFloat)
    {
        ASSERT_NEAR(17.0f, OCI_GetFloat(rs, 4), 0.001f);
    }

    /* --- OcilibNumberTranslateInternal arms --- */

    class TestNumberConv : public ::testing::Test
    {
    protected:

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            num = OCI_NumberCreate(env);
            ASSERT_NE(nullptr, num);
        }

        void TearDown() override
        {
            OCI_NumberFree(num);
        }

        OCI_Number* num{ nullptr };
    };

    TEST_F(TestNumberConv, TranslateRealToNumber)
    {
        /* real -> OCINumber (no public API performs this direction) */

        double dv = 7.5;
        ASSERT_TRUE(OCI_TestNumberTranslate(num, &dv, OCI_NUM_DOUBLE, OCI_NumberGetContent(num), OCI_NUM_NUMBER));

        double dv2 = 0.0;
        ASSERT_TRUE(OCI_NumberGetValue(num, OCI_NUM_DOUBLE, &dv2));
        ASSERT_NEAR(7.5, dv2, 0.001);
    }

    TEST_F(TestNumberConv, TranslateIntToReal)
    {
        OCI_Int16 sv = 42;
        double dv = 0.0;

        ASSERT_TRUE(OCI_TestNumberTranslate(num, &sv, OCI_NUM_SHORT, &dv, OCI_NUM_DOUBLE));
        ASSERT_NEAR(42.0, dv, 0.001);
    }

    TEST_F(TestNumberConv, TranslateIntToBigInt)
    {
        /* integer -> integer with different size/sign */

        OCI_Int16 sv = 42;
        OCI_Int64 bv = 0;

        ASSERT_TRUE(OCI_TestNumberTranslate(num, &sv, OCI_NUM_SHORT, &bv, OCI_NUM_BIGINT));
        ASSERT_EQ(42, bv);
    }

    TEST_F(TestNumberConv, TranslateIntToUInt)
    {
        OCI_Int16 sv = 42;
        OCI_UInt32 uiv = 0;

        ASSERT_TRUE(OCI_TestNumberTranslate(num, &sv, OCI_NUM_SHORT, &uiv, OCI_NUM_UINT));
        ASSERT_EQ(42u, uiv);
    }

    TEST_F(TestNumberConv, TranslateRealToInt)
    {
        /* real -> integer (double/float truncation through OCINumber) */

        double dv = 9.75;
        OCI_Int32 iv = 0;

        ASSERT_TRUE(OCI_TestNumberTranslate(num, &dv, OCI_NUM_DOUBLE, &iv, OCI_NUM_INT));
        ASSERT_EQ(9, iv);
    }

    /* --- binary types rendered as text through OcilibNumberToStringInternal() --- */

    struct NumToStringCase
    {
        OCI_UInt32 type;
        double       value;
        const otext* expected;
    };

    class TestNumberToString : public TestNumberConv,
                               public ::testing::WithParamInterface<NumToStringCase> {};

    TEST_P(TestNumberToString, Renders)
    {
        OCI_Int16   sv = 0;
        OCI_Int32     iv = 0;
        float   fv = 0.0f;
        double  dv = 0.0;

        const void* data = nullptr;

        switch (GetParam().type)
        {
            case OCI_NUM_SHORT:  sv = (OCI_Int16) GetParam().value;  data = &sv; break;
            case OCI_NUM_INT:    iv = (OCI_Int32) GetParam().value;    data = &iv; break;
            case OCI_NUM_FLOAT:  fv = (float) GetParam().value;  data = &fv; break;
            case OCI_NUM_DOUBLE: dv = GetParam().value;          data = &dv; break;
        }

        ASSERT_NE(nullptr, data);

        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
        ASSERT_TRUE(OCI_TestNumberToString(num, const_cast<void*>(data), GetParam().type, buffer, OCI_SIZE_BUFFER));
        ASSERT_EQ(0, ostring(buffer).find(GetParam().expected));
    }

    INSTANTIATE_TEST_CASE_P(Types, TestNumberToString,
        ::testing::Values(
            NumToStringCase{ OCI_NUM_SHORT,  -17.0,  OTEXT("-17") },
            NumToStringCase{ OCI_NUM_INT,    12345,  OTEXT("12345") },
            NumToStringCase{ OCI_NUM_DOUBLE, 2.5,    OTEXT("2.5") },
            NumToStringCase{ OCI_NUM_FLOAT,  1.5,    OTEXT("1.5") }));
}
