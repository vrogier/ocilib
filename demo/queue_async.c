#include "ocilib.h"

/* requires script demo/queue.sql */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

void on_message(OCI_Dequeue *deq)
{
    OCI_Msg    *msg = OCI_DequeueGet(deq);
    OCI_Object *obj = OCI_MsgGetObject(msg);

    printf("Consumer %s - MSG '%s' => %s\n", OCI_DequeueGetConsumer(deq), OCI_ObjectGetString(obj, "TITLE"), OCI_ObjectGetString(obj, "CONTENT"));
}


int main(int argc, char *argv[])
{
    OCI_Connection *con;
    OCI_Enqueue    *enq;
    OCI_Dequeue    *deq1;
    OCI_Dequeue    *deq2;
    OCI_Msg        *msg;
    OCI_TypeInfo   *inf;
    OCI_Object     *obj;
    OCI_Agent      *agents[2];

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT | OCI_ENV_EVENTS))
    {
        return EXIT_FAILURE;
    }

    con = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);

    OCI_QueueTableCreate(con, "message_queue_table", "message_t", NULL, NULL, TRUE, OCI_AGM_NONE, NULL,  0, 0, NULL);
    OCI_QueueCreate(con, "message_queue", "message_queue_table", OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL);
    OCI_QueueStart(con, "message_queue", TRUE, TRUE);
  
    inf = OCI_TypeInfoGet(con, "message_t", OCI_TIF_TYPE);

    enq = OCI_EnqueueCreate(inf, "message_queue");
    deq1 = OCI_DequeueCreate(inf, "message_queue");
    deq2 = OCI_DequeueCreate(inf, "message_queue");

    agents[0] = OCI_AgentCreate(con, "C1", NULL);
    agents[1] = OCI_AgentCreate(con, "C2", NULL);

    OCI_DequeueSetConsumer(deq1, "C1");
    OCI_DequeueSetNavigation(deq1, OCI_ADN_FIRST_MSG);
    OCI_DequeueSetConsumer(deq2, "C2");
    OCI_DequeueSetNavigation(deq2, OCI_ADN_FIRST_MSG);

    OCI_DequeueSubscribe(deq1, 9998, 0, on_message);
    OCI_DequeueSubscribe(deq2, 9999, 0, on_message);

    msg = OCI_MsgCreate(inf);
    obj = OCI_ObjectCreate(con, inf);

    OCI_ObjectSetString(obj, "title", "NEXT MEETING");
    OCI_ObjectSetString(obj, "content", "12:00 PM IN STARBUCKS");

    OCI_MsgSetObject(msg, obj);
    OCI_MsgSetConsumers(msg, agents, 2);

    OCI_EnqueuePut(enq, msg);

    OCI_MsgFree(msg);
    OCI_ObjectFree(obj);

    OCI_Commit(con);

    printf("Wait for messages to be delivered and a press a key....\n\n");
    getchar();

    OCI_DequeueUnsubscribe(deq1);
    OCI_DequeueUnsubscribe(deq2);

    OCI_AgentFree(agents[0]);
    OCI_AgentFree(agents[1]);
    OCI_EnqueueFree(enq);
    OCI_DequeueFree(deq1);
    OCI_DequeueFree(deq2);

    OCI_QueueStop(con, "message_queue", TRUE, TRUE, FALSE);
    OCI_QueueDrop(con, "message_queue");
    OCI_QueueTableDrop(con, "message_queue_table", TRUE);

    OCI_ConnectionFree(con);

    OCI_Cleanup();

    return EXIT_SUCCESS;
}
