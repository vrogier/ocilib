#include "ocilib_tests.h"

using namespace ocilib;

namespace TestCppApi
{

    static Resultset GetColsResultset(Statement& st)
    {
        return st.ExecuteAndGetResultset(
            OTEXT("select 1 as id, 'name' as ename, 1000 as sal, sysdate as hired from dual"));
    }

    /* ---------------------------------------------------- */
    /* ResultsetColumns — ColumnView<Resultset>              */
    /* ---------------------------------------------------- */

    TEST(TestColumnsView, Size)
    {
        auto& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        auto rs = GetColsResultset(st);

        ASSERT_EQ(4u, rs.Columns().Size());
    }

    TEST(TestColumnsView, SubscriptByIndexIsZeroBased)
    {
        auto& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        auto rs = GetColsResultset(st);
        auto cols = rs.Columns();

        ASSERT_EQ(String(OTEXT("ID")),    cols[0].GetName());
        ASSERT_EQ(String(OTEXT("ENAME")), cols[1].GetName());
        ASSERT_EQ(String(OTEXT("HIRED")), cols[3].GetName());

        /* [0] maps to the 1-based GetColumn(0) */
        ASSERT_EQ(cols[0].GetName(), rs.GetColumn(0).GetName());
    }

    TEST(TestColumnsView, SubscriptByName)
    {
        auto& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        auto rs = GetColsResultset(st);
        auto cols = rs.Columns();

        ASSERT_EQ(String(OTEXT("ENAME")), cols[OTEXT("ENAME")].GetName());
        ASSERT_EQ(String(OTEXT("SAL")),   cols[OTEXT("SAL")].GetName());
    }

    TEST(TestColumnsView, AtIsBoundsChecked)
    {
        auto& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        auto rs = GetColsResultset(st);
        auto cols = rs.Columns();

        ASSERT_EQ(String(OTEXT("ID")), cols.At(0).GetName());
        ASSERT_THROW((void)cols.At(cols.Size()), Exception);
    }

    TEST(TestColumnsView, Contains)
    {
        auto& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        auto rs = GetColsResultset(st);
        auto cols = rs.Columns();

        ASSERT_TRUE(cols.Contains(OTEXT("ENAME")));
        ASSERT_TRUE(cols.Contains(OTEXT("ID")));
        ASSERT_FALSE(cols.Contains(OTEXT("NOSUCHCOL")));
    }

    TEST(TestColumnsView, IndexOf)
    {
        auto& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        auto rs = GetColsResultset(st);
        auto cols = rs.Columns();

        ASSERT_EQ(0, cols.IndexOf(OTEXT("ID")));
        ASSERT_EQ(1, cols.IndexOf(OTEXT("ENAME")));
        ASSERT_EQ(3, cols.IndexOf(OTEXT("HIRED")));
        ASSERT_EQ(-1, cols.IndexOf(OTEXT("NOSUCHCOL")));
    }

    TEST(TestColumnsView, IterateAll)
    {
        auto& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        auto rs = GetColsResultset(st);
        auto cols = rs.Columns();

        static const otext* names[] = { OTEXT("ID"), OTEXT("ENAME"), OTEXT("SAL"), OTEXT("HIRED") };

        UInt32 i = 0;
        for (auto&& col : cols)
        {
            ASSERT_LT(i, cols.Size());
            ASSERT_EQ(String(names[i]), col.GetName());
            ++i;
        }
        ASSERT_EQ(cols.Size(), i);
    }

    TEST(TestColumnsView, IteratorIsRandomAccess)
    {
        auto& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        auto rs = GetColsResultset(st);
        auto cols = rs.Columns();

        auto it = cols.begin();
        ASSERT_EQ(String(OTEXT("ID")), (*it).GetName());

        /* operator+ / operator- between iterators */
        auto it2 = it + 2;
        ASSERT_EQ(String(OTEXT("SAL")), (*it2).GetName());
        ASSERT_EQ(2, static_cast<int>(it2 - it));

        /* compound assignment and -- */
        it2 -= 1;
        ASSERT_TRUE(it2 == it + 1);
        ASSERT_TRUE(it2 > it);
        ASSERT_TRUE(it < it2);
        ASSERT_TRUE(it2 != it);

        --it2;
        ASSERT_TRUE(it2 == it);

        /* operator[] offset (0-based) */
        ASSERT_EQ(String(OTEXT("HIRED")), it[3].GetName());

        /* end() sentinel */
        auto end = cols.end();
        ASSERT_TRUE(end != it);
        ASSERT_EQ(static_cast<int>(cols.Size()), static_cast<int>(end - it));
    }

    /* ---------------------------------------------------- */
    /* ResultsetRows                                         */
    /* ---------------------------------------------------- */

    TEST(TestResultsetRows, IterateAll)
    {
        auto& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        auto rs = st.ExecuteAndGetResultset(
            OTEXT("select rownum from (select 1 from dual connect by level <= 5)"));

        UInt32 count = 0;
        for (auto&& row : rs.Rows())
        {
            ++count;
            ASSERT_EQ(static_cast<int>(count), row.Get<int>(0));
        }
        ASSERT_EQ(5u, count);
    }

    TEST(TestResultsetRows, SizeTracksFetchedRows)
    {
        auto& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        auto rs = st.ExecuteAndGetResultset(
            OTEXT("select rownum from (select 1 from dual connect by level <= 3)"));
        auto rows = rs.Rows();

        ASSERT_EQ(0u, rows.Size());
        auto it = rows.begin();
        ASSERT_EQ(1u, rows.Size());
        ++it;
        ASSERT_EQ(2u, rows.Size());
    }

    TEST(TestResultsetRows, SubscriptOnScrollable)
    {
        auto& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        st.SetFetchMode(Statement::FetchScrollable);
        auto rs = st.ExecuteAndGetResultset(
            OTEXT("select rownum from (select 1 from dual connect by level <= 5)"));
        auto rows = rs.Rows();

        /* 0-based: rows[2] is the 3rd row */
        ASSERT_EQ(3, rows[2].Get<int>(0));
        ASSERT_EQ(1, rows[0].Get<int>(0));
    }

    TEST(TestResultsetRows, SubscriptOnForwardOnlyThrows)
    {
        auto& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        auto rs = st.ExecuteAndGetResultset(
            OTEXT("select rownum from (select 1 from dual connect by level <= 3)"));

        ASSERT_THROW((void)rs.Rows()[0], Exception);
    }

    TEST(TestResultsetRows, EndIsSentinel)
    {
        auto& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        auto rs = st.ExecuteAndGetResultset(OTEXT("select rownum from dual where 1 = 0"));

        ASSERT_TRUE(rs.Rows().begin() == rs.Rows().end());
    }

    /* ---------------------------------------------------- */
    /* TypeInfoColumns — ColumnView<TypeInfo>                */
    /* ---------------------------------------------------- */

    TEST(TestTypeInfoColumns, IterateTableColumns)
    {
        auto& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        TypeInfo ti(con, OTEXT("DUAL"), TypeInfo::Table);
        auto cols = ti.Columns();

        ASSERT_EQ(1u, cols.Size());
        ASSERT_EQ(String(OTEXT("DUMMY")), cols[0].GetName());
        ASSERT_TRUE(cols.Contains(OTEXT("DUMMY")));
        ASSERT_EQ(0, cols.IndexOf(OTEXT("DUMMY")));

        UInt32 count = 0;
        for (auto&& col : cols)
        {
            ++count;
            ASSERT_EQ(String(OTEXT("DUMMY")), col.GetName());
        }
        ASSERT_EQ(1u, count);
    }

    TEST(TestTypeInfoColumns, IndexOfMiss)
    {
        auto& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        TypeInfo ti(con, OTEXT("DUAL"), TypeInfo::Table);
        auto cols = ti.Columns();

        ASSERT_EQ(-1, cols.IndexOf(OTEXT("NOSUCHCOL")));
        ASSERT_FALSE(cols.Contains(OTEXT("NOSUCHCOL")));
    }

    /* ---------------------------------------------------- */
    /* Zero-based [] regression — JsonNode / Collection      */
    /* ---------------------------------------------------- */

    TEST(TestZeroBased, CollectionSubscriptFirstElement)
    {
        auto& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        auto rs = st.ExecuteAndGetResultset(
            OTEXT("select sys.odcivarchar2list('a','b','c') from dual"));
        rs.Next();

        Collection<String> coll = rs.Get<Collection<String>>(0);
        ASSERT_EQ(3u, coll.GetSize());
        /* 0-based: coll[0] is the first element */
        ASSERT_EQ(String(OTEXT("a")), static_cast<String>(coll[0]));
        ASSERT_EQ(String(OTEXT("c")), static_cast<String>(coll[2]));
    }

    TEST(TestZeroBased, JsonNodeSubscriptFirstItem)
    {
        auto& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json doc(con);
        doc.LoadText(OTEXT("[10, 20, 30]"));
        JsonNode root = doc.GetRoot();

        /* 0-based: node[0] is the first item */
        ASSERT_EQ(10, root[0].GetNumeric<int>());
        ASSERT_EQ(30, root[2].GetNumeric<int>());

        /* iterator positions are 0-based: first deref = first item */
        auto it = root.begin();
        ASSERT_EQ(10, (*it).GetNumeric<int>());
    }

}
