#include "ocilib.h"

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement *st;
    OCI_Resultset *rs;
    OCI_Coll *coll;
    OCI_Iter *iter;
    OCI_Elem *elem;
    OCI_TypeInfo *type;
    OCI_Object *obj;
    int i, n;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
       return EXIT_FAILURE;

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);


    /* Varray binding -------------------------------------------------------- */

    st = OCI_StatementCreate(cn);

    /* create the collection */

    type = OCI_TypeInfoGet(cn, "Varray_type", OCI_TIF_TYPE);
    coll = OCI_CollCreate(type);

    /* bind the local collection to a PL/SQL procedure */
    
    OCI_Prepare(st, "begin load_array(:array); end;");
    OCI_BindColl(st, ":array", coll);
    OCI_Execute(st);
 
    /* the procedure has filled the collection and 
       we can iterate it using an iterator */
    
    iter = OCI_IterCreate(coll);
    elem = OCI_IterGetNext(iter);

    while (elem != NULL)
    {
        printf("value %s\n", OCI_ElemGetString(elem));
        elem = OCI_IterGetNext(iter);
    }

    OCI_IterFree(iter);
    OCI_CollFree(coll);
 
    /* Varray SQL fetch ------------------------------------------------------- */
  
    /* query on a table with varray column */
 
    OCI_ExecuteStmt(st, "SELECT * from table_article");

    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
    {
        /* iterate the collection using an iterator */

        coll = OCI_GetColl(rs, 2);

        iter = OCI_IterCreate(coll);
        elem = OCI_IterGetNext(iter);

        printf("article #%d\n", OCI_GetInt(rs, 1));

        while (elem != NULL)
        {
            obj = OCI_ElemGetObject(elem);
            printf(".... code %d, name%s \n", OCI_ObjectGetInt(obj, "ID"),
                                              OCI_ObjectGetString(obj, "NAME"));
            elem = OCI_IterGetNext(iter);
        }

        OCI_IterFree(iter);
    }

    /* Nested table fetch ------------------------------------------------------- */
    
    /* query on a table with nested table column */
 
    OCI_ExecuteStmt(st, "SELECT * from table_sales");

    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
    {
        coll = OCI_GetColl(rs, 2);

        printf("Sale #%d\n", OCI_GetInt(rs, 1));

        /* iterate the collection by accessing element by index */
   
        n = OCI_CollGetSize(coll);

        for(i = 1; i <= n; i++)
        {
            elem = OCI_CollGetAt(coll, i);
            obj  = OCI_ElemGetObject(elem);

            printf(".... employee %s, amount %s \n", OCI_ObjectGetString(obj, "EMP"),
                                                     OCI_ObjectGetString(obj, "AMOUNT"));
        }
    }

    OCI_Cleanup();
    
    return EXIT_SUCCESS;
}
