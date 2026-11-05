#include "ocilib_tests.h"

using namespace ocilib;

namespace TestCppApi
{

    static Resultset GetRowsResultset(Statement& st, UInt32 count)
    {
        return st.ExecuteAndGetResultset(OTEXT("select rownum, 'Row ' || rownum from (select 1 from dual connect by level <= ") +
                                         TO_STRING(count) + OTEXT(")"));
    }

    TEST(TestResultsetIterator, IterateAllRowsByIndex)
    {
        auto& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        auto rs = GetRowsResultset(st, 5);

        UInt32 count = 0;
        for (auto&& row : rs)
        {
            ++count;
            ASSERT_EQ(static_cast<int>(count), row.Get<int>(1));
        }

        ASSERT_EQ(5u, count);
    }

    TEST(TestResultsetIterator, IterateAllRowsByColumnName)
    {
        auto& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        st.Execute(OTEXT("select rownum as num, 'Row ' || rownum as name from (select 1 from dual connect by level <= 3)"));
        auto rs = st.GetResultset();

        UInt32 count = 0;
        for (auto&& row : rs)
        {
            ++count;
            ASSERT_EQ(String(OTEXT("Row ")) + TO_STRING(count), row.Get<String>(OTEXT("name")));
            ASSERT_EQ(static_cast<int>(count), row.Get<int>(OTEXT("num")));
        }

        ASSERT_EQ(3u, count);
    }

    TEST(TestResultsetIterator, EmptyResultset)
    {
        auto& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        st.Execute(OTEXT("select rownum from dual where 1 = 0"));
        auto rs = st.GetResultset();

        UInt32 count = 0;
        for (auto&& row : rs)
        {
            ++count;
        }

        ASSERT_EQ(0u, count);
        ASSERT_TRUE(rs.begin() == rs.end());
    }

    TEST(TestResultsetIterator, SingleRow)
    {
        auto& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        auto rs = GetRowsResultset(st, 1);

        auto it = rs.begin();
        ASSERT_TRUE(it != rs.end());
        ASSERT_EQ(1, (*it).Get<int>(1));
        ++it;
        ASSERT_TRUE(it == rs.end());
    }

    TEST(TestResultsetIterator, ScrollablePrev)
    {
        auto& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        st.SetFetchMode(Statement::FetchScrollable);
        auto rs = GetRowsResultset(st, 5);

        auto it = rs.begin();
        ++it;
        ++it;
        ASSERT_EQ(3, (*it).Get<int>(1));
        --it;
        ASSERT_EQ(2, (*it).Get<int>(1));
    }

    TEST(TestResultsetIterator, ScrollableRelativeSeek)
    {
        auto& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        st.SetFetchMode(Statement::FetchScrollable);
        auto rs = GetRowsResultset(st, 20);

        auto it = rs.begin();
        it += 10;
        ASSERT_EQ(11, (*it).Get<int>(1));
        it -= 5;
        ASSERT_EQ(6, (*it).Get<int>(1));
    }

    TEST(TestResultsetIterator, ScrollableSubscript)
    {
        auto& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        st.SetFetchMode(Statement::FetchScrollable);
        auto rs = GetRowsResultset(st, 10);

        auto it = rs.begin();
        ASSERT_EQ(4, it[3].Get<int>(1));
    }

    TEST(TestResultsetIterator, ScrollableRangeFor)
    {
        auto& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        st.SetFetchMode(Statement::FetchScrollable);
        auto rs = GetRowsResultset(st, 4);

        UInt32 count = 0;
        for (auto&& row : rs)
        {
            ++count;
            ASSERT_EQ(static_cast<int>(count), row.Get<int>(1));
        }

        ASSERT_EQ(4u, count);
    }

    TEST(TestResultsetIterator, NonScrollablePrevThrows)
    {
        auto& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        auto rs = GetRowsResultset(st, 5);

        auto it = rs.begin();
        ++it;
        ASSERT_THROW((void)(--it), Exception);
    }

    TEST(TestResultsetIterator, NonScrollableSeekThrows)
    {
        auto& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        auto rs = GetRowsResultset(st, 5);

        auto it = rs.begin();
        ASSERT_THROW((void)(it += 2), Exception);
    }

    TEST(TestResultsetIterator, NonScrollableSubscriptThrows)
    {
        auto& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        auto rs = GetRowsResultset(st, 5);

        auto it = rs.begin();
        ASSERT_THROW((void)(it[2]), Exception);
    }

    TEST(TestResultsetIterator, GetStringViewByIndex)
    {
        auto& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        auto rs = GetRowsResultset(st, 3);

        UInt32 count = 0;
        for (auto&& row : rs)
        {
            ++count;
            auto sv = row.Get<StringView>(2);
            ASSERT_EQ(ostring(OTEXT("Row ")) + TO_STRING(count), ostring(sv));
        }
    }

    TEST(TestResultsetIterator, GetStringViewByName)
    {
        auto& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        st.Execute(OTEXT("select 'abc' as name from dual"));
        auto rs = st.GetResultset();

        auto it = rs.begin();
        ASSERT_TRUE(it != rs.end());
        ASSERT_EQ(OTEXT("abc"), (*it).Get<StringView>(OTEXT("name")));
    }

    TEST(TestResultsetIterator, GetStringViewNull)
    {
        auto& env = GetEnvironmentCpp();

        Connection con(env, DBS, USR, PWD);
        Statement st(con);
        st.Execute(OTEXT("select null as name from dual"));
        auto rs = st.GetResultset();

        auto it = rs.begin();
        ASSERT_TRUE(it != rs.end());
        ASSERT_TRUE((*it).Get<StringView>(OTEXT("name")).empty());
    }

}
