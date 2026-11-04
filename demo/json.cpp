#include <iostream>

#include "ocilib.hpp"

/* JSON DOM demo : requires Oracle client library 21c or higher */

using namespace ocilib;

/* recursively dump a JSON DOM tree */

void PrintNode(const JsonNode& node, OCI_Int32 depth)
{
    const std::string indent(depth, ' ');

    switch (node.GetType())
    {
        case Json::NodeObject:
        {
            std::cout << "{\n";

            const OCI_UInt32 n = node.GetFieldCount();
            for (OCI_UInt32 i = 0; i < n; ++i)
            {
                std::cout << indent << "  \"" << node.GetFieldName(i + 1) << "\": ";
                PrintNode(node.GetFieldByIndex(i + 1), depth + 2);
            }

            std::cout << indent << "}\n";
            break;
        }
        case Json::NodeArray:
        {
            std::cout << "[\n";

            const OCI_UInt32 n = node.GetItemCount();
            for (OCI_UInt32 i = 0; i < n; ++i)
            {
                std::cout << indent << "  ";
                PrintNode(node.GetItem(i + 1), depth + 2);
            }

            std::cout << indent << "]\n";
            break;
        }
        case Json::NodeString:
        {
            std::cout << "\"" << node.GetString() << "\"\n";
            break;
        }
        case Json::NodeNull:
        {
            std::cout << "null\n";
            break;
        }
        default:
        {
            std::cout << "<type " << node.GetType() << ">\n";
            break;
        }
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

        /* parse a JSON text into a descriptor */

        Json json(con);
        json.LoadText("{\"employee\":{\"name\":\"Fred\",\"skills\":[\"sql\",\"c\"]}}");
        json.SetMutable(true);

        /* navigate the DOM tree */

        JsonNode root   = json.GetRoot();
        JsonNode emp    = root.GetFieldByName("employee");
        JsonNode skills = emp.GetFieldByName("skills");

        std::cout << "employee.name = " << emp.GetFieldByName("name").GetString() << std::endl;

        /* mutate the DOM : append an item to the skills array */

        skills.AppendItem(json.CreateString("plsql"));

        /* dump the resulting DOM */

        PrintNode(json.GetRoot(), 0);

        /* serialize the document back to text */

        std::cout << "serialized: " << json.ToText() << std::endl;
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
