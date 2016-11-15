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
    OCI_TypeInfo   *typinf,
    OCI_Ref        *ref,
    void           *handle
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_ALLOCATE_DATA(OCI_IPC_REF, ref, 1);

    if (OCI_STATUS)
    {
        ref->handle = handle;
        ref->con    = con;
        ref->typinf = typinf;

        if (!ref->handle || (OCI_OBJECT_ALLOCATED_ARRAY == ref->hstate))
        {
            /* allocates handle for non fetched object */

            if (OCI_OBJECT_ALLOCATED_ARRAY != ref->hstate)
            {
                ref->hstate = OCI_OBJECT_ALLOCATED;
            }

            OCI_EXEC
            (
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

    if (!OCI_STATUS && ref)
    {
        OCI_RefFree(ref);
        ref = NULL;
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
    void *obj_handle = NULL;

    OCI_CALL_DECLARE_CONTEXT(TRUE)

    OCI_CHECK(NULL == ref, FALSE)

    OCI_CALL_CONTEXT_SET_FROM_CONN(ref->con);

    OCI_RefUnpin(ref);

    OCI_EXEC
    (
        OCIObjectPin(ref->con->env, ref->con->err, ref->handle, (OCIComplexObject *) 0, 
                     OCI_PIN_ANY, OCI_DURATION_SESSION, OCI_LOCK_NONE, &obj_handle)
    )

    if (OCI_STATUS)
    {
        ref->obj = OCI_ObjectInit(ref->con, (OCI_Object *) ref->obj, obj_handle, ref->typinf, NULL, -1, TRUE);

        OCI_STATUS = ref->pinned = (NULL != ref->obj);
    }

    return OCI_STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RefUnpin
 * --------------------------------------------------------------------------------------------- */

boolean OCI_RefUnpin
(
    OCI_Ref *ref
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)

    OCI_CHECK(NULL == ref, FALSE)
    OCI_CHECK(NULL == ref->obj, FALSE)

    OCI_CALL_CONTEXT_SET_FROM_CONN(ref->con);

    if (ref->pinned)
    {
        OCI_EXEC(OCIObjectUnpin(ref->con->env, ref->con->err, ref->obj->handle))

        ref->pinned = FALSE;
    }

    if (ref->obj)
    {
        ref->obj->hstate = OCI_OBJECT_FETCHED_DIRTY;
        OCI_ObjectFree(ref->obj);
        ref->obj = NULL;
    }

    return OCI_STATUS;
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
    OCI_CALL_ENTER(OCI_Ref *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_RETVAL = OCI_RefInit(con, typinf, NULL, NULL);
    OCI_STATUS = (NULL != OCI_RETVAL);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RefFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RefFree
(
    OCI_Ref *ref
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_REF, ref)
    OCI_CALL_CHECK_OBJECT_FETCHED(ref)
    OCI_CALL_CONTEXT_SET_FROM_CONN(ref->con)

    OCI_RefUnpin(ref);

    if ((OCI_OBJECT_ALLOCATED == ref->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == ref->hstate))
    {
        OCI_OCIObjectFree(ref->con->env, ref->con->err, ref->handle, OCI_DEFAULT);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != ref->hstate)
    {
        OCI_FREE(ref)
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(OCI_Ref **, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_REF, 0, sizeof(OCIRef *), sizeof(OCI_Ref), 0, typinf);
    OCI_STATUS = (NULL != arr);

    if (OCI_STATUS)
    {
        OCI_RETVAL = (OCI_Ref **) arr->tab_obj;
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RefArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RefArrayFree
(
    OCI_Ref **refs
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_ARRAY, refs)

    OCI_RETVAL = OCI_STATUS = OCI_ArrayFreeFromHandles((void **)refs);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RefGetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OCI_API OCI_RefGetObject
(
    OCI_Ref *ref
)
{
    OCI_CALL_ENTER(OCI_Object*, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_REF, ref)
    OCI_CALL_CONTEXT_SET_FROM_CONN(ref->con)

    if (!OCI_RefIsNull(ref))
    {
        OCI_STATUS = OCI_RefPin(ref);

        if (OCI_STATUS)
        {
            OCI_RETVAL = ref->obj;
        }
    }

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_REF, ref)
    OCI_CALL_CHECK_PTR(OCI_IPC_REF, ref_src)
    OCI_CALL_CHECK_COMPAT(ref->con, ref->typinf->tdo == ref_src->typinf->tdo)
    OCI_CALL_CONTEXT_SET_FROM_CONN(ref->con)

    OCI_EXEC(OCIRefAssign(ref->con->env, ref->con->err, ref_src->handle, &ref->handle))

    if (OCI_STATUS)
    {
        if (ref->obj)
        {
            OCI_ObjectFree(ref->obj);
            ref->obj = NULL;
        }

        ref->typinf = ref_src->typinf;
        ref->pinned = ref_src->pinned;
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RefIsNull
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RefIsNull
(
    OCI_Ref *ref
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_REF, ref)
    OCI_CALL_CONTEXT_SET_FROM_CONN(ref->con)

    OCI_RETVAL = OCIRefIsNull(ref->con->env, ref->handle);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RefSetNull
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RefSetNull
(
    OCI_Ref *ref
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_REF, ref)
    OCI_CALL_CONTEXT_SET_FROM_CONN(ref->con)

    OCI_STATUS = OCI_RefUnpin(ref);

    if (OCI_STATUS)
    {
        OCIRefClear(ref->con->env, ref->handle);

        if (ref->obj)
        {
            OCI_ObjectFree(ref->obj);
            ref->obj = NULL;
        }
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_REF, ref)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, str)
    OCI_CALL_CONTEXT_SET_FROM_CONN(ref->con)

    /* initialize output buffer in case of OCI failure */

    str[0] = 0;

    dbstr = OCI_StringGetOracleString(str, &dbsize);

    OCI_EXEC(OCIRefToHex(ref->con->env, ref->con->err, ref->handle, (OraText *) dbstr, (ub4 *) &dbsize))

    OCI_StringCopyOracleStringToNativeString(dbstr, str, dbcharcount(dbsize));
    OCI_StringReleaseOracleString(dbstr);

    /* set null string terminator */

    str[dbcharcount(dbsize)] = 0;

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(unsigned int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_REF, ref)
    OCI_CALL_CONTEXT_SET_FROM_CONN(ref->con)

    size = OCIRefHexSize(ref->con->env, (const OCIRef *)ref->handle) / (ub4) sizeof(dbtext);

    OCI_RETVAL = size;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollRefGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * OCI_API OCI_RefGetTypeInfo
(
    OCI_Ref *ref
)
{
    OCI_GET_PROP(OCI_TypeInfo*, NULL, OCI_IPC_REF, ref, typinf, ref->con, NULL, ref->con->err)
}

