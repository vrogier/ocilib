#include "ocilib.h"

/* SODA demo : requires Oracle client library 18c or higher and Oracle Database 18c or higher */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    const char *db  = getenv("OCILIB_DEMO_DB");
    const char *usr = getenv("OCILIB_DEMO_USR");
    const char *pwd = getenv("OCILIB_DEMO_PWD");

    OCI_Connection *cn  = OCI_ConnectionCreate(env, db, usr, pwd, OCI_SESSION_DEFAULT);
    OCI_SodaDatabase     *sdb = OCI_SodaDatabaseCreate(cn);

    /* create a collection backed by a new table (default metadata) */

    OCI_SodaCollection *coll = OCI_SodaDatabaseCreateCollection(sdb, "soda_employees", NULL);

    printf("collection '%s' created\n", OCI_SodaCollectionGetName(coll));
    printf("metadata: %s\n", OCI_SodaCollectionGetMetadata(coll));

    /* insert documents - keys are server-assigned (UUID) by default */

    OCI_SodaDocument *doc = OCI_SodaDocumentCreateFromText(sdb, "{\"name\":\"Fred\",\"age\":42}", 0, NULL);
    OCI_SodaCollectionInsertDocument(coll, doc);

    printf("inserted doc key='%s' version='%s' created='%s'\n",
           OCI_SodaDocumentGetKey(doc), OCI_SodaDocumentGetVersion(doc), OCI_SodaDocumentGetCreatedOn(doc));

    OCI_SodaDocumentFree(doc);

    doc = OCI_SodaDocumentCreateFromText(sdb, "{\"name\":\"Jane\",\"age\":35}", 0, NULL);
    OCI_SodaCollectionInsertDocument(coll, doc);
    OCI_SodaDocumentFree(doc);

    /* fetch all documents through a cursor */

    OCI_SodaDocumentCursor *cur = OCI_SodaCollectionFind(coll, NULL);

    while ((doc = OCI_SodaDocumentCursorGetNext(cur)) != NULL)
    {
        printf("doc key='%s' content=%s\n", OCI_SodaDocumentGetKey(doc), OCI_SodaDocumentGetText(doc));
        OCI_SodaDocumentFree(doc);
    }

    OCI_SodaDocumentCursorFree(cur);

    /* query by example : count documents matching a JSON filter spec */

    OCI_SodaOptions *opt = OCI_SodaOptionsCreate(sdb);
    OCI_SodaOptionsSetFilter(opt, "{\"name\":\"Jane\"}");

    printf("%llu doc(s) match QBE '{\"name\":\"Jane\"}'\n",
           (unsigned long long) OCI_SodaCollectionDocumentCount(coll, opt));

    OCI_SodaOptionsFree(opt);

    /* drop the collection and its backing table */

    OCI_ConnectionCommit(cn);
    OCI_SodaCollectionDrop(coll);
    OCI_SodaCollectionFree(coll);

    OCI_SodaDatabaseFree(sdb);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
