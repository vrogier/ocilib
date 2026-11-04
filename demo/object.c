#include "ocilib.h"

/* requires script demo/object.sql */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Object *sale, *vendor;
    OCI_Date *date;

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    const char *db  = getenv("OCILIB_DEMO_DB");
    const char *usr = getenv("OCILIB_DEMO_USR");
    const char *pwd = getenv("OCILIB_DEMO_PWD");

    OCI_Connection * cn = OCI_ConnectionCreate(env, db, usr, pwd, OCI_SESSION_DEFAULT);
    OCI_Statement * st = OCI_StatementCreate(cn);
    date = OCI_DateCreate(env);

    OCI_DateSysDate(date);

    vendor = OCI_ObjectCreate(OCI_TypeInfoGet(cn, "t_vendor", OCI_TIF_TYPE));
    OCI_ObjectSetInt(vendor, "CODE", 134);
    OCI_ObjectSetString(vendor, "NAME", "JOHN SMITH");

    sale = OCI_ObjectCreate(OCI_TypeInfoGet(cn, "t_sale", OCI_TIF_TYPE));

    OCI_ObjectSetInt(sale, "CODE", 1);
    OCI_ObjectSetDouble(sale, "PRICE", 12.99);
    OCI_ObjectSetString(sale, "NAME", "USB KEY 2go");
    OCI_ObjectSetString(sale, "REF", "A56547WSAA");
    OCI_ObjectSetObject(sale, "VENDOR", vendor);
    OCI_ObjectSetDate(sale, "DATE_SALE", date);
    OCI_DateFree(date);

    OCI_Prepare(st, "insert into sales values(:sale)");
    OCI_BindObject(st, ":sale", sale);
    OCI_Execute(st);

    printf("\n%d row(s) inserted\n", OCI_GetAffectedRows(st));

    OCI_ObjectFree(sale);
    OCI_ObjectFree(vendor);

    OCI_ExecuteStmt(st, "select * from sales");
    OCI_Resultset * rs = OCI_GetResultset(st);
    while (OCI_FetchNext(rs))
    {
        sale = OCI_GetObject(rs, 1);
        vendor = OCI_ObjectGetObject(sale, "VENDOR");

        printf("Sale %d - %s vendor %s\n", OCI_ObjectGetInt(sale, "CODE"), OCI_ObjectGetString(sale, "NAME"), OCI_ObjectGetString(vendor, "NAME"));
    }

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
