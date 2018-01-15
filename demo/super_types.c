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
    OCI_Object *racing_car;
    OCI_TypeInfo *vehicule_type_inf;
    OCI_TypeInfo *car_type_inf;
    OCI_TypeInfo *racing_car_type_inf;

    char buffer[512];
    unsigned int size = sizeof(buffer) - 1;

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    racing_car = OCI_ObjectCreate(cn, OCI_TypeInfoGet(cn, "racing_car_type", OCI_TIF_TYPE));

    OCI_Prepare(st, "begin :obj := racing_car_type(1,'Formula1', 123456789, 300); end;");
    OCI_BindObject(st, "obj", racing_car);
    OCI_Execute(st);
    OCI_ObjectToText(racing_car, &size, buffer);
    printf("Object => %s\n", buffer);

    racing_car_type_inf = OCI_ObjectGetTypeInfo(racing_car);
    car_type_inf = OCI_TypeInfoGetSuperType(racing_car_type_inf);
    vehicule_type_inf = OCI_TypeInfoGetSuperType(car_type_inf);

    printf("Is type '%s' final => %d\n", OCI_TypeInfoGetName(racing_car_type_inf), OCI_TypeInfoIsFinalType(racing_car_type_inf));
    printf("Is type '%s' final => %d\n", OCI_TypeInfoGetName(car_type_inf), OCI_TypeInfoIsFinalType(car_type_inf));
    printf("Is type '%s' final => %d\n", OCI_TypeInfoGetName(vehicule_type_inf), OCI_TypeInfoIsFinalType(vehicule_type_inf));

    OCI_ObjectFree(racing_car);
    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
