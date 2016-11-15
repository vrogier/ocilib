#include "ocilib.h"

/* requires script demo/queue.sql */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(int argc, char *argv[])
{
    OCI_Connection *con;
    OCI_Enqueue    *enq;
    OCI_Dequeue    *deq;
    OCI_Msg        *msg;
    OCI_TypeInfo   *inf;
    OCI_Object     *obj;

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }

    con = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);

    OCI_QueueTableCreate(con, "message_queue_table", "message_t", NULL, NULL, FALSE, OCI_AGM_NONE, NULL,  0, 0, NULL);
    OCI_QueueCreate(con, "message_queue", "message_queue_table", OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL);
    OCI_QueueStart(con, "message_queue", TRUE, TRUE);
  
    inf = OCI_TypeInfoGet(con, "message_t", OCI_TIF_TYPE);

    enq = OCI_EnqueueCreate(inf, "message_queue");
    deq = OCI_DequeueCreate(inf, "message_queue");

    msg = OCI_MsgCreate(inf);
    obj = OCI_ObjectCreate(con, inf);

    OCI_ObjectSetString(obj, "title", "NEXT MEETING");
    OCI_ObjectSetString(obj, "content", "12:00 PM IN STARBUCKS");

    OCI_MsgSetObject(msg, obj);

    OCI_EnqueuePut(enq, msg);

    OCI_MsgFree(msg);
    OCI_ObjectFree(obj);

    OCI_Commit(con);

    msg = OCI_DequeueGet(deq);
    obj = OCI_MsgGetObject(msg);

    printf("MSG '%s' => %s\n", OCI_ObjectGetString(obj, "title"),  OCI_ObjectGetString(obj, "content"));

    OCI_EnqueueFree(enq);
    OCI_DequeueFree(deq);

    OCI_QueueStop(con, "message_queue", TRUE, TRUE, FALSE);
    OCI_QueueDrop(con, "message_queue");
    OCI_QueueTableDrop(con, "message_queue_table", TRUE);

    OCI_ConnectionFree(con);

    OCI_Cleanup();

    return EXIT_SUCCESS;
}
