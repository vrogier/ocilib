#include "ocilib.hpp"

using namespace ocilib;

int main(void)
{
    try
    {
        Environment::Initialize();

        /* start remote instance */
        Environment::StartDatabase("db", "sys_usr", "sys_pwd", Environment::StartForce, Environment::StartFull);

        /* shutdown remote instance */
        Environment::ShutdownDatabase("db", "sys_usr", "sys_pwd", Environment::ShutdownAbort, Environment::ShutdownFull);
    }
    catch(Exception &ex)
    {
         std::cout << ex.GetMessage() << std::endl;
    }

    Environment::Cleanup();
 
    return EXIT_SUCCESS;
}
