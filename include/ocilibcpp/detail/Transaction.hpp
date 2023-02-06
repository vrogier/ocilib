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

// ReSharper disable CppClangTidyPerformanceUnnecessaryValueParam

namespace ocilib
{

inline Transaction::Transaction(const Connection &connection, unsigned int timeout, TransactionFlags flags, OCI_XID *pxid)
{
    AcquireAllocated
    (
        core::Check(OCI_TransactionCreate(connection, timeout, flags.GetValues(), pxid)), 
        Environment::GetEnvironmentHandle()
    );
}

inline Transaction::Transaction(OCI_Transaction *trans)
{
    AcquireTransient(trans, Environment::GetEnvironmentHandle());
}

inline void Transaction::Prepare()
{
    core::Check(OCI_TransactionPrepare(*this));
}

inline void Transaction::Start()
{
    core::Check(OCI_TransactionStart(*this));
}

inline void Transaction::Stop()
{
    core::Check(OCI_TransactionStop(*this));
}

inline void Transaction::Resume()
{
    core::Check(OCI_TransactionResume(*this));
}

inline void Transaction::Forget()
{
    core::Check(OCI_TransactionForget(*this));
}

inline Transaction::TransactionFlags Transaction::GetFlags() const
{
    return TransactionFlags(static_cast<TransactionFlags::Type>(core::Check(OCI_TransactionGetMode(*this))));
}

inline unsigned int Transaction::GetTimeout() const
{
    return core::Check(OCI_TransactionGetTimeout(*this));
}

}
