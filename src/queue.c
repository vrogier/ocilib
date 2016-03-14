/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2016 Vincent ROGIER <vince.rogier@ocilib.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE VARIABLES
 * ********************************************************************************************* */

static unsigned int DeliveryModeValues[] = { OCI_APM_BUFFERED, OCI_APM_PERSISTENT, OCI_APM_ALL };
static unsigned int GroupingModeValues[] = { OCI_AGM_NONE, OCI_AGM_TRANSACTIONNAL };
static unsigned int QueueTypeValues[   ] = { OCI_AQT_NORMAL, OCI_AQT_EXCEPTION, OCI_AQT_NON_PERSISTENT };

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueCreate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueCreate
(
    OCI_Connection *con,
    const otext    *queue_name,
    const otext    *queue_table,
    unsigned int    queue_type,
    unsigned int    max_retries,
    unsigned int    retry_delay,
    unsigned int    retention_time,
    boolean         dependency_tracking,
    const otext    *comment
)
{
    OCI_Statement *st = NULL;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_name)
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_table)

    OCI_CHECK_ENUM_VALUE(con, NULL, queue_type, QueueTypeValues, OTEXT("Queue type"))

    st = OCI_StatementCreate(con);

    if (st)
    {
        call_status = OCI_Prepare
                      (
                            st,
                            OTEXT("DECLARE ")
                            OTEXT("    v_dependency_tracking BOOLEAN  := FALSE; ")
                            OTEXT("BEGIN ")
                            OTEXT("    IF (:dependency_tracking = 1) then ")
                            OTEXT("        v_dependency_tracking := TRUE; ")
                            OTEXT("    END IF; ")
                            OTEXT("    DBMS_AQADM.CREATE_QUEUE ")
                            OTEXT("    (")
                            OTEXT("        queue_name           => :queue_name, ")
                            OTEXT("        queue_table          => :queue_table, ")
                            OTEXT("        queue_type           => :queue_type, ")
                            OTEXT("        max_retries          => :max_retries, ")
                            OTEXT("        retry_delay          => :retry_delay, ")
                            OTEXT("        retention_time       => :retention_time, ")
                            OTEXT("        dependency_tracking  => v_dependency_tracking, ")
                            OTEXT("        comment              => :comment ")
                            OTEXT("    ); ")
                            OTEXT("END; ")
                      );

        call_status = call_status && OCI_BindString(st, OTEXT(":queue_name"), (otext *) queue_name, 0);
        call_status = call_status && OCI_BindString(st, OTEXT(":queue_table"), (otext *)  queue_table, 0);
        call_status = call_status && OCI_BindUnsignedInt(st, OTEXT(":queue_type"),  &queue_type);
        call_status = call_status && OCI_BindUnsignedInt(st, OTEXT(":max_retries"),  &max_retries);
        call_status = call_status && OCI_BindUnsignedInt(st, OTEXT(":retry_delay"),  &retry_delay);
        call_status = call_status && OCI_BindUnsignedInt(st, OTEXT(":retention_time"),  &retention_time);
        call_status = call_status && OCI_BindInt(st, OTEXT(":dependency_tracking"),  &dependency_tracking);
        call_status = call_status && OCI_BindString(st, OTEXT(":comment"), (otext *) (comment ? comment : OCI_STRING_EMPTY), 0);

        call_status = call_status && OCI_Execute(st);

        OCI_StatementFree(st);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueAlter
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueAlter
(
    OCI_Connection *con,
    const otext    *queue_name,
    unsigned int    max_retries,
    unsigned int    retry_delay,
    unsigned int    retention_time,
    const otext    *comment
)
{
    OCI_Statement *st = NULL;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_name)

    st = OCI_StatementCreate(con);

    if (st)
    {
        call_status = OCI_Prepare
                    (
                        st,
                        OTEXT("BEGIN ")
                        OTEXT("    DBMS_AQADM.ALTER_QUEUE ")
                        OTEXT("    (")
                        OTEXT("        queue_name           => :queue_name, ")
                        OTEXT("        max_retries          => :max_retries, ")
                        OTEXT("        retry_delay          => :retry_delay, ")
                        OTEXT("        retention_time       => :retention_time, ")
                        OTEXT("        comment              => :comment ")
                        OTEXT("    ); ")
                        OTEXT("END; ")
                    );

        call_status = call_status && OCI_BindString(st, OTEXT(":queue_name"),  (otext *) queue_name, 0);
        call_status = call_status && OCI_BindUnsignedInt(st, OTEXT(":max_retries"),  &max_retries);
        call_status = call_status && OCI_BindUnsignedInt(st, OTEXT(":retry_delay"),  &retry_delay);
        call_status = call_status && OCI_BindUnsignedInt(st, OTEXT(":retention_time"),  &retention_time);
        call_status = call_status && OCI_BindString(st, OTEXT(":comment"), (otext *) (comment ? comment : OCI_STRING_EMPTY), 0);

        call_status = call_status && OCI_Execute(st);

        OCI_StatementFree(st);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueDestroy
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueDrop
(
    OCI_Connection *con,
    const otext    *queue_name
)
{
    OCI_Statement *st = NULL;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_name)
    
    st = OCI_StatementCreate(con);

    if (st)
    {
        call_status = OCI_Prepare
                      (
                            st,
                            OTEXT("BEGIN ")
                            OTEXT("    DBMS_AQADM.DROP_QUEUE ")
                            OTEXT("    (")
                            OTEXT("        queue_name  => :queue_name ")
                            OTEXT("    ); ")
                            OTEXT("END; ")
                      );

        call_status = call_status && OCI_BindString(st, OTEXT(":queue_name"),  (otext *) queue_name, 0);

        call_status = call_status && OCI_Execute(st);

        OCI_StatementFree(st);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueStart
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueStart
(
    OCI_Connection *con,
    const otext    *queue_name,
    boolean         enqueue,
    boolean         dequeue
)
{
    OCI_Statement *st = NULL;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_name)
    
    st = OCI_StatementCreate(con);

    if (st)
    {
        call_status = OCI_Prepare
                      (
                            st,
                            OTEXT("DECLARE ")
                            OTEXT("    v_enqueue BOOLEAN  := FALSE; ")
                            OTEXT("    v_dequeue BOOLEAN  := FALSE; ")
                            OTEXT("BEGIN ")
                            OTEXT("    IF (:enqueue = 1) then ")
                            OTEXT("        v_enqueue := TRUE; ")
                            OTEXT("    END IF; ")
                            OTEXT("    IF (:dequeue = 1) then ")
                            OTEXT("        v_dequeue := TRUE; ")
                            OTEXT("    END IF; ")
                            OTEXT("    DBMS_AQADM.START_QUEUE ")
                            OTEXT("   (")
                            OTEXT("       queue_name => :queue_name, ")
                            OTEXT("       enqueue    => v_enqueue, ")
                            OTEXT("       dequeue    => v_dequeue ")
                            OTEXT("   ); ")
                            OTEXT("END; ")
                      );

        call_status = call_status && OCI_BindString(st, OTEXT(":queue_name"),  (otext *) queue_name, 0);
        call_status = call_status && OCI_BindInt(st, OTEXT(":enqueue"),  &enqueue);
        call_status = call_status && OCI_BindInt(st, OTEXT(":dequeue"),  &dequeue);

        call_status = call_status && OCI_Execute(st);

        OCI_StatementFree(st);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueStop
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueStop
(
    OCI_Connection *con,
    const otext    *queue_name,
    boolean         enqueue,
    boolean         dequeue,
    boolean         wait
)
{
    OCI_Statement *st = NULL;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_name)

    st = OCI_StatementCreate(con);

    if (st)
    {
        call_status = OCI_Prepare
                      (
                            st,
                            OTEXT("DECLARE ")
                            OTEXT("    v_enqueue BOOLEAN  := FALSE; ")
                            OTEXT("    v_dequeue BOOLEAN  := FALSE; ")
                            OTEXT("    v_wait    BOOLEAN  := FALSE; ")
                            OTEXT("BEGIN ")
                            OTEXT("    IF (:enqueue = 1) then ")
                            OTEXT("        v_enqueue := TRUE; ")
                            OTEXT("    END IF; ")
                            OTEXT("    IF (:dequeue = 1) then ")
                            OTEXT("        v_dequeue := TRUE; ")
                            OTEXT("    END IF; ")
                            OTEXT("    IF (:wait = 1) then ")
                            OTEXT("        v_wait := TRUE; ")
                            OTEXT("    END IF; ")
                            OTEXT("    DBMS_AQADM.STOP_QUEUE ")
                            OTEXT("   (")
                            OTEXT("       queue_name => :queue_name, ")
                            OTEXT("       enqueue    => v_enqueue, ")
                            OTEXT("       dequeue    => v_dequeue, ")
                            OTEXT("       wait       => v_wait ")
                            OTEXT("   ); ")
                            OTEXT("END; ")
                      );

        call_status = call_status && OCI_BindString(st, OTEXT(":queue_name"),  (otext *) queue_name, 0);
        call_status = call_status && OCI_BindInt(st, OTEXT(":enqueue"),  &enqueue);
        call_status = call_status && OCI_BindInt(st, OTEXT(":dequeue"),  &dequeue);
        call_status = call_status && OCI_BindInt(st, OTEXT(":wait"),  &wait);

        call_status = call_status && OCI_Execute(st);

        OCI_StatementFree(st);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueTableCreate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueTableCreate
(
    OCI_Connection *con,
    const otext    *queue_table,
    const otext    *queue_payload_type,
    const otext    *storage_clause,
    const otext    *sort_list,
    boolean         multiple_consumers,
    unsigned int    message_grouping,
    const otext    *comment,
    unsigned int    primary_instance,
    unsigned int    secondary_instance,
    const otext    *compatible
)
{
    OCI_Statement *st = NULL;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_table)
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_payload_type)

    OCI_CHECK_ENUM_VALUE(con, NULL, message_grouping, GroupingModeValues, OTEXT("Grouping mode"))

    st = OCI_StatementCreate(con);

    if (st)
    {
        call_status = OCI_Prepare
                      (
                            st,
                            OTEXT("DECLARE ")
                            OTEXT("    v_multiple_consumers BOOLEAN  := FALSE; ")
                            OTEXT("BEGIN ")
                            OTEXT("    IF (:multiple_consumers = 1) then ")
                            OTEXT("        v_multiple_consumers := TRUE; ")
                            OTEXT("    END IF; ")
                            OTEXT("    DBMS_AQADM.CREATE_QUEUE_TABLE ")
                            OTEXT("   (")
                            OTEXT("       queue_table        => :queue_table, ")
                            OTEXT("       queue_payload_type => :queue_payload_type, ")
                            OTEXT("       storage_clause     => :storage_clause, ")
                            OTEXT("       sort_list          => :sort_list, ")
                            OTEXT("       multiple_consumers => v_multiple_consumers, ")
                            OTEXT("       message_grouping   => :message_grouping, ")
                            OTEXT("       comment            => :comment, ")
                            OTEXT("       primary_instance   => :primary_instance, ")
                            OTEXT("       secondary_instance => :secondary_instance, ")
                            OTEXT("       compatible         => :compatible")
                            OTEXT("   ); ")
                            OTEXT("END; ")
                      );

        call_status = call_status && OCI_BindString(st, OTEXT(":queue_table"), (otext *) queue_table, 0);
        call_status = call_status && OCI_BindString(st, OTEXT(":queue_payload_type"), (otext *) queue_payload_type, 0);
        call_status = call_status && OCI_BindString(st, OTEXT(":storage_clause"), (otext *) (storage_clause ? storage_clause : OCI_STRING_EMPTY), 0);
        call_status = call_status && OCI_BindString(st, OTEXT(":sort_list"), (otext *) (sort_list ? sort_list : OCI_STRING_EMPTY), 0);
        call_status = call_status && OCI_BindInt(st, OTEXT(":multiple_consumers"),  &multiple_consumers);
        call_status = call_status && OCI_BindUnsignedInt(st, OTEXT(":message_grouping"),  &message_grouping);
        call_status = call_status && OCI_BindString(st, OTEXT(":comment"), (otext *)( comment ? comment : OCI_STRING_EMPTY), 0);
        call_status = call_status && OCI_BindUnsignedInt(st, OTEXT(":primary_instance"),  &primary_instance);
        call_status = call_status && OCI_BindUnsignedInt(st, OTEXT(":secondary_instance"),  &secondary_instance);
        call_status = call_status && OCI_BindString(st, OTEXT(":compatible"), (otext *) (compatible ? compatible : OCI_STRING_EMPTY), 0);

        call_status = call_status && OCI_Execute(st);

        OCI_StatementFree(st);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueTableAlter
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueTableAlter
(
    OCI_Connection *con,
    const otext    *queue_table,
    const otext    *comment,
    unsigned int    primary_instance,
    unsigned int    secondary_instance
)
{
    OCI_Statement *st = NULL;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_table)

    st = OCI_StatementCreate(con);

    if (st)
    {
        call_status = OCI_Prepare
                      (
                            st,
                            OTEXT("BEGIN ")
                            OTEXT("    DBMS_AQADM.ALTER_QUEUE_TABLE ")
                            OTEXT("   (")
                            OTEXT("       queue_table        => :queue_table, ")
                            OTEXT("       comment            => :comment, ")
                            OTEXT("       primary_instance   => :primary_instance, ")
                            OTEXT("       secondary_instance => :secondary_instance ")
                            OTEXT("   ); ")
                            OTEXT("END; ")
                      );

        call_status = call_status && OCI_BindString(st, OTEXT(":queue_table"), (otext *)  queue_table, 0);
        call_status = call_status && OCI_BindString(st, OTEXT(":comment"),  (otext *) (comment ?  comment: OCI_STRING_EMPTY), 0);
        call_status = call_status && OCI_BindUnsignedInt(st, OTEXT(":primary_instance"),  &primary_instance);
        call_status = call_status && OCI_BindUnsignedInt(st, OTEXT(":secondary_instance"),  &secondary_instance);

        call_status = call_status && OCI_Execute(st);

        OCI_StatementFree(st);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueTableDrop
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueTableDrop
(
    OCI_Connection *con,
    const otext    *queue_table,
    boolean         force
)
{
    OCI_Statement *st = NULL;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_table)

    st = OCI_StatementCreate(con);

    if (st)
    {
        call_status = OCI_Prepare
                      (
                            st,
                            OTEXT("DECLARE ")
                            OTEXT("    v_force       BOOLEAN  := FALSE; ")
                            OTEXT("BEGIN ")
                            OTEXT("    IF (:force = 1) then ")
                            OTEXT("        v_force := TRUE; ")
                            OTEXT("    END IF; ")
                            OTEXT("    DBMS_AQADM.DROP_QUEUE_TABLE ")
                            OTEXT("   (")
                            OTEXT("       queue_table  => :queue_table, ")
                            OTEXT("       force        => v_force ")
                            OTEXT("   ); ")
                            OTEXT("END; ")
                      );

        call_status = call_status && OCI_BindString(st, OTEXT(":queue_table"), (otext *)queue_table, 0);
        call_status = call_status && OCI_BindInt(st, OTEXT(":force"), &force);

        call_status = call_status && OCI_Execute(st);

        OCI_StatementFree(st);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueTablePurge
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueTablePurge
(
    OCI_Connection *con,
    const otext    *queue_table,
    const otext    *purge_condition,
    boolean         block,
    unsigned int    delivery_mode
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_table)

    OCI_CHECK_ENUM_VALUE(con, NULL, delivery_mode, DeliveryModeValues, OTEXT("Delivery mode"))

    if (con->ver_num >= OCI_10_1)
    {
        OCI_Statement *st = NULL;

        st = OCI_StatementCreate(con);

        if (st)
        {
            call_status = OCI_Prepare
                          (
                                st,
                                OTEXT("DECLARE ")
                                OTEXT("    v_purge_options DBMS_AQADM.AQ$_PURGE_OPTIONS_T; ")
                                OTEXT("    v_block         BOOLEAN := FALSE; ")
                                OTEXT("BEGIN ")
                                OTEXT("    v_purge_options.block         := FALSE; ")
                                OTEXT("    v_purge_options.delivery_mode := :delivery_mode; ")
                                OTEXT("    IF (:block = 1) then ")
                                OTEXT("        v_purge_options.block := TRUE; ")
                                OTEXT("    END IF; ")
                                OTEXT("    DBMS_AQADM.PURGE_QUEUE_TABLE ")
                                OTEXT("    (")
                                OTEXT("        queue_table      => :queue_table, ")
                                OTEXT("        purge_condition  => :purge_condition, ")
                                OTEXT("        purge_options    => v_purge_options ")
                                OTEXT("   ); ")
                                OTEXT("END; ")
                          );

            call_status = call_status && OCI_BindString(st, OTEXT(":queue_table"), (otext *)queue_table, 0);
            call_status = call_status && OCI_BindString(st, OTEXT(":purge_condition"),
                                                        (otext *) (purge_condition ? purge_condition : OCI_STRING_EMPTY), 0);
            call_status = call_status && OCI_BindInt(st, OTEXT(":block"), &block);
            call_status = call_status && OCI_BindUnsignedInt(st, OTEXT(":delivery_mode"), &delivery_mode);

            call_status = call_status && OCI_Execute(st);

            OCI_StatementFree(st);
        }

        call_retval = call_status;
    }
    else
    {
        call_status = TRUE;
        call_retval = FALSE;
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueTableMigrate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueTableMigrate
(
    OCI_Connection *con,
    const otext    *queue_table,
    const otext    *compatible
)
{
    OCI_Statement *st = NULL;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_table)
    OCI_CHECK_PTR(OCI_IPC_STRING, compatible)

    st = OCI_StatementCreate(con);

    if (st)
    {
        call_status = OCI_Prepare
                      (
                            st,
                            OTEXT("BEGIN ")
                            OTEXT("   DBMS_AQADM.MIGRATE_QUEUE_TABLE")
                            OTEXT("   (")
                            OTEXT("       queue_table => :queue_table, ")
                            OTEXT("       compatible  => :compatible ")
                            OTEXT("   );")
                            OTEXT("END;")
                      );

        call_status = call_status && OCI_BindString(st, OTEXT(":queue_table"), (otext *)queue_table, 0);
        call_status = call_status && OCI_BindString(st, OTEXT(":compatible"), (otext *)compatible, 0);

        call_status = call_status && OCI_Execute(st);

        OCI_StatementFree(st);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}
