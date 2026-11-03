#include "ocilib_tests.h"

#include <cstring>

using namespace ocilib;

namespace
{
    OCI_Json* JsonFromText(OCI_Connection* conn, const otext* text)
    {
        OCI_Json* json = OCI_JsonCreate(conn);
        if (!json) return nullptr;
        if (!OCI_JsonLoadText(json, text, 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT))
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

namespace TestCApi
{
    TEST(TestJsonText, LoadTextAndToText)
    {
        auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = JsonFromText(conn, OTEXT("{\"name\":\"test\",\"value\":42}"));
        ASSERT_NE(nullptr, json);

        ostring text = GetJsonText(json);
        ASSERT_NE(ostring::npos, text.find(OTEXT("name")));
        ASSERT_NE(ostring::npos, text.find(OTEXT("test")));
        ASSERT_NE(ostring::npos, text.find(OTEXT("value")));
        ASSERT_NE(ostring::npos, text.find(OTEXT("42")));

        OCI_UInt32 prettyLen = 0;
        ASSERT_TRUE(OCI_JsonToText(json, nullptr, &prettyLen, OCI_JTP_PRETTY, TRUE));
        ASSERT_GT(prettyLen, 0u);

        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
    TEST(TestJsonText, BinaryRoundTrip)
    {
        auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json1 = JsonFromText(conn, OTEXT("[1,2,3]"));
        ASSERT_NE(nullptr, json1);

        OCI_UInt32 binLen = 0;
        ASSERT_TRUE(OCI_JsonToBinary(json1, nullptr, &binLen));
        ASSERT_GT(binLen, 0u);

        unsigned char* buffer = new unsigned char[binLen];
        OCI_UInt32 binLen2 = binLen;
        ASSERT_TRUE(OCI_JsonToBinary(json1, buffer, &binLen2));

        const auto json2 = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, json2);
        ASSERT_TRUE(OCI_JsonLoadBinary(json2, buffer, binLen2));

        ostring text1 = GetJsonText(json1);
        ostring text2 = GetJsonText(json2);
        ASSERT_EQ(text1, text2);

        delete[] buffer;
        ASSERT_TRUE(OCI_JsonFree(json2));
        ASSERT_TRUE(OCI_JsonFree(json1));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
    TEST(TestJsonText, Clone)
    {
        auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto src = JsonFromText(conn, OTEXT("{\"clone\":true}"));
        ASSERT_NE(nullptr, src);

        const auto dest = OCI_JsonCreate(conn);
        ASSERT_NE(nullptr, dest);
        ASSERT_TRUE(OCI_JsonClone(src, dest));

        ASSERT_EQ(GetJsonText(src), GetJsonText(dest));

        ASSERT_TRUE(OCI_JsonFree(dest));
        ASSERT_TRUE(OCI_JsonFree(src));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
}
