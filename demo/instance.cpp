#include "ocilib.hpp"

using namespace ocilib;

int main(void)
{
    try
    {
        Environment::Initialize();

        /* start remote instance */
        Environment::StartDatabase("db", "sys_usr", "sys_pwd", OCI_DB_SPM_FULL, OCI_DB_SPF_FORCE);

        /* shutdown remote instance */
        Environment::ShutdownDatabase("db", "sys_usr", "sys_pwd",  OCI_DB_SDM_FULL, OCI_DB_SDF_ABORT);
    }
    catch(Exception &ex)
    {
         std::cout << ex.GetMessage() << std::endl;
    }

    Environment::Cleanup();
 
    return EXIT_SUCCESS;
}
