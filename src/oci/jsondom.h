/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@gmail.com>
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
 * FILE AND HAS PARTIALLY INTEGRATED SOME ORACLE JSON DOM DEFINITIONS TO
 * ALLOW COMPILATION OF THIS OPEN SOURCE LIBRARY WITHOUT HAVING
 * ORACLE PRODUCTS INSTALLED ON DEVELOPMENT ENVIRONMENTS
 */

#ifndef OCILIB_OCI_JSONDOM_H_INCLUDED
#define OCILIB_OCI_JSONDOM_H_INCLUDED

#include "types.h"

/* In OCI_IMPORT_LINKAGE builds against Oracle 21c+ SDKs, the real jzntypes.h
   and jzndom.h are already included via import.h — skip the vendored
   declarations below to avoid duplicate/conflicting definitions.
   JsonDomGetFields is a macro defined by the real jzndom.h, so its presence
   is a reliable marker that the real header was seen.                       */

#if !defined(JsonDomGetFields)

/* JSON value types */
typedef enum jznvaltype
{
    JZNVAL_UNKNOWN = 0,
    JZNVAL_NONE    = 1,
    JZNVAL_NULL    = 2,
    JZNVAL_STRING  = 3,
    JZNVAL_NUMBER  = 4,
    JZNVAL_FALSE   = 5,
    JZNVAL_TRUE    = 6,
    JZNVAL_SINT    = 7,
    JZNVAL_SLONG   = 8,
    JZNVAL_UINT    = 9,
    JZNVAL_ULONG   = 10,
    JZNVAL_FLOAT   = 11,
    JZNVAL_DOUBLE  = 12,
    JZNVAL_BINARY  = 13,
    JZNVAL_TSTAMP  = 14,
    JZNVAL_OID     = 15,
    JZNVAL_UUID    = 16,
    JZNVAL_ORA_NUMBER        = 17,
    JZNVAL_ORA_DATE          = 18,
    JZNVAL_ORA_TIMESTAMP     = 19,
    JZNVAL_ORA_TIMESTAMPTZ   = 20,
    JZNVAL_ORA_YEARMONTH_DUR = 21,
    JZNVAL_ORA_DAYSECOND_DUR = 22,
    JZNVAL_OPAQUE            = 23,
    JZNVAL_ORA_TIME          = 26,
    JZNVAL_OCI_NUMBER        = 32,
    JZNVAL_OCI_DATE          = 33,
    JZNVAL_OCI_DATETIME      = 34,
    JZNVAL_OCI_STRING        = 35,
    JZNVAL_OCI_RAW           = 36,
    JZNVAL_BASE64            = 37,
    JZNVAL_CANONICAL_FLOAT   = 38,
    JZNVAL_CANONICAL_DOUBLE  = 39,
    JZNVAL_OCI_INTERVAL      = 40,
    JZNVAL_ORA_BOOLEAN       = 41,
    JZNVAL_ANY_TYPE          = 42,
    JZNVAL_ANSI_DATE         = 43,
    JZNVAL_ID                = 31
} jznvaltype;

/* JSON DOM node types */
typedef enum
{
    JZNDOM_UNKNOWN = 0,
    JZNDOM_SCALAR  = 1,
    JZNDOM_OBJECT  = 2,
    JZNDOM_ARRAY   = 3
} jznnodetype;

/* JSON DOM types */
typedef struct jznDomDoc     jznDomDoc;
typedef struct jznDomNode    jznDomNode;
typedef struct jznDomObject  jznDomObject;
typedef struct jznDomArray   jznDomArray;
typedef struct jznDomScalar  jznDomScalar;

typedef struct jznDomObjFieldName jznDomObjFieldName;
typedef struct jznDomNameValuePair jznDomNameValuePair;

typedef sword jsonerr;

typedef struct jznEventSource jznEventSource;
typedef struct jznEventWriter jznEventWriter;
typedef struct JsonDomVisitorInfo JsonDomVisitorInfo;
typedef void (*JsonDomVisitorFunc)(JsonDomVisitorInfo *vinfo, void *appctx);

typedef ub4 jznoct_hashidt;
typedef ub4 jzndom_fnmOff;

/* DOM scalar value union */
typedef union jzn_sval_union
{
    struct string_value
    {
        oratext *value;
        ub4      valuelen;
        ub1      dummy1;
        ub1      dummy2;
        ub1      dummy3;
        ub1      flags;
    } val;
    struct binary_value
    {
        ub1     *value;
        ub4      valuelen;
        ub1      subtype;
        ub1      dummy2;
        ub1      dummy3;
        ub1      flags;
    } bval;
    struct integer_value
    {
        sb8      longval;
        sb4      intval;
    } ival;
    struct unsigned_value
    {
        ub8      longval;
        ub4      intval;
    } uval;
    struct float_value
    {
        float    ftval;
    } fval;
    struct double_value
    {
        double   fpval;
    } dval;
    struct obuf_value
    {
        ub1     *obinptr_obinval;
        ub4      oblen_obinval;
        ub1      dummy1;
        ub1      dummy2;
        ub1      dummy3;
        ub1      flags;
    } obinval;
} jzn_sval_union;

/* DOM scalar value */
typedef struct jznScalarVal
{
    jznvaltype     type_jznScalarVal;
    jzn_sval_union uval_jznScalarVal;
} jznScalarVal;

#define len_jznScalarVal       uval_jznScalarVal.val.valuelen
#define val_jznScalarVal       uval_jznScalarVal.val.value
#define binval_jznScalarVal    uval_jznScalarVal.bval.value
#define binlen_jznScalarVal    uval_jznScalarVal.bval.valuelen
#define sb4val_jznScalarVal    uval_jznScalarVal.ival.intval
#define ub4val_jznScalarVal    uval_jznScalarVal.uval.intval
#define sb8val_jznScalarVal    uval_jznScalarVal.ival.longval
#define ub8val_jznScalarVal    uval_jznScalarVal.uval.longval
#define flt_jznScalarVal       uval_jznScalarVal.fval.ftval
#define db_jznScalarVal        uval_jznScalarVal.dval.fpval
#define oranumval_jznScalarVal uval_jznScalarVal.obinval.obinptr_obinval
#define oranumlen_jznScalarVal uval_jznScalarVal.obinval.oblen_obinval

/* Timestamp helper */
typedef struct JsonDateTime
{
    sb2 year_JsonDateTime;
    ub1 month_JsonDateTime;
    ub1 day_JsonDateTime;
    ub1 hour_JsonDateTime;
    ub1 minute_JsonDateTime;
    ub1 second_JsonDateTime;
    ub1 flags_JsonDateTime;
    ub4 fsecond_JsonDateTime;
    sb1 tzHourOffset_JsonDateTime;
    sb1 tzMinuteOffset_JsonDateTime;
    ub2 unused_JsonDateTime;
} JsonDateTime;

/* Year-month interval helper */
typedef struct JsonYearInterval
{
    sb4 years_JsonYearInterval;
    sb4 months_JsonYearInterval;
} JsonYearInterval;

/* Day-second interval helper */
typedef struct JsonDayInterval
{
    sb4 days_JsonDayInterval;
    sb4 hours_JsonDayInterval;
    sb4 minutes_JsonDayInterval;
    sb4 seconds_JsonDayInterval;
    sb4 fseconds_JsonDayInterval;
} JsonDayInterval;

/* OCI-compatible value union */
typedef union JsonOCIVal
{
    JsonDateTime    dt_JsonOCIVal;
    JsonDayInterval dayInv_JsonOCIVal;
    JsonYearInterval yrInv_JsonOCIVal;
    ub1             num_JsonOCIVal[22];
} JsonOCIVal;

/* Field name key */
struct jznDomObjFieldName
{
    oratext       *fnm_jznDomObjFieldName;
    ub4            len_jznDomObjFieldName;
    jznoct_hashidt hashid_jznDomObjFieldName;
    ub4            id_jznDomObjFieldName;
    ub2            ub2hash_jznDomObjFieldName;
    ub1            flag_jznDomObjFieldName;
    ub1            ub1hash_jznDomObjFieldName;
    ub2            idInSetOSON_jznoctFieldName;
    ub2            pos_jznoctFieldName;
    jzndom_fnmOff  nmoff_jznoctFieldName;
};

/* Name/value pair used by field enumeration */
struct jznDomNameValuePair
{
    jznDomObjFieldName name_jznDomNameValuePair;
    jznDomNode        *node_jznDomNameValuePair;
};

/* DOM method table - the real Oracle jznDomAPI structure is a superset; the
   first members have the same layout and can be reached through the returned
   method pointer. */
typedef struct jznDomAPI
{
    void *(*jznDomLoadFromInputEventSrc)(jznDomDoc *jdoc, void *evtsrc);
    void *(*jznDomLoadFromInputOSON)(jznDomDoc *jdoc, void *octbsrc);

    jznnodetype (*jznDomGetNodeType)(jznDomDoc *jdoc, jznDomNode *node);
    sword       (*jznDomGetScalarInfo)(jznDomDoc *jdoc, jznDomScalar *nd,
                                       jznScalarVal *val);
    jznDomNode *(*jznDomGetRootNode)(jznDomDoc *jdoc);

    ub4         (*jznDomGetNumObjField)(jznDomDoc *jdoc, jznDomObject *obj);
        jznDomNode *(*jznDomGetFieldVal)(jznDomDoc *jdoc, jznDomObject *obj, jznDomObjFieldName *nmkey);


    jznDomNode *(*jznDomGetFieldByName)(jznDomDoc *jdoc, jznDomObject *obj,
                                        oratext *nm, ub2 nmlen);
        void (*jznDomGetAllFieldNamesAndVals)(jznDomDoc *jdoc, jznDomObject *obj, jznDomNameValuePair nvps[] /* OUT */);


    ub4         (*jznDomGetFieldNamesAndValsBatch)(jznDomDoc *jdoc,
                                                   jznDomObject *obj,
                                                   ub4 startPos, ub4 fetchSz,
                                                   jznDomNameValuePair nvps[]);

    ub4         (*jznDomGetArraySize)(jznDomDoc *jdoc, jznDomArray *ary);
    jznDomNode *(*jznDomGetArrayElem)(jznDomDoc *jdoc,
                                      jznDomArray *ary, ub4 index);
        ub4 (*jznDomGetArrayElemBatch)(jznDomDoc *jdoc, jznDomArray *ary, ub4 startPos, ub4 fetchSz, jznDomNode *ndary[] /* OUT */);



    sword       (*jznDomSetRootNode)(jznDomDoc *jdoc, jznDomNode *root);
    sword       (*jznDomPutFieldValue)(jznDomDoc *jdoc, jznDomObject *obj,
                                       oratext *key, ub2 keylen,
                                       jznDomNode *val);
    boolean     (*jznDomPutItem)(jznDomDoc *jdoc, jznDomArray *arr,
                                 jznDomNode *node, ub4 pos);
    boolean     (*jznDomAppendItem)(jznDomDoc *jdoc,
                                   jznDomArray *arr, jznDomNode *node);
        boolean (*jznDomReplaceItem)(jznDomDoc *jdoc, jznDomArray *arr, jznDomNode *node, ub4 pos);


    boolean     (*jznDomDeleteField)(jznDomDoc *jdoc, jznDomObject *obj,
                                     jznDomObjFieldName *nmkey);
    jznDomNode *(*jznDomUnlinkField)(jznDomDoc *jdoc, jznDomObject *obj,
                                     jznDomObjFieldName *nmkey);
    ub4         (*jznDomRenameField)(jznDomDoc *jdoc, jznDomObject *obj,
                                     oratext *oldName, ub2 oldNameLen,
                                     oratext *newName, ub2 newNameLen);
    boolean     (*jznDomDeleteItem)(jznDomDoc *jdoc, jznDomArray *arr, ub4 idx);
    jznDomNode *(*jznDomUnlinkItem)(jznDomDoc *jdoc, jznDomArray *arr, ub4 idx);
    ub4         (*jznDomDeleteItemBatch)(jznDomDoc *jdoc, jznDomArray *arr,
                                         ub4 start, ub4 deleteSz);

    jznDomObject *(*jznDomNewObject)(jznDomDoc *jdoc, ub4 sz);
    jznDomArray  *(*jznDomNewArray)(jznDomDoc *jdoc, ub4 sz);
    jznDomScalar *(*jznDomNewScalar)(jznDomDoc *jdoc, jznScalarVal *val);
    void          (*jznDomDocReset)(jznDomDoc *jdoc);
    void          (*jznDomDocFree)(jznDomDoc *jdoc);
        jznEventSource *(*jznDomGetOutputEventSrc)(jznDomDoc *jdoc);


    boolean       (*jznDomEquals)(jznDomDoc *jdoc1, jznDomNode *nd1,
                                  jznDomDoc *jdoc2, jznDomNode *nd2);
    jznDomNode   *(*jznDomCopy)(jznDomDoc *srcdoc, jznDomNode *srcnode,
                                jznDomDoc *destdoc);
        jsonerr (*jznDomValidFid)(jznDomDoc *jdoc, jznDomObjFieldName *fnms, ub2 fnmsn);


        jsonerr (*jznDomStoreField)(jznDomDoc *jdoc, oratext *fname, ub4 fnlen, jznDomObjFieldName *name /* OUT */);


        jsonerr (*jznDomPrintNode)(jznDomDoc *jdoc, jznDomNode *node, jznEventWriter *writer);


        void (*jznDomNodeVisitor)(jznDomDoc *jdoc, jznDomNode *node, JsonDomVisitorFunc func, void *ctx);


    jznDomScalar *(*jznDomNewScalarVal)(jznDomDoc *jdoc, jznvaltype typ, ...);
    boolean       (*jznDomDeleteFieldByName)(jznDomDoc *jdoc, jznDomObject *obj,
                                             oratext *name, ub2 namelen);
        jznDomNode *(*jznDomUnlinkFieldByName)(jznDomDoc *jdoc, jznDomObject *obj, oratext *name, ub2 namelen);


    void          (*jznDomFreeNode)(jznDomDoc *jdoc, jznDomNode *node);
    sword         (*jznDomGetScalarInfoOci)(jznDomDoc *jdoc, jznDomScalar *nd,
                                            jznScalarVal *val,
                                            JsonOCIVal *aux);
    oratext      *(*jznDomGetError)(jznDomDoc *jdoc,
                                    sword *err, boolean clear);
        jznDomNode *(*jznDomReplaceRootNode)(jznDomDoc *jdoc, jznDomNode *root);


        jznDomNode *(*jznDomImportFromEventSrc)(jznDomDoc *jdoc, jznEventSource *evtsrc, boolean readOneItem);


    jznDomNode   *(*jznDomGetParent)(jznDomDoc *jdoc, jznDomNode *node);
        jsonerr (*jznDomSetNodeProperties)(jznDomDoc *jdoc, jznDomNode *node, ...);


        jznDomNode *(*jznDomLoad)(jznDomDoc *jdoc, jsonerr *jerr, ...);


        jsonerr (*jznDomSerialize)(jznDomDoc *jdoc, ...);


} jznDomAPI;

/* DOM document container */
struct jznDomDoc
{
    jznDomAPI *mthds_jznDomDoc;
    void      *xctx_jznDomDoc;
    sword      errcode_jznDomDoc;
    ub4        modcount_jznDomDoc;
};

/* Map public names used in OCILIB to vendored jzn* names */
#define JsonDomDoc  jznDomDoc
#define JsonDomNode jznDomNode

#define JSONDOM_H

/* Convenience DOM access macros */
#define JsonDomGetNodeType(jdoc, node) \
    ((jdoc)->mthds_jznDomDoc->jznDomGetNodeType((jdoc), (node)))

#define JsonDomGetRoot(jdoc) \
    ((jdoc)->mthds_jznDomDoc->jznDomGetRootNode((jdoc)))

#define JsonDomGetScalarInfo(jdoc, nd, val) \
    ((jdoc)->mthds_jznDomDoc->jznDomGetScalarInfo((jdoc), (nd), (val)))

#define JsonDomGetScalarInfoOci(jdoc, nd, val, aux) \
    ((jdoc)->mthds_jznDomDoc->jznDomGetScalarInfoOci((jdoc), (nd), (val), (aux)))

#define JsonDomGetNumFields(jdoc, obj) \
    ((jdoc)->mthds_jznDomDoc->jznDomGetNumObjField((jdoc), (obj)))

#define JsonDomGetFieldByName(jdoc, obj, fname, fnlen) \
    ((jdoc)->mthds_jznDomDoc->jznDomGetFieldByName((jdoc),(obj),(fname),(fnlen)))

#define JsonDomGetFields(jdoc, obj, startPos, fetchSz, nvps) \
    ((jdoc)->mthds_jznDomDoc->jznDomGetFieldNamesAndValsBatch((jdoc),(obj),\
                                                (startPos),(fetchSz),(nvps)))

#define JsonDomGetArraySize(jdoc, ary) \
    ((jdoc)->mthds_jznDomDoc->jznDomGetArraySize((jdoc), (ary)))

#define JsonDomGetArrayElement(jdoc, ary, index) \
    ((jdoc)->mthds_jznDomDoc->jznDomGetArrayElem((jdoc), (ary), (index)))

#define JsonDomNewObject(jdoc, sz) \
    ((jdoc)->mthds_jznDomDoc->jznDomNewObject((jdoc), (sz)))

#define JsonDomNewArray(jdoc, sz) \
    ((jdoc)->mthds_jznDomDoc->jznDomNewArray((jdoc), (sz)))

#define JsonDomNewScalar(jdoc, val) \
    ((jdoc)->mthds_jznDomDoc->jznDomNewScalar((jdoc), (val)))

#define JsonDomNewScalarVal(jdoc, vtyp, ...) \
    ((jdoc)->mthds_jznDomDoc->jznDomNewScalarVal((jdoc), (vtyp), __VA_ARGS__))

#define JsonDomFreeNode(jdoc, nd) \
    ((jdoc)->mthds_jznDomDoc->jznDomFreeNode((jdoc), (nd)))

#define JsonDomPutFieldValue(jdoc, obj, key, keylen, val) \
    ((jdoc)->mthds_jznDomDoc->jznDomPutFieldValue((jdoc),(obj),(key),(keylen),(val)))

#define JsonDomSetRoot(jdoc, root) \
    ((jdoc)->mthds_jznDomDoc->jznDomSetRootNode((jdoc), (root)))

#define JsonDomPutItem(jdoc, ary, index, val) \
    ((jdoc)->mthds_jznDomDoc->jznDomPutItem((jdoc),(ary),(val),(index)))

#define JsonDomAppendItem(jdoc, ary, val) \
    ((jdoc)->mthds_jznDomDoc->jznDomAppendItem((jdoc),(ary),(val)))

#define JsonDomDeleteField(jdoc, obj, nmkey) \
    ((jdoc)->mthds_jznDomDoc->jznDomDeleteField((jdoc),(obj),(nmkey)))

#define JsonDomDeleteFieldByName(jdoc, obj, fname, fnlen) \
    ((jdoc)->mthds_jznDomDoc->jznDomDeleteFieldByName((jdoc),(obj),(fname),(fnlen)))

#define JsonDomDeleteItem(jdoc, ary, index) \
    ((jdoc)->mthds_jznDomDoc->jznDomDeleteItem((jdoc),(ary),(index)))

#define JsonDomDeleteItems(jdoc, ary, start, cnt) \
    ((jdoc)->mthds_jznDomDoc->jznDomDeleteItemBatch((jdoc),(ary),(start),(cnt)))

#define JsonDomRenameField(jdoc, obj, oldName, oldNameLen, newName, newNameLen)     ((jdoc)->mthds_jznDomDoc->jznDomRenameField((jdoc),(obj),(oldName),(oldNameLen),(newName),(newNameLen)))

#define JsonDomEquals(jdoc1, nd1, jdoc2, nd2)     ((jdoc1)->mthds_jznDomDoc->jznDomEquals((jdoc1),(nd1),(jdoc2),(nd2)))

#define JsonDomCopy(srcdoc, srcnode, destdoc)     ((srcdoc)->mthds_jznDomDoc->jznDomCopy((srcdoc),(srcnode),(destdoc)))

#define JsonDomInsertElements(jdoc, ary, pos, node) \
    ((jdoc)->mthds_jznDomDoc->jznDomPutItem((jdoc),(ary),(node),(pos)))

#define JsonDomGetError(jdoc, jerr, clear) \
    ((jdoc)->mthds_jznDomDoc->jznDomGetError((jdoc), (jerr), (clear)))

#define JsonDomGetParent(jdoc, node) \
    ((jdoc)->mthds_jznDomDoc->jznDomGetParent((jdoc),(node)))

#define JsonDomGetFieldPos(jdoc, obj, fname, fnlen) \
    ((jdoc)->mthds_jznDomDoc->jznDomGetFieldByName((jdoc),(obj),(fname),(fnlen)))

#define JsonDomDocFree(jdoc) \
    ((jdoc)->mthds_jznDomDoc->jznDomDocFree((jdoc)))

#endif /* !JsonDomGetFields */

#endif /* OCILIB_OCI_JZNDOM_H_INCLUDED */
