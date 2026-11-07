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

#pragma once

#include "ocilibcpp/config.hpp"
#include "ocilibcpp/core.hpp"

namespace ocilib
{
    namespace support
    {
        /**
        * @brief Internal usage.
        * Allow resolving a deleter for C handles
        */
        template<class T>
        struct HandleDeleter
        {
            bool operator()([[maybe_unused]] T handle, AnyPointer) const { return true; }
        };

        template<> struct HandleDeleter<OCI_Environment*>
        {
            bool operator()(OCI_Environment* handle, AnyPointer) const { return OCI_EnvironmentFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Connection*>
        {
            bool operator()(OCI_Connection* handle, AnyPointer) const { return OCI_ConnectionFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Pool*>
        {
            bool operator()(OCI_Pool* handle, AnyPointer env) const { return OCI_PoolFree(static_cast<OCI_Environment*>(env), handle); }
        };

        template<> struct HandleDeleter<OCI_Transaction*>
        {
            bool operator()(OCI_Transaction* handle, AnyPointer) const { return OCI_TransactionFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Number*>
        {
            bool operator()(OCI_Number* handle, AnyPointer) const { return OCI_NumberFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Date*>
        {
            bool operator()(OCI_Date* handle, AnyPointer) const { return OCI_DateFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Interval*>
        {
            bool operator()(OCI_Interval* handle, AnyPointer) const { return OCI_IntervalFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Timestamp*>
        {
            bool operator()(OCI_Timestamp* handle, AnyPointer) const { return OCI_TimestampFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Lob*>
        {
            bool operator()(OCI_Lob* handle, AnyPointer) const { return OCI_LobFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Long*>
        {
            bool operator()(OCI_Long* handle, AnyPointer) const { return OCI_LongFree(handle); }
        };

        template<> struct HandleDeleter<OCI_File*>
        {
            bool operator()(OCI_File* handle, AnyPointer) const { return OCI_FileFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Coll*>
        {
            bool operator()(OCI_Coll* handle, AnyPointer) const { return OCI_CollFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Object*>
        {
            bool operator()(OCI_Object* handle, AnyPointer) const { return OCI_ObjectFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Ref*>
        {
            bool operator()(OCI_Ref* handle, AnyPointer) const { return OCI_RefFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Vector*>
        {
            bool operator()(OCI_Vector* handle, AnyPointer) const { return OCI_VectorFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Json*>
        {
            bool operator()(OCI_Json* handle, AnyPointer) const { return OCI_JsonFree(handle); }
        };

        template<> struct HandleDeleter<OCI_JsonNode*>
        {
            bool operator()(OCI_JsonNode* handle, AnyPointer) const { return OCI_JsonNodeFree(handle); }
        };

        template<> struct HandleDeleter<OCI_XmlType*>
        {
            bool operator()([[maybe_unused]] OCI_XmlType* handle, AnyPointer) const { return true; }
        };

        template<> struct HandleDeleter<OCI_Statement*>
        {
            bool operator()(OCI_Statement* handle, AnyPointer) const { return OCI_StatementFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Subscription*>
        {
            bool operator()(OCI_Subscription* handle, AnyPointer) const { return OCI_SubscriptionUnregister(handle); }
        };

        template<> struct HandleDeleter<OCI_Agent*>
        {
            bool operator()(OCI_Agent* handle, AnyPointer) const { return OCI_AgentFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Msg*>
        {
            bool operator()(OCI_Msg* handle, AnyPointer) const { return OCI_MsgFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Enqueue*>
        {
            bool operator()(OCI_Enqueue* handle, AnyPointer) const { return OCI_EnqueueFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Dequeue*>
        {
            bool operator()(OCI_Dequeue* handle, AnyPointer) const { return OCI_DequeueFree(handle); }
        };

        template<> struct HandleDeleter<OCI_DirPath*>
        {
            bool operator()(OCI_DirPath* handle, AnyPointer) const { return OCI_DirPathFree(handle); }
        };

        template<> struct HandleDeleter<OCI_SodaDb*>
        {
            bool operator()(OCI_SodaDb* handle, AnyPointer) const { return OCI_SodaDbFree(handle); }
        };

        template<> struct HandleDeleter<OCI_ShardingKey*>
        {
            bool operator()(OCI_ShardingKey* handle, AnyPointer) const { return OCI_ShardingKeyFree(handle); }
        };

        template<> struct HandleDeleter<OCI_SodaColl*>
        {
            bool operator()(OCI_SodaColl* handle, AnyPointer) const { return OCI_SodaCollFree(handle); }
        };

        template<> struct HandleDeleter<OCI_SodaDoc*>
        {
            bool operator()(OCI_SodaDoc* handle, AnyPointer) const { return OCI_SodaDocFree(handle); }
        };

        template<> struct HandleDeleter<OCI_SodaOptions*>
        {
            bool operator()(OCI_SodaOptions* handle, AnyPointer) const { return OCI_SodaOptionsFree(handle); }
        };

        template<> struct HandleDeleter<OCI_SodaCollCursor*>
        {
            bool operator()(OCI_SodaCollCursor* handle, AnyPointer) const { return OCI_SodaCollCursorFree(handle); }
        };

        template<> struct HandleDeleter<OCI_SodaDocCursor*>
        {
            bool operator()(OCI_SodaDocCursor* handle, AnyPointer) const { return OCI_SodaDocCursorFree(handle); }
        };
    }
}
