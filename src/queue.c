/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2010 Vincent ROGIER <vince.rogier@ocilib.net>            |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |             This library is free software; you can redistribute it and/or               |
    |             modify it under the terms of the GNU Lesser General Public                  |
    |             License as published by the Free Software Foundation; either                |
    |             version 2 of the License, or (at your option) any later version.            |
    |                                                                                         |
    |             This library is distributed in the hope that it will be useful,             |
    |             but WITHOUT ANY WARRANTY; without even the implied warranty of              |
    |             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           |
    |             Lesser General Public License for more details.                             |
    |                                                                                         |
    |             You should have received a copy of the GNU Lesser General Public            |
    |             License along with this library; if not, write to the Free                  |
    |             Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.          |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
*/

/* --------------------------------------------------------------------------------------------- *
 * $Id: queue.c, v 3.8.1 2010-12-13 00:00 Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueCreate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueCreate
(
    OCI_Connection *con,
    const mtext    *queue_name,
    const mtext    *queue_table,
    unsigned int    queue_type,
    unsigned int    max_retries,
    unsigned int    retry_delay,
    unsigned int    retention_time,
    boolean         dependency_tracking,
    const mtext    *comment
)
{
    boolean res       = FALSE;
    OCI_Statement *st = NULL;
    void *bstr1       = NULL, *bstr2 = NULL, *bstr3 = NULL;
    int bsize1        = -1, bsize2 = -1, bsize3 = -1;
    dtext *null_str   = DT("");

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_name, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_table, FALSE);

    bstr1 = OCI_GetMetaFromDataString(queue_name,  &bsize1);
    bstr2 = OCI_GetMetaFromDataString(queue_table, &bsize2);
    bstr3 = OCI_GetMetaFromDataString(comment,     &bsize3);

    if (bstr3 == NULL)
        bstr3 = null_str;

    st = OCI_StatementCreate(con);

    if (st)
    {
        res = OCI_Prepare
              (
                st,
                MT("DECLARE ")
                MT("    v_dependency_tracking BOOLEAN  := FALSE; ")
                MT("BEGIN ")
                MT("    IF (:dependency_tracking = 1) then ")
                MT("        v_dependency_tracking := TRUE; ")
                MT("    END IF; ")
                MT("    DBMS_AQADM.CREATE_QUEUE ")
                MT("    (")
                MT("        queue_name           => :queue_name, ")
                MT("        queue_table          => :queue_table, ")
                MT("        queue_type           => :queue_type, ")
                MT("        max_retries          => :max_retries, ")
                MT("        retry_delay          => :retry_delay, ")
                MT("        retention_time       => :retention_time, ")
                MT("        dependency_tracking  => v_dependency_tracking, ")
                MT("        comment              => :comment ")
                MT("    ); ")
                MT("END; ")
              );

        res = res && OCI_BindString(st, MT(":queue_name"),  bstr1, 0);
        res = res && OCI_BindString(st, MT(":queue_table"), bstr2, 0);
        res = res && OCI_BindUnsignedInt(st, MT(":queue_type"),  &queue_type);
        res = res && OCI_BindUnsignedInt(st, MT(":max_retries"),  &max_retries);
        res = res && OCI_BindUnsignedInt(st, MT(":retry_delay"),  &retry_delay);
        res = res && OCI_BindUnsignedInt(st, MT(":retention_time"),  &retention_time);
        res = res && OCI_BindInt(st, MT(":dependency_tracking"),  &dependency_tracking);
        res = res && OCI_BindString(st, MT(":comment"), bstr3, 0);

        res = res && OCI_Execute(st);

        OCI_StatementFree(st);
    }

    OCI_ReleaseDataString(bstr1);
    OCI_ReleaseDataString(bstr2);

    if (comment != NULL)
        OCI_ReleaseDataString(bstr3);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueAlter
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueAlter
(
    OCI_Connection *con,
    const mtext    *queue_name,
    unsigned int    max_retries,
    unsigned int    retry_delay,
    unsigned int    retention_time,
    const mtext    *comment
)
{
    boolean res       = FALSE;
    OCI_Statement *st = NULL;
    void *bstr1       = NULL, *bstr2 = NULL;
    int bsize1        = -1, bsize2 = -1;
    dtext *null_str   = DT("");

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_name, FALSE);

    bstr1 = OCI_GetMetaFromDataString(queue_name,  &bsize1);
    bstr2 = OCI_GetMetaFromDataString(comment,     &bsize2);

    if (bstr2 == NULL)
        bstr2 = null_str;

    st = OCI_StatementCreate(con);

    if (st)
    {
        res = OCI_Prepare
              (
                st,
                MT("BEGIN ")
                MT("    DBMS_AQADM.ALTER_QUEUE ")
                MT("    (")
                MT("        queue_name           => :queue_name, ")
                MT("        max_retries          => :max_retries, ")
                MT("        retry_delay          => :retry_delay, ")
                MT("        retention_time       => :retention_time, ")
                MT("        comment              => :comment ")
                MT("    ); ")
                MT("END; ")
              );

        res = res && OCI_BindString(st, MT(":queue_name"),  bstr1, 0);
        res = res && OCI_BindUnsignedInt(st, MT(":max_retries"),  &max_retries);
        res = res && OCI_BindUnsignedInt(st, MT(":retry_delay"),  &retry_delay);
        res = res && OCI_BindUnsignedInt(st, MT(":retention_time"),  &retention_time);
        res = res && OCI_BindString(st, MT(":comment"), bstr2, 0);

        res = res && OCI_Execute(st);

        OCI_StatementFree(st);
    }

    OCI_ReleaseDataString(bstr1);

    if (comment != NULL)
        OCI_ReleaseDataString(bstr2);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueDestroy
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueDrop
(
    OCI_Connection *con,
    const mtext    *queue_name
)
{
    boolean res       = FALSE;
    OCI_Statement *st = NULL;
    void *bstr1       = NULL;
    int bsize1        = -1;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_name, FALSE);

    bstr1 = OCI_GetMetaFromDataString(queue_name,  &bsize1);

    st = OCI_StatementCreate(con);

    if (st)
    {
        res = OCI_Prepare
              (
                st,
                MT("BEGIN ")
                MT("    DBMS_AQADM.DROP_QUEUE ")
                MT("    (")
                MT("        queue_name  => :queue_name ")
                MT("    ); ")
                MT("END; ")
              );

        res = res && OCI_BindString(st, MT(":queue_name"),  bstr1, 0);

        res = res && OCI_Execute(st);

        OCI_StatementFree(st);
    }

    OCI_ReleaseDataString(bstr1);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueStart
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueStart
(
    OCI_Connection *con,
    const mtext    *queue_name,
    boolean         enqueue,
    boolean         dequeue
)
{
    boolean res       = FALSE;
    OCI_Statement *st = NULL;
    void *bstr1       = NULL;
    int bsize1        = -1;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_name, FALSE);

    bstr1 = OCI_GetMetaFromDataString(queue_name,  &bsize1);

    st = OCI_StatementCreate(con);

    if (st)
    {
        res = OCI_Prepare
              (
            st,
            MT("DECLARE ")
            MT("    v_enqueue BOOLEAN  := FALSE; ")
            MT("    v_dequeue BOOLEAN  := FALSE; ")
            MT("BEGIN ")
            MT("    IF (:enqueue = 1) then ")
            MT("        v_enqueue := TRUE; ")
            MT("    END IF; ")
            MT("    IF (:dequeue = 1) then ")
            MT("        v_dequeue := TRUE; ")
            MT("    END IF; ")
            MT("    DBMS_AQADM.START_QUEUE ")
            MT("   (")
            MT("       queue_name => :queue_name, ")
            MT("       enqueue    => v_enqueue, ")
            MT("       dequeue    => v_dequeue ")
            MT("   ); ")
            MT("END; ")
              );

        res = res && OCI_BindString(st, MT(":queue_name"),  bstr1, 0);
        res = res && OCI_BindInt(st, MT(":enqueue"),  &enqueue);
        res = res && OCI_BindInt(st, MT(":dequeue"),  &dequeue);

        res = res && OCI_Execute(st);

        OCI_StatementFree(st);
    }

    OCI_ReleaseDataString(bstr1);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueStop
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueStop
(
    OCI_Connection *con,
    const mtext    *queue_name,
    boolean         enqueue,
    boolean         dequeue,
    boolean         wait
)
{
    boolean res       = FALSE;
    OCI_Statement *st = NULL;
    void *bstr1       = NULL;
    int bsize1        = -1;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_name, FALSE);

    bstr1 = OCI_GetMetaFromDataString(queue_name,  &bsize1);

    st = OCI_StatementCreate(con);

    if (st)
    {
        res = OCI_Prepare
              (
            st,
            MT("DECLARE ")
            MT("    v_enqueue BOOLEAN  := FALSE; ")
            MT("    v_dequeue BOOLEAN  := FALSE; ")
            MT("    v_wait    BOOLEAN  := FALSE; ")
            MT("BEGIN ")
            MT("    IF (:enqueue = 1) then ")
            MT("        v_enqueue := TRUE; ")
            MT("    END IF; ")
            MT("    IF (:dequeue = 1) then ")
            MT("        v_dequeue := TRUE; ")
            MT("    END IF; ")
            MT("    IF (:wait = 1) then ")
            MT("        v_wait := TRUE; ")
            MT("    END IF; ")
            MT("    DBMS_AQADM.STOP_QUEUE ")
            MT("   (")
            MT("       queue_name => :queue_name, ")
            MT("       enqueue    => v_enqueue, ")
            MT("       dequeue    => v_dequeue, ")
            MT("       wait       => v_wait ")
            MT("   ); ")
            MT("END; ")
              );

        res = res && OCI_BindString(st, MT(":queue_name"),  bstr1, 0);
        res = res && OCI_BindInt(st, MT(":enqueue"),  &enqueue);
        res = res && OCI_BindInt(st, MT(":dequeue"),  &dequeue);
        res = res && OCI_BindInt(st, MT(":wait"),  &wait);

        res = res && OCI_Execute(st);

        OCI_StatementFree(st);
    }

    OCI_ReleaseDataString(bstr1);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueTableCreate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueTableCreate
(
    OCI_Connection *con,
    const mtext    *queue_table,
    const mtext    *queue_payload_type,
    const mtext    *storage_clause,
    const mtext    *sort_list,
    boolean         multiple_consumers,
    unsigned int    message_grouping,
    const mtext    *comment,
    unsigned int    primary_instance,
    unsigned int    secondary_instance,
    const mtext    *compatible
)
{
    boolean res       = FALSE;
    OCI_Statement *st = NULL;
    void *bstr1       = NULL, *bstr2 = NULL, *bstr3 = NULL;
    void *bstr4       = NULL, *bstr5 = NULL, *bstr6 = NULL;
    int bsize1        = -1, bsize2 = -1, bsize3 = -1;
    int bsize4        = -1, bsize5 = -1, bsize6 = -1;
    dtext *null_str   = DT("");

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_table, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_payload_type, FALSE);

    bstr1 = OCI_GetMetaFromDataString(queue_table,        &bsize1);
    bstr2 = OCI_GetMetaFromDataString(queue_payload_type, &bsize2);
    bstr3 = OCI_GetMetaFromDataString(storage_clause,     &bsize3);
    bstr4 = OCI_GetMetaFromDataString(sort_list,          &bsize4);
    bstr5 = OCI_GetMetaFromDataString(comment,            &bsize5);
    bstr6 = OCI_GetMetaFromDataString(compatible,         &bsize6);

    if (bstr3 == NULL)
        bstr3 = null_str;

    if (bstr4 == NULL)
        bstr4 = null_str;

    if (bstr5 == NULL)
        bstr5 = null_str;

    if (bstr6 == NULL)
        bstr6 = null_str;

    st = OCI_StatementCreate(con);

    if (st)
    {
        res = OCI_Prepare
              (
            st,
            MT("DECLARE ")
            MT("    v_multiple_consumers BOOLEAN  := FALSE; ")
            MT("BEGIN ")
            MT("    IF (:multiple_consumers = 1) then ")
            MT("        v_multiple_consumers := TRUE; ")
            MT("    END IF; ")
            MT("    DBMS_AQADM.CREATE_QUEUE_TABLE ")
            MT("   (")
            MT("       queue_table        => :queue_table, ")
            MT("       queue_payload_type => :queue_payload_type, ")
            MT("       storage_clause     => :storage_clause, ")
            MT("       sort_list          => :sort_list, ")
            MT("       multiple_consumers => v_multiple_consumers, ")
            MT("       message_grouping   => :message_grouping, ")
            MT("       comment            => :comment, ")
            MT("       primary_instance   => :primary_instance, ")
            MT("       secondary_instance => :secondary_instance, ")
            MT("       compatible         => :compatible")
            MT("   ); ")
            MT("END; ")
              );

        res = res && OCI_BindString(st, MT(":queue_table"),  bstr1, 0);
        res = res && OCI_BindString(st, MT(":queue_payload_type"), bstr2, 0);
        res = res && OCI_BindString(st, MT(":storage_clause"), bstr3, 0);
        res = res && OCI_BindString(st, MT(":sort_list"), bstr4, 0);
        res = res && OCI_BindInt(st, MT(":multiple_consumers"),  &multiple_consumers);
        res = res && OCI_BindUnsignedInt(st, MT(":message_grouping"),  &message_grouping);
        res = res && OCI_BindString(st, MT(":comment"), bstr5, 0);
        res = res && OCI_BindUnsignedInt(st, MT(":primary_instance"),  &primary_instance);
        res = res && OCI_BindUnsignedInt(st, MT(":secondary_instance"),  &secondary_instance);
        res = res && OCI_BindString(st, MT(":compatible"), bstr6, 0);

        res = res && OCI_Execute(st);

        OCI_StatementFree(st);
    }

    OCI_ReleaseDataString(bstr1);
    OCI_ReleaseDataString(bstr2);

    if (storage_clause != NULL)
        OCI_ReleaseDataString(bstr3);

    if (sort_list != NULL)
        OCI_ReleaseDataString(bstr4);

    if (comment != NULL)
        OCI_ReleaseDataString(bstr5);

    if (compatible != NULL)
        OCI_ReleaseDataString(bstr6);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueTableAlter
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueTableAlter
(
    OCI_Connection *con,
    const mtext    *queue_table,
    const mtext    *comment,
    unsigned int    primary_instance,
    unsigned int    secondary_instance
)
{
    boolean res       = FALSE;
    OCI_Statement *st = NULL;
    void *bstr1       = NULL, *bstr2 = NULL;
    int bsize1        = -1, bsize2 = -1;
    dtext *null_str   = DT("");

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_table, FALSE);

    bstr1 = OCI_GetMetaFromDataString(queue_table,    &bsize1);
    bstr2 = OCI_GetMetaFromDataString(comment,        &bsize2);

    if (bstr2 == NULL)
        bstr2 = null_str;

    st = OCI_StatementCreate(con);

    if (st)
    {
        res = OCI_Prepare
              (
            st,
            MT("BEGIN ")
            MT("    DBMS_AQADM.ALTER_QUEUE_TABLE ")
            MT("   (")
            MT("       queue_table        => :queue_table, ")
            MT("       comment            => :comment, ")
            MT("       primary_instance   => :primary_instance, ")
            MT("       secondary_instance => :secondary_instance ")
            MT("   ); ")
            MT("END; ")
              );

        res = res && OCI_BindString(st, MT(":queue_table"),  bstr1, 0);
        res = res && OCI_BindString(st, MT(":comment"), bstr2, 0);
        res = res && OCI_BindUnsignedInt(st, MT(":primary_instance"),  &primary_instance);
        res = res && OCI_BindUnsignedInt(st, MT(":secondary_instance"),  &secondary_instance);

        res = res && OCI_Execute(st);

        OCI_StatementFree(st);
    }

    OCI_ReleaseDataString(bstr1);

    if (comment != NULL)
        OCI_ReleaseDataString(bstr2);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueTableDrop
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueTableDrop
(
    OCI_Connection *con,
    const mtext    *queue_table,
    boolean         force
)
{
    boolean res       = FALSE;
    void *bstr1       = NULL;
    int bsize1        = -1;
    OCI_Statement *st = NULL;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_table, FALSE);

    bstr1 = OCI_GetMetaFromDataString(queue_table, &bsize1);

    st = OCI_StatementCreate(con);

    if (st)
    {
        res = OCI_Prepare
              (
            st,
            MT("DECLARE ")
            MT("    v_force       BOOLEAN  := FALSE; ")
            MT("BEGIN ")
            MT("    IF (:force = 1) then ")
            MT("        v_force := TRUE; ")
            MT("    END IF; ")
            MT("    DBMS_AQADM.DROP_QUEUE_TABLE ")
            MT("   (")
            MT("       queue_table  => :queue_table, ")
            MT("       force        => v_force ")
            MT("   ); ")
            MT("END; ")
              );

        res = res && OCI_BindString(st, MT(":queue_table"), bstr1, 0);
        res = res && OCI_BindInt(st, MT(":force"),  &force);

        res = res && OCI_Execute(st);

        OCI_StatementFree(st);
    }

    OCI_ReleaseDataString(bstr1);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueTablePurge
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueTablePurge
(
    OCI_Connection *con,
    const mtext    *queue_table,
    const mtext    *purge_condition,
    boolean         block,
    unsigned int    delivery_mode
)
{
    boolean res       = FALSE;
    OCI_Statement *st = NULL;
    void *bstr1       = NULL, *bstr2 = NULL;
    int bsize1        = -1, bsize2 = -1;
    dtext *null_str   = DT("");

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_table, FALSE);

    if (con->ver_num >= OCI_10_1)
    {
        bstr1 = OCI_GetMetaFromDataString(queue_table,     &bsize1);
        bstr2 = OCI_GetMetaFromDataString(purge_condition, &bsize2);

        if (bstr2 == NULL)
            bstr2 = null_str;

        st = OCI_StatementCreate(con);

        if (st)
        {
            res = OCI_Prepare
                  (
                st,
                MT("DECLARE ")
                MT("    v_purge_options DBMS_AQADM.AQ$_PURGE_OPTIONS_T; ")
                MT("    v_block         BOOLEAN := FALSE; ")
                MT("BEGIN ")
                MT("    v_purge_options.block         := FALSE; ")
                MT("    v_purge_options.delivery_mode := :delivery_mode; ")
                MT("    IF (:block = 1) then ")
                MT("        v_purge_options.block := TRUE; ")
                MT("    END IF; ")
                MT("    DBMS_AQADM.PURGE_QUEUE_TABLE ")
                MT("    (")
                MT("        queue_table      => :queue_table, ")
                MT("        purge_condition  => :purge_condition, ")
                MT("        purge_options    => v_purge_options ")
                MT("   ); ")
                MT("END; ")
                  );

            res = res && OCI_BindString(st, MT(":queue_table"), bstr1, 0);
            res = res && OCI_BindString(st, MT(":purge_condition"), bstr2, 0);
            res = res && OCI_BindInt(st, MT(":block"),  &block);
            res = res && OCI_BindUnsignedInt(st, MT(":delivery_mode"),  &delivery_mode);

            res = res && OCI_Execute(st);

            OCI_StatementFree(st);
        }

        OCI_ReleaseDataString(bstr1);

        if (purge_condition != NULL)
            OCI_ReleaseDataString(bstr2);
    }
    else
    {
        res = TRUE;
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_QueueTableMigrate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_QueueTableMigrate
(
    OCI_Connection *con,
    const mtext    *queue_table,
    const mtext    *compatible
)
{
    boolean res       = FALSE;
    OCI_Statement *st = NULL;
    void *bstr1       = NULL, *bstr2 = NULL;
    int bsize1        = -1, bsize2 = -1;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, queue_table, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, compatible, FALSE);

    bstr1 = OCI_GetMetaFromDataString(queue_table, &bsize1);
    bstr2 = OCI_GetMetaFromDataString(compatible,  &bsize2);

    st = OCI_StatementCreate(con);

    if (st)
    {
        res = OCI_Prepare
              (
            st,
            MT("BEGIN ")
            MT("   DBMS_AQADM.MIGRATE_QUEUE_TABLE")
            MT("   (")
            MT("       queue_table => :queue_table, ")
            MT("       compatible  => :compatible ")
            MT("   );")
            MT("END;")
              );

        res = res && OCI_BindString(st, MT(":queue_table"), bstr1, 0);
        res = res && OCI_BindString(st, MT(":compatible"),  bstr2, 0);

        res = res && OCI_Execute(st);

        OCI_StatementFree(st);
    }

    OCI_ReleaseDataString(bstr1);
    OCI_ReleaseDataString(bstr2);

    OCI_RESULT(res);

    return res;
}
