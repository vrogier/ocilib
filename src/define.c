/*
   +----------------------------------------------------------------------+   
   |                                                                      |
   |                     OCILIB - C Driver for Oracle                     |
   |                                                                      |
   |                      (C Wrapper for Oracle OCI)                      |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                      Website : http://ocilib.net                     |
   +----------------------------------------------------------------------+
   |               Copyright (c) 2007-2009 Vincent ROGIER                 |
   +----------------------------------------------------------------------+
   | This library is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU Library General Public          |
   | License as published by the Free Software Foundation; either         |
   | version 2 of the License, or (at your option) any later version.     |
   |                                                                      |
   | This library is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    |
   | Library General Public License for more details.                     |
   |                                                                      |
   | You should have received a copy of the GNU Library General Public    |
   | License along with this library; if not, write to the Free           |
   | Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   |
   +----------------------------------------------------------------------+
   |          Author: Vincent ROGIER <vince.rogier@gmail.com>             |
   +----------------------------------------------------------------------+
*/

/* ------------------------------------------------------------------------ *
 * $Id: define.c, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_GetDefine
 * ------------------------------------------------------------------------ */

OCI_Define * OCI_GetDefine(OCI_Resultset *rs, unsigned int index)
{
    OCI_CHECK_PTR(OCI_IPC_RESULTSET, rs, NULL);
    OCI_CHECK_BOUND(rs->stmt->con, index,  1,  rs->nb_defs, NULL);

    return &rs->defs[index-1];
}

/* ------------------------------------------------------------------------ *
 * OCI_GetDefineIndex
 * ------------------------------------------------------------------------ */

int OCI_GetDefineIndex(OCI_Resultset *rs, const mtext *name)
{
    OCI_HashEntry *he = NULL;
    int index         = -1;

    OCI_CHECK_PTR(OCI_IPC_RESULTSET, rs, -1);
    OCI_CHECK_PTR(OCI_IPC_STRING, name, -1);

    if (rs->map == NULL)
    {
        /* create the map at the first call to OCI_Getxxxxx2() to save
           time and memory when it's not needed */

        rs->map = OCI_HashCreate(OCI_HASH_DEFAULT_SIZE, OCI_HASH_INTEGER);

        if (rs->map != NULL)
        {
            ub4 i;

            for (i = 0; i < rs->nb_defs; i++)
            {
               OCI_HashAddInt(rs->map, rs->defs[i].col.name, (i+1));
            }
        }
    }

    /* check out we got our map object */

    OCI_CHECK(rs->map == NULL, -1);

    he = OCI_HashLookup(rs->map, name, FALSE);

    while (he != NULL)
    {
        /* no more entries or key matched => so we got it ! */

        if (he->next == NULL || mtscasecmp(he->key, name) == 0)
        {
            index = he->values->value.num;
            break;
        }
    }

    return index;
}

/* ------------------------------------------------------------------------ *
 * OCI_DefineGetData
 * ------------------------------------------------------------------------ */

void * OCI_DefineGetData(OCI_Define *def)
{
    OCI_CHECK(def == NULL, NULL);
    OCI_CHECK(def->rs->row_cur < 1, NULL);

    switch (def->col.type)
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

            /* handle based types */

            return def->buf.data[def->rs->row_cur-1];

        default:

            /* scalar types */

            return (((ub1*)def->buf.data) + (def->col.bufsize  * (def->rs->row_cur-1)));
    }
}

/* ------------------------------------------------------------------------ *
 * OCI_DefineGetNumber
 * ------------------------------------------------------------------------ */

boolean OCI_DefineGetNumber(OCI_Resultset *rs, unsigned int index, void *value,
                            uword type, uword size)
{
    OCI_Define *def = OCI_GetDefine(rs, index);
    boolean res     = FALSE;

    if (OCI_NOT_NULL(def) == TRUE)
    {
        void *data = OCI_DefineGetData(def);

        switch (def->col.type)
        {
            case OCI_CDT_NUMERIC:
            {
                res = OCI_NumberGet(rs->stmt->con, (OCINumber *) data, value,
                                    size, type);
                break;
            }
            case OCI_CDT_TEXT:
            {
                const mtext *fmt = OCI_GetDefaultFormatNumeric(rs->stmt->con);
                ub4 fmt_size     = (ub4) mtslen(fmt);

                res = OCI_NumberGetFromStr(rs->stmt->con, value, size, type,
                                           (dtext *) data,
                                           (int) dtslen((dtext *) data),
                                           fmt, fmt_size);
                break;
            }
        }
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DefineAlloc
 * ------------------------------------------------------------------------ */

boolean OCI_DefineAlloc(OCI_Define *def)
{
    boolean res = TRUE;
    ub4 bufsize = 0;
    ub4 indsize = 0;
    ub4 i;

    /* this function allocates internal buffers, handles, indicators, arrays, ...
       for the given output define handle */

    OCI_CHECK(def == NULL, FALSE);

    /* Allocate null indicators array */

    if (def->col.ocode == SQLT_NTY || def->col.ocode == SQLT_REF)
        indsize = (ub4) sizeof(void*);
    else
        indsize = (ub4) sizeof(sb2);

    if (res == TRUE)
    {
        def->buf.inds = (void *) OCI_MemAlloc(OCI_IPC_INDICATOR_ARRAY,
                                              (int) indsize, def->buf.count, TRUE);
        res = (def->buf.inds != NULL);
    }

    /* Allocate row data sizes array */

    if (res == TRUE)
    {
        def->buf.lens = (void *) OCI_MemAlloc(OCI_IPC_LEN_ARRAY, def->buf.sizelen,
                                              def->buf.count, TRUE);

        res = (def->buf.lens != NULL);
    }

    /* initialize length array with buffer default size.
       But, Oracle uses different sizes for static fetch and callback fetch....*/

    if (res == TRUE)
    {
        for (i=0; i < def->buf.count; i++)
        {
            if (def->buf.sizelen == sizeof(ub2))
                *(ub2*)(((ub1 *)def->buf.lens) + def->buf.sizelen*(i)) = (ub2) def->col.bufsize;
            else if (def->buf.sizelen == sizeof(ub4))
                *(ub4*)(((ub1 *)def->buf.lens) + def->buf.sizelen*(i)) = (ub4) def->col.bufsize;
       }
    }

    /* Allocate buffer array */

    if (res == TRUE)
    {
        if (def->col.type == OCI_CDT_LONG)
            bufsize = (ub4) sizeof(OCI_Long *);
        else
            bufsize = def->col.bufsize;

        def->buf.data = (void *) OCI_MemAlloc(OCI_IPC_BUFF_ARRAY, (int) bufsize,
                                              def->buf.count, TRUE);

        res = (def->buf.data != NULL);
    }

    /* Allocate descriptor for cursor, lob and file, interval and timestamp */

    if (res == TRUE)
    {
        if (def->col.dtype != 0)
        {
            for (i = 0; i < def->buf.count; i++)
            {
                if (def->col.type == OCI_CDT_CURSOR)
                {
                    res = (OCI_SUCCESS == OCI_HandleAlloc((dvoid  *) OCILib.env,
                                                          (dvoid **) &(def->buf.data[i]),
                                                          (ub4) def->col.dtype,
                                                          (size_t) 0, (dvoid **) NULL));
                }
                else
                {
                    res = (OCI_SUCCESS == OCI_DescriptorAlloc((dvoid  *) OCILib.env,
                                                              (dvoid **) &(def->buf.data[i]),
                                                              (ub4) def->col.dtype,
                                                              (size_t) 0, (dvoid **) NULL));

                    if ((res == TRUE) && (def->col.type == OCI_CDT_LOB))
                    {
                        ub4 empty = 0;

                        OCI_CALL1
                        (
                            res, def->rs->stmt->con, def->rs->stmt,

                            OCIAttrSet((dvoid *) def->buf.data[i],
                                       (ub4) def->col.dtype,
                                       (void *) &empty, (ub4) sizeof(empty),
                                       (ub4) OCI_ATTR_LOBEMPTY,
                                       def->rs->stmt->con->err)
                        )
                    }
                }

                if (res == FALSE)
                    break;
            }
        }
    }

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DefineDef
 * ------------------------------------------------------------------------ */

boolean OCI_DefineDef(OCI_Define *def)
{
    boolean res    = TRUE;
    ub2 fetch_mode = 0;

    OCI_CHECK(def == NULL, FALSE);

    /*check define mode for long columns */

    if (def->col.type == OCI_CDT_LONG)
        fetch_mode = OCI_DYNAMIC_FETCH;
    else
        fetch_mode = OCI_DEFAULT;

    /* oracle defining */

    OCI_CALL1
    (
        res, def->rs->stmt->con, def->rs->stmt,

        OCIDefineByPos(def->rs->stmt->stmt,
                       (OCIDefine **) &def->buf.handle,
                       def->rs->stmt->con->err,
                       def->rs->nb_defs,
                       (void *) def->buf.data,
                       (sb4   ) def->col.bufsize,
                       (ub2   ) def->col.icode,
                       (void *) def->buf.inds,
                       (ub2  *) def->buf.lens,
                       (ub2  *) NULL,
                       (ub4   ) fetch_mode)
    )

    if (def->col.ocode == SQLT_NTY || def->col.ocode == SQLT_REF)
    {
        OCI_CALL1
        (
            res, def->rs->stmt->con, def->rs->stmt,

            OCIDefineObject((OCIDefine *) def->buf.handle,
                            def->rs->stmt->con->err,
                            def->col.typinf->tdo,
                            (void **) def->buf.data,
                            (ub4   *) NULL,
                           (void **) def->buf.inds,
                            (ub4   *) NULL)
        )
    }

    if (def->col.csfrm == SQLCS_NCHAR
#ifdef OCI_USERDATA_UNICODE
        || (def->col.type == OCI_CDT_TEXT && OCILib.ver_runtime >= OCI_9)
#endif
        )
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

#ifdef OCI_CHARSET_MIXED

    /* setup Unicode mode for user data on mixed builds */
    {
        ub2 csid = OCI_UTF16ID;

        OCI_CALL1
        (
            res, def->rs->stmt->con, def->rs->stmt,

            OCIAttrSet((dvoid *) def->buf.handle,
                       (ub4    ) OCI_HTYPE_DEFINE,
                       (dvoid *) &csid,
                       (ub4    ) sizeof(csid),
                       (ub4    ) OCI_ATTR_CHARSET_ID,
                       def->rs->stmt->con->err)
        )
    }

#endif

    return res;
}
