#include "ocilib_tests.h"

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

    /* --- public-API coverage of the dropped OcilibHash* internals:
       OCI_HashLookup / OCI_HashGetValue / OCI_HashGetEntry /
       OCI_HashNextEntry / OCI_HashGetString traverse the internal value
       chains (OcilibHashEntryValueCount / OcilibHashLookup /
       OcilibHashAddString paths) --- */

    TEST(TestHash, AddStringLookupAndGet)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto table = OCI_HashCreate(env, 16, OCI_HASH_STRING);
        ASSERT_NE(nullptr, table);

        ASSERT_TRUE(OCI_HashAddString(table, OTEXT("k1"), OTEXT("v1")));
        ASSERT_TRUE(OCI_HashAddString(table, OTEXT("k2"), OTEXT("v2")));

        /* OcilibHashLookup + OcilibHashGetValue via public getters */
        ASSERT_EQ(ostring(OTEXT("v1")), ostring(OCI_HashGetString(table, OTEXT("k1"))));
        ASSERT_EQ(ostring(OTEXT("v2")), ostring(OCI_HashGetString(table, OTEXT("k2"))));
        ASSERT_EQ(nullptr, OCI_HashGetString(table, OTEXT("nope")));

        const auto e1 = OCI_HashLookup(table, OTEXT("k1"), FALSE);
        ASSERT_NE(nullptr, e1);
        const auto v1 = OCI_HashGetValue(table, OTEXT("k1"));
        ASSERT_NE(nullptr, v1);
        /* entry-by-index walks internal buckets: index 0 may or may not have
           an entry; out-of-range must return NULL */
        const auto e1g = OCI_HashGetEntry(table, 0);
        (void)e1g;
        ASSERT_EQ(nullptr, OCI_HashGetEntry(table, 0xFFFFFFFFu));

        ASSERT_TRUE(OCI_HashFree(table));
    }

    TEST(TestHash, NullHashErrors)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        /* NULL table hits CHECK_PTR arms */
        ASSERT_EQ(0u, OCI_HashGetSize(nullptr));
        ASSERT_EQ(OCI_UNKNOWN, (OCI_Int32)OCI_HashGetType(nullptr));
        ASSERT_FALSE(OCI_HashAddString(nullptr, OTEXT("k"), OTEXT("v")));
        ASSERT_EQ(nullptr, OCI_HashGetString(nullptr, OTEXT("k")));
        ASSERT_FALSE(OCI_HashAddInt(nullptr, OTEXT("k"), 1));
        ASSERT_EQ(0, OCI_HashGetInt(nullptr, OTEXT("k")));
        ASSERT_FALSE(OCI_HashAddPointer(nullptr, OTEXT("k"), nullptr));
        ASSERT_EQ(nullptr, OCI_HashGetPointer(nullptr, OTEXT("k")));
        ASSERT_EQ(nullptr, OCI_HashLookup(nullptr, OTEXT("k"), FALSE));
        ASSERT_EQ(nullptr, OCI_HashGetValue(nullptr, OTEXT("k")));
        ASSERT_EQ(nullptr, OCI_HashGetEntry(nullptr, 0));
        ASSERT_FALSE(OCI_HashFree(nullptr));
    }
}
