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

/*
 * IMPORTANT NOTICE
 *
 * THIS FILE CONTAINS CONSTANTS AND STRUCTURES DECLARATIONS THAT WERE
 * PICKED UP FROM ORACLE PUBLIC HEADER FILES.
 *
 * SO THE CONTENT OF THIS FILE MAY BE UNDER ORACLE COPYRIGHT AND THE
 * DECLARATIONS REPRODUCED HERE ARE ORIGINALLY WRITTEN BY ORACLE CORPORATION
 *
 * THE AUTHOR OF OCILIB LIBRARY HAS NOT WRITTEN THE CONTENT OF THIS
 * FILE AND HAS PARTIALLY INTEGRATED SOME ORACLE OCI DEFINITIONS TO 
 * ALLOW COMPILATION OF THIS OPEN SOURCE LIBRARY WITHOUT HAVING
 * ORACLE PRODUCTS INSTALLED ON DEVELOPMENT ENVIRONMENTS
 *
 */

#ifndef OCILIB_OCI_API_H_INCLUDED
#define OCILIB_OCI_API_H_INCLUDED

#ifndef CONST
#define CONST const
#endif

#if !defined(OCI_IMPORT_LINKAGE)
#include "defs.h"
#endif

/* ********************************************************************************************* *
 *                         PUBLIC OCI API PROTOTYPES
 * ********************************************************************************************* */

/* API introduced in 8.0 */

typedef sword (*OCIINITIALIZE)
( 
    ub4 mode,
    void *ctxp,
    void *(*malocfp)
    (
        void *ctxp, 
        size_t size
    ),
    void *(*ralocfp)
    (
        void *ctxp, 
        void *memptr, 
        size_t newsize),
    void (*mfreefp)
    (
        void *ctxp,
        void *memptr
    )
);

typedef sword (*OCIENVINIT)
(
    OCIEnv **envp,
    ub4      mode,
    size_t   xtramem_sz,
    void   **usrmempp
);

typedef sword (*OCIHANDLEALLOC)
(
    const void  *parenth,
    void       **hndlpp,
    const ub4    type,
    const size_t xtramem_sz,
    void       **usrmempp
);

typedef sword (*OCIHANDLEFREE)
(
    void     *hndlp,
    const ub4 type
);

typedef sword (*OCIDESCRIPTORALLOC)
(
    const void  *parenth,
    void       **descpp,
    const ub4    type,
    const size_t xtramem_sz,
    void       **usrmempp
);

typedef sword (*OCIDESCRIPTORFREE)
(
    void     *descp,
    const ub4 type
);

typedef sword (*OCISERVERATTACH)
(
    OCIServer     *srvhp,
    OCIError      *errhp,
    const OraText *dblink,
    sb4            dblink_len,
    ub4            mode
);

typedef sword (*OCISERVERDETACH)
(
    OCIServer *srvhp,
    OCIError  *errhp,
    ub4        mode
);

typedef sword (*OCISESSIONBEGIN)
(
    OCISvcCtx  *svchp,
    OCIError   *errhp,
    OCISession *usrhp,
    ub4         credt,
    ub4         mode
);

typedef sword (*OCISESSIONEND)
(
    OCISvcCtx  *svchp,
    OCIError   *errhp,
    OCISession *usrhp,
    ub4         mode
);

typedef sword (*OCIPASSWORDCHANGE)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    CONST OraText *user_name,
    ub4            usernm_len,
    CONST OraText *opasswd,
    ub4            opasswd_len,
    CONST OraText *npasswd,
    ub4            npasswd_len,
    ub4            mode
);

typedef sword (*OCISTMTPREPARE)
(
    OCIStmt       *stmtp,
    OCIError      *errhp,
    const OraText *stmt,
    ub4            stmt_len,
    ub4            language,
    ub4            mode
);

typedef sword (*OCIBINDBYPOS)
(
    OCIStmt  *stmtp,
    OCIBind **bindp,
    OCIError *errhp,
    ub4       position,
    void     *valuep,
    sb4       value_sz,
    ub2       dty,
    void     *indp,
    ub2      *alenp,
    ub2      *rcodep,
    ub4       maxarr_len,
    ub4      *curelep,
    ub4       mode
);

typedef sword (*OCIBINDBYNAME)
(
    OCIStmt       *stmtp,
    OCIBind      **bindp,
    OCIError      *errhp,
    const OraText *placeholder,
    sb4            placeh_len,
    void          *valuep,
    sb4            value_sz,
    ub2            dty,
    void          *indp,
    ub2           *alenp,
    ub2           *rcodep,
    ub4            maxarr_len,
    ub4           *curelep,
    ub4            mode
);

typedef sword (*OCIBINDDYNAMIC)
(
    OCIBind     *bindp,
    OCIError    *errhp,
    dvoid       *ictxp,
    sb4          (*icbfp)
    (
        dvoid   *ictxp,
        OCIBind *bindp,
        ub4      iter,
        ub4      index,
        dvoid  **bufpp,
        ub4     *alenp,
        ub1     *piecep,
        dvoid  **indp
    ),
    dvoid       *octxp,
    sb4          (*ocbfp)
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
);

typedef sword (*OCIBINDOBJECT)
(
    OCIBind       *bindp,
    OCIError      *errhp,
    CONST OCIType *type,
    dvoid        **pgvpp,
    ub4           *pvszsp,
    dvoid        **indpp,
    ub4           *indszp
);

typedef sword (*OCISTMTGETPIECEINFO)
(
    OCIStmt  *stmtp,
    OCIError *errhp,
    void    **hndlpp,
    ub4      *typep,
    ub1      *in_outp,
    ub4      *iterp,
    ub4      *idxp,
    ub1      *piecep
);

typedef sword (*OCISTMTSETPIECEINFO)
(
    void       *hndlp,
    ub4         type,
    OCIError   *errhp,
    const void *bufp,
    ub4        *alenp,
    ub1         piece,
    const void *indp,
    ub2        *rcodep
);

typedef sword (*OCISTMTEXECUTE)
(
    OCISvcCtx         *svchp,
    OCIStmt           *stmtp,
    OCIError          *errhp,
    ub4                iters,
    ub4                rowoff,
    const OCISnapshot *snap_in,
    OCISnapshot       *snap_out,
    ub4                mode
);

typedef sword (*OCIDEFINEBYPOS)
(
    OCIStmt    *stmtp,
    OCIDefine **defnp,
    OCIError   *errhp,
    ub4         position,
    void       *valuep,
    sb4         value_sz,
    ub2         dty,
    void       *indp,
    ub2        *rlenp,
    ub2        *rcodep,
    ub4         mode
);

typedef sword (*OCIDEFINEDYNAMIC)
(
    OCIDefine   *defnp,
    OCIError    *errhp,
    void        *octxp, 
    sb4      (*ocbfp)
    (
        void          *octxp,
        OCIDefine     *defnp,
        ub4            iter, 
        void         **bufpp,
        ub4          **alenpp,
        ub1           *piecep,
        void         **indpp,
        ub2          **rcodep
   )
);

typedef sword (*OCIDEFINEOBJECT)
(
    OCIDefine     *defnp,
    OCIError      *errhp,
    CONST OCIType *type,
    dvoid        **pgvpp,
    ub4           *pvszsp,
    dvoid        **indpp,
    ub4           *indszp
);

typedef sword (*OCISTMTFETCH )
(
    OCIStmt  *stmtp,
    OCIError *errhp,
    ub4       nrows,
    ub2       orientation,
    ub4       mode
);

typedef sword (*OCIPARAMGET)
(
    const void *hndlp,
    ub4         htype,
    OCIError   *errhp,
    void      **parmdpp,
    ub4         pos
);

typedef sword (*OCIPARAMSET)
(
    void       *hdlp,
    ub4         htyp,
    OCIError   *errhp,
    const void *dscp,
    ub4         dtyp,
    ub4         pos
);

typedef sword (*OCITRANSSTART)
(
    OCISvcCtx *svchp,
    OCIError  *errhp,
    uword      timeout,
    ub4        flags
);

typedef sword (*OCITRANSDETACH)
(
    OCISvcCtx *svchp,
    OCIError  *errhp,
    ub4        flags
);

typedef sword (*OCITRANSPREPARE)
(
    OCISvcCtx *svchp,
    OCIError  *errhp,
    ub4        flags
);

typedef sword (*OCITRANSFORGET)
(
    OCISvcCtx *svchp,
    OCIError  *errhp,
    ub4        flags
);

typedef sword (*OCITRANSCOMMIT)
(
    OCISvcCtx *svchp,
    OCIError  *errhp,
    ub4        flags
);

typedef sword (*OCITRANSROLLBACK)
(
    OCISvcCtx *svchp,
    OCIError  *errhp,
    ub4        flags
);

typedef sword (*OCIERRORGET)
(
    void    *hndlp,
    ub4      recordno,
    OraText *sqlstate,
    sb4     *errcodep,
    OraText *bufp,
    ub4      bufsiz,
    ub4      type
);

typedef sword (*OCILOBCREATETEMPORARY)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *locp,
    ub2            csid,
    ub1            csfrm,
    ub1            lobtype,
    boolean        cache,
    OCIDuration    duration
);

typedef sword (*OCILOBFREETEMPORARY)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *locp
);

typedef sword (*OCILOBISTEMPORARY)
(
    OCIEnv        *envp,
    OCIError      *errhp,
    OCILobLocator *locp,
    boolean       *is_temporary
);

typedef sword (*OCILOBAPPEND)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *dst_locp,
    OCILobLocator *src_locp
);

typedef sword (*OCILOBCOPY)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *dst_locp,
    OCILobLocator *src_locp,
    ub4            amount,
    ub4            dst_offset,
    ub4            src_offset
);

typedef sword (*OCILOBREAD)
(
    OCISvcCtx      *svchp,
    OCIError       *errhp,
    OCILobLocator  *locp,
    ub4            *amtp,
    ub4             offset,
    void           *bufp,
    ub4             bufl,
    void           *ctxp,
    sb4             (*cbfp)
    (
        void       *ctxp,
        const void *bufp,
        ub4         len,
        ub1         piece
    ),
    ub2             csid,
    ub1             csfrm
);

typedef sword (*OCILOBTRIM)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *locp,
    ub4            newlen
);

typedef sword (*OCILOBERASE)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *locp,
    ub4           *amount,
    ub4            offset
);

typedef sword (*OCILOBWRITE)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *locp,
    ub4           *amtp,
    ub4            offset,
    void          *bufp,
    ub4            buflen,
    ub1            piece,
    void          *ctxp,
    sb4            (*cbfp)
    (
        void      *ctxp,
        void      *bufp,
        ub4       *len,
        ub1       *piece
    ),
    ub2            csid,
    ub1            csfrm
);

typedef sword (*OCILOBGETLENGTH)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *locp,
    ub4           *lenp
);

typedef sword (*OCILOBGETCHUNKSIZE)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *locp,
    ub4           *chunksizep
);

typedef sword (*OCILOBOPEN)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *locp,
    ub1            mode
);

typedef sword (*OCILOBCLOSE)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *locp
);

typedef sword (*OCILOBLOCATORASSIGN)
(
    OCISvcCtx           *svchp,
    OCIError            *errhp,
    CONST OCILobLocator *src_locp,
    OCILobLocator      **dst_locpp
);

typedef sword (*OCILOBASSIGN)
(
    OCIEnv              *envhp,
    OCIError            *errhp,
    CONST OCILobLocator *src_locp,
    OCILobLocator      **dst_locpp
);

typedef sword (*OCILOBENABLEBUFFERING)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *locp
);

typedef sword (*OCILOBDISABLEBUFFERING)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *locp
);

typedef sword (*OCILOBFILEOPEN)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *filep,
    ub1            mode
);

typedef sword (*OCILOBFILECLOSE)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *filep
);

typedef sword (*OCILOBFILECLOSEALL)
(
    OCISvcCtx *svchp,
    OCIError  *errhp
);

typedef sword (*OCILOBFILEISOPEN)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *filep,
    boolean       *flag
);

typedef sword (*OCILOBFILEEXISTS)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *filep,
    boolean       *flag
);

typedef sword (*OCILOBFIELGETNAME)
(
    OCIEnv              *envhp,
    OCIError            *errhp,
    CONST OCILobLocator *filep,
    OraText             *dir_alias,
    ub2                 *d_length,
    OraText             *filename,
    ub2                 *f_length
);

typedef sword (*OCILOBFILESETNAME)
(
    OCIEnv         *envhp,
    OCIError       *errhp,
    OCILobLocator **filepp,
    CONST OraText  *dir_alias,
    ub2             d_length,
    CONST OraText  *filename,
    ub2             f_length
);

typedef sword (*OCILOBLOADFROMFILE)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *dst_locp,
    OCILobLocator *src_filep,
    ub4            amount,
    ub4            dst_offset,
    ub4            src_offset
);

typedef sword (*OCILOBWRITEAPPEND)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *lobp,
    ub4           *amtp,
    dvoid         *bufp,
    ub4            bufl,
    ub1            piece,
    dvoid         *ctxp,
    sb4            (*cbfp)
    (
        void      *ctxp,
        void      *bufp,
        ub4       *len,
        ub1       *piece
    ),
    ub2            csid,
    ub1            csfrm
);

typedef sword (*OCILOBISEQUAL)
(
    OCIEnv              *envhp,
    CONST OCILobLocator *x,
    CONST OCILobLocator *y,
    boolean             *is_equal
);

typedef sword (*OCILOBFLUSHBUFFER)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *locp,
    ub4            flag
);

typedef sword (*OCISERVERVERSION)
(
    void     *hndlp,
    OCIError *errhp,
    OraText  *bufp,
    ub4       bufsz,
    ub1       hndltype
);

typedef sword (*OCIBREAK)
(
    dvoid    *hndlp,
    OCIError *errhp
);

typedef sword (*OCIATTRGET)
(
    const void *trgthndlp,
    ub4         trghndltyp,
    void       *attributep,
    ub4        *sizep,
    ub4         attrtype,
    OCIError   *errhp
);

typedef sword (*OCIATTRSET)
(
    void     *trgthndlp,
    ub4       trghndltyp,
    void     *attributep,
    ub4       size,
    ub4       attrtype,
    OCIError *errhp
);

typedef sword (*OCIDATEASSIGN)
(
    OCIError      *err,
    CONST OCIDate *from,
    OCIDate       *to
);

typedef sword (*OCIDATETOTEXT)
(
    OCIError      *err,
    CONST OCIDate *date,
    CONST text    *fmt,
    ub1            fmt_length,
    CONST text    *lang_name,
    ub4            lang_length,
    ub4           *buf_size,
    text          *buf
);

typedef sword (*OCIDATEFROMTEXT)
(
    OCIError   *err,
    CONST text *date_str,
    ub4         d_str_length,
    CONST text *fmt,
    ub1         fmt_length,
    CONST text *lang_name,
    ub4         lang_length,
    OCIDate    *date
);

typedef sword (*OCIDATECOMPARE)
(
    OCIError      *err,
    CONST OCIDate *date1,
    CONST OCIDate *date2,
    sword         *result
);

typedef sword (*OCIDATEADDMONTHS)
(
    OCIError      *err,
    CONST OCIDate *date,
    sb4            num_months,
    OCIDate       *result
);

typedef sword (*OCIDATEADDDAYS)
(
    OCIError      *err,
    CONST OCIDate *date,
    sb4            num_days,
    OCIDate       *result
);

typedef sword (*OCIDATELASTDAY)
(
    OCIError      *err,
    CONST OCIDate *date,
    OCIDate       *last_day
);

typedef sword (*OCIDATEDAYSBETWEEN)
(
    OCIError      *err,
    CONST OCIDate *date1,
    CONST OCIDate *date2,
    sb4           *num_days
);

typedef sword (*OCIDATEZONETOZONE)
(
    OCIError      *err,
    CONST OCIDate *date1,
    CONST text    *zon1,
    ub4            zon1_length,
    CONST text    *zon2,
    ub4            zon2_length,
    OCIDate       *date2
);

typedef sword (*OCIDATENEXTDAY)
(
    OCIError      *err,
    CONST OCIDate *date,
    CONST text    *day_p,
    ub4            day_length,
    OCIDate       *next_day
);

typedef sword (*OCIDATECHECK)
(
    OCIError      *err,
    CONST OCIDate *date,
    uword         *valid
);

typedef sword (*OCIDATESYSDATE)
(
    OCIError *err,
    OCIDate  *sys_date
);

typedef sword (*OCIDESCRIBEANY)
(
    OCISvcCtx   *svchp,
    OCIError    *errhp,
    dvoid       *objptr,
    ub4          objnm_len,
    ub1          objptr_typ,
    ub1          info_level,
    ub1          objtyp,
    OCIDescribe *dschp
);

typedef sword (*OCIINTERVALASSIGN)
(
    dvoid             *hndl,
    OCIError          *err,
    CONST OCIInterval *inpinter,
    OCIInterval       *outinter
);

typedef sword (*OCIINTERVALCHECK)
(
    dvoid             *hndl,
    OCIError          *err,
    CONST OCIInterval *interval,
    ub4               *valid
);

typedef sword (*OCIINTERVALCOMPARE)
(
    dvoid       *hndl,
    OCIError    *err,
    OCIInterval *inter1,
    OCIInterval *inter2,
    sword       *result
);

typedef sword (*OCIINTERVALTOTEXT)
(
    dvoid             *hndl,
    OCIError          *err,
    CONST OCIInterval *interval,
    ub1                lfprec,
    ub1                fsprec,
    OraText           *buffer,
    size_t             buflen,
    size_t            *resultlen
);

typedef sword (*OCIINTERVALFROMTEXT)
(
    dvoid         *hndl,
    OCIError      *err,
    CONST OraText *inpstring,
    size_t         str_len,
    OCIInterval   *result
);

typedef sword (*OCIINTERVALFROMTZ)
(
    dvoid         *hndl,
    OCIError      *err,
    CONST oratext *inpstring,
    size_t         str_len,
    OCIInterval   *result
) ;

typedef sword (*OCIINTERVALGETDAYSECOND)
(
    dvoid             *hndl,
    OCIError          *err,
    sb4               *dy,
    sb4               *hr,
    sb4               *mm,
    sb4               *ss,
    sb4               *fsec,
    CONST OCIInterval *interval
);

typedef sword (*OCIINTERVALGETYEARMONTH)
(
    dvoid             *hndl,
    OCIError          *err,
    sb4               *yr,
    sb4               *mnth,
    CONST OCIInterval *interval
);

typedef sword (*OCIINTERVALSETDAYSECOND)
(
    dvoid       *hndl,
    OCIError    *err,
    sb4          dy,
    sb4          hr,
    sb4          mm,
    sb4          ss,
    sb4          fsec,
    OCIInterval *result
);

typedef sword (*OCIINTERVALSETYEARMONTH)
(
    dvoid       *hndl,
    OCIError    *err,
    sb4          yr,
    sb4          mnth,
    OCIInterval *result
);

typedef sword (*OCIINTERVALADD)
(
    dvoid       *hndl,
    OCIError    *err,
    OCIInterval *addend1,
    OCIInterval *addend2,
    OCIInterval *result
);

typedef sword (*OCIINTERVALSUBTRACT)
(
    dvoid       *hndl,
    OCIError    *err,
    OCIInterval *minuend,
    OCIInterval *subtrahend,
    OCIInterval *result
);

typedef sword (*OCIDATETIMEASSIGN)
(
    dvoid             *hndl,
    OCIError          *err,
    CONST OCIDateTime *from,
    OCIDateTime       *to
);

typedef sword (*OCIDATETIMECHECK)
(
    dvoid             *hndl,
    OCIError          *err,
    CONST OCIDateTime *date,
    ub4               *valid
);

typedef sword (*OCIDATETIMECOMPARE)
(
    dvoid             *hndl,
    OCIError          *err,
    CONST OCIDateTime *date1,
    CONST OCIDateTime *date2,
    sword             *result
);

typedef sword (*OCIDATETIMECONSTRUCT)
(
    dvoid       *hndl,
    OCIError    *err,
    OCIDateTime *datetime,
    sb2          year,
    ub1          month,
    ub1          day,
    ub1          hour,
    ub1          min,
    ub1          sec,
    ub4          fsec,
    OraText     *timezone,
    size_t       timezone_length
);

typedef sword (*OCIDATETIMECONVERT)
(
    dvoid       *hndl,
    OCIError    *err,
    OCIDateTime *indate,
    OCIDateTime *outdate
);

typedef sword (*OCIDATETIMEFROMARRAY)
(
    void              *hndl,
    OCIError          *err,
    ub1               *inarray,
    ub4                len,
    ub1                type,
    OCIDateTime       *datetime,
    const OCIInterval *reftz,
    ub1                fsprec
);

typedef sword (*OCIDATETIMETOARRAY)
(
    dvoid             *hndl,
    OCIError          *err,
    CONST OCIDateTime *datetime,
    CONST OCIInterval *reftz,
    ub1               *outarray,
    ub4               *len,
    ub1                fsprec
);

typedef sword (*OCIDATETIMEFROMTEXT)
(
    dvoid         *hndl,
    OCIError      *err,
    CONST OraText *date_str,
    size_t         dstr_length,
    CONST OraText *fmt,
    ub1            fmt_length,
    CONST OraText *lang_name,
    size_t         lang_length,
    OCIDateTime   *datetime
);

typedef sword (*OCIDATETIMETOTEXT)
(
    dvoid             *hndl,
    OCIError          *err,
    CONST OCIDateTime *date,
    CONST OraText     *fmt,
    ub1                fmt_length,
    ub1                fsprec,
    CONST OraText     *lang_name,
    size_t             lang_length,
    ub4               *buf_size,
    OraText           *buf
);

typedef sword (*OCIDATETIMEGETDATE)
(
    dvoid             *hndl,
    OCIError          *err,
    CONST OCIDateTime *datetime,
    sb2               *year,
    ub1               *month,
    ub1               *day
);

typedef sword (*OCIDATETIMEGETTIME)
(
    dvoid       *hndl,
    OCIError    *err,
    OCIDateTime *datetime,
    ub1         *hour,
    ub1         *min,
    ub1         *sec,
    ub4         *fsec
);

typedef sword (*OCIDATETIMEGETTIMEZONENAME)
(
    dvoid             *hndl,
    OCIError          *err,
    CONST OCIDateTime *datetime,
    ub1               *buf,
    ub4               *buflen
);

typedef sword (*OCIDATETIMEGETTIMEZONEOFFSET)
(
    dvoid             *hndl,
    OCIError          *err,
    CONST OCIDateTime *datetime,
    sb1               *hour,
    sb1               *min
);

typedef sword (*OCIDATETIMEINTERVALADD)
(
    dvoid       *hndl,
    OCIError    *err,
    OCIDateTime *datetime,
    OCIInterval *inter,
    OCIDateTime *outdatetime
);

typedef sword (*OCIDATETIMEINTERVALSUB)
(
    dvoid       *hndl,
    OCIError    *err,
    OCIDateTime *datetime,
    OCIInterval *inter,
    OCIDateTime *outdatetime
);

typedef sword (*OCIDATETIMESUBTRACT)
(
    dvoid       *hndl,
    OCIError    *err,
    OCIDateTime *indate1,
    OCIDateTime *indate2,
    OCIInterval *inter
);

typedef sword (*OCIDATETIMESYSTIMESTAMP)
(
    dvoid       *hndl,
    OCIError    *err,
    OCIDateTime *sys_date
);

typedef sword (*OCITYPEBYREF)
(
    OCIEnv         *env,
    OCIError       *err,
    const OCIRef   *type_ref,
    OCIDuration     pin_duration,
    OCITypeGetOpt   get_option,
    OCIType       **tdo
);

typedef sword (*OCITYPEBYNAME)
(
    OCIEnv          *env, 
    OCIError        *err, 
    const OCISvcCtx *svc,
    const oratext*   schema_name,
    ub4              s_length,
    const oratext   *type_name,
    ub4              t_length,
    const oratext   *version_name, 
    ub4              v_length,
    OCIDuration      pin_duration,
    OCITypeGetOpt    get_option,
    OCIType        **tdo
);

typedef sword (*OCIOBJECTNEW)
(
    OCIEnv          *env,
    OCIError        *err,
    CONST OCISvcCtx *svc,
    OCITypeCode      typecode,
    OCIType         *tdo,
    dvoid           *table,
    OCIDuration      duration,
    boolean          value,
    dvoid          **instance
);

typedef sword (*OCIOBJECTFREE)
(
    OCIEnv   *env,
    OCIError *err,
    dvoid    *instance,
    ub2       flags
);

typedef sword (*OCIOBJECTGETATTR)
(
    OCIEnv          *env,
    OCIError        *err,
    dvoid           *instance,
    dvoid           *null_struct,
    struct OCIType  *tdo,
    CONST text     **names,
    CONST ub4       *lengths,
    CONST ub4        name_count,
    CONST ub4       *indexes,
    CONST ub4        index_count,
    OCIInd          *attr_null_status,
    dvoid          **attr_null_struct,
    dvoid          **attr_value,
    struct OCIType **attr_tdo
);

typedef sword (*OCIOBJECTSETATTR)
(
    OCIEnv         *env,
    OCIError       *err,
    dvoid          *instance,
    dvoid          *null_struct,
    struct OCIType *tdo,
    CONST text    **names,
    CONST ub4      *lengths,
    CONST ub4       name_count,
    CONST ub4      *indexes,
    CONST ub4       index_count,
    CONST OCIInd    null_status,
    CONST dvoid    *attr_null_struct,
    CONST dvoid    *attr_value
);

typedef sword (*OCIOBJECTPIN)
(
    OCIEnv           *env,
    OCIError         *err,
    OCIRef           *object_ref,
    OCIComplexObject *corhdl,
    OCIPinOpt         pin_option,
    OCIDuration       pin_duration,
    OCILockOpt        lock_option,
    dvoid           **object
);

typedef sword (*OCIOBJECTUNPIN)
(
    OCIEnv   *env,
    OCIError *err,
    dvoid    *object
);

typedef sword (*OCIOBJECTCOPY)
(
    OCIEnv          *env,
    OCIError        *err,
    CONST OCISvcCtx *svc,
    dvoid           *source,
    dvoid           *null_source,
    dvoid           *target,
    dvoid           *null_target,
    OCIType         *tdo,
    OCIDuration      duration,
    ub1              option
);

typedef sword (*OCIOBJECTGETOBJECTREF)
(
    OCIEnv   *env,
    OCIError *err,
    dvoid    *object,
    OCIRef   *object_ref
);

typedef sword (*OCIOBJECTGETPROPERTY)
(
    OCIEnv         *envh,
    OCIError       *errh,
    const void     *obj,
    OCIObjectPropId propertyId,
    void           *property,
    ub4            *size
);

typedef sword (*OCIOBJECTGETIND)
(
    OCIEnv   *env,
    OCIError *err,
    dvoid    *instance,
    dvoid   **null_struct
);

typedef sword (*OCIOBJECTGETTYPEREF)
(
    OCIEnv        *env,
    OCIError      *err,
    void          *instance,
    OCIRef        *type_ref
);

typedef sword (*OCIREFASSIGN)
(
    OCIEnv       *env,
    OCIError     *err,
    CONST OCIRef *source,
    OCIRef      **target
);

typedef boolean (*OCIREFISNULL)
(
    OCIEnv       *env,
    CONST OCIRef *ref
);

typedef void (*OCIREFCLEAR)
(
    OCIEnv *env,
    OCIRef *ref
);

typedef sword (*OCIREFTOHEX)
(
    OCIEnv       *env,
    OCIError     *err,
    CONST OCIRef *ref,
    oratext      *hex,
    ub4          *hex_length
);

typedef ub4 (*OCIREFHEXSIZE)
(
    OCIEnv       *env,
    const OCIRef *ref
);

typedef sword (*OCINUMBERTOINT)
(
    OCIError        *err,
    CONST OCINumber *number,
    uword            rsl_length,
    uword            rsl_flag,
    dvoid           *rsl
);

typedef sword (*OCINUMBERFROMINT)
(
    OCIError    *err,
    CONST dvoid *inum,
    uword        inum_length,
    uword        inum_s_flag,
    OCINumber   *number
);

typedef sword (*OCINUMBERFROMREAL)
(
    OCIError    *err,
    CONST dvoid *rnum,
    uword        rnum_length,
    OCINumber   *number
);

typedef sword (*OCINUMBERTOREAL)
(
    OCIError        *err,
    CONST OCINumber *number,
    uword            rsl_length,
    dvoid           *rsl
);

typedef sword (*OCINUMBERTOTEXT)
(
    OCIError        *err,
    CONST OCINumber *number,
    CONST text      *fmt,
    ub4              fmt_length,
    CONST text      *nls_params,
    ub4              nls_p_length,
    ub4             *buf_size,
    text            *buf
);

typedef sword (*OCINUMBERFROMTEXT)
(
    OCIError      *err,
    CONST oratext *str,
    ub4            str_length,
    CONST oratext *fmt,
    ub4            fmt_length,
    CONST oratext *nls_params,
    ub4            nls_p_length,
    OCINumber     *number
);

typedef sword (*OCINUMBERASSIGN)
(
    OCIError        *err, 
    CONST OCINumber *from,
    OCINumber       *to
);

typedef sword (*OCINUMBERADD)
(
    OCIError        *err, 
    const OCINumber *number1,
    const OCINumber *number2,
    OCINumber       *result
);

typedef sword (*OCINUMBERSUB)
(
    OCIError        *err, 
    const OCINumber *number1,
    const OCINumber *number2,
    OCINumber       *result
);

typedef sword (*OCINUMBERMUL)
(
    OCIError        *err, 
    const OCINumber *number1,
    const OCINumber *number2,
    OCINumber       *result
);

typedef sword (*OCINUMBERDIV)
(
    OCIError        *err, 
    const OCINumber *number1,
    const OCINumber *number2,
    OCINumber       *result
);

typedef sword(*OCINUMBERCMP)
(
    OCIError            *err,
    const OCINumber     *number1,
    const OCINumber     *number2,
    sword               *result
);

typedef oratext * (*OCISTRINGPTR)
(
    OCIEnv          *env,
    CONST OCIString *vs
);

typedef ub4 (*OCISTRINGSIZE) 
(
    OCIEnv          *env, 
    CONST OCIString *vs
);

typedef sword (*OCISTRINGASSIGNTEXT)
(
    OCIEnv        *env,
    OCIError      *err,
    CONST oratext *rhs,
    ub4            rhs_len,
    OCIString    **lhs
);

typedef sword (*OCISTRINGRESIZE)
(
    OCIEnv      *env, 
    OCIError    *err, 
    ub4         new_size,
    OCIString **str
);

typedef ub1 * (*OCIRAWPTR)
(
    OCIEnv      * env,
    CONST OCIRaw *raw
);

typedef sword (*OCIRAWASSIGNBYTES)
(
    OCIEnv    *env,
    OCIError  *err,
    CONST ub1 *rhs,
    ub4        rhs_len,
    OCIRaw   **lhs
);

typedef sword (*OCIRAWRESIZE)
(
    OCIEnv    *env, 
    OCIError  *err, 
    ub4        new_size, 
    OCIRaw   **raw    
);

typedef sword (*OCIRAWALLOCSIZE)
(
    OCIEnv       *env,
    OCIError     *err,
    CONST OCIRaw *raw,
    ub4          *allocsize
);

typedef ub4 (*OCIRAWSIZE)
(
    OCIEnv       *env,
    const OCIRaw *raw
);

typedef sword (*OCICONNECTIONPOOLCREATE)
(
    OCIEnv        *envhp,
    OCIError      *errhp,
    OCICPool      *poolhp,
    OraText      **poolName,
    sb4           *poolNameLen,
    CONST OraText *dblink,
    sb4            dblinkLen,
    ub4            connMin,
    ub4            connMax,
    ub4            connIncr,
    CONST OraText *poolUserName,
    sb4            poolUserLen,
    CONST OraText *poolPassword,
    sb4            poolPassLen,
    ub4            mode
);

typedef sword (*OCICONNECTIONPOOLDESTROY)
(
    OCICPool *poolhp,
    OCIError *errhp,
    ub4       mode
);

typedef sword (*OCISESSIONPOOLCREATE)
(
    OCIEnv        *envhp,
    OCIError      *errhp,
    OCISPool      *spoolhp,
    OraText      **poolName,
    ub4           *poolNameLen,
    CONST OraText *connStr,
    ub4            connStrLen,
    ub4            sessMin,
    ub4            sessMax,
    ub4            sessIncr,
    OraText       *userid,
    ub4            useridLen,
    OraText       *password,
    ub4            passwordLen,
    ub4            mode
);

typedef sword (*OCISESSIONPOOLDESTROY)
(
    OCISPool *spoolhp,
    OCIError *errhp,
    ub4       mode
);

typedef sword (*OCISESSIONGET)
(
    OCIEnv        *envhp,
    OCIError      *errhp,
    OCISvcCtx    **svchp,
    OCIAuthInfo   *authhp,
    OraText       *poolName,
    ub4            poolName_len,
    CONST OraText *tagInfo,
    ub4            tagInfo_len,
    OraText      **retTagInfo,
    ub4           *retTagInfo_len,
    boolean       *found,
    ub4            mode
);

typedef sword (*OCISESSIONRELEASE)
(
    OCISvcCtx *svchp,
    OCIError  *errhp,
    OraText   *tag,
    ub4        tag_len,
    ub4        mode
);

typedef sword (*OCICOLLSIZE)
(
    OCIEnv        *env,
    OCIError      *err,
    CONST OCIColl *coll,
    sb4           *size
);

typedef sb4 (*OCICOLLMAX)
(
    OCIEnv        *env,
    CONST OCIColl *coll
);

typedef sword (*OCICOLLGETITEM)
(
    OCIEnv        *env,
    OCIError      *err,
    CONST OCIColl *coll,
    sb4            index,
    boolean       *exists,
    dvoid        **elem,
    dvoid        **elemind
);

typedef sword (*OCICOLLASSIGNELEM)
(
    OCIEnv      *env,
    OCIError    *err,
    sb4          index,
    CONST dvoid *elem,
    CONST dvoid *elemind,
    OCIColl     *coll
);

typedef sword (*OCICOLLASSIGN)
(
    OCIEnv        *env,
    OCIError      *err,
    CONST OCIColl *rhs,
    OCIColl       *lhs
);

typedef sword (*OCICOLLAPPEND)
(
    OCIEnv      *env,
    OCIError    *err,
    CONST dvoid *elem,
    CONST dvoid *elemind,
    OCIColl     *coll
);

typedef sword (*OCICOLLTRIM)
(
    OCIEnv   *env,
    OCIError *err,
    sb4       trim_num,
    OCIColl  *coll
);

typedef sword (*OCITABLESIZE)
(
    OCIEnv         *env,
    OCIError       *err,
    CONST OCITable *tbl,
    sb4            *size
);

typedef sword (*OCITABLEDELETE)
(
    OCIEnv      *env,
    OCIError    *err,
    sb4          index,
    OCITable     *tbl
);

typedef sword (*OCIITERCREATE)
(
    OCIEnv        *env,
    OCIError      *err,
    CONST OCIColl *coll,
    OCIIter      **itr
);

typedef sword (*OCIITERDELETE)
(
    OCIEnv   *env,
    OCIError *err,
    OCIIter **itr
);

typedef sword (*OCIITERINIT)
(
    OCIEnv        *env,
    OCIError      *err,
    CONST OCIColl *coll,
    OCIIter       *itr
);

typedef sword (*OCIITERNEXT)
(
    OCIEnv   *env,
    OCIError *err,
    OCIIter  *itr,
    void    **elem,
    dvoid   **elemind,
    boolean  *eoc
);

typedef sword (*OCIITERPREV)
(
    OCIEnv   *env,
    OCIError *err,
    OCIIter  *itr,
    dvoid   **elem,
    dvoid   **elemind,
    boolean  *boc
);

typedef sword (*OCIAQENQ)
(
    OCISvcCtx          *svchp,
    OCIError           *errhp,
    OraText            *queue_name,
    OCIAQEnqOptions    *enqopt,
    OCIAQMsgProperties *msgprop,
    OCIType            *payload_tdo,
    dvoid             **payload,
    dvoid             **payload_ind,
    OCIRaw            **msgid,
    ub4                 flags
);

typedef sword (*OCIAQDEQ)
(
    OCISvcCtx          *svchp,
    OCIError           *errhp,
    OraText            *queue_name,
    OCIAQDeqOptions    *deqopt,
    OCIAQMsgProperties *msgprop,
    OCIType            *payload_tdo,
    dvoid             **payload,
    dvoid             **payload_ind,
    OCIRaw            **msgid,
    ub4                 flags
);

typedef sword (*OCIAQLISTEN)
(
    OCISvcCtx   *svchp,
    OCIError    *errhp,
    OCIAQAgent **agent_list,
    ub4          num_agents,
    sb4          wait,
    OCIAQAgent **agent,
    ub4          flags
);

typedef sword (*OCIROWIDTOCHAR)
(
    OCIRowid *rowidDesc, 
    OraText  *outbfp, 
    ub2      *outbflp,
    OCIError *errhp
);

typedef sword(*OCISTMTGETBINDINFO)
(
    OCIStmt   *stmtp, 
    OCIError  *errhp, 
    ub4        size,
    ub4        startloc,
    sb4       *found, 
    OraText   *bvnp[], 
    ub1        bvnl[],
    OraText   *invp[], 
    ub1        inpl[], 
    ub1        dupl[],
    OCIBind  **hndl
);

/* API introduced in 8.1 */

typedef sword (*OCIENVCREATE)
(
    OCIEnv **envhpp,
    ub4 mode,
    void *ctxp,
    void *(*malocfp)
    (
        void *ctxp, 
        size_t size
    ),
    void *(*ralocfp)
    (
        void *ctxp, 
        void *memptr, 
        size_t newsize),
    void (*mfreefp)
    (
        void *ctxp,
        void *memptr
    ),
    size_t xtramem_sz,
    void   **usrmempp
);

typedef sword (*OCIDIRPATHABORT)
(
    OCIDirPathCtx *dpctx,
    OCIError      *errhp
);

typedef sword (*OCIDIRPATHDATASAVE)
(
    OCIDirPathCtx *dpctx,
    OCIError      *errhp,
    ub4            action
);

typedef sword (*OCIDIRPATHFINISH)
(
    OCIDirPathCtx *dpctx,
    OCIError      *errhp
);

typedef sword (*OCIDIRPATHPREPARE)
(
    OCIDirPathCtx *dpctx,
    OCISvcCtx     *svchp,
    OCIError      *errhp
);

typedef sword (*OCIDIRPATHLOADSTREAM)
(
    OCIDirPathCtx    *dpctx,
    OCIDirPathStream *dpstr,
    OCIError         *errhp
);

typedef sword (*OCIDIRPATHCOLARRAYENTRYSET)
(
    OCIDirPathColArray *dpca,
    OCIError           *errhp,
    ub4                 rownum,
    ub2                 colIdx,
    ub1                *cvalp,
    ub4                 clen,
    ub1                 cflg
);

typedef sword (*OCIDIRPATHCOLARRAYRESET)
(
    OCIDirPathColArray *dpca,
    OCIError           *errhp
);

typedef sword (*OCIDIRPATHCOLARRAYTOSTREAM)
(
    OCIDirPathColArray *dpca,
    OCIDirPathCtx      *dpctx,
    OCIDirPathStream   *dpstr,
    OCIError           *errhp,
    ub4                 rowcnt,
    ub4                 rowoff
);

typedef sword (*OCIDIRPATHSTREAMRESET)
(
    OCIDirPathStream *dpstr,
    OCIError         *errhp
);

typedef sword (*OCIDIRPATHFLUSHROW)
(
    OCIDirPathCtx *dpctx,
    OCIError      *errhp
);

typedef sword (*OCICACHEFREE)
(
    OCIEnv          *env,
    OCIError        *err,
    CONST OCISvcCtx *svc
);

typedef sword (*OCISUBSCRIPTIONREGISTER)
(
    OCISvcCtx        *svchp,
    OCISubscription **subscrhpp,
    ub2               count,
    OCIError         *errhp,
    ub4               mode
);

typedef sword (*OCISUBSCRIPTIONUNREGISTER)
(
    OCISvcCtx       *svchp,
    OCISubscription *subscrhp,
    OCIError        *errhp,
    ub4              mode
);

/* API introduced in 9.1 */

typedef sword (*OCISTMTFETCH2 )
(
    OCIStmt  *stmtp,
    OCIError *errhp,
    ub4       nrows,
    ub2       orientation,
    sb4       fetchOffset,
    ub4       mode
);

/* API introduced in 9.2 */

typedef sword (*OCINLSGETINFO)
(
    void        *hndl, 
    OCIError    *errhp, 
    OraText     *buf, 
    size_t       buflen, 
    ub2          item 
);

typedef sword (*OCIENVNLSCREATE)
(
    OCIEnv **envp, ub4 mode, void  *ctxp,
    void  *(*malocfp)(void  *ctxp, size_t size),
    void  *(*ralocfp)(void  *ctxp, void  *memptr, size_t newsize),
    void   (*mfreefp)(void  *ctxp, void  *memptr),
    size_t xtramem_sz, void  **usrmempp,
    ub2 charset,
    ub2 ncharset
);

typedef sword (*OCISTMTPREPARE2)
(
    OCISvcCtx     *svchp,
    OCIStmt      **stmtp,
    OCIError      *errhp,
    CONST OraText *stmt,
    ub4            stmt_len,
    CONST OraText *key,
    ub4            key_len,
    ub4            language,
    ub4            mode
);

typedef sword (*OCISTMTRELEASE)
(
    OCIStmt       *stmtp,
    OCIError      *errhp,
    CONST OraText *key,
    ub4            key_len,
    ub4            mode
);

/* API introduced in 10.1 */

#ifdef ORAXB8_DEFINED

typedef sword (*OCILOBGETSTORAGELIMIT)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *lobp,
    oraub8        *limitp
);

typedef sword (*OCILOBCOPY2)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *dst_locp,
    OCILobLocator *src_locp,
    oraub8         amount,
    oraub8         dst_offset,
    oraub8         src_offset
);

typedef sword (*OCILOBERASE2)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *locp,
    oraub8        *amount,
    oraub8         offset
);

typedef sword (*OCILOBGETLENGTH2)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *locp,
    oraub8        *lenp
);

typedef sword (*OCILOBLOADFROMFILE2)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *dst_locp,
    OCILobLocator *src_filep,
    oraub8         amount,
    oraub8         dst_offset,
    oraub8         src_offset
);

typedef sword (*OCILOBREAD2)
(
    OCISvcCtx       *svchp,
    OCIError        *errhp,
    OCILobLocator   *locp,
    oraub8          *byte_amtp,
    oraub8          *char_amtp,
    oraub8           offset,
    dvoid           *bufp,
    oraub8           bufl,
    ub1              piece,
    dvoid           *ctxp,
    sb4              (*cbfp)
    (
        dvoid       *ctxp,
        CONST dvoid *bufp,
        oraub8       len,
        ub1          piece,
        dvoid      **changed_bufpp,
        oraub8      *changed_lenp
    ),
    ub2              csid,
    ub1              csfrm
);

typedef sword (*OCILOBTRIM2)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *locp,
    oraub8         newlen
);

typedef sword (*OCILOBWRITE2)
(
    OCISvcCtx       *svchp,
    OCIError        *errhp,
    OCILobLocator   *locp,
    oraub8          *byte_amtp,
    oraub8          *char_amtp,
    oraub8           offset,
    void            *bufp,
    oraub8           buflen,
    ub1              piece,
    void            *ctxp,
    sb4              (*cbfp)
    (
        void        *ctxp,
        void        *bufp,
        oraub8      *lenp,
        ub1         *piece,
        dvoid      **changed_bufpp,
        oraub8      *changed_lenp
    ),
    ub2              csid,
    ub1              csfrm
);

typedef sword (*OCILOBWRITEAPPEND2)
(
    OCISvcCtx     *svchp,
    OCIError      *errhp,
    OCILobLocator *lobp,
    oraub8        *byte_amtp,
    oraub8        *char_amtp,
    dvoid         *bufp,
    oraub8         bufl,
    ub1            piece,
    dvoid         *ctxp,
    sb4            (*cbfp)
    (
        dvoid     *ctxp,
        dvoid     *bufp,
        oraub8    *lenp,
        ub1       *piece,
        dvoid    **changed_bufpp,
        oraub8    *changed_lenp
    ),
    ub2            csid,
    ub1            csfrm
);

#endif /* ORAXB8_DEFINED */

/* API introduced in 10.2 */

/* Oracle 10g test */

typedef void (*OCICLIENTVERSION)
(
    sword *major_version,
    sword *minor_version,
    sword *update_num,
    sword *patch_num,
    sword *port_update_num
);

typedef sword (*OCIDBSTARTUP)
(
    OCISvcCtx *svchp,
    OCIError  *errhp,
    OCIAdmin  *admhp,
    ub4        mode,
    ub4        flags
);

typedef sword (*OCIDBSHUTDOWN)
(
    OCISvcCtx *svchp,
    OCIError  *errhp,
    OCIAdmin  *admhp,
    ub4        mode
);

typedef sword (*OCIPING)
(
    OCISvcCtx *svchp,
    OCIError  *errhp,
    ub4        mode
);

/* API introduced in 11.1 */

typedef sword (*OCIARRAYDESCRIPTORALLOC)
(
    const void  *parenth,
    void       **descpp,
    const ub4    type,
    ub4          array_size,
    const size_t xtramem_sz,
    void       **usrmempp
);

typedef sword (*OCIARRAYDESCRIPTORFREE)
(
    void    **descp,
    const ub4 type
);

/* API introduced in 11.2 */

typedef sword (*OCILOBGETCONTENTTYPE)
(
    OCISvcCtx       *svchp, 
    OCIError        *errhp,
    OCILobLocator   *lobp,
    oratext         *contenttypep, 
    ub4             *contenttypelenp,
    ub4              mode
);

/* API introduced in 12.1 */

typedef sword (*OCISTMTGETNEXTRESULT)
(
    OCIStmt     *stmthp,
    OCIError    *errhp,
    void       **result, 
    ub4         *rtype,
    ub4          mode
);

/* API introduced in 18.1 */

typedef sword(*OCISERVERRELEASE2)
(
    void        *hndlp,
    OCIError    *errhp, 
    OraText     *bufp, 
    ub4          bufsz, 
    ub1          hndltype, 
    ub4         *versionp, 
    ub4          mode
);

/* API introduced in 18.1 */

typedef sword(*OCISODACOLLCREATE)
(
    OCISvcCtx       *svchp,
    const OraText   *collname,
    ub4              collnamelen,
    OCISodaColl    **collection,
    OCIError        *errhp,
    ub4              mode
);

typedef sword(*OCISODAMETADATABUILD)
(
    OCISvcCtx             *svchp,
    const OCISodaMetadata *mdatahp,
    OraText               *metadata,
    ub4                   *metadatalen,
    OCIError              *errhp,
    ub4                    mode
);

typedef sword(*OCISODACOLLCREATEWITHMETADATA)
(
    OCISvcCtx      *svchp,
    const OraText  *collname,
    ub4             collnamelen,
    OraText        *metadata,
    ub4             metadatalen,
    OCISodaColl   **collection,
    OCIError       *errhp,
    ub4             mode
);

typedef sword(*OCISODACOLLOPEN)
(
    OCISvcCtx      *svchp,
    const OraText  *collname,
    ub4             collnamelen,
    OCISodaColl   **coll,
    OCIError       *errhp,
    ub4             mode
);

typedef sword(*OCISODADOCCREATE)
(
    OCIEnv            *envhp,
    const void        *content,
    ub4                contentLength,
    ub4                docFlags,
    OCISodaDoc       **document,
    OCIError          *errhp,
    ub4                mode
);

typedef sword(*OCISODADOCCREATEWITHKEY)
(
    OCIEnv        *envhp,
    const void    *content,
    ub4            contentLength,
    const OraText *key,
    ub4            keylen,
    ub4            docFlags,
    OCISodaDoc   **document,
    OCIError      *errhp,
    ub4            mode
);

typedef sword(*OCISODADOCCREATEWITHKEYANDMTYPE)
(
    OCIEnv              *envhp,
    const void          *content,
    ub4                  contentLength,
    const OraText       *key,
    ub4                  keylen,
    const OraText       *mediaType,
    ub4                  mediaTypeLength,
    ub4                  docFlags,
    OCISodaDoc         **document,
    OCIError            *errhp,
    ub4                  mode
);

typedef sword(*OCISODAINSERT)
(
    OCISvcCtx   *svchp,
    OCISodaColl *collection,
    OCISodaDoc  *document,
    OCIError    *errhp,
    ub4          mode
);

typedef sword(*OCISODAINSERTANDGET)
(
    OCISvcCtx      *svchp,
    OCISodaColl    *collection,
    OCISodaDoc    **document,
    OCIError       *errhp,
    ub4             mode
);

typedef sword(*OCISODAINSERTWITHCTNT)
(
    OCISvcCtx           *svchp,
    OCISodaColl         *collection,
    const OraText       *key,
    ub4                  keyLength,
    const void          *content,
    ub4                  contentLength,
    ub4                  docFlags,
    OCIError            *errhp,
    ub4                  mode
);

typedef sword(*OCISODAINSERTANDGETWITHCTNT)
(
    OCISvcCtx        *svchp,
    OCISodaColl      *collection,
    const OraText    *key,
    ub4               keyLength,
    const void       *content,
    ub4               contentLength,
    ub4               docFlags,
    OCISodaDoc      **document,
    OCIError         *errhp,
    ub4               mode
);

typedef sword(*OCISODAFIND)
(
    OCISvcCtx                     *svchp,
    const OCISodaColl             *coll,
    const OCISodaOperationOptions *findOptions,
    ub4                            docFlags,
    OCISodaDocCursor             **cursor,
    OCIError                      *errhp,
    ub4                            mode
);

typedef sword(*OCISODAFINDONE)
(
    OCISvcCtx                     *svchp,
    const OCISodaColl             *coll,
    const OCISodaOperationOptions *findOptions,
    ub4                            docFlags,
    OCISodaDoc                   **doc,
    OCIError                      *errhp,
    ub4                            mode
);

typedef sword(*OCISODAFINDONEWITHKEY)
(
    OCISvcCtx         *svchp,
    const OCISodaColl *coll,
    const OraText     *key,
    ub4                keylen,
    ub4                docFlags,
    OCISodaDoc       **doc,
    OCIError          *errhp,
    ub4                mode
);

typedef sword(*OCISODADOCGETNEXT)
(
    OCISvcCtx              *svchp,
    const OCISodaDocCursor *cursor,
    OCISodaDoc            **doc,
    OCIError               *errhp,
    ub4                     mode
);

typedef sword(*OCISODACOLLLIST)
(
    OCISvcCtx          *svchp,
    const OraText      *startname,
    ub4                 stnamelen,
    OCISodaCollCursor **cur,
    OCIError           *errhp,
    ub4                 mode
);

typedef sword(*OCISODACOLLGETNEXT)
(
    OCISvcCtx                *svchp,
    const OCISodaCollCursor  *cur,
    OCISodaColl             **coll,
    OCIError                 *errhp,
    ub4                       mode
);

typedef sword(*OCISODADOCCOUNT)
(
    OCISvcCtx                     *svchp,
    const OCISodaColl             *coll,
    const OCISodaOperationOptions *optns,
    ub8                           *numdocs,
    OCIError                      *errhp,
    ub4                            mode
);

typedef sword(*OCISODADOCCOUNTWITHFILTER)
(
    OCISvcCtx         *svchp,
    const OCISodaColl *coll,
    const OraText     *filterSpec,
    ub4                filterSpecLen,
    ub8               *numdocs,
    OCIError          *errhp,
    ub4                mode
);

typedef sword(*OCISODAREPLONE)
(
    OCISvcCtx                      *svchp,
    const OCISodaColl              *coll,
    const OCISodaOperationOptions  *optns,
    OCISodaDoc                     *document,
    boolean                        *isReplaced,
    OCIError                       *errhp,
    ub4                             mode
);

typedef sword(*OCISODAREPLONEANDGET)
(
    OCISvcCtx                      *svchp,
    const OCISodaColl              *coll,
    const OCISodaOperationOptions  *optns,
    OCISodaDoc                    **document,
    boolean                        *isReplaced,
    OCIError                       *errhp,
    ub4                             mode
);

typedef sword(*OCISODAREPLONEWITHKEY)
(
    OCISvcCtx           *svchp,
    const OCISodaColl   *coll,
    const OraText       *key,
    ub4                  keylength,
    OCISodaDoc          *document,
    boolean             *isReplaced,
    OCIError            *errhp,
    ub4                  mode
);

typedef sword(*OCISODAREPLONEANDGETWITHKEY)
(
    OCISvcCtx          *svchp,
    const OCISodaColl  *coll,
    const OraText      *key,
    ub4                 keylength,
    OCISodaDoc        **document,
    boolean             *isReplaced,
    OCIError           *errhp,
    ub4                 mode
);

typedef sword(*OCISODAREPLONEWITHCTNT)
(
    OCISvcCtx                     *svchp,
    const OCISodaColl             *coll,
    const OCISodaOperationOptions *optns,
    const void                    *content,
    ub4                            contentLength,
    ub4                            docFlags,
    boolean                       *isReplaced,
    OCIError                      *errhp,
    ub4                            mode
);

typedef sword(*OCISODAREPLONEANDGETWITHCTNT)
(
    OCISvcCtx                       *svchp,
    const OCISodaColl               *coll,
    const OCISodaOperationOptions   *optns,
    const void                      *content,
    ub4                              contentLen,
    ub4                              docFlags,
    OCISodaDoc                     **document,
    boolean                         *isReplaced,
    OCIError                        *errhp,
    ub4                              mode
);

typedef sword(*OCISODAREMOVE)
(
    OCISvcCtx                       *svchp,
    const OCISodaColl               *coll,
    const OCISodaOperationOptions   *optns,
    ub8                             *removeCount,
    OCIError                        *errhp,
    ub4                              mode
);

typedef sword(*OCISODAREMOVEONEWITHKEY)
(
    OCISvcCtx          *svchp,
    const OCISodaColl  *coll,
    const OraText      *key,
    ub4                 keylength,
    boolean            *isRemoved,
    OCIError           *errhp,
    ub4                 mode
);

typedef sword(*OCISODACOLLDROP)
(
    OCISvcCtx   *svchp,
    OCISodaColl *coll,
    boolean     *isDropped,
    OCIError    *errhp,
    ub4          mode
);

typedef sword(*OCISODAINDEXCREATE)
(
    OCISvcCtx         *svchp,
    const OCISodaColl *coll,
    const OraText     *indexspec,
    ub4                speclen,
    OCIError          *errhp,
    ub4                mode
);

typedef sword(*OCISODAINDEXDROP)
(
    OCISvcCtx   *svchp,
    OraText     *indexname,
    ub4          indexnamelen,
    boolean     *isDropped,
    OCIError    *errhp,
    ub4          mode
);

typedef sword(*OCISODASAVE)
(
    OCISvcCtx    *svchp,
    OCISodaColl  *coll,
    OCISodaDoc   *document,
    OCIError     *errhp,
    ub4           mode
);

typedef sword(*OCISODASAVEANDGET)
(
    OCISvcCtx    *svchp,
    OCISodaColl  *coll,
    OCISodaDoc  **document,
    OCIError     *errhp,
    ub4           mode
);

typedef sword(*OCISODAINDEXLIST)
(
    OCISvcCtx          *svchp,
    const OCISodaColl  *coll,
    ub4                 flags,
    OCIColl           **indexList,
    OCIError           *errhp,
    ub4                 mode
);

typedef sword(*OCISODADATAGUIDEGET)
(
    OCISvcCtx          *svchp,
    const OCISodaColl  *collection,
    ub4                 docFlags,
    OCISodaDoc        **doc,
    OCIError           *errhp,
    ub4                 mode
);

/* API introduced in 18.3 */

typedef sword(*OCISODAOPERKEYSSET)
(
    const OCISodaOperationOptions *operhp,
    OraText                       **keysArray,
    ub4                            *lengthsArray,
    ub4                             count,
    OCIError                       *errhp,
    ub4                             mode
);

/* API introduced in 18.5 */

typedef sword(*OCISODABULKINSERTWITHCTNT)
(
    OCISvcCtx            *svchp,
    OCISodaColl          *collection,
    void                **contentStrings,
    ub4                  *contentStringLengths,
    OraText              **keys,
    ub4                  *keyLengths,
    ub4                   arrayLength,
    ub4                   docFlags,
    OCISodaOutputOptions *opoptns,
    OCIError             *errhp,
    ub4                   mode
);

typedef sword(*OCISODABULKINSERT)
(
    OCISvcCtx            *svchp,
    OCISodaColl          *collection,
    OCISodaDoc          **documentarray,
    ub4                   arraylen,
    OCISodaOutputOptions *opoptns,
    OCIError             *errhp,
    ub4                   mode
);

typedef sword(*OCISODABULKINSERTANDGET)
(
    OCISvcCtx            *svchp,
    OCISodaColl          *collection,
    OCISodaDoc          **documentarray,
    ub4                   arraylen,
    OCISodaOutputOptions *opoptns,
    OCIError             *errhp,
    ub4                   mode
);

typedef sword(*OCISODABULKINSERTANDGETWITHCTNT)
(
    OCISvcCtx       *svchp,
    OCISodaColl     *collection,
    void           **contentStrings,
    ub4             *contentStringLengths,
    OraText        **keys,
    ub4             *keyLengths,
    ub4              arrayLength,
    ub4              docFlags,
    OCISodaDoc     **documents,
    OCISodaOutputOptions *opoptns,
    OCIError        *errhp,
    ub4              mode
);

typedef sword(*OCISODABULKINSERTANDGETWITHOPTS)
(
    OCISvcCtx                 *svchp,
    OCISodaColl               *collection,
    OCISodaDoc               **documentarray,
    ub4                        arraylen,
    OCISodaOperationOptions   *oproptns,
    OCISodaOutputOptions      *opoptns,
    OCIError                  *errhp,
    ub4                        mode
);

/* API introduced in 20.1 */

typedef sword(*OCISODACOLLTRUNCATE)
(
    OCISvcCtx         *svchp,
    const OCISodaColl *collection,
    OCIError          *errhp,
    ub4                mode
);

/* API introduced in 19.3 */

typedef sword(*OCISERVERDATALENGTHGET)
(
    void     *hndlp,
    boolean  *lengthValidp,
    ub8      *lengthp,
    OCIError *errhp,
    ub4       mode
);

/* API introduced in 23.4 */

typedef sword(*OCIVECTORTOARRAY)
(
    OCIVector *vectord, 
    OCIError *errhp, 
    ub1 vformat,
    ub4 *vdim, 
    void *vecarray,
    ub4 mode
);

typedef sword(*OCIVECTORFROMARRAY)
(
    OCIVector *vectord, 
    OCIError *errhp, 
    ub1 vformat,
    ub4 vdim, 
    void *vecarray,
    ub4 mode
);

typedef sword(*OCIVECTORTOTEXT)
(
    OCIVector *vectord,
    OCIError *errhp, 
    OraText *vtext,
    ub4 *vtextlen, 
    ub4 mode
);

typedef sword(*OCIVECTORFROMTEXT)
(
    OCIVector *vectord,
    OCIError *errhp,
    ub1 vformat,
    ub4 vdim,
    const OraText *vtext,
    ub4 vtextlen,
    ub4 mode
);

/* API introduced in 21.1 */

typedef sword(*OCIJSONTEXTBUFFERPARSE)
(
    void     *hndlp,
    OCIJson  *jsond,
    void     *bufp,
    oraub8    buf_sz,
    ub4       validation,
    ub2       encoding,
    OCIError *errhp,
    ub4       mode
);

typedef sword(*OCIJSONTOTEXTBUFFER)
(
    OCISvcCtx *svchp,
    OCIJson   *jsond,
    OraText   *bufp,
    oraub8    *byte_amtp,
    ub4        print,
    OCIError  *errhp,
    ub4        mode
);

typedef sword(*OCIJSONBINARYBUFFERLOAD)
(
    void     *hndlp,
    OCIJson  *jsond,
    ub1      *bufp,
    oraub8   buf_sz,
    OCIError *errhp,
    ub4      mode
);

typedef sword(*OCIJSONTOBINARYBUFFER)
(
    OCISvcCtx *svchp,
    OCIJson   *jsond,
    ub1       *bufp,
    oraub8    *byte_amtp,
    OCIError  *errhp,
    ub4        mode
);

#ifndef JSONDOM_H
#define JSONDOM_H
typedef struct JsonDomDoc JsonDomDoc;
typedef struct JsonDomNode JsonDomNode;
#endif

typedef sword(*OCIJSONDOMDOCGET)
(
    OCISvcCtx  *svchp,
    OCIJson    *jsond,
    JsonDomDoc **domdoc,
    OCIError   *errhp,
    ub4         mode
);

typedef sword(*OCIJSONDOMDOCSET)
(
    void       *hndlp,
    OCIJson    *jsond,
    JsonDomDoc *domdoc,
    OCIError   *errhp,
    ub4         mode
);

typedef sword(*OCIJSONCLONE)
(
    OCISvcCtx *svchp,
    OCIJson   *src_jsond,
    OCIJson   *dst_jsond,
    OCIError  *errhp,
    ub4        mode
);

typedef sword(*OCIJSONBINARYLENGTHGET)
(
    OCISvcCtx *svchp,
    OCIJson   *jsond,
    oraub8    *byte_amtp,
    OCIError  *errhp,
    ub4        mode
);

#ifndef JSONDOM_H
#define JSONDOM_H
typedef struct JsonDomDoc JsonDomDoc;
typedef struct JsonDomNode JsonDomNode;
#endif



/* per-environment OCI symbol dispatch table */

typedef struct OCI_Symbols
{
    OCIINITIALIZE                            OCIInitialize;
    OCIENVINIT                               OCIEnvInit;
    OCIENVCREATE                             OCIEnvCreate;
    OCISERVERATTACH                          OCIServerAttach;
    OCISERVERDETACH                          OCIServerDetach;
    OCIHANDLEALLOC                           OCIHandleAlloc;
    OCIHANDLEFREE                            OCIHandleFree;
    OCIDESCRIPTORALLOC                       OCIDescriptorAlloc;
    OCIDESCRIPTORFREE                        OCIDescriptorFree;
    OCISESSIONBEGIN                          OCISessionBegin;
    OCISESSIONEND                            OCISessionEnd;
    OCIPASSWORDCHANGE                        OCIPasswordChange;
    OCIBINDBYPOS                             OCIBindByPos;
    OCIBINDBYNAME                            OCIBindByName;
    OCIBINDDYNAMIC                           OCIBindDynamic;
    OCIBINDOBJECT                            OCIBindObject;
    OCIDEFINEBYPOS                           OCIDefineByPos;
    OCIDEFINEDYNAMIC                         OCIDefineDynamic;
    OCIDEFINEOBJECT                          OCIDefineObject;
    OCISTMTPREPARE                           OCIStmtPrepare;
    OCISTMTEXECUTE                           OCIStmtExecute;
    OCISTMTFETCH                             OCIStmtFetch;
    OCISTMTFETCH2                            OCIStmtFetch2;
    OCISTMTGETPIECEINFO                      OCIStmtGetPieceInfo;
    OCISTMTSETPIECEINFO                      OCIStmtSetPieceInfo;
    OCIPARAMGET                              OCIParamGet;
    OCIPARAMSET                              OCIParamSet;
    OCITRANSSTART                            OCITransStart;
    OCITRANSDETACH                           OCITransDetach;
    OCITRANSPREPARE                          OCITransPrepare;
    OCITRANSFORGET                           OCITransForget;
    OCITRANSCOMMIT                           OCITransCommit;
    OCITRANSROLLBACK                         OCITransRollback;
    OCIERRORGET                              OCIErrorGet;
    OCILOBCREATETEMPORARY                    OCILobCreateTemporary;
    OCILOBFREETEMPORARY                      OCILobFreeTemporary;
    OCILOBISTEMPORARY                        OCILobIsTemporary;
    OCILOBAPPEND                             OCILobAppend;
    OCILOBCOPY                               OCILobCopy;
    OCILOBGETLENGTH                          OCILobGetLength;
    OCILOBGETCHUNKSIZE                       OCILobGetChunkSize;
    OCILOBREAD                               OCILobRead;
    OCILOBWRITE                              OCILobWrite;
    OCILOBTRIM                               OCILobTrim;
    OCILOBERASE                              OCILobErase;
    OCILOBOPEN                               OCILobOpen;
    OCILOBCLOSE                              OCILobClose;
    OCILOBLOCATORASSIGN                      OCILobLocatorAssign;
    OCILOBASSIGN                             OCILobAssign;
    OCILOBISEQUAL                            OCILobIsEqual;
    OCILOBFLUSHBUFFER                        OCILobFlushBuffer;
    OCILOBGETSTORAGELIMIT                    OCILobGetStorageLimit;
    OCILOBENABLEBUFFERING                    OCILobEnableBuffering;
    OCILOBDISABLEBUFFERING                   OCILobDisableBuffering;
    OCILOBFILEOPEN                           OCILobFileOpen;
    OCILOBFILECLOSE                          OCILobFileClose;
    OCILOBFILECLOSEALL                       OCILobFileCloseAll;
    OCILOBFILEISOPEN                         OCILobFileIsOpen;
    OCILOBFILEEXISTS                         OCILobFileExists;
    OCILOBFIELGETNAME                        OCILobFileGetName;
    OCILOBFILESETNAME                        OCILobFileSetName;
    OCILOBLOADFROMFILE                       OCILobLoadFromFile;
    OCILOBWRITEAPPEND                        OCILobWriteAppend;
    OCISERVERVERSION                         OCIServerVersion;
    OCIBREAK                                 OCIBreak;
    OCIATTRGET                               OCIAttrGet;
    OCIATTRSET                               OCIAttrSet;
    OCIDATEASSIGN                            OCIDateAssign;
    OCIDATETOTEXT                            OCIDateToText;
    OCIDATEFROMTEXT                          OCIDateFromText;
    OCIDATECOMPARE                           OCIDateCompare;
    OCIDATEADDMONTHS                         OCIDateAddMonths;
    OCIDATEADDDAYS                           OCIDateAddDays;
    OCIDATELASTDAY                           OCIDateLastDay;
    OCIDATEDAYSBETWEEN                       OCIDateDaysBetween;
    OCIDATEZONETOZONE                        OCIDateZoneToZone;
    OCIDATENEXTDAY                           OCIDateNextDay;
    OCIDATECHECK                             OCIDateCheck;
    OCIDATESYSDATE                           OCIDateSysDate;
    OCIDESCRIBEANY                           OCIDescribeAny;
    OCIINTERVALASSIGN                        OCIIntervalAssign;
    OCIINTERVALCHECK                         OCIIntervalCheck;
    OCIINTERVALCOMPARE                       OCIIntervalCompare;
    OCIINTERVALFROMTEXT                      OCIIntervalFromText;
    OCIINTERVALTOTEXT                        OCIIntervalToText;
    OCIINTERVALFROMTZ                        OCIIntervalFromTZ;
    OCIINTERVALGETDAYSECOND                  OCIIntervalGetDaySecond;
    OCIINTERVALGETYEARMONTH                  OCIIntervalGetYearMonth;
    OCIINTERVALSETDAYSECOND                  OCIIntervalSetDaySecond;
    OCIINTERVALSETYEARMONTH                  OCIIntervalSetYearMonth;
    OCIINTERVALSUBTRACT                      OCIIntervalSubtract;
    OCIINTERVALADD                           OCIIntervalAdd;
    OCIDATETIMEASSIGN                        OCIDateTimeAssign;
    OCIDATETIMECHECK                         OCIDateTimeCheck;
    OCIDATETIMECOMPARE                       OCIDateTimeCompare;
    OCIDATETIMECONSTRUCT                     OCIDateTimeConstruct;
    OCIDATETIMECONVERT                       OCIDateTimeConvert;
    OCIDATETIMEFROMARRAY                     OCIDateTimeFromArray;
    OCIDATETIMETOARRAY                       OCIDateTimeToArray;
    OCIDATETIMEFROMTEXT                      OCIDateTimeFromText;
    OCIDATETIMETOTEXT                        OCIDateTimeToText;
    OCIDATETIMEGETDATE                       OCIDateTimeGetDate;
    OCIDATETIMEGETTIME                       OCIDateTimeGetTime;
    OCIDATETIMEGETTIMEZONENAME               OCIDateTimeGetTimeZoneName;
    OCIDATETIMEGETTIMEZONEOFFSET             OCIDateTimeGetTimeZoneOffset;
    OCIDATETIMEINTERVALADD                   OCIDateTimeIntervalAdd;
    OCIDATETIMEINTERVALSUB                   OCIDateTimeIntervalSub;
    OCIDATETIMESUBTRACT                      OCIDateTimeSubtract;
    OCIDATETIMESYSTIMESTAMP                  OCIDateTimeSysTimeStamp;
    OCIARRAYDESCRIPTORALLOC                  OCIArrayDescriptorAlloc;
    OCIARRAYDESCRIPTORFREE                   OCIArrayDescriptorFree;
    OCICLIENTVERSION                         OCIClientVersion;
    OCITYPEBYREF                             OCITypeByRef;
    OCITYPEBYNAME                            OCITypeByName;
    OCINUMBERTOINT                           OCINumberToInt;
    OCINUMBERFROMINT                         OCINumberFromInt;
    OCINUMBERTOREAL                          OCINumberToReal;
    OCINUMBERFROMREAL                        OCINumberFromReal;
    OCINUMBERTOTEXT                          OCINumberToText;
    OCINUMBERFROMTEXT                        OCINumberFromText;
    OCINUMBERASSIGN                          OCINumberAssign;
    OCINUMBERADD                             OCINumberAdd;
    OCINUMBERSUB                             OCINumberSub;
    OCINUMBERMUL                             OCINumberMul;
    OCINUMBERDIV                             OCINumberDiv;
    OCINUMBERCMP                             OCINumberCmp;
    OCISTRINGPTR                             OCIStringPtr;
    OCISTRINGSIZE                            OCIStringSize;
    OCISTRINGASSIGNTEXT                      OCIStringAssignText;
    OCISTRINGRESIZE                          OCIStringResize;
    OCIRAWPTR                                OCIRawPtr;
    OCIRAWASSIGNBYTES                        OCIRawAssignBytes;
    OCIRAWRESIZE                             OCIRawResize;
    OCIRAWALLOCSIZE                          OCIRawAllocSize;
    OCIRAWSIZE                               OCIRawSize;
    OCIOBJECTNEW                             OCIObjectNew;
    OCIOBJECTFREE                            OCIObjectFree;
    OCIOBJECTSETATTR                         OCIObjectSetAttr;
    OCIOBJECTGETATTR                         OCIObjectGetAttr;
    OCIOBJECTPIN                             OCIObjectPin;
    OCIOBJECTUNPIN                           OCIObjectUnpin;
    OCIOBJECTCOPY                            OCIObjectCopy;
    OCIOBJECTGETOBJECTREF                    OCIObjectGetObjectRef;
    OCIOBJECTGETTYPEREF                      OCIObjectGetTypeRef;
    OCIOBJECTGETPROPERTY                     OCIObjectGetProperty;
    OCIOBJECTGETIND                          OCIObjectGetInd;
    OCIREFASSIGN                             OCIRefAssign;
    OCIREFISNULL                             OCIRefIsNull;
    OCIREFCLEAR                              OCIRefClear;
    OCIREFTOHEX                              OCIRefToHex;
    OCIREFHEXSIZE                            OCIRefHexSize;
    OCICONNECTIONPOOLCREATE                  OCIConnectionPoolCreate;
    OCICONNECTIONPOOLDESTROY                 OCIConnectionPoolDestroy;
    OCISESSIONPOOLCREATE                     OCISessionPoolCreate;
    OCISESSIONPOOLDESTROY                    OCISessionPoolDestroy;
    OCISESSIONGET                            OCISessionGet;
    OCISESSIONRELEASE                        OCISessionRelease;
    OCICOLLSIZE                              OCICollSize;
    OCICOLLMAX                               OCICollMax;
    OCICOLLGETITEM                           OCICollGetElem;
    OCICOLLASSIGNELEM                        OCICollAssignElem;
    OCICOLLASSIGN                            OCICollAssign;
    OCICOLLAPPEND                            OCICollAppend;
    OCICOLLTRIM                              OCICollTrim;
    OCITABLESIZE                             OCITableSize;
    OCITABLEDELETE                           OCITableDelete;
    OCIITERCREATE                            OCIIterCreate;
    OCIITERDELETE                            OCIIterDelete;
    OCIITERINIT                              OCIIterInit;
    OCIITERNEXT                              OCIIterNext;
    OCIITERPREV                              OCIIterPrev;
    OCIDIRPATHABORT                          OCIDirPathAbort;
    OCIDIRPATHDATASAVE                       OCIDirPathDataSave;
    OCIDIRPATHFINISH                         OCIDirPathFinish;
    OCIDIRPATHPREPARE                        OCIDirPathPrepare;
    OCIDIRPATHLOADSTREAM                     OCIDirPathLoadStream;
    OCIDIRPATHCOLARRAYENTRYSET               OCIDirPathColArrayEntrySet;
    OCIDIRPATHCOLARRAYRESET                  OCIDirPathColArrayReset;
    OCIDIRPATHCOLARRAYTOSTREAM               OCIDirPathColArrayToStream;
    OCIDIRPATHSTREAMRESET                    OCIDirPathStreamReset;
    OCIDIRPATHFLUSHROW                       OCIDirPathFlushRow;
    OCICACHEFREE                             OCICacheFree;
    OCIPING                                  OCIPing;
    OCIAQENQ                                 OCIAQEnq;
    OCIAQDEQ                                 OCIAQDeq;
    OCIAQLISTEN                              OCIAQListen;
    OCIDBSTARTUP                             OCIDBStartup;
    OCIDBSHUTDOWN                            OCIDBShutdown;
    OCIENVNLSCREATE                          OCIEnvNlsCreate;
    OCINLSGETINFO                            OCINlsGetInfo;
    OCISTMTPREPARE2                          OCIStmtPrepare2;
    OCISTMTRELEASE                           OCIStmtRelease;
    OCISUBSCRIPTIONREGISTER                  OCISubscriptionRegister;
    OCISUBSCRIPTIONUNREGISTER                OCISubscriptionUnRegister;
    OCILOBGETCONTENTTYPE                     OCILobGetContentType;
    OCISTMTGETNEXTRESULT                     OCIStmtGetNextResult;
    OCISERVERRELEASE2                        OCIServerRelease2;
    OCISODAOPERKEYSSET                       OCISodaOperKeysSet;
    OCIROWIDTOCHAR                           OCIRowidToChar;
    OCISTMTGETBINDINFO                       OCIStmtGetBindInfo;
    OCISODABULKINSERT                        OCISodaBulkInsert;

    OCISODACOLLCREATE                        OCISodaCollCreate;
    OCISODAMETADATABUILD                     OCISodaMetadataBuild;
    OCISODACOLLCREATEWITHMETADATA            OCISodaCollCreateWithMetadata;
    OCISODACOLLOPEN                          OCISodaCollOpen;
    OCISODADOCCREATE                         OCISodaDocCreate;
    OCISODADOCCREATEWITHKEY                  OCISodaDocCreateWithKey;
    OCISODADOCCREATEWITHKEYANDMTYPE          OCISodaDocCreateWithKeyAndMType;
    OCISODAINSERT                            OCISodaInsert;
    OCISODAINSERTANDGET                      OCISodaInsertAndGet;
    OCISODAINSERTWITHCTNT                    OCISodaInsertWithCtnt;
    OCISODAINSERTANDGETWITHCTNT              OCISodaInsertAndGetWithCtnt;
    OCISODAFIND                              OCISodaFind;
    OCISODAFINDONE                           OCISodaFindOne;
    OCISODAFINDONEWITHKEY                    OCISodaFindOneWithKey;
    OCISODADOCGETNEXT                        OCISodaDocGetNext;
    OCISODACOLLLIST                          OCISodaCollList;
    OCISODACOLLGETNEXT                       OCISodaCollGetNext;
    OCISODADOCCOUNT                          OCISodaDocCount;
    OCISODADOCCOUNTWITHFILTER                OCISodaDocCountWithFilter;
    OCISODAREPLONE                           OCISodaReplOne;
    OCISODAREPLONEANDGET                     OCISodaReplOneAndGet;
    OCISODAREPLONEWITHKEY                    OCISodaReplOneWithKey;
    OCISODAREPLONEANDGETWITHKEY              OCISodaReplOneAndGetWithKey;
    OCISODAREPLONEWITHCTNT                   OCISodaReplOneWithCtnt;
    OCISODAREPLONEANDGETWITHCTNT             OCISodaReplOneAndGetWithCtnt;
    OCISODAREMOVE                            OCISodaRemove;
    OCISODAREMOVEONEWITHKEY                  OCISodaRemoveOneWithKey;
    OCISODACOLLDROP                          OCISodaCollDrop;
    OCISODAINDEXCREATE                       OCISodaIndexCreate;
    OCISODAINDEXDROP                         OCISodaIndexDrop;
    OCISODASAVE                              OCISodaSave;
    OCISODASAVEANDGET                        OCISodaSaveAndGet;
    OCISODAINDEXLIST                         OCISodaIndexList;
    OCISODADATAGUIDEGET                      OCISodaDataGuideGet;
    OCISODABULKINSERTWITHCTNT                OCISodaBulkInsertWithCtnt;
    OCISODABULKINSERTANDGET                  OCISodaBulkInsertAndGet;
    OCISODABULKINSERTANDGETWITHCTNT          OCISodaBulkInsertAndGetWithCtnt;
    OCISODABULKINSERTANDGETWITHOPTS          OCISodaBulkInsertAndGetWithOpts;
    OCISODACOLLTRUNCATE                      OCISodaCollTruncate;
    OCISERVERDATALENGTHGET                   OCIServerDataLengthGet;
    OCIVECTORTOARRAY                         OCIVectorToArray;
    OCIVECTORFROMARRAY                       OCIVectorFromArray;
    OCIVECTORTOTEXT                          OCIVectorToText;
    OCIVECTORFROMTEXT                        OCIVectorFromText;

    OCIJSONTEXTBUFFERPARSE                   OCIJsonTextBufferParse;
    OCIJSONTOTEXTBUFFER                      OCIJsonToTextBuffer;
    OCIJSONBINARYBUFFERLOAD                  OCIJsonBinaryBufferLoad;
    OCIJSONTOBINARYBUFFER                    OCIJsonToBinaryBuffer;
    OCIJSONDOMDOCGET                         OCIJsonDomDocGet;
    OCIJSONDOMDOCSET                         OCIJsonDomDocSet;
    OCIJSONCLONE                             OCIJsonClone;
    OCIJSONBINARYLENGTHGET                   OCIJsonBinaryLengthGet;

#ifdef ORAXB8_DEFINED
    OCILOBCOPY2                              OCILobCopy2;
    OCILOBERASE2                             OCILobErase2;
    OCILOBGETLENGTH2                         OCILobGetLength2;
    OCILOBLOADFROMFILE2                      OCILobLoadFromFile2;
    OCILOBREAD2                              OCILobRead2;
    OCILOBTRIM2                              OCILobTrim2;
    OCILOBWRITE2                             OCILobWrite2;
    OCILOBWRITEAPPEND2                       OCILobWriteAppend2;
#endif
} OCI_Symbols;
#endif /* OCILIB_OCI_API_H_INCLUDED */

