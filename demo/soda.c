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
    OCI_SodaDb     *sdb = OCI_SodaDbCreate(cn);

    /* create a collection backed by a new table (default metadata) */

    OCI_SodaColl *coll = OCI_SodaDbCreateColl(sdb, "soda_employees", NULL);

    printf("collection '%s' created\n", OCI_SodaCollGetName(coll));
    printf("metadata: %s\n", OCI_SodaCollGetMetadata(coll));

    /* insert documents - keys are server-assigned (UUID) by default */

    OCI_SodaDoc *doc = OCI_SodaDocCreateFromText(sdb, "{\"name\":\"Fred\",\"age\":42}",
                                                 (OCI_UInt32) strlen("{\"name\":\"Fred\",\"age\":42}"), NULL);
    OCI_SodaCollInsertDoc(coll, doc);

    printf("inserted doc key='%s' version='%s' created='%s'\n",
           OCI_SodaDocGetKey(doc), OCI_SodaDocGetVersion(doc), OCI_SodaDocGetCreatedOn(doc));

    OCI_SodaDocFree(doc);

    doc = OCI_SodaDocCreateFromText(sdb, "{\"name\":\"Jane\",\"age\":35}",
                                    (OCI_UInt32) strlen("{\"name\":\"Jane\",\"age\":35}"), NULL);
    OCI_SodaCollInsertDoc(coll, doc);
    OCI_SodaDocFree(doc);

    /* fetch all documents through a cursor */

    OCI_SodaDocCursor *cur = OCI_SodaCollFind(coll, NULL);

    while ((doc = OCI_SodaDocCursorGetNext(cur)) != NULL)
    {
        printf("doc key='%s' content=%s\n", OCI_SodaDocGetKey(doc), OCI_SodaDocGetText(doc));
        OCI_SodaDocFree(doc);
    }

    OCI_SodaDocCursorFree(cur);

    /* query by example : count documents matching a JSON filter spec */

    OCI_SodaOptions *opt = OCI_SodaOptionsCreate(sdb);
    OCI_SodaOptionsSetFilter(opt, "{\"name\":\"Jane\"}");

    printf("%llu doc(s) match QBE '{\"name\":\"Jane\"}'\n",
           (unsigned long long) OCI_SodaCollDocCount(coll, opt));

    OCI_SodaOptionsFree(opt);

    /* drop the collection and its backing table */

    OCI_Commit(cn);
    OCI_SodaCollDrop(coll);
    OCI_SodaCollFree(coll);

    OCI_SodaDbFree(sdb);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
