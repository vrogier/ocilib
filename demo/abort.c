#include "ocilib.h"

#if defined(_WINDOWS)
    #define sleep(x) Sleep(x*1000)
#else
    #include <unistd.h>
#endif

void long_oracle_call(OCI_Thread *thread, void *data)
{
    clock_t t_start = clock();
    int err_code = 0;

    if (!OCI_ExecuteStmt((OCI_Statement *)data, "begin dbms_lock.sleep(10); end;"))
    {
        OCI_Error *err = OCI_GetLastError();
        if (err)
        {
            err_code = OCI_ErrorGetOCICode(err);
        }
    }
    
    printf("call duration %d, expected oracle error is 1013, got %d", clock() - t_start, err_code);
}


int main(void)
{
    OCI_Connection *cn;
    OCI_Statement *st;
    OCI_Thread *th;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_THREADED | OCI_ENV_CONTEXT))
    {
        return EXIT_FAILURE;
    }

    th = OCI_ThreadCreate();
    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    OCI_ThreadRun(th, long_oracle_call, st);

    sleep(1);
    OCI_Break(cn);

    OCI_ThreadJoin(th);
    OCI_ThreadFree(th);

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
