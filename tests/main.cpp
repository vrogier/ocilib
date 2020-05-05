#include "ocilib_tests.h"


void ExecDML(ostring dml)
{
    OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT);
    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    OCI_Immediate(conn, dml.data());
    OCI_Commit(conn);
    OCI_Cleanup();
}   

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}