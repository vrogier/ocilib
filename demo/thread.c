#include "ocilib.h"

#define MAX_THREADS 50


void key_cleanup(void *str)
{
    free(str);
}

void worker(OCI_Thread *thread, void *data)
{
   const void *id = OCI_HandleGetThreadID(thread);
   char *str = malloc(50);

   sprintf(str, "thread %p !\n", id);

   OCI_ThreadKeySetValue("ID", str);

   /* ... do some more processing here... */

   str = OCI_ThreadKeyGetValue("ID");

   printf(str);
}

int main(void)
{
    OCI_Thread *th[MAX_THREADS];
    int i;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT | OCI_ENV_THREADED))
        return EXIT_FAILURE;

    OCI_ThreadKeyCreate("ID", key_cleanup);

     /* create threads */

    for (i = 0; i < MAX_THREADS; i++)
    {
        th[i] = OCI_ThreadCreate();
        OCI_ThreadRun(th[i], worker, NULL);
    }
  
    /* wait for threads and cleanup */

    for (i = 0; i < MAX_THREADS; i++)
    {
       OCI_ThreadJoin(th[i]);
       OCI_ThreadFree(th[i]);
    }

    OCI_Cleanup();
 
    return EXIT_SUCCESS;
}
