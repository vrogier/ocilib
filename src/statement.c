/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2014 Vincent ROGIER <vince.rogier@ocilib.net>            |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |             This library is free software; you can redistribute it and/or               |
    |             modify it under the terms of the GNU Lesser General Public                  |
    |             License as published by the Free Software Foundation; either                |
    |             version 2 of the License, or (at your option) any later version.            |
    |                                                                                         |
    |             This library is distributed in the hope that it will be useful,             |
    |             but WITHOUT ANY WARRANTY; without even the implied warranty of              |
    |             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           |
    |             Lesser General Public License for more details.                             |
    |                                                                                         |
    |             You should have received a copy of the GNU Lesser General Public            |
    |             License along with this library; if not, write to the Free                  |
    |             Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.          |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
*/

/* --------------------------------------------------------------------------------------------- *
 * $Id: statement.c, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

#define SET_ARG_NUM(type, func)                                     \
    type src = func(rs, i), *dst = ( type *) va_arg(args, type *);  \
    if (dst)                                                        \
    {                                                               \
        *dst = src;                                                 \
    }                                                               \

#define SET_ARG_HANDLE(type, func, assign)                          \
    type *src = func(rs, i), *dst = ( type *) va_arg(args, type *); \
    if (src && dst)                                                 \
    {                                                               \
       res = assign(dst, src);                                      \
    }                                                               \

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindFreeAll
 * --------------------------------------------------------------------------------------------- */

boolean OCI_BindFreeAll
(
    OCI_Statement *stmt
)
{
    int i;

    OCI_CHECK(stmt == NULL, FALSE);

    /* free user binds */

    if (stmt->ubinds)
    {
        for(i = 0; i < stmt->nb_ubinds; i++)
        {
            OCI_BindFree(stmt->ubinds[i]);
        }

        OCI_FREE(stmt->ubinds);
    }

    /* free register binds */

    if (stmt->rbinds)
    {
        for(i = 0; i < stmt->nb_rbinds; i++)
        {
            OCI_BindFree(stmt->rbinds[i]);
        }

        OCI_FREE(stmt->rbinds);
    }

    stmt->nb_ubinds = 0;
    stmt->nb_rbinds = 0;

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindCheck
 * --------------------------------------------------------------------------------------------- */

boolean OCI_BindCheck
(
    OCI_Statement *stmt
)
{
    boolean res   = TRUE;
    OCI_Bind *bnd = NULL;
    sb2 *ind      = NULL;
    ub4 i, j;

    OCI_CHECK(stmt == NULL, FALSE)
    OCI_CHECK(stmt->ubinds == NULL, TRUE);

    for(i = 0; i < stmt->nb_ubinds; i++)
    {
        bnd = stmt->ubinds[i];
        ind = (sb2 *) bnd->buffer.inds;

        if (OCI_CDT_CURSOR == bnd->type)
        {
            OCI_Statement *bnd_stmt = (OCI_Statement *) bnd->buffer.data;

            OCI_StatementReset(bnd_stmt);

            bnd_stmt->hstate = OCI_OBJECT_ALLOCATED_BIND_STMT;
        
            /* allocate stmt handle */

            res = OCI_SUCCESSFUL(OCI_HandleAlloc((dvoid *) bnd_stmt->con->env,
                                                 (dvoid **) (void *) &bnd_stmt->stmt,
                                                 (ub4) OCI_HTYPE_STMT,
                                                 (size_t) 0, (dvoid **) NULL));
            if (res)
            {
                ub4 size = 0;

                size = bnd_stmt->prefetch_size ? bnd_stmt->prefetch_size : OCI_PREFETCH_SIZE;
                res  = (res && OCI_SetPrefetchSize(bnd_stmt, size));
                
                size = bnd_stmt->fetch_size ? bnd_stmt->fetch_size : OCI_FETCH_SIZE;
                res  = (res && OCI_SetFetchSize(bnd_stmt, size));
            }
        }

        if (bnd->direction & OCI_BDM_IN)
        {
            /* for strings, re-initialize length array with buffer default size */

            if (OCI_CDT_TEXT == bnd->type)
            {
                for (j=0; j < bnd->buffer.count; j++)
                {
                    *(ub2*)(((ub1 *)bnd->buffer.lens) + (sizeof(ub2) * (size_t) j)) = (ub2) bnd->size;
                }
            }

            /* extra work for internal allocated binds buffers */

            if (!bnd->is_array)
            {
                /* - For big integer (64 bits), we use an OCINumber.

                   - Oracle date/time type is the only non scalar type
                     implemented by oracle through a public structure instead
                     of using a handle. So we need to copy the value
                */

                if ((OCI_CDT_NUMERIC == bnd->type) && (SQLT_VNU == bnd->code))
                {
                    res = OCI_NumberSet(stmt->con,  (OCINumber *) bnd->buffer.data,
                                        (uword) sizeof(big_int), bnd->subtype, bnd->code, 
                                        (void *) bnd->input);
                }
                else if (bnd->alloc)
                {
                    if (OCI_CDT_DATETIME == bnd->type)
                    {
                        if (bnd->input)
                        {
                            memcpy((void *) bnd->buffer.data, ((OCI_Date *) bnd->input)->handle, sizeof(OCIDate));
                        }
                    } 
                    else if (OCI_CDT_TEXT == bnd->type)
                    {
                        if (OCILib.use_wide_char_conv)
                        {
                            /* need conversion if bind buffer was allocated */

                            OCI_StringUTF32ToUTF16(bnd->input, bnd->buffer.data, bnd->size);
                        }
                    }
                    else
                    {
                        if (bnd->input)
                        {
                            bnd->buffer.data[0] = ((OCI_Datatype *) bnd->input)->handle;
                        }
                    }
                }

                /* for handles, check anyway the value for null data */

                if ((OCI_CDT_NUMERIC != bnd->type) &&
                    (OCI_CDT_TEXT    != bnd->type) &&
                    (OCI_CDT_RAW     != bnd->type)  &&
                    (OCI_CDT_OBJECT  != bnd->type))
                {
                    if (ind && *ind != ((sb2) OCI_IND_NULL))
                    {
                        *ind = OCI_IND(bnd->buffer.data);
                    }
                }

                /* update bind object indicator pointer with object indicator */

                if (OCI_CDT_OBJECT == bnd->type)
                {
                   if (*ind != ((sb2) OCI_IND_NULL) && bnd->buffer.data)
                   {
                        bnd->buffer.obj_inds[0] = ((OCI_Object *) bnd->input)->tab_ind;
                   }
                   else
                   {
                       *ind = bnd->buffer.null_inds[0] = OCI_IND_NULL;

                       bnd->buffer.obj_inds[0] = (void* ) &bnd->buffer.null_inds[0];
                   }
                }

                if (!res)
                {
                    break;
                }
            }
            else
            {
                for (j = 0; j < bnd->buffer.count; j++, ind++)
                {

                    /* - For big integer (64 bits), we use an OCINumber.

                       - Oracle date/time type is the only non scalar type
                         implemented by oracle through a public structure instead
                         of using a handle. So we need to copy the value
                    */

                    if ((OCI_CDT_NUMERIC == bnd->type) && (SQLT_VNU == bnd->code))
                    {
                        res = OCI_NumberSet(stmt->con,
                                            (OCINumber *) ((ub1 *) bnd->buffer.data +
                                            (size_t) (j*bnd->size)),
                                            (uword) sizeof(big_int), bnd->subtype, bnd->code,
                                            (void *) (((ub1 *) bnd->input) +
                                            (((size_t)j)*sizeof(big_int))));
                    }
                    else if (bnd->alloc)
                    {
                        if (bnd->type == OCI_CDT_DATETIME)
                        {
                            if (bnd->input[j])
                            {
                                memcpy(((ub1 *) bnd->buffer.data) + (size_t) (j*bnd->size),
                                       ((OCI_Date *) bnd->input[j])->handle, sizeof(OCIDate));
                            }
                        }  
                        else if (OCI_CDT_TEXT == bnd->type)
                        {
                            if (OCILib.use_wide_char_conv)
                            {
                                /* need conversion if bind buffer was allocated */

                                int offset1 = (bnd->size/sizeof(dbtext))*sizeof(otext);
                                int offset2 = bnd->size;

                                OCI_StringUTF16ToUTF32( (((ub1 *) bnd->input      ) + (j*offset1)),
                                                        (((ub1 *) bnd->buffer.data) + (j*offset2)),
                                                        bnd->size/sizeof(otext)-1);                               
                            }
                        }
                        else
                        {
                            if (bnd->input[j])
                            {
                                bnd->buffer.data[j] = ((OCI_Datatype *) bnd->input[j])->handle;
                            }
                        }
                    }

                    /* for handles, check anyway the value for null data */

                    if ((OCI_CDT_NUMERIC != bnd->type) &&
                        (OCI_CDT_TEXT    != bnd->type) &&
                        (OCI_CDT_RAW     != bnd->type) &&
                        (OCI_CDT_OBJECT  != bnd->type))
                    {
                        if (ind && *ind != ((sb2) OCI_IND_NULL))
                        {
                            *ind = OCI_IND((((OCI_Datatype *) bnd->input[j])->handle));
                        }
                    }

                    /* update bind object indicator pointer with object indicator */

                    if (OCI_CDT_OBJECT == bnd->type)
                    {
                        if (*ind != ((sb2) OCI_IND_NULL) && bnd->buffer.data)
                        {
                            bnd->buffer.obj_inds[j] = ((OCI_Object *) bnd->input[j])->tab_ind;
                        }
                        else
                        {
                            *ind = bnd->buffer.null_inds[j] = OCI_IND_NULL;

                            bnd->buffer.obj_inds[j] = (void* ) &bnd->buffer.null_inds[j];
                        }
                    }

                    if (!res)
                    {
                        break;
                    }
                }
            }
        }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindReset
 * --------------------------------------------------------------------------------------------- */

boolean OCI_BindReset
(
    OCI_Statement *stmt
)
{
    ub4 i, j;
    boolean res = TRUE;

    OCI_CHECK(stmt == NULL, FALSE)
    OCI_CHECK(stmt->ubinds == NULL, FALSE);

    /* avoid unused param warning from compiler */

    i = j = 0;

    for(i = 0; i < stmt->nb_ubinds; i++)
    {
        OCI_Bind *bnd = stmt->ubinds[i];

        if (OCI_CDT_CURSOR == bnd->type)
        {
            OCI_Statement *bnd_stmt = (OCI_Statement *) bnd->buffer.data;

            bnd_stmt->status = OCI_STMT_PREPARED  | OCI_STMT_PARSED | 
                               OCI_STMT_DESCRIBED | OCI_STMT_EXECUTED;

            bnd_stmt->type   = OCI_CST_SELECT;
        }

        if ((bnd->direction & OCI_BDM_OUT) && (bnd->input) && (bnd->buffer.data))
        {
            /* only reset bind indicators if bind was not a PL/SQL bind
               that can have oupout values
            */

            if ((OCI_CST_BEGIN != stmt->type) && (OCI_CST_DECLARE != stmt->type))
            {
                memset(bnd->buffer.inds, 0, ((size_t) bnd->buffer.count) * sizeof(sb2));
            }
            else
            {
                /* extra work for internal allocated binds buffers with PL/SQL */

                if (!bnd->is_array)
                {
                    /* - For big integer (64 bits), we use an OCINumber.

                       - Oracle date/time type is the only non scalar type
                         implemented by oracle through a public structure instead
                         of using a handle. So we need to copy the value
                    */

                    if ((OCI_CDT_NUMERIC == bnd->type) && (SQLT_VNU == bnd->code))
                    {
                        res = OCI_NumberGet(stmt->con, (OCINumber *) bnd->buffer.data,
                                            (uword) sizeof(big_int), bnd->subtype, bnd->code,
                                            (void *) bnd->input);
                    }
                    else if (bnd->alloc)
                    {

                        if (OCI_CDT_DATETIME == bnd->type)
                        {
                            if (bnd->input)
                            {
                                memcpy(((OCI_Date *) bnd->input)->handle,
                                       (void *) bnd->buffer.data, sizeof(OCIDate));
                            }
                        }

                        /* update object indicator with bind object indicator
                         *pointer */

                        if (OCI_CDT_OBJECT == bnd->type)
                        {
                            if (bnd->input)
                            {
                                ((OCI_Object *) bnd->input)->tab_ind = (sb2*) bnd->buffer.obj_inds[0];
                            }
                        }
                    }
                }
                else
                {
                    for (j = 0; j < bnd->buffer.count; j++)
                    {

                        /* - For big integer (64 bits), we use an OCINumber.

                           - Oracle date/time type is the only non scalar type
                             implemented by oracle through a public structure instead
                             of using a handle. So we need to copy the value
                        */

                        if ((OCI_CDT_NUMERIC == bnd->type) && (SQLT_VNU == bnd->code))
                        {

                            res = OCI_NumberGet(stmt->con,
                                                (OCINumber *) ((ub1 *) bnd->buffer.data +
                                                (size_t) (j*bnd->size)),
                                                (uword) sizeof(big_int), bnd->subtype, bnd->code,
                                                (void *) (((ub1 *) bnd->input) +
                                                (((size_t)j)*sizeof(big_int))));
                        }
                        else if (bnd->alloc)
                        {
                            if (OCI_CDT_DATETIME == bnd->type)
                            {
                                if (bnd->input[j])
                                {
                                    memcpy(((OCI_Date *) bnd->input[j])->handle,
                                           ((ub1 *) bnd->buffer.data) + (size_t) (j*bnd->size),
                                           sizeof(OCIDate));
                                }
                            }

                            /* update bind object indicator pointer with object
                             *indicator */

                            if (OCI_CDT_OBJECT == bnd->type)
                            {
                                if (bnd->input)
                                {
                                    ((OCI_Object *) bnd->input[j])->tab_ind = (sb2 *) bnd->buffer.obj_inds[j];
                                }
                            }
                        }
                    }
                }
            }

            if (OCILib.use_wide_char_conv)
            {
                if (OCI_CDT_TEXT == bnd->type)
                {
                    for (j = 0; j < bnd->buffer.count; j++)
                    {
                        /* need conversion if bind buffer was allocated */

                        int dbsize   = -1;
                        int offset1 = (bnd->size/sizeof(dbtext))*sizeof(otext);
                        int offset2 = bnd->size;

                        if (bnd->buffer.lens)
                        {
                            dbsize = (int) ((ub2 *) bnd->buffer.lens)[j];
                        }

                        if (bnd->size == (sb4) dbsize)
                        {
                            dbsize -= sizeof(dbtext);
                        }

                        OCI_StringUTF16ToUTF32( (((ub1 *) bnd->buffer.data) + (j*offset2) ),
                                                (((ub1 *) bnd->input      ) + (j*offset1) ),
                                                dbsize);
                    }
                }
            }
        }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindData
 * --------------------------------------------------------------------------------------------- */

boolean OCI_BindData
(
    OCI_Statement *stmt,
    void          *data,
    ub4            size,
    const otext   *name,
    ub1            type,
    unsigned int   code,
    unsigned int   mode,
    unsigned int   subtype,
    OCI_TypeInfo  *typinf,
    unsigned int   nbelem
)
{
    boolean res      = TRUE;
    OCI_Bind *bnd    = NULL;
    ub4 exec_mode    = OCI_DEFAULT;
    boolean is_pltbl = FALSE;
    boolean is_array = FALSE;
    boolean reused   = FALSE;
    ub4 *pnbelem     = NULL;
    int index        = 0;
    int prev_index   = -1;
    size_t nballoc   = (size_t) nbelem;

    /* check index if necessary */

    if (OCI_BIND_BY_POS == stmt->bind_mode)
    {
        index = (int) ostrtol(&name[1], NULL, 10);

        if (index <= 0 || index > OCI_BIND_MAX)
        {
            OCI_ExceptionOutOfBounds(stmt->con, index);
            res = FALSE;
        }
    }
   
    /* check if the bind name has already been used */

    if (res)
    {
        if (OCI_BIND_INPUT == mode)
        {
            prev_index = OCI_BindGetIndex(stmt, name);

            if (prev_index > 0)
            {
                if (!stmt->bind_reuse)
                {
                    OCI_ExceptionBindAlreadyUsed(stmt, name);
                    res = FALSE;
                }
                else
                {
                    bnd = stmt->ubinds[prev_index-1];

                    if (bnd->type != type)
                    {
                        OCI_ExceptionRebindBadDatatype(stmt, name);
                        res = FALSE;
                    }
                    else
                    {
                        reused = TRUE;
                    }
                }

                index = prev_index;
            }
        }
    }

    /* check if we can handle another bind */

    if (res)
    {
        if (OCI_BIND_INPUT == mode)
        {
            if (stmt->nb_ubinds >= OCI_BIND_MAX)
            {
                OCI_ExceptionMaxBind(stmt);
                res = FALSE;
            }

            if (res)
            {
                /* allocate user bind array if necessary */

                if (!stmt->ubinds)
                {
                    stmt->ubinds = (OCI_Bind **) OCI_MemAlloc(OCI_IPC_BIND_ARRAY,
                                                              sizeof(*stmt->ubinds),
                                                              (size_t) OCI_BIND_MAX,
                                                              TRUE);
                }

                res = (stmt->ubinds != NULL);
            }
        }
        else
        {
            if (stmt->nb_rbinds >= OCI_BIND_MAX)
            {
                OCI_ExceptionMaxBind(stmt);
                res = FALSE;
            }

            if (res)
            {
                /* allocate register bind array if necessary */

                if (!stmt->rbinds)
                {
                    stmt->rbinds = (OCI_Bind **) OCI_MemAlloc(OCI_IPC_BIND_ARRAY,
                                                                sizeof(*stmt->rbinds),
                                                                (size_t) OCI_BIND_MAX,
                                                                TRUE);
                }

                res = (stmt->rbinds != NULL);
            }
        }
    }

    /* checks done */

    if (res)
    {
        /* check out the number of elements that the bind variable will hold */

        if (nbelem > 0)
        {
            /* is it a pl/sql table bind ? */

            if ((OCI_CST_BEGIN == stmt->type) || (OCI_CST_DECLARE == stmt->type))
            {
                is_pltbl = TRUE;
                is_array = TRUE;
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

        if (!stmt->map)
        {
            stmt->map = OCI_HashCreate(OCI_HASH_DEFAULT_SIZE, OCI_HASH_INTEGER);

            res = (stmt->map != NULL);
        }
    }

    /* allocate bind object */

    if (res)
    {
        if (!bnd)
        {
            bnd = (OCI_Bind *) OCI_MemAlloc(OCI_IPC_BIND, sizeof(*bnd), (size_t) 1, TRUE);
        }

        res = (bnd != NULL);
    }

    /* allocate indicators array */

    if (res)
    {
        if (!bnd->buffer.inds)
        {
            bnd->buffer.inds = (void *) OCI_MemAlloc(OCI_IPC_INDICATOR_ARRAY,
                                                     sizeof(sb2), nballoc, TRUE);
        }

        res = (bnd->buffer.inds != NULL);
    }

    /* allocate object indicators pointer array */

    if (res)
    {
        if (OCI_CDT_OBJECT == type) 
        {
            if (!bnd->buffer.obj_inds)
            {
                bnd->buffer.obj_inds = (void **) OCI_MemAlloc(OCI_IPC_INDICATOR_ARRAY,
                                                             sizeof(void *), nballoc, TRUE);

                res = (bnd->buffer.obj_inds != NULL);
            }

            if (!bnd->buffer.null_inds)
            {
                bnd->buffer.null_inds = (sb2 *) OCI_MemAlloc(OCI_IPC_INDICATOR_ARRAY,
                                                             sizeof(sb2 *), nballoc, TRUE);

                res = (bnd->buffer.null_inds != NULL);
            }
        }
    }

    /* check need for PL/SQL table extra info */

    if (res && is_pltbl)
    {
        bnd->nbelem = nbelem;
        pnbelem     = &bnd->nbelem;

        /* allocate array of returned codes */

        if (!bnd->plrcds)
        {
            bnd->plrcds = (ub2 *) OCI_MemAlloc(OCI_IPC_PLS_RCODE_ARRAY,
                                                sizeof(ub2), nballoc, TRUE);
        }

        res = (bnd->plrcds != NULL);
    }

    /* for handle based datatypes, we need to allocate an array of handles for
       bind calls because OCILIB uses external arrays of OCILIB Objects */

    if (res && (OCI_BIND_INPUT == mode))
    {
        if (OCI_BAM_EXTERNAL == stmt->bind_alloc_mode)
        {
            if ((OCI_CDT_RAW     != type)  &&
                (OCI_CDT_LONG    != type)  &&
                (OCI_CDT_CURSOR  != type)  &&
                (OCI_CDT_NUMERIC != type || SQLT_VNU == code) &&
                (OCI_CDT_TEXT    != type || OCILib.use_wide_char_conv))
            {
                bnd->alloc = TRUE;

                if (reused && bnd->buffer.data && (bnd->size != (sb4) size))
                {
                    OCI_FREE(bnd->buffer.data);
                }

                if (!bnd->buffer.data)
                {
                    bnd->buffer.data = (void **) OCI_MemAlloc(OCI_IPC_BUFF_ARRAY, (size_t) size,
                                                                   (size_t) nballoc, TRUE);
                }

                res = (bnd->buffer.data != NULL);
            }
            else
            {
                bnd->buffer.data = (void **) data;
            }
        }
    }

    /* setup data length array */

    if (res && ((OCI_CDT_RAW == type) || (OCI_CDT_TEXT == type)))
    {
        if (!bnd->buffer.lens)
        {
            bnd->buffer.lens = (void *) OCI_MemAlloc(OCI_IPC_LEN_ARRAY, sizeof(ub2), nballoc, TRUE);
        }

        res = (bnd->buffer.lens != NULL);

        /* initialize length array with buffer default size */

        if (res)
        {
            unsigned int i;

            for (i=0; i < nbelem; i++)
            {
                *(ub2*)(((ub1 *)bnd->buffer.lens) + sizeof(ub2) * (size_t) i) = (ub2) size;
            }
        }
    }

    /* initialize bind object */

    if (res)
    {
        /* initialize bind attributes */

        bnd->stmt      = stmt;
        bnd->input     = (void **) data;
        bnd->type      = type;
        bnd->size      = size;
        bnd->code      = (ub2) code;
        bnd->subtype   = (ub1) subtype;
        bnd->is_array  = is_array;
        bnd->csfrm     = OCI_CSF_NONE;
        bnd->direction = OCI_BDM_IN_OUT;

        if (!bnd->name)
        {
            bnd->name = ostrdup(name);
        }

        /* initialize buffer */

        bnd->buffer.count   = nbelem;
        bnd->buffer.sizelen = sizeof(ub2);

        /* internal allocation if needed */

        if (!data && (OCI_BAM_INTERNAL == stmt->bind_alloc_mode))
        {
            res = OCI_BindAllocData(bnd);
        }

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
    }

    /* OCI binding */

    if (res)
    {
        if (OCI_BIND_BY_POS == stmt->bind_mode)
        {
            OCI_CALL1
            (
                res, stmt->con, stmt,

                OCIBindByPos(stmt->stmt, (OCIBind **) &bnd->buffer.handle,
                             stmt->con->err, (ub4) index, (void *) bnd->buffer.data,
                             bnd->size, bnd->code, bnd->buffer.inds, (ub2 *) bnd->buffer.lens,
                             bnd->plrcds, (ub4) (is_pltbl == TRUE ? nbelem : 0),
                             pnbelem, exec_mode)
            )
        }
        else
        {
            dbtext * dbstr  = NULL;
            int      dbsize = -1;

            dbstr = OCI_StringGetOracleString(bnd->name, &dbsize);

            OCI_CALL1
            (
                res, stmt->con, stmt,

                OCIBindByName(stmt->stmt, (OCIBind **) &bnd->buffer.handle,
                              stmt->con->err, (OraText *) dbstr, (sb4) dbsize,
                              (void *) bnd->buffer.data, bnd->size, bnd->code,
                              bnd->buffer.inds, (ub2 *) bnd->buffer.lens, bnd->plrcds,
                              (ub4) (is_pltbl == TRUE ? nbelem : 0),
                              pnbelem, exec_mode)
            )

            OCI_StringReleaseOracleString(dbstr);
        }

        if (SQLT_NTY == code  || SQLT_REF == code)
        {
            OCI_CALL1
            (
                res, stmt->con, stmt,

                OCIBindObject((OCIBind *) bnd->buffer.handle, stmt->con->err,
                              (OCIType *) typinf->tdo, (void **) bnd->buffer.data,
                              (ub4 *) NULL, (void **) bnd->buffer.obj_inds,
                              (ub4 *) bnd->buffer.inds)
            )
        }

        if (OCI_BIND_OUTPUT == mode)
        {
            /* register output placeholder */

            OCI_CALL1
            (
                res, stmt->con, stmt,

                OCIBindDynamic((OCIBind *) bnd->buffer.handle, stmt->con->err,
                               (dvoid *) bnd, OCI_ProcInBind,
                               (dvoid *) bnd, OCI_ProcOutBind)
            )
        }
    }

    /* set charset form */

    if (res)
    {
        if ((OCI_CDT_LOB == bnd->type) && (OCI_NCLOB == bnd->subtype))
        {
            ub1 csfrm = SQLCS_NCHAR;

            OCI_CALL1
            (
                res, bnd->stmt->con, bnd->stmt,

                OCIAttrSet((dvoid *) bnd->buffer.handle,
                           (ub4    ) OCI_HTYPE_BIND,
                           (dvoid *) &csfrm,
                           (ub4    ) sizeof(csfrm),
                           (ub4    ) OCI_ATTR_CHARSET_FORM,
                           bnd->stmt->con->err)
            )
        }
    }

    /* on success, we :
         - add the bind handle to the bind array
         - add the bind index to the map
    */

    if (res)
    {
        if (OCI_BIND_INPUT == mode)
        {
            if (!reused)
            {
                stmt->ubinds[stmt->nb_ubinds++] = bnd;

                /* for user binds, add a positive index */

                OCI_HashAddInt(stmt->map, name, stmt->nb_ubinds);
            }
        }
        else
        {
            /* for register binds, add a negative index */

            stmt->rbinds[stmt->nb_rbinds++] = bnd;

            index = (int) stmt->nb_rbinds;

            OCI_HashAddInt(stmt->map, name, -index);
        }
    }

    if (!res)
    {
        if (bnd && (prev_index  == -1))
        {
            OCI_BindFree(bnd);
        }
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindGetIndex
 * --------------------------------------------------------------------------------------------- */

int OCI_BindGetIndex
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    OCI_HashEntry *he = NULL;
    int index         = -1;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, -1);
    OCI_CHECK_PTR(OCI_IPC_STRING, name, -1);

    if (stmt->map)
    {
        he = OCI_HashLookup(stmt->map, name, FALSE);

        while (he)
        {
            /* no more entries or key matched => so we got it ! */

            if (!he->next || ostrcasecmp(he->key, name) == 0)
            {
                /* in order to sue the same map for user binds and
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
    }

    return index;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FetchIntoUserVariables
 * --------------------------------------------------------------------------------------------- */

boolean OCI_FetchIntoUserVariables
(
    OCI_Statement *stmt,
    va_list        args
)
{
    OCI_Resultset *rs = NULL;
    boolean res       = FALSE;
   
    /* get resultset */

    rs = OCI_GetResultset(stmt);

    /* fetch data */

    if (rs)
    {
        res = OCI_FetchNext(rs);
    }

    if (res)
    {
        unsigned int i, n;
        
        /* loop on column list for updating user given placeholders */

        for (i = 1, n = OCI_GetColumnCount(rs); (i <= n) && (res == TRUE); i++)
        {
            OCI_Column *col = OCI_GetColumn(rs, i);

            int type = va_arg(args, int);

            switch (type)
            {
               case OCI_ARG_TEXT:
                {
                    const otext *src;
                    otext *dst;

                    src = OCI_GetString(rs, i);
                    dst = va_arg(args, otext *);

                    if (dst != NULL)
                    {
                        dst[0] = 0;
                    }

                    if ((dst != NULL) && (src != NULL))
                    {
                        ostrcat(dst, src);
                    }

                    break;
                }
                case OCI_ARG_SHORT:
                {
                    SET_ARG_NUM(short, OCI_GetShort);
                    break;
                }
                case OCI_ARG_USHORT:
                {
                    SET_ARG_NUM(unsigned short, OCI_GetUnsignedShort); 
                    break;
                }
                case OCI_ARG_INT:
                {
                    SET_ARG_NUM(int, OCI_GetInt); 
                    break;
                }
                case OCI_ARG_UINT:
                {
                    SET_ARG_NUM(unsigned int, OCI_GetUnsignedInt); 
                    break;
                }
                case OCI_ARG_BIGINT:
                {
                    SET_ARG_NUM(big_int, OCI_GetBigInt); 
                    break;
                }
                case OCI_ARG_BIGUINT:
                {
                    SET_ARG_NUM(big_uint, OCI_GetUnsignedBigInt); 
                    break;
                }
                case OCI_ARG_DOUBLE:
                {
                    SET_ARG_NUM(double, OCI_GetDouble); 
                    break;
                }
                case OCI_ARG_FLOAT:
                {
                    SET_ARG_NUM(float, OCI_GetFloat); 
                    break;
                }
                case OCI_ARG_DATETIME:
                {
                    SET_ARG_HANDLE(OCI_Date, OCI_GetDate, OCI_DateAssign);
                    break;
                }
                case OCI_ARG_RAW:
                {
                    OCI_GetRaw(rs, i, va_arg(args, otext *), col->bufsize);
                    break;
                }
                case OCI_ARG_LOB:
                {
                    SET_ARG_HANDLE(OCI_Lob, OCI_GetLob, OCI_LobAssign);
                    break;
                }
                case OCI_ARG_FILE:
                {
                    SET_ARG_HANDLE(OCI_File, OCI_GetFile, OCI_FileAssign);
                    break;
                }
                case OCI_ARG_TIMESTAMP:
                {
                    SET_ARG_HANDLE(OCI_Timestamp, OCI_GetTimestamp, OCI_TimestampAssign);
                    break;
                }
                case OCI_ARG_INTERVAL:
                {
                    SET_ARG_HANDLE(OCI_Interval, OCI_GetInterval, OCI_IntervalAssign);
                    break;
                }
                case OCI_ARG_OBJECT:
                {
                    SET_ARG_HANDLE(OCI_Object, OCI_GetObject, OCI_ObjectAssign);
                    break;
                }
                case OCI_ARG_COLLECTION:
                {
                    SET_ARG_HANDLE(OCI_Coll, OCI_GetColl, OCI_CollAssign);
                    break;
                }
                case OCI_ARG_REF:
                {
                    SET_ARG_HANDLE(OCI_Ref, OCI_GetRef, OCI_RefAssign);
                    break;
                }
                default:
                {
                    OCI_ExceptionMappingArgument(stmt->con, stmt, type);

                    res = FALSE;

                    break;
                }
            }
        }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StatementInit
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * OCI_StatementInit
(
    OCI_Connection *con,
    OCI_Statement **pstmt,
    OCIStmt        *handle,
    OCI_Define     *def
)
{
    OCI_Statement * stmt = NULL;
    boolean res          = FALSE;

    OCI_CHECK(pstmt == NULL, NULL);

    if (!*pstmt)
    {
        *pstmt = (OCI_Statement *) OCI_MemAlloc(OCI_IPC_STATEMENT, sizeof(*stmt), (size_t) 1, TRUE);
    }

    if (*pstmt)
    {
        stmt = *pstmt;

        stmt->con  = con;
        stmt->stmt = handle;

        stmt->exec_mode       = OCI_DEFAULT;
        stmt->long_size       = OCI_SIZE_LONG;
        stmt->bind_reuse      = FALSE;
        stmt->bind_mode       = OCI_BIND_BY_NAME;
        stmt->long_mode       = OCI_LONG_EXPLICIT;
        stmt->bind_alloc_mode = OCI_BAM_EXTERNAL;

        res = TRUE;

        /* reset statement */

        OCI_StatementReset(stmt);

        if (def)
        {
            stmt->hstate = OCI_OBJECT_FETCHED_CLEAN;
            stmt->status = OCI_STMT_PREPARED  | OCI_STMT_PARSED | 
                           OCI_STMT_DESCRIBED | OCI_STMT_EXECUTED;
            stmt->type   = OCI_CST_SELECT;

            res = (res && OCI_SetPrefetchSize(stmt, OCI_PREFETCH_SIZE));
            res = (res && OCI_SetFetchSize(stmt, OCI_FETCH_SIZE));

            /* not really perfect, but better than nothing */

            if (def->col.name)
            {
                stmt->sql = ostrdup(def->col.name);
            }
        }
        else
        {
            /* allocate handle for non fetched cursor */

            stmt->hstate = OCI_OBJECT_ALLOCATED;
        }
    }

    /* check for failurec */

    if (!res)
    {
        OCI_StatementFree(stmt);
        stmt = NULL;
    }

    return stmt;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StatementReset
 * --------------------------------------------------------------------------------------------- */

boolean OCI_StatementReset
(
    OCI_Statement *stmt
)
{
    boolean res = TRUE;

#if OCI_VERSION_COMPILE >= OCI_9_2

    ub4 mode = OCI_DEFAULT;

    if ((OCILib.version_runtime >= OCI_9_2) && (stmt->nb_rbinds > 0))
    {
        /*  if we had registered binds, we must delete the stmt from the cache.
            Because, if we execute another sql with "returnning into clause",  
            OCI_ProcInBind won't be called by OCI. Nice Oracle bug ! */
        mode = OCI_STRLS_CACHE_DELETE;
    }

#endif

    /* reset batch errors */

    res = OCI_BatchErrorClear(stmt);

    /* free resultsets */

    res = OCI_ReleaseResultsets(stmt);

    /* free in/out binds */

    res = OCI_BindFreeAll(stmt);

    /* free bind map */

    if (stmt->map)
    {
        OCI_HashFree(stmt->map);
    }

    /* free handle if needed */

    if (stmt->stmt)
    {
        if (OCI_OBJECT_ALLOCATED == stmt->hstate)
        {

        #if OCI_VERSION_COMPILE >= OCI_9_2


            if (OCILib.version_runtime >= OCI_9_2)
            {
                OCIStmtRelease(stmt->stmt, stmt->con->err, NULL, 0, mode);
            }
            else

        #endif

            {
                OCI_HandleFree((dvoid *) stmt->stmt, (ub4) OCI_HTYPE_STMT);
            }
                
            stmt->stmt = NULL;
        }
        else if (OCI_OBJECT_ALLOCATED_BIND_STMT == stmt->hstate)
        {  
            OCI_HandleFree((dvoid *) stmt->stmt, (ub4) OCI_HTYPE_STMT);

            stmt->stmt = NULL;
        }
    }

    /* free sql statement */

    OCI_FREE(stmt->sql);

    stmt->rsts  = NULL;
    stmt->sql   = NULL;
    stmt->map   = NULL;
    stmt->batch = NULL;

    stmt->status     = OCI_STMT_CLOSED;
    stmt->type       = OCI_UNKNOWN;
    stmt->bind_array = FALSE;

    stmt->nb_iters      = 1;
    stmt->nb_iters_init = 1;
    stmt->dynidx        = 0;
    stmt->err_pos       = 0;

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StatementClose
 * --------------------------------------------------------------------------------------------- */

boolean OCI_StatementClose
(
    OCI_Statement *stmt
)
{
    boolean res    = TRUE;
    OCI_Error *err = NULL;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    /* clear statement reference from current error object */

    err = OCI_ErrorGet(FALSE, FALSE);

    if (err && err->stmt == stmt)
    {
        err->stmt = NULL;
    }

    /* reset data */

    res = OCI_StatementReset(stmt);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BatchErrorClear
 * --------------------------------------------------------------------------------------------- */

boolean OCI_BatchErrorClear
(
    OCI_Statement *stmt
)
{
    boolean res = TRUE;

    if (stmt->batch)
    {
        /* free internal array of OCI_Errors */

        OCI_FREE(stmt->batch->errs);

        /* free batch structure */

        OCI_FREE(stmt->batch);
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BatchErrorsInit
 * --------------------------------------------------------------------------------------------- */

boolean OCI_BatchErrorInit
(
    OCI_Statement *stmt
)
{
    boolean res   = TRUE;
    ub4 err_count = 0;

    OCI_BatchErrorClear(stmt);

    /* all OCI call here are not checked for errors as we already dealing
       with an array DML error */

    OCIAttrGet((dvoid *) stmt->stmt, (ub4) OCI_HTYPE_STMT,
               (dvoid *) &err_count, (ub4 *) NULL,
               (ub4) OCI_ATTR_NUM_DML_ERRORS, stmt->con->err);

    if (err_count > 0)
    {
        OCIError *hndl = NULL;

        /* allocate batch error structure */

        stmt->batch = (OCI_BatchErrors *) OCI_MemAlloc(OCI_IPC_BATCH_ERRORS,
                                                       sizeof(*stmt->batch),
                                                       (size_t) 1, TRUE);

        res = (stmt->batch != NULL);

        /* allocate array of error objects */

        if (res)
        {
            stmt->batch->errs = (OCI_Error *) OCI_MemAlloc(OCI_IPC_ERROR,
                                                           sizeof(*stmt->batch->errs),
                                                           (size_t) err_count, TRUE);

            res = (stmt->batch->errs != NULL);
        }

        if (res)
        {
            /* allocate OCI error handle */

            res = OCI_SUCCESSFUL(OCI_HandleAlloc((dvoid  *) stmt->con->env,
                                                 (dvoid **) (void *) &hndl,
                                                 (ub4) OCI_HTYPE_ERROR,
                                                 (size_t) 0, (dvoid **) NULL));
        }

        /* loop on the OCI errors to fill OCILIB error objects */

        if (res)
        {
            ub4 i;

            stmt->batch->count = err_count;

            for (i = 0; i < stmt->batch->count; i++)
            {
                int dbsize  = -1;
                dbtext *dbstr = NULL;

                OCI_Error *err = &stmt->batch->errs[i];

                OCIParamGet((dvoid *) stmt->con->err, OCI_HTYPE_ERROR,
                            stmt->con->err, (dvoid **) (void *) &hndl, i);

                /* get row offset */

                OCIAttrGet((dvoid *) hndl, (ub4) OCI_HTYPE_ERROR,
                           (void *) &err->row, (ub4 *) NULL,
                           (ub4) OCI_ATTR_DML_ROW_OFFSET, stmt->con->err);

                /* fill error attributes */

                err->type = OCI_ERR_ORACLE;
                err->con  = stmt->con;
                err->stmt = stmt;

                /* OCILIB indexes start at 1 */

                err->row++;

                /* get error string */

                dbsize = (int) osizeof(err->str) - 1;

                dbstr = OCI_StringGetOracleString(err->str, &dbsize);

                OCIErrorGet((dvoid *) hndl,
                            (ub4) 1,
                            (OraText *) NULL, &err->sqlcode,
                            (OraText *) dbstr,
                            (ub4) dbsize,
                            (ub4) OCI_HTYPE_ERROR);

                OCI_StringCopyOracleStringToNativeString(dbstr, err->str, dbcharcount(dbsize));
                OCI_StringReleaseOracleString(dbstr);
            }
        }

        /* release error handle */

        if (hndl)
        {
            OCI_HandleFree(hndl, OCI_HTYPE_ERROR);
        }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExecuteInternal
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ExecuteInternal
(
    OCI_Statement *stmt,
    ub4            mode
)
{
    boolean res  = TRUE;
    sword status = OCI_SUCCESS;
    ub4 iters    = 0;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);
    OCI_CHECK_STMT_STATUS(stmt, OCI_STMT_PREPARED, FALSE);

    /* set up iters and mode values for execution */

    if (OCI_CST_SELECT == stmt->type)
    {
        mode |= stmt->exec_mode;
    }
    else
    {
        iters = stmt->nb_iters;

        /* for array DML, use batch error mode */

        if (iters > 1)
        {
            mode = mode | OCI_BATCH_ERRORS;
        }
    }

    /* reset batch errors */

    OCI_BatchErrorClear(stmt);

    /* check bind objects for updating their null indicator status */

    res = res && OCI_BindCheck(stmt);

    /* check current resultsets */

    if (res && stmt->rsts)
    {
        /* resultsets are freed before any prepare operations.
           So, if we got ones here, it means the same SQL order
           is re-executed */

        if (OCI_CST_SELECT == stmt->type)
        {
            /* just reinitialize the current resultet */

            res = OCI_ResultsetInit(stmt->rsts[0]);
        }
        else
        {
            /* Must free previous resulsets for 'returning into'
               SQL orders that can produce multiple resulsets */

            res = OCI_ReleaseResultsets(stmt);
        }
    }

    /* Oracle execute call */

    status = OCIStmtExecute(stmt->con->cxt, stmt->stmt, stmt->con->err, iters,
                            (ub4) 0, (OCISnapshot *) NULL, (OCISnapshot *) NULL, mode);

    /* reset input binds indicators status even if execution failed */

    OCI_BindReset(stmt);

    /* check result */

    res = ((OCI_SUCCESS == status) || (OCI_SUCCESS_WITH_INFO == status) || (OCI_NEED_DATA == status));

    if (OCI_SUCCESS_WITH_INFO == status)
    {
        OCI_ExceptionOCI(stmt->con->err, stmt->con, stmt, TRUE);
    }

    /* on batch mode, check if any error occured */

    if (mode & OCI_BATCH_ERRORS)
    {
        /* build batch error list if the statement is array DML */

        OCI_BatchErrorInit(stmt);

        if (stmt->batch)
        {
            res = (stmt->batch->count == 0);
        }
    }

    /* update status on success */

    if (res)
    {
        if (mode & OCI_PARSE_ONLY)
        {
            stmt->status |= OCI_STMT_PARSED;
        }
        else if (mode & OCI_DESCRIBE_ONLY)
        {
            stmt->status |= OCI_STMT_PARSED;
            stmt->status |= OCI_STMT_DESCRIBED;
        }
        else
        {
            stmt->status |= OCI_STMT_PARSED;
            stmt->status |= OCI_STMT_DESCRIBED;
            stmt->status |= OCI_STMT_EXECUTED;

            /* commit if necessary */

            if (stmt->con->autocom)
            {
                OCI_Commit(stmt->con);
            }
        }
    }
    else
    {
        /* get parse error position type */

        /* (one of the rare OCI call not enclosed with a OCI_CALLX macro ...) */

        OCIAttrGet((dvoid *) stmt->stmt, (ub4) OCI_HTYPE_STMT,
                   (dvoid *) &stmt->err_pos, (ub4 *) NULL,
                   (ub4) OCI_ATTR_PARSE_ERROR_OFFSET, stmt->con->err);

        /* raise exception */

        OCI_ExceptionOCI(stmt->con->err, stmt->con, stmt, FALSE);
    }

    OCI_RESULT(res);

    return res;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_StatementCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * OCI_API OCI_StatementCreate
(
    OCI_Connection *con
)
{
    OCI_Statement *stmt = NULL;
    OCI_Item *item      = NULL;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    /* create statement object */

    item = OCI_ListAppend(con->stmts, sizeof(*stmt));

    if (item)
    {
        stmt = OCI_StatementInit(con, (OCI_Statement **) &item->data, NULL, FALSE);
    }

    OCI_RESULT(stmt != NULL);

    return stmt;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StatementFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_StatementFree
(
    OCI_Statement *stmt
)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    OCI_CHECK_OBJECT_FETCHED(stmt, FALSE);

    res = OCI_StatementClose(stmt);

    OCI_ListRemove(stmt->con->stmts, stmt);

    OCI_FREE(stmt);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ReleaseResultsets
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ReleaseResultsets
(
    OCI_Statement *stmt
)
{
    boolean res = TRUE;
    ub4 nb_err = 0;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    if (stmt->rsts)
    {
        ub4 i;
        
        for (i = 0; i  < stmt->nb_rs; i++)
        {
            if (stmt->rsts[i])
            {
                if (!OCI_ResultsetFree(stmt->rsts[i]))
                {
                    nb_err++;
                }
            }
        }

        OCI_FREE(stmt->rsts);
    }

    res = (nb_err == 0);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Prepare
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Prepare
(
    OCI_Statement *stmt,
    const otext   *sql
)
{
    boolean res    = TRUE;
    dbtext *dbstr  = NULL;
    int     dbsize = -1;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, sql, FALSE);

    /* reset statement */

    res = OCI_StatementReset(stmt);

    if (res)
    {
        /* store SQL */

        stmt->sql = ostrdup(sql);

        dbstr = OCI_StringGetOracleString(stmt->sql, &dbsize);

        if (OCILib.version_runtime < OCI_9_2)
        {
            /* allocate handle */

            res = OCI_SUCCESSFUL(OCI_HandleAlloc((dvoid *) stmt->con->env,
                                                 (dvoid **) (void *) &stmt->stmt,
                                                 (ub4) OCI_HTYPE_STMT,
                                                 (size_t) 0, (dvoid **) NULL));
        }
    }

    if (res )
    {
        /* prepare SQL */

    #if OCI_VERSION_COMPILE >= OCI_9_2

        if (OCILib.version_runtime >= OCI_9_2)
        {
            OCI_CALL1
            (
                res, stmt->con, stmt,

                OCIStmtPrepare2(stmt->con->cxt, &stmt->stmt, stmt->con->err, (OraText *) dbstr,
                               (ub4) dbsize, NULL, 0, (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT)
            )
        }
        else

    #endif

        {
            OCI_CALL1
            (
                res, stmt->con, stmt,

                OCIStmtPrepare(stmt->stmt,stmt->con->err, (OraText *) dbstr,
                               (ub4) dbsize, (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT)
            )
        }


        /* get statement type */

        OCI_CALL1
        (
            res, stmt->con, stmt,

            OCIAttrGet((dvoid *) stmt->stmt, (ub4) OCI_HTYPE_STMT,
                       (dvoid *) &stmt->type, (ub4 *) NULL,
                       (ub4) OCI_ATTR_STMT_TYPE, stmt->con->err)
        )
    }
        
    OCI_StringReleaseOracleString(dbstr);

    /* update statement status */

    if (res)
    {
        ub4 size = 0;

        stmt->status = OCI_STMT_PREPARED;

        size = stmt->prefetch_size ? stmt->prefetch_size : OCI_PREFETCH_SIZE;
        res  = (res && OCI_SetPrefetchSize(stmt, size));
                
        size = stmt->fetch_size ? stmt->fetch_size : OCI_FETCH_SIZE;
        res  = (res && OCI_SetFetchSize(stmt, size));
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Execute
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Execute
(
    OCI_Statement *stmt
)
{
    return OCI_ExecuteInternal(stmt, OCI_DEFAULT);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExecuteStmt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ExecuteStmt
(
    OCI_Statement *stmt,
    const otext   *sql
)
{
    return (OCI_Prepare(stmt, sql) && OCI_ExecuteInternal(stmt, OCI_DEFAULT));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Parse
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Parse
(
    OCI_Statement *stmt,
    const otext   *sql
)
{
    return (OCI_Prepare(stmt, sql) && OCI_ExecuteInternal(stmt, OCI_PARSE_ONLY));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Describe
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Describe
(
    OCI_Statement *stmt,
    const otext   *sql
)
{
    boolean res = TRUE;

    res = OCI_Prepare(stmt, sql);
    
    if (res && (OCI_CST_SELECT == stmt->type))
    {
        res = OCI_ExecuteInternal(stmt, OCI_DESCRIBE_ONLY);
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_PrepareFmt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_PrepareFmt
(
    OCI_Statement *stmt,
    const otext   *sql,
    ...
)
{
    boolean res = FALSE;
    va_list args;
    int     size;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, sql, FALSE);

    /* first, get buffer size */

    va_start(args, sql);

    size = OCI_ParseSqlFmt(stmt, NULL, sql, &args);

    va_end(args);

    if (size > 0)
    {
        /* allocate buffer */

        otext *sql_fmt = (otext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(otext), (size_t) (size+1), TRUE);

        if (sql_fmt)
        {
            /* format buffer */

            va_start(args, sql);

            if (OCI_ParseSqlFmt(stmt, sql_fmt, sql, &args) > 0)
            {
                /* parse buffer */

                res = OCI_Prepare(stmt, sql_fmt);
            }

            va_end(args);

            OCI_FREE(sql_fmt);
        }
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExecuteStmtFmt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ExecuteStmtFmt
(
    OCI_Statement *stmt,
    const otext   *sql,
    ...
)
{
    boolean res = FALSE;
    va_list args;
    int     size;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, sql, FALSE);

    /* first, get buffer size */

    va_start(args, sql);

    size = OCI_ParseSqlFmt(stmt, NULL, sql, &args);

    va_end(args);

    if (size > 0)
    {
        /* allocate buffer */

        otext *sql_fmt = (otext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(otext), (size_t) (size+1), TRUE);

        if (sql_fmt)
        {
            /* format buffer */

            va_start(args, sql);

            if (OCI_ParseSqlFmt(stmt, sql_fmt, sql, &args) > 0)
            {
                /* prepare and execute SQL buffer */

                res = (OCI_Prepare(stmt, sql_fmt) && OCI_ExecuteInternal(stmt, OCI_DEFAULT));
            }

            va_end(args);

            OCI_FREE(sql_fmt);
        }
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ParseFmt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ParseFmt
(
    OCI_Statement *stmt,
    const otext   *sql,
    ...
)
{
    boolean res     = FALSE;
    int     size    = 0;
    va_list args;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, sql, FALSE);

    /* first, get buffer size */

    va_start(args, sql);

    size = OCI_ParseSqlFmt(stmt, NULL, sql, &args);

    va_end(args);

    if (size > 0)
    {       
        /* allocate buffer */

        otext  *sql_fmt = (otext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(otext), (size_t) (size+1), TRUE);

        if (sql_fmt)
        {
            /* format buffer */

            va_start(args, sql);

            if (OCI_ParseSqlFmt(stmt, sql_fmt, sql, &args) > 0)
            {
                /* prepare and execute SQL buffer */

                res = (OCI_Prepare(stmt, sql_fmt) && OCI_ExecuteInternal(stmt, OCI_PARSE_ONLY));
            }

            va_end(args);

            OCI_FREE(sql_fmt);
        }
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DescribeFmt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_DescribeFmt
(
    OCI_Statement *stmt,
    const otext   *sql,
    ...
)
{
    boolean res     = FALSE;
    int     size    = 0;
    va_list args;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, sql, FALSE);

    /* first, get buffer size */

    va_start(args, sql);

    size = OCI_ParseSqlFmt(stmt, NULL, sql, &args);

    va_end(args);

    if (size > 0)
    {
        /* allocate buffer */

        otext  *sql_fmt = (otext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(otext), (size_t) (size+1), TRUE);

        if (sql_fmt )
        {
            /* format buffer */

            va_start(args, sql);

            if (OCI_ParseSqlFmt(stmt, sql_fmt, sql, &args) > 0)
            {
                /* prepare and execute SQL buffer */

                res = (OCI_Prepare(stmt, sql_fmt) && OCI_ExecuteInternal(stmt, OCI_DESCRIBE_ONLY));
            }

            va_end(args);

            OCI_FREE(sql_fmt);
        }
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Immediate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_Immediate
(
    OCI_Connection *con,
    const otext    *sql,
    ...
)
{
    OCI_Statement *stmt = NULL;
    boolean res         = FALSE;
    va_list args;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, sql, FALSE);

    /* First, execute SQL */

    stmt = OCI_StatementCreate(con);

    if (stmt)
    {
        res = OCI_ExecuteStmt(stmt, sql);

        if (res)
        {
            /* get resultset and set up variables */

            if (OCI_GetStatementType(stmt) == OCI_CST_SELECT)
            {
                va_start(args, sql);

                res = OCI_FetchIntoUserVariables(stmt, args);

                va_end(args);
            }
        }

        OCI_StatementFree(stmt);
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ImmediateFmt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ImmediateFmt
(
    OCI_Connection *con,
    const otext    *sql,
    ...
)
{
    boolean         res     = FALSE;
    OCI_Statement  *stmt    = NULL;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, sql, FALSE);

    stmt = OCI_StatementCreate(con);

    if (stmt)
    {
        int     size = 0;
        va_list args;
  
        /* first, get buffer size */

        va_start(args, sql);

        size = OCI_ParseSqlFmt(stmt, NULL, sql, &args);

        va_end(args);

        if (size > 0)
        {
            /* allocate buffer */

            otext  *sql_fmt = (otext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(otext), (size_t) (size+1), TRUE);

            if (sql_fmt)
            {
                /* format buffer */

                va_start(args, sql);

                if (OCI_ParseSqlFmt(stmt, sql_fmt, sql, &args) > 0)
                {
                    /* prepare and execute SQL buffer */

                    res = (OCI_Prepare(stmt, sql_fmt) && OCI_ExecuteInternal(stmt, OCI_DEFAULT));

                    /* get resultset and set up variables */

                    if (res && (OCI_CST_SELECT == OCI_GetStatementType(stmt)))
                    {
                        res = OCI_FetchIntoUserVariables(stmt, args);
                    }
                }

                va_end(args);

                OCI_FREE(sql_fmt);
            }
        }

        OCI_StatementFree(stmt);
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArraySetSize
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindArraySetSize
(
    OCI_Statement *stmt,
    unsigned int   size
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    OCI_CHECK_MIN(stmt->con, stmt, size, 1, FALSE);

    OCI_CHECK_STMT_STATUS(stmt, OCI_STMT_PREPARED, FALSE);

    /* if the statements already has binds, we need to check if the new size is
       not greater than the initial size
    */

    if ((stmt->nb_ubinds > 0) && (stmt->nb_iters_init < size))
    {
        OCI_ExceptionBindArraySize(stmt, stmt->nb_iters_init, stmt->nb_iters, size);

        res = FALSE;
    }
    else
    {
        stmt->nb_iters   = size;
        stmt->bind_array = TRUE;

        if (stmt->nb_ubinds == 0)
        {
            stmt->nb_iters_init = stmt->nb_iters;
        }
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArrayGetSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_BindArrayGetSize
(
    OCI_Statement *stmt
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    OCI_RESULT(TRUE);

    return stmt->nb_iters;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_AllowRebinding
 * --------------------------------------------------------------------------------------------- */

OCI_EXPORT boolean OCI_API OCI_AllowRebinding
(
    OCI_Statement *stmt,
    boolean        value
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    OCI_RESULT(TRUE);

    stmt->bind_reuse = value;

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindShort
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindShort
(
    OCI_Statement *stmt,
    const otext   *name,
    short         *data
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_SHORT);

    return OCI_BindData(stmt, data, sizeof(short), name, OCI_CDT_NUMERIC,
                        SQLT_INT, OCI_BIND_INPUT, OCI_NUM_SHORT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArrayOfShorts
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindArrayOfShorts
(
    OCI_Statement *stmt,
    const otext   *name,
    short         *data,
    unsigned int   nbelem
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_SHORT);

    return OCI_BindData(stmt, data, sizeof(short), name, OCI_CDT_NUMERIC,
                        SQLT_INT, OCI_BIND_INPUT, OCI_NUM_SHORT, NULL, nbelem);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindUnsignedShort
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindUnsignedShort
(
    OCI_Statement  *stmt,
    const otext    *name,
    unsigned short *data
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_SHORT);

    return OCI_BindData(stmt, data, sizeof(unsigned short), name, OCI_CDT_NUMERIC,
                        SQLT_UIN, OCI_BIND_INPUT, OCI_NUM_USHORT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArrayOfUnsignedShorts
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindArrayOfUnsignedShorts
(
    OCI_Statement  *stmt,
    const otext    *name,
    unsigned short *data,
    unsigned int    nbelem
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_SHORT);

    return OCI_BindData(stmt, data, sizeof(unsigned short), name, OCI_CDT_NUMERIC,
                        SQLT_UIN, OCI_BIND_INPUT, OCI_NUM_USHORT, NULL, nbelem);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindInt
(
    OCI_Statement *stmt,
    const otext   *name,
    int           *data
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_INT);

    return OCI_BindData(stmt, data, sizeof(int), name, OCI_CDT_NUMERIC,
                        SQLT_INT, OCI_BIND_INPUT, OCI_NUM_INT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArrayOfInts
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindArrayOfInts
(
    OCI_Statement *stmt,
    const otext   *name,
    int           *data,
    unsigned int   nbelem
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_INT);

    return OCI_BindData(stmt, data, sizeof(int), name, OCI_CDT_NUMERIC,
                        SQLT_INT, OCI_BIND_INPUT, OCI_NUM_INT, NULL, nbelem);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindUnsignedInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindUnsignedInt
(
    OCI_Statement *stmt,
    const otext   *name,
    unsigned int  *data
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_INT);

    return OCI_BindData(stmt, data, sizeof(unsigned int), name, OCI_CDT_NUMERIC,
                        SQLT_UIN, OCI_BIND_INPUT, OCI_NUM_UINT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArrayOfUnsignedInts
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindArrayOfUnsignedInts
(
    OCI_Statement *stmt,
    const otext   *name,
    unsigned int  *data,
    unsigned int   nbelem
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_INT);

    return OCI_BindData(stmt, data, sizeof(unsigned int), name, OCI_CDT_NUMERIC,
                        SQLT_UIN, OCI_BIND_INPUT, OCI_NUM_UINT, NULL, nbelem);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindBigInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindBigInt
(
    OCI_Statement *stmt,
    const otext   *name,
    big_int       *data
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_BIGINT);

    return OCI_BindData(stmt, data, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_INPUT, OCI_NUM_BIGINT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArrayOfBigInts
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindArrayOfBigInts
(
    OCI_Statement *stmt,
    const otext   *name,
    big_int       *data,
    unsigned int   nbelem
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_BIGINT);

    return OCI_BindData(stmt, data, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_INPUT, OCI_NUM_BIGINT, NULL, nbelem);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindUnsignedBigInt
(
    OCI_Statement *stmt,
    const otext   *name,
    big_uint      *data
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_BIGINT);

    return OCI_BindData(stmt, data, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_INPUT, OCI_NUM_BIGUINT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArrayOfUnsignedInts
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindArrayOfUnsignedBigInts
(
    OCI_Statement *stmt,
    const otext   *name,
    big_uint      *data,
    unsigned int   nbelem
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_BIGINT);

    return OCI_BindData(stmt, data, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_INPUT, OCI_NUM_BIGUINT, NULL, nbelem);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindString
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindString
(
    OCI_Statement *stmt,
    const otext   *name,
    otext         *data,
    unsigned int   len
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_STRING);

    if ((len == 0) || len == (UINT_MAX))
    {
        if (data)
        {
            /* only compute length for external bind if no valid length has been provided */

            len = (unsigned int) ostrlen(data);
        }
        else
        {
            /* if data is NULL, it means that binding mode is OCI_BAM_INTERNAL.
               An invalid length passed to the function, we do not have a valid length to 
               allocate internal array, thus we need to raise an exception */
    
            OCI_ExceptionMinimumValue(stmt->con, stmt, 1);

            return FALSE;
        }
    }


    return OCI_BindData(stmt, data, (len + 1) * (ub4) sizeof(dbtext), name,
                        OCI_CDT_TEXT, SQLT_STR, OCI_BIND_INPUT, 0, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArrayOfStrings
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindArrayOfStrings
(
    OCI_Statement *stmt,
    const otext   *name,
    otext         *data,
    unsigned int   len,
    unsigned int   nbelem
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_STRING);

    OCI_CHECK_MIN(stmt->con, stmt, len, 1, FALSE);

    return OCI_BindData(stmt, data, (len + 1) * (ub4) sizeof(dbtext), name,
                        OCI_CDT_TEXT, SQLT_STR, OCI_BIND_INPUT, 0, NULL, nbelem);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindRaw
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindRaw
(
    OCI_Statement *stmt,
    const otext   *name,
    void          *data,
    unsigned int   len
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_VOID);

    OCI_CHECK_MIN(stmt->con, stmt, len, 1, FALSE);

    return OCI_BindData(stmt, data, len, name, OCI_CDT_RAW,
                        SQLT_BIN, OCI_BIND_INPUT, 0, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArrayOfRaws
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindArrayOfRaws
(
    OCI_Statement *stmt,
    const otext   *name,
    void          *data,
    unsigned int   len,
    unsigned int   nbelem
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_VOID);

    OCI_CHECK_MIN(stmt->con, stmt, len, 1, FALSE);

    return OCI_BindData(stmt, data, len, name, OCI_CDT_RAW,
                        SQLT_BIN, OCI_BIND_INPUT, 0, NULL, nbelem);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindDouble
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindDouble
(
    OCI_Statement *stmt,
    const otext   *name,
    double        *data
)
{
    int code = SQLT_FLT;

    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_DOUBLE);

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (OCILib.version_runtime >= OCI_10_1)
    {
        code = SQLT_BDOUBLE;
    }

#endif

    return OCI_BindData(stmt, data, sizeof(double), name, OCI_CDT_NUMERIC,
                        code, OCI_BIND_INPUT, OCI_NUM_DOUBLE, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArrayOfDoubles
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindArrayOfDoubles
(
    OCI_Statement *stmt,
    const otext   *name,
    double        *data,
    unsigned int   nbelem
)
{
    int code = SQLT_FLT;

    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_DOUBLE);

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (OCILib.version_runtime >= OCI_10_1)
    {
        code = SQLT_BDOUBLE;
    }

#endif

    return OCI_BindData(stmt, data, sizeof(double), name, OCI_CDT_NUMERIC,
                        code, OCI_BIND_INPUT, OCI_NUM_DOUBLE, NULL, nbelem);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindFloat
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindFloat
(
    OCI_Statement *stmt,
    const otext   *name,
    float         *data
)
{
    int code = SQLT_FLT;

    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_FLOAT);

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (OCILib.version_runtime >= OCI_10_1)
    {
        code = SQLT_BFLOAT;
    }

#endif

    return OCI_BindData(stmt, data, sizeof(float), name, OCI_CDT_NUMERIC,
                        code, OCI_BIND_INPUT, OCI_NUM_FLOAT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArrayOfFloats
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindArrayOfFloats
(
    OCI_Statement *stmt,
    const otext   *name,
    float         *data,
    unsigned int   nbelem
)
{
    int code = SQLT_FLT;

    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_FLOAT);

#if OCI_VERSION_COMPILE >= OCI_10_1

    if (OCILib.version_runtime >= OCI_10_1)
    {
        code = SQLT_BFLOAT;
    }

#endif

    return OCI_BindData(stmt, data, sizeof(float), name, OCI_CDT_NUMERIC,
                        code, OCI_BIND_INPUT, OCI_NUM_FLOAT, NULL, nbelem);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindDate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindDate
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Date      *data
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_DATE);

    return OCI_BindData(stmt, data, sizeof(OCIDate), name, OCI_CDT_DATETIME,
                        SQLT_ODT, OCI_BIND_INPUT, 0, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArrayOfDates
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindArrayOfDates
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Date     **data,
    unsigned int   nbelem
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_DATE);

    return OCI_BindData(stmt, data, sizeof(OCIDate), name, OCI_CDT_DATETIME,
                        SQLT_ODT, OCI_BIND_INPUT, 0, NULL, nbelem);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindTimestamp
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindTimestamp
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Timestamp *data
)
{
    int code    = SQLT_TIMESTAMP;
    boolean res = FALSE;

    OCI_CHECK_BIND_CALL1(stmt, name, data, OCI_IPC_TIMESTAMP);

    OCI_CHECK_TIMESTAMP_ENABLED(stmt->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* map oracle internal type */

    if (OCI_TIMESTAMP_TZ == data->type)
    {
        code = SQLT_TIMESTAMP_TZ;
    }
    else if (OCI_TIMESTAMP_LTZ == data->type)
    {
        code = SQLT_TIMESTAMP_LTZ;
    }

    res = OCI_BindData(stmt, data, sizeof(OCIDateTime *), name, OCI_CDT_TIMESTAMP,
                       code, OCI_BIND_INPUT, data->type, NULL, 0);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(code);
    OCI_NOT_USED(code);

#endif

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArrayOfTimestamps
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindArrayOfTimestamps
(
    OCI_Statement  *stmt,
    const otext    *name,
    OCI_Timestamp **data,
    unsigned int    type,
    unsigned int    nbelem
)
{
    int code     = SQLT_TIMESTAMP;
    boolean res  = FALSE;

    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_TIMESTAMP);

    OCI_CHECK_TIMESTAMP_ENABLED(stmt->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* map oracle internal type */

    if (OCI_TIMESTAMP_TZ == type)
    {
        code = SQLT_TIMESTAMP_TZ;
    }
    else if (OCI_TIMESTAMP_LTZ == type)
    {
        code = SQLT_TIMESTAMP_LTZ;
    }

    res =  OCI_BindData(stmt, data, sizeof(OCIDateTime *), name, OCI_CDT_TIMESTAMP,
                        code, OCI_BIND_INPUT, type, NULL, nbelem);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(type);
    OCI_NOT_USED(code);
    OCI_NOT_USED(nbelem);

#endif

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindInterval
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindInterval
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Interval  *data
)
{
    int code    = 0;
    boolean res = FALSE;

    OCI_CHECK_BIND_CALL1(stmt, name, data, OCI_IPC_INTERVAL);

    OCI_CHECK_INTERVAL_ENABLED(stmt->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* map oracle internal type */

    if (OCI_INTERVAL_YM == data->type)
    {
        code = SQLT_INTERVAL_YM;
    }
    else if (OCI_INTERVAL_DS == data->type)
    {
        code = SQLT_INTERVAL_DS;
    }

    res = OCI_BindData(stmt, data, sizeof(OCIInterval *), name, OCI_CDT_INTERVAL,
                       code, OCI_BIND_INPUT, data->type, NULL, 0);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(code);

#endif

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArrayOfIntervals
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindArrayOfIntervals
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Interval **data,
    unsigned int   type,
    unsigned int   nbelem
)
{
    unsigned int code = 0;
    boolean res       = FALSE;

    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_INTERVAL);

    OCI_CHECK_INTERVAL_ENABLED(stmt->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* map oracle internal type */

    if (OCI_INTERVAL_YM == type)
    {
        code = SQLT_INTERVAL_YM;
    }
    else if (OCI_INTERVAL_DS == type)
    {
        code = SQLT_INTERVAL_DS;
    }

    res = OCI_BindData(stmt, data, sizeof(OCIInterval *), name, OCI_CDT_INTERVAL,
                       code, OCI_BIND_INPUT, type, NULL, nbelem);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(type);
    OCI_NOT_USED(code);
    OCI_NOT_USED(nbelem);

#endif

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindObject
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindObject
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Object    *data
)
{
    OCI_CHECK_BIND_CALL1(stmt, name, data, OCI_IPC_OBJECT);

    return OCI_BindData(stmt, data, sizeof(void*), name, OCI_CDT_OBJECT,
                        SQLT_NTY, OCI_BIND_INPUT, 0, data->typinf, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArrayOfObjects
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindArrayOfObjects
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Object   **data,
    OCI_TypeInfo  *typinf,
    unsigned int   nbelem
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_OBJECT);

    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, FALSE);

    return OCI_BindData(stmt, data, sizeof(void *), name, OCI_CDT_OBJECT,
                        SQLT_NTY, OCI_BIND_INPUT, 0, typinf, nbelem);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindLob
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindLob
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Lob       *data
)
{
    int code = SQLT_BLOB;

    OCI_CHECK_BIND_CALL1(stmt, name, data, OCI_IPC_LOB);

    /* map oracle internal type */

    if ((OCI_CLOB == data->type) || (OCI_NCLOB == data->type))
    {
        code = SQLT_CLOB;
    }

    return OCI_BindData(stmt, data, sizeof(OCILobLocator*), name, OCI_CDT_LOB,
                        code, OCI_BIND_INPUT, data->type, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArrayOfLobs
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindArrayOfLobs
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Lob      **data,
    unsigned int   type,
    unsigned int   nbelem
)
{
    unsigned int code = SQLT_BLOB;

    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_LOB);

    /* map oracle internal type */

    if ((OCI_CLOB == type) || (OCI_NCLOB == type))
    {
        code = SQLT_CLOB;
    }

    return OCI_BindData(stmt, data, sizeof(OCILobLocator*), name, OCI_CDT_LOB,
                        code, OCI_BIND_INPUT, type, NULL, nbelem);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindFile
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindFile
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_File      *data
)
{
    int code = SQLT_BFILE;

    OCI_CHECK_BIND_CALL1(stmt, name, data, OCI_IPC_FILE);

    /* map oracle internal type */

    if (OCI_CFILE == data->type)
    {
        code = SQLT_CFILE;
    }

    return OCI_BindData(stmt, data, sizeof(OCILobLocator*), name, OCI_CDT_FILE,
                        code, OCI_BIND_INPUT, data->type, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArrayOfFiles
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindArrayOfFiles
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_File     **data,
    unsigned int   type,
    unsigned int   nbelem
)
{
    unsigned int code = SQLT_BFILE;

    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_FILE);

    /* map oracle internal type */

    if (OCI_CFILE == type)
    {
        code = SQLT_CFILE;
    }

    return OCI_BindData(stmt, data, sizeof(OCILobLocator*), name, OCI_CDT_FILE,
                        code, OCI_BIND_INPUT, type, NULL, nbelem);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindRef
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindRef
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Ref       *data
)
{
    OCI_CHECK_BIND_CALL1(stmt, name, data, OCI_IPC_REF);

    return OCI_BindData(stmt, data, sizeof(OCIRef *), name, OCI_CDT_REF,
                        SQLT_REF, OCI_BIND_INPUT, 0, data->typinf, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArrayOfRefs
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindArrayOfRefs
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Ref      **data,
    OCI_TypeInfo  *typinf,
    unsigned int   nbelem
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_REF);

    return OCI_BindData(stmt, data, sizeof(OCIRef *), name, OCI_CDT_REF,
                        SQLT_REF, OCI_BIND_INPUT, 0, typinf, nbelem);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindColl
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindColl
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Coll      *data
)
{
    OCI_CHECK_BIND_CALL1(stmt, name, data, OCI_IPC_COLLECTION);

    return OCI_BindData(stmt, data, sizeof(OCIColl*), name, OCI_CDT_COLLECTION, SQLT_NTY,
                        OCI_BIND_INPUT, 0, data->typinf, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindArrayOfColls
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindArrayOfColls
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Coll     **data,
    OCI_TypeInfo  *typinf,
    unsigned int   nbelem
)
{
    OCI_CHECK_BIND_CALL2(stmt, name, data, OCI_IPC_COLLECTION);

    return OCI_BindData(stmt, data, sizeof(OCIColl*), name, OCI_CDT_COLLECTION, SQLT_NTY, 
                        OCI_BIND_INPUT, 0, typinf, nbelem);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindStatement
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindStatement
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Statement *data
)
{
    boolean res = FALSE;

    OCI_CHECK_BIND_CALL1(stmt, name, data, OCI_IPC_STATEMENT);

    res = OCI_BindData(stmt, &data->stmt, sizeof(OCIStmt*), name, OCI_CDT_CURSOR,
                       SQLT_RSET, OCI_BIND_INPUT, 0, NULL, 0);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_BindLong
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_BindLong
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_Long      *data,
    unsigned int   size
)
{
    int code = SQLT_LBI;

    OCI_CHECK_BIND_CALL1(stmt, name, data, OCI_IPC_LONG);

    /* map oracle internal type */

    if (OCI_CLONG == data->type)
    {
        code = SQLT_LNG;

        size *= (unsigned int) sizeof(otext);
    }

    return OCI_BindData(stmt, data, size, name, OCI_CDT_LONG,
                        code, OCI_BIND_INPUT, data->type, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RegisterShort
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RegisterShort
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    return OCI_BindData(stmt, NULL, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_OUTPUT, OCI_NUM_SHORT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RegisterUnsignedShort
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RegisterUnsignedShort
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    return OCI_BindData(stmt, NULL, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_OUTPUT, OCI_NUM_USHORT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RegisterInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RegisterInt
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    return OCI_BindData(stmt, NULL, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_OUTPUT, OCI_NUM_INT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RegisterUnsignedInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RegisterUnsignedInt
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    return OCI_BindData(stmt, NULL, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_OUTPUT, OCI_NUM_UINT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RegisterBigInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RegisterBigInt
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    return OCI_BindData(stmt, NULL, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_OUTPUT, OCI_NUM_BIGINT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RegisterUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RegisterUnsignedBigInt
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    return OCI_BindData(stmt, NULL, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_OUTPUT, OCI_NUM_BIGUINT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RegisterString
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RegisterString
(
    OCI_Statement *stmt,
    const otext   *name,
    unsigned int   len
)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    OCI_CHECK_MIN(stmt->con, stmt, len, 1, FALSE);

    return OCI_BindData(stmt, NULL, (len + 1) * (ub4) sizeof(dbtext), name,
                        OCI_CDT_TEXT, SQLT_STR, OCI_BIND_OUTPUT, 0, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RegisterRaw
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RegisterRaw
(
    OCI_Statement *stmt,
    const otext   *name,
    unsigned int   len
)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    OCI_CHECK_MIN(stmt->con, stmt, len, 1, FALSE);

    return OCI_BindData(stmt, NULL, len, name, OCI_CDT_RAW,
                        SQLT_BIN, OCI_BIND_OUTPUT, 0, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RegisterDouble
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RegisterDouble
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    return OCI_BindData(stmt, NULL, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_OUTPUT, OCI_NUM_DOUBLE, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RegisterFloat
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RegisterFloat
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    return OCI_BindData(stmt, NULL, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_OUTPUT, OCI_NUM_FLOAT, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RegisterDate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RegisterDate
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    int code = SQLT_ODT;
    int size = sizeof(OCIDate);

    OCI_CHECK_REGISTER_CALL(stmt, name);

    /* versions of OCI (< 10.2) crashes if SQLT_ODT is passed for output
       data with returning clause.
       It's an Oracle known bug #3269146 */

    if (OCI_GetVersionConnection(stmt->con) < OCI_10_2)
    {
        code = SQLT_DAT;
        size = 7;
    }

    return OCI_BindData(stmt, NULL, size, name, OCI_CDT_DATETIME,
                        code, OCI_BIND_OUTPUT, 0, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RegisterTimestamp
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RegisterTimestamp
(
    OCI_Statement *stmt,
    const otext   *name,
    unsigned int   type
)
{
    int code    = SQLT_TIMESTAMP;
    boolean res = FALSE;

    OCI_CHECK_REGISTER_CALL(stmt, name);

    OCI_CHECK_TIMESTAMP_ENABLED(stmt->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* map oracle internal type */

    if (OCI_TIMESTAMP_TZ == type)
    {
        code = SQLT_TIMESTAMP_TZ;
    }
    else if (OCI_TIMESTAMP_LTZ == type)
    {
        code = SQLT_TIMESTAMP_LTZ;
    }
 
    res = OCI_BindData(stmt, NULL, sizeof(OCIDateTime *), name, OCI_CDT_TIMESTAMP,
                       code, OCI_BIND_OUTPUT, type, NULL, 0);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(type);
    OCI_NOT_USED(code);

#endif

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RegisterInterval
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RegisterInterval
(
    OCI_Statement *stmt,
    const otext   *name,
    unsigned int   type
)
{
    unsigned int code = 0;
    boolean res       = FALSE;

    OCI_CHECK_REGISTER_CALL(stmt, name);

    OCI_CHECK_INTERVAL_ENABLED(stmt->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* map oracle internal type */

    if (OCI_INTERVAL_YM == type)
    {
        code = SQLT_INTERVAL_YM;
    }
    else if (OCI_INTERVAL_DS == type)
    {
        code = SQLT_INTERVAL_DS;
    }
    
    res = OCI_BindData(stmt, NULL, sizeof(OCIInterval *), name, OCI_CDT_INTERVAL,
                       code, OCI_BIND_OUTPUT, type, NULL, 0);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(type);
    OCI_NOT_USED(code);

#endif

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RegisterObject
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RegisterObject
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_TypeInfo  *typinf
)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, FALSE);

    return OCI_BindData(stmt, NULL, sizeof(OCIInterval *), name, OCI_CDT_OBJECT,
                        SQLT_NTY, OCI_BIND_OUTPUT, 0, typinf, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RegisterLob
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RegisterLob
(
    OCI_Statement *stmt,
    const otext   *name,
    unsigned int   type
)
{
    unsigned int code = SQLT_BLOB;

    OCI_CHECK_REGISTER_CALL(stmt, name);

    /* map oracle internal type */

    if ((OCI_CLOB == type) || (OCI_NCLOB == type))
    {
        code = SQLT_CLOB;
    }
  
    return OCI_BindData(stmt, NULL, sizeof(OCILobLocator*), name, OCI_CDT_LOB,
                        code, OCI_BIND_OUTPUT, type, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RegisterFile
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RegisterFile
(
    OCI_Statement *stmt,
    const otext   *name,
    unsigned int   type
)
{
    unsigned int code = SQLT_BFILE;

    OCI_CHECK_REGISTER_CALL(stmt, name);

    /* map oracle internal type */

    if (OCI_CFILE == type)
    {
        code = SQLT_CFILE;
    }

    return OCI_BindData(stmt, NULL, sizeof(OCILobLocator*), name, OCI_CDT_FILE,
                        code, OCI_BIND_OUTPUT, type, NULL, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_RegisterRef
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_RegisterRef
(
    OCI_Statement *stmt,
    const otext   *name,
    OCI_TypeInfo  *typinf
)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, FALSE);

    return OCI_BindData(stmt, NULL, sizeof(OCIRef *), name, OCI_CDT_REF,
                        SQLT_REF, OCI_BIND_OUTPUT, 0, typinf, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetStatementType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetStatementType
(
    OCI_Statement *stmt
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return stmt->type;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetFetchMode
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetFetchMode
(
    OCI_Statement *stmt,
    unsigned int   mode
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    OCI_CHECK_SCROLLABLE_CURSOR_ENABLED(stmt->con, FALSE);

    stmt->exec_mode = mode;

    OCI_RESULT(TRUE);

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetFetchMode
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetFetchMode
(
    OCI_Statement *stmt
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, OCI_UNKNOWN);

    OCI_CHECK_SCROLLABLE_CURSOR_ENABLED(stmt->con, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return stmt->exec_mode;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetBindMode
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetBindMode
(
    OCI_Statement *stmt,
    unsigned int   mode
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    stmt->bind_mode = mode;

    OCI_RESULT(TRUE);

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetBindMode
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetBindMode
(
    OCI_Statement *stmt
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return stmt->bind_mode;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetBindAllocation
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetBindAllocation
(
    OCI_Statement *stmt,
    unsigned int   mode
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    stmt->bind_alloc_mode = mode;

    OCI_RESULT(TRUE);

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetBindAllocation
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetBindAllocation
(
    OCI_Statement *stmt
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return stmt->bind_alloc_mode;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetFetchSize
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetFetchSize
(
    OCI_Statement *stmt,
    unsigned int   size
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    OCI_CHECK_MIN(stmt->con, stmt, size, 1, FALSE);

    stmt->fetch_size = size;

    OCI_RESULT(TRUE);

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetFetchSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetFetchSize
(
    OCI_Statement *stmt
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, 0);

    OCI_RESULT(TRUE);

    return stmt->fetch_size;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetPrefetchSize
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetPrefetchSize
(
    OCI_Statement *stmt,
    unsigned int   size
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt,  FALSE);
  
    stmt->prefetch_size = size;
    
    if (stmt->stmt)
    {
        OCI_CALL1
        (
            res, stmt->con, stmt,

            OCIAttrSet((dvoid *) stmt->stmt, (ub4) OCI_HTYPE_STMT,
                       (dvoid *) &stmt->prefetch_size,
                       (ub4) sizeof(stmt->prefetch_size),
                       (ub4) OCI_ATTR_PREFETCH_ROWS, stmt->con->err)
        )
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetPrefetchSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetPrefetchSize
(
    OCI_Statement *stmt
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, 0);

    OCI_RESULT(TRUE);

    return stmt->prefetch_size;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetPrefetchMemory
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetPrefetchMemory
(
    OCI_Statement *stmt,
    unsigned int   size
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt,  FALSE);

    stmt->prefetch_mem = size;

    if (stmt->stmt)
    {
        OCI_CALL1
        (
            res, stmt->con, stmt,

            OCIAttrSet((dvoid *) stmt->stmt, (ub4) OCI_HTYPE_STMT,
                       (dvoid *) &stmt->prefetch_mem,
                       (ub4) sizeof(stmt->prefetch_mem),
                       (ub4) OCI_ATTR_PREFETCH_MEMORY, stmt->con->err)
        )
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetPrefetchMemory
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetPrefetchMemory
(
    OCI_Statement *stmt
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, 0);

    OCI_RESULT(TRUE);

    return stmt->prefetch_mem;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetLongMaxSize
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetLongMaxSize
(
    OCI_Statement *stmt,
    unsigned int   size
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt,  FALSE);

    OCI_CHECK_MIN(stmt->con, stmt, size, 1, FALSE);

    stmt->long_size = size;

    OCI_RESULT(TRUE);

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetLongMaxSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetLongMaxSize
(
    OCI_Statement *stmt
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, 0);

    OCI_RESULT(TRUE);

    return stmt->long_size;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetLongMode
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetLongMode
(
    OCI_Statement *stmt,
    unsigned int   mode
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    stmt->long_mode = (ub1) mode;

    OCI_RESULT(TRUE);

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetLongMode
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetLongMode
(
    OCI_Statement *stmt
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return stmt->long_mode;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_StatementGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OCI_API OCI_StatementGetConnection
(
    OCI_Statement *stmt
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, NULL);

    OCI_RESULT(TRUE);

    return stmt->con;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetSql
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetSql
(
    OCI_Statement *stmt
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, NULL);

    OCI_RESULT(TRUE);

    return stmt->sql;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetSqlErrorPos
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetSqlErrorPos
(
    OCI_Statement *stmt
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, 0);

    OCI_RESULT(TRUE);

    return stmt->err_pos;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetAffecteddRows
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetAffectedRows
(
    OCI_Statement *stmt
)
{
    boolean res = TRUE;
    ub4 count   = 0;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt,  0);
    OCI_CHECK_STMT_STATUS(stmt, OCI_STMT_EXECUTED, 0);

    OCI_CALL1
    (
        res, stmt->con, stmt,

        OCIAttrGet((dvoid *) stmt->stmt, (ub4) OCI_HTYPE_STMT,
                   (void *) &count, (ub4 *) NULL, (ub4) OCI_ATTR_ROW_COUNT,
                   stmt->con->err)
    )

    OCI_RESULT(res);

    return count;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetBindCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetBindCount
(
    OCI_Statement *stmt
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, 0);

    OCI_RESULT(TRUE);

    return (unsigned int) stmt->nb_ubinds;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetBind
 * --------------------------------------------------------------------------------------------- */

OCI_Bind * OCI_API OCI_GetBind
(
    OCI_Statement *stmt,
    unsigned int   index
)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, NULL);
    OCI_CHECK_BOUND(stmt->con, index, 1, stmt->nb_ubinds, NULL);

    OCI_RESULT(TRUE);

    return stmt->ubinds[index-1];
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetBind2
 * --------------------------------------------------------------------------------------------- */

OCI_Bind * OCI_API OCI_GetBind2
(
    OCI_Statement *stmt,
    const otext   *name
)
{
    OCI_Bind *bnd = NULL;
    int index     = -1;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, NULL);
    OCI_CHECK_PTR(OCI_IPC_STRING, name, NULL);

    index =  OCI_BindGetIndex(stmt, name);

    if (index > 0)
    {
        bnd = stmt->ubinds[index-1];
    }

    OCI_RESULT(bnd != NULL);

    return bnd;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetSQLCommand
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetSQLCommand
(
    OCI_Statement *stmt
)
{
    boolean res = TRUE;
    ub2 code    = OCI_UNKNOWN;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, OCI_UNKNOWN);
    OCI_CHECK_STMT_STATUS(stmt, OCI_STMT_EXECUTED, OCI_UNKNOWN);

    OCI_CALL1
    (
        res, stmt->con, stmt,

        OCIAttrGet((dvoid *) stmt->stmt, (ub4) OCI_HTYPE_STMT,
                   (dvoid *) &code, (ub4 *) NULL,
                   (ub4) OCI_ATTR_SQLFNCODE, stmt->con->err)
    )

    OCI_RESULT(res);

    return (unsigned int) code;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetSQLVerb
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetSQLVerb
(
    OCI_Statement *stmt
)
{
    otext * desc      = NULL;
    unsigned int code = OCI_UNKNOWN;

    code = OCI_GetSQLCommand(stmt);

    if (OCI_UNKNOWN != code)
    {
        int i;
        
        for (i = 0; i < OCI_SQLCMD_COUNT; i++)
        {
            if (code == SQLCmds[i].code)
            {
                desc = SQLCmds[i].verb;
                break;
            }
        }
    }

    return desc;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetBatchError
 * --------------------------------------------------------------------------------------------- */

OCI_Error * OCI_API OCI_GetBatchError
(
    OCI_Statement *stmt
)
{
    OCI_Error *err = NULL;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, NULL);

    if (stmt->batch)
    {
        if (stmt->batch->cur < stmt->batch->count)
        {
            err = &stmt->batch->errs[stmt->batch->cur++];
        }
    }

    OCI_RESULT(TRUE);

    return err;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetBatchErrorCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetBatchErrorCount
(
    OCI_Statement *stmt
)
{
    unsigned int count = 0;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, 0);

    if (stmt->batch)
    {
        count = stmt->batch->count;
    }

    OCI_RESULT(TRUE);

    return count;
}

