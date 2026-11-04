#include <iostream>

#include "ocilib.hpp"

/* requires script demo/object.sql */

using namespace ocilib;

int main()
{
    try
    {
        Environment env;

        const char *db  = getenv("OCILIB_DEMO_DB");
        const char *usr = getenv("OCILIB_DEMO_USR");
        const char *pwd = getenv("OCILIB_DEMO_PWD");

        Connection con(env, db, usr, pwd);

        Date date = Date::SysDate(env);

        Object sale(TypeInfo(con, "t_sale", TypeInfo::Type));
        Object vendor(TypeInfo(con, "t_vendor", TypeInfo::Type));

        vendor.Set<OCI_Int32>("code", 134);
        vendor.Set<ostring>("name", "JOHN SMITH");

        sale.Set<OCI_Int32>("code", 1);
        sale.Set<OCI_Double>("price", 12.99);
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
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
