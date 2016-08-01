#include <iostream>

#include "ocilib.hpp"

/* requires script demo/collections.sql */

using namespace ocilib;

void print_product(const Object &product)
{
    std::cout << "...product: " << product.Get<int>("code") << " - " << product.Get<ostring>("name") << std::endl;
}

void print_coll_by_iterator(Collection<Object> &coll)
{
    std::cout << "fetch using iterators" << std::endl;

    Collection<Object>::Iterator it1 = coll.begin();
    Collection<Object>::Iterator it2 = coll.end();

    for (; it1 != it2; ++it1)
    {
        print_product(static_cast<Object>(*it1));
    }
}

void print_coll_by_index(Collection<Object> &coll)
{
    std::cout << "fetch using index access" << std::endl;

    unsigned int i = 1, n = coll.GetCount();

    for (; i <= n; i++)
    {
        print_product(static_cast<Object>(coll[i]));
    }
}


void print_const_coll_by_iterator(const Collection<Object> &coll)
{
    std::cout << "fetch using const iterators" << std::endl;

    Collection<Object>::ConstIterator it1 = coll.begin();
    Collection<Object>::ConstIterator it2 = coll.end();

    for (; it1 != it2; ++it1)
    {
        print_product(static_cast<Object>(*it1));
    }
}

void print_const_coll_by_index(const Collection<Object> &coll)
{
    std::cout << "fetch using const index access" << std::endl;

    unsigned int i = 1, n = coll.GetCount();

    for (; i <=n ; i++)
    {
        print_product(static_cast<Object>(coll[i]));
    }
}

void bind_coll(Connection &con)
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
void fetch_coll(Connection &con, ostring table_name, T func)
{
    Statement st(con);

    st.Execute("select * from " + table_name);
    Resultset rs = st.GetResultset();
    while (rs++)
    {
        std::cout << "#" << rs.Get<ostring>(1) << std::endl;

        func(rs.Get<Collection<Object> >(2));
    }
}

int main(void)
{
    try
    {
        Environment::Initialize();

        Connection con("db", "usr", "pwd");

        bind_coll(con);

        fetch_coll(con, "products_varray", print_const_coll_by_iterator);
        fetch_coll(con, "products_nested_table", print_const_coll_by_index);
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    Environment::Cleanup();

    return EXIT_SUCCESS;
}
