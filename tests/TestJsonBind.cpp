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
    TEST(TestJsonBind, BindAndFetch)
    {
        ExecDML(OTEXT("CREATE TABLE TestJson_CApiBind (id NUMBER, value JSON)"));

        auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = JsonFromText(conn, OTEXT("{\"bound\":true}"));
        ASSERT_NE(nullptr, json);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("INSERT INTO TestJson_CApiBind (id, value) VALUES (:id, :j)")));
        OCI_Int32 id = 1;
        ASSERT_NE(nullptr, OCI_BindInt(stmt, OTEXT(":id"), &id));
        ASSERT_NE(nullptr, OCI_BindJson(stmt, OTEXT(":j"), json));
        ASSERT_TRUE(OCI_Execute(stmt));
        ASSERT_TRUE(OCI_StatementFree(stmt));

        const auto stmt2 = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt2);
        ASSERT_TRUE(OCI_Prepare(stmt2, OTEXT("SELECT value FROM TestJson_CApiBind WHERE id = :id")));
        ASSERT_NE(nullptr, OCI_BindInt(stmt2, OTEXT(":id"), &id));
        ASSERT_TRUE(OCI_Execute(stmt2));

        const auto rs = OCI_GetResultset(stmt2);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto fetched = OCI_GetJson(rs, 1);
        ASSERT_NE(nullptr, fetched);
        const auto fetchedByName = OCI_GetJsonByName(rs, OTEXT("VALUE"));
        ASSERT_NE(nullptr, fetchedByName);

        ostring text = GetJsonText(fetched);
        ASSERT_NE(ostring::npos, text.find(OTEXT("bound")));

        ASSERT_TRUE(OCI_StatementFree(stmt2));
        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TABLE TestJson_CApiBind"));
    }
    TEST(TestJsonBind, BindArray)
    {
        ExecDML(OTEXT("CREATE TABLE TestJson_CApiBindArray (id NUMBER, value JSON)"));

        auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("INSERT INTO TestJson_CApiBindArray (id, value) VALUES (:id, :j)")));

        OCI_Int32 ids[2] = { 1, 2 };
        OCI_Json** jsons = OCI_JsonArrayCreate(conn, 2);
        ASSERT_NE(nullptr, jsons);

        ASSERT_TRUE(OCI_JsonLoadText(jsons[0], OTEXT("{\"a\":1}"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));
        ASSERT_TRUE(OCI_JsonLoadText(jsons[1], OTEXT("{\"a\":2}"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));

        ASSERT_TRUE(OCI_BindArraySetSize(stmt, 2));
        ASSERT_NE(nullptr, OCI_BindArrayOfInts(stmt, OTEXT(":id"), ids, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfJsons(stmt, OTEXT(":j"), jsons, 0));
        ASSERT_TRUE(OCI_Execute(stmt));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_JsonArrayFree(jsons));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TABLE TestJson_CApiBindArray"));
    }
    TEST(TestJsonBind, RegisterReturning)
    {
        ExecDML(OTEXT("CREATE TABLE TestJson_CApiRegister (id NUMBER, value JSON)"));
        ExecDML(OTEXT("INSERT INTO TestJson_CApiRegister VALUES (1, '{\"x\":1}')"));

        auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto json = JsonFromText(conn, OTEXT("{\"updated\":true}"));
        ASSERT_NE(nullptr, json);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("UPDATE TestJson_CApiRegister SET value = :j WHERE id = :id RETURNING value INTO :out")));
        OCI_Int32 id = 1;
        ASSERT_NE(nullptr, OCI_BindInt(stmt, OTEXT(":id"), &id));
        ASSERT_NE(nullptr, OCI_BindJson(stmt, OTEXT(":j"), json));
        ASSERT_TRUE(OCI_RegisterJson(stmt, OTEXT(":out")));
        ASSERT_TRUE(OCI_Execute(stmt));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto out = OCI_GetJson(rs, 1);
        ASSERT_NE(nullptr, out);

        ostring text = GetJsonText(out);
        ASSERT_NE(ostring::npos, text.find(OTEXT("updated")));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_JsonFree(json));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TABLE TestJson_CApiRegister"));
    }
}
