#include "ocilib.h"

/* 
    DML for the test 

    create type t_vendor as object
    ( 
        code  number, 
        name  varchar2(30)
    ); 

    create type t_sale as object 
    ( 
        code  number, 
        price  float, 
        name  varchar2(30),
        ref  varchar2(30), 
        date_sale date, 
        vendor  t_vendor
    ); 

    create table sales(item t_sale);

*/

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement *st;
    OCI_Object *obj, *obj2;
    OCI_Date *date;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    obj  = OCI_ObjectCreate(cn, OCI_TypeInfoGet(cn, "t_sale", OCI_TIF_TYPE));

    OCI_ObjectSetInt(obj, "CODE", 1);
    OCI_ObjectSetDouble(obj, "PRICE", 12.99);
    OCI_ObjectSetString(obj, "NAME", "USB KEY 2go");
    OCI_ObjectSetString(obj, "REF", "A56547WSAA");

    date = OCI_ObjectGetDate(obj, "DATE_SALE");
    OCI_DateSysDate(date);

    obj2 = OCI_ObjectGetObject(obj, "VENDOR");
    OCI_ObjectSetInt(obj2, "CODE", 134);
    OCI_ObjectSetString(obj2, "NAME", "JOHN SMITH");

    OCI_Prepare(st, "insert into sales values(:obj)");
    OCI_BindObject(st, ":obj", obj);
    OCI_Execute(st);

    printf("\n%d row(s) inserted\n", OCI_GetAffectedRows(st));

    OCI_Commit(cn);

    OCI_ObjectFree(obj);

    OCI_Cleanup();

    return EXIT_SUCCESS;
}
