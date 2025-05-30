#include "ocilib_tests.h"

namespace TestCApi
{
    template<typename T, int C>
    void SetAndGetValue(T value)
    {
        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto number = OCI_NumberCreate(nullptr);
        T value_in = value, value_out{};

        ASSERT_TRUE(OCI_NumberSetValue(number, C, &value_in));
        ASSERT_TRUE(OCI_NumberGetValue(number, C, &value_out));
        ASSERT_EQ(value_in, value_out);

        ASSERT_TRUE(OCI_NumberFree(number));

        ASSERT_TRUE(OCI_Cleanup());
    }

    template<typename T, int C, typename F>
    void NumberOperation(T value1, T value2, T expected, F func)
    {
        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto number = OCI_NumberCreate(nullptr);
        T value{};

        ASSERT_TRUE(OCI_NumberSetValue(number, C, &value1));
        ASSERT_TRUE(func(number, C, &value2));
        ASSERT_TRUE(OCI_NumberGetValue(number, C, &value));
        ASSERT_EQ(expected, value);

        ASSERT_TRUE(OCI_NumberFree(number));

        ASSERT_TRUE(OCI_Cleanup());
    }


    template<typename T, int C>
    void AddValue(T value1, T value2, T expected)
    {
        NumberOperation<T, C, decltype(OCI_NumberAdd)>(value1, value2, expected, OCI_NumberAdd);
    }

    template<typename T, int C>
    void SubValue(T value1, T value2, T expected)
    {
        NumberOperation<T, C, decltype(OCI_NumberSub)>(value1, value2, expected, OCI_NumberSub);
    }

    template<typename T, int C>
    void MultiplyValue(T value1, T value2, T expected)
    {
        NumberOperation<T, C, decltype(OCI_NumberMultiply)>(value1, value2, expected, OCI_NumberMultiply);
    }

    template<typename T, int C>
    void DivideValue(T value1, T value2, T expected)
    {
        NumberOperation<T, C, decltype(OCI_NumberDivide)>(value1, value2, expected, OCI_NumberDivide);
    }

    TEST(TestNumber, CreateAndFree)
    {
        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto number = OCI_NumberCreate(nullptr);
        ASSERT_TRUE(nullptr != number);
        ASSERT_TRUE(OCI_NumberFree(number));

        ASSERT_TRUE(OCI_Cleanup());
    }

    TEST(TestNumber, ArrayCreateAndFree)
    {
        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto numbers = OCI_NumberArrayCreate(nullptr, ARRAY_SIZE);
        ASSERT_TRUE(nullptr != numbers);

        for (int i = 0; i < ARRAY_SIZE; i++)
        {
            ASSERT_TRUE(nullptr != numbers[i]);
        }
        ASSERT_TRUE(OCI_NumberArrayFree(numbers));

        ASSERT_TRUE(OCI_Cleanup());
    }

    TEST(TestNumber, SetAndGetValueUnsignedShort)
    {
        SetAndGetValue<unsigned short, OCI_NUM_USHORT>(13);
    }

    TEST(TestNumber, SetAndGetValueSignedShort)
    {
        SetAndGetValue<short, OCI_NUM_SHORT>(-13);
    }

    TEST(TestNumber, SetAndGetValueUnsignedInt)
    {
        SetAndGetValue<unsigned int, OCI_NUM_UINT>(99123456);
    }

    TEST(TestNumber, SetAndGetValueSignedInt)
    {
        SetAndGetValue<int, OCI_NUM_INT>(-99123456);
    }

    TEST(TestNumber, SetAndGetValueUnsignedBigInt)
    {
        SetAndGetValue<big_uint, OCI_NUM_BIGUINT>(81234123412341234);
    }

    TEST(TestNumber, SetAndGetValueSignedBigInt)
    {
        SetAndGetValue<big_int, OCI_NUM_BIGINT>(-34123412341234);
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
        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto number1 = OCI_NumberCreate(nullptr);
        const auto number2 = OCI_NumberCreate(nullptr);
        int value_in = 13, value_out = 0;

        ASSERT_TRUE(OCI_NumberSetValue(number1, OCI_NUM_INT, &value_in));
        ASSERT_TRUE(OCI_NumberSetValue(number2, OCI_NUM_NUMBER, OCI_NumberGetContent(number1)));
        ASSERT_TRUE(OCI_NumberGetValue(number2, OCI_NUM_INT, &value_out));
        ASSERT_EQ(value_in, value_out);

        ASSERT_TRUE(OCI_NumberFree(number1));
        ASSERT_TRUE(OCI_NumberFree(number2));

        ASSERT_TRUE(OCI_Cleanup());
    }

    TEST(TestNumber, SetContent)
    {
        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto number1 = OCI_NumberCreate(nullptr);
        const auto number2 = OCI_NumberCreate(nullptr);
        int value_in = 13, value_out = 0;

        ASSERT_TRUE(OCI_NumberSetValue(number1, OCI_NUM_INT, &value_in));
        ASSERT_TRUE(OCI_NumberSetContent(number2, OCI_NumberGetContent(number1)));
        ASSERT_TRUE(OCI_NumberGetValue(number2, OCI_NUM_INT, &value_out));
        ASSERT_EQ(value_in, value_out);

        ASSERT_TRUE(OCI_NumberFree(number1));
        ASSERT_TRUE(OCI_NumberFree(number2));

        ASSERT_TRUE(OCI_Cleanup());
    }

    TEST(TestNumber, Assign)
    {
        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto number1 = OCI_NumberCreate(nullptr);
        const auto number2 = OCI_NumberCreate(nullptr);
        int value_in = 13, value_out = 0;

        ASSERT_TRUE(OCI_NumberSetValue(number1, OCI_NUM_INT, &value_in));
        ASSERT_TRUE(OCI_NumberAssign(number2, number1));
        ASSERT_TRUE(OCI_NumberGetValue(number2, OCI_NUM_INT, &value_out));
        ASSERT_EQ(value_in, value_out);

        ASSERT_TRUE(OCI_NumberFree(number1));
        ASSERT_TRUE(OCI_NumberFree(number2));

        ASSERT_TRUE(OCI_Cleanup());
    }

    TEST(TestNumber, FromTextDefaultFormat)
    {
        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto number = OCI_NumberCreate(nullptr);
        int value_test = 123456789, value_out = 0;

        ASSERT_TRUE(OCI_NumberFromText(number, OTEXT("123456789"), OCI_STRING_FORMAT_NUM));
        ASSERT_TRUE(OCI_NumberGetValue(number, OCI_NUM_INT, &value_out));

        ASSERT_EQ(value_test, value_out);

        ASSERT_TRUE(OCI_NumberFree(number));

        ASSERT_TRUE(OCI_Cleanup());
    }

    TEST(TestNumber, ToTextDefaultFormat)
    {
        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
        int value_test = 123456789;

        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto number = OCI_NumberCreate(nullptr);

        ASSERT_TRUE(OCI_NumberSetValue(number, OCI_NUM_INT, &value_test));
        ASSERT_TRUE(OCI_NumberToText(number, OCI_STRING_FORMAT_NUM, 512, buffer));

        ASSERT_EQ(ostring(OTEXT("123456789")), ostring(buffer));

        ASSERT_TRUE(OCI_NumberFree(number));

        ASSERT_TRUE(OCI_Cleanup());
    }

    TEST(TestNumber, AddValueUnsignedShort)
    {
        AddValue<unsigned short, OCI_NUM_USHORT>(13, 14, 27);
    }

    TEST(TestNumber, AddValueSignedShort)
    {
        AddValue<short, OCI_NUM_SHORT>(-13, 4, -9);
    }

    TEST(TestNumber, AddValueUnsignedInt)
    {
        AddValue<unsigned int, OCI_NUM_UINT>(1876543210, 1876543210, 3753086420);
    }

    TEST(TestNumber, AddValueSignedInt)
    {
        AddValue<int, OCI_NUM_INT>(-938271605, 1876543210, 938271605);
    }

    TEST(TestNumber, AddValueUnsignedBigInt)
    {
        AddValue<big_uint, OCI_NUM_BIGUINT>(18765432100, 18765432100, 37530864200);
    }

    TEST(TestNumber, AddValueSignedBigInt)
    {
        AddValue<big_int, OCI_NUM_BIGINT>(-18765432100, 37530864200, 18765432100);
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
        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto number1 = OCI_NumberCreate(nullptr);
        const auto number2 = OCI_NumberCreate(nullptr);
        int value_in_1 = 13, value_in_2 = 15, value_out = 0;

        ASSERT_TRUE(OCI_NumberSetValue(number1, OCI_NUM_INT, &value_in_1));
        ASSERT_TRUE(OCI_NumberSetValue(number2, OCI_NUM_INT, &value_in_2));
        ASSERT_TRUE(OCI_NumberAdd(number1, OCI_NUM_NUMBER, OCI_NumberGetContent(number2)));
        ASSERT_TRUE(OCI_NumberGetValue(number1, OCI_NUM_INT, &value_out));
        ASSERT_EQ(28, value_out);

        ASSERT_TRUE(OCI_NumberFree(number1));
        ASSERT_TRUE(OCI_NumberFree(number2));

        ASSERT_TRUE(OCI_Cleanup());
    }

    TEST(TestNumber, SubValueUnsignedShort)
    {
        SubValue<unsigned short, OCI_NUM_USHORT>(14, 13, 1);
    }

    TEST(TestNumber, SubValueSignedShort)
    {
        SubValue<short, OCI_NUM_SHORT>(-13, 4, -17);
    }

    TEST(TestNumber, SubValueUnsignedInt)
    {
        SubValue<unsigned int, OCI_NUM_UINT>(3753086420, 1876543210, 1876543210);
    }

    TEST(TestNumber, SubValueSignedInt)
    {
        SubValue<int, OCI_NUM_INT>(-938271605, 938271605, -1876543210);
    }

    TEST(TestNumber, SubValueUnsignedBigInt)
    {
        SubValue<big_uint, OCI_NUM_BIGUINT>(37530864200, 18765432100, 18765432100);
    }

    TEST(TestNumber, SubValueSignedBigInt)
    {
        SubValue<big_int, OCI_NUM_BIGINT>(-18765432100, 18765432100, -37530864200);
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
        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto number1 = OCI_NumberCreate(nullptr);
        const auto number2 = OCI_NumberCreate(nullptr);
        int value_in_1 = 13, value_in_2 = 15, value_out = 0;

        ASSERT_TRUE(OCI_NumberSetValue(number1, OCI_NUM_INT, &value_in_1));
        ASSERT_TRUE(OCI_NumberSetValue(number2, OCI_NUM_INT, &value_in_2));
        ASSERT_TRUE(OCI_NumberSub(number1, OCI_NUM_NUMBER, OCI_NumberGetContent(number2)));
        ASSERT_TRUE(OCI_NumberGetValue(number1, OCI_NUM_INT, &value_out));
        ASSERT_EQ(-2, value_out);

        ASSERT_TRUE(OCI_NumberFree(number1));
        ASSERT_TRUE(OCI_NumberFree(number2));

        ASSERT_TRUE(OCI_Cleanup());
    }

    TEST(TestNumber, MultiplyValueUnsignedShort)
    {
        MultiplyValue<unsigned short, OCI_NUM_USHORT>(13, 2, 26);
    }

    TEST(TestNumber, MultiplyValueSignedShort)
    {
        MultiplyValue<short, OCI_NUM_SHORT>(-13, 2, -26);
    }

    TEST(TestNumber, MultiplyValueUnsignedInt)
    {
        MultiplyValue<unsigned int, OCI_NUM_UINT>(1876543210, 2, 3753086420);
    }

    TEST(TestNumber, MultiplyValueSignedInt)
    {
        MultiplyValue<int, OCI_NUM_INT>(-938271605, 2, -1876543210);
    }

    TEST(TestNumber, MultiplyValueUnsignedBigInt)
    {
        MultiplyValue<big_uint, OCI_NUM_BIGUINT>(18765432100, 2, 37530864200);
    }

    TEST(TestNumber, MultiplyValueSignedBigInt)
    {
        MultiplyValue<big_int, OCI_NUM_BIGINT>(-18765432100, 2, -37530864200);
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
        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto number1 = OCI_NumberCreate(nullptr);
        const auto number2 = OCI_NumberCreate(nullptr);
        int value_in_1 = 13, value_in_2 = 2, value_out = 0;

        ASSERT_TRUE(OCI_NumberSetValue(number1, OCI_NUM_INT, &value_in_1));
        ASSERT_TRUE(OCI_NumberSetValue(number2, OCI_NUM_INT, &value_in_2));
        ASSERT_TRUE(OCI_NumberMultiply(number1, OCI_NUM_NUMBER, OCI_NumberGetContent(number2)));
        ASSERT_TRUE(OCI_NumberGetValue(number1, OCI_NUM_INT, &value_out));
        ASSERT_EQ(26, value_out);

        ASSERT_TRUE(OCI_NumberFree(number1));
        ASSERT_TRUE(OCI_NumberFree(number2));

        ASSERT_TRUE(OCI_Cleanup());
    }

    TEST(TestNumber, DivideValueUnsignedShort)
    {
        DivideValue<unsigned short, OCI_NUM_USHORT>(26, 2, 13);
    }

    TEST(TestNumber, DivideValueSignedShort)
    {
        DivideValue<short, OCI_NUM_SHORT>(-26, 2, -13);
    }

    TEST(TestNumber, DivideValueUnsignedInt)
    {
        DivideValue<unsigned int, OCI_NUM_UINT>(3753086420, 2, 1876543210);
    }

    TEST(TestNumber, DivideValueSignedInt)
    {
        DivideValue<int, OCI_NUM_INT>(-1876543210, 2, -938271605);
    }

    TEST(TestNumber, DivideValueUnsignedBigInt)
    {
        DivideValue<big_uint, OCI_NUM_BIGUINT>(37530864200, 2, 18765432100);
    }

    TEST(TestNumber, DivideValueSignedBigInt)
    {
        DivideValue<big_int, OCI_NUM_BIGINT>(-37530864200, 2, -18765432100);
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
        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto number1 = OCI_NumberCreate(nullptr);
        const auto number2 = OCI_NumberCreate(nullptr);
        int value_in_1 = 13, value_in_2 = 2;
        float value_out = 0;

        ASSERT_TRUE(OCI_NumberSetValue(number1, OCI_NUM_INT, &value_in_1));
        ASSERT_TRUE(OCI_NumberSetValue(number2, OCI_NUM_INT, &value_in_2));
        ASSERT_TRUE(OCI_NumberDivide(number1, OCI_NUM_NUMBER, OCI_NumberGetContent(number2)));
        ASSERT_TRUE(OCI_NumberGetValue(number1, OCI_NUM_FLOAT, &value_out));
        ASSERT_EQ(6.5f, value_out);

        ASSERT_TRUE(OCI_NumberFree(number1));
        ASSERT_TRUE(OCI_NumberFree(number2));

        ASSERT_TRUE(OCI_Cleanup());
    }

    TEST(TestNumber, TestPositiveInfinity)
    {
        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");

        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto number = OCI_NumberCreate(nullptr);

        ASSERT_TRUE(OCI_NumberFromText(number, OTEXT("~"), OCI_STRING_FORMAT_NUM));
        ASSERT_TRUE(OCI_NumberToText(number, OCI_STRING_FORMAT_NUM, 512, buffer));

        ASSERT_EQ(ostring(OTEXT("~")), ostring(buffer));

        ASSERT_TRUE(OCI_NumberFree(number));

        ASSERT_TRUE(OCI_Cleanup());
    }

    TEST(TestNumber, TestNegativeInfinity)
    {
        otext buffer[OCI_SIZE_BUFFER] = OTEXT("");

        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto number = OCI_NumberCreate(nullptr);

        ASSERT_TRUE(OCI_NumberFromText(number, OTEXT("-~"), OCI_STRING_FORMAT_NUM));
        ASSERT_TRUE(OCI_NumberToText(number, OCI_STRING_FORMAT_NUM, 512, buffer));

        ASSERT_EQ(ostring(OTEXT("-~")), ostring(buffer));

        ASSERT_TRUE(OCI_NumberFree(number));

        ASSERT_TRUE(OCI_Cleanup());
    }
}
