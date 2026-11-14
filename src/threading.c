/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@gmail.com>
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

#include "threading.h"

#if defined(OCILIB_POSIX_THREADS)

/* --------------------------------------------------------------------------------------------- *
 * Mutex
 * --------------------------------------------------------------------------------------------- */

int OcilibThreadsMtxInit
(
    mtx_t *mutex,
    int    type
)
{
    pthread_mutexattr_t attr;

    if (0 != pthread_mutexattr_init(&attr))
    {
        return thrd_error;
    }

    if (0 != pthread_mutexattr_settype(&attr, (mtx_recursive == type)
                                                ? PTHREAD_MUTEX_RECURSIVE
                                                : PTHREAD_MUTEX_NORMAL))
    {
        pthread_mutexattr_destroy(&attr);
        return thrd_error;
    }

    const int rc = pthread_mutex_init(mutex, &attr);

    pthread_mutexattr_destroy(&attr);

    return (0 == rc) ? thrd_success : thrd_error;
}

int OcilibThreadsMtxLock
(
    mtx_t *mutex
)
{
    return (0 == pthread_mutex_lock(mutex)) ? thrd_success : thrd_error;
}

int OcilibThreadsMtxUnlock
(
    mtx_t *mutex
)
{
    return (0 == pthread_mutex_unlock(mutex)) ? thrd_success : thrd_error;
}

int OcilibThreadsMtxTrylock
(
    mtx_t *mutex
)
{
    return (0 == pthread_mutex_trylock(mutex)) ? thrd_success : thrd_error;
}

void OcilibThreadsMtxDestroy
(
    mtx_t *mutex
)
{
    pthread_mutex_destroy(mutex);
}

/* --------------------------------------------------------------------------------------------- *
 * Thread-specific storage
 * --------------------------------------------------------------------------------------------- */

int OcilibThreadsTssCreate
(
    tss_t     *key,
    tss_dtor_t dtor
)
{
    return (0 == pthread_key_create(key, dtor)) ? thrd_success : thrd_error;
}

void * OcilibThreadsTssGet
(
    tss_t key
)
{
    return pthread_getspecific(key);
}

int OcilibThreadsTssSet
(
    tss_t key,
    void *value
)
{
    return (0 == pthread_setspecific(key, value)) ? thrd_success : thrd_error;
}

void OcilibThreadsTssDelete
(
    tss_t key
)
{
    pthread_key_delete(key);
}

/* --------------------------------------------------------------------------------------------- *
 * One-time initialisation
 * --------------------------------------------------------------------------------------------- */

void OcilibThreadsCallOnce
(
    once_flag *flag,
    void     (*func)
    (
        void
    )
)
{
    pthread_once(flag, func);
}

#endif /* OCILIB_POSIX_THREADS */
