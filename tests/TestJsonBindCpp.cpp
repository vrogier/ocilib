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

    TEST(TestJsonBindCpp, BindAndFetch)
    {
        ExecDML(OTEXT("CREATE TABLE TestJson_CppBind (id NUMBER, value JSON)"));

        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json json = MakeJson(con, OTEXT("{\"cpp\":true}"));

        Statement stmt(con);
        stmt.Prepare(OTEXT("INSERT INTO TestJson_CppBind (id, value) VALUES (:id, :j)"));
        OCI_Int32 id = 1;
        stmt.Bind(OTEXT(":id"), id, BindInfo::In);
        stmt.Bind(OTEXT(":j"), json, BindInfo::In);
        stmt.ExecutePrepared();

        stmt.Prepare(OTEXT("SELECT value FROM TestJson_CppBind WHERE id = :id"));
        stmt.Bind(OTEXT(":id"), id, BindInfo::In);
        stmt.ExecutePrepared();

        Resultset rs = stmt.GetResultset();
        ASSERT_TRUE(rs.Next());

        Json fetched = rs.Get<Json>(1);
        ASSERT_FALSE(fetched.IsNull());
        ASSERT_NE(ostring::npos, fetched.ToText().find(OTEXT("cpp")));

        Json fetchedByName = rs.Get<Json>(OTEXT("VALUE"));
        ASSERT_FALSE(fetchedByName.IsNull());

        ExecDML(OTEXT("DROP TABLE TestJson_CppBind"));
    }
    TEST(TestJsonBindCpp, BindArray)
    {
        ExecDML(OTEXT("CREATE TABLE TestJson_CppBindArray (id NUMBER, value JSON)"));

        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        static std::vector<OCI_Int32> ids = { 1, 2 };
        static std::vector<Json> jsons;
        jsons.emplace_back(con);
        jsons.back().LoadText(OTEXT("{\"a\":1}"));
        jsons.emplace_back(con);
        jsons.back().LoadText(OTEXT("{\"a\":2}"));

        Statement stmt(con);
        stmt.Prepare(OTEXT("INSERT INTO TestJson_CppBindArray (id, value) VALUES (:id, :j)"));
        stmt.SetBindArraySize(2);
        stmt.Bind(OTEXT(":id"), ids, BindInfo::In);
        stmt.Bind(OTEXT(":j"), jsons, BindInfo::In);
        stmt.ExecutePrepared();

        ExecDML(OTEXT("DROP TABLE TestJson_CppBindArray"));
    }
    TEST(TestJsonBindCpp, RegisterReturning)
    {
        ExecDML(OTEXT("CREATE TABLE TestJson_CppRegister (id NUMBER, value JSON)"));
        ExecDML(OTEXT("INSERT INTO TestJson_CppRegister VALUES (1, '{\"x\":1}')"));

        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json json = MakeJson(con, OTEXT("{\"updated\":true}"));

        Statement stmt(con);
        stmt.Prepare(OTEXT("UPDATE TestJson_CppRegister SET value = :j WHERE id = :id RETURNING value INTO :out"));
        OCI_Int32 id = 1;
        stmt.Bind(OTEXT(":id"), id, BindInfo::In);
        stmt.Bind(OTEXT(":j"), json, BindInfo::In);
        stmt.Register<Json>(OTEXT(":out"));
        stmt.ExecutePrepared();

        Resultset rs = stmt.GetResultset();
        ASSERT_TRUE(rs.Next());

        Json out = rs.Get<Json>(1);
        ASSERT_FALSE(out.IsNull());
        ASSERT_NE(ostring::npos, out.ToText().find(OTEXT("updated")));

        ExecDML(OTEXT("DROP TABLE TestJson_CppRegister"));
    }
}
