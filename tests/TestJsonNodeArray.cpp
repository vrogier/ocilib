#include "ocilib_tests.h"

#include <cstring>

using namespace ocilib;

namespace
{
    OCI_Number* MakeDoubleNumber(OCI_Environment* env, OCI_Double value)
    {
        OCI_Number* num = OCI_NumberCreate(env);
        if (num)
        {
            OCI_NumberSetValue(num, OCI_NUM_DOUBLE, &value);
        }
        return num;
    }
}

namespace TestCApi
{
    TEST(TestJsonNodeArray, ArrayAccessC)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);
        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));

        OCI_JsonNode* arr = OCI_JsonNodeCreateArray(json);
        ASSERT_NE(nullptr, arr);

        OCI_JsonNode* item1 = OCI_JsonNodeCreateString(json, OTEXT("first"), 0);
        OCI_JsonNode* item2 = OCI_JsonNodeCreateString(json, OTEXT("second"), 0);

        ASSERT_TRUE(OCI_JsonNodeAppendItem(arr, item1));
        ASSERT_TRUE(OCI_JsonNodeAppendItem(arr, item2));

        ASSERT_EQ(2u, OCI_JsonNodeGetItemCount(arr));

        OCI_JsonNode* it1 = OCI_JsonNodeGetItem(arr, 0);
        ASSERT_NE(nullptr, it1);
        ASSERT_EQ(OCI_JNT_STRING, OCI_JsonNodeGetType(it1));
        ASSERT_EQ(String(OTEXT("first")), String(OCI_JsonNodeGetString(it1, nullptr)));
        OCI_JsonNodeFree(it1);

        OCI_JsonNode* it2 = OCI_JsonNodeGetItem(arr, 1);
        ASSERT_NE(nullptr, it2);
        ASSERT_EQ(OCI_JNT_STRING, OCI_JsonNodeGetType(it2));
        ASSERT_EQ(String(OTEXT("second")), String(OCI_JsonNodeGetString(it2, nullptr)));
        OCI_JsonNodeFree(it2);

        OCI_JsonNode* v1 = OCI_JsonNodeGetItem(arr, 0);
        ASSERT_NE(nullptr, v1);
        ASSERT_EQ(OCI_JNT_STRING, OCI_JsonNodeGetType(v1));
        ASSERT_EQ(String(OTEXT("first")), String(OCI_JsonNodeGetString(v1, nullptr)));
        OCI_JsonNodeFree(v1);

        OCI_JsonNodeFree(item1);
        OCI_JsonNodeFree(item2);

        ASSERT_TRUE(OCI_JsonSetRoot(json, arr));
        OCI_JsonNodeFree(arr);

        OCI_JsonNode* verify = OCI_JsonGetRoot(json);
        ASSERT_NE(nullptr, verify);
        ASSERT_EQ(2u, OCI_JsonNodeGetItemCount(verify));
        OCI_JsonNodeFree(verify);

        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
    TEST(TestJsonNodeArray, BadPathsC)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);
        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));

        OCI_JsonNode* scalar = OCI_JsonNodeCreateString(json, OTEXT("hello"), 5);
        OCI_JsonNode* obj = OCI_JsonNodeCreateObject(json);
        OCI_JsonNode* arr = OCI_JsonNodeCreateArray(json);
        OCI_JsonNode* child = OCI_JsonNodeCreateString(json, OTEXT("child"), 5);

        ASSERT_EQ(nullptr, OCI_JsonNodeGetFieldByName(scalar, OTEXT("x")));
        ASSERT_EQ(nullptr, OCI_JsonNodeGetFieldByIndex(scalar, 1));
        ASSERT_EQ(nullptr, OCI_JsonNodeGetFieldName(scalar, 1));
        ASSERT_FALSE(OCI_JsonNodeSetField(scalar, OTEXT("x"), child));

        ASSERT_EQ(nullptr, OCI_JsonNodeGetItem(obj, 1));
        ASSERT_FALSE(OCI_JsonNodeAppendItem(obj, child));
        ASSERT_FALSE(OCI_JsonNodeSetItem(obj, 1, child));

        ASSERT_FALSE(OCI_JsonNodeSetField(arr, OTEXT("x"), child));
        ASSERT_EQ(nullptr, OCI_JsonNodeGetFieldByName(arr, OTEXT("x")));
        ASSERT_EQ(nullptr, OCI_JsonNodeGetFieldByIndex(arr, 1));

        OCI_Double v = 1.0;
        OCI_Number* num = MakeDoubleNumber(env, v);
        OCI_JsonNode* number = OCI_JsonNodeCreateNumber(json, num);
        OCI_NumberFree(num);

        ASSERT_FALSE(OCI_JsonNodeSetString(number, OTEXT("x"), 1));

        OCI_JsonNode* one = OCI_JsonNodeCreateString(json, OTEXT("only"), 4);
        ASSERT_TRUE(OCI_JsonNodeAppendItem(arr, one));
        OCI_JsonNodeFree(one);

        ASSERT_TRUE(OCI_JsonNodeSetField(obj, OTEXT("f"), child));

        ASSERT_EQ(nullptr, OCI_JsonNodeGetItem(arr, 100));
        ASSERT_FALSE(OCI_JsonNodeSetItem(arr, 100, child));
        ASSERT_EQ(nullptr, OCI_JsonNodeGetFieldByIndex(obj, 100));
        ASSERT_EQ(nullptr, OCI_JsonNodeGetFieldName(obj, 100));

        OCI_JsonNodeFree(number);
        OCI_JsonNodeFree(child);
        OCI_JsonNodeFree(arr);
        OCI_JsonNodeFree(obj);
        OCI_JsonNodeFree(scalar);

        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
}
