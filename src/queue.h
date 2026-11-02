/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@ocilib.net>
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

#ifndef OCILIB_QUEUE_H_INCLUDED
#define OCILIB_QUEUE_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Boolean OcilibQueueCreate
(
    OCI_Connection *con,
    const otext    *queue_name,
    const otext    *queue_table,
    OCI_QueueType   queue_type,
    OCI_UInt32    max_retries,
    OCI_UInt32    retry_delay,
    OCI_UInt32    retention_time,
    OCI_Boolean     dependency_tracking,
    const otext    *comment
);

OCI_SYM_LOCAL OCI_Boolean OcilibQueueAlter
(
    OCI_Connection *con,
    const otext    *queue_name,
    OCI_UInt32    max_retries,
    OCI_UInt32    retry_delay,
    OCI_UInt32    retention_time,
    const otext    *comment
);

OCI_SYM_LOCAL OCI_Boolean OcilibQueueDrop
(
    OCI_Connection *con,
    const otext    *queue_name
);

OCI_SYM_LOCAL OCI_Boolean OcilibQueueStart
(
    OCI_Connection *con,
    const otext    *queue_name,
    OCI_Boolean     enqueue,
    OCI_Boolean     dequeue
);

OCI_SYM_LOCAL OCI_Boolean OcilibQueueStop
(
    OCI_Connection *con,
    const otext    *queue_name,
    OCI_Boolean     enqueue,
    OCI_Boolean     dequeue,
    OCI_Boolean     wait
);

OCI_SYM_LOCAL OCI_Boolean OcilibQueueTableCreate
(
    OCI_Connection   *con,
    const otext      *queue_table,
    const otext      *queue_payload_type,
    const otext      *storage_clause,
    const otext      *sort_list,
    OCI_Boolean       multiple_consumers,
    OCI_QueueGrouping message_grouping,
    const otext      *comment,
    OCI_UInt32      primary_instance,
    OCI_UInt32      secondary_instance,
    const otext      *compatible
);

OCI_SYM_LOCAL OCI_Boolean OcilibQueueTableAlter
(
    OCI_Connection *con,
    const otext    *queue_table,
    const otext    *comment,
    OCI_UInt32    primary_instance,
    OCI_UInt32    secondary_instance
);

OCI_SYM_LOCAL OCI_Boolean OcilibQueueTableDrop
(
    OCI_Connection *con,
    const otext    *queue_table,
    OCI_Boolean     force
);

OCI_SYM_LOCAL OCI_Boolean OcilibQueueTablePurge
(
    OCI_Connection    *con,
    const otext       *queue_table,
    const otext       *purge_condition,
    OCI_Boolean        block,
    OCI_QueuePurgeMode delivery_mode
);

OCI_SYM_LOCAL OCI_Boolean OcilibQueueTableMigrate
(
    OCI_Connection *con,
    const otext    *queue_table,
    const otext    *compatible
);

#endif /* OCILIB_QUEUE_H_INCLUDED */
