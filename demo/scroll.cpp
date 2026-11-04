#include <iostream>

#include "ocilib.hpp"

using namespace ocilib;

void print_row(Resultset &rs)
{
    std::cout << rs.Get<OCI_Int32>(1) << " - " << rs.Get<ostring>(2) << std::endl;
}

int main()
{
    try
    {
        Environment env;

        const char *db  = getenv("OCILIB_DEMO_DB");
        const char *usr = getenv("OCILIB_DEMO_USR");
        const char *pwd = getenv("OCILIB_DEMO_PWD");

        Connection con(env, db, usr, pwd);

        Statement st(con);

        st.SetFetchMode(Statement::FetchScrollable);
        st.Execute("select rownum, 'Row ' || rownum from (select 1 from dual connect by level <= 65)");

        Resultset rs = st.GetResultset();

        rs.Last();
        std::cout << "Total rows : " << rs.GetCount() << std::endl;

        rs.First();
        print_row(rs);

        while (rs.Next())
        {
            print_row(rs);
        }

        while (rs.Prev())
        {
            print_row(rs);
        }

        rs.Seek(Resultset::SeekAbsolute, 30);
        print_row(rs);

        while (rs.GetCurrentRow() < 60 && rs.Next())
        {
            print_row(rs);
        }

        rs.Seek(Resultset::SeekRelative, -15);
        print_row(rs);

    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
