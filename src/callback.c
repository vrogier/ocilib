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

#include "callback.h"

#include "event.h"
#include "list.h"
#include "macros.h"
#include "resultset.h"
#include "strings.h"
#include "timestamp.h"

typedef struct HAEventParams
{
    OCIServer   *srvhp;
    OCIDateTime *dthp;
    ub4          source;
    ub4          event;
} HAEventParams;

/* --------------------------------------------------------------------------------------------- *
 * ProcInBind
 * --------------------------------------------------------------------------------------------- */

sb4 CallbackInBind
(
    dvoid   *ictxp,
    OCIBind *bindp,
    ub4      iter,
    ub4      index,
    dvoid  **bufpp,
    ub4     *alenp,
    ub1     *piecep,
    dvoid  **indp
)
{
    ENTER_FUNC
    (
        /* returns */ sb4, OCI_ERROR,
        /* context */ OCI_IPC_BIND, ictxp
    )

    OCI_Bind * bnd = (OCI_Bind *) ictxp;
    sb2 *ind = (sb2 *) bnd ? bnd->buffer.inds : NULL;
    ub4  i   = 0;

    /* those checks may be not necessary but they keep away compilers warning
       away if the warning level is set to maximum !
    */

    OCI_NOT_USED(index)
    OCI_NOT_USED(bindp)

    /* check objects and bounds */

    CHECK_PTR(OCI_IPC_BIND, bnd)

    /* indicators must be set to -1 depending on data type,
       so let's do it for all */

    for (i = 0; i < bnd->buffer.count; i++, ind++)
    {
        *ind = -1;
    }

    /* setup bind index because OCI_RegisterXXX() might not have been called
       in the same order than the variables in the returning clause */

    if (0 == iter)
    {
        bnd->dynpos = bnd->stmt->dynidx++;
    }

    /* we do not need to do anything here except setting indicators */

    *bufpp  = (dvoid *) 0;
    *alenp  = (ub4    ) 0;
    *indp   = (dvoid *) bnd->buffer.inds;
    *piecep = (ub1    ) OCI_ONE_PIECE;

    SET_RETVAL(OCI_CONTINUE)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ProcOutBind
 * --------------------------------------------------------------------------------------------- */

sb4 CallbackOutBind
(
    dvoid   *octxp,
    OCIBind *bindp,
    ub4      iter,
    ub4      index,
    dvoid  **bufpp,
    ub4    **alenp,
    ub1     *piecep,
    dvoid  **indp,
    ub2    **rcodep
)
{
    ENTER_FUNC
    (
        /* returns */ sb4, OCI_ERROR,
        /* context */ OCI_IPC_BIND, octxp
    )

    OCI_Bind      *bnd = (OCI_Bind *)octxp;
    OCI_Define    *def  = NULL;
    OCI_Resultset *rs   = NULL;
    ub4            rows = 0;

    /* those checks may be not necessary but they keep away compilers warning
       away if the warning level is set to maximum !
    */

    OCI_NOT_USED(bindp)

    /* check objects  */

    CHECK_PTR(OCI_IPC_BIND, bnd)

    /* update statement status */

    bnd->stmt->status |= OCI_STMT_EXECUTED;

    /* create resultset on the first row processed for each iteration */

    if (0 == index)
    {
        bnd->stmt->nb_rs  = bnd->stmt->nb_iters;
        bnd->stmt->cur_rs = 0;

        /* allocate resultset handles array */

        ALLOC_DATA(OCI_IPC_RESULTSET_ARRAY, bnd->stmt->rsts, bnd->stmt->nb_rs)

        /* create resultset as needed */

        if (NULL == bnd->stmt->rsts[iter])
        {
            CHECK_ATTRIB_GET
            (
                OCI_HTYPE_BIND, OCI_ATTR_ROWS_RETURNED,
                bnd->buffer.handle, &rows, NULL,
                bnd->stmt->con->err
            )

            bnd->stmt->rsts[iter] = ResultsetCreate(bnd->stmt, rows);

            CHECK_NULL(bnd->stmt->rsts[iter])

            bnd->stmt->rsts[iter]->row_count = rows;
        }
    }

    CHECK_NULL(bnd->stmt->rsts)

    rs = bnd->stmt->rsts[iter];

    CHECK_NULL(rs)

    /* Let's Oracle update its buffers */

    def = &rs->defs[bnd->dynpos];

    switch (def->col.datatype)
    {
        case OCI_CDT_CURSOR:
        case OCI_CDT_TIMESTAMP:
        case OCI_CDT_INTERVAL:
        case OCI_CDT_LOB:
        case OCI_CDT_FILE:
        {
            *bufpp = def->buf.data[index];
            break;
        }
        default:
        {
            *bufpp = (((ub1*)def->buf.data) + (size_t) (def->col.bufsize * index));
            break;
        }
    }

    *alenp  = (ub4   *) (((ub1 *) def->buf.lens) + (size_t) ((ub4) def->buf.sizelen * index));
    *indp   = (dvoid *) (((ub1 *) def->buf.inds) + (size_t) ((ub4) sizeof(sb2)      * index));
    *piecep = (ub1    ) OCI_ONE_PIECE;
    *rcodep = (ub2   *) NULL;

    SET_RETVAL(OCI_CONTINUE)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ProcNotifyMessages
 * --------------------------------------------------------------------------------------------- */

ub4 CallbackNotifyMessages
(
    void            *oci_ctx,
    OCISubscription *subscrhp,
    void            *payload,
    ub4              paylen,
    void            *desc,
    ub4              mode
)
{
    ENTER_FUNC
    (
        /* returns */ ub4, OCI_SUCCESS,
        /* context */ OCI_IPC_DEQUEUE, oci_ctx
    )

    OCI_Dequeue *dequeue = (OCI_Dequeue *)oci_ctx;

    OCI_NOT_USED(paylen)
    OCI_NOT_USED(payload)
    OCI_NOT_USED(mode)
    OCI_NOT_USED(subscrhp)
    OCI_NOT_USED(desc)

    CHECK_PTR(OCI_IPC_DEQUEUE, dequeue)

    dequeue->callback(dequeue);

    SET_RETVAL(OCI_SUCCESS)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ProcNotifyChanges
 * --------------------------------------------------------------------------------------------- */

ub4 CallbackNotifyChanges
(
    void            *oci_ctx,
    OCISubscription *subscrhp,
    void            *payload,
    ub4              paylen,
    void            *desc,
    ub4              mode
)
{
    ENTER_FUNC
    (
        /* returns */ ub4, OCI_SUCCESS,
        /* context */ OCI_IPC_NOTIFY, oci_ctx
    )

    OCI_Subscription* sub = (OCI_Subscription*)oci_ctx;

    ub4 type = 0;

    OCI_NOT_USED(paylen)
    OCI_NOT_USED(payload)
    OCI_NOT_USED(mode)
    OCI_NOT_USED(subscrhp)

    CHECK_PTR(OCI_IPC_VOID, sub)

    CHECK(EventReset(&sub->event))

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* get database that generated the notification */

    CHECK(StringGetAttribute(sub->con, desc, OCI_DTYPE_CHDES, OCI_ATTR_CHDES_DBNAME,
                             &sub->event.dbname, &sub->event.dbname_size))

    /* get notification type */

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_CHDES, OCI_ATTR_CHDES_NFYTYPE,
        desc, &type, NULL,
        sub->err
    )

    switch(type)
    {
        case OCI_EVENT_STARTUP:
        case OCI_EVENT_SHUTDOWN:
        case OCI_EVENT_SHUTDOWN_ANY:
        {
            if (sub->type & OCI_CNT_DATABASES)
            {
                sub->event.type = type;
            }
            break;
        }
        case OCI_EVENT_DEREG:
        {
            sub->event.type = type;
            break;
        }
        case OCI_EVENT_OBJCHANGE:
        {
            if (sub->type & OCI_CNT_OBJECTS)
            {
                sub->event.type = type;
            }
            break;
        }
        default:
        {
            break;
        }
    }

    /* for object, much work to do for retrieving data */

    if (OCI_EVENT_OBJCHANGE == sub->event.type)
    {
        OCIColl *tables = NULL;

        /* get collection of modified tables */

        CHECK_ATTRIB_GET
        (
            OCI_DTYPE_CHDES, OCI_ATTR_CHDES_TABLE_CHANGES,
            desc, &tables, NULL,
            sub->err
        )

        if (tables)
        {
            dvoid **tbl_elem  = NULL;
            dvoid  *tbl_ind   = NULL;
            boolean tbl_exist = FALSE;
            sb4     nb_tables = 0;
            sb4     nb_rows   = 0;

            /* get number of tables in the collection */

            CHECK_OCI(sub->err, OCICollSize, sub->env, sub->err, tables, &nb_tables)

            for (sb4 i = 0; i < nb_tables; i++)
            {
                nb_rows = 0;

                /* partial reset of the event object  */

                if (sub->event.objname)
                {
                    sub->event.objname[0] = 0;
                }

                if (sub->event.rowid)
                {
                    sub->event.rowid[0] = 0;
                }

                /* get table element */

                CHECK_OCI
                (
                    sub->err,
                    OCICollGetElem,
                    sub->env, sub->err,  tables, i, &tbl_exist,
                    (dvoid**) (dvoid*) &tbl_elem, (dvoid**) &tbl_ind
                )

                /* get table name */

                CHECK(StringGetAttribute(sub->con, *tbl_elem, OCI_DTYPE_TABLE_CHDES,
                                         OCI_ATTR_CHDES_TABLE_NAME, &sub->event.objname,
                                         &sub->event.objname_size))

                /* get table modification type */

                CHECK_ATTRIB_GET
                (
                    OCI_DTYPE_TABLE_CHDES, OCI_ATTR_CHDES_TABLE_OPFLAGS,
                    *tbl_elem, &sub->event.op, NULL,
                    sub->err
                )

                sub->event.op = sub->event.op & (~OCI_OPCODE_ALLROWS);
                sub->event.op = sub->event.op & (~OCI_OPCODE_ALLOPS);

                /* if requested, get row details */

                if (sub->type & OCI_CNT_ROWS)
                {
                    OCIColl *rows = NULL;

                    /* get collection of modified rows */

                    CHECK_ATTRIB_GET
                    (
                        OCI_DTYPE_TABLE_CHDES, OCI_ATTR_CHDES_TABLE_ROW_CHANGES,
                        *tbl_elem, &rows, NULL,
                        sub->err
                    )

                    if (rows)
                    {
                        dvoid **row_elem  = NULL;
                        dvoid  *row_ind   = NULL;
                        boolean row_exist = FALSE;

                        /* get number of rows */

                        CHECK_OCI
                        (
                            sub->err,
                            OCICollSize,
                            sub->env, sub->err,
                            rows, &nb_rows
                        )

                        for (sb4 j = 0; j < nb_rows; j++)
                        {
                            /* partial reset of the event  */

                            if (sub->event.rowid)
                            {
                                sub->event.rowid[0] = 0;
                            }

                            /* get row element */

                            CHECK_OCI
                            (
                                sub->err,
                                OCICollGetElem,
                                sub->env, sub->err, rows, j, &row_exist,
                                (dvoid**) (dvoid*) &row_elem, (dvoid**) &row_ind
                            )

                            /* get rowid  */

                            CHECK(StringGetAttribute(sub->con, *row_elem, OCI_DTYPE_ROW_CHDES,
                                                     OCI_ATTR_CHDES_ROW_ROWID, &sub->event.rowid,
                                                     &sub->event.rowid_size))

                            /* get opcode  */

                            CHECK_ATTRIB_GET
                            (
                                OCI_DTYPE_ROW_CHDES, OCI_ATTR_CHDES_ROW_OPFLAGS,
                                *row_elem, &sub->event.op, NULL,
                                sub->err
                            )

                            sub->handler(&sub->event);
                        }
                    }
                }

                if (0 == nb_rows)
                {
                    sub->handler(&sub->event);
                }
            }
        }
    }
    else if (sub->event.type > 0)
    {
        sub->handler(&sub->event);
    }

    SET_RETVAL(OCI_SUCCESS)

#else

    OCI_NOT_USED(desc)
    OCI_NOT_USED(subscrhp)
    OCI_NOT_USED(type)

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ProcFailOver
 * --------------------------------------------------------------------------------------------- */

sb4 CallbackFailOver
(
    dvoid *svchp,
    dvoid *envhp,
    dvoid *fo_ctx,
    ub4    fo_type,
    ub4    fo_event
)
{
    ENTER_FUNC
    (
        /* returns */ sb4, OCI_FOC_OK,
        /* context */ OCI_IPC_CONNECTION, fo_ctx
    )

    OCI_Connection* con = (OCI_Connection*)fo_ctx;

    OCI_NOT_USED(envhp)
    OCI_NOT_USED(svchp)

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    CHECK_NULL(con->taf_handler)

    SET_RETVAL((sb4)con->taf_handler(con, fo_type, fo_event))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
* ProcHAEventInvoke
* --------------------------------------------------------------------------------------------- */

void ProcHAEventInvoke
(
    OCI_Connection *con,
    HAEventParams * ha_params
)
{
    ENTER_VOID
    (
        /* context */ OCI_IPC_CONNECTION, con
    )

    OCI_Timestamp* tmsp = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    if (NULL != con && con->svr == ha_params->srvhp)
    {
        tmsp = TimestampInitialize(NULL, tmsp, ha_params->dthp, OCI_TIMESTAMP);

        CHECK_NULL(tmsp)

        Env.ha_handler(con, (unsigned int)ha_params->source, (unsigned int)ha_params->event, tmsp);
    }

    if (NULL != tmsp)
    {
        tmsp->hstate = OCI_OBJECT_FETCHED_DIRTY;
        TimestampFree(tmsp);
    }

    EXIT_VOID()
}

/* --------------------------------------------------------------------------------------------- *
 * ProcHAEvent
 * --------------------------------------------------------------------------------------------- */

void CallbackHAEvent
(
    dvoid *evtctx,
    dvoid *eventptr
)
{
    ENTER_VOID
    (
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_PTR(OCI_IPC_VOID, evtctx)

#if OCI_VERSION_COMPILE >= OCI_10_2

    CHECK_PTR(OCI_IPC_VOID, Env.ha_handler)

    if (Env.version_runtime >= OCI_10_2)
    {
        HAEventParams params;

        memset(&params, 0, sizeof(params));

        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_SERVER, OCI_ATTR_HA_SRVFIRST,
            (OCIEvent *)eventptr, &params.srvhp, NULL,
            Env.err
        )

        while (params.srvhp)
        {
            params.dthp   = NULL;
            params.event  = OCI_HA_STATUS_DOWN;
            params.source = OCI_HA_SOURCE_INSTANCE;

            /* get event timestamp */

            CHECK_ATTRIB_GET
            (
                OCI_HTYPE_SERVER, OCI_ATTR_HA_TIMESTAMP,
                params.srvhp, &params.dthp, NULL,
                Env.err
            )

            /* get status */

            CHECK_ATTRIB_GET
            (
                OCI_HTYPE_SERVER, OCI_ATTR_HA_STATUS,
                params.srvhp, &params.event, NULL,
                Env.err
            )

            /* get source */

            CHECK_ATTRIB_GET
            (
                OCI_HTYPE_SERVER, OCI_ATTR_HA_SOURCE,
                params.srvhp, &params.source, NULL,
                Env.err
            )

            /* notify all related connections */

            ListForEachWithParam(Env.cons, &params, (POCI_LIST_FOR_EACH_WITH_PARAM) ProcHAEventInvoke);

            /* get next server */

            CHECK_ATTRIB_GET
            (
                OCI_HTYPE_SERVER, OCI_ATTR_HA_SRVNEXT,
                eventptr, &params.srvhp, NULL,
                Env.err
            )
        }
    }

#else

    OCI_NOT_USED(eventptr)

#endif

    EXIT_VOID()
}
