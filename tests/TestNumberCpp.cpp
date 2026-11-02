#include "ocilib_tests.h"

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestNumberCpp, ArithmeticAndConversions)
    {
        Environment& env = GetEnvironmentCpp();

        Number n1(env, 123);
        ASSERT_EQ(123, static_cast<OCI_Int32>(n1));
        ASSERT_EQ(123.0, static_cast<double>(n1));

        Number n2(env, true);
        n2 = 456;
        ASSERT_EQ(456, static_cast<OCI_Int32>(n2));

        Number n3 = n1.Clone();
        ASSERT_EQ(123, static_cast<OCI_Int32>(n3));

        Number n4 = n1 + 1;
        Number n5 = n1 - 1;
        Number n6 = n1 * 2;
        Number n7 = n1 / 3;

        ASSERT_EQ(124, static_cast<OCI_Int32>(n4));
        ASSERT_EQ(122, static_cast<OCI_Int32>(n5));
        ASSERT_EQ(246, static_cast<OCI_Int32>(n6));
        ASSERT_EQ(41, static_cast<OCI_Int32>(n7));

        ASSERT_TRUE(n1 == n3);
        ASSERT_TRUE(n1 != n2);
        ASSERT_TRUE(n4 > n1);
        ASSERT_TRUE(n5 < n1);
        ASSERT_TRUE(n1 >= n3);
        ASSERT_TRUE(n1 <= n3);

        Number n8 = n1;
        n8 += 1;
        n8 -= 1;
        n8 *= 2;
        n8 /= 2;
        ASSERT_EQ(123, static_cast<OCI_Int32>(n8));

        Number n9 = n8++;
        ASSERT_EQ(124, static_cast<OCI_Int32>(n9));
        ASSERT_EQ(123, static_cast<OCI_Int32>(n8));

        ++n8;
        ASSERT_EQ(124, static_cast<OCI_Int32>(n8));

        ostring str = n1.ToString();
        ASSERT_FALSE(str.empty());

        ostring str2 = n1.ToString(OTEXT("999"));
        ASSERT_EQ(ostring(OTEXT(" 123")), str2);

        Number n10(env, ostring(OTEXT("789")), ostring(OTEXT("999")));
        ASSERT_EQ(789, static_cast<OCI_Int32>(n10));

        Number n11(env, OTEXT("-42"), OTEXT("S999"));
        ASSERT_EQ(-42, static_cast<OCI_Int32>(n11));
    }

    TEST(TestNumberCpp, NullToString)
    {
        Number n;
        ASSERT_EQ(ostring(OTEXT("NULL")), n.ToString());
    }
}
