#include <iostream>

#include "ocilib.hpp"

/* SODA demo : requires Oracle client library 18c or higher and Oracle Database 18c or higher */

using namespace ocilib;

int main()
{
    try
    {
        Environment env;

        Connection con(env, "db", "usr", "pwd");

        SodaDb sdb(con);

        /* create a collection backed by a new table (default metadata) */

        SodaColl coll = sdb.CreateCollection("soda_employees");

        std::cout << "collection '" << coll.GetName() << "' created" << std::endl;
        std::cout << "metadata: " << coll.GetMetadata() << std::endl;

        /* insert documents - keys are server-assigned (UUID) by default */

        SodaDoc doc = sdb.CreateDocumentFromText("{\"name\":\"Fred\",\"age\":42}");
        coll.Insert(doc);

        std::cout << "inserted doc key='" << doc.GetKey() << "' version='" << doc.GetVersion()
                  << "' created='" << doc.GetCreatedOn() << "'" << std::endl;

        doc = sdb.CreateDocumentFromText("{\"name\":\"Jane\",\"age\":35}");
        coll.Insert(doc);

        /* fetch all documents through a cursor */

        SodaDocCursor cur = coll.Find();

        while ((doc = cur.GetNext()))
        {
            std::cout << "doc key='" << doc.GetKey() << "' content=" << doc.GetText() << std::endl;
        }

        /* query by example : count documents matching a JSON filter spec */

        SodaOptions opt(sdb);
        opt.SetFilter("{\"name\":\"Jane\"}");

        std::cout << coll.GetCount(opt) << " doc(s) match QBE '{\"name\":\"Jane\"}'" << std::endl;

        /* drop the collection and its backing table */

        coll.Drop();
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
