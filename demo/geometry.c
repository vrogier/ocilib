#include "ocilib.h"

/* requires script demo/geometry.sql */

#define NB_ELEM 500

void error(OCI_Error *err)
{
    printf("msg   : %s\n", OCI_ErrorGetString(err));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    OCI_Connection *cn;
    OCI_Statement  *st;
    OCI_Object     *obj_sdo;
    OCI_Coll       *coll_inf, *coll_ord;
    OCI_Elem       *elem_inf, *elem_ord;
    OCI_TypeInfo   *tif_inf, *tif_ord, *tif_sdo;

    int i;

    /* check command line */
    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s database user password\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* initialize OCILIB */
    if (!OCI_Initialize(error, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }
    
    /* connect to oracle */
    cn = OCI_ConnectionCreate(argv[1], argv[2], argv[3], OCI_SESSION_DEFAULT);
    
    if (!cn)
    {
        return EXIT_FAILURE;
    }

    printf("\nConnected to Oracle.\n");

    /*create statement object */
    st = OCI_StatementCreate(cn);

    /* retrieve type info */
    tif_sdo = OCI_TypeInfoGet(cn, "MDSYS.SDO_GEOMETRY", OCI_TIF_TYPE);
    tif_inf = OCI_TypeInfoGet(cn, "MDSYS.SDO_ELEM_INFO_ARRAY", OCI_TIF_TYPE);
    tif_ord = OCI_TypeInfoGet(cn, "MDSYS.SDO_ORDINATE_ARRAY", OCI_TIF_TYPE);

    /* create sdo object */
    obj_sdo = OCI_ObjectCreate(cn, tif_sdo);

    /* create sub arrays */
    coll_inf = OCI_CollCreate(tif_inf);
    coll_ord = OCI_CollCreate(tif_ord);

    /* create sub array element accessors */
    elem_inf = OCI_ElemCreate(tif_inf);
    elem_ord = OCI_ElemCreate(tif_ord);

    /* build ordinates collection with test values */
    for (i = 0; i < NB_ELEM; i++)
    {
        OCI_ElemSetDouble(elem_ord, (double)i);
        OCI_CollAppend(coll_ord, elem_ord);
        OCI_CollAppend(coll_ord, elem_ord);
    }

    /* setup information collection attribute 'starting_offset' */
    OCI_ElemSetUnsignedInt(elem_inf, 1);
    OCI_CollAppend(coll_inf, elem_inf);

    /* setup information collection attribute 'element_type' */
    OCI_ElemSetUnsignedInt(elem_inf, 1);
    OCI_CollAppend(coll_inf, elem_inf);

    /* setup information collection attribute 'interpretation' */
    OCI_ElemSetUnsignedInt(elem_inf, 1);
    OCI_CollAppend(coll_inf, elem_inf);

    /* set sdo object member attributes */
    OCI_ObjectSetInt(obj_sdo, "SDO_GTYPE", 4);
    OCI_ObjectSetNull(obj_sdo, "SDO_SRID");
    OCI_ObjectSetNull(obj_sdo, "SDO_POINT");
    OCI_ObjectSetColl(obj_sdo, "SDO_ELEM_INFO", coll_inf);
    OCI_ObjectSetColl(obj_sdo, "SDO_ORDINATES", coll_ord);

    /* prepare, bind and excute statement then commit*/
    OCI_Prepare(st, "INSERT INTO geometry (code, obj) VALUES (1, :sdo)");
    OCI_BindObject(st, "sdo", obj_sdo);
    OCI_Execute(st);
    OCI_Commit(cn);

    printf("\n%d row inserted.\n", OCI_GetAffectedRows(st));

    /* free local objects */
    OCI_ObjectFree(obj_sdo);
    OCI_CollFree(coll_inf);
    OCI_CollFree(coll_ord);
    OCI_ElemFree(elem_inf);
    OCI_ElemFree(elem_ord);

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);

    /* cleanup OCILIB */
    OCI_Cleanup();

    printf("\nDisconnected from Oracle.\n");

    return EXIT_SUCCESS;
}