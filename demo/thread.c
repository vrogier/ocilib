#include "ocilib.h"

#define MAX_THREADS 50

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

void key_cleanup(void *str)
{
    free(str);
}

void worker(OCI_Thread *thread, void *data)
{
    const void *id = thread;
    char *str = malloc(50);

    sprintf(str, "thread %p !\n", id);

    OCI_ThreadKeySetValue(OCI_HandleGetEnvironment(thread), "ID", str);

    str = OCI_ThreadKeyGetValue(OCI_HandleGetEnvironment(thread), "ID");

    printf(str);
}

int main(void)
{
    OCI_Thread *th[MAX_THREADS];
    OCI_Int32 i;

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT | OCI_ENV_THREADED);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    OCI_ThreadKeyCreate(env, "ID", key_cleanup);

    /* create threads */

    for (i = 0; i < MAX_THREADS; i++)
    {
        th[i] = OCI_ThreadCreate(env);
        OCI_ThreadRun(th[i], worker, NULL);
    }

    /* wait for threads and cleanup */

    for (i = 0; i < MAX_THREADS; i++)
    {
        OCI_ThreadJoin(th[i]);
        OCI_ThreadFree(th[i]);
    }

    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
