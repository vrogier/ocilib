#include "ocilib.h"

/* requires script demo/queue.sql */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(OCI_Int32 argc, char *argv[])
{

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    const char *db  = getenv("OCILIB_DEMO_DB");
    const char *usr = getenv("OCILIB_DEMO_USR");
    const char *pwd = getenv("OCILIB_DEMO_PWD");

    OCI_Connection * con = OCI_ConnectionCreate(env, db, usr, pwd, OCI_SESSION_DEFAULT);

    OCI_QueueTableCreate(con, "message_queue_table", "message_t", NULL, NULL, FALSE, OCI_AGM_NONE, NULL,  0, 0, NULL);
    OCI_QueueCreate(con, "message_queue", "message_queue_table", OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL);
    OCI_QueueStart(con, "message_queue", TRUE, TRUE);
  
    OCI_TypeInfo   * inf = OCI_TypeInfoGet(con, "message_t", OCI_TIF_TYPE);

    OCI_Enqueue    * enq = OCI_EnqueueCreate(inf, "message_queue");
    OCI_Dequeue    * deq = OCI_DequeueCreate(inf, "message_queue");

    OCI_Message        * msg = OCI_MessageCreate(inf);
    OCI_Object     * obj = OCI_ObjectCreate(inf);

    OCI_ObjectSetString(obj, "title", "NEXT MEETING");
    OCI_ObjectSetString(obj, "content", "12:00 PM IN STARBUCKS");

    OCI_MessageSetObject(msg, obj);

    OCI_EnqueuePut(enq, msg);

    OCI_MessageFree(msg);
    OCI_ObjectFree(obj);

    OCI_ConnectionCommit(con);

    msg = OCI_DequeueGet(deq);
    obj = OCI_MessageGetObject(msg);

    printf("MSG '%s' => %s\n", OCI_ObjectGetString(obj, "title"),  OCI_ObjectGetString(obj, "content"));

    OCI_EnqueueFree(enq);
    OCI_DequeueFree(deq);

    OCI_QueueStop(con, "message_queue", TRUE, TRUE, FALSE);
    OCI_QueueDrop(con, "message_queue");
    OCI_QueueTableDrop(con, "message_queue_table", TRUE);

    OCI_ConnectionFree(con);

    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
