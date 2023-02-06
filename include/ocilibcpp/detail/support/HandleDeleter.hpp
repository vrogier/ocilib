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
            bool operator()(T handle) const { return true; }
        };

        template<> struct HandleDeleter<OCI_Connection*>
        {
            bool operator()(OCI_Connection* handle) const { return OCI_ConnectionFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Pool*>
        {
            bool operator()(OCI_Pool* handle) const { return OCI_PoolFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Transaction*>
        {
            bool operator()(OCI_Transaction* handle) const { return OCI_TransactionFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Number*>
        {
            bool operator()(OCI_Number* handle) const { return OCI_NumberFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Date*>
        {
            bool operator()(OCI_Date* handle) const { return OCI_DateFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Interval*>
        {
            bool operator()(OCI_Interval* handle) const { return OCI_IntervalFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Timestamp*>
        {
            bool operator()(OCI_Timestamp* handle) const { return OCI_TimestampFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Lob*>
        {
            bool operator()(OCI_Lob* handle) const { return OCI_LobFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Long*>
        {
            bool operator()(OCI_Long* handle) const { return OCI_LongFree(handle); }
        };
    
        template<> struct HandleDeleter<OCI_File*>
        {
            bool operator()(OCI_File* handle) const { return OCI_FileFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Coll*>
        {
            bool operator()(OCI_Coll* handle) const { return OCI_CollFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Object*>
        {
            bool operator()(OCI_Object* handle) const { return OCI_ObjectFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Ref*>
        {
            bool operator()(OCI_Ref* handle) const { return OCI_RefFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Statement*>
        {
            bool operator()(OCI_Statement* handle) const { return OCI_StatementFree(handle); }
        };     

        template<> struct HandleDeleter<OCI_Subscription*>
        {
            bool operator()(OCI_Subscription* handle) const { return OCI_SubscriptionUnregister(handle); }
        };

        template<> struct HandleDeleter<OCI_Agent*>
        {
            bool operator()(OCI_Agent* handle) const { return OCI_AgentFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Msg*>
        {
            bool operator()(OCI_Msg* handle) const { return OCI_MsgFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Enqueue*>
        {
            bool operator()(OCI_Enqueue* handle) const { return OCI_EnqueueFree(handle); }
        };

        template<> struct HandleDeleter<OCI_Dequeue*>
        {
            bool operator()(OCI_Dequeue* handle) const { return OCI_DequeueFree(handle); }
        };

        template<> struct HandleDeleter<OCI_DirPath*>
        {
            bool operator()(OCI_DirPath* handle) const { return OCI_DirPathFree(handle); }
        };
    }
}
