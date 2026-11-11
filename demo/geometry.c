#include "ocilib.h"

/* requires script demo/geometry.sql */

#define NB_ELEM 500

void error(OCI_Error *err)
{
    printf("msg   : %s\n", OCI_ErrorGetString(err));
    exit(EXIT_FAILURE);
}

int main(void)
{
    OCI_Collection       *coll_inf, *coll_ord;
    OCI_Element       *elem_inf, *elem_ord;
    OCI_TypeInfo   *tif_inf, *tif_ord, *tif_sdo;

    OCI_Int32 i;

    /* initialize OCILIB */
    OCI_Environment *env = OCI_EnvironmentCreate(error, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    const char *db  = getenv("OCILIB_DEMO_DB");
    const char *usr = getenv("OCILIB_DEMO_USR");
    const char *pwd = getenv("OCILIB_DEMO_PWD");

    /* connect to oracle */
    OCI_Connection * cn = OCI_ConnectionCreate(env, db, usr, pwd, OCI_SESSION_DEFAULT);
    
    if (!cn)
    {
        return EXIT_FAILURE;
    }

    printf("\nConnected to Oracle.\n");

    /*create statement object */
    OCI_Statement  * st = OCI_StatementCreate(cn);

    /* retrieve type info */
    tif_sdo = OCI_TypeInfoGet(cn, "MDSYS.SDO_GEOMETRY", OCI_TIF_TYPE);
    tif_inf = OCI_TypeInfoGet(cn, "MDSYS.SDO_ELEM_INFO_ARRAY", OCI_TIF_TYPE);
    tif_ord = OCI_TypeInfoGet(cn, "MDSYS.SDO_ORDINATE_ARRAY", OCI_TIF_TYPE);

    /* create sdo object */
    OCI_Object     * obj_sdo = OCI_ObjectCreate(tif_sdo);

    /* create sub arrays */
    coll_inf = OCI_CollectionCreate(tif_inf);
    coll_ord = OCI_CollectionCreate(tif_ord);

    /* create sub array element accessors */
    elem_inf = OCI_ElementCreate(tif_inf);
    elem_ord = OCI_ElementCreate(tif_ord);

    /* build ordinates collection with test values */
    for (i = 0; i < NB_ELEM; i++)
    {
        OCI_ElementSetDouble(elem_ord, (OCI_Double)i);
        OCI_CollectionAppend(coll_ord, elem_ord);
        OCI_CollectionAppend(coll_ord, elem_ord);
    }

    /* setup information collection attribute 'starting_offset' */
    OCI_ElementSetUInt32(elem_inf, 1);
    OCI_CollectionAppend(coll_inf, elem_inf);

    /* setup information collection attribute 'element_type' */
    OCI_ElementSetUInt32(elem_inf, 1);
    OCI_CollectionAppend(coll_inf, elem_inf);

    /* setup information collection attribute 'interpretation' */
    OCI_ElementSetUInt32(elem_inf, 1);
    OCI_CollectionAppend(coll_inf, elem_inf);

    /* set sdo object member attributes */
    OCI_ObjectSetInt32(obj_sdo, "SDO_GTYPE", 4);
    OCI_ObjectSetNull(obj_sdo, "SDO_SRID");
    OCI_ObjectSetNull(obj_sdo, "SDO_POINT");
    OCI_ObjectSetColl(obj_sdo, "SDO_ELEM_INFO", coll_inf);
    OCI_ObjectSetColl(obj_sdo, "SDO_ORDINATES", coll_ord);

    /* prepare, bind and excute statement then commit*/
    OCI_StatementPrepare(st, "INSERT INTO geometry (code, obj) VALUES (1, :sdo)");
    OCI_StatementBindObject(st, "sdo", obj_sdo);
    OCI_StatementExecutePrepared(st);
    OCI_ConnectionCommit(cn);

    printf("\n%d row inserted.\n", OCI_StatementGetAffectedRows(st));

    /* free local objects */
    OCI_ObjectFree(obj_sdo);
    OCI_CollectionFree(coll_inf);
    OCI_CollectionFree(coll_ord);
    OCI_ElementFree(elem_inf);
    OCI_ElementFree(elem_ord);

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);

    /* cleanup OCILIB */
    OCI_EnvironmentFree(env);

    printf("\nDisconnected from Oracle.\n");

    return EXIT_SUCCESS;
}