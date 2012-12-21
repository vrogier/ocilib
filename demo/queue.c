#include "ocilib.h"

int main(int argc, char *argv[])
{
    OCI_Connection *con;
    OCI_Enqueue    *enq;
    OCI_Dequeue    *deq;
    OCI_Msg        *msg;
    OCI_TypeInfo   *inf;
    OCI_Object     *obj;
   
    OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT);
     
    con = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);

    inf = OCI_TypeInfoGet(con, "MY_MESSAGE", OCI_TIF_TYPE);
    
    enq = OCI_EnqueueCreate(inf, "my_queue");
    deq = OCI_DequeueCreate(inf, "my_queue");
    
    msg = OCI_MsgCreate(inf);
    obj = OCI_ObjectCreate(con, inf);

    OCI_ObjectSetString(obj, "TITLE", "NEXT MEETING");
    OCI_ObjectSetString(obj, "CONTENT", "12:00 PM IN STARBUCKS");

    OCI_MsgSetObject(msg, obj);

    OCI_EnqueuePut(enq, msg);

    OCI_MsgFree(msg);
    OCI_ObjectFree(obj);

    OCI_Commit(con);
    
    msg = OCI_DequeueGet(deq);
    obj = OCI_MsgGetObject(msg);

    printf("MSG '%s' => %s\n",  OCI_ObjectGetString(obj, "TITLE"),
                                OCI_ObjectGetString(obj, "CONTENT"));

    OCI_EnqueueFree(enq);
    OCI_DequeueFree(deq);
    OCI_ConnectionFree(con);

    OCI_Cleanup();

   return EXIT_SUCCESS;
}
