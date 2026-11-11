#include "ocilib_tests.h"
#include "NullArgs.h"

namespace TestCApi
{
    TEST(TestHash, CreateStringHash)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto table = OCI_HashTableCreate(env, OCI_HASH_DEFAULT_SIZE, OCI_HASH_STRING);
        ASSERT_NE(nullptr, table);
        ASSERT_EQ(OCI_HASH_STRING, OCI_HashTableGetType(table));
        ASSERT_EQ(OCI_HASH_DEFAULT_SIZE, OCI_HashTableGetSize(table));

        ASSERT_TRUE(OCI_HashTableFree(table));
    }

    TEST(TestHash, AddAndGetString)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto table = OCI_HashTableCreate(env, OCI_HASH_DEFAULT_SIZE, OCI_HASH_STRING);
        ASSERT_NE(nullptr, table);

        ASSERT_TRUE(OCI_HashTableAddString(table, OTEXT("key"), OTEXT("value")));
        const otext* str = OCI_HashTableGetString(table, OTEXT("key"));
        ASSERT_NE(nullptr, str);
        ASSERT_EQ(String(OTEXT("value")), String(str));

        ASSERT_TRUE(OCI_HashTableFree(table));
    }

    TEST(TestHash, LookupAndGetValue)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto table = OCI_HashTableCreate(env, OCI_HASH_DEFAULT_SIZE, OCI_HASH_STRING);
        ASSERT_NE(nullptr, table);

        ASSERT_TRUE(OCI_HashTableAddString(table, OTEXT("key"), OTEXT("value")));

        auto entry = OCI_HashTableLookup(table, OTEXT("key"), TRUE);
        ASSERT_NE(nullptr, entry);
        auto value = OCI_HashTableGetValue(table, OTEXT("key"));
        ASSERT_NE(nullptr, value);

        ASSERT_TRUE(OCI_HashTableFree(table));
    }

    TEST(TestHash, AddAndGetInt)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto itable = OCI_HashTableCreate(env, 16, OCI_HASH_INTEGER);
        ASSERT_NE(nullptr, itable);
        ASSERT_TRUE(OCI_HashTableAddInt32(itable, OTEXT("i"), 123));
        ASSERT_EQ(123, OCI_HashTableGetInt32(itable, OTEXT("i")));
        ASSERT_TRUE(OCI_HashTableFree(itable));
    }

    TEST(TestHash, AddAndGetPointer)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto ptable = OCI_HashTableCreate(env, 16, OCI_HASH_POINTER);
        ASSERT_NE(nullptr, ptable);
        OCI_Int32 x = 42;
        ASSERT_TRUE(OCI_HashTableAddPointer(ptable, OTEXT("p"), &x));
        ASSERT_EQ(&x, OCI_HashTableGetPointer(ptable, OTEXT("p")));
        ASSERT_TRUE(OCI_HashTableFree(ptable));
    }

    /* --- public-API coverage of the dropped OcilibHash* internals:
       OCI_HashTableLookup / OCI_HashTableGetValue / OCI_HashTableGetEntry /
       OCI_HashNextEntry / OCI_HashTableGetString traverse the internal value
       chains (OcilibHashEntryValueCount / OcilibHashLookup /
       OcilibHashAddString paths) --- */

    TEST(TestHash, AddStringLookupAndGet)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto table = OCI_HashTableCreate(env, 16, OCI_HASH_STRING);
        ASSERT_NE(nullptr, table);

        ASSERT_TRUE(OCI_HashTableAddString(table, OTEXT("k1"), OTEXT("v1")));
        ASSERT_TRUE(OCI_HashTableAddString(table, OTEXT("k2"), OTEXT("v2")));

        /* OcilibHashLookup + OcilibHashGetValue via public getters */
        ASSERT_EQ(String(OTEXT("v1")), String(OCI_HashTableGetString(table, OTEXT("k1"))));
        ASSERT_EQ(String(OTEXT("v2")), String(OCI_HashTableGetString(table, OTEXT("k2"))));
        ASSERT_EQ(nullptr, OCI_HashTableGetString(table, OTEXT("nope")));

        const auto e1 = OCI_HashTableLookup(table, OTEXT("k1"), FALSE);
        ASSERT_NE(nullptr, e1);
        const auto v1 = OCI_HashTableGetValue(table, OTEXT("k1"));
        ASSERT_NE(nullptr, v1);
        /* entry-by-index walks internal buckets: index 0 may or may not have
           an entry; out-of-range must return NULL */
        const auto e1g = OCI_HashTableGetEntry(table, 0);
        (void)e1g;
        ASSERT_EQ(nullptr, OCI_HashTableGetEntry(table, 0xFFFFFFFFu));

        ASSERT_TRUE(OCI_HashTableFree(table));
    }

    TEST(TestHash, NullHashErrors)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        /* NULL table hits CHECK_PTR arms */
        ASSERT_EQ(0u, OCI_HashTableGetSize(nullptr));
        ASSERT_EQ(OCI_UNKNOWN, (OCI_Int32)OCI_HashTableGetType(nullptr));
        ASSERT_FALSE(OCI_HashTableAddString(nullptr, OTEXT("k"), OTEXT("v")));
        ASSERT_EQ(nullptr, OCI_HashTableGetString(nullptr, OTEXT("k")));
        ASSERT_FALSE(OCI_HashTableAddInt32(nullptr, OTEXT("k"), 1));
        ASSERT_EQ(0, OCI_HashTableGetInt32(nullptr, OTEXT("k")));
        ASSERT_FALSE(OCI_HashTableAddPointer(nullptr, OTEXT("k"), nullptr));
        ASSERT_EQ(nullptr, OCI_HashTableGetPointer(nullptr, OTEXT("k")));
        ASSERT_EQ(nullptr, OCI_HashTableLookup(nullptr, OTEXT("k"), FALSE));
        ASSERT_EQ(nullptr, OCI_HashTableGetValue(nullptr, OTEXT("k")));
        ASSERT_EQ(nullptr, OCI_HashTableGetEntry(nullptr, 0));
        ASSERT_FALSE(OCI_HashTableFree(nullptr));
    }

    class TestHashNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestHashNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestHashNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "HashAddInt", [] { return !OCI_HashTableAddInt32(nullptr, OTEXT("x"), 1); } },
            { "HashAddPointer", [] { return !OCI_HashTableAddPointer(nullptr, OTEXT("x"), NullArgBuffer()); } },
            { "HashAddString", [] { return !OCI_HashTableAddString(nullptr, OTEXT("x"), OTEXT("x")); } },
            { "HashCreate", [] { return nullptr == OCI_HashTableCreate(nullptr, 1, 1); } },
            { "HashFree", [] { return !OCI_HashTableFree(nullptr); } },
            { "HashGetEntry", [] { return nullptr == OCI_HashTableGetEntry(nullptr, 1); } },
            { "HashGetInt", [] { return 0 == OCI_HashTableGetInt32(nullptr, OTEXT("x")); } },
            { "HashGetPointer", [] { return nullptr == OCI_HashTableGetPointer(nullptr, OTEXT("x")); } },
            { "HashGetSize", [] { return 0 == OCI_HashTableGetSize(nullptr); } },
            { "HashGetString", [] { return nullptr == OCI_HashTableGetString(nullptr, OTEXT("x")); } },
            { "HashGetType", [] { return OCI_UNKNOWN == OCI_HashTableGetType(nullptr); } },
            { "HashGetValue", [] { return nullptr == OCI_HashTableGetValue(nullptr, OTEXT("x")); } },
            { "HashLookup", [] { return nullptr == OCI_HashTableLookup(nullptr, OTEXT("x"), FALSE); } },
        }),
        &NullArgCaseName);

}
