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
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_RefInit
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * OCI_RefInit
(
    OCI_Connection *con,
    OCI_TypeInfo  **typinf,
    OCI_Ref       **pref,
    void           *handle
)
{
    boolean  res = FALSE;
    OCI_Ref *ref = NULL;

    OCI_CHECK(NULL == pref, NULL);

    if (!*pref)
    {
        *pref = (OCI_Ref *) OCI_MemAlloc(OCI_IPC_REF, sizeof(*ref), (size_t) 1, TRUE);
    }

    if (*pref)
    {
        res = TRUE;

        ref = *pref;

        ref->handle = handle;
        ref->con    = con;
        ref->typinf = typinf ? *typinf : NULL;

        if (!ref->handle || (OCI_OBJECT_ALLOCATED_ARRAY == ref->hstate))
        {
            /* allocates handle for non fetched object */

            if (OCI_OBJECT_ALLOCATED_ARRAY != ref->hstate)
            {
                ref->hstate = OCI_OBJECT_ALLOCATED;
            }

            OCI_CALL2
            (
                res, ref->con,

                OCI_ObjectNew(ref->con->env, ref->con->err, ref->con->cxt,
                              (OCITypeCode) SQLT_REF,
                              (OCIType*) NULL,
                              (dvoid *) NULL,
                              (OCIDuration) OCI_DURATION_SESSION,
                              (boolean) FALSE,
                              (dvoid **) &ref->handle)
            )
        }
        else
        {
            ref->hstate = OCI_OBJECT_FETCHED_CLEAN;

            OCI_RefUnpin(ref);
        }
    }

    /* check for failure */

    if (!res && ref)
    {
        OCI_RefFree(ref);
        *pref = ref = NULL;
    }

    return ref;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RefPin
 * --------------------------------------------------------------------------------------------- */

boolean OCI_RefPin
(
    OCI_Ref *ref
)
{
    boolean res      = TRUE;
    void *obj_handle = NULL;

    OCI_CHECK(NULL == ref, FALSE)

    OCI_RefUnpin(ref);

    OCI_CALL2
    (
        res, ref->con,

        OCIObjectPin(ref->con->env, ref->con->err, ref->handle,
                     (OCIComplexObject *) 0, OCI_PIN_ANY, OCI_DURATION_SESSION,
                     OCI_LOCK_NONE, &obj_handle)
    )

    if (res)
    {
        OCI_Object *obj = OCI_ObjectInit(ref->con, (OCI_Object **) &ref->obj, obj_handle,
                                         ref->typinf, NULL, -1, TRUE);

        res = ref->pinned = (NULL != obj);
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RefUnpin
 * --------------------------------------------------------------------------------------------- */

boolean OCI_RefUnpin
(
    OCI_Ref *ref
)
{
    boolean res = TRUE;

    OCI_CHECK(NULL == ref, FALSE)
    OCI_CHECK(NULL == ref->obj, FALSE)

    if (ref->pinned)
    {
        OCI_CALL2
        (
            res, ref->con,

            OCIObjectUnpin(ref->con->env, ref->con->err, ref->obj->handle)
        )

        ref->pinned = FALSE;
    }

    if (ref->obj)
    {
        ref->obj->hstate = OCI_OBJECT_FETCHED_DIRTY;
        OCI_ObjectFree(ref->obj);
        ref->obj = NULL;
    }

    return res;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_RefCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * OCI_API OCI_RefCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf
)
{
    OCI_LIB_CALL_ENTER(OCI_Ref *, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)

    call_retval = OCI_RefInit(con, &typinf, &call_retval, NULL);
    call_status = (NULL != call_retval);

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RefFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RefFree
(
    OCI_Ref *ref
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_REF, ref)
    OCI_CHECK_OBJECT_FETCHED(ref)

    OCI_RefUnpin(ref);

    if ((OCI_OBJECT_ALLOCATED == ref->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == ref->hstate))
    {
        OCI_OCIObjectFree(ref->con->env, ref->con->err, ref->handle, OCI_DEFAULT);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != ref->hstate)
    {
        OCI_FREE(ref)
    }

    call_retval = call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RefArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Ref ** OCI_API OCI_RefArrayCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
)
{
    OCI_Array *arr = NULL;

    OCI_LIB_CALL_ENTER(OCI_Ref **, NULL)
    
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, con)

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_REF, 0, sizeof(OCIRef *), sizeof(OCI_Ref), 0, typinf);

    if (arr)
    {
        call_retval = (OCI_Ref **) arr->tab_obj;
        call_status = TRUE;
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RefArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RefArrayFree
(
    OCI_Ref **refs
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ARRAY, refs)

    call_retval = call_status = OCI_ArrayFreeFromHandles((void **)refs);

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RefGetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OCI_API OCI_RefGetObject
(
    OCI_Ref *ref
)
{
    OCI_LIB_CALL_ENTER(OCI_Object*, NULL)

    OCI_CHECK_PTR(OCI_IPC_REF, ref)

    call_status = TRUE;

    if (!OCI_RefIsNull(ref))
    {
        call_status = OCI_RefPin(ref);

        if (call_status)
        {
            call_retval = ref->obj;
        }
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RefAssign
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RefAssign
(
    OCI_Ref *ref,
    OCI_Ref *ref_src
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_REF, ref)
    OCI_CHECK_PTR(OCI_IPC_REF, ref_src)
    OCI_CHECK_COMPAT(ref->con, ref->typinf->tdo == ref_src->typinf->tdo)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, ref->con,

        OCIRefAssign(ref->con->env, ref->con->err, ref_src->handle, &ref->handle)
    )

    if (call_status)
    {
        if (ref->obj)
        {
            OCI_ObjectFree(ref->obj);
            ref->obj = NULL;
        }

        ref->typinf = ref_src->typinf;
        ref->pinned = ref_src->pinned;
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RefIsNull
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RefIsNull
(
    OCI_Ref *ref
)
{
    OCI_LIB_CALL_ENTER(boolean, TRUE)

    OCI_CHECK_PTR(OCI_IPC_REF, ref)

    call_retval = OCIRefIsNull(ref->con->env, ref->handle);
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RefSetNull
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RefSetNull
(
    OCI_Ref *ref
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_REF, ref)

    call_status = OCI_RefUnpin(ref);

    if (call_status)
    {
        OCIRefClear(ref->con->env, ref->handle);

        if (ref->obj)
        {
            OCI_ObjectFree(ref->obj);
            ref->obj = NULL;
        }
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RefToText
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RefToText
(
    OCI_Ref     *ref,
    unsigned int size,
    otext       *str
)
{
    dbtext *dbstr  = NULL;
    int     dbsize = (int) size * (int) sizeof(otext);

    OCI_LIB_CALL_ENTER(boolean, FALSE)
    
    OCI_CHECK_PTR(OCI_IPC_REF, ref)
    OCI_CHECK_PTR(OCI_IPC_STRING, str)

    call_status = TRUE;

    /* initialize output buffer in case of OCI failure */

    str[0] = 0;

    dbstr = OCI_StringGetOracleString(str, &dbsize);

    OCI_CALL2
    (
        call_status, ref->con,

        OCIRefToHex(ref->con->env, ref->con->err, ref->handle,
                    (OraText *) dbstr, (ub4 *) &dbsize)
    )

    OCI_StringCopyOracleStringToNativeString(dbstr, str, dbcharcount(dbsize));
    OCI_StringReleaseOracleString(dbstr);

    /* set null string terminator */

    str[dbcharcount(dbsize)] = 0;

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RefGetHexSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_RefGetHexSize
(
    OCI_Ref *ref
)
{
    ub4 size = 0;

    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_REF, ref)

    size = OCIRefHexSize(ref->con->env, (const OCIRef *) ref->handle);

    size /= (ub4) sizeof(dbtext);

    call_retval = size;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollRefGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * OCI_API OCI_RefGetTypeInfo
(
    OCI_Ref *ref
)
{
    OCI_LIB_CALL_ENTER(OCI_TypeInfo*, NULL)

    OCI_CHECK_PTR(OCI_IPC_REF, ref)

    call_retval = ref->typinf;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

