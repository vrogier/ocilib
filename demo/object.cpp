#include <iostream>

#include "ocilib.hpp"

/* requires script demo/object.sql */

using namespace ocilib;

int main(void)
{
    try
    {
        Environment::Initialize();

        Connection con("db", "usr", "pwd");

        Date date;
        date.SysDate();

        Object sale(TypeInfo(con, "t_sale", TypeInfo::Type));
        Object vendor(TypeInfo(con, "t_vendor", TypeInfo::Type));

        vendor.Set<int>("code", 134);
        vendor.Set<ostring>("name", "JOHN SMITH");

        sale.Set<int>("code", 1);
        sale.Set<double>("price", 12.99);
        sale.Set<ostring>("name", "USB KEY 2go");
        sale.Set<ostring>("ref", "A56547WSAA");
        sale.Set<Date>("date_sale", date);
        sale.Set<Object>("vendor", vendor);

        Statement st(con);
        st.Prepare("insert into sales values(:obj)");
        st.Bind(":obj", sale, BindInfo::In);
        st.ExecutePrepared();

        std::cout << "Rows inserted :  " << st.GetAffectedRows() << std::endl;

        con.Commit();
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    Environment::Cleanup();

    return EXIT_SUCCESS;
}
