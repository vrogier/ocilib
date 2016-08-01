#include "ocilib.h"

/* requires script demo/object.sql */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement *st;
    OCI_Resultset *rs;
    OCI_Object *sale, *vendor;
    OCI_Date *date;

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);
    date = OCI_DateCreate(NULL);

    OCI_DateSysDate(date);

    vendor = OCI_ObjectCreate(cn, OCI_TypeInfoGet(cn, "t_vendor", OCI_TIF_TYPE));
    OCI_ObjectSetInt(vendor, "CODE", 134);
    OCI_ObjectSetString(vendor, "NAME", "JOHN SMITH");

    sale = OCI_ObjectCreate(cn, OCI_TypeInfoGet(cn, "t_sale", OCI_TIF_TYPE));

    OCI_ObjectSetInt(sale, "CODE", 1);
    OCI_ObjectSetDouble(sale, "PRICE", 12.99);
    OCI_ObjectSetString(sale, "NAME", "USB KEY 2go");
    OCI_ObjectSetString(sale, "REF", "A56547WSAA");
    OCI_ObjectSetObject(sale, "VENDOR", vendor);
    OCI_ObjectSetDate(sale, "DATE_SALE", date);

    OCI_Prepare(st, "insert into sales values(:sale)");
    OCI_BindObject(st, ":sale", sale);
    OCI_Execute(st);

    printf("\n%d row(s) inserted\n", OCI_GetAffectedRows(st));

    OCI_ObjectFree(sale);
    OCI_ObjectFree(vendor);

    OCI_ExecuteStmt(st, "select * from sales");
    rs = OCI_GetResultset(st);
    while (OCI_FetchNext(rs))
    {
        sale = OCI_GetObject(rs, 1);
        vendor = OCI_ObjectGetObject(sale, "VENDOR");

        printf("Sale %d - %s vendor %s\n", OCI_ObjectGetInt(sale, "CODE"), OCI_ObjectGetString(sale, "NAME"), OCI_ObjectGetString(vendor, "NAME"));
    }

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
