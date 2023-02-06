/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2023 Vincent ROGIER <vince.rogier@ocilib.net>
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

#pragma once

#include "ocilibcpp/types.hpp"

namespace ocilib
{

inline void Queue::Create(const Connection &connection, const ostring& queue, const ostring& table, QueueType queueType, unsigned int maxRetries,
            unsigned int retryDelay, unsigned int retentionTime, bool dependencyTracking, const ostring& comment)
{
    core::Check(OCI_QueueCreate(connection, queue.c_str(), table.c_str(), queueType, maxRetries, retryDelay, retentionTime, dependencyTracking, comment.c_str()));
}

inline void Queue::Alter(const Connection &connection, const ostring& queue, unsigned int maxRetries, unsigned int retryDelay, unsigned int retentionTime, const ostring& comment)
{
    core::Check(OCI_QueueAlter(connection, queue.c_str(), maxRetries, retryDelay, retentionTime, comment.c_str()));
}

inline void Queue::Drop(const Connection &connection, const ostring& queue)
{
    core::Check(OCI_QueueDrop(connection, queue.c_str()));
}

inline void Queue::Start(const Connection &connection, const ostring& queue, bool enableEnqueue, bool enableDequeue)
{
    core::Check(OCI_QueueStart(connection, queue.c_str(), enableEnqueue, enableDequeue));
}

inline void Queue::Stop(const Connection &connection, const ostring& queue, bool stopEnqueue, bool stopDequeue, bool wait)
{
    core::Check(OCI_QueueStop(connection, queue.c_str(), stopEnqueue, stopDequeue, wait));
}

}
