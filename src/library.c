/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2018 Vincent ROGIER <vince.rogier@ocilib.net>
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
 *                             INTERNAL VARIABLES
 * ********************************************************************************************* */

OCI_Library OCILib;

const char * EnvironmentVarNames[OCI_VARS_COUNT] =
{
    VAR_OCILIB_WORKAROUND_UTF16_COLUMN_NAME
};


const OCI_SQLCmdInfo SQLCmds[OCI_SQLCMD_COUNT] =
{
    {OCI_SFC_CREATE_TABLE,             OTEXT("CREATE TABLE")                                         },
    {OCI_SFC_SET_ROLE,                 OTEXT("SET ROLE")                                             },
    {OCI_SFC_INSERT,                   OTEXT("INSERT")                                               },
    {OCI_SFC_SELECT,                   OTEXT("SELECT")                                               },
    {OCI_SFC_UPDATE,                   OTEXT("UPDATE")                                               },
    {OCI_SFC_DROP_ROLE,                OTEXT("DROP ROLE")                                            },
    {OCI_SFC_DROP_VIEW,                OTEXT("DROP VIEW")                                            },
    {OCI_SFC_DROP_TABLE,               OTEXT("DROP TABLE")                                           },
    {OCI_SFC_DELETE,                   OTEXT("DELETE")                                               },
    {OCI_SFC_CREATE_VIEW,              OTEXT("CREATE VIEW")                                          },
    {OCI_SFC_DROP_USER,                OTEXT("DROP USER")                                            },
    {OCI_SFC_CREATE_ROLE,              OTEXT("CREATE ROLE")                                          },
    {OCI_SFC_CREATE_SEQUENCE,          OTEXT("CREATE SEQUENCE")                                      },
    {OCI_SFC_ALTER_SEQUENCE,           OTEXT("ALTER SEQUENCE")                                       },
    {OCI_SFC_DROP_SEQUENCE,            OTEXT("DROP SEQUENCE")                                        },
    {OCI_SFC_CREATE_SCHEMA,            OTEXT("CREATE SCHEMA")                                        },
    {OCI_SFC_CREATE_CLUSTER,           OTEXT("CREATE CLUSTER")                                       },
    {OCI_SFC_CREATE_USER,              OTEXT("CREATE USER")                                          },
    {OCI_SFC_CREATE_INDEX,             OTEXT("CREATE INDEX")                                         },
    {OCI_SFC_DROP_INDEX,               OTEXT("DROP INDEX")                                           },
    {OCI_SFC_DROP_CLUSTER,             OTEXT("DROP CLUSTER")                                         },
    {OCI_SFC_VALIDATE_INDEX,           OTEXT("VALIDATE INDEX")                                       },
    {OCI_SFC_CREATE_PROCEDURE,         OTEXT("CREATE PROCEDURE")                                     },
    {OCI_SFC_ALTER_PROCEDURE,          OTEXT("ALTER PROCEDURE")                                      },
    {OCI_SFC_ALTER_TABLE,              OTEXT("ALTER TABLE")                                          },
    {OCI_SFC_EXPLAIN,                  OTEXT("EXPLAIN")                                              },
    {OCI_SFC_GRANT,                    OTEXT("GRANT")                                                },
    {OCI_SFC_REVOKE,                   OTEXT("REVOKE")                                               },
    {OCI_SFC_CREATE_SYNONYM,           OTEXT("CREATE SYNONYM")                                       },
    {OCI_SFC_DROP_SYNONYM,             OTEXT("DROP SYNONYM")                                         },
    {OCI_SFC_ALTER_SYSTEM_SWITCHLOG,   OTEXT("ALTER SYSTEM SWITCHLOG")                               },
    {OCI_SFC_SET_TRANSACTION,          OTEXT("SET TRANSACTION")                                      },
    {OCI_SFC_PLSQL_EXECUTE,            OTEXT("PL/SQL EXECUTE")                                       },
    {OCI_SFC_LOCK,                     OTEXT("LOCK")                                                 },
    {OCI_SFC_NOOP,                     OTEXT("NOOP")                                                 },
    {OCI_SFC_RENAME,                   OTEXT("RENAME")                                               },
    {OCI_SFC_COMMENT,                  OTEXT("COMMENT")                                              },
    {OCI_SFC_AUDIT,                    OTEXT("AUDIT")                                                },
    {OCI_SFC_NO_AUDIT,                 OTEXT("NO AUDIT")                                             },
    {OCI_SFC_ALTER_INDEX,              OTEXT("ALTER INDEX")                                          },
    {OCI_SFC_CREATE_EXTERNAL_DATABASE, OTEXT("CREATE EXTERNAL DATABASE")                             },
    {OCI_SFC_DROP_EXTERNALDATABASE,    OTEXT("DROP EXTERNALDATABASE")                                },
    {OCI_SFC_CREATE_DATABASE,          OTEXT("CREATE DATABASE")                                      },
    {OCI_SFC_ALTER_DATABASE,           OTEXT("ALTER DATABASE")                                       },
    {OCI_SFC_CREATE_ROLLBACK_SEGMENT,  OTEXT("CREATE ROLLBACK SEGMENT")                              },
    {OCI_SFC_ALTER_ROLLBACK_SEGMENT,   OTEXT("ALTER ROLLBACK SEGMENT")                               },
    {OCI_SFC_DROP_ROLLBACK_SEGMENT,    OTEXT("DROP ROLLBACK SEGMENT")                                },
    {OCI_SFC_CREATE_TABLESPACE,        OTEXT("CREATE TABLESPACE")                                    },
    {OCI_SFC_ALTER_TABLESPACE,         OTEXT("ALTER TABLESPACE")                                     },
    {OCI_SFC_DROP_TABLESPACE,          OTEXT("DROP TABLESPACE")                                      },
    {OCI_SFC_ALTER_SESSION,            OTEXT("ALTER SESSION")                                        },
    {OCI_SFC_ALTER_USER,               OTEXT("ALTER USER")                                           },
    {OCI_SFC_COMMIT_WORK,              OTEXT("COMMIT (WORK)")                                        },
    {OCI_SFC_ROLLBACK,                 OTEXT("ROLLBACK")                                             },
    {OCI_SFC_SAVEPOINT,                OTEXT("SAVEPOINT")                                            },
    {OCI_SFC_CREATE_CONTROL_FILE,      OTEXT("CREATE CONTROL FILE")                                  },
    {OCI_SFC_ALTER_TRACING,            OTEXT("ALTER TRACING")                                        },
    {OCI_SFC_CREATE_TRIGGER,           OTEXT("CREATE TRIGGER")                                       },
    {OCI_SFC_ALTER_TRIGGER,            OTEXT("ALTER TRIGGER")                                        },
    {OCI_SFC_DROP_TRIGGER,             OTEXT("DROP TRIGGER")                                         },
    {OCI_SFC_ANALYZE_TABLE,            OTEXT("ANALYZE TABLE")                                        },
    {OCI_SFC_ANALYZE_INDEX,            OTEXT("ANALYZE INDEX")                                        },
    {OCI_SFC_ANALYZE_CLUSTER,          OTEXT("ANALYZE CLUSTER")                                      },
    {OCI_SFC_CREATE_PROFILE,           OTEXT("CREATE PROFILE")                                       },
    {OCI_SFC_DROP_PROFILE,             OTEXT("DROP PROFILE")                                         },
    {OCI_SFC_ALTER_PROFILE,            OTEXT("ALTER PROFILE")                                        },
    {OCI_SFC_DROP_PROCEDURE,           OTEXT("DROP PROCEDURE")                                       },
    {OCI_SFC_ALTER_RESOURCE_COST,      OTEXT("ALTER RESOURCE COST")                                  },
    {OCI_SFC_CREATE_SNAPSHOT_LOG,      OTEXT("CREATE SNAPSHOT LOG")                                  },
    {OCI_SFC_ALTER_SNAPSHOT_LOG,       OTEXT("ALTER SNAPSHOT LOG")                                   },
    {OCI_SFC_DROP_SNAPSHOT_LOG,        OTEXT("DROP SNAPSHOT LOG")                                    },
    {OCI_SFC_DROP_SUMMARY,             OTEXT("DROP SUMMARY")                                         },
    {OCI_SFC_CREATE_SNAPSHOT,          OTEXT("CREATE SNAPSHOT")                                      },
    {OCI_SFC_ALTER_SNAPSHOT,           OTEXT("ALTER SNAPSHOT")                                       },
    {OCI_SFC_DROP_SNAPSHOT,            OTEXT("DROP SNAPSHOT")                                        },
    {OCI_SFC_CREATE_TYPE,              OTEXT("CREATE TYPE")                                          },
    {OCI_SFC_DROP_TYPE,                OTEXT("DROP TYPE")                                            },
    {OCI_SFC_ALTER_ROLE,               OTEXT("ALTER ROLE")                                           },
    {OCI_SFC_ALTER_TYPE,               OTEXT("ALTER TYPE")                                           },
    {OCI_SFC_CREATE_TYPE_BODY,         OTEXT("CREATE TYPE BODY")                                     },
    {OCI_SFC_ALTER_TYPE_BODY,          OTEXT("ALTER TYPE BODY")                                      },
    {OCI_SFC_DROP_TYPE_BODY,           OTEXT("DROP TYPE BODY")                                       },
    {OCI_SFC_DROP_LIBRARY,             OTEXT("DROP LIBRARY")                                         },
    {OCI_SFC_TRUNCATE_TABLE,           OTEXT("TRUNCATE TABLE")                                       },
    {OCI_SFC_TRUNCATE_CLUSTER,         OTEXT("TRUNCATE CLUSTER")                                     },
    {OCI_SFC_CREATE_BITMAPFILE,        OTEXT("CREATE BITMAPFILE")                                    },
    {OCI_SFC_ALTER_VIEW,               OTEXT("ALTER VIEW")                                           },
    {OCI_SFC_DROP_BITMAPFILE,          OTEXT("DROP BITMAPFILE")                                      },
    {OCI_SFC_SET_CONSTRAINTS,          OTEXT("SET CONSTRAINTS")                                      },
    {OCI_SFC_CREATE_FUNCTION,          OTEXT("CREATE FUNCTION")                                      },
    {OCI_SFC_ALTER_FUNCTION,           OTEXT("ALTER FUNCTION")                                       },
    {OCI_SFC_DROP_FUNCTION,            OTEXT("DROP FUNCTION")                                        },
    {OCI_SFC_CREATE_PACKAGE,           OTEXT("CREATE PACKAGE")                                       },
    {OCI_SFC_ALTER_PACKAGE,            OTEXT("ALTER PACKAGE")                                        },
    {OCI_SFC_DROP_PACKAGE,             OTEXT("DROP PACKAGE")                                         },
    {OCI_SFC_CREATE_PACKAGE_BODY,      OTEXT("CREATE PACKAGE BODY")                                  },
    {OCI_SFC_ALTER_PACKAGE_BODY,       OTEXT("ALTER PACKAGE BODY")                                   },
    {OCI_SFC_DROP_PACKAGE_BODY,        OTEXT("DROP PACKAGE BODY")                                    },
    {OCI_SFC_CREATE_DIRECTORY,         OTEXT("CREATE DIRECTORY")                                     },
    {OCI_SFC_DROP_DIRECTORY,           OTEXT("DROP DIRECTORY")                                       },
    {OCI_SFC_CREATE_LIBRARY,           OTEXT("CREATE LIBRARY")                                       },
    {OCI_SFC_CREATE_JAVA,              OTEXT("CREATE JAVA")                                          },
    {OCI_SFC_ALTER_JAVA,               OTEXT("ALTER JAVA")                                           },
    {OCI_SFC_DROP_JAVA,                OTEXT("DROP JAVA")                                            },
    {OCI_SFC_CREATE_OPERATOR,          OTEXT("CREATE OPERATOR")                                      },
    {OCI_SFC_CREATE_INDEXTYPE,         OTEXT("CREATE INDEXTYPE")                                     },
    {OCI_SFC_DROP_INDEXTYPE,           OTEXT("DROP INDEXTYPE")                                       },
    {OCI_SFC_ALTER_INDEXTYPE,          OTEXT("ALTER INDEXTYPE")                                      },
    {OCI_SFC_DROP_OPERATOR,            OTEXT("DROP OPERATOR")                                        },
    {OCI_SFC_ASSOCIATE_STATISTICS,     OTEXT("ASSOCIATE STATISTICS")                                 },
    {OCI_SFC_DISASSOCIATE_STATISTICS,  OTEXT("DISASSOCIATE STATISTICS")                              },
    {OCI_SFC_CALL_METHOD,              OTEXT("CALL METHOD")                                          },
    {OCI_SFC_CREATE_SUMMARY,           OTEXT("CREATE SUMMARY")                                       },
    {OCI_SFC_ALTER_SUMMARY,            OTEXT("ALTER SUMMARY")                                        },
    {OCI_SFC_CREATE_DIMENSION,         OTEXT("CREATE DIMENSION")                                     },
    {OCI_SFC_ALTER_DIMENSION,          OTEXT("ALTER DIMENSION")                                      },
    {OCI_SFC_DROP_DIMENSION,           OTEXT("DROP DIMENSION")                                       },
    {OCI_SFC_CREATE_CONTEXT,           OTEXT("CREATE CONTEXT")                                       },
    {OCI_SFC_DROP_CONTEXT,             OTEXT("DROP CONTEXT")                                         },
    {OCI_SFC_ALTER_OUTLINE,            OTEXT("ALTER OUTLINE")                                        },
    {OCI_SFC_CREATE_OUTLINE,           OTEXT("CREATE OUTLINE")                                       },
    {OCI_SFC_DROP_OUTLINE,             OTEXT("DROP OUTLINE")                                         },
    {OCI_SFC_UPDATE_INDEXES,           OTEXT("UPDATE INDEXES")                                       },
    {OCI_SFC_ALTER_OPERATOR,           OTEXT("ALTER OPERATOR")                                       }
};

static const unsigned int FormatTypeValues[] =
{
    OCI_FMT_DATE,
    OCI_FMT_TIMESTAMP,
    OCI_FMT_NUMERIC,
    OCI_FMT_BINARY_DOUBLE,
    OCI_FMT_BINARY_FLOAT,
    OCI_FMT_TIMESTAMP_TZ
};

static const otext * FormatDefaultValues[OCI_FMT_COUNT] =
{
    OCI_STRING_FORMAT_DATE,
    OCI_STRING_FORMAT_TIMESTAMP,
    OCI_STRING_FORMAT_NUM,
    OCI_STRING_FORMAT_NUM_BDOUBLE,
    OCI_STRING_FORMAT_NUM_BFLOAT,
    OCI_STRING_FORMAT_TIMESTAMP_TZ
};

#ifdef OCI_IMPORT_RUNTIME

/* OCI function pointers */

OCIENVCREATE                 OCIEnvCreate                 = NULL;
OCISERVERATTACH              OCIServerAttach              = NULL;
OCISERVERDETACH              OCIServerDetach              = NULL;
OCIHANDLEALLOC               OCIHandleAlloc               = NULL;
OCIHANDLEFREE                OCIHandleFree                = NULL;
OCIDESCRIPTORALLOC           OCIDescriptorAlloc           = NULL;
OCIDESCRIPTORFREE            OCIDescriptorFree            = NULL;
OCISESSIONBEGIN              OCISessionBegin              = NULL;
OCISESSIONEND                OCISessionEnd                = NULL;
OCIPASSWORDCHANGE            OCIPasswordChange            = NULL;
OCIBINDBYPOS                 OCIBindByPos                 = NULL;
OCIBINDBYNAME                OCIBindByName                = NULL;
OCIBINDDYNAMIC               OCIBindDynamic               = NULL;
OCIBINDOBJECT                OCIBindObject                = NULL;
OCIDEFINEBYPOS               OCIDefineByPos               = NULL;
OCIDEFINEOBJECT              OCIDefineObject              = NULL;
OCISTMTPREPARE               OCIStmtPrepare               = NULL;
OCISTMTEXECUTE               OCIStmtExecute               = NULL;
OCISTMTFETCH                 OCIStmtFetch                 = NULL;
OCISTMTFETCH2                OCIStmtFetch2                = NULL;
OCISTMTGETPIECEINFO          OCIStmtGetPieceInfo          = NULL;
OCISTMTSETPIECEINFO          OCIStmtSetPieceInfo          = NULL;
OCIPARAMGET                  OCIParamGet                  = NULL;
OCIPARAMSET                  OCIParamSet                  = NULL;
OCITRANSSTART                OCITransStart                = NULL;
OCITRANSDETACH               OCITransDetach               = NULL;
OCITRANSPREPARE              OCITransPrepare              = NULL;
OCITRANSFORGET               OCITransForget               = NULL;
OCITRANSCOMMIT               OCITransCommit               = NULL;
OCITRANSROLLBACK             OCITransRollback             = NULL;
OCIERRORGET                  OCIErrorGet                  = NULL;
OCILOBCREATETEMPORARY        OCILobCreateTemporary        = NULL;
OCILOBFREETEMPORARY          OCILobFreeTemporary          = NULL;
OCILOBISTEMPORARY            OCILobIsTemporary            = NULL;
OCILOBAPPEND                 OCILobAppend                 = NULL;
OCILOBCOPY                   OCILobCopy                   = NULL;
OCILOBGETLENGTH              OCILobGetLength              = NULL;
OCILOBGETCHUNKSIZE           OCILobGetChunkSize           = NULL;
OCILOBREAD                   OCILobRead                   = NULL;
OCILOBWRITE                  OCILobWrite                  = NULL;
OCILOBTRIM                   OCILobTrim                   = NULL;
OCILOBERASE                  OCILobErase                  = NULL;
OCILOBOPEN                   OCILobOpen                   = NULL;
OCILOBCLOSE                  OCILobClose                  = NULL;
OCILOBLOCATORASSIGN          OCILobLocatorAssign          = NULL;
OCILOBASSIGN                 OCILobAssign                 = NULL;
OCILOBISEQUAL                OCILobIsEqual                = NULL;
OCILOBFLUSHBUFFER            OCILobFlushBuffer            = NULL;
OCILOBENABLEBUFFERING        OCILobEnableBuffering        = NULL;
OCILOBDISABLEBUFFERING       OCILobDisableBuffering       = NULL;
OCILOBGETSTORAGELIMIT        OCILobGetStorageLimit        = NULL;
OCILOBFILEOPEN               OCILobFileOpen               = NULL;
OCILOBFILECLOSE              OCILobFileClose              = NULL;
OCILOBFILECLOSEALL           OCILobFileCloseAll           = NULL;
OCILOBFILEISOPEN             OCILobFileIsOpen             = NULL;
OCILOBFILEEXISTS             OCILobFileExists             = NULL;
OCILOBFIELGETNAME            OCILobFileGetName            = NULL;
OCILOBFILESETNAME            OCILobFileSetName            = NULL;
OCILOBLOADFROMFILE           OCILobLoadFromFile           = NULL;
OCILOBWRITEAPPEND            OCILobWriteAppend            = NULL;
OCISERVERVERSION             OCIServerVersion             = NULL;
OCIBREAK                     OCIBreak                     = NULL;
OCIATTRGET                   OCIAttrGet                   = NULL;
OCIATTRSET                   OCIAttrSet                   = NULL;
OCIDATEASSIGN                OCIDateAssign                = NULL;
OCIDATETOTEXT                OCIDateToText                = NULL;
OCIDATEFROMTEXT              OCIDateFromText              = NULL;
OCIDATECOMPARE               OCIDateCompare               = NULL;
OCIDATEADDMONTHS             OCIDateAddMonths             = NULL;
OCIDATEADDDAYS               OCIDateAddDays               = NULL;
OCIDATELASTDAY               OCIDateLastDay               = NULL;
OCIDATEDAYSBETWEEN           OCIDateDaysBetween           = NULL;
OCIDATEZONETOZONE            OCIDateZoneToZone            = NULL;
OCIDATENEXTDAY               OCIDateNextDay               = NULL;
OCIDATECHECK                 OCIDateCheck                 = NULL;
OCIDATESYSDATE               OCIDateSysDate               = NULL;
OCIDESCRIBEANY               OCIDescribeAny               = NULL;
OCIINTERVALASSIGN            OCIIntervalAssign            = NULL;
OCIINTERVALCHECK             OCIIntervalCheck             = NULL;
OCIINTERVALCOMPARE           OCIIntervalCompare           = NULL;
OCIINTERVALFROMTEXT          OCIIntervalFromText          = NULL;
OCIINTERVALTOTEXT            OCIIntervalToText            = NULL;
OCIINTERVALFROMTZ            OCIIntervalFromTZ            = NULL;
OCIINTERVALGETDAYSECOND      OCIIntervalGetDaySecond      = NULL;
OCIINTERVALGETYEARMONTH      OCIIntervalGetYearMonth      = NULL;
OCIINTERVALSETDAYSECOND      OCIIntervalSetDaySecond      = NULL;
OCIINTERVALSETYEARMONTH      OCIIntervalSetYearMonth      = NULL;
OCIINTERVALSUBTRACT          OCIIntervalSubtract          = NULL;
OCIINTERVALADD               OCIIntervalAdd               = NULL;
OCIDATETIMEASSIGN            OCIDateTimeAssign            = NULL;
OCIDATETIMECHECK             OCIDateTimeCheck             = NULL;
OCIDATETIMECOMPARE           OCIDateTimeCompare           = NULL;
OCIDATETIMECONSTRUCT         OCIDateTimeConstruct         = NULL;
OCIDATETIMECONVERT           OCIDateTimeConvert           = NULL;
OCIDATETIMEFROMARRAY         OCIDateTimeFromArray         = NULL;
OCIDATETIMETOARRAY           OCIDateTimeToArray           = NULL;
OCIDATETIMEFROMTEXT          OCIDateTimeFromText          = NULL;
OCIDATETIMETOTEXT            OCIDateTimeToText            = NULL;
OCIDATETIMEGETDATE           OCIDateTimeGetDate           = NULL;
OCIDATETIMEGETTIME           OCIDateTimeGetTime           = NULL;
OCIDATETIMEGETTIMEZONENAME   OCIDateTimeGetTimeZoneName   = NULL;
OCIDATETIMEGETTIMEZONEOFFSET OCIDateTimeGetTimeZoneOffset = NULL;
OCIDATETIMEINTERVALADD       OCIDateTimeIntervalAdd       = NULL;
OCIDATETIMEINTERVALSUB       OCIDateTimeIntervalSub       = NULL;
OCIDATETIMESUBTRACT          OCIDateTimeSubtract          = NULL;
OCIDATETIMESYSTIMESTAMP      OCIDateTimeSysTimeStamp      = NULL;
OCIARRAYDESCRIPTORALLOC      OCIArrayDescriptorAlloc      = NULL;
OCIARRAYDESCRIPTORFREE       OCIArrayDescriptorFree       = NULL;
OCICLIENTVERSION             OCIClientVersion             = NULL;
OCITYPEBYREF                 OCITypeByRef                 = NULL;
OCINUMBERTOINT               OCINumberToInt               = NULL;
OCINUMBERFROMINT             OCINumberFromInt             = NULL;
OCINUMBERTOREAL              OCINumberToReal              = NULL;
OCINUMBERFROMREAL            OCINumberFromReal            = NULL;
OCINUMBERTOTEXT              OCINumberToText              = NULL;
OCINUMBERFROMTEXT            OCINumberFromText            = NULL;
OCINUMBERASSIGN              OCINumberAssign              = NULL;
OCINUMBERADD                 OCINumberAdd                 = NULL;
OCINUMBERSUB                 OCINumberSub                 = NULL;
OCINUMBERMUL                 OCINumberMul                 = NULL;
OCINUMBERDIV                 OCINumberDiv                 = NULL;
OCINUMBERCMP                 OCINumberCmp                 = NULL;
OCISTRINGPTR                 OCIStringPtr                 = NULL;
OCISTRINGSIZE                OCIStringSize                = NULL;
OCISTRINGASSIGNTEXT          OCIStringAssignText          = NULL;
OCISTRINGRESIZE              OCIStringResize              = NULL;
OCIRAWPTR                    OCIRawPtr                    = NULL;
OCIRAWASSIGNBYTES            OCIRawAssignBytes            = NULL;
OCIRAWRESIZE                 OCIRawResize                 = NULL;
OCIRAWALLOCSIZE              OCIRawAllocSize              = NULL;
OCIRAWSIZE                   OCIRawSize                   = NULL;
OCIOBJECTNEW                 OCIObjectNew                 = NULL;
OCIOBJECTFREE                OCIObjectFree                = NULL;
OCIOBJECTSETATTR             OCIObjectSetAttr             = NULL;
OCIOBJECTGETATTR             OCIObjectGetAttr             = NULL;
OCIOBJECTPIN                 OCIObjectPin                 = NULL;
OCIOBJECTUNPIN               OCIObjectUnpin               = NULL;
OCIOBJECTCOPY                OCIObjectCopy                = NULL;
OCIOBJECTGETOBJECTREF        OCIObjectGetObjectRef        = NULL;
OCIOBJECTGETTYPEREF          OCIObjectGetTypeRef          = NULL;
OCIOBJECTGETPROPERTY         OCIObjectGetProperty         = NULL;
OCIOBJECTGETIND              OCIObjectGetInd              = NULL;
OCIREFASSIGN                 OCIRefAssign                 = NULL;
OCIREFISNULL                 OCIRefIsNull                 = NULL;
OCIREFCLEAR                  OCIRefClear                  = NULL;
OCIREFTOHEX                  OCIRefToHex                  = NULL;
OCIREFHEXSIZE                OCIRefHexSize                = NULL;
OCITHREADPROCESSINIT         OCIThreadProcessInit         = NULL;
OCITHREADINIT                OCIThreadInit                = NULL;
OCITHREADTERM                OCIThreadTerm                = NULL;
OCITHREADIDINIT              OCIThreadIdInit              = NULL;
OCITHREADIDDESTROY           OCIThreadIdDestroy           = NULL;
OCITHREADHNDINIT             OCIThreadHndInit             = NULL;
OCITHREADHNDDESTROY          OCIThreadHndDestroy          = NULL;
OCITHREADCREATE              OCIThreadCreate              = NULL;
OCITHREADJOIN                OCIThreadJoin                = NULL;
OCITHREADCLOSE               OCIThreadClose               = NULL;
OCITHREADMUTEXINIT           OCIThreadMutexInit           = NULL;
OCITHREADMUTEXDESTROY        OCIThreadMutexDestroy        = NULL;
OCITHREADMUTEXACQUIRE        OCIThreadMutexAcquire        = NULL;
OCITHREADMUTEXRELEASE        OCIThreadMutexRelease        = NULL;
OCITHREADKEYINIT             OCIThreadKeyInit             = NULL;
OCITHREADKEYDESTROY          OCIThreadKeyDestroy          = NULL;
OCITHREADKEYSET              OCIThreadKeySet              = NULL;
OCITHREADKEYGET              OCIThreadKeyGet              = NULL;
OCICONNECTIONPOOLCREATE      OCIConnectionPoolCreate      = NULL;
OCICONNECTIONPOOLDESTROY     OCIConnectionPoolDestroy     = NULL;
OCISESSIONPOOLCREATE         OCISessionPoolCreate         = NULL;
OCISESSIONPOOLDESTROY        OCISessionPoolDestroy        = NULL;
OCISESSIONGET                OCISessionGet                = NULL;
OCISESSIONRELEASE            OCISessionRelease            = NULL;
OCICOLLSIZE                  OCICollSize                  = NULL;
OCICOLLMAX                   OCICollMax                   = NULL;
OCICOLLGETITEM               OCICollGetElem               = NULL;
OCICOLLASSIGNELEM            OCICollAssignElem            = NULL;
OCICOLLASSIGN                OCICollAssign                = NULL;
OCICOLLAPPEND                OCICollAppend                = NULL;
OCICOLLTRIM                  OCICollTrim                  = NULL;
OCITABLESIZE                 OCITableSize                 = NULL;
OCITABLEDELETE               OCITableDelete               = NULL;
OCIITERCREATE                OCIIterCreate                = NULL;
OCIITERDELETE                OCIIterDelete                = NULL;
OCIITERINIT                  OCIIterInit                  = NULL;
OCIITERNEXT                  OCIIterNext                  = NULL;
OCIITERPREV                  OCIIterPrev                  = NULL;
OCIDIRPATHABORT              OCIDirPathAbort              = NULL;
OCIDIRPATHDATASAVE           OCIDirPathDataSave           = NULL;
OCIDIRPATHFINISH             OCIDirPathFinish             = NULL;
OCIDIRPATHPREPARE            OCIDirPathPrepare            = NULL;
OCIDIRPATHLOADSTREAM         OCIDirPathLoadStream         = NULL;
OCIDIRPATHCOLARRAYENTRYSET   OCIDirPathColArrayEntrySet   = NULL;
OCIDIRPATHCOLARRAYRESET      OCIDirPathColArrayReset      = NULL;
OCIDIRPATHCOLARRAYTOSTREAM   OCIDirPathColArrayToStream   = NULL;
OCIDIRPATHSTREAMRESET        OCIDirPathStreamReset        = NULL;
OCIDIRPATHFLUSHROW           OCIDirPathFlushRow           = NULL;
OCICACHEFREE                 OCICacheFree                 = NULL;
OCIPING                      OCIPing                      = NULL;
OCIDBSTARTUP                 OCIDBStartup                 = NULL;
OCIDBSHUTDOWN                OCIDBShutdown                = NULL;
OCISTMTPREPARE2              OCIStmtPrepare2              = NULL;
OCISTMTRELEASE               OCIStmtRelease               = NULL;
OCISUBSCRIPTIONREGISTER      OCISubscriptionRegister      = NULL;
OCISUBSCRIPTIONUNREGISTER    OCISubscriptionUnRegister    = NULL;
OCIAQENQ                     OCIAQEnq                     = NULL;
OCIAQDEQ                     OCIAQDeq                     = NULL;
OCIAQLISTEN                  OCIAQListen                  = NULL;
XAOSVCCTX                    xaoSvcCtx                    = NULL;
XAOENV                       xaoEnv                       = NULL;
OCILOBGETCONTENTTYPE         OCILobGetContentType         = NULL;
OCISTMTGETNEXTRESULT         OCIStmtGetNextResult         = NULL;

#ifdef ORAXB8_DEFINED

OCILOBCOPY2                  OCILobCopy2                  = NULL;
OCILOBERASE2                 OCILobErase2                 = NULL;
OCILOBGETLENGTH2             OCILobGetLength2             = NULL;
OCILOBLOADFROMFILE2          OCILobLoadFromFile2          = NULL;
OCILOBREAD2                  OCILobRead2                  = NULL;
OCILOBTRIM2                  OCILobTrim2                  = NULL;
OCILOBWRITE2                 OCILobWrite2                 = NULL;
OCILOBWRITEAPPEND2           OCILobWriteAppend2           = NULL;

#endif /* ORAXB8_DEFINED */

#endif /* OCI_IMPORT_RUNTIME */

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
* OCI_ExternalSubTypeToSQLType
* --------------------------------------------------------------------------------------------- */

unsigned int OCI_ExternalSubTypeToSQLType
(
    unsigned int type,
    unsigned int subtype
)
{
    unsigned int res = OCI_UNKNOWN;

    switch (type)
    {
        case OCI_CDT_TIMESTAMP:
        {
            switch (subtype)
            {
                case OCI_TIMESTAMP:
                    res = SQLT_TIMESTAMP;
                    break;
                case OCI_TIMESTAMP_TZ:
                    res = SQLT_TIMESTAMP_TZ;
                    break;
                case OCI_TIMESTAMP_LTZ:
                    res = SQLT_TIMESTAMP_LTZ;
                    break;
            }
            break;
        }
        case OCI_CDT_INTERVAL:
        {
            switch (subtype)
            {
                case OCI_INTERVAL_YM:
                    res = SQLT_INTERVAL_YM;
                    break;
                case OCI_INTERVAL_DS:
                    res = SQLT_INTERVAL_DS;
                    break;
            }
            break;
        }
        case OCI_CDT_LOB:
        {
            switch (subtype)
            {
                case OCI_CLOB:
                case OCI_NCLOB:
                    res = SQLT_CLOB;
                    break;
                case OCI_BLOB:
                    res = SQLT_BLOB;
                    break;
            }
            break;
        }
        case OCI_CDT_FILE:
        {
            switch (subtype)
            {
                case OCI_CFILE:
                    res = SQLT_CFILE;
                    break;
                case OCI_BFILE:
                    res = SQLT_BFILE;
                    break;
            }
            break;
        }
        case OCI_CDT_LONG:
        {
            switch (subtype)
            {
                case OCI_CLONG:
                    res = SQLT_LNG;
                    break;
                case OCI_BLONG:
                    res = SQLT_LBI;
                    break;
            }
            break;
        }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
* OCI_ExternalSubTypeToHandleType
* --------------------------------------------------------------------------------------------- */

unsigned int OCI_ExternalSubTypeToHandleType
(
    unsigned int type,
    unsigned int subtype
)
{
    unsigned int res = OCI_UNKNOWN;

    switch (type)
    {
        case OCI_CDT_TIMESTAMP:
        {
            switch (subtype)
            {
                case OCI_TIMESTAMP:
                    res = OCI_DTYPE_TIMESTAMP;
                    break;
                case OCI_TIMESTAMP_TZ:
                    res = OCI_DTYPE_TIMESTAMP_TZ;
                    break;
                case OCI_TIMESTAMP_LTZ:
                    res = OCI_DTYPE_TIMESTAMP_LTZ;
                    break;
            }
            break;
        }
        case OCI_CDT_INTERVAL:
        {
            switch (subtype)
            {
                case OCI_INTERVAL_YM:
                    res = OCI_DTYPE_INTERVAL_YM;
                    break;
                case OCI_INTERVAL_DS:
                    res = OCI_DTYPE_INTERVAL_DS;
                    break;
            }
            break;
        }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
* OCI_FreeObjectFromType
* --------------------------------------------------------------------------------------------- */

boolean OCI_FreeObjectFromType(void *obj, unsigned int type)
{
    boolean res = FALSE;

    switch (type)
    {
        case OCI_CDT_NUMERIC:
        {
            res = OCI_NumberFree((OCI_Number *)obj);
            break;
        }
        case OCI_CDT_DATETIME:
        {
            res = OCI_DateFree((OCI_Date *)obj);
            break;
        }
        case OCI_CDT_LOB:
        {
            res = OCI_LobFree((OCI_Lob *)obj);
            break;
        }
        case OCI_CDT_FILE:
        {
            res = OCI_FileFree((OCI_File *)obj);
            break;
        }
        case OCI_CDT_OBJECT:
        {
            res = OCI_ObjectFree((OCI_Object *)obj);
            break;
        }
        case OCI_CDT_COLLECTION:
        {
            res = OCI_CollFree((OCI_Coll *)obj);
            break;
        }
        case OCI_CDT_TIMESTAMP:
        {
            res = OCI_TimestampFree((OCI_Timestamp *)obj);
            break;
        }
        case OCI_CDT_INTERVAL:
        {
            res = OCI_IntervalFree((OCI_Interval *)obj);
            break;
        }
        case OCI_CDT_REF:
        {
            res= OCI_RefFree((OCI_Ref *)obj);
            break;
        }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_KeyMapFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_KeyMapFree
(
    void
)
{
    boolean res      = TRUE;
    OCI_HashEntry *e = NULL;
    OCI_HashValue *v = NULL;
    int nb_err       = 0;

    OCI_CHECK(NULL == OCILib.key_map, TRUE)

    const int n = OCI_HashGetSize(OCILib.key_map);

    for (int i = 0; i < n; i++)
    {
        e = OCI_HashGetEntry(OCILib.key_map, i);

        while (e)
        {
            v = e->values;

            while (v)
            {
                if (!OCI_ThreadKeyFree((OCI_ThreadKey *) (v->value.p_void)))
                {
                    nb_err++;
                }

                v = v->next;
            }

            e = e->next;
        }
    }

    res = (OCI_HashFree(OCILib.key_map) && (nb_err == 0));

    OCILib.key_map = NULL;

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CallEnter
 * --------------------------------------------------------------------------------------------- */

void OCI_CallEnter
(
    OCI_Context *ctx
)
{
    if (ctx && ctx->call_err)
    {
        if (ctx->call_err->depth == 0 && ctx->call_err->type != OCI_UNKNOWN)
        {
            OCI_ErrorReset(ctx->call_err);
        }

        ctx->call_err->depth++;
    }
}

/* --------------------------------------------------------------------------------------------- *
* OCI_CallExit
* --------------------------------------------------------------------------------------------- */

void OCI_CallExit
(
    OCI_Context *ctx
)
{
    if (ctx && ctx->call_err)
    {
        if (ctx->call_err->depth > 0) ctx->call_err->depth--;

        ctx->call_err->raise = (ctx->call_err->depth == 0) &&
                               (ctx->call_err->type != OCI_UNKNOWN) &&
                               (!ctx->call_status || (OCI_ERR_WARNING == ctx->call_err->type && OCILib.warnings_on));
    } 
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_Initialize
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Initialize
(
    POCI_ERROR   err_handler,
    const otext *lib_path,
    unsigned int mode
)
{
    unsigned int i = 0;
    ub4 oci_mode = OCI_ENV_MODE | OCI_OBJECT;

#ifdef OCI_IMPORT_RUNTIME

    char path[OCI_SIZE_BUFFER+1];

    size_t len = (size_t) 0;

#endif

    OCI_CALL_DECLARE_VARIABLES(boolean, FALSE, TRUE)
    OCI_CALL_CONTEXT_ENTER(mode)

    /* check if it was already initialized */

    if (OCILib.loaded)
    {
        return TRUE;
    }

#if defined(OCI_CHARSET_WIDE) && (OCI_VERSION_COMPILE >= OCI_10_2)

    /* activate support for N' substitution (nchar replacement in SQL statement strings) */

    oci_mode |= OCI_NCHAR_LITERAL_REPLACE_ON;

#endif

    memset(&OCILib, 0, sizeof(OCI_Library));

    OCILib.error_handler        = err_handler;

    OCILib.version_compile      = OCI_VERSION_COMPILE;
    OCILib.version_runtime      = OCI_VERSION_RUNTIME;

    OCILib.env_mode             = mode;
    OCILib.charset              = (sizeof(otext) == sizeof(wchar_t)) ? OCI_CHAR_WIDE : OCI_CHAR_ANSI;
    OCILib.use_wide_char_conv   = (OCILib.charset == OCI_CHAR_WIDE && (WCHAR_MAX == WCHAR_4_BYTES));

    for (i = 0; i < OCI_FMT_COUNT; i++)
    {
        OCILib.formats[i] = ostrdup(FormatDefaultValues[i]);
    }

    /* load any specific environment variable */

    for (i = 0; i < OCI_VARS_COUNT; i++)
    {
        char *value = getenv(EnvironmentVarNames[i]);

        OCILib.env_vars[i] = value && (ocistrcasecmp(value, OCI_VARS_TRUE_VALUE) == 0 || atoi(value) == 1);
    }

    /* test for UTF8 environment */

    if (OCI_CHAR_ANSI == OCILib.charset)
    {
        char *str = getenv("NLS_LANG");

        if (str)
        {
            char nls_lang[OCI_SIZE_OBJ_NAME + 1] = "";

            strncat(nls_lang, str, OCI_SIZE_OBJ_NAME);

            for (str = nls_lang; *str; str++)
            {
                *str = (char) toupper(*str);
            }

            OCILib.nls_utf8 = (NULL != strstr(nls_lang, "UTF8"));
        }
    }

#ifdef OCI_IMPORT_LINKAGE

    OCI_NOT_USED(lib_path)

    #if defined(OCI_BIG_UINT_ENABLED)

    OCILib.use_lob_ub8 = TRUE;

    #endif

    #if defined(OCI_STMT_SCROLLABLE_READONLY)

    OCILib.use_scrollable_cursors = TRUE;

    #endif

#else

    memset(path, 0, sizeof(path));

    #if defined(OCI_CHARSET_WIDE)

    if (lib_path && lib_path[0])
    {
        len = wcstombs(path, lib_path, sizeof(path));
    }

    #else

    if (lib_path && lib_path[0])
    {
        strncat(path, lib_path, sizeof(path) - strlen(path) - 1);

        len = strlen(path);
    }

    #endif

    if ((len > (size_t) 0) && (len < sizeof(path) -1) && (OCI_CHAR_SLASH != path[len - (size_t) 1]))
    {
        path[len] = OCI_CHAR_SLASH;
        len++;
    }

    strncat(path, OCI_DL_ANSI_NAME, sizeof(path) - len - 1);

    OCILib.lib_handle = LIB_OPEN(path);

    if (OCILib.lib_handle)
    {

        /* Now loading all symbols - no check is performed on each function,
           Basic checks will be done to ensure we're loading an
           Oracle and compatible library ...
        */

        LIB_SYMBOL(OCILib.lib_handle, "OCIEnvCreate", OCIEnvCreate,
                   OCIENVCREATE);

        LIB_SYMBOL(OCILib.lib_handle, "OCIServerAttach", OCIServerAttach,
                   OCISERVERATTACH);
        LIB_SYMBOL(OCILib.lib_handle, "OCIServerDetach", OCIServerDetach,
                   OCISERVERDETACH);

        LIB_SYMBOL(OCILib.lib_handle, "OCIHandleAlloc", OCIHandleAlloc,
                   OCIHANDLEALLOC);
        LIB_SYMBOL(OCILib.lib_handle, "OCIHandleFree",  OCIHandleFree,
                   OCIHANDLEFREE);

        LIB_SYMBOL(OCILib.lib_handle, "OCIDescriptorAlloc", OCIDescriptorAlloc,
                   OCIDESCRIPTORALLOC);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDescriptorFree", OCIDescriptorFree,
                   OCIDESCRIPTORFREE);

        LIB_SYMBOL(OCILib.lib_handle, "OCIAttrSet", OCIAttrSet,
                   OCIATTRSET);
        LIB_SYMBOL(OCILib.lib_handle, "OCIAttrGet", OCIAttrGet,
                   OCIATTRGET);

        LIB_SYMBOL(OCILib.lib_handle, "OCIParamSet", OCIParamSet,
                   OCIPARAMSET);
        LIB_SYMBOL(OCILib.lib_handle, "OCIParamGet", OCIParamGet,
                   OCIPARAMGET);

        LIB_SYMBOL(OCILib.lib_handle, "OCISessionBegin", OCISessionBegin,
                   OCISESSIONBEGIN);
        LIB_SYMBOL(OCILib.lib_handle, "OCISessionEnd", OCISessionEnd,
                   OCISESSIONEND);

        LIB_SYMBOL(OCILib.lib_handle, "OCIPasswordChange", OCIPasswordChange,
                   OCIPASSWORDCHANGE);

        LIB_SYMBOL(OCILib.lib_handle, "OCITransStart", OCITransStart,
                   OCITRANSSTART);
        LIB_SYMBOL(OCILib.lib_handle, "OCITransDetach", OCITransDetach,
                   OCITRANSDETACH);
        LIB_SYMBOL(OCILib.lib_handle, "OCITransPrepare", OCITransPrepare,
                   OCITRANSPREPARE);
        LIB_SYMBOL(OCILib.lib_handle, "OCITransForget", OCITransForget,
                   OCITRANSFORGET);
        LIB_SYMBOL(OCILib.lib_handle, "OCITransCommit", OCITransCommit,
                   OCITRANSCOMMIT);
        LIB_SYMBOL(OCILib.lib_handle, "OCITransRollback", OCITransRollback,
                   OCITRANSROLLBACK);

        LIB_SYMBOL(OCILib.lib_handle, "OCIErrorGet",  OCIErrorGet,
                   OCIERRORGET);
        LIB_SYMBOL(OCILib.lib_handle, "OCIServerVersion", OCIServerVersion,
                   OCISERVERVERSION);
        LIB_SYMBOL(OCILib.lib_handle, "OCIBreak", OCIBreak,
                   OCIBREAK);

        LIB_SYMBOL(OCILib.lib_handle, "OCIBindByPos", OCIBindByPos,
                   OCIBINDBYPOS);
        LIB_SYMBOL(OCILib.lib_handle, "OCIBindByName", OCIBindByName,
                   OCIBINDBYNAME);
        LIB_SYMBOL(OCILib.lib_handle, "OCIBindDynamic", OCIBindDynamic,
                   OCIBINDDYNAMIC);
        LIB_SYMBOL(OCILib.lib_handle, "OCIBindObject", OCIBindObject,
                   OCIBINDOBJECT);

        LIB_SYMBOL(OCILib.lib_handle, "OCIDefineByPos", OCIDefineByPos,
                   OCIDEFINEBYPOS);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDefineObject", OCIDefineObject,
                   OCIDEFINEOBJECT);

        LIB_SYMBOL(OCILib.lib_handle, "OCIStmtPrepare", OCIStmtPrepare,
                   OCISTMTPREPARE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIStmtExecute", OCIStmtExecute,
                   OCISTMTEXECUTE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIStmtFetch", OCIStmtFetch,
                   OCISTMTFETCH);
        LIB_SYMBOL(OCILib.lib_handle, "OCIStmtFetch2", OCIStmtFetch2,
                   OCISTMTFETCH2);

        LIB_SYMBOL(OCILib.lib_handle, "OCIStmtGetPieceInfo", OCIStmtGetPieceInfo,
                   OCISTMTGETPIECEINFO);
        LIB_SYMBOL(OCILib.lib_handle, "OCIStmtSetPieceInfo", OCIStmtSetPieceInfo,
                   OCISTMTSETPIECEINFO);

        LIB_SYMBOL(OCILib.lib_handle, "OCILobCreateTemporary", OCILobCreateTemporary,
                   OCILOBCREATETEMPORARY);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobFreeTemporary", OCILobFreeTemporary,
                   OCILOBFREETEMPORARY);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobIsTemporary", OCILobIsTemporary,
                   OCILOBISTEMPORARY);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobRead", OCILobRead,
                   OCILOBREAD);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobWrite", OCILobWrite,
                   OCILOBWRITE);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobCopy", OCILobCopy,
                   OCILOBCOPY);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobTrim", OCILobTrim,
                   OCILOBTRIM);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobErase", OCILobErase,
                   OCILOBERASE);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobAppend", OCILobAppend,
                   OCILOBAPPEND);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobGetLength", OCILobGetLength,
                   OCILOBGETLENGTH);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobGetChunkSize", OCILobGetChunkSize,
                   OCILOBGETCHUNKSIZE);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobOpen", OCILobOpen,
                   OCILOBOPEN);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobClose", OCILobClose,
                   OCILOBCLOSE);

    #ifdef ORAXB8_DEFINED

        LIB_SYMBOL(OCILib.lib_handle, "OCILobCopy2", OCILobCopy2,
                   OCILOBCOPY2);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobErase2", OCILobErase2,
                   OCILOBERASE2);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobGetLength2", OCILobGetLength2,
                   OCILOBGETLENGTH2);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobLoadFromFile2", OCILobLoadFromFile2,
                   OCILOBLOADFROMFILE2);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobRead2", OCILobRead2,
                   OCILOBREAD2);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobTrim2", OCILobTrim2,
                   OCILOBTRIM2);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobWrite2", OCILobWrite2,
                   OCILOBWRITE2);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobWriteAppend2", OCILobWriteAppend2,
                   OCILOBWRITEAPPEND2);

    #endif

        LIB_SYMBOL(OCILib.lib_handle, "OCILobFileOpen", OCILobFileOpen,
                   OCILOBFILEOPEN);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobFileClose", OCILobFileClose,
                   OCILOBFILECLOSE);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobFileCloseAll", OCILobFileCloseAll,
                   OCILOBFILECLOSEALL);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobFileIsOpen", OCILobFileIsOpen,
                   OCILOBFILEISOPEN);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobFileExists", OCILobFileExists,
                   OCILOBFILEEXISTS);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobFileGetName", OCILobFileGetName,
                   OCILOBFIELGETNAME);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobFileSetName", OCILobFileSetName,
                   OCILOBFILESETNAME);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobLoadFromFile", OCILobLoadFromFile,
                   OCILOBLOADFROMFILE);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobWriteAppend", OCILobWriteAppend,
                   OCILOBWRITEAPPEND);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobIsEqual", OCILobIsEqual,
                   OCILOBISEQUAL);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobAssign", OCILobAssign,
                   OCILOBASSIGN);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobLocatorAssign", OCILobLocatorAssign,
                   OCILOBLOCATORASSIGN);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobFlushBuffer", OCILobFlushBuffer,
                   OCILOBFLUSHBUFFER);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobGetStorageLimit", OCILobGetStorageLimit,
                   OCILOBGETSTORAGELIMIT);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobEnableBuffering", OCILobEnableBuffering,
                   OCILOBENABLEBUFFERING);
        LIB_SYMBOL(OCILib.lib_handle, "OCILobDisableBuffering", OCILobDisableBuffering,
                   OCILOBDISABLEBUFFERING);

        LIB_SYMBOL(OCILib.lib_handle, "OCIDateAssign", OCIDateAssign,
                   OCIDATEASSIGN);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateToText", OCIDateToText,
                   OCIDATETOTEXT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateFromText", OCIDateFromText,
                   OCIDATEFROMTEXT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateCompare", OCIDateCompare,
                   OCIDATECOMPARE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateAddMonths", OCIDateAddMonths,
                   OCIDATEADDMONTHS);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateAddDays", OCIDateAddDays,
                   OCIDATEADDDAYS);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateLastDay", OCIDateLastDay,
                   OCIDATELASTDAY);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateDaysBetween", OCIDateDaysBetween,
                   OCIDATEDAYSBETWEEN);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateZoneToZone", OCIDateZoneToZone,
                   OCIDATEZONETOZONE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateNextDay", OCIDateNextDay,
                   OCIDATENEXTDAY);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateCheck", OCIDateCheck,
                   OCIDATECHECK);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateSysDate", OCIDateSysDate,
                   OCIDATESYSDATE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDescribeAny", OCIDescribeAny,
                   OCIDESCRIBEANY);

        LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalAssign", OCIIntervalAssign,
                   OCIINTERVALASSIGN);
        LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalCheck", OCIIntervalCheck,
                   OCIINTERVALCHECK);
        LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalCompare", OCIIntervalCompare,
                   OCIINTERVALCOMPARE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalFromText", OCIIntervalFromText,
                   OCIINTERVALFROMTEXT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalToText", OCIIntervalToText,
                   OCIINTERVALTOTEXT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalFromTZ", OCIIntervalFromTZ,
                   OCIINTERVALFROMTZ);
        LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalGetDaySecond", OCIIntervalGetDaySecond,
                   OCIINTERVALGETDAYSECOND);
        LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalGetYearMonth", OCIIntervalGetYearMonth,
                   OCIINTERVALGETYEARMONTH);
        LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalSetDaySecond", OCIIntervalSetDaySecond,
                   OCIINTERVALSETDAYSECOND);
        LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalSetYearMonth", OCIIntervalSetYearMonth,
                   OCIINTERVALSETYEARMONTH);
        LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalSubtract", OCIIntervalSubtract,
                   OCIINTERVALSUBTRACT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalAdd", OCIIntervalAdd,
                   OCIINTERVALADD);

        LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeAssign", OCIDateTimeAssign,
                   OCIDATETIMEASSIGN);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeCheck", OCIDateTimeCheck,
                   OCIDATETIMECHECK);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeCompare", OCIDateTimeCompare,
                   OCIDATETIMECOMPARE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeConstruct", OCIDateTimeConstruct,
                   OCIDATETIMECONSTRUCT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeConvert", OCIDateTimeConvert,
                   OCIDATETIMECONVERT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeFromArray", OCIDateTimeFromArray,
                   OCIDATETIMEFROMARRAY);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeToArray", OCIDateTimeToArray,
                   OCIDATETIMETOARRAY);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeFromText", OCIDateTimeFromText,
                   OCIDATETIMEFROMTEXT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeToText", OCIDateTimeToText,
                   OCIDATETIMETOTEXT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeGetDate", OCIDateTimeGetDate,
                   OCIDATETIMEGETDATE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeGetTime", OCIDateTimeGetTime,
                   OCIDATETIMEGETTIME);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeGetTimeZoneName", OCIDateTimeGetTimeZoneName,
                   OCIDATETIMEGETTIMEZONENAME);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeGetTimeZoneOffset", OCIDateTimeGetTimeZoneOffset,
                   OCIDATETIMEGETTIMEZONEOFFSET);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeIntervalAdd", OCIDateTimeIntervalAdd,
                   OCIDATETIMEINTERVALADD);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeIntervalSub", OCIDateTimeIntervalSub,
                   OCIDATETIMEINTERVALSUB);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeSubtract", OCIDateTimeSubtract,
                   OCIDATETIMESUBTRACT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeSysTimeStamp", OCIDateTimeSysTimeStamp,
                   OCIDATETIMESYSTIMESTAMP);

        LIB_SYMBOL(OCILib.lib_handle, "OCITypeByRef", OCITypeByRef,
                   OCITYPEBYREF);

        LIB_SYMBOL(OCILib.lib_handle, "OCINumberToInt", OCINumberToInt,
                   OCINUMBERTOINT);
        LIB_SYMBOL(OCILib.lib_handle, "OCINumberFromInt", OCINumberFromInt,
                   OCINUMBERFROMINT);
        LIB_SYMBOL(OCILib.lib_handle, "OCINumberToReal", OCINumberToReal,
                   OCINUMBERTOREAL);
        LIB_SYMBOL(OCILib.lib_handle, "OCINumberFromReal", OCINumberFromReal,
                   OCINUMBERFROMREAL);
        LIB_SYMBOL(OCILib.lib_handle, "OCINumberToText", OCINumberToText,
                   OCINUMBERTOTEXT);
        LIB_SYMBOL(OCILib.lib_handle, "OCINumberFromText", OCINumberFromText,
                   OCINUMBERFROMTEXT);
        LIB_SYMBOL(OCILib.lib_handle, "OCINumberAssign", OCINumberAssign,
                   OCINUMBERASSIGN);
        LIB_SYMBOL(OCILib.lib_handle, "OCINumberAdd", OCINumberAdd,
                   OCINUMBERADD);
        LIB_SYMBOL(OCILib.lib_handle, "OCINumberSub", OCINumberSub,
                   OCINUMBERSUB);
        LIB_SYMBOL(OCILib.lib_handle, "OCINumberMul", OCINumberMul,
                   OCINUMBERMUL);
        LIB_SYMBOL(OCILib.lib_handle, "OCINumberDiv", OCINumberDiv,
                   OCINUMBERDIV);
        LIB_SYMBOL(OCILib.lib_handle, "OCINumberCmp", OCINumberCmp,
                   OCINUMBERCMP);

        LIB_SYMBOL(OCILib.lib_handle, "OCIStringPtr", OCIStringPtr,
                   OCISTRINGPTR);
        LIB_SYMBOL(OCILib.lib_handle, "OCIStringSize", OCIStringSize,
                   OCISTRINGSIZE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIStringAssignText", OCIStringAssignText,
                   OCISTRINGASSIGNTEXT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIStringResize", OCIStringResize,
                   OCISTRINGRESIZE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIRawPtr", OCIRawPtr,
                   OCIRAWPTR);
        LIB_SYMBOL(OCILib.lib_handle, "OCIRawAssignBytes", OCIRawAssignBytes,
                   OCIRAWASSIGNBYTES);
        LIB_SYMBOL(OCILib.lib_handle, "OCIRawResize", OCIRawResize,
                   OCIRAWRESIZE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIRawAllocSize", OCIRawAllocSize,
                   OCIRAWALLOCSIZE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIRawSize", OCIRawSize,
                   OCIRAWSIZE);

        LIB_SYMBOL(OCILib.lib_handle, "OCIObjectNew", OCIObjectNew,
                   OCIOBJECTNEW);
        LIB_SYMBOL(OCILib.lib_handle, "OCIObjectFree", OCIObjectFree,
                   OCIOBJECTFREE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIObjectSetAttr", OCIObjectSetAttr,
                   OCIOBJECTSETATTR);
        LIB_SYMBOL(OCILib.lib_handle, "OCIObjectGetAttr", OCIObjectGetAttr,
                   OCIOBJECTGETATTR);
        LIB_SYMBOL(OCILib.lib_handle, "OCIObjectPin", OCIObjectPin,
                   OCIOBJECTPIN);
        LIB_SYMBOL(OCILib.lib_handle, "OCIObjectUnpin", OCIObjectUnpin,
                   OCIOBJECTUNPIN);
        LIB_SYMBOL(OCILib.lib_handle, "OCIObjectCopy", OCIObjectCopy,
                   OCIOBJECTCOPY);
        LIB_SYMBOL(OCILib.lib_handle, "OCIObjectGetObjectRef", OCIObjectGetObjectRef,
                   OCIOBJECTGETOBJECTREF);
        LIB_SYMBOL(OCILib.lib_handle, "OCIObjectGetProperty", OCIObjectGetProperty,
                   OCIOBJECTGETPROPERTY);
        LIB_SYMBOL(OCILib.lib_handle, "OCIObjectGetInd", OCIObjectGetInd,
                   OCIOBJECTGETIND);
        LIB_SYMBOL(OCILib.lib_handle, "OCIObjectGetTypeRef", OCIObjectGetTypeRef,
            OCIOBJECTGETOBJECTREF);

        LIB_SYMBOL(OCILib.lib_handle, "OCIRefAssign", OCIRefAssign,
                   OCIREFASSIGN);
        LIB_SYMBOL(OCILib.lib_handle, "OCIRefIsNull", OCIRefIsNull,
                   OCIREFISNULL);
        LIB_SYMBOL(OCILib.lib_handle, "OCIRefClear", OCIRefClear,
                   OCIREFCLEAR);
        LIB_SYMBOL(OCILib.lib_handle, "OCIRefToHex", OCIRefToHex,
                   OCIREFTOHEX);
        LIB_SYMBOL(OCILib.lib_handle, "OCIRefHexSize", OCIRefHexSize,
                   OCIREFHEXSIZE);

        LIB_SYMBOL(OCILib.lib_handle, "OCIArrayDescriptorAlloc", OCIArrayDescriptorAlloc,
                   OCIARRAYDESCRIPTORALLOC);
        LIB_SYMBOL(OCILib.lib_handle, "OCIArrayDescriptorFree", OCIArrayDescriptorFree,
                   OCIARRAYDESCRIPTORFREE);

        LIB_SYMBOL(OCILib.lib_handle, "OCIClientVersion", OCIClientVersion,
                   OCICLIENTVERSION);

        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadProcessInit", OCIThreadProcessInit,
                   OCITHREADPROCESSINIT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadInit", OCIThreadInit,
                   OCITHREADINIT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadTerm", OCIThreadTerm,
                   OCITHREADTERM);

        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadIdInit", OCIThreadIdInit,
                   OCITHREADIDINIT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadIdDestroy", OCIThreadIdDestroy,
                   OCITHREADIDDESTROY);
        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadHndInit", OCIThreadHndInit,
                   OCITHREADHNDINIT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadHndDestroy", OCIThreadHndDestroy,
                   OCITHREADHNDDESTROY);
        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadCreate", OCIThreadCreate,
                   OCITHREADCREATE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadJoin", OCIThreadJoin,
                   OCITHREADJOIN);
        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadClose", OCIThreadClose,
                   OCITHREADCLOSE);

        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadMutexInit", OCIThreadMutexInit,
                   OCITHREADMUTEXINIT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadMutexDestroy", OCIThreadMutexDestroy,
                   OCITHREADMUTEXDESTROY);
        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadMutexAcquire", OCIThreadMutexAcquire,
                   OCITHREADMUTEXACQUIRE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadMutexRelease", OCIThreadMutexRelease,
                   OCITHREADMUTEXRELEASE);

        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadKeyInit", OCIThreadKeyInit,
                   OCITHREADKEYINIT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadKeyDestroy", OCIThreadKeyDestroy,
                   OCITHREADKEYDESTROY);
        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadKeySet", OCIThreadKeySet,
                   OCITHREADKEYSET);
        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadKeyGet", OCIThreadKeyGet,
                   OCITHREADKEYGET);

        LIB_SYMBOL(OCILib.lib_handle, "OCIConnectionPoolCreate", OCIConnectionPoolCreate,
                   OCICONNECTIONPOOLCREATE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIConnectionPoolDestroy", OCIConnectionPoolDestroy,
                   OCICONNECTIONPOOLDESTROY);

        LIB_SYMBOL(OCILib.lib_handle, "OCISessionPoolCreate", OCISessionPoolCreate,
                   OCISESSIONPOOLCREATE);
        LIB_SYMBOL(OCILib.lib_handle, "OCISessionPoolDestroy", OCISessionPoolDestroy,
                   OCISESSIONPOOLDESTROY);

        LIB_SYMBOL(OCILib.lib_handle, "OCISessionGet", OCISessionGet,
                   OCISESSIONGET);
        LIB_SYMBOL(OCILib.lib_handle, "OCISessionRelease", OCISessionRelease,
                   OCISESSIONRELEASE);

        LIB_SYMBOL(OCILib.lib_handle, "OCICollSize", OCICollSize,
                   OCICOLLSIZE);
        LIB_SYMBOL(OCILib.lib_handle, "OCICollMax", OCICollMax,
                   OCICOLLMAX);
        LIB_SYMBOL(OCILib.lib_handle, "OCICollGetElem", OCICollGetElem,
                   OCICOLLGETITEM);
        LIB_SYMBOL(OCILib.lib_handle, "OCICollAssignElem", OCICollAssignElem,
                   OCICOLLASSIGNELEM);
        LIB_SYMBOL(OCILib.lib_handle, "OCICollAssign", OCICollAssign,
                   OCICOLLASSIGN);
        LIB_SYMBOL(OCILib.lib_handle, "OCICollAppend", OCICollAppend,
                   OCICOLLAPPEND);
        LIB_SYMBOL(OCILib.lib_handle, "OCICollTrim", OCICollTrim,
                   OCICOLLTRIM);
        LIB_SYMBOL(OCILib.lib_handle, "OCITableDelete", OCITableDelete,
                   OCITABLEDELETE);
        LIB_SYMBOL(OCILib.lib_handle, "OCITableSize", OCITableSize,
                   OCITABLESIZE);

        LIB_SYMBOL(OCILib.lib_handle, "OCIIterCreate", OCIIterCreate,
                   OCIITERCREATE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIIterDelete", OCIIterDelete,
                   OCIITERDELETE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIIterInit", OCIIterInit,
                   OCIITERINIT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIIterNext", OCIIterNext,
                   OCIITERNEXT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIIterPrev", OCIIterPrev,
                   OCIITERPREV);

        LIB_SYMBOL(OCILib.lib_handle, "OCIDirPathAbort", OCIDirPathAbort,
                   OCIDIRPATHABORT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDirPathDataSave", OCIDirPathDataSave,
                   OCIDIRPATHDATASAVE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDirPathFinish", OCIDirPathFinish,
                   OCIDIRPATHFINISH);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDirPathPrepare", OCIDirPathPrepare,
                   OCIDIRPATHPREPARE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDirPathLoadStream", OCIDirPathLoadStream,
                   OCIDIRPATHLOADSTREAM);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDirPathColArrayEntrySet", OCIDirPathColArrayEntrySet,
                   OCIDIRPATHCOLARRAYENTRYSET);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDirPathColArrayReset", OCIDirPathColArrayReset,
                   OCIDIRPATHCOLARRAYRESET);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDirPathColArrayToStream", OCIDirPathColArrayToStream,
                   OCIDIRPATHCOLARRAYTOSTREAM);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDirPathStreamReset", OCIDirPathStreamReset,
                   OCIDIRPATHSTREAMRESET);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDirPathFlushRow", OCIDirPathFlushRow,
                   OCIDIRPATHFLUSHROW);

        LIB_SYMBOL(OCILib.lib_handle, "OCICacheFree", OCICacheFree,
                   OCICACHEFREE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIPing", OCIPing,
                   OCIPING);

        LIB_SYMBOL(OCILib.lib_handle, "OCIDBStartup", OCIDBStartup,
                   OCIDBSTARTUP);
        LIB_SYMBOL(OCILib.lib_handle, "OCIDBShutdown", OCIDBShutdown,
                   OCIDBSHUTDOWN);

        LIB_SYMBOL(OCILib.lib_handle, "OCIStmtPrepare2", OCIStmtPrepare2,
                   OCISTMTPREPARE2);
        LIB_SYMBOL(OCILib.lib_handle, "OCIStmtRelease", OCIStmtRelease,
                   OCISTMTRELEASE);

        LIB_SYMBOL(OCILib.lib_handle, "OCISubscriptionRegister", OCISubscriptionRegister,
                   OCISUBSCRIPTIONREGISTER);
        LIB_SYMBOL(OCILib.lib_handle, "OCISubscriptionUnRegister", OCISubscriptionUnRegister,
                   OCISUBSCRIPTIONUNREGISTER);

        LIB_SYMBOL(OCILib.lib_handle, "OCIAQEnq", OCIAQEnq,
                   OCIAQENQ);
        LIB_SYMBOL(OCILib.lib_handle, "OCIAQDeq", OCIAQDeq,
                   OCIAQDEQ);
        LIB_SYMBOL(OCILib.lib_handle, "OCIAQListen", OCIAQListen,
                   OCIAQLISTEN);

        LIB_SYMBOL(OCILib.lib_handle, "xaoSvcCtx", xaoSvcCtx,
                   XAOSVCCTX);
        LIB_SYMBOL(OCILib.lib_handle, "xaoEnv", xaoEnv,
                   XAOENV);

        LIB_SYMBOL(OCILib.lib_handle, "OCILobGetContentType", OCILobGetContentType,
                   OCILOBGETCONTENTTYPE);

        LIB_SYMBOL(OCILib.lib_handle, "OCIStmtGetNextResult", OCIStmtGetNextResult,
                   OCISTMTGETNEXTRESULT);

        /* API Version checking */

        if (OCIStmtGetNextResult)
        {
            OCILib.version_runtime = OCI_12_1;
        }
        else if (OCILobGetContentType)
        {
            OCILib.version_runtime = OCI_11_2;
        }
        else if (OCIArrayDescriptorFree)
        {
            OCILib.version_runtime = OCI_11_1;
        }
        else if (OCIClientVersion)
        {
            OCILib.version_runtime = OCI_10_2;
        }
        else if (OCILobWrite2)
        {
            OCILib.version_runtime = OCI_10_1;
        }
        else if (OCIStmtPrepare2)
        {
            OCILib.version_runtime = OCI_9_2;
        }
        else if (OCIDateTimeGetTimeZoneName)
        {
            OCILib.version_runtime = OCI_9_0;
        }
        else if (OCIThreadCreate)
        {
            OCILib.version_runtime = OCI_8_1;
        }
        else if (OCIEnvCreate)
        {
            OCILib.version_runtime = OCI_8_0;
        }
        else
        {
            LIB_CLOSE(OCILib.lib_handle);
            OCI_RAISE_EXCEPTION(OCI_ExceptionLoadingSymbols())
        }
    }
    else
    {
        OCI_RAISE_EXCEPTION(OCI_ExceptionLoadingSharedLib())
    }

    #if defined(OCI_BIG_UINT_ENABLED)

    if ((OCILib.version_runtime >= OCI_10_1) && OCILobCopy2)
    {
        OCILib.use_lob_ub8 = TRUE;
    }

    #endif

    #if defined(OCI_STMT_SCROLLABLE_READONLY)

    if ((OCILib.version_runtime >= OCI_9_0) && OCIStmtFetch2)
    {
        OCILib.use_scrollable_cursors = TRUE;
    }

    #endif

#endif

#if defined(OCI_CHARSET_WIDE)

    /* Oracle 8i does not support full Unicode mode */

    if (OCILib.version_runtime < OCI_9_0)
    {
        OCI_RAISE_EXCEPTION(OCI_ExceptionNotAvailable(NULL, OCI_FEATURE_WIDE_USERDATA))
    }

#endif

    /* Initialize OCI environment */

    if (mode & OCI_ENV_THREADED)
    {
        oci_mode |= OCI_THREADED;
    }

    if (mode & OCI_ENV_EVENTS)
    {
        oci_mode |= OCI_EVENTS;
    }

    /* create environment on success */

    OCI_STATUS = OCI_SUCCESSFUL(OCIEnvCreate(&OCILib.env, oci_mode,
                                               (dvoid *) &OCILib,
                                               OCI_MemAllocOracleClient,
                                               OCI_MemReallocOracleClient,
                                               OCI_MemFreeOracleClient,
                                               (size_t) 0, (dvoid **) NULL));

    /*  allocate error handle */
    if (OCI_STATUS)
    {
        OCI_STATUS = OCI_HandleAlloc((dvoid *)OCILib.env, (dvoid **) (void *) &OCILib.err, OCI_HTYPE_ERROR);
    }
    else
    {
        OCI_RAISE_EXCEPTION(OCI_ExceptionOCIEnvironment())
    }

    /* on success, we need to initialize OCIThread object support */

    if (OCI_STATUS)
    {
        if (OCI_LIB_THREADED)
        {
            OCIThreadProcessInit();

            OCI_STATUS = OCI_SUCCESSFUL(OCIThreadInit(OCILib.env, OCILib.err));

            OCILib.mem_mutex = OCI_MutexCreateInternal();
            OCI_STATUS = (NULL != OCILib.mem_mutex);
        }

        /* create thread key for thread errors */

        if (OCI_STATUS)
        {
            OCILib.key_errs = OCI_ThreadKeyCreateInternal((POCI_THREADKEYDEST) OCI_ErrorFree);
            OCI_STATUS = (NULL != OCILib.key_errs);
        }

        /* allocate connections internal list */

        if (OCI_STATUS)
        {
            OCILib.cons = OCI_ListCreate(OCI_IPC_CONNECTION);
            OCI_STATUS = (NULL != OCILib.cons);
        }

        /* allocate pools internal list */

        if (OCI_STATUS)
        {
            OCILib.pools = OCI_ListCreate(OCI_IPC_POOL);
            OCI_STATUS = (NULL != OCILib.pools);
        }

    #if OCI_VERSION_COMPILE >= OCI_10_2

        /* allocate connection pools internal list */

        if (OCI_STATUS)
        {
            OCILib.subs = OCI_ListCreate(OCI_IPC_NOTIFY);

            OCI_STATUS = (NULL != OCILib.subs);
        }

    #endif

        if (OCI_STATUS)
        {
            OCILib.arrs = OCI_ListCreate(OCI_IPC_ARRAY);
            OCI_STATUS = (NULL != OCILib.arrs);
        }
    }

    OCILib.loaded = OCI_RETVAL = OCI_STATUS;

    /* test for XA support */

#ifdef _WINDOWS
    #if OCI_VERSION_COMPILE >= OCI_10_1
        OCILib.use_xa = (NULL != xaoEnv);
    #else
        OCILib.use_xa = FALSE;
    #endif
#else
    OCILib.use_xa = TRUE;
#endif

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Cleanup
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Cleanup
(
    void
)
{
    boolean      res = TRUE;
    unsigned int i   = 0;

    /* free all arrays */

    OCI_ListForEach(OCILib.arrs, (POCI_LIST_FOR_EACH) OCI_ArrayClose);
    OCI_ListClear(OCILib.arrs);

    /* free all subscriptions */

    OCI_ListForEach(OCILib.subs, (POCI_LIST_FOR_EACH) OCI_SubscriptionClose);
    OCI_ListClear(OCILib.subs);

    /* free all connections */

    OCI_ListForEach(OCILib.cons, (POCI_LIST_FOR_EACH) OCI_ConnectionClose);
    OCI_ListClear(OCILib.cons);

    /* free all pools */

    OCI_ListForEach(OCILib.pools, (POCI_LIST_FOR_EACH) OCI_PoolClose);
    OCI_ListClear(OCILib.pools);

    /* free objects */

    OCI_KeyMapFree();

    OCI_ListFree(OCILib.cons);
    OCI_ListFree(OCILib.pools);
    OCI_ListFree(OCILib.subs);
    OCI_ListFree(OCILib.arrs);

    OCILib.cons    = NULL;
    OCILib.pools   = NULL;
    OCILib.subs    = NULL;
    OCILib.key_map = NULL;

    for (i = 0; i < OCI_FMT_COUNT; i++)
    {
        OCI_FREE(OCILib.formats[i])
    }

    /* finalize OCIThread object support */

    if (OCI_LIB_THREADED)
    {
        /* free the memory mutex. We set its reference in the library structure to NULL first otherwise
           it would generate an OCI error when calling OCI_HandleAlloc() for freeing the mutex object error handle
        */

        OCI_Mutex * mutex = OCILib.mem_mutex;

        OCILib.mem_mutex = NULL;

        if (mutex)
        {
            OCI_MutexFree(mutex);
        }

        res = OCI_SUCCESSFUL(OCIThreadTerm(OCILib.env, OCILib.err));
    }

    /* free error thread key */

    if (OCILib.key_errs)
    {        
        OCI_ThreadKey *key = OCILib.key_errs;
        OCI_Error     *err = OCI_ErrorGet(FALSE);

        OCILib.key_errs = NULL;

        OCI_ErrorFree(err);
        OCI_ThreadKeySet(key, NULL);
        OCI_ThreadKeyFree(key);
    }

    /* set unloaded flag */

    OCILib.loaded = FALSE;

    /* close error handle */

    if (OCILib.err)
    {
        OCI_HandleFree(OCILib.err, OCI_HTYPE_ERROR);
    }

    /* close environment handle
       => direct OCIHandleFree() because this handle was not allocated
       with OCI_HandleAlloc()
    */

    if (OCILib.env)
    {
        OCIHandleFree(OCILib.env, OCI_HTYPE_ENV);
    }

#ifdef OCI_IMPORT_RUNTIME

    if (OCILib.lib_handle)
    {
        LIB_CLOSE(OCILib.lib_handle);
    }

#endif

    /* checks for non freed handles */

    if (OCILib.nb_hndlp > 0)
    {
        OCI_ExceptionUnfreedData(OCI_HDLE_HANDLE, OCILib.nb_hndlp);
        res = FALSE;
    }

    /* checks for non freed descriptors */

    if (OCILib.nb_descp > 0)
    {
        OCI_ExceptionUnfreedData(OCI_HDLE_DESCRIPTOR, OCILib.nb_descp);
        res = FALSE;
    }

    /* checks for non freed objects */

    if (OCILib.nb_objinst > 0)
    {
        OCI_ExceptionUnfreedData(OCI_HDLE_OBJECT, OCILib.nb_objinst);
        res = FALSE;
    }

    memset(&OCILib, 0, sizeof(OCILib));

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetOCICompileVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetOCICompileVersion
(
    void
)
{
    OCI_GET_LIB_PROP(unsigned int, OCI_UNKNOWN, OCILib.version_compile)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetOCIRuntimeVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetOCIRuntimeVersion
(
    void
)
{
    OCI_GET_LIB_PROP(unsigned int, OCI_UNKNOWN, OCILib.version_runtime)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetImportMode
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetImportMode
(
    void
)
{
     OCI_GET_LIB_PROP(unsigned int, OCI_UNKNOWN, OCI_IMPORT_MODE)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetCharset
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetCharset
(
    void
)
{
    OCI_GET_LIB_PROP(unsigned int, OCI_UNKNOWN, OCI_CHAR_TEXT)
}

/* --------------------------------------------------------------------------------------------- *
* OCI_GetAllocatedBytes
* --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_GetAllocatedBytes
(
    unsigned int mem_type
)
{
    OCI_CALL_ENTER(big_uint, 0)
    OCI_CALL_CHECK_INITIALIZED()

    if (mem_type & OCI_MEM_ORACLE)
    {
        call_retval += OCILib.mem_bytes_oci;
    }

    if (mem_type & OCI_MEM_OCILIB)
    {
        call_retval += OCILib.mem_bytes_lib;
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetLastError
 * --------------------------------------------------------------------------------------------- */

OCI_Error * OCI_API OCI_GetLastError
(
    void
)
{
    OCI_Error *err = NULL;

    if (!OCILib.loaded || OCILib.env_mode & OCI_ENV_CONTEXT)
    {
        err = OCI_ErrorGet(TRUE);

        if (err && (!err->raise))
        {
            err = NULL;
        }
    }

    return err;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EnableWarnings
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_EnableWarnings
(
    boolean value
)
{
    OCI_SET_LIB_PROP(OCILib.warnings_on, value)
 }

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetErrorHandler
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetErrorHandler
(
    POCI_ERROR handler
)
{
    OCI_SET_LIB_PROP(OCILib.error_handler, handler)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DatabaseStartup
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DatabaseStartup
(
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int sess_mode,
    unsigned int start_mode,
    unsigned int start_flag,
    const otext *spfile
)
{
    OCI_Connection *con = NULL;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_REMOTE_DBS_CONTROL_ENABLED()

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (start_mode & OCI_DB_SPM_START)
    {
        OCIAdmin *adm = NULL;

        /* connect with preliminary authentication mode */

        con = OCI_ConnectionCreate(db, user, pwd, sess_mode | OCI_PRELIM_AUTH);

        OCI_STATUS = (NULL != con);

        if (OCI_STATUS)
        {
            if (spfile && spfile[0])
            {
                dbtext *dbstr  = NULL;
                int     dbsize = -1;

                /* allocate administration handle */

                OCI_STATUS = OCI_HandleAlloc((dvoid *)OCILib.env,  (dvoid **) (void *) &adm, OCI_HTYPE_ADMIN);

                /* set client file if provided */

                dbstr = OCI_StringGetOracleString(spfile, &dbsize);

                OCI_SET_ATTRIB(OCI_HTYPE_ADMIN, OCI_ATTR_ADMIN_PFILE, adm, dbstr, dbsize)

                OCI_StringReleaseOracleString(dbstr);
            }

            /* startup DB */

            OCI_EXEC(OCIDBStartup(con->cxt, con->err, (OCIAdmin *) adm, OCI_DEFAULT, start_flag))

            /* release security administration handle */

            if (adm)
            {
                OCI_HandleFree(OCILib.err, OCI_HTYPE_ADMIN);
            }

            /* disconnect */

            OCI_ConnectionFree(con);
        }
    }

    if (OCI_STATUS)
    {
        /* connect without preliminary mode */

        con = OCI_ConnectionCreate(db, user, pwd, sess_mode);

        OCI_STATUS = (NULL != con);

        /* alter database */

        if (OCI_STATUS)
        {
            OCI_Statement *stmt = OCI_StatementCreate(con);

            /* mount database */

            if (start_mode & OCI_DB_SPM_MOUNT)
            {
                OCI_STATUS = OCI_STATUS && OCI_ExecuteStmt(stmt, OTEXT("ALTER DATABASE MOUNT"));
            }

            /* open database */

            if (start_mode & OCI_DB_SPM_OPEN)
            {
                OCI_STATUS = OCI_STATUS && OCI_ExecuteStmt(stmt, OTEXT("ALTER DATABASE OPEN"));
            }

            OCI_StatementFree(stmt);

            /* disconnect */

            OCI_ConnectionFree(con);
        }
    }

#else

    OCI_NOT_USED(db)
    OCI_NOT_USED(user)
    OCI_NOT_USED(pwd)
    OCI_NOT_USED(sess_mode)
    OCI_NOT_USED(start_mode)
    OCI_NOT_USED(start_flag)
    OCI_NOT_USED(spfile)
    OCI_NOT_USED(con)

#endif

    call_retval = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DatabaseShutdown
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DatabaseShutdown
(
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int sess_mode,
    unsigned int shut_mode,
    unsigned int shut_flag
)
{
    OCI_Connection *con = NULL;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_REMOTE_DBS_CONTROL_ENABLED()

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* connect to server */

    con = OCI_ConnectionCreate(db, user, pwd, sess_mode);

    OCI_STATUS = (NULL != con);

    if (OCI_STATUS)
    {
        /* delete current transaction before the abort */

        if (con->trs && (OCI_DB_SDF_ABORT == shut_flag))
        {
            OCI_TransactionFree(con->trs);

            con->trs = NULL;
        }

        /* start shutdown */

        if (shut_mode & OCI_DB_SDM_SHUTDOWN)
        {
            /* start shutdown process */

            OCI_EXEC(OCIDBShutdown(con->cxt, con->err, (OCIAdmin *) NULL, shut_flag))
        }

        /* alter database if we are not in abort mode */

        if (OCI_STATUS && (OCI_DB_SDF_ABORT != shut_flag))
        {
            OCI_Statement *stmt = OCI_StatementCreate(con);

            /* close database */

            if (shut_mode & OCI_DB_SDM_CLOSE)
            {
                OCI_STATUS = OCI_STATUS && OCI_ExecuteStmt(stmt, OTEXT("ALTER DATABASE CLOSE NORMAL"));
            }

            /* unmount database */

            if (shut_mode & OCI_DB_SDM_DISMOUNT)
            {
                OCI_STATUS = OCI_STATUS && OCI_ExecuteStmt(stmt, OTEXT("ALTER DATABASE DISMOUNT"));
            }

            OCI_StatementFree(stmt);

            /* delete current transaction before the shutdown */

            if (con->trs)
            {
                OCI_TransactionFree(con->trs);

                con->trs = NULL;
            }

            /* do the final shutdown if we are not in abort mode */

            OCI_EXEC(OCIDBShutdown(con->cxt, con->err, (OCIAdmin *) 0, OCI_DBSHUTDOWN_FINAL))
        }

        /* disconnect */

        OCI_ConnectionFree(con);
    }

#else

    OCI_NOT_USED(db)
    OCI_NOT_USED(user)
    OCI_NOT_USED(pwd)
    OCI_NOT_USED(sess_mode)
    OCI_NOT_USED(shut_mode)
    OCI_NOT_USED(shut_flag)
    OCI_NOT_USED(con)

#endif

    call_retval = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetHAHandler
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetHAHandler
(
    POCI_HA_HANDLER  handler
)
{
    void *callback = NULL;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_INITIALIZED()
    OCI_CALL_CHECK_HIGH_AVAILABILITY_ENABLED()

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* On MSVC, casting a function pointer to a data pointer generates a warning.
       As there is no other to way to do regarding the OCI API, let's disable this
       warning just the time to set the callback attribute to the environment handle */

    #ifdef _MSC_VER
    #pragma warning(disable: 4054)
    #endif

    if (handler)
    {
        callback = (void*) OCI_ProcHAEvent;
    }

    OCI_SET_ATTRIB(OCI_HTYPE_ENV, OCI_ATTR_EVTCBK, OCILib.env, callback, 0)

    if (OCI_STATUS)
    {
        OCILib.ha_handler = handler;
    }

    #ifdef _MSC_VER
    #pragma warning(default: 4054)
    #endif

#else

    OCI_NOT_USED(callback)

#endif

    call_retval = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
* OCI_SetFormat
* --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetFormat
(
    OCI_Connection *con,
    unsigned int    type,
    const otext    *format
)
{
    otext **value = NULL;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_INITIALIZED()
    OCI_CALL_CHECK_ENUM_VALUE(con, NULL, type, FormatTypeValues, OTEXT("Format Type"))

    value = con ? &con->formats[type - 1] : &OCILib.formats[type - 1];

    OCI_FREE(*value)

   *value = ostrdup(format ? format : FormatDefaultValues[type-1]);

    call_retval = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
* OCI_GetFormat
* --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_GetFormat
(
    OCI_Connection *con,
    unsigned int    type
)
{
    otext **value = NULL;

    OCI_CALL_ENTER(const otext *, NULL)
    OCI_CALL_CHECK_INITIALIZED()
    OCI_CALL_CHECK_ENUM_VALUE(con, NULL, type, FormatTypeValues, OTEXT("Format Type"))

    value = con ? &con->formats[type-1] : &OCILib.formats[type-1];

    if (!*value)
    {
        OCI_SetFormat(con, type, NULL);
    }

    call_retval = *value;

    OCI_CALL_EXIT()
}
