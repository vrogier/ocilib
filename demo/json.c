#include "ocilib.h"

/* JSON DOM demo : requires Oracle client library 21c or higher */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

/* recursively dump a JSON DOM tree */

void print_node(OCI_JsonNode *node, OCI_Int32 depth)
{
    OCI_UInt32 i, n;

    switch (OCI_JsonNodeGetType(node))
    {
        case OCI_JNT_OBJECT:
        {
            printf("{\n");
            n = OCI_JsonNodeGetFieldCount(node);
            for (i = 0; i < n; i++)
            {
                OCI_JsonNode *field = OCI_JsonNodeGetFieldByIndex(node, i);

                printf("%*s\"%s\": ", depth + 2, "", OCI_JsonNodeGetFieldName(node, i));
                print_node(field, depth + 2);
                OCI_JsonNodeFree(field);
            }
            printf("%*s}\n", depth, "");
            break;
        }
        case OCI_JNT_ARRAY:
        {
            printf("[\n");
            n = OCI_JsonNodeGetItemCount(node);
            for (i = 0; i < n; i++)
            {
                OCI_JsonNode *item = OCI_JsonNodeGetItem(node, i);

                printf("%*s", depth + 2, "");
                print_node(item, depth + 2);
                OCI_JsonNodeFree(item);
            }
            printf("%*s]\n", depth, "");
            break;
        }
        case OCI_JNT_STRING:
        {
            printf("\"%s\"\n", OCI_JsonNodeGetString(node, NULL));
            break;
        }
        case OCI_JNT_NULL:
        {
            printf("null\n");
            break;
        }
        default:
        {
            printf("<type %u>\n", OCI_JsonNodeGetType(node));
            break;
        }
    }
}

int main(void)
{
    OCI_Connection  *cn;
    OCI_Json        *json;
    OCI_JsonNode    *root, *emp, *skills, *item;
    otext           *text;
    OCI_UInt32     len;

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate(env, "db", "usr", "pwd", OCI_SESSION_DEFAULT);

    /* parse a JSON text into a descriptor */

    json = OCI_JsonCreate(cn);

    OCI_JsonLoadText(json,
                     "{\"employee\":{\"name\":\"Fred\",\"skills\":[\"sql\",\"c\"]}}",
                     0, OCI_JV_ALLOW_NONE, OCI_JE_UTF8);

    /* navigate the DOM tree */

    root   = OCI_JsonGetRoot(json);
    emp    = OCI_JsonNodeGetFieldByName(root, "employee");
    skills = OCI_JsonNodeGetFieldByName(emp, "skills");

    item = OCI_JsonNodeGetFieldByName(emp, "name");
    printf("employee.name = %s\n", OCI_JsonNodeGetString(item, NULL));
    OCI_JsonNodeFree(item);

    /* mutate the DOM : append an item to the skills array */

    OCI_JsonSetMutable(json, TRUE);

    item = OCI_JsonNodeCreateString(json, "plsql", 0);
    OCI_JsonNodeAppendItem(skills, item);
    OCI_JsonNodeFree(item);

    OCI_JsonNodeFree(skills);
    OCI_JsonNodeFree(emp);
    OCI_JsonNodeFree(root);

    /* dump the resulting DOM */

    root = OCI_JsonGetRoot(json);
    print_node(root, 0);
    OCI_JsonNodeFree(root);

    /* serialize the document back to text */

    OCI_JsonToText(json, NULL, &len, 0, TRUE);
    text = (otext *) malloc((len + 1) * sizeof(otext));

    len++;
    OCI_JsonToText(json, text, &len, 0, TRUE);

    printf("serialized: %s\n", text);
    free(text);

    OCI_JsonFree(json);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
