#include "ocilib.h"

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{ 
    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    /* start remote instance */

    OCI_DatabaseStartup(env,
                        "db", 
                        "sys_usr", 
                        "sys_pwd", 
                        OCI_SESSION_SYSDBA, 
                        OCI_DB_SPM_FULL,
                        OCI_DB_SPF_FORCE,
                        NULL);


     /* shutdown remote instance */

    OCI_DatabaseShutdown(env,
                         "db", 
                         "sys_usr", 
                         "sys_pwd",  
                         OCI_SESSION_SYSDBA,
                         OCI_DB_SDM_FULL,
                         OCI_DB_SDF_ABORT);
    OCI_EnvironmentFree(env);
    
    return EXIT_SUCCESS;
}   

