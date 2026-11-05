#include "ocilib.hpp"

#include <iostream>
#include <cstdlib>

using namespace ocilib;

int main()
{
    try
    {
        Environment env;

        const char *db  = std::getenv("OCILIB_DEMO_DB");
        const char *usr = std::getenv("OCILIB_DEMO_USR");
        const char *pwd = std::getenv("OCILIB_DEMO_PWD");

        /* build a composite sharding key: NUMBER column + VARCHAR2 column */

        Number cust_id(env, 42);

        ShardingKey skey(env);
        skey.AddColumn(cust_id);
        skey.AddColumn(String(OTEXT("GOLD")));

        /* build a super sharding key (shard group) */

        ShardingKey sskey(env);
        sskey.AddColumn(String(OTEXT("EUROPE")));

        std::cout << "sharding key (base64)       : " << skey.ToString() << std::endl;
        std::cout << "super sharding key (base64) : " << sskey.ToString() << std::endl;

        /* on a non-sharded database the keys are accepted and a regular
           connection is established */

        Connection con(env, db ? db : OTEXT(""), usr ? usr : OTEXT(""), pwd ? pwd : OTEXT(""),
                       Environment::SessionDefault, skey, sskey);

        Statement st(con);
        st.Execute(OTEXT("select cust_id, name, region from customers where rownum <= 5"));

        Resultset rs = st.GetResultset();
        for (auto&& row : rs)
        {
            std::cout << "cust_id=" << row.Get<String>(1)
                      << " name=" << row.Get<String>(2)
                      << " region=" << row.Get<String>(3) << std::endl;
        }
    }
    catch (const Exception& ex)
    {
        std::cerr << ex.GetMessage() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
