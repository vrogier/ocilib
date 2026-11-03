#include "ocilib_tests.h"

#include <cstring>

using namespace ocilib;

namespace TestCppApi
{

    static Json MakeJson(const Connection& con, const ostring& text)
    {
        Json json(con);
        json.LoadText(text);
        return json;
    }

    TEST(TestJsonTextCpp, LoadTextAndToText)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json json = MakeJson(con, OTEXT("{\"name\":\"test\",\"value\":42}"));
        ostring text = json.ToText();
        ASSERT_NE(ostring::npos, text.find(OTEXT("name")));
        ASSERT_NE(ostring::npos, text.find(OTEXT("test")));
        ASSERT_NE(ostring::npos, text.find(OTEXT("value")));
        ASSERT_NE(ostring::npos, text.find(OTEXT("42")));

        ASSERT_EQ(json.ToText(), json.ToString());

        Json json2(con);
        json2.LoadText(OTEXT("{\"pointer\":true}"), 0);
        ASSERT_NE(ostring::npos, json2.ToText().find(OTEXT("pointer")));

        Json json3(con);
        json3.LoadText(OTEXT("[1,2,3]"));
        ASSERT_NE(ostring::npos, json3.ToText(Json::Pretty).find(OTEXT("1")));
    }
    TEST(TestJsonTextCpp, BinaryRoundTrip)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json src = MakeJson(con, OTEXT("[1,2,3]"));
        Raw bin = src.ToBinary();
        ASSERT_FALSE(bin.empty());

        Json dest(con);
        dest.LoadBinary(bin);
        ASSERT_EQ(src.ToText(), dest.ToText());
    }
    TEST(TestJsonTextCpp, Clone)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json src = MakeJson(con, OTEXT("{\"clone\":true}"));
        Json dest(con);
        ASSERT_TRUE(src.Clone(dest));
        ASSERT_EQ(src.ToText(), dest.ToText());
    }
}
