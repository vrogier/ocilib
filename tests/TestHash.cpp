#include "ocilib_tests.h"
#include "test_internals.h"

namespace TestCApi
{
    TEST(TestHash, CreateStringHash)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto table = OCI_HashCreate(env, OCI_HASH_DEFAULT_SIZE, OCI_HASH_STRING);
        ASSERT_NE(nullptr, table);
        ASSERT_EQ(OCI_HASH_STRING, OCI_HashGetType(table));
        ASSERT_EQ(OCI_HASH_DEFAULT_SIZE, OCI_HashGetSize(table));

        ASSERT_TRUE(OCI_HashFree(table));
    }

    TEST(TestHash, AddAndGetString)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto table = OCI_HashCreate(env, OCI_HASH_DEFAULT_SIZE, OCI_HASH_STRING);
        ASSERT_NE(nullptr, table);

        ASSERT_TRUE(OCI_HashAddString(table, OTEXT("key"), OTEXT("value")));
        const otext* str = OCI_HashGetString(table, OTEXT("key"));
        ASSERT_NE(nullptr, str);
        ASSERT_EQ(ostring(OTEXT("value")), ostring(str));

        ASSERT_TRUE(OCI_HashFree(table));
    }

    TEST(TestHash, LookupAndGetValue)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto table = OCI_HashCreate(env, OCI_HASH_DEFAULT_SIZE, OCI_HASH_STRING);
        ASSERT_NE(nullptr, table);

        ASSERT_TRUE(OCI_HashAddString(table, OTEXT("key"), OTEXT("value")));

        auto entry = OCI_HashLookup(table, OTEXT("key"), TRUE);
        ASSERT_NE(nullptr, entry);
        auto value = OCI_HashGetValue(table, OTEXT("key"));
        ASSERT_NE(nullptr, value);

        ASSERT_TRUE(OCI_HashFree(table));
    }

    TEST(TestHash, AddAndGetInt)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto itable = OCI_HashCreate(env, 16, OCI_HASH_INTEGER);
        ASSERT_NE(nullptr, itable);
        ASSERT_TRUE(OCI_HashAddInt(itable, OTEXT("i"), 123));
        ASSERT_EQ(123, OCI_HashGetInt(itable, OTEXT("i")));
        ASSERT_TRUE(OCI_HashFree(itable));
    }

    TEST(TestHash, AddAndGetPointer)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto ptable = OCI_HashCreate(env, 16, OCI_HASH_POINTER);
        ASSERT_NE(nullptr, ptable);
        OCI_Int32 x = 42;
        ASSERT_TRUE(OCI_HashAddPointer(ptable, OTEXT("p"), &x));
        ASSERT_EQ(&x, OCI_HashGetPointer(ptable, OTEXT("p")));
        ASSERT_TRUE(OCI_HashFree(ptable));
    }

    TEST(TestHash, ValueChainAppends)
    {
        /* a second value on the same key is appended to the entry value chain */

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto table = OCI_TestHashCreate(env, 4, OCI_HASH_STRING);
        ASSERT_NE(nullptr, table);

        ASSERT_TRUE(OCI_TestHashAddString(table, OTEXT("key"), OTEXT("first")));
        ASSERT_TRUE(OCI_TestHashAddString(table, OTEXT("key"), OTEXT("second")));

        OCI_Int32 total = 0;
        bool found = false;
        for (OCI_UInt32 i = 0; i < 4; i++)
        {
            const OCI_Int32 n = OCI_TestHashEntryValueCount(table, i);
            total += n;
            found = found || (n == 2);
        }

        ASSERT_TRUE(found);
        ASSERT_EQ(2, total);

        ASSERT_TRUE(OCI_TestHashFree(table));
    }

    TEST(TestHash, EntryValueCountOutOfRange)
    {
        /* out of range index is rejected */

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto table = OCI_TestHashCreate(env, 4, OCI_HASH_STRING);
        ASSERT_NE(nullptr, table);

        ASSERT_EQ(0, OCI_TestHashEntryValueCount(table, 99));

        ASSERT_TRUE(OCI_TestHashFree(table));
    }
}
