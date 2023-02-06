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

inline void QueueTable::Create(const Connection &connection, const ostring& table, const ostring& payloadType, bool multipleConsumers, const ostring& storageClause, const ostring& sortList,
                   GroupingMode groupingMode, const ostring& comment, unsigned int primaryInstance, unsigned int secondaryInstance, const ostring& compatible)

{
    core::Check(OCI_QueueTableCreate(connection, table.c_str(), payloadType.c_str(), storageClause.c_str(), sortList.c_str(), multipleConsumers,
                               groupingMode, comment.c_str(), primaryInstance, secondaryInstance, compatible.c_str()));
}

inline void QueueTable::Alter(const Connection &connection, const ostring& table, const ostring& comment, unsigned int primaryInstance, unsigned int secondaryInstance)
{
    core::Check(OCI_QueueTableAlter(connection, table.c_str(), comment.c_str(), primaryInstance, secondaryInstance));
}

inline void QueueTable::Drop(const Connection &connection, const ostring& table, bool force)
{
    core::Check(OCI_QueueTableDrop(connection, table.c_str(), force));
}

inline void QueueTable::Purge(const Connection &connection, const ostring& table, PurgeMode mode, const ostring& condition, bool block)
{
    core::Check(OCI_QueueTablePurge(connection, table.c_str(), condition.c_str(), block, static_cast<unsigned int>(mode)));
}

inline void QueueTable::Migrate(const Connection &connection, const ostring& table, const ostring& compatible)
{
    core::Check(OCI_QueueTableMigrate(connection, table.c_str(), compatible.c_str()));
}

}
