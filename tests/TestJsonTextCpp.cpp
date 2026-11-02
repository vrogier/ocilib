#include "ocilib_tests.h"

#include <cstring>

using namespace ocilib;

namespace
{
    OCI_Json* JsonFromText(OCI_Connection* conn, const otext* text)
    {
        OCI_Json* json = OCI_JsonCreate(conn);
        if (!json) return nullptr;
        if (!OCI_JsonLoadText(json, text, 0, OCI_JV_ALLOW_ALL, OCI_JE_DETECT))
        {
            OCI_JsonFree(json);
            return nullptr;
        }
        return json;
    }

    ostring GetJsonText(OCI_Json* json)
    {
        OCI_UInt32 len = 0;
        if (!OCI_JsonToText(json, nullptr, &len, 0, TRUE))
        {
            ADD_FAILURE() << "OCI_JsonToText size query failed";
            return ostring();
        }

        otext* buffer = new otext[len + 1];
        OCI_UInt32 len2 = len + 1;
        if (!OCI_JsonToText(json, buffer, &len2, 0, TRUE))
        {
            delete[] buffer;
            ADD_FAILURE() << "OCI_JsonToText failed";
            return ostring();
        }
        buffer[len2] = 0;

        ostring result(buffer);
        delete[] buffer;
        return result;
    }
}

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
