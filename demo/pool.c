#include "ocilib.h"

#define MAX_THREADS 50
#define MAX_CONN    10
#define SIZE_STR   260

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

void pool_worker(OCI_Thread *thread, void *data)
{
    OCI_Connection *cn = OCI_PoolGetConnection(data, NULL);
    char str[SIZE_STR + 1] = "";

    OCI_Immediate(cn, "select to_char(sysdate, 'YYYYMMDD HH24:MI:SS') from dual", OCI_ARG_TEXT, str);

    printf("%s\n", str);

    OCI_ConnectionFree(cn);
}

int main(void)
{
    OCI_Thread *th[MAX_THREADS];
    OCI_ConnPool *pool;

    int i;

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT | OCI_ENV_THREADED))
    {
        return EXIT_FAILURE;
    }

    /* create pool */

    pool = OCI_PoolCreate("db", "usr", "pwd", OCI_POOL_SESSION, OCI_SESSION_DEFAULT, 0, MAX_CONN, 1);

    /* create threads */

    for (i = 0; i < MAX_THREADS; i++)
    {
        th[i] = OCI_ThreadCreate();
        OCI_ThreadRun(th[i], pool_worker, pool);
    }

    /* wait for threads and cleanup */

    for (i = 0; i < MAX_THREADS; i++)
    {
        OCI_ThreadJoin(th[i]);
        OCI_ThreadFree(th[i]);
    }

    OCI_PoolFree(pool);

    OCI_Cleanup();

    return EXIT_SUCCESS;
}
