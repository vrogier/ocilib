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
 *                             PRIVATE VARIABLES
 * ********************************************************************************************* */

static unsigned int CharsetFormValues[]   = { OCI_CSF_DEFAULT, OCI_CSF_NATIONAL };
static unsigned int BindDirectionValues[] = { OCI_BDM_IN, OCI_BDM_OUT, OCI_BDM_IN_OUT };

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_BindFree
(
    OCI_Bind *bnd
)
{
    boolean res = TRUE;

    OCI_CHECK(NULL == bnd, FALSE)

    if (OCI_BAM_INTERNAL == bnd->stmt->bind_alloc_mode)
    {
        if (bnd->is_array)
        {
            res = OCI_ArrayFreeFromHandles(bnd->input);
        }
        else
        {
            switch (bnd->type)
            {
                case OCI_CDT_NUMERIC:
                case OCI_CDT_TEXT:
                {
                    OCI_MemFree(bnd->input);
                    
                    if (bnd->alloc)
                    {
                        OCI_FREE(bnd->buffer.data)
                    }  
                    break;
                }
                default:
                {
                    OCI_FreeObjectFromType(bnd->input, bnd->type);
                }
            }
        }
    }
    else
    {
        if (bnd->alloc)
        {
            OCI_FREE(bnd->buffer.data)
        }
    }

    OCI_FREE(bnd->buffer.inds)
    OCI_FREE(bnd->buffer.obj_inds)
    OCI_FREE(bnd->buffer.null_inds)
    OCI_FREE(bnd->buffer.lens)
    OCI_FREE(bnd->buffer.tmpbuf)
    OCI_FREE(bnd->plrcds)
    OCI_FREE(bnd->name)
    OCI_FREE(bnd)

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindAllocData
 * --------------------------------------------------------------------------------------------- */

boolean OCI_BindAllocData
(
    OCI_Bind *bnd
)
{
    OCI_CHECK(NULL == bnd, FALSE)

    if (bnd->is_array)
    {
        unsigned int struct_size = 0;
        unsigned int elem_size   = 0;
        unsigned int handle_type = 0;

        OCI_Array *arr = NULL;

        switch (bnd->type)
        {
            case OCI_CDT_NUMERIC:
            {
                if (SQLT_VNU == bnd->code)
                {
                    struct_size = sizeof(big_int);
                    elem_size   = sizeof(OCINumber);
                }
                else
                {
                    struct_size = bnd->size;
                }
                break;
            }
            case OCI_CDT_DATETIME:
            {
                struct_size = sizeof(OCI_Date);
                elem_size   = sizeof(OCIDate);
                break;
            }
            case OCI_CDT_TEXT:
            {
                struct_size = bnd->size;

                if (OCILib.use_wide_char_conv)
                {
                    elem_size  = bnd->size * (sizeof(otext) / sizeof(dbtext));
                }
                break;
            }
            case OCI_CDT_LOB:
            {
                struct_size = sizeof(OCI_Lob);
                elem_size   = sizeof(OCILobLocator *);
                handle_type = OCI_DTYPE_LOB;
                break;
            }
            case OCI_CDT_FILE:
            {
                struct_size = sizeof(OCI_File);
                elem_size   = sizeof(OCILobLocator *);
                handle_type = OCI_DTYPE_LOB;
                break;
            }
            case OCI_CDT_TIMESTAMP:
            {
                struct_size = sizeof(OCI_Timestamp);
                elem_size   = sizeof(OCIDateTime *);
                handle_type = OCI_ExternalSubTypeToHandleType(OCI_CDT_TIMESTAMP, bnd->subtype);
                break;
            }
            case OCI_CDT_INTERVAL:
            {
                struct_size = sizeof(OCI_Interval);
                elem_size   = sizeof(OCIInterval *);
                handle_type = OCI_ExternalSubTypeToHandleType(OCI_CDT_INTERVAL, bnd->subtype);
                break;
            }
            case OCI_CDT_RAW:
            {
                struct_size = bnd->size;
                break;
            }
            case OCI_CDT_OBJECT:
            {
                struct_size = sizeof(OCI_Object);
                elem_size   = sizeof(void *);
                break;
            }
            case OCI_CDT_COLLECTION:
            {
                struct_size = sizeof(OCI_Coll);
                elem_size   = sizeof(OCIColl *);
                break;
            }
            case OCI_CDT_REF:
            {
                struct_size = sizeof(OCI_Ref);
                elem_size   = sizeof(OCIRef *);
                break;
            }
        }

        arr = OCI_ArrayCreate(bnd->stmt->con, bnd->buffer.count, bnd->type, bnd->subtype,
                              elem_size, struct_size, handle_type, bnd->typinf);

        if (arr)
        {
            switch (bnd->type)
            {
                case OCI_CDT_NUMERIC:
                {
                    if (SQLT_VNU == bnd->code)
                    {
                        bnd->buffer.data = (void **) arr->mem_handle;
                        bnd->input       = (void **) arr->mem_struct;
                        bnd->alloc       = TRUE;
                    }
                    else
                    {
                        bnd->buffer.data = (void **) arr->mem_struct;
                        bnd->input       = (void **) bnd->buffer.data;
                    }
                    break;
                }
                case OCI_CDT_TEXT:
                {
                    if (OCILib.use_wide_char_conv)
                    {
                        bnd->buffer.data = (void **) arr->mem_handle;
                        bnd->input       = (void **) arr->mem_struct;
                        bnd->alloc       = TRUE;
                    }
                    else
                    {
                        bnd->buffer.data = (void **) arr->mem_struct;
                        bnd->input       = (void **) bnd->buffer.data;
                    }

                    break;
                }
                case OCI_CDT_RAW:
                {
                    bnd->buffer.data = (void **) arr->mem_struct;
                    bnd->input       = (void **) bnd->buffer.data;
                    break;
                }
                case OCI_CDT_DATETIME:
                case OCI_CDT_LOB:
                case OCI_CDT_FILE:
                case OCI_CDT_TIMESTAMP:
                case OCI_CDT_INTERVAL:
                case OCI_CDT_OBJECT:
                case OCI_CDT_COLLECTION:
                case OCI_CDT_REF:
                {
                    bnd->buffer.data = (void **) arr->mem_handle;
                    bnd->input       = (void **) arr->tab_obj;
                    break;
                }
            }
        }
    }
    else
    {
        switch (bnd->type)
        {
            case OCI_CDT_NUMERIC:
            {
                if (SQLT_VNU == bnd->code)
                {
                    bnd->input       = (void **) OCI_MemAlloc(OCI_IPC_VOID, sizeof(big_int),   1, TRUE);
                    bnd->buffer.data = (void **) OCI_MemAlloc(OCI_IPC_VOID, sizeof(OCINumber), 1, TRUE);
                }
                else
                {
                    bnd->input       = (void **) OCI_MemAlloc(OCI_IPC_VOID, bnd->size, 1, TRUE);
                    bnd->buffer.data = (void **) bnd->input;
                }
                break;
            }
            case OCI_CDT_DATETIME:
            {
                OCI_Date *date = OCI_DateCreate(bnd->stmt->con);

                if (date)
                {
                    bnd->input       = (void **) date;
                    bnd->buffer.data = (void **) date->handle;
                }
                break;
            }
            case OCI_CDT_TEXT:
            {
                if (OCILib.use_wide_char_conv)
                {
                    bnd->buffer.data = (void **) OCI_MemAlloc(OCI_IPC_STRING, bnd->size * (sizeof(otext) / sizeof(dbtext)), 1, TRUE);
                    bnd->input       = (void **) OCI_MemAlloc(OCI_IPC_STRING, bnd->size, 1, TRUE);
                }
                else
                {
                    bnd->buffer.data = (void **) OCI_MemAlloc(OCI_IPC_STRING, bnd->size, 1, TRUE);
                    bnd->input       = (void **) bnd->buffer.data;
                }
                break;
            }
            case OCI_CDT_LOB:
            {
                OCI_Lob *lob = OCI_LobCreate(bnd->stmt->con, bnd->subtype);

                if (lob)
                {
                    bnd->input       = (void **) lob;
                    bnd->buffer.data = (void **) lob->handle;
                }
                break;
            }
            case OCI_CDT_FILE:
            {
                OCI_File *file = OCI_FileCreate(bnd->stmt->con,  bnd->subtype);

                if (file)
                {
                    bnd->input       = (void **) file;
                    bnd->buffer.data = (void **) file->handle;
                }
                break;
            }
            case OCI_CDT_TIMESTAMP:
            {
                OCI_Timestamp *tmsp = OCI_TimestampCreate(bnd->stmt->con, bnd->subtype);

                if (tmsp)
                {
                    bnd->input       = (void **) tmsp;
                    bnd->buffer.data = (void **) tmsp->handle;
                }
                break;
            }
            case OCI_CDT_INTERVAL:
            {
                OCI_Interval *itv = OCI_IntervalCreate(bnd->stmt->con, bnd->subtype);

                if (itv)
                {
                    bnd->input       = (void **) itv;
                    bnd->buffer.data = (void **) itv->handle;
                }
                break;
            }
            case OCI_CDT_RAW:
            {
                bnd->input       = (void **) OCI_MemAlloc(OCI_IPC_VOID, bnd->size, 1, TRUE);
                bnd->buffer.data = (void **) bnd->input;
                break;
            }
            case OCI_CDT_OBJECT:
            {
                OCI_Object *obj = OCI_ObjectCreate(bnd->stmt->con, bnd->typinf);

                if (obj)
                {
                    bnd->input       = (void **) obj;
                    bnd->buffer.data = (void **) obj->handle;
                }
                break;
            }
            case OCI_CDT_COLLECTION:
            {
                OCI_Coll *coll = OCI_CollCreate(bnd->typinf);

                if (coll)
                {
                    bnd->input       = (void **) coll;
                    bnd->buffer.data = (void **) coll->handle;
                }
                break;
            }
            case OCI_CDT_REF:
            {
                OCI_Ref *ref = OCI_RefCreate(bnd->stmt->con, bnd->typinf);

                if (ref)
                {
                    bnd->input       = (void **) ref;
                    bnd->buffer.data = (void **) ref->handle;
                }
                break;
            }
        }
    }

    return (NULL != bnd->input);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindSetNullIndicator
 * --------------------------------------------------------------------------------------------- */

boolean OCI_BindSetNullIndicator
(
    OCI_Bind    *bnd,
    unsigned int position,
    sb2          value
)
{
    OCI_CHECK(NULL == bnd, FALSE)

    if (bnd->buffer.inds)
    {
        ((sb2*) bnd->buffer.inds)[position-1] = value;
    }

    return TRUE;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindGetName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_BindGetName
(
    OCI_Bind *bnd
)
{
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_BIND, bnd)

    call_retval = (const otext *)bnd->name;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_BindGetType
(
    OCI_Bind *bnd
)
{
    OCI_LIB_CALL_ENTER(unsigned int, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_BIND, bnd)

    call_retval = (unsigned int)bnd->type;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindGetSubtype
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_BindGetSubtype
(
    OCI_Bind *bnd
)
{
    OCI_LIB_CALL_ENTER(unsigned int, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_BIND, bnd)

    if (OCI_CDT_NUMERIC   == bnd->type ||
        OCI_CDT_LONG      == bnd->type ||
        OCI_CDT_LOB       == bnd->type ||
        OCI_CDT_FILE      == bnd->type ||
        OCI_CDT_TIMESTAMP == bnd->type ||
        OCI_CDT_INTERVAL  == bnd->type)
    {
        call_retval = (unsigned int)bnd->subtype;
    }

    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindGetDataCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_BindGetDataCount
(
    OCI_Bind *bnd
)
{
    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_BIND, bnd)

    call_retval = (unsigned int)bnd->buffer.count;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindGetData
 * --------------------------------------------------------------------------------------------- */

void * OCI_API OCI_BindGetData
(
    OCI_Bind *bnd
)
{
    OCI_LIB_CALL_ENTER(void *, NULL)

    OCI_CHECK_PTR(OCI_IPC_BIND, bnd)

    call_retval = (void *)bnd->input;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindGetStatement
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * OCI_API OCI_BindGetStatement
(
    OCI_Bind *bnd
)
{
    OCI_LIB_CALL_ENTER(OCI_Statement *, NULL)

    OCI_CHECK_PTR(OCI_IPC_BIND, bnd)

    call_retval = bnd->stmt;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindSetDataSize
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindSetDataSize
(
    OCI_Bind    *bnd,
    unsigned int size
)
{
    return OCI_BindSetDataSizeAtPos(bnd, 1, size);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindSetDataSizeAtPos
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindSetDataSizeAtPos
(
    OCI_Bind    *bnd,
    unsigned int position,
    unsigned int size
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_BIND, bnd)
    OCI_CHECK_BOUND(bnd->stmt->con, position, 1, bnd->buffer.count)
    OCI_CHECK_MIN(bnd->stmt->con, bnd->stmt, size, 1)

    call_status = TRUE;

    if (bnd->buffer.lens)
    {
        if (OCI_CDT_TEXT == bnd->type)
        {
            if (bnd->size == (sb4) size)
            {
                size += (unsigned int) (size_t) sizeof(dbtext);
            }

            size *= (unsigned int) sizeof(dbtext);
        }

        ((ub2 *) bnd->buffer.lens)[position-1] = (ub2) size;

        call_retval = TRUE;
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindGetDataSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_BindGetDataSize
(
    OCI_Bind *bnd
)
{
    return OCI_BindGetDataSizeAtPos(bnd, 1);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindGetDataSizeAtPos
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_BindGetDataSizeAtPos
(
    OCI_Bind    *bnd,
    unsigned int position
)
{
    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_BIND, bnd)
    OCI_CHECK_BOUND(bnd->stmt->con, position, 1, bnd->buffer.count)

    call_status = TRUE;

    if (bnd->buffer.lens)
    {
        call_retval = (unsigned int)((ub2 *)bnd->buffer.lens)[position - 1];

        if (OCI_CDT_TEXT == bnd->type)
        {
            if (bnd->size == (sb4)call_retval)
            {
                call_retval -= (unsigned int) sizeof(dbtext);
            }

            call_retval /= (unsigned int) sizeof(dbtext);
        }
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindSetNullAtPos
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindSetNullAtPos
(
    OCI_Bind    *bnd,
    unsigned int position
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_BIND, bnd)
    OCI_CHECK_BOUND(bnd->stmt->con, position, 1, bnd->buffer.count)

    call_retval = call_status = OCI_BindSetNullIndicator(bnd, position, OCI_IND_NULL);

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindSetNull
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindSetNull
(
    OCI_Bind *bnd
)
{
    return OCI_BindSetNullAtPos(bnd, 1);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindSetNotNullAtPos
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindSetNotNullAtPos
(
    OCI_Bind    *bnd,
    unsigned int position
)
{   
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_BIND, bnd)
    OCI_CHECK_BOUND(bnd->stmt->con, position, 1, bnd->buffer.count)

    call_retval = call_status = OCI_BindSetNullIndicator(bnd, position, OCI_IND_NOTNULL);

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindSetNotNull
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindSetNotNull
(
    OCI_Bind *bnd
)
{
    return OCI_BindSetNotNullAtPos(bnd, 1);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindIsNullAtPos
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindIsNullAtPos
(
    OCI_Bind    *bnd,
    unsigned int position
)
{
    OCI_LIB_CALL_ENTER(boolean, TRUE)

    OCI_CHECK_PTR(OCI_IPC_BIND, bnd)
    OCI_CHECK_BOUND(bnd->stmt->con, position, 1, bnd->buffer.count)

    call_status = TRUE;

    if (bnd->buffer.inds)
    {
        call_retval = (OCI_IND_NULL == (((sb2*) bnd->buffer.inds)[position-1]));
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindIsNull
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindIsNull
(
    OCI_Bind *bnd
)
{
    return OCI_BindIsNullAtPos(bnd, 1);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindSetCharsetForm
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindSetCharsetForm
(
    OCI_Bind    *bnd,
    unsigned int csfrm
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_BIND, bnd)
    OCI_CHECK_ENUM_VALUE(bnd->stmt->con, bnd->stmt, csfrm, CharsetFormValues, OTEXT("CharsetForm"))

    call_status = TRUE;

    if ((OCI_CDT_TEXT == bnd->type) || (OCI_CDT_LONG == bnd->type))
    {
        if (OCI_CSF_NATIONAL == csfrm)
        {
            bnd->csfrm = SQLCS_NCHAR;
        }
        else if (OCI_CSF_DEFAULT == csfrm)
        {
            bnd->csfrm = SQLCS_IMPLICIT;
        }

        OCI_CALL1
        (
            call_status, bnd->stmt->con, bnd->stmt,

            OCIAttrSet((dvoid *) bnd->buffer.handle,
                       (ub4    ) OCI_HTYPE_BIND,
                       (dvoid *) &bnd->csfrm,
                       (ub4    ) sizeof(bnd->csfrm),
                       (ub4    ) OCI_ATTR_CHARSET_FORM,
                       bnd->stmt->con->err)
        )
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindSetDirection
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindSetDirection
(
    OCI_Bind    *bnd,
    unsigned int direction
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_BIND, bnd)
    OCI_CHECK_ENUM_VALUE(bnd->stmt->con, bnd->stmt, direction, BindDirectionValues, OTEXT("Direction"))

    bnd->direction = (ub1) direction;

    call_retval = call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindGetDirection
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_BindGetDirection
(
    OCI_Bind *bnd
)
{
    OCI_LIB_CALL_ENTER(unsigned int, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_BIND, bnd)

    call_retval = (unsigned int) bnd->direction;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

