#include "ocilib_tests.h"
#include "TestFeatures.h"

#include <cstring>

using namespace ocilib;

namespace TestCppApi
{

    TEST(TestJsonNodeObjectCpp, ObjectFieldAccess_ReturnsValues)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_JSON);

        Json json(con);
        json.SetMutable(true);
        json.LoadText(OTEXT("null"));

        JsonNode root = json.CreateObject();

        JsonNode field = json.CreateString(OTEXT("alpha"));

        root.SetField(OTEXT("only"), field);

        ASSERT_EQ(1u, root.GetFieldCount());

        JsonNode a = root.GetFieldByName(OTEXT("only"));
        ASSERT_EQ(Json::NodeString, a.GetType());

        JsonNode b = root.GetFieldByIndex(0);
        ASSERT_EQ(Json::NodeString, b.GetType());

        json.SetRoot(root);

        JsonNode verify = json.GetRoot();
        ASSERT_EQ(1u, verify.GetFieldCount());
    }
    TEST(TestJsonNodeObjectCpp, GetFieldNameAtIndex_ReturnsName)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_JSON);

        Json json(con);
        json.SetMutable(true);
        json.LoadText(OTEXT("null"));

        JsonNode root = json.CreateObject();
        JsonNode field = json.CreateString(OTEXT("alpha"));
        root.SetField(OTEXT("only"), field);

        String name = root.GetFieldName(0);
        ASSERT_EQ(String(OTEXT("only")), name);
    }

    TEST(TestJsonNodeObjectCpp, RenameThenAssignThenClone_PropagatesValues)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_JSON);

        Json json(con);
        json.SetMutable(true);
        json.LoadText(OTEXT("{\"a\":1,\"b\":2}"));

        JsonNode root = json.GetRoot();

        root.RenameField(OTEXT("a"), OTEXT("c"));
        ASSERT_TRUE(root.HasField(OTEXT("c")));
        ASSERT_FALSE(root.HasField(OTEXT("a")));

        JsonNode clone = root.Clone();
        ASSERT_TRUE(root.Equals(clone));
        ASSERT_TRUE(clone.Equals(root));
        ASSERT_FALSE(clone.GetJson().IsNull());

        JsonNode other = json.CreateObject();
        other.Assign(root);
        ASSERT_TRUE(other.Equals(root));

        /* same shape, one different value — proves Equals detects a real diff
           (comparing two totally-unrelated objects would pass even on error) */
        JsonNode different = root.Clone();
        different.SetField(OTEXT("b"), json.CreateString(OTEXT("changed")));
        ASSERT_FALSE(different.Equals(root));
    }
}