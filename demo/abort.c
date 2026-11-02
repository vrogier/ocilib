#include "ocilib.h"

#if defined(_WINDOWS)
    #define sleep(x) Sleep(x*1000)
#else
    #include <unistd.h>
#endif

void long_oracle_call(OCI_Thread *thread, void *data)
{
    clock_t t_start = clock();
    OCI_Int32 err_code = 0;

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

    OCI_Environment *env = OCI_EnvironmentCreate(NULL, NULL, OCI_ENV_THREADED);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    th = OCI_ThreadCreate(env);
    cn = OCI_ConnectionCreate(env, "db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    OCI_ThreadRun(th, long_oracle_call, st);

    sleep(1);
    OCI_Break(cn);

    OCI_ThreadJoin(th);
    OCI_ThreadFree(th);

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
