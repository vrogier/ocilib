#include "ocilib.h"

int main(void)
{ 
    OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT);

    /* start remote instance */

    OCI_DatabaseStartup("db", 
                        "sys_usr", 
                        "sys_pwd", 
                        OCI_SESSION_SYSDBA, 
                        OCI_DB_SPM_FULL,
                        OCI_DB_SPF_FORCE,
                        NULL);


     /* shutdown remote instance */

    OCI_DatabaseShutdown("db", 
                         "sys_usr", 
                         "sys_pwd",  
                         OCI_SESSION_SYSDBA,
                         OCI_DB_SDM_FULL,
                         OCI_DB_SDF_ABORT);
    OCI_Cleanup();
    
    return EXIT_SUCCESS;
}   

