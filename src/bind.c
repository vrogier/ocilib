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

#include "bind.h"

#include "array.h"
#include "callback.h"
#include "collection.h"
#include "date.h"
#include "file.h"
#include "hash.h"
#include "helpers.h"
#include "interval.h"
#include "json.h"
#include "lob.h"
#include "macros.h"
#include "memory.h"
#include "number.h"
#include "object.h"
#include "reference.h"
#include "stringutils.h"
#include "timestamp.h"
#include "vector.h"

static const OCI_UInt32 CharsetFormValues[] =
{
    OCI_CSF_DEFAULT,
    OCI_CSF_NATIONAL
};

static const OCI_UInt32 BindDirectionValues[] =
{
    OCI_BDM_IN,
    OCI_BDM_OUT,
    OCI_BDM_IN_OUT
};

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindAllocateInternalData
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibBindAllocateInternalData
(
    OCI_Bind* bnd
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    );

    CHECK_PTR(OCI_IPC_BIND, bnd);

    if (bnd->is_array)
    {
        OCI_UInt32 struct_size = 0;
        OCI_UInt32 elem_size   = 0;
        OCI_UInt32 handle_type = 0;

        OCI_Array* arr = NULL;

        switch (bnd->type)
        {
            case OCI_CDT_NUMERIC:
            {
                if (SQLT_VNU == bnd->code)
                {
                    struct_size = (OCI_NUM_NUMBER == bnd->subtype) ? sizeof(OCI_Number) : sizeof(OCI_Int64);
                    elem_size   = sizeof(OCINumber);
                }
                else
                {
                    struct_size = (OCI_UInt32) bnd->size;
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
                struct_size = (OCI_UInt32) bnd->size;

                if (bnd->stmt->con->env->use_wide_char_conv)
                {
                    elem_size = (OCI_UInt32) bnd->size * (OCI_UInt32)(sizeof(otext) / sizeof(dbtext));
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
                handle_type = OcilibExternalSubTypeToHandleType(OCI_CDT_TIMESTAMP, bnd->subtype);
                break;
            }
            case OCI_CDT_INTERVAL:
            {
                struct_size = sizeof(OCI_Interval);
                elem_size   = sizeof(OCIInterval*);
                handle_type = OcilibExternalSubTypeToHandleType(OCI_CDT_INTERVAL, bnd->subtype);
                break;
            }
            case OCI_CDT_RAW:
            {
                struct_size = (OCI_UInt32) bnd->size;
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
#if OCI_VERSION_COMPILE >= OCI_23_4
            case OCI_CDT_VECTOR:
            {
                struct_size = sizeof(OCI_Vector);
                elem_size   = sizeof(OCIVector*);
                break;
            }
#endif
            case OCI_CDT_JSON:
            {
                struct_size = sizeof(OCI_Json);
                elem_size   = sizeof(OCIJson*);
                handle_type = OCI_DTYPE_JSON;
                break;
            }
        }

        arr = OcilibArrayCreate(bnd->stmt->con->env,bnd->stmt->con, bnd->buffer.count,
                                bnd->type, bnd->subtype, elem_size,
                                struct_size, handle_type, bnd->typinf);

        CHECK_NULL(arr);

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
                if (bnd->stmt->con->env->use_wide_char_conv)
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
            case OCI_CDT_VECTOR:
            case OCI_CDT_JSON:
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
                    OCI_Number* number = OcilibNumberCreate(bnd->stmt->con->env);

                    CHECK_NULL(number);

                    bnd->input       = (void**)number;
                    bnd->buffer.data = (void**)number->handle;
                }
                else if (SQLT_VNU == bnd->code)
                {
                    bnd->input       = (void**)OcilibMemoryAlloc(bnd->stmt->con->env,OCI_IPC_VOID, sizeof(OCI_Int64), 1, TRUE);
                    bnd->buffer.data = (void**)OcilibMemoryAlloc(bnd->stmt->con->env,OCI_IPC_VOID, sizeof(OCINumber), 1, TRUE);
                    bnd->alloc       = TRUE;
                }
                else
                {
                    bnd->input       = (void**)OcilibMemoryAlloc(bnd->stmt->con->env,OCI_IPC_VOID, (OCI_Size) bnd->size, 1, TRUE);
                    bnd->buffer.data = (void**)bnd->input;
                }
                break;
            }
            case OCI_CDT_DATETIME:
            {
                OCI_Date* date = OcilibDateCreate(bnd->stmt->con->env);

                CHECK_NULL(date);

                bnd->input       = (void**)date;
                bnd->buffer.data = (void**)date->handle;

                break;
            }
            case OCI_CDT_TEXT:
            {
                if (bnd->stmt->con->env->use_wide_char_conv)
                {
                    bnd->buffer.data = (void**)OcilibMemoryAlloc(bnd->stmt->con->env,OCI_IPC_STRING,
                                                                 (OCI_Size) bnd->size * (sizeof(otext) / sizeof(dbtext)), 1, TRUE);
                    bnd->input = (void**)OcilibMemoryAlloc(bnd->stmt->con->env,OCI_IPC_STRING, (OCI_Size) bnd->size, 1, TRUE);
                    bnd->alloc = TRUE;
                }
                else
                {
                    bnd->buffer.data = (void**)OcilibMemoryAlloc(bnd->stmt->con->env,OCI_IPC_STRING, (OCI_Size) bnd->size, 1, TRUE);
                    bnd->input       = (void**)bnd->buffer.data;
                }
                break;
            }
            case OCI_CDT_LOB:
            {
                OCI_Lob* lob = OcilibLobCreate(bnd->stmt->con, bnd->subtype);

                CHECK_NULL(lob);

                bnd->input       = (void**)lob;
                bnd->buffer.data = (void**)lob->handle;

                break;
            }
            case OCI_CDT_FILE:
            {
                OCI_File* file = OcilibFileCreate(bnd->stmt->con, bnd->subtype);

                CHECK_NULL(file);

                bnd->input       = (void**)file;
                bnd->buffer.data = (void**)file->handle;

                break;
            }
            case OCI_CDT_TIMESTAMP:
            {
                OCI_Timestamp* tmsp = OcilibTimestampCreate(bnd->stmt->con->env, bnd->subtype);

                CHECK_NULL(tmsp);

                bnd->input       = (void**)tmsp;
                bnd->buffer.data = (void**)tmsp->handle;

                break;
            }
            case OCI_CDT_INTERVAL:
            {
                OCI_Interval* itv = OcilibIntervalCreate(bnd->stmt->con->env, bnd->subtype);

                CHECK_NULL(itv);

                bnd->input       = (void**)itv;
                bnd->buffer.data = (void**)itv->handle;

                break;
            }
            case OCI_CDT_RAW:
            {
                bnd->input       = (void**)OcilibMemoryAlloc(bnd->stmt->con->env,OCI_IPC_VOID, (OCI_Size) bnd->size, 1, TRUE);
                bnd->buffer.data = (void**)bnd->input;
                break;
            }
            case OCI_CDT_OBJECT:
            {
                OCI_Object* obj = OcilibObjectCreate(bnd->typinf);

                CHECK_NULL(obj);

                bnd->input       = (void**)obj;
                bnd->buffer.data = (void**)obj->handle;

                break;
            }
            case OCI_CDT_COLLECTION:
            {
                OCI_Coll* coll = OcilibCollectionCreate(bnd->typinf);

                CHECK_NULL(coll);

                bnd->input       = (void**)coll;
                bnd->buffer.data = (void**)coll->handle;

                break;
            }
            case OCI_CDT_REF:
            {
                OCI_Ref* ref = OcilibReferenceCreate(bnd->typinf);

                CHECK_NULL(ref);

                bnd->input       = (void**)ref;
                bnd->buffer.data = (void**)ref->handle;

                break;
            }
            case OCI_CDT_VECTOR:
            {
                OCI_Vector* vect = OcilibVectorCreate(bnd->stmt->con);

                CHECK_NULL(vect);

                bnd->input       = (void**)vect;
                bnd->buffer.data = (void**)vect->handle;

                break;
            }
            case OCI_CDT_JSON:
            {
                OCI_Json* json = OcilibJsonCreate(bnd->stmt->con);

                CHECK_NULL(json);

                bnd->input       = (void**)json;
                bnd->buffer.data = (void**)json->handle;

                break;
            }
        }
    }

    CHECK_NULL(bnd->input);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindAllocateBuffers
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibBindAllocateBuffers
(
    OCI_Bind   *bnd,
    OCI_UInt32  mode,
    OCI_Boolean reused,
    OCI_UInt32  nballoc,
    OCI_UInt32  nbelem,
    OCI_Boolean plsql_table
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    );

    CHECK_PTR(OCI_IPC_BIND, bnd);

    /* allocate indicators array */

    ALLOC_DATA(OCI_IPC_BIND, bnd->buffer.inds, nballoc);

    if (SQLT_NTY == bnd->code)
    {
        ALLOC_DATA(OCI_IPC_INDICATOR_ARRAY, bnd->buffer.obj_inds, nballoc);
    }

    /* check need for PL/SQL table extra info */

    if (plsql_table)
    {
        bnd->nbelem = nbelem;

        /* allocate array of returned codes */

        ALLOC_DATA(OCI_IPC_PLS_RCODE_ARRAY, bnd->plrcds, nballoc);
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
                (OCI_CDT_LONG    != bnd->type)  &&
                (OCI_CDT_BOOLEAN != bnd->type)  &&
                (OCI_CDT_NUMERIC != bnd->type || SQLT_VNU == bnd->code) &&
                (OCI_CDT_TEXT    != bnd->type || bnd->stmt->con->env->use_wide_char_conv)
                && (OCI_CDT_JSON != bnd->type || bnd->is_array)
                )
            {
                bnd->alloc = TRUE;

                if (reused)
                {
                    FREE(bnd->buffer.data);
                }

                ALLOC_BUFFER(OCI_IPC_BUFF_ARRAY, bnd->buffer.data, (OCI_Size) bnd->size, nballoc);
            }
            else if (OCI_CDT_JSON == bnd->type && !bnd->is_array)
            {
                if (reused && bnd->alloc)
                {
                    FREE(bnd->buffer.data);
                    bnd->alloc = FALSE;
                }

                bnd->buffer.data = (void **) &((OCI_Json *)bnd->input)->handle;
            }
            else
            {
                if (reused && bnd->alloc)
                {
                    FREE(bnd->buffer.data);
                    bnd->alloc = FALSE;
                }

                bnd->buffer.data = (void **)bnd->input;
            }
        }
    }

    /* setup data length array */

    if (OCI_CDT_RAW == bnd->type || OCI_CDT_TEXT == bnd->type)
    {
        ALLOC_BUFFER(OCI_IPC_BUFF_ARRAY, bnd->buffer.lens, sizeof(ub2), nballoc);

        /* initialize length array with buffer default size */

        for (OCI_UInt32 i = 0; i < nbelem; i++)
        {
            *(ub2*)(((ub1 *)bnd->buffer.lens) + sizeof(ub2) * (OCI_Size) i) = (ub2)bnd->size;
        }
    }

    /* internal allocation if needed */

    if (!bnd->input && (OCI_BAM_INTERNAL == bnd->alloc_mode))
    {
        CHECK(OcilibBindAllocateInternalData(bnd));
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
* OcilibBindCheckAvailability
* --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibBindCheckAvailability
(
    OCI_Statement *stmt,
    OCI_UInt32     mode,
    OCI_Boolean    reused
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    if (!reused)
    {
        if (OCI_BIND_INPUT == mode)
        {
            if (stmt->nb_ubinds >= OCI_BIND_MAX)
            {
                THROW_NO_ARGS(OcilibExceptionMaxBind);
            }

            /* allocate user bind array if necessary */

            REALLOC_DATA
            (
                OCI_IPC_BIND_ARRAY,
                stmt->ubinds,
                stmt->nb_ubinds,
                stmt->allocated_ubinds,
                min(stmt->nb_ubinds + OCI_BIND_ARRAY_GROWTH_FACTOR, OCI_BIND_MAX)
            );
        }
        else
        {
            if (stmt->nb_rbinds >= OCI_BIND_MAX)
            {
                THROW_NO_ARGS(OcilibExceptionMaxBind);
            }

            /* allocate register bind array if necessary */

            REALLOC_DATA
            (
                OCI_IPC_BIND_ARRAY,
                stmt->rbinds,
                stmt->nb_rbinds,
                stmt->allocated_rbinds,
                min(stmt->nb_rbinds + OCI_BIND_ARRAY_GROWTH_FACTOR, OCI_BIND_MAX)
            );
        }
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
  * OcilibBindPerformBinding
  * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibBindPerformBinding
(
    OCI_Bind   *bnd,
    OCI_UInt32  mode,
    OCI_UInt32  index,
    OCI_UInt32  exec_mode,
    OCI_Boolean plsql_table
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    );

    dbtext  * dbstr  = NULL;
    OCI_Int32 dbsize = -1;

    CHECK_PTR(OCI_IPC_BIND, bnd);

    if (OCI_BIND_BY_POS == bnd->stmt->bind_mode)
    {
        CHECK_OCI
        (
            bnd->stmt->con->err,
            OCIBindByPos,
            bnd->stmt->handle,
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
        );
    }
    else
    {
        dbstr = OcilibStringGetDBString(bnd->stmt->con->env, bnd->name, &dbsize);

        CHECK_OCI
        (
            bnd->stmt->con->err,
            OCIBindByName,
            bnd->stmt->handle,
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
        );
    }

    if (SQLT_NTY == bnd->code || SQLT_REF == bnd->code)
    {
        CHECK_OCI
        (
            bnd->stmt->con->err,
            OCIBindObject,
            (OCIBind *)bnd->buffer.handle,
            bnd->stmt->con->err,
            (OCIType *)bnd->typinf->handle,
            (void **)bnd->buffer.data,
            (ub4 *)NULL,
            (void **)bnd->buffer.obj_inds,
            (ub4 *)NULL
        );
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
            OcilibCallbackInBind,
            (dvoid *)bnd,
            OcilibCallbackOutBind
        );
    }

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(bnd->stmt->con->env, dbstr);
    );

}

/* --------------------------------------------------------------------------------------------- *
  * OcilibBindAddToStatement
  * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibBindAddToStatement
(
    OCI_Bind   *bnd,
    OCI_UInt32  mode,
    OCI_Boolean reused
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    );

    CHECK_PTR(OCI_IPC_BIND, bnd);

    if (OCI_BIND_INPUT == mode)
    {
        if (!reused)
        {
            bnd->stmt->ubinds[bnd->stmt->nb_ubinds++] = bnd;

            /* for user binds, add a positive index */

            CHECK(OcilibHashAddInt(bnd->stmt->map, bnd->name, bnd->stmt->nb_ubinds));
        }
    }
    else
    {
        /* for register binds, add a negative index */

        bnd->stmt->rbinds[bnd->stmt->nb_rbinds++] = bnd;

        const OCI_Int32 index = (OCI_Int32)bnd->stmt->nb_rbinds;

        CHECK(OcilibHashAddInt(bnd->stmt->map, bnd->name, -index));
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OcilibBindCreate
(
    OCI_Statement *stmt,
    void          *data,
    const otext   *name,
    OCI_UInt32     mode,
    ub4            size,
    ub1            type,
    OCI_SqlType    code,
    OCI_SubType    subtype,
    OCI_TypeInfo  *typinf,
    OCI_UInt32     nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Bind*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    OCI_Bind *bnd = NULL;

    ub4         exec_mode   = OCI_DEFAULT;
    OCI_Boolean plsql_table = FALSE;
    OCI_Boolean is_array    = FALSE;
    OCI_Boolean reused      = FALSE;
    OCI_Int32   index       = 0;
    OCI_Int32   prev_index  = -1;
    OCI_UInt32  nballoc     = nbelem;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);

    /* check index if necessary */

    if (OCI_BIND_BY_POS == stmt->bind_mode)
    {
        index = (OCI_Int32) ostrtol(&name[1], NULL, 10);

        if (index <= 0 || index > OCI_BIND_MAX)
        {
            THROW(OcilibExceptionOutOfBounds, index);
        }
    }

    /* check if the bind name has already been used */

    if (OCI_BIND_INPUT == mode)
    {
        prev_index = OcilibBindGetIndex(stmt, name);

        if (prev_index >= 0)
        {
            if (!stmt->bind_reuse)
            {
                THROW(OcilibExceptionBindAlreadyUsed, name);
            }
            else
            {
                bnd = stmt->ubinds[prev_index];

                if (bnd->type != type)
                {
                    THROW(OcilibExceptionRebindBadDatatype, name);
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

    CHECK(OcilibBindCheckAvailability(stmt, mode, reused));

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
        nballoc = (OCI_Size) stmt->nb_iters_init;
    }

    /* create hash table for mapping bind names / index */

    if (NULL == stmt->map)
    {
        stmt->map = OcilibHashCreate(stmt->con->env, OCI_HASH_DEFAULT_SIZE, OCI_HASH_INTEGER);
        CHECK_NULL(stmt->map);
    }

    /* allocate bind object */

    ALLOC_HANDLE(OCI_IPC_BIND, bnd);

    /* initialize bind object */

    bnd->stmt      = stmt;
    bnd->input     = (void **) data;
    bnd->type      = type;
    bnd->size      = (sb4) size;
    bnd->code      = (ub2) code;
    bnd->subtype   = (ub1) subtype;
    bnd->is_array  = is_array;
    bnd->typinf    = typinf;
    bnd->csfrm     = OCI_CSF_NONE;
    bnd->direction = OCI_BDM_IN_OUT;

    if (NULL == bnd->name)
    {
        bnd->name = OcilibStringDuplicate(bnd->stmt->con->env, name);
    }

    /* initialize buffer */

    bnd->buffer.count   = nbelem;
    bnd->buffer.sizelen = sizeof(ub2);

    CHECK(OcilibBindAllocateBuffers(bnd, mode, reused, nballoc, nbelem, plsql_table));

    /* if we bind an OCI_Long or any output bind, we need to change the
       execution mode to provide data at execute time */

    if (OCI_CDT_LONG == bnd->type)
    {
        OCI_Long *lg = (OCI_Long *)  bnd->input;

        lg->maxsize = size;
        exec_mode   = OCI_DATA_AT_EXEC;

        if (OCI_CLONG == bnd->subtype)
        {
            lg->maxsize /= (OCI_UInt32) sizeof(otext);
            lg->maxsize *= (OCI_UInt32) sizeof(dbtext);
        }
    }
    else if (OCI_BIND_OUTPUT == mode)
    {
        exec_mode = OCI_DATA_AT_EXEC;
    }

    /* OCI binding */

    CHECK(OcilibBindPerformBinding(bnd, mode, (OCI_UInt32) index, exec_mode, plsql_table));

    /* set charset form */

    if ((OCI_CDT_LOB == bnd->type) && (OCI_NCLOB == bnd->subtype))
    {
        ub1 csfrm = SQLCS_NCHAR;

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_BIND, OCI_ATTR_CHARSET_FORM,
            bnd->buffer.handle, &csfrm, sizeof(csfrm),
            bnd->stmt->con->err
        );
    }

    /* on success, we :
         - add the bind handle to the bind array
         - add the bind index to the map
    */

    CHECK(OcilibBindAddToStatement(bnd, mode, reused));

    CLEANUP_AND_EXIT_FUNC
    (
        /* on error, only free the bind if it was a new one */

        if (FAILURE && NULL != bnd && prev_index == -1)
        {
            OcilibBindFree(bnd);
            bnd = NULL;
        }

        SET_RETVAL(bnd);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibBindFree
(
    OCI_Bind *bnd
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    );

    CHECK_PTR(OCI_IPC_BIND, bnd);

    if (OCI_BAM_INTERNAL == bnd->alloc_mode)
    {
        if (bnd->is_array)
        {
            OcilibArrayFreeFromHandles(bnd->stmt->con->env, bnd->input);
        }
        else
        {
            switch (bnd->type)
            {
                case OCI_CDT_NUMERIC:
                case OCI_CDT_TEXT:
                case OCI_CDT_RAW:
                {
                    /* OCINumber binds */

                    if (bnd->type == OCI_CDT_NUMERIC && bnd->subtype == OCI_NUM_NUMBER)
                    {
                        OcilibFreeObjectFromType(bnd->input, bnd->type);
                    }
                    else
                    {
                        /* strings requiring otext / dbtext conversions and 64 bit integers */

                        OcilibMemoryFree(bnd->stmt->con->env, bnd->input);

                        if (bnd->alloc)
                        {
                            FREE(bnd->buffer.data);
                        }
                    }
                    break;
                }
                default:
                {
                    OcilibFreeObjectFromType(bnd->input, bnd->type);
                }
            }
        }
    }
    else
    {
        if (bnd->alloc)
        {
            FREE(bnd->buffer.data);
        }
    }

    OcilibErrorResetSource(NULL, bnd);

    FREE(bnd->buffer.inds);
    FREE(bnd->buffer.obj_inds);
    FREE(bnd->buffer.lens);
    FREE(bnd->buffer.tmpbuf);
    FREE(bnd->plrcds);
    FREE(bnd->name);
    FREE(bnd);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindGetIndex
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibBindGetIndex
(
    OCI_Statement* stmt,
    const otext  * name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Int32, -1,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_PTR(OCI_IPC_STRING,    name);

    CHECK_NULL(stmt->map);

    OCI_HashEntry* he = OcilibHashLookup(stmt->map, name, FALSE);

    OCI_Int32 index = -1;

    while (NULL != he)
    {
        /* no more entries or key matched => so we got it ! */

        if (NULL == he->next || OcilibStringCaseCompare(he->key, name) == 0)
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

        if (index > 0)
        {
            index--;
        }

            break;
        }
    }

    SET_RETVAL(index);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindSetNullIndicator
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibBindSetNullIndicator
(
    OCI_Bind  *bnd,
    OCI_UInt32 position,
    sb2        value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    );

    CHECK_PTR(OCI_IPC_BIND, bnd);

    if (bnd->buffer.inds)
    {
        bnd->buffer.inds[position] = value;
    }

    SET_SUCCESS();

    EXIT_FUNC();
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
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindGetType
 * --------------------------------------------------------------------------------------------- */

OCI_SqlType OcilibBindGetType
(
    OCI_Bind *bnd
)
{
    GET_PROP
    (
        /* result */ OCI_SqlType, OCI_UNKNOWN,
        /* handle */ OCI_IPC_BIND, bnd,
        /* member */ type
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindGetSubtype
 * --------------------------------------------------------------------------------------------- */

OCI_SubType OcilibBindGetSubtype
(
    OCI_Bind *bnd
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SubType, OCI_UNKNOWN,
        /* context */ OCI_IPC_BIND, bnd
    );

    CHECK_PTR(OCI_IPC_BIND, bnd);

    OCI_SubType type = OCI_UNKNOWN;

    if (OCI_CDT_NUMERIC   == bnd->type ||
        OCI_CDT_LONG      == bnd->type ||
        OCI_CDT_LOB       == bnd->type ||
        OCI_CDT_FILE      == bnd->type ||
        OCI_CDT_TIMESTAMP == bnd->type ||
        OCI_CDT_INTERVAL  == bnd->type)
    {
        type = (OCI_SubType)bnd->subtype;
    }

    SET_RETVAL(type);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindGetDataCount
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibBindGetDataCount
(
    OCI_Bind *bnd
)
{
    GET_PROP
    (
        /* result */ OCI_UInt32, 0,
        /* handle */ OCI_IPC_BIND, bnd,
        /* member */ buffer.count
    );
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
    );
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
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindSetDataSize
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibBindSetDataSize
(
    OCI_Bind  *bnd,
    OCI_UInt32 size
)
{
    return OcilibBindSetDataSizeAtPos(bnd, 0, size);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindSetDataSizeAtPos
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibBindSetDataSizeAtPos
(
    OCI_Bind  *bnd,
    OCI_UInt32 position,
    OCI_UInt32 size
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    );

    CHECK_PTR(OCI_IPC_BIND, bnd);
    CHECK_INDEX(position, bnd->buffer.count);
    CHECK_MIN(size, 1);

    CHECK_NULL(bnd->buffer.lens);

    if (OCI_CDT_TEXT == bnd->type)
    {
        if (bnd->size == (sb4) size)
        {
            size += (OCI_UInt32) (OCI_Size) sizeof(dbtext);
        }

        size *= (OCI_UInt32) sizeof(dbtext);
    }

    ((ub2 *) bnd->buffer.lens)[position] = (ub2) size;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindGetDataSize
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibBindGetDataSize
(
    OCI_Bind *bnd
)
{
    return OcilibBindGetDataSizeAtPos(bnd, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindGetDataSizeAtPos
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibBindGetDataSizeAtPos
(
    OCI_Bind  *bnd,
    OCI_UInt32 position
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_BIND, bnd
    );

    CHECK_PTR(OCI_IPC_BIND, bnd);
    CHECK_INDEX(position, bnd->buffer.count);

    CHECK_NULL(bnd->buffer.lens);

    OCI_UInt32 size = (OCI_UInt32)((ub2 *)bnd->buffer.lens)[position];

    if (OCI_CDT_TEXT == bnd->type)
    {
        if (bnd->size == (sb4)size)
        {
            size -= (OCI_UInt32) sizeof(dbtext);
        }

        size /= (OCI_UInt32) sizeof(dbtext);
    }

    SET_RETVAL(size);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindSetNullAtPos
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibBindSetNullAtPos
(
    OCI_Bind  *bnd,
    OCI_UInt32 position
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    );

    CHECK_PTR(OCI_IPC_BIND, bnd);
    CHECK_INDEX(position, bnd->buffer.count);

    CHECK(OcilibBindSetNullIndicator(bnd, position, OCI_IND_NULL));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindSetNull
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibBindSetNull
(
    OCI_Bind *bnd
)
{
    return OcilibBindSetNullAtPos(bnd, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindSetNotNullAtPos
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibBindSetNotNullAtPos
(
    OCI_Bind  *bnd,
    OCI_UInt32 position
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    );

    CHECK_PTR(OCI_IPC_BIND, bnd);
    CHECK_INDEX(position, bnd->buffer.count);

    CHECK(OcilibBindSetNullIndicator(bnd, position, OCI_IND_NOTNULL));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindSetNotNull
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibBindSetNotNull
(
    OCI_Bind *bnd
)
{
    return OcilibBindSetNotNullAtPos(bnd, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindIsNullAtPos
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibBindIsNullAtPos
(
    OCI_Bind  *bnd,
    OCI_UInt32 position
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, TRUE,
        /* context */ OCI_IPC_BIND, bnd
    );

    CHECK_PTR(OCI_IPC_BIND, bnd);
    CHECK_INDEX(position, bnd->buffer.count);

    CHECK_NULL(bnd->buffer.inds);

    SET_RETVAL(OCI_IND_NULL == bnd->buffer.inds[position]);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindIsNull
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibBindIsNull
(
    OCI_Bind *bnd
)
{
    return OcilibBindIsNullAtPos(bnd, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindSetCharsetForm
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibBindSetCharsetForm
(
    OCI_Bind       *bnd,
    OCI_CharsetForm csfrm
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_BIND, bnd
    );

    CHECK_PTR(OCI_IPC_BIND, bnd);
    CHECK_ENUM_VALUE(csfrm, CharsetFormValues, OTEXT("CharsetForm"));

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
        );
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindSetDirection
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibBindSetDirection
(
    OCI_Bind         *bnd,
    OCI_BindDirection direction
)
{
    SET_PROP_ENUM
    (
        /* handle */ OCI_IPC_BIND, bnd,
        /* member */ direction, ub1,
        /* value  */ direction, BindDirectionValues, OTEXT("Direction")
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibBindGetDirection
 * --------------------------------------------------------------------------------------------- */

OCI_BindDirection OcilibBindGetDirection
(
    OCI_Bind *bnd
)
{
    GET_PROP
    (
        /* result */ OCI_BindDirection, OCI_UNKNOWN,
        /* handle */ OCI_IPC_BIND, bnd,
        /* member */ direction
    );
}

/* --------------------------------------------------------------------------------------------- *
* OcilibBindGetAllocationMode
* --------------------------------------------------------------------------------------------- */

OCI_BindAllocationMode OcilibBindGetAllocationMode
(
    OCI_Bind *bnd
)
{
    GET_PROP
    (
        /* result */ OCI_BindAllocationMode, OCI_UNKNOWN,
        /* handle */ OCI_IPC_BIND, bnd,
        /* member */ alloc_mode
    );
}
