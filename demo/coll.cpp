#include <iostream>

#include "ocilib.hpp"

/* requires script demo/collections.sql */

using namespace ocilib;

void print_product(const Object &product)
{
    std::cout << "...product: " << product.Get<OCI_Int32>("code") << " - " << product.Get<String>("name") << std::endl;
}

void print_coll_by_iterator(Collection<Object> &coll)
{
    std::cout << "fetch using iterators" << std::endl;

    for (auto& item : coll)
    {
        print_product(item);
    }
}

void print_coll_by_index(Collection<Object> &coll)
{
    std::cout << "fetch using index access" << std::endl;

    const OCI_UInt32 n = coll.GetCount();

    for (OCI_UInt32 i = 1; i <= n; ++i)
    {
        print_product(coll[i]);
    }
}


void print_const_coll_by_iterator(const Collection<Object> &coll)
{
    std::cout << "fetch using const iterators" << std::endl;

    for (const auto& item : coll)
    {
        print_product(item);
    }
}

void print_const_coll_by_index(const Collection<Object> &coll)
{
    std::cout << "fetch using const index access" << std::endl;

    const OCI_UInt32 n = coll.GetCount();

    for (OCI_UInt32 i = 1; i <= n; ++i)
    {
        print_product(coll[i]);
    }
}

void bind_coll(Connection &con, Environment &env)
{
    Statement st(con);

    Collection<Object> coll(TypeInfo(con, "product_varray_t", TypeInfo::Type));

    st.Prepare("begin :array := product_varray_t(product_t(123, 'name 123'), product_t(456, 'name 456'), product_t(789, 'name 789')); end;");
    st.Bind(":array", coll, BindInfo::In);
    st.ExecutePrepared();

    print_coll_by_iterator(coll);
    print_coll_by_index(coll);
}

template <class T>
void fetch_coll(Connection &con, String table_name, T func, Environment &env)
{
    Statement st(con);

    st.Execute("select * from " + table_name);
    Resultset rs = st.GetResultset();
    while (rs++)
    {
        std::cout << "#" << rs.Get<String>(1) << std::endl;

        func(rs.Get<Collection<Object>>(2));
    }
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

        bind_coll(con, env);

        fetch_coll(con, "products_varray", print_const_coll_by_iterator, env);
        fetch_coll(con, "products_nested_table", print_const_coll_by_index, env);
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
