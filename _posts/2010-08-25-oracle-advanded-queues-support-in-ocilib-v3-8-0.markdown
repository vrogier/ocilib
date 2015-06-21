---
author: admin
comments: true
date: 2010-08-25 22:39:17+00:00
layout: post
slug: oracle-advanded-queues-support-in-ocilib-v3-8-0
title: Oracle Advanced Queues support in OCILIB v3.8.0
wordpress_id: 554
categories:
- Code
- News
- Updates
tags:
- 3.8.0
- advanced
- aq
- OCILIB
- oracle
- queue
- stream
---

Hi,

Few weeks ago, in my post ["OCILIB new Features coming soon !"](http://orclib.sourceforge.net/2010/06/ocilib-new-features-coming-soon/), i showed a basic example of Oracle AQ support in OCILIB.

I've finished to implement all advanced queues features in OCILIB and I'm now working on writing the documentation.

Here is another example :)

In this code, we're using a multiple consumers queue. It means that we can :



	
  * specify the message recipient when posting the message 

	
  * specify the consumer when checking later the queue for available messages



The following complete application code (about 65 lines only without comments !!) :

	
  * connect to the DB

	
  * create and post a first structured message (title/content) for agent 1

	
  * create and post a second structured message (title/content) for agent 2

	
  * check the queue for available messages for agent 1 and agent 2

	
  * loop and print out retrieved messages

        
  * disconnect from DB



Advanced queues has never been so easy ;)


    
    
    #include "ocilib.h"
    
    int main(int argc, char *argv[])
    {
        OCI_Connection *con;
        OCI_Enqueue    *enq;
        OCI_Dequeue    *deq;
        OCI_Msg        *msg;
        OCI_TypeInfo   *inf;
        OCI_Object     *obj;
        OCI_Agent      *agt ;
        OCI_Agent      *tab_agt[2];
     
        /* initialize OCILIB */
        OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT);
     
        /* connect to DB */
        con = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
     
        /* get message payload type information */
        inf = OCI_TypeInfoGet(con, "MY_MESSAGE", OCI_TIF_TYPE);
     
        /* create enqueue and dequeue objects for the given queue */
        enq = OCI_EnqueueCreate(inf, "my_queue_consumers");
        deq = OCI_DequeueCreate(inf, "my_queue_consumers");
     
        /* create message */
        msg = OCI_MsgCreate(inf);
        obj = OCI_ObjectCreate(con, inf);
     
        /* create 2 agents to send messages to */
        tab_agt[0] = OCI_AgentCreate(con, "AGENT1", "my_queue_consumers");
        tab_agt[1] = OCI_AgentCreate(con, "AGENT2", "my_queue_consumers");
      
        /* setup and enqueue message for consumer 1 */
        OCI_ObjectSetString(obj, "TITLE", "NEXT MEETING FOR AGENT 1");
        OCI_ObjectSetString(obj, "CONTENT", "12:00 PM IN STARBUCKS");
        OCI_MsgSetObject(msg, obj);
        OCI_MsgSetConsumers(msg, &tab;_agt[0], 1);
        OCI_EnqueuePut(enq, msg);
    
        OCI_MsgReset(msg);
    
        /* setup and enqueue message for consumer 2 */
        OCI_ObjectSetString(obj, "TITLE", "NEXT MEETING FOR AGENT 2");
        OCI_ObjectSetString(obj, "CONTENT", "14:00 PM IN NERO");
        OCI_MsgSetObject(msg, obj);
        OCI_MsgSetConsumers(msg, &tab;_agt[1], 1);
        OCI_EnqueuePut(enq, msg);
     
        /* free local message */
        OCI_ObjectFree(obj);
        OCI_MsgFree(msg);
     
        /* commit enqueuing */
        OCI_Commit(con);
     
        /* set list of consumers to wait message for */
        OCI_DequeueSetAgentList(deq, tab_agt, 2);
     
        /* listen for available messages */
        while (agt = OCI_DequeueListen(deq, 0))
        {
            /* inform Oracle we want to dequeue message for the given consumer */    
            OCI_DequeueSetConsumer(deq, OCI_AgentGetName(agt));
     
            /* get messages from queue */
            msg = OCI_DequeueGet(deq);
     
            if (msg == NULL)
                break;
    
            /* get message object payload */
            obj = OCI_MsgGetObject(msg);
     
            if (obj != NULL)
            {
                printf("'%s' : MSG '%s' => %s\n",  OCI_AgentGetName(agt),
                                                   OCI_ObjectGetString(obj, "TITLE"),
                                                   OCI_ObjectGetString(obj, "CONTENT"));
            }
        }
    
         /* commit dequeuing */
        OCI_Commit(con);
    
        /* free objects*/
        OCI_EnqueueFree(enq);
        OCI_DequeueFree(deq);
        OCI_AgentFree(tab_agt[0]);
        OCI_AgentFree(tab_agt[1]);
     
        /* disconnect from DB */
        OCI_ConnectionFree(con);
     
        /* cleanup OCILIB internal stuff */
        OCI_Cleanup();
     
       return EXIT_SUCCESS;
    }
    



And the output is :


    
    
    MSG 'NEXT MEETING FOR AGENT 1' => 12:00 PM IN STARBUCKS
    MSG 'NEXT MEETING FOR AGENT 2' => 14:00 PM IN NERO
    



Version 3.8.0 will be released by September 2010 :)





