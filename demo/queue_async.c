#include "ocilib.h"

void on_message(OCI_Dequeue *deq)
{
    OCI_Msg    *msg = OCI_DequeueGet(deq);
    OCI_Object *obj = OCI_MsgGetObject(msg);

    printf("Consumer %s - MSG '%s' => %s\n",  OCI_DequeueGetConsumer(deq), OCI_ObjectGetString(obj, "TITLE"), OCI_ObjectGetString(obj, "CONTENT"));
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

    OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT | OCI_ENV_THREADED |OCI_ENV_EVENTS);

    con = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    
    OCI_Immediate(con, "create type my_message as object (title varchar2(50), content varchar2(50)) ");

    OCI_QueueTableCreate(con, "my_queue_table", "my_message", NULL, NULL, TRUE, FALSE, NULL, 0,0,NULL);
    OCI_QueueCreate(con, "my_queue", "my_queue_table", OCIT_NORMAL, 0, 0, 0, 0, NULL);
    OCI_QueueStart(con, "my_queue", TRUE, TRUE);

    agents[0] = OCI_AgentCreate(con, "C1", NULL);
    agents[1] = OCI_AgentCreate(con, "C2", NULL);

    inf = OCI_TypeInfoGet(con, "MY_MESSAGE", OCI_TIF_TYPE);

    enq = OCI_EnqueueCreate(inf, "my_queue");
    deq1 = OCI_DequeueCreate(inf, "my_queue");
    deq2 = OCI_DequeueCreate(inf, "my_queue");

    OCI_DequeueSetConsumer(deq1, "C1");
    OCI_DequeueSetNavigation(deq1, OCI_ADN_FIRST_MSG);
    OCI_DequeueSetConsumer(deq2, "C2");
    OCI_DequeueSetNavigation(deq2, OCI_ADN_FIRST_MSG);

    OCI_DequeueSubscribe(deq1, 9998, 0, on_message);
    OCI_DequeueSubscribe(deq2, 9999, 0, on_message);

    msg = OCI_MsgCreate(inf);
    obj = OCI_ObjectCreate(con, inf);

    OCI_ObjectSetString(obj, "TITLE", "NEXT MEETING");
    OCI_ObjectSetString(obj, "CONTENT", "12:00 PM IN STARBUCKS");

    OCI_MsgSetObject(msg, obj);
    OCI_MsgSetConsumers(msg, agents, 2);

    OCI_EnqueuePut(enq, msg);

    OCI_MsgFree(msg);
    OCI_ObjectFree(obj);

    OCI_Commit(con);

    getchar();

    OCI_DequeueUnsubscribe(deq1);    
    OCI_DequeueUnsubscribe(deq2);    

    OCI_AgentFree(agents[0]);
    OCI_AgentFree(agents[1]);
    OCI_EnqueueFree(enq);
    OCI_DequeueFree(deq1);
    OCI_DequeueFree(deq2);
 
    OCI_QueueStop(con, "my_queue", TRUE, TRUE, 0);
    OCI_QueueDrop(con, "my_queue");
    OCI_QueueTableDrop(con, "my_queue_table", TRUE);
    OCI_Immediate(con, "drop type my_message");

    OCI_ConnectionFree(con);

    OCI_Cleanup();

    return EXIT_SUCCESS;
}
