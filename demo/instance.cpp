#include <iostream>

#include "ocilib.hpp"

using namespace ocilib;

int main()
{
    try
    {
        Environment env;

        /* start remote instance */
        env.StartDatabase("db", "sys_usr", "sys_pwd", Environment::StartForce, Environment::StartFull);

        /* shutdown remote instance */
        env.ShutdownDatabase("db", "sys_usr", "sys_pwd", Environment::ShutdownAbort, Environment::ShutdownFull);
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
