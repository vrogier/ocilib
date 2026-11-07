#include "ocilib_tests.h"

#include <cstring>

using namespace ocilib;

namespace TestCApi
{
    TEST(TestJsonNodeObject, ObjectAccessC)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);
        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));

        OCI_JsonNode* root = OCI_JsonNodeCreateObject(json);
        ASSERT_NE(nullptr, root);

        OCI_JsonNode* field = OCI_JsonNodeCreateString(json, OTEXT("alpha"), 0);

        ASSERT_TRUE(OCI_JsonNodeSetField(root, OTEXT("only"), field));

        OCI_JsonNodeFree(field);

        ASSERT_EQ(1u, OCI_JsonNodeGetFieldCount(root));

        OCI_JsonNode* a = OCI_JsonNodeGetFieldByName(root, OTEXT("only"));
        ASSERT_NE(nullptr, a);
        ASSERT_EQ(OCI_JNT_STRING, OCI_JsonNodeGetType(a));
        OCI_JsonNodeFree(a);

        OCI_JsonNode* b = OCI_JsonNodeGetFieldByIndex(root, 0);
        ASSERT_NE(nullptr, b);
        ASSERT_EQ(OCI_JNT_STRING, OCI_JsonNodeGetType(b));
        OCI_JsonNodeFree(b);

        ASSERT_TRUE(OCI_JsonSetRoot(json, root));
        OCI_JsonNodeFree(root);

        OCI_JsonNode* verify = OCI_JsonGetRoot(json);
        ASSERT_NE(nullptr, verify);
        ASSERT_EQ(1u, OCI_JsonNodeGetFieldCount(verify));
        OCI_JsonNodeFree(verify);

        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
    TEST(TestJsonNodeObject, FieldNameC)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json);
        ASSERT_TRUE(OCI_JsonSetMutable(json, TRUE));
        ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("null"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));

        OCI_JsonNode* root = OCI_JsonNodeCreateObject(json);
        ASSERT_NE(nullptr, root);

        OCI_JsonNode* field = OCI_JsonNodeCreateString(json, OTEXT("alpha"), 1);
        ASSERT_TRUE(OCI_JsonNodeSetField(root, OTEXT("only"), field));
        OCI_JsonNodeFree(field);

        const otext* name = OCI_JsonNodeGetFieldName(root, 0);
        ASSERT_NE(nullptr, name);
        ASSERT_EQ(String(OTEXT("only")), String(name));

        OCI_JsonNodeFree(root);

        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
}
