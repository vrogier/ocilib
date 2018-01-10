#include "ocilib.h"

/* requires script demo/super_types.sql */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement *st;
    OCI_Object *obj;
    OCI_TypeInfo *base_type_inf;
    OCI_TypeInfo *derived_type_inf;
     
    char buffer[512];
    unsigned int size = sizeof(buffer)-1;
 
    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);
    
    obj = OCI_ObjectCreate(cn, OCI_TypeInfoGet(cn, "derived_type", OCI_TIF_TYPE));

    OCI_Prepare(st, "begin :obj := derived_type(1,'Car', 31245.99); end;");
    OCI_BindObject(st, "obj", obj);
    OCI_Execute(st);
    OCI_ObjectToText(obj, &size, buffer);
    printf("Object => %s\n", buffer);

    derived_type_inf = OCI_ObjectGetTypeInfo(obj);
    printf("Is derived type '%s' final => %d\n", OCI_TypeInfoGetName(derived_type_inf), OCI_TypeInfoIsFinalType(derived_type_inf));

    base_type_inf = OCI_TypeInfoGetSuperType(derived_type_inf);
    printf("Is base type '%s' final => %d\n", OCI_TypeInfoGetName(base_type_inf), OCI_TypeInfoIsFinalType(base_type_inf));

    OCI_ObjectFree(obj);

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
