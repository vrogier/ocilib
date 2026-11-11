#include "ocilib.h"

/* requires script demo/reference.sql */

#define SIZE_STR 100

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

void dump_ref(OCI_Reference *ref)
{
    char data[SIZE_STR + 1];

    /* print ref hexadecimal value */

    OCI_ReferenceToText(ref, SIZE_STR, data);
    printf("...Ref Hex value : %s\n", data);

    /* get object from ref */

    OCI_Object * vendor = OCI_ReferenceGetObject(ref);

    /* print object values */

    printf("...%i - %s\n", OCI_ObjectGetInt32(vendor, "code"), OCI_ObjectGetString(vendor, "name"));
}

int main(void)
{
    OCI_Reference       *ref;

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    const char *db  = getenv("OCILIB_DEMO_DB");
    const char *usr = getenv("OCILIB_DEMO_USR");
    const char *pwd = getenv("OCILIB_DEMO_PWD");

    OCI_Connection * cn = OCI_ConnectionCreate(env, db, usr, pwd, OCI_SESSION_DEFAULT);
    OCI_Statement  * st = OCI_StatementCreate(cn);

    OCI_StatementExecute(st, "select ref(v) from vendors v");
    OCI_Resultset  * rs = OCI_StatementGetResultset(st);

    printf("\n\n=> fetch refs from object table\n\n");

    while (OCI_ResultsetFetchNext(rs))
    {
        dump_ref(OCI_ResultsetGetReference(rs, 0));
    }

    printf("\n\n=> bind a local ref object to a PL/SQL statement\n\n");

    ref = OCI_ReferenceCreate(OCI_TypeInfoGet(cn, "vendor_t", OCI_TIF_TYPE));

    OCI_StatementPrepare(st, "begin select ref(v) into :vendor from vendors v where v.code = 1; end; ");

    OCI_StatementBindReference(st, ":vendor", ref);
    OCI_StatementExecutePrepared(st);

    dump_ref(ref);

    OCI_ReferenceFree(ref);
    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
