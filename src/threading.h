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

/*
 * Internal threading abstraction.
 *
 * OCILIB uses a small C11 <threads.h> surface: mtx_t, tss_t and call_once().
 * C11 threads are an OPTIONAL feature; several advertised platforms do not
 * ship the header (macOS SDK, older glibc/toolchains, MinGW, embedded).
 *
 * Selection order:
 *   1. Windows: require MSVC >= 17.8 (VS2022), which ships <threads.h>.
 *   2. Any toolchain where <threads.h> is found via __has_include.
 *   3. POSIX fallback implemented over pthreads (see threading.c).
 *
 * An explicit escape hatch is provided for cross-compiles where
 * __has_include reports wrong results:
 *   -DOCILIB_NO_C11_THREADS  forces the POSIX/pthread implementation.
 */

#ifndef OCILIB_THREADING_H_INCLUDED
#define OCILIB_THREADING_H_INCLUDED

#if defined(_WIN32) || defined(_WIN64)

#  if defined(_MSC_VER)

    /* C11 <threads.h> requires MSVC 17.8 (VS2022) or newer — ocilib ships
       Windows binaries built with VS2026, so a lower floor is a hard error
       rather than a shim. */

#    if _MSC_VER < 1938
#      error "OCILIB requires MSVC 17.8+ (Visual Studio 2022) for C11 <threads.h> on Windows"
#    endif

    #include <threads.h>

#  endif

    /* non-MSVC Windows toolchains (MinGW, clang-cl without MSVC STL) fall
       through to the feature detection below — MinGW-w64 resolves to the
       pthread shim via winpthreads. */

#endif

#if !defined(_WIN32) || !defined(_WIN64) || !defined(_MSC_VER)

#  if !defined(OCILIB_NO_C11_THREADS) && \
      defined(__has_include) && __has_include(<threads.h>)

    #include <threads.h>

#  else

    /* POSIX fallback over pthreads (implementations live in threading.c). */

    #define OCILIB_POSIX_THREADS 1

    #include <pthread.h>

    /* status codes (mirror C11 values ordering: any nonzero means error) */

    #define thrd_success  0
    #define thrd_error    1
    #define thrd_busy     2
    #define thrd_timedout 3
    #define thrd_nomem    4

    /* ---- mutex ------------------------------------------------------------ */

    typedef pthread_mutex_t mtx_t;

        /* C11 kind selectors passed to mtx_init() */

    #define mtx_plain     0
    #define mtx_timed     1
    #define mtx_recursive 2

    /* ---- thread-specific storage ------------------------------------------ */

    typedef pthread_key_t tss_t;
    typedef void (*tss_dtor_t)(void *);

    /* ---- one-time initialisation ------------------------------------------ */

    typedef pthread_once_t once_flag;

    #define ONCE_FLAG_INIT  PTHREAD_ONCE_INIT

    /* ---- POSIX implementations (see threading.c) --------------------------- */

    int   OcilibThreadsMtxInit(mtx_t *mutex, int type);
    int   OcilibThreadsMtxLock(mtx_t *mutex);
    int   OcilibThreadsMtxUnlock(mtx_t *mutex);
    int   OcilibThreadsMtxTrylock(mtx_t *mutex);
    void  OcilibThreadsMtxDestroy(mtx_t *mutex);

    int   OcilibThreadsTssCreate(tss_t *key, tss_dtor_t dtor);
    void *OcilibThreadsTssGet(tss_t key);
    int   OcilibThreadsTssSet(tss_t key, void *value);
    void  OcilibThreadsTssDelete(tss_t key);

    void  OcilibThreadsCallOnce(once_flag *flag, void (*func)(void));

    #define mtx_init(m, t)   OcilibThreadsMtxInit((m), (t))
    #define mtx_lock(m)      OcilibThreadsMtxLock(m)
    #define mtx_unlock(m)    OcilibThreadsMtxUnlock(m)
    #define mtx_trylock(m)   OcilibThreadsMtxTrylock(m)
    #define mtx_destroy(m)   OcilibThreadsMtxDestroy(m)

    #define tss_create(k, d) OcilibThreadsTssCreate((k), (d))
    #define tss_get(k)       OcilibThreadsTssGet(k)
    #define tss_set(k, v)    OcilibThreadsTssSet((k), (v))
    #define tss_delete(k)    OcilibThreadsTssDelete(k)

    #define call_once(f, fn) OcilibThreadsCallOnce((f), (fn))

#  endif /* OCILIB_NO_C11_THREADS / __has_include(<threads.h>) */

#endif /* !Windows || non-MSVC */

#endif /* OCILIB_THREADING_H_INCLUDED */
