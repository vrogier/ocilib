#include "ocilib_tests.h"

#include <cstring>

using namespace ocilib;

namespace TestCppApi
{

    TEST(TestJsonNodeObjectCpp, ObjectAccess)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json json(con);
        json.SetMutable(true);
        json.LoadText(OTEXT("null"));

        JsonNode root = json.CreateObject();

        JsonNode field = json.CreateString(OTEXT("alpha"));

        root.SetField(OTEXT("only"), field);

        ASSERT_EQ(1u, root.GetFieldCount());

        JsonNode a = root.GetFieldByName(OTEXT("only"));
        ASSERT_EQ(Json::NodeString, a.GetType());

        JsonNode b = root.GetFieldByIndex(1);
        ASSERT_EQ(Json::NodeString, b.GetType());

        json.SetRoot(root);

        JsonNode verify = json.GetRoot();
        ASSERT_EQ(1u, verify.GetFieldCount());
    }
    TEST(TestJsonNodeObjectCpp, FieldName)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json json(con);
        json.SetMutable(true);
        json.LoadText(OTEXT("null"));

        JsonNode root = json.CreateObject();
        JsonNode field = json.CreateString(OTEXT("alpha"));
        root.SetField(OTEXT("only"), field);

        ostring name = root.GetFieldName(1);
        ASSERT_EQ(ostring(OTEXT("only")), name);
    }
}
