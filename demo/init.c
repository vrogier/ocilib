#include "ocilib.h"

int main(void)
{
    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    /* ... application code here ... */

    OCI_Cleanup();

 
    return EXIT_SUCCESS;
}
