#include "ocilib_tests.h"
#include "NullArgs.h"

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
        SetAndGetValue<OCI_Float, OCI_NUM_FLOAT>(3.14f);
    }

    TEST(TestNumber, SetAndGetValueDouble)
    {
        SetAndGetValue<OCI_Double, OCI_NUM_DOUBLE>(3.14);
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

        ASSERT_EQ(String(OTEXT("123456789")), String(buffer));

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

        ASSERT_EQ(String(OTEXT("123456789")), String(buffer));

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
        AddValue<OCI_Float, OCI_NUM_FLOAT>(3.14f, 6.28f, 9.42f);
    }

    TEST(TestNumber, AddValueDouble)
    {
        AddValue<OCI_Double, OCI_NUM_DOUBLE>(3.14, 6.28, 9.42);
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
        SubValue<OCI_Float, OCI_NUM_FLOAT>(9.42f, 3.14f, 6.28f);
    }

    TEST(TestNumber, SubValueDouble)
    {
        SubValue<OCI_Double, OCI_NUM_DOUBLE>(9.42, 3.14, 6.28);
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
        MultiplyValue<OCI_Float, OCI_NUM_FLOAT>(3.14f, 2.0f, 6.28f);
    }

    TEST(TestNumber, MultiplyValueDouble)
    {
        MultiplyValue<OCI_Double, OCI_NUM_DOUBLE>(3.14, 2.0, 6.28);
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
        DivideValue<OCI_Float, OCI_NUM_FLOAT>(6.28f, 2.0f, 3.14f);
    }

    TEST(TestNumber, DivideValueDouble)
    {
        DivideValue<OCI_Double, OCI_NUM_DOUBLE>(6.28, 2.0, 3.14);
    }

    TEST(TestNumber, DivideValueNumber)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto number1 = OCI_NumberCreate(env);
        const auto number2 = OCI_NumberCreate(env);
        OCI_Int32 value_in_1 = 13, value_in_2 = 2;
        OCI_Float value_out = 0;

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

        ASSERT_EQ(String(OTEXT("~")), String(buffer));

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

        ASSERT_EQ(String(OTEXT("-~")), String(buffer));

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
        ASSERT_EQ(String(OTEXT("999999")), String(fmt));

        auto num = OCI_NumberCreate(env);
        ASSERT_NE(nullptr, num);

        OCI_Int32 value = 12345;
        ASSERT_TRUE(OCI_NumberSetValue(num, OCI_NUM_INT, &value));

        otext buffer[64];
        ASSERT_TRUE(OCI_NumberToText(num, OTEXT("999999"), 64, buffer));
        const otext* trimmed = buffer;
        while (*trimmed == OTEXT(' ')) ++trimmed;
        ASSERT_EQ(String(OTEXT("12345")), String(trimmed));

        ASSERT_TRUE(OCI_NumberFromText(num, OTEXT("42"), OTEXT("99")));
        ASSERT_TRUE(OCI_NumberToText(num, OTEXT("999"), 64, buffer));
        trimmed = buffer;
        while (*trimmed == OTEXT(' ')) ++trimmed;
        ASSERT_EQ(String(OTEXT("42")), String(trimmed));

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

    TEST_F(TestNumberCvt, GetIntOnNumberColumn_Converts)
    {
        ASSERT_EQ(42, OCI_GetInt(rs, 0));
    }

    TEST_F(TestNumberCvt, GetShortOnNumberColumn_Converts)
    {
        ASSERT_EQ(42, OCI_GetShort(rs, 0));
    }

    TEST_F(TestNumberCvt, GetBigIntOnNumberColumn_Converts)
    {
        ASSERT_EQ(42, OCI_GetBigInt(rs, 0));
    }

    TEST_F(TestNumberCvt, GetDoubleOnNumberColumn_Converts)
    {
        ASSERT_NEAR(42.0, OCI_GetDouble(rs, 0), 0.001);
    }

    TEST_F(TestNumberCvt, GetFloatOnNumberColumn_Converts)
    {
        ASSERT_NEAR(42.0f, OCI_GetFloat(rs, 0), 0.001f);
    }

    TEST_F(TestNumberCvt, GetDoubleOnBinaryFloatColumn_Converts)
    {
        ASSERT_NEAR(2.5, OCI_GetDouble(rs, 1), 0.001);
    }

    TEST_F(TestNumberCvt, GetIntOnBinaryFloatColumn_Converts)
    {
        ASSERT_EQ(2, OCI_GetInt(rs, 1));
    }

    TEST_F(TestNumberCvt, GetFloatOnBinaryDoubleColumn_Converts)
    {
        ASSERT_NEAR(7.25f, OCI_GetFloat(rs, 2), 0.001f);
    }

    TEST_F(TestNumberCvt, GetIntOnBinaryDoubleColumn_Converts)
    {
        ASSERT_EQ(7, OCI_GetInt(rs, 2));
    }

    TEST_F(TestNumberCvt, GetShortOnVarcharColumn_Converts)
    {
        /* VARCHAR2 column parsed to binary numeric types */

        ASSERT_EQ(17, OCI_GetShort(rs, 3));
    }

    TEST_F(TestNumberCvt, GetIntOnVarcharColumn_Converts)
    {
        ASSERT_EQ(17, OCI_GetInt(rs, 3));
    }

    TEST_F(TestNumberCvt, GetBigIntOnVarcharColumn_Converts)
    {
        ASSERT_EQ(17, OCI_GetBigInt(rs, 3));
    }

    TEST_F(TestNumberCvt, GetDoubleOnVarcharColumn_Converts)
    {
        ASSERT_NEAR(17.0, OCI_GetDouble(rs, 3), 0.001);
    }

    TEST_F(TestNumberCvt, GetFloatOnVarcharColumn_Converts)
    {
        ASSERT_NEAR(17.0f, OCI_GetFloat(rs, 3), 0.001f);
    }

    /* --- public-API coverage of the dropped OcilibNumber* internals:
       OCI_NumberToText / OCI_NumberFromText / OCI_NumberGetContent /
       OCI_NumberSetValue / OCI_NumberGetValue / OCI_NumberAdd exercise
       OcilibNumberToText / OcilibNumberFromText / OcilibNumberTranslate /
       OcilibNumberToString / OcilibNumberGetContent --- */

    TEST(TestNumber, ToTextAndFromText)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto num = OCI_NumberCreate(env);
        ASSERT_NE(nullptr, num);

        int32_t val = 42;
        ASSERT_TRUE(OCI_NumberSetValue(num, OCI_NUM_INT, &val));

        /* OcilibNumberToText / OcilibNumberGetContent via public text roundtrip */
        otext buf[64] = { 0 };
        ASSERT_TRUE(OCI_NumberToText(num, OTEXT("999"), (OCI_Int32)(sizeof(buf) / sizeof(otext)), buf));
        ASSERT_NE(0u, (OCI_UInt32)ostrlen(buf));

        const auto num2 = OCI_NumberCreate(env);
        ASSERT_NE(nullptr, num2);
        ASSERT_TRUE(OCI_NumberFromText(num2, OTEXT("42"), nullptr));
        ASSERT_NE(nullptr, OCI_NumberGetContent(num2));

        int32_t val2 = 0;
        ASSERT_TRUE(OCI_NumberGetValue(num2, OCI_NUM_INT, &val2));
        ASSERT_EQ(42, val2);

        ASSERT_TRUE(OCI_NumberFree(num));
        ASSERT_TRUE(OCI_NumberFree(num2));
    }

    TEST(TestNumber, SetValueGetValueAndAdd)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto num = OCI_NumberCreate(env);
        ASSERT_NE(nullptr, num);

        int32_t v = 10;
        ASSERT_TRUE(OCI_NumberSetValue(num, OCI_NUM_INT, &v));
        int32_t out = 0;
        ASSERT_TRUE(OCI_NumberGetValue(num, OCI_NUM_INT, &out));
        ASSERT_EQ(10, out);

        /* OcilibNumberAdd via OCI_NumberAdd */
        int32_t addend = 5;
        ASSERT_TRUE(OCI_NumberAdd(num, OCI_NUM_INT, &addend));
        ASSERT_TRUE(OCI_NumberGetValue(num, OCI_NUM_INT, &out));
        ASSERT_EQ(15, out);

        ASSERT_TRUE(OCI_NumberFree(num));
    }

    TEST(TestNumber, NullNumberErrors)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        int32_t v = 0;
        ASSERT_FALSE(OCI_NumberFree(nullptr));
        ASSERT_FALSE(OCI_NumberAssign(nullptr, nullptr));
        ASSERT_FALSE(OCI_NumberToText(nullptr, nullptr, 0, nullptr));
        ASSERT_FALSE(OCI_NumberFromText(nullptr, nullptr, nullptr));
        ASSERT_EQ(nullptr, OCI_NumberGetContent(nullptr));
        ASSERT_FALSE(OCI_NumberSetContent(nullptr, nullptr));
        ASSERT_FALSE(OCI_NumberSetValue(nullptr, OCI_NUM_INT, &v));
        ASSERT_FALSE(OCI_NumberGetValue(nullptr, OCI_NUM_INT, &v));
        ASSERT_FALSE(OCI_NumberAdd(nullptr, OCI_NUM_INT, &v));
    }

    class TestNumberNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestNumberNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestNumberNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "NumberAdd", [] { return !OCI_NumberAdd(nullptr, 1, NullArgBuffer()); } },
            { "NumberArrayCreate", [] { return nullptr == OCI_NumberArrayCreate(nullptr, 1); } },
            { "NumberArrayFree", [] { return !OCI_NumberArrayFree(nullptr); } },
            { "NumberAssign", [] { return !OCI_NumberAssign(nullptr, nullptr); } },
            { "NumberCompare", [] { return -1 == OCI_NumberCompare(nullptr, nullptr); } },
            { "NumberCreate", [] { return nullptr == OCI_NumberCreate(nullptr); } },
            { "NumberDivide", [] { return !OCI_NumberDivide(nullptr, 1, NullArgBuffer()); } },
            { "NumberFree", [] { return !OCI_NumberFree(nullptr); } },
            { "NumberFromText", [] { return !OCI_NumberFromText(nullptr, OTEXT("x"), OTEXT("x")); } },
            { "NumberGetContent", [] { return nullptr == OCI_NumberGetContent(nullptr); } },
            { "NumberGetValue", [] { return !OCI_NumberGetValue(nullptr, 1, NullArgBuffer()); } },
            { "NumberMultiply", [] { return !OCI_NumberMultiply(nullptr, 1, NullArgBuffer()); } },
            { "NumberSetContent", [] { return !OCI_NumberSetContent(nullptr, NullArgRaw()); } },
            { "NumberSetValue", [] { return !OCI_NumberSetValue(nullptr, 1, NullArgBuffer()); } },
            { "NumberSub", [] { return !OCI_NumberSub(nullptr, 1, NullArgBuffer()); } },
            { "NumberToText", [] { return !OCI_NumberToText(nullptr, OTEXT("x"), 1, NullArgText()); } },
        }),
        &NullArgCaseName);

}
