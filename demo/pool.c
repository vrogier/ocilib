#include "ocilib.h"
#include <threads.h>

#define MAX_THREADS 50
#define MAX_CONN    10
#define SIZE_STR   260

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int pool_worker(void *data)
{
    OCI_Connection *cn = OCI_PoolGetConnection(OCI_HandleGetEnvironment(data), data, NULL);
    char str[SIZE_STR + 1] = "";

    OCI_Immediate(cn, "select to_char(sysdate, 'YYYYMMDD HH24:MI:SS') from dual", OCI_ARG_TEXT, str);

    printf("%s\n", str);

    OCI_ConnectionFree(cn);

    return 0;
}

int main(void)
{
    thrd_t th[MAX_THREADS];
    OCI_Pool *pool;

    OCI_Int32 i;

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT | OCI_ENV_THREADED);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    /* create pool */

    const char *db  = getenv("OCILIB_DEMO_DB");
    const char *usr = getenv("OCILIB_DEMO_USR");
    const char *pwd = getenv("OCILIB_DEMO_PWD");

    pool = OCI_PoolCreate(env, db, usr, pwd, OCI_POOL_SESSION, OCI_SESSION_DEFAULT, 0, MAX_CONN, 1);

    /* create threads */

    for (i = 0; i < MAX_THREADS; i++)
    {
        thrd_create(&th[i], pool_worker, pool);
    }

    /* wait for threads and cleanup */

    for (i = 0; i < MAX_THREADS; i++)
    {
        thrd_join(th[i], NULL);
    }

    OCI_PoolFree(env, pool);

    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
