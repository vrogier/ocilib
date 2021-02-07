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

#include "define.h"

#include "connection.h"
#include "hash.h"
#include "macros.h"
#include "number.h"
#include "stringutils.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibDefineGet
 * --------------------------------------------------------------------------------------------- */

OCI_Define * OcilibDefineGet
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Define*, NULL,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)

    OCI_Define *def = NULL;

    if ((OCI_DESCRIBE_ONLY != rs->stmt->exec_mode) && (OCI_PARSE_ONLY != rs->stmt->exec_mode))
    {
        def =  &rs->defs[index-1];
    }

    SET_RETVAL(def)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDefineGetIndex
 * --------------------------------------------------------------------------------------------- */

int OcilibDefineGetIndex
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    ENTER_FUNC
    (
        /* returns */ int, -1,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    CHECK_PTR(OCI_IPC_RESULTSET, rs)

    if (NULL == rs->map)
    {
        /* create the map at the first call to save time and memory when it's not needed */

        rs->map = OcilibHashCreate(OCI_HASH_DEFAULT_SIZE, OCI_HASH_INTEGER);
        CHECK_NULL(rs->map)

        for (ub4 i = 0; i < rs->nb_defs; i++)
        {
            CHECK(OcilibHashAddInt(rs->map, rs->defs[i].col.name, (int)(i + 1)))
        }
    }

    /* check out we got our map object */

    int index = -1;

    OCI_HashEntry *he = OcilibHashLookup(rs->map, name, FALSE);

    while (NULL != he)
    {
        /* no more entries or key matched => so we got it ! */

        if (NULL == he->next || 0 == OcilibStringCaseCompare(he->key, name))
        {
            index = he->values->value.num;
            break;
        }

        he = he->next;
    }

    if (index < 0)
    {
        THROW(OcilibExceptionItemNotFound, name, OCI_IPC_COLUMN)
    }

    SET_RETVAL(index)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDefineGetData
 * --------------------------------------------------------------------------------------------- */

void * OcilibDefineGetData
(
    OCI_Define *def
)
{
    ENTER_FUNC
    (
        /* returns */ void*, NULL,
        /* context */ OCI_IPC_DEFINE, def
    )

    CHECK_PTR(OCI_IPC_DEFINE, def)

    void* data = NULL;

    switch (def->col.datatype)
    {
        case OCI_CDT_LONG:
        case OCI_CDT_CURSOR:
        case OCI_CDT_TIMESTAMP:
        case OCI_CDT_INTERVAL:
        case OCI_CDT_LOB:
        case OCI_CDT_FILE:
        case OCI_CDT_OBJECT:
        case OCI_CDT_COLLECTION:
        case OCI_CDT_REF:
        {
            /* handle based types */

            data =  def->buf.data[def->rs->row_cur-1];
            break;
        }
        default:
        {
            /* special case for ROWIDs exposed as strings but internally handled as rowid descriptors */

            if (IS_ROWID_COL(&def->col))
            {
                data = def->buf.data[def->rs->row_cur - 1];
            }
            else
            {
                /* scalar types */

                data = (((ub1 *) (def->buf.data)) + (size_t) (def->col.bufsize * (def->rs->row_cur-1)));
            }
        }
    }

    SET_RETVAL(data)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDefineIsDataNotNull
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDefineIsDataNotNull
(
    OCI_Define *def
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DEFINE, def
    )

    CHECK_PTR(OCI_IPC_DEFINE, def)

    OCIInd ind = OCI_IND_NULL;

    if (def->rs->row_cur > 0)
    {
        if (SQLT_NTY == def->col.sqlcode)
        {
            ind = *(OCIInd *)def->buf.obj_inds[def->rs->row_cur - 1];
        }
        else
        {
            ind = def->buf.inds[def->rs->row_cur - 1];
        }
    }

    SET_RETVAL(ind != OCI_IND_NULL)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDefineGetNumber
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDefineGetNumber
(
    OCI_Resultset *rs,
    unsigned int   index,
    void          *value,
    uword          type
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_RESULTSET, rs
    )

    OCI_Define* def = NULL;

    CHECK_PTR(OCI_IPC_RESULTSET, rs)

    def = OcilibDefineGet(rs, index);
    CHECK_NULL(def)

    if (OcilibDefineIsDataNotNull(def))
    {
        void* data = OcilibDefineGetData(def);

        switch (def->col.datatype)
        {
            case OCI_CDT_NUMERIC:
            {
                CHECK(OcilibNumberTranslateValue(rs->stmt->con, data,
                                                 def->col.subtype, value,
                                                 type))
                break;
            }
            case OCI_CDT_TEXT:
            {
                CHECK(OcilibNumberFromStringInternal(rs->stmt->con, value, type,
                                                     (const otext*)data, NULL))
                break;
            }
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDefineAlloc
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDefineAlloc
(
    OCI_Define *def
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DEFINE, def
    )

    ub4 i;

    CHECK_PTR(OCI_IPC_DEFINE, def)

    /* Allocate indicators */

    ALLOC_DATA(OCI_IPC_INDICATOR_ARRAY, def->buf.inds, def->buf.count)

    if (SQLT_NTY == def->col.sqlcode)
    {
        ALLOC_DATA(OCI_IPC_INDICATOR_ARRAY, def->buf.obj_inds, def->buf.count)
    }

    /* Allocate row data sizes array */

    ALLOC_BUFFER(OCI_IPC_LEN_ARRAY, def->buf.lens, def->buf.sizelen, def->buf.count)

    /* initialize length array with buffer default size.
       But, Oracle uses different sizes for static fetch and callback fetch....*/

    /* Allocate buffer array */

    const ub4 bufsize = (ub4)(OCI_CDT_LONG == def->col.datatype ? sizeof(OCI_Long *) : def->col.bufsize);

    ALLOC_BUFFER(OCI_IPC_BUFF_ARRAY, def->buf.data, bufsize, def->buf.count)

    /* Allocate buffer length array */

    for (i = 0; i < def->buf.count; i++)
    {
        if (def->buf.sizelen == (int) sizeof(ub2))
        {
            ARRAY_SET_AT(def->buf.lens, ub2, i, def->col.bufsize)
        }
        else if (def->buf.sizelen == (int) sizeof(ub4))
        {
            ARRAY_SET_AT(def->buf.lens, ub4, i, def->col.bufsize)
        }
    }

    /* Allocate descriptor for cursor, lob and file, interval and timestamp */

    if (OCI_UNKNOWN != def->col.handletype)
    {
        if (OCI_CDT_CURSOR == def->col.datatype)
        {
            for (i = 0; i < def->buf.count; i++)
            {
                CHECK(OcilibMemoryAllocHandle((dvoid  *)def->rs->stmt->con->env,
                                              (dvoid **) &(def->buf.data[i]),
                                              (ub4) def->col.handletype))
            }
        }
        else
        {
            CHECK(OcilibMemoryAllocDescriptorArray((dvoid  *)def->rs->stmt->con->env,
                                                   (dvoid **)def->buf.data, (ub4)def->col.handletype,
                                                   (ub4)def->buf.count))

            if (OCI_CDT_LOB == def->col.datatype)
            {
                ub4 empty = 0;

                for (i = 0; i < def->buf.count; i++)
                {
                    CHECK_ATTRIB_SET
                    (
                        def->col.handletype, OCI_ATTR_LOBEMPTY,
                        def->buf.data[i], &empty, sizeof(empty),
                        def->rs->stmt->con->err
                    )
                }
            }
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDefineDef
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDefineDef
(
    OCI_Define *def,
    ub4         position
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DEFINE, def
    )

    ub2 mode = OCI_DEFAULT;

    CHECK_PTR(OCI_IPC_DEFINE, def)

    /*check define mode for long columns */

    if (OCI_CDT_LONG == def->col.datatype)
    {
        mode = OCI_DYNAMIC_FETCH;
    }

    /* oracle defining */

    CHECK_OCI
    (
        def->rs->stmt->con->err,
        OCIDefineByPos,
        def->rs->stmt->stmt,
        (OCIDefine **) &def->buf.handle,
        def->rs->stmt->con->err,
        position,
        (void *) def->buf.data,
        (sb4   ) def->col.bufsize,
        (ub2   ) def->col.libcode,
        (void *) def->buf.inds,
        (ub2  *) def->buf.lens,
        (ub2  *) NULL,
        (ub4   ) mode
    )

    if (SQLT_NTY == def->col.sqlcode || SQLT_REF == def->col.sqlcode)
    {
        CHECK_OCI
        (
            def->rs->stmt->con->err,
            OCIDefineObject,
            (OCIDefine *)def->buf.handle,
            def->rs->stmt->con->err,
            def->col.typinf->tdo,
            (void **) def->buf.data,
            (ub4   *) NULL,
            (void **) def->buf.obj_inds,
            (ub4   *) NULL
        )
    }

    if(((OCI_CDT_TEXT == def->col.datatype))  ||
       ((OCI_CDT_LOB  == def->col.datatype)  && (OCI_BLOB  != def->col.subtype)) ||
       ((OCI_CDT_FILE == def->col.datatype)  && (OCI_BFILE != def->col.subtype)) ||
       ((OCI_CDT_LONG == def->col.datatype)  && (OCI_BLONG != def->col.subtype)))
    {

        if ((SQLCS_NCHAR == def->col.csfrm) || Env.nls_utf8)
        {
            ub1 csfrm = SQLCS_NCHAR;

            CHECK_ATTRIB_SET
            (
                OCI_HTYPE_DEFINE, OCI_ATTR_CHARSET_FORM,
                def->buf.handle, &csfrm, sizeof(csfrm),
                def->rs->stmt->con->err
            )
        }
    }

#if OCI_VERSION_COMPILE >= OCI_11_1

    if (OcilibConnectionIsVersionSupported(def->rs->stmt->con, OCI_11_1))
    {
        if (OCI_CDT_LOB == def->col.datatype)
        {
            ub2 value = 1;

            CHECK_ATTRIB_SET
            (
                OCI_HTYPE_DEFINE, OCI_ATTR_LOBPREFETCH_LENGTH,
                def->buf.handle, &value, sizeof(value),
                def->rs->stmt->con->err
            )
        }
    }

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}
