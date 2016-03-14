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
 * OCI_GetDefine
 * --------------------------------------------------------------------------------------------- */

OCI_Define * OCI_GetDefine
(
    OCI_Resultset *rs,
    unsigned int   index
)
{
    OCI_Define * def = NULL;

    if ((OCI_DESCRIBE_ONLY != rs->stmt->exec_mode) && (OCI_PARSE_ONLY != rs->stmt->exec_mode))
    {
        def =  &rs->defs[index-1];
    }

    return def;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDefineIndex
 * --------------------------------------------------------------------------------------------- */

int OCI_GetDefineIndex
(
    OCI_Resultset *rs,
    const otext   *name
)
{
    OCI_HashEntry *he = NULL;
    int index         = -1;

    if (!rs->map)
    {
        /* create the map at the first call to OCI_Getxxxxx2() to save
           time and memory when it's not needed */

        rs->map = OCI_HashCreate(OCI_HASH_DEFAULT_SIZE, OCI_HASH_INTEGER);

        if (rs->map)
        {
            ub4 i;

            for (i = 0; i < rs->nb_defs; i++)
            {
                OCI_HashAddInt(rs->map, rs->defs[i].col.name, (i+1));
            }
        }
    }

    /* check out we got our map object */

    OCI_CHECK(NULL == rs->map, -1);

    he = OCI_HashLookup(rs->map, name, FALSE);

    while (he)
    {
        /* no more entries or key matched => so we got it ! */

        if (!he->next || 0 == ostrcasecmp(he->key, name))
        {
            index = he->values->value.num;
            break;
        }

        he = he->next;
    }

    if (index < 0)
    {
        OCI_ExceptionItemNotFound(rs->stmt->con, rs->stmt, name, OCI_IPC_COLUMN);
    }

    return index;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DefineGetData
 * --------------------------------------------------------------------------------------------- */

void * OCI_DefineGetData
(
    OCI_Define *def
)
{
    OCI_CHECK(NULL == def, NULL);
    OCI_CHECK(NULL == def->rs, NULL);
    OCI_CHECK(def->rs->row_cur < 1, NULL);

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

            return def->buf.data[def->rs->row_cur-1];
        }
        default:
        {
            /* scalar types */

            return (((ub1 *) (def->buf.data)) + (size_t) (def->col.bufsize * (def->rs->row_cur-1)));
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DefineIsDataNotNull
 * --------------------------------------------------------------------------------------------- */

boolean OCI_DefineIsDataNotNull
(
    OCI_Define *def
)
{
    boolean res = FALSE;
    
    if (def && def->rs->row_cur > 0)
    {
        OCIInd ind = OCI_IND_NULL;

        if (OCI_CDT_OBJECT == def->col.datatype)
        {
           ind =  * (OCIInd *) def->buf.obj_inds[def->rs->row_cur-1];
        }
        else
        {
            ind = ((OCIInd *) (def->buf.inds))[def->rs->row_cur-1];

        }
        
        res = (ind != OCI_IND_NULL);
    }

    return res;
}


/* --------------------------------------------------------------------------------------------- *
 * OCI_DefineGetNumber
 * --------------------------------------------------------------------------------------------- */

boolean OCI_DefineGetNumber
(
    OCI_Resultset *rs,
    unsigned int   index,
    void          *value,
    uword          type,
    uword          size
)
{
    OCI_Define *def = NULL; 
    boolean     res = FALSE;

    def = OCI_GetDefine(rs, index);

    if (OCI_DefineIsDataNotNull(def))
    {
        void *data = OCI_DefineGetData(def);

        switch (def->col.datatype)
        {
            case OCI_CDT_NUMERIC:
            {
                res = OCI_NumberGet(rs->stmt->con, data, size, type, def->col.libcode, value);
                break;
            }
            case OCI_CDT_TEXT:
            {
                res = OCI_NumberFromString(rs->stmt->con, value, size, type, def->col.libcode,
                                           (const otext *) data, NULL);
                break;
            }
        }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DefineAlloc
 * --------------------------------------------------------------------------------------------- */

boolean OCI_DefineAlloc
(
    OCI_Define *def
)
{
    boolean res = TRUE;
    ub4 indsize = 0;
    ub4 i;

    /* this function allocates internal buffers, handles, indicators, arrays, ...
       for the given output define handle */

    OCI_CHECK(NULL == def, FALSE)

    /* Allocate null indicators array */

    if (SQLT_NTY == def->col.sqlcode || SQLT_REF == def->col.sqlcode)
    {
        indsize = (ub4) sizeof(void*);
    }
    else
    {
        indsize = (ub4) sizeof(sb2);
    }

    def->buf.inds = (void *) OCI_MemAlloc(OCI_IPC_INDICATOR_ARRAY, (size_t) indsize,
                                          (size_t) def->buf.count, TRUE);
    res = (NULL != def->buf.inds);

    if (OCI_CDT_OBJECT == def->col.datatype)
    {
        def->buf.obj_inds = (void **) OCI_MemAlloc(OCI_IPC_INDICATOR_ARRAY, sizeof(void *),
                                                   (size_t) def->buf.count, TRUE);
        res = (NULL != def->buf.obj_inds);
    }

    /* Allocate row data sizes array */

    if (res)
    {
        def->buf.lens = (void *) OCI_MemAlloc(OCI_IPC_LEN_ARRAY, (size_t) def->buf.sizelen,
                                              (size_t) def->buf.count, TRUE);
        res = (NULL != def->buf.lens);
    }

    /* initialize length array with buffer default size.
       But, Oracle uses different sizes for static fetch and callback fetch....*/

    if (res)
    {
       ub4 bufsize = 0;

        for (i=0; i < def->buf.count; i++)
        {
            if (def->buf.sizelen == (int) sizeof(ub2))
            {
                *(ub2*)(((ub1 *)def->buf.lens) + (size_t) (def->buf.sizelen*i)) = (ub2) def->col.bufsize;
            }
            else if (def->buf.sizelen == (int) sizeof(ub4))
            {
                *(ub4*)(((ub1 *)def->buf.lens) + (size_t) (def->buf.sizelen*i)) = (ub4) def->col.bufsize;
            }
        }

        /* Allocate buffer array */

        if (OCI_CDT_LONG == def->col.datatype)
        {
            bufsize = (ub4) sizeof(OCI_Long *);
        }
        else
        {
            bufsize = def->col.bufsize;
        }

        def->buf.data = (void **) OCI_MemAlloc(OCI_IPC_BUFF_ARRAY, (size_t) bufsize,
                                                (size_t) def->buf.count, TRUE);

        res = (NULL != def->buf.data);
    }

    /* Allocate descriptor for cursor, lob and file, interval and timestamp */

    if (res && OCI_UNKNOWN != def->col.handletype)
    {
        if (OCI_CDT_CURSOR == def->col.datatype)
        {
            for (i = 0; (i < def->buf.count) && res; i++)
            {
                res = OCI_SUCCESSFUL(OCI_HandleAlloc((dvoid  *) def->rs->stmt->con->env,
                                                        (dvoid **) &(def->buf.data[i]),
                                                        (ub4) def->col.handletype,
                                                        (size_t) 0, (dvoid **) NULL));
            }
        }
        else
        {
            res = OCI_SUCCESSFUL(OCI_DescriptorArrayAlloc((dvoid  *) def->rs->stmt->con->env,
                                                            (dvoid **) def->buf.data,
                                                            (ub4) def->col.handletype,
                                                            (ub4) def->buf.count,
                                                            (size_t) 0, (dvoid **) NULL));

            if (res && (OCI_CDT_LOB == def->col.datatype))
            {
                ub4 empty = 0;

                for (i = 0; (i < def->buf.count) && res; i++)
                {
                    OCI_CALL1
                    (
                        res, def->rs->stmt->con, def->rs->stmt,

                        OCIAttrSet((dvoid *) def->buf.data[i],  (ub4) def->col.handletype,
                                   (void *) &empty, (ub4) sizeof(empty),
                                   (ub4) OCI_ATTR_LOBEMPTY, def->rs->stmt->con->err)
                    )
                }
            }
        }
    }
 
    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DefineDef
 * --------------------------------------------------------------------------------------------- */

boolean OCI_DefineDef
(
    OCI_Define *def,
    ub4         position
)
{
    boolean res  = TRUE;
    ub2     mode = OCI_DEFAULT;

    OCI_CHECK(NULL == def, FALSE)

    /*check define mode for long columns */

    if (OCI_CDT_LONG == def->col.datatype)
    {
        mode = OCI_DYNAMIC_FETCH;
    }

    /* oracle defining */

    OCI_CALL1
    (
        res, def->rs->stmt->con, def->rs->stmt,

        OCIDefineByPos(def->rs->stmt->stmt,
                       (OCIDefine **) &def->buf.handle,
                       def->rs->stmt->con->err,
                       position,
                       (void *) def->buf.data,
                       (sb4   ) def->col.bufsize,
                       (ub2   ) def->col.libcode,
                       (void *) def->buf.inds,
                       (ub2  *) def->buf.lens,
                       (ub2  *) NULL,
                       (ub4   ) mode)
    )

    if (SQLT_NTY == def->col.sqlcode || SQLT_REF == def->col.sqlcode)
    {
        OCI_CALL1
        (
            res, def->rs->stmt->con, def->rs->stmt,

            OCIDefineObject((OCIDefine *) def->buf.handle,
                            def->rs->stmt->con->err,
                            def->col.typinf->tdo,
                            (void **) def->buf.data,
                            (ub4   *) NULL,
                            (void **) def->buf.obj_inds,
                            (ub4   *) NULL)
        )
    }

    if(((OCI_CDT_TEXT == def->col.datatype))  ||
       ((OCI_CDT_LOB  == def->col.datatype)  && (OCI_BLOB  != def->col.subtype)) ||
       ((OCI_CDT_FILE == def->col.datatype)  && (OCI_BFILE != def->col.subtype)) ||
       ((OCI_CDT_LONG == def->col.datatype)  && (OCI_BLONG != def->col.subtype)))
    {

        if ((SQLCS_NCHAR == def->col.csfrm) || OCILib.nls_utf8)
        {
            ub1 csfrm = SQLCS_NCHAR;

            OCI_CALL1
            (
                res, def->rs->stmt->con, def->rs->stmt,

                OCIAttrSet((dvoid *) def->buf.handle,
                           (ub4    ) OCI_HTYPE_DEFINE,
                           (dvoid *) &csfrm,
                           (ub4    ) sizeof(csfrm),
                           (ub4    ) OCI_ATTR_CHARSET_FORM,
                           def->rs->stmt->con->err)
            )
        }
    }

    return res;
}
