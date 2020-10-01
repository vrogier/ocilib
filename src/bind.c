/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2020 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "bind.h"

#include "array.h"
#include "callback.h"
#include "collection.h"
#include "date.h"
#include "file.h"
#include "hash.h"
#include "helpers.h"
#include "interval.h"
#include "lob.h"
#include "macros.h"
#include "memory.h"
#include "number.h"
#include "object.h"
#include "reference.h"
#include "stringutils.h"
#include "timestamp.h"

static const unsigned int CharsetFormValues[] =
{
    OCI_CSF_DEFAULT,
    OCI_CSF_NATIONAL
};

static const unsigned int BindDirectionValues[] =
{
    OCI_BDM_IN,
    OCI_BDM_OUT,
    OCI_BDM_IN_OUT
};

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindAllocateInternalData
 * --------------------------------------------------------------------------------------------- */

boolean OcilibBindAllocateInternalData
(
    OCI_Bind* bnd
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    )

    CHECK_PTR(OCI_IPC_BIND, bnd)

    if (bnd->is_array)
    {
        unsigned int struct_size = 0;
        unsigned int elem_size   = 0;
        unsigned int handle_type = 0;

        OCI_Array* arr = NULL;

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

                if (Env.use_wide_char_conv)
                {
                    elem_size = bnd->size * (sizeof(otext) / sizeof(dbtext));
                }
                break;
            }
            case OCI_CDT_LOB:
            {
                struct_size = sizeof(OCI_Lob);
                elem_size   = sizeof(OCILobLocator*);
                handle_type = OCI_DTYPE_LOB;
                break;
            }
            case OCI_CDT_FILE:
            {
                struct_size = sizeof(OCI_File);
                elem_size   = sizeof(OCILobLocator*);
                handle_type = OCI_DTYPE_LOB;
                break;
            }
            case OCI_CDT_TIMESTAMP:
            {
                struct_size = sizeof(OCI_Timestamp);
                elem_size   = sizeof(OCIDateTime*);
                handle_type = ExternalSubTypeToHandleType(OCI_CDT_TIMESTAMP, bnd->subtype);
                break;
            }
            case OCI_CDT_INTERVAL:
            {
                struct_size = sizeof(OCI_Interval);
                elem_size   = sizeof(OCIInterval*);
                handle_type = ExternalSubTypeToHandleType(OCI_CDT_INTERVAL, bnd->subtype);
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
                elem_size   = sizeof(void*);
                break;
            }
            case OCI_CDT_COLLECTION:
            {
                struct_size = sizeof(OCI_Coll);
                elem_size   = sizeof(OCIColl*);
                break;
            }
            case OCI_CDT_REF:
            {
                struct_size = sizeof(OCI_Ref);
                elem_size   = sizeof(OCIRef*);
                break;
            }
        }

        arr = OcilibArrayCreate(bnd->stmt->con, bnd->buffer.count,
                                bnd->type, bnd->subtype, elem_size,
                                struct_size, handle_type, bnd->typinf);

        CHECK_NULL(arr)

        switch (bnd->type)
        {
            case OCI_CDT_NUMERIC:
            {
                if (bnd->subtype == OCI_NUM_NUMBER)
                {
                    bnd->buffer.data = (void**)arr->mem_handle;
                    bnd->input       = (void**)arr->tab_obj;

                }
                else if (SQLT_VNU == bnd->code)
                {
                    bnd->buffer.data = (void**)arr->mem_handle;
                    bnd->input       = (void**)arr->mem_struct;
                    bnd->alloc       = TRUE;
                }
                else
                {
                    bnd->buffer.data = (void**)arr->mem_struct;
                    bnd->input       = (void**)bnd->buffer.data;
                }
                break;
            }
            case OCI_CDT_TEXT:
            {
                if (Env.use_wide_char_conv)
                {
                    bnd->buffer.data = (void**)arr->mem_handle;
                    bnd->input       = (void**)arr->mem_struct;
                    bnd->alloc       = TRUE;
                }
                else
                {
                    bnd->buffer.data = (void**)arr->mem_struct;
                    bnd->input       = (void**)bnd->buffer.data;
                }

                break;
            }
            case OCI_CDT_RAW:
            {
                bnd->buffer.data = (void**)arr->mem_struct;
                bnd->input       = (void**)bnd->buffer.data;
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
                bnd->buffer.data = (void**)arr->mem_handle;
                bnd->input       = (void**)arr->tab_obj;
                break;
            }
        }
    }
    else
    {
        switch (bnd->type)
        {
            case OCI_CDT_NUMERIC:
            {
                if (bnd->subtype == OCI_NUM_NUMBER)
                {
                    OCI_Number* number = NumberCreate(bnd->stmt->con);

                    CHECK_NULL(number)

                    bnd->input       = (void**)number;
                    bnd->buffer.data = (void**)number->handle;
                }
                else if (SQLT_VNU == bnd->code)
                {
                    bnd->input       = (void**)MemoryAlloc(OCI_IPC_VOID, sizeof(big_int), 1, TRUE);
                    bnd->buffer.data = (void**)MemoryAlloc(OCI_IPC_VOID, sizeof(OCINumber), 1, TRUE);
                }
                else
                {
                    bnd->input       = (void**)MemoryAlloc(OCI_IPC_VOID, bnd->size, 1, TRUE);
                    bnd->buffer.data = (void**)bnd->input;
                }
                break;
            }
            case OCI_CDT_DATETIME:
            {
                OCI_Date* date = DateCreate(bnd->stmt->con);

                CHECK_NULL(date)

                bnd->input       = (void**)date;
                bnd->buffer.data = (void**)date->handle;

                break;
            }
            case OCI_CDT_TEXT:
            {
                if (Env.use_wide_char_conv)
                {
                    bnd->buffer.data = (void**)MemoryAlloc(OCI_IPC_STRING, bnd->size * (sizeof(otext) / sizeof(dbtext)), 1, TRUE);
                    bnd->input       = (void**)MemoryAlloc(OCI_IPC_STRING, bnd->size, 1, TRUE);
                }
                else
                {
                    bnd->buffer.data = (void**)MemoryAlloc(OCI_IPC_STRING, bnd->size, 1, TRUE);
                    bnd->input       = (void**)bnd->buffer.data;
                }
                break;
            }
            case OCI_CDT_LOB:
            {
                OCI_Lob* lob = LobCreate(bnd->stmt->con, bnd->subtype);

                CHECK_NULL(lob)

                bnd->input       = (void**)lob;
                bnd->buffer.data = (void**)lob->handle;

                break;
            }
            case OCI_CDT_FILE:
            {
                OCI_File* file = FileCreate(bnd->stmt->con, bnd->subtype);

                CHECK_NULL(file)

                bnd->input       = (void**)file;
                bnd->buffer.data = (void**)file->handle;

                break;
            }
            case OCI_CDT_TIMESTAMP:
            {
                OCI_Timestamp* tmsp = TimestampCreate(bnd->stmt->con, bnd->subtype);

                CHECK_NULL(tmsp)

                bnd->input       = (void**)tmsp;
                bnd->buffer.data = (void**)tmsp->handle;

                break;
            }
            case OCI_CDT_INTERVAL:
            {
                OCI_Interval* itv = IntervalCreate(bnd->stmt->con, bnd->subtype);

                CHECK_NULL(itv)

                bnd->input       = (void**)itv;
                bnd->buffer.data = (void**)itv->handle;

                break;
            }
            case OCI_CDT_RAW:
            {
                bnd->input       = (void**)MemoryAlloc(OCI_IPC_VOID, bnd->size, 1, TRUE);
                bnd->buffer.data = (void**)bnd->input;
                break;
            }
            case OCI_CDT_OBJECT:
            {
                OCI_Object* obj = ObjectCreate(bnd->stmt->con, bnd->typinf);

                CHECK_NULL(obj)

                bnd->input       = (void**)obj;
                bnd->buffer.data = (void**)obj->handle;

                break;
            }
            case OCI_CDT_COLLECTION:
            {
                OCI_Coll* coll = CollectionCreate(bnd->typinf);

                CHECK_NULL(coll)

                bnd->input       = (void**)coll;
                bnd->buffer.data = (void**)coll->handle;

                break;
            }
            case OCI_CDT_REF:
            {
                OCI_Ref* ref = ReferenceCreate(bnd->stmt->con, bnd->typinf);

                CHECK_NULL(ref)

                bnd->input       = (void**)ref;
                bnd->buffer.data = (void**)ref->handle;

                break;
            }
        }
    }

    CHECK_NULL(bnd->input)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindAllocateBuffers
 * --------------------------------------------------------------------------------------------- */

boolean OcilibBindAllocateBuffers
(
    OCI_Bind    *bnd,
    unsigned int mode,
    boolean      reused,
    unsigned int nballoc,
    unsigned int nbelem,
    boolean      plsql_table
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    )

    CHECK_PTR(OCI_IPC_BIND, bnd)

    /* allocate indicators array */

    ALLOC_DATA(OCI_IPC_BIND, bnd->buffer.inds, nballoc)

    if (SQLT_NTY == bnd->code)
    {
        ALLOC_DATA(OCI_IPC_INDICATOR_ARRAY, bnd->buffer.obj_inds, nballoc)
    }

    /* check need for PL/SQL table extra info */

    if (plsql_table)
    {
        bnd->nbelem = nbelem;

        /* allocate array of returned codes */

        ALLOC_DATA(OCI_IPC_PLS_RCODE_ARRAY, bnd->plrcds, nballoc)
    }

    /* set allocation mode prior any required data allocation */

    bnd->alloc_mode = (ub1)bnd->stmt->bind_alloc_mode;

    /* for handle based data types, we need to allocate an array of handles for
       bind calls because OCILIB uses external arrays of OCILIB Objects */

    if (OCI_BIND_INPUT == mode)
    {
        if (OCI_BAM_EXTERNAL == bnd->alloc_mode)
        {
            if ((OCI_CDT_RAW     != bnd->type)  &&
                (OCI_CDT_LONG    != bnd->type)  &&
                (OCI_CDT_CURSOR  != bnd->type)  &&
                (OCI_CDT_LONG    != bnd->type)  &&
                (OCI_CDT_BOOLEAN != bnd->type)  &&
                (OCI_CDT_NUMERIC != bnd->type || SQLT_VNU == bnd->code) &&
                (OCI_CDT_TEXT    != bnd->type || Env.use_wide_char_conv))
            {
                bnd->alloc = TRUE;

                if (reused)
                {
                    FREE(bnd->buffer.data)
                }

                ALLOC_BUFFER(OCI_IPC_BUFF_ARRAY, bnd->buffer.data, bnd->size, nballoc)
            }
            else
            {
                bnd->buffer.data = (void **)bnd->input;
            }
        }
    }

    /* setup data length array */

    if (OCI_CDT_RAW == bnd->type || OCI_CDT_TEXT == bnd->type)
    {
        ALLOC_BUFFER(OCI_IPC_BUFF_ARRAY, bnd->buffer.lens, sizeof(ub2), nballoc)

        /* initialize length array with buffer default size */

        for (unsigned int i = 0; i < nbelem; i++)
        {
            *(ub2*)(((ub1 *)bnd->buffer.lens) + sizeof(ub2) * (size_t) i) = (ub2)bnd->size;
        }
    }

    /* internal allocation if needed */

    if (!bnd->input && (OCI_BAM_INTERNAL == bnd->alloc_mode))
    {
        CHECK(OcilibBindAllocateInternalData(bnd))
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
* OcilibBindCheckAvailability
* --------------------------------------------------------------------------------------------- */

boolean OcilibBindCheckAvailability
(
    OCI_Statement *stmt,
    unsigned int   mode,
    boolean        reused
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    if (!reused)
    {
        if (OCI_BIND_INPUT == mode)
        {
            if (stmt->nb_ubinds >= OCI_BIND_MAX)
            {
                THROW_NO_ARGS(ExceptionMaxBind)
            }

            /* allocate user bind array if necessary */

            REALLOC_DATA
            (
                OCI_IPC_BIND_ARRAY,
                stmt->ubinds,
                stmt->nb_ubinds,
                stmt->allocated_ubinds,
                min(stmt->nb_ubinds + OCI_BIND_ARRAY_GROWTH_FACTOR, OCI_BIND_MAX)
            )
        }
        else
        {
            if (stmt->nb_rbinds >= OCI_BIND_MAX)
            {
                THROW_NO_ARGS(ExceptionMaxBind)
            }

            /* allocate register bind array if necessary */

            REALLOC_DATA
            (
                OCI_IPC_BIND_ARRAY,
                stmt->rbinds,
                stmt->nb_rbinds,
                stmt->allocated_rbinds,
                min(stmt->nb_rbinds + OCI_BIND_ARRAY_GROWTH_FACTOR, OCI_BIND_MAX)
            )
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
  * OcilibBindPerformBinding
  * --------------------------------------------------------------------------------------------- */

boolean OcilibBindPerformBinding
(
    OCI_Bind    *bnd,
    unsigned int mode,
    unsigned int index,
    unsigned int exec_mode,
    boolean      plsql_table
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    )

    dbtext* dbstr = NULL;
    int dbsize = -1;

    CHECK_PTR(OCI_IPC_BIND, bnd)

    if (OCI_BIND_BY_POS == bnd->stmt->bind_mode)
    {
        CHECK_OCI
        (
            bnd->stmt->con->err,
            OCIBindByPos,
            bnd->stmt->stmt,
            (OCIBind **)&bnd->buffer.handle,
            bnd->stmt->con->err,
            (ub4)index,
            (void *)bnd->buffer.data,
            bnd->size,
            bnd->code,
            (void *)bnd->buffer.inds,
            (ub2 *)bnd->buffer.lens,
            bnd->plrcds,
            (ub4)(plsql_table ? bnd->nbelem : 0),
            (ub4*)(plsql_table ? &bnd->nbelem : NULL),
            (ub4) exec_mode
        )
    }
    else
    {
        dbstr = StringGetDBString(bnd->name, &dbsize);

        CHECK_OCI
        (
            bnd->stmt->con->err,
            OCIBindByName,
            bnd->stmt->stmt,
            (OCIBind **)&bnd->buffer.handle,
            bnd->stmt->con->err,
            (OraText *)dbstr,
            (sb4)dbsize,
            (void *)bnd->buffer.data,
            bnd->size,
            bnd->code,
            (void *)bnd->buffer.inds,
            (ub2 *)bnd->buffer.lens,
            bnd->plrcds,
            (ub4)(plsql_table ? bnd->nbelem : 0),
            (ub4*)(plsql_table ? &bnd->nbelem : NULL),
            (ub4) exec_mode
        )
    }

    if (SQLT_NTY == bnd->code || SQLT_REF == bnd->code)
    {
        CHECK_OCI
        (
            bnd->stmt->con->err,
            OCIBindObject,
            (OCIBind *)bnd->buffer.handle,
            bnd->stmt->con->err,
            (OCIType *)bnd->typinf->tdo,
            (void **)bnd->buffer.data,
            (ub4 *)NULL,
            (void **)bnd->buffer.obj_inds,
            (ub4 *)NULL
        )
    }

    if (OCI_BIND_OUTPUT == mode)
    {
        /* register output placeholder */

        CHECK_OCI
        (
            bnd->stmt->con->err,
            OCIBindDynamic,
            (OCIBind *)bnd->buffer.handle,
            bnd->stmt->con->err,
            (dvoid *)bnd,
            CallbackInBind,
            (dvoid *)bnd,
            CallbackOutBind
        )
    }

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr);
    )

}

/* --------------------------------------------------------------------------------------------- *
  * OcilibBindAddToStatement
  * --------------------------------------------------------------------------------------------- */

boolean OcilibBindAddToStatement
(
    OCI_Bind    *bnd,
    unsigned int mode,
    boolean      reused
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    )

    CHECK_PTR(OCI_IPC_BIND, bnd)

    if (OCI_BIND_INPUT == mode)
    {
        if (!reused)
        {
            bnd->stmt->ubinds[bnd->stmt->nb_ubinds++] = bnd;

            /* for user binds, add a positive index */

            CHECK(HashAddInt(bnd->stmt->map, bnd->name, bnd->stmt->nb_ubinds))
        }
    }
    else
    {
        /* for register binds, add a negative index */

        bnd->stmt->rbinds[bnd->stmt->nb_rbinds++] = bnd;

        const int index = (int)bnd->stmt->nb_rbinds;

        CHECK(HashAddInt(bnd->stmt->map, bnd->name, -index))
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibBindCreate
(
    OCI_Statement *stmt,
    void          *data,
    const otext   *name,
    unsigned int   mode,
    ub4            size,
    ub1            type,
    unsigned int   code,
    unsigned int   subtype,
    OCI_TypeInfo  *typinf,
    unsigned int   nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    OCI_Bind    *bnd = NULL;

    ub4          exec_mode   = OCI_DEFAULT;
    boolean      plsql_table = FALSE;
    boolean      is_array    = FALSE;
    boolean      reused      = FALSE;
    int          index       = 0;
    int          prev_index  = -1;
    unsigned int nballoc     = nbelem;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)

    /* check index if necessary */

    if (OCI_BIND_BY_POS == stmt->bind_mode)
    {
        index = (int) ostrtol(&name[1], NULL, 10);

        if (index <= 0 || index > OCI_BIND_MAX)
        {
            THROW(ExceptionOutOfBounds, index)
        }
    }

    /* check if the bind name has already been used */

    if (OCI_BIND_INPUT == mode)
    {
        prev_index = OcilibBindGetIndex(stmt, name);

        if (prev_index > 0)
        {
            if (!stmt->bind_reuse)
            {
                THROW(ExceptionBindAlreadyUsed, name)
            }
            else
            {
                bnd = stmt->ubinds[prev_index-1];

                if (bnd->type != type)
                {
                    THROW(ExceptionRebindBadDatatype, name)
                }
                else
                {
                    reused = TRUE;
                }
            }

            index = prev_index;
        }
    }

    /* check if we can handle another bind */

    CHECK(OcilibBindCheckAvailability(stmt, mode, reused))

    /* check out the number of elements that the bind variable will hold */

    if (nbelem > 0)
    {
        /* is it a pl/sql table bind ? */

        if (IS_PLSQL_STMT(stmt->type))
        {
            plsql_table = TRUE;
            is_array    = TRUE;
        }
    }
    else
    {
        nbelem   = stmt->nb_iters;
        is_array = stmt->bind_array;
    }

    /* compute iterations */

    if (nballoc < stmt->nb_iters_init)
    {
        nballoc = (size_t) stmt->nb_iters_init;
    }

    /* create hash table for mapping bind names / index */

    if (NULL == stmt->map)
    {
        stmt->map = HashCreate(OCI_HASH_DEFAULT_SIZE, OCI_HASH_INTEGER);
        CHECK_NULL(stmt->map)
    }

    /* allocate bind object */

    ALLOC_DATA(OCI_IPC_BIND, bnd, 1)

    /* initialize bind object */

    bnd->stmt      = stmt;
    bnd->input     = (void **) data;
    bnd->type      = type;
    bnd->size      = size;
    bnd->code      = (ub2) code;
    bnd->subtype   = (ub1) subtype;
    bnd->is_array  = is_array;
    bnd->typinf    = typinf;
    bnd->csfrm     = OCI_CSF_NONE;
    bnd->direction = OCI_BDM_IN_OUT;

    if (NULL == bnd->name)
    {
        bnd->name = ostrdup(name);
    }

    /* initialize buffer */

    bnd->buffer.count   = nbelem;
    bnd->buffer.sizelen = sizeof(ub2);

    CHECK(OcilibBindAllocateBuffers(bnd, mode, reused, nballoc, nbelem, plsql_table))

    /* if we bind an OCI_Long or any output bind, we need to change the
       execution mode to provide data at execute time */

    if (OCI_CDT_LONG == bnd->type)
    {
        OCI_Long *lg = (OCI_Long *)  bnd->input;

        lg->maxsize = size;
        exec_mode   = OCI_DATA_AT_EXEC;

        if (OCI_CLONG == bnd->subtype)
        {
            lg->maxsize /= (unsigned int) sizeof(otext);
            lg->maxsize *= (unsigned int) sizeof(dbtext);
        }
    }
    else if (OCI_BIND_OUTPUT == mode)
    {
        exec_mode = OCI_DATA_AT_EXEC;
    }

    /* OCI binding */

    CHECK(OcilibBindPerformBinding(bnd, mode, index, exec_mode, plsql_table))

    /* set charset form */

    if ((OCI_CDT_LOB == bnd->type) && (OCI_NCLOB == bnd->subtype))
    {
        ub1 csfrm = SQLCS_NCHAR;

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_BIND, OCI_ATTR_CHARSET_FORM,
            bnd->buffer.handle, &csfrm, sizeof(csfrm),
            bnd->stmt->con->err
        )
    }

    /* on success, we :
         - add the bind handle to the bind array
         - add the bind index to the map
    */

    CHECK(OcilibBindAddToStatement(bnd, mode, reused))

    CLEANUP_AND_EXIT_FUNC
    (
        /* on error, only free the bind if it was a new one */

        if (FAILURE && NULL != bnd && prev_index == -1)
        {
            OcilibBindFree(bnd);
            bnd = NULL;
        }

        SET_RETVAL(bnd)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindFree
 * --------------------------------------------------------------------------------------------- */

boolean OcilibBindFree
(
    OCI_Bind *bnd
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    )

    CHECK_PTR(OCI_IPC_BIND, bnd)

    if (OCI_BAM_INTERNAL == bnd->alloc_mode)
    {
        if (bnd->is_array)
        {
            OcilibArrayFreeFromHandles(bnd->input);
        }
        else
        {
            switch (bnd->type)
            {
                case OCI_CDT_NUMERIC:
                case OCI_CDT_TEXT:
                {
                    /* OCINumber binds */

                    if (bnd->type == OCI_CDT_NUMERIC && bnd->subtype == OCI_NUM_NUMBER)
                    {
                        FreeObjectFromType(bnd->input, bnd->type);
                    }
                    else
                    {
                        /* strings requiring otext / dbtext conversions and 64 bit integers */

                        MemoryFree(bnd->input);

                        if (bnd->alloc)
                        {
                            FREE(bnd->buffer.data)
                        }
                    }
                    break;
                }
                default:
                {
                    FreeObjectFromType(bnd->input, bnd->type);
                }
            }
        }
    }
    else
    {
        if (bnd->alloc)
        {
            FREE(bnd->buffer.data)
        }
    }

    ErrorResetSource(NULL, bnd);

    FREE(bnd->buffer.inds)
    FREE(bnd->buffer.obj_inds)
    FREE(bnd->buffer.lens)
    FREE(bnd->buffer.tmpbuf)
    FREE(bnd->plrcds)
    FREE(bnd->name)
    FREE(bnd)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindGetIndex
 * --------------------------------------------------------------------------------------------- */

int OcilibBindGetIndex
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    ENTER_FUNC
    (
        /* returns */ int, -1,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_PTR(OCI_IPC_STRING,    name)

    CHECK_NULL(stmt->map)

    OCI_HashEntry* he = HashLookup(stmt->map, name, FALSE);

    int index = -1;

    while (NULL != he)
    {
        /* no more entries or key matched => so we got it ! */

        if (NULL == he->next || ostrcasecmp(he->key, name) == 0)
        {
            /* in order to use the same map for user binds and
               register binds :
                  - user binds are stored as positive values
                  - registers binds are stored as negatives values
            */

            index = he->values->value.num;

            if (index < 0)
            {
                index = -index;
            }

            break;
        }
    }

    SET_RETVAL(index)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindSetNullIndicator
 * --------------------------------------------------------------------------------------------- */

boolean OcilibBindSetNullIndicator
(
    OCI_Bind    *bnd,
    unsigned int position,
    sb2          value
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    )

    CHECK_PTR(OCI_IPC_BIND, bnd)

    if (bnd->buffer.inds)
    {
        bnd->buffer.inds[position - 1] = value;
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindGetName
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibBindGetName
(
    OCI_Bind *bnd
)
{
    GET_PROP
    (
        /* result */ const otext*, NULL,
        /* handle */ OCI_IPC_BIND, bnd,
        /* member */ name
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibBindGetType
(
    OCI_Bind *bnd
)
{
    GET_PROP
    (
        /* result */ unsigned int, OCI_UNKNOWN,
        /* handle */ OCI_IPC_BIND, bnd,
        /* member */ type
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindGetSubtype
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibBindGetSubtype
(
    OCI_Bind *bnd
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, OCI_UNKNOWN,
        /* context */ OCI_IPC_BIND, bnd
    )

    CHECK_PTR(OCI_IPC_BIND, bnd)

    unsigned int type = OCI_UNKNOWN;

    if (OCI_CDT_NUMERIC   == bnd->type ||
        OCI_CDT_LONG      == bnd->type ||
        OCI_CDT_LOB       == bnd->type ||
        OCI_CDT_FILE      == bnd->type ||
        OCI_CDT_TIMESTAMP == bnd->type ||
        OCI_CDT_INTERVAL  == bnd->type)
    {
        type = (unsigned int)bnd->subtype;
    }

    SET_RETVAL(type)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindGetDataCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibBindGetDataCount
(
    OCI_Bind *bnd
)
{
    GET_PROP
    (
        /* result */ unsigned int, 0,
        /* handle */ OCI_IPC_BIND, bnd,
        /* member */ buffer.count
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindGetData
 * --------------------------------------------------------------------------------------------- */

void * OcilibBindGetData
(
    OCI_Bind *bnd
)
{
    GET_PROP
    (
        /* result */ void *, NULL,
        /* handle */ OCI_IPC_BIND, bnd,
        /* member */ input
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindGetStatement
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * OcilibBindGetStatement
(
    OCI_Bind *bnd
)
{
    GET_PROP
    (
        /* result */ OCI_Statement *, NULL,
        /* handle */ OCI_IPC_BIND, bnd,
        /* member */ stmt
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindSetDataSize
 * --------------------------------------------------------------------------------------------- */

boolean OcilibBindSetDataSize
(
    OCI_Bind    *bnd,
    unsigned int size
)
{
    return OcilibBindSetDataSizeAtPos(bnd, 1, size);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindSetDataSizeAtPos
 * --------------------------------------------------------------------------------------------- */

boolean OcilibBindSetDataSizeAtPos
(
    OCI_Bind    *bnd,
    unsigned int position,
    unsigned int size
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    )

    CHECK_PTR(OCI_IPC_BIND, bnd)
    CHECK_BOUND(position, 1, bnd->buffer.count)
    CHECK_MIN(size, 1)

    CHECK_NULL(bnd->buffer.lens)

    if (OCI_CDT_TEXT == bnd->type)
    {
        if (bnd->size == (sb4) size)
        {
            size += (unsigned int) (size_t) sizeof(dbtext);
        }

        size *= (unsigned int) sizeof(dbtext);
    }

    ((ub2 *) bnd->buffer.lens)[position-1] = (ub2) size;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindGetDataSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibBindGetDataSize
(
    OCI_Bind *bnd
)
{
    return OcilibBindGetDataSizeAtPos(bnd, 1);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindGetDataSizeAtPos
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibBindGetDataSizeAtPos
(
    OCI_Bind    *bnd,
    unsigned int position
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_BIND, bnd
    )

    CHECK_PTR(OCI_IPC_BIND, bnd)
    CHECK_BOUND(position, 1, bnd->buffer.count)

    CHECK_NULL(bnd->buffer.lens)

    unsigned int size = (unsigned int)((ub2 *)bnd->buffer.lens)[position - 1];

    if (OCI_CDT_TEXT == bnd->type)
    {
        if (bnd->size == (sb4)size)
        {
            size -= (unsigned int) sizeof(dbtext);
        }

        size /= (unsigned int) sizeof(dbtext);
    }

    SET_RETVAL(size)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindSetNullAtPos
 * --------------------------------------------------------------------------------------------- */

boolean OcilibBindSetNullAtPos
(
    OCI_Bind    *bnd,
    unsigned int position
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    )

    CHECK_PTR(OCI_IPC_BIND, bnd)
    CHECK_BOUND(position, 1, bnd->buffer.count)

    CHECK(OcilibBindSetNullIndicator(bnd, position, OCI_IND_NULL))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindSetNull
 * --------------------------------------------------------------------------------------------- */

boolean OcilibBindSetNull
(
    OCI_Bind *bnd
)
{
    return OcilibBindSetNullAtPos(bnd, 1);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindSetNotNullAtPos
 * --------------------------------------------------------------------------------------------- */

boolean OcilibBindSetNotNullAtPos
(
    OCI_Bind    *bnd,
    unsigned int position
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    )

    CHECK_PTR(OCI_IPC_BIND, bnd)
    CHECK_BOUND(position, 1, bnd->buffer.count)

    CHECK(OcilibBindSetNullIndicator(bnd, position, OCI_IND_NOTNULL))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindSetNotNull
 * --------------------------------------------------------------------------------------------- */

boolean OcilibBindSetNotNull
(
    OCI_Bind *bnd
)
{
    return OcilibBindSetNotNullAtPos(bnd, 1);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindIsNullAtPos
 * --------------------------------------------------------------------------------------------- */

boolean OcilibBindIsNullAtPos
(
    OCI_Bind    *bnd,
    unsigned int position
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, TRUE,
        /* context */ OCI_IPC_BIND, bnd
    )

    CHECK_PTR(OCI_IPC_BIND, bnd)
    CHECK_BOUND(position, 1, bnd->buffer.count)

    CHECK_NULL(bnd->buffer.inds)

    SET_RETVAL(OCI_IND_NULL == bnd->buffer.inds[position - 1]);

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindIsNull
 * --------------------------------------------------------------------------------------------- */

boolean OcilibBindIsNull
(
    OCI_Bind *bnd
)
{
    return OcilibBindIsNullAtPos(bnd, 1);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindSetCharsetForm
 * --------------------------------------------------------------------------------------------- */

boolean OcilibBindSetCharsetForm
(
    OCI_Bind    *bnd,
    unsigned int csfrm
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    )

    CHECK_PTR(OCI_IPC_BIND, bnd)
    CHECK_ENUM_VALUE(csfrm, CharsetFormValues, OTEXT("CharsetForm"))

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

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_BIND, OCI_ATTR_CHARSET_FORM,
            bnd->buffer.handle, &bnd->csfrm, sizeof(bnd->csfrm),
            bnd->stmt->con->err
        )
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindSetDirection
 * --------------------------------------------------------------------------------------------- */

boolean OcilibBindSetDirection
(
    OCI_Bind    *bnd,
    unsigned int direction
)
{
    SET_PROP_ENUM
    (
        /* handle */ OCI_IPC_BIND, bnd,
        /* member */ direction, ub1,
        /* value  */ direction, BindDirectionValues, OTEXT("Direction")
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindGetDirection
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibBindGetDirection
(
    OCI_Bind *bnd
)
{
    GET_PROP
    (
        /* result */ unsigned int, OCI_UNKNOWN,
        /* handle */ OCI_IPC_BIND, bnd,
        /* member */ direction
    )
}

/* --------------------------------------------------------------------------------------------- *
* OcilibBindGetAllocationMode
* --------------------------------------------------------------------------------------------- */

unsigned int OcilibBindGetAllocationMode
(
    OCI_Bind *bnd
)
{
    GET_PROP
    (
        /* result */ unsigned int, OCI_UNKNOWN,
        /* handle */ OCI_IPC_BIND, bnd,
        /* member */ alloc_mode
    )
}
