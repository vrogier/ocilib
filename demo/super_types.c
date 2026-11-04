#include "ocilib.h"

/* requires script demo/super_types.sql */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{

    char buffer[512];
    OCI_UInt32 size = sizeof(buffer) - 1;

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

    OCI_Object * racing_car = OCI_ObjectCreate(OCI_TypeInfoGet(cn, "racing_car_type", OCI_TIF_TYPE));

    OCI_Prepare(st, "begin :obj := racing_car_type(1,'Formula1', 123456789, 300); end;");
    OCI_BindObject(st, "obj", racing_car);
    OCI_Execute(st);
    OCI_ObjectToText(racing_car, &size, buffer);
    printf("Object => %s\n", buffer);

    OCI_TypeInfo * racing_car_type_inf = OCI_ObjectGetTypeInfo(racing_car);
    OCI_TypeInfo * car_type_inf = OCI_TypeInfoGetSuperType(racing_car_type_inf);
    OCI_TypeInfo * vehicule_type_inf = OCI_TypeInfoGetSuperType(car_type_inf);

    printf("Is type '%s' final => %d\n", OCI_TypeInfoGetName(racing_car_type_inf), OCI_TypeInfoIsFinalType(racing_car_type_inf));
    printf("Is type '%s' final => %d\n", OCI_TypeInfoGetName(car_type_inf), OCI_TypeInfoIsFinalType(car_type_inf));
    printf("Is type '%s' final => %d\n", OCI_TypeInfoGetName(vehicule_type_inf), OCI_TypeInfoIsFinalType(vehicule_type_inf));

    OCI_ObjectFree(racing_car);
    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
