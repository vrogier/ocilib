#include "ocilib.h"
#include <threads.h>

#if defined(_WINDOWS)
    #define sleep(x) Sleep(x*1000)
#else
    #include <unistd.h>
#endif

int long_oracle_call(void *data)
{
    clock_t t_start = clock();
    OCI_Int32 err_code = 0;

    if (!OCI_StatementExecute((OCI_Statement *)data, "begin dbms_lock.sleep(10); end;"))
    {
        OCI_Error *err = OCI_GetLastError();
        if (err)
        {
            err_code = OCI_ErrorGetCode(err);
        }
    }
    
    printf("call duration %ld, expected oracle error is 1013, got %d", (long)(clock() - t_start), err_code);

    return 0;
}


int main(void)
{
    thrd_t th;

    OCI_Environment *env = OCI_EnvironmentCreate(NULL, NULL, OCI_ENV_THREADED);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    const char *db  = getenv("OCILIB_DEMO_DB");
    const char *usr = getenv("OCILIB_DEMO_USR");
    const char *pwd = getenv("OCILIB_DEMO_PWD");

    OCI_Connection * cn = OCI_ConnectionCreate(env, db, usr, pwd, OCI_SESSION_DEFAULT);
    OCI_Statement * st = OCI_StatementCreate(cn);

    thrd_create(&th, long_oracle_call, st);

    sleep(1);
    OCI_ConnectionBreak(cn);

    thrd_join(th, NULL);

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
