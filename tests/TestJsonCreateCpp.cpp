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

    TEST(TestJsonCreateCpp, CreateAndFree)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json json;
        ASSERT_TRUE(json.IsNull());

        Json json2(con);
        ASSERT_FALSE(json2.IsNull());
    }
}
