/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2010 Vincent ROGIER <vince.rogier@ocilib.net>            |
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
 * $Id: library.c, v 3.8.1 2010-12-13 00:00 Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             INTERNAL VARIABLES
 * ********************************************************************************************* */
OCI_Library OCILib;

OCI_SQLCmdInfo SQLCmds[OCI_SQLCMD_COUNT] =
{
    {OCI_SFC_CREATE_TABLE,             MT("CREATE TABLE")                                         },
    {OCI_SFC_SET_ROLE,                 MT("SET ROLE")                                             },
    {OCI_SFC_INSERT,                   MT("INSERT")                                               },
    {OCI_SFC_SELECT,                   MT("SELECT")                                               },
    {OCI_SFC_UPDATE,                   MT("UPDATE")                                               },
    {OCI_SFC_DROP_ROLE,                MT("DROP ROLE")                                            },
    {OCI_SFC_DROP_VIEW,                MT("DROP VIEW")                                            },
    {OCI_SFC_DROP_TABLE,               MT("DROP TABLE")                                           },
    {OCI_SFC_DELETE,                   MT("DELETE")                                               },
    {OCI_SFC_CREATE_VIEW,              MT("CREATE VIEW")                                          },
    {OCI_SFC_DROP_USER,                MT("DROP USER")                                            },
    {OCI_SFC_CREATE_ROLE,              MT("CREATE ROLE")                                          },
    {OCI_SFC_CREATE_SEQUENCE,          MT("CREATE SEQUENCE")                                      },
    {OCI_SFC_ALTER_SEQUENCE,           MT("ALTER SEQUENCE")                                       },
    {OCI_SFC_DROP_SEQUENCE,            MT("DROP SEQUENCE")                                        },
    {OCI_SFC_CREATE_SCHEMA,            MT("CREATE SCHEMA")                                        },
    {OCI_SFC_CREATE_CLUSTER,           MT("CREATE CLUSTER")                                       },
    {OCI_SFC_CREATE_USER,              MT("CREATE USER")                                          },
    {OCI_SFC_CREATE_INDEX,             MT("CREATE INDEX")                                         },
    {OCI_SFC_DROP_INDEX,               MT("DROP INDEX")                                           },
    {OCI_SFC_DROP_CLUSTER,             MT("DROP CLUSTER")                                         },
    {OCI_SFC_VALIDATE_INDEX,           MT("VALIDATE INDEX")                                       },
    {OCI_SFC_CREATE_PROCEDURE,         MT("CREATE PROCEDURE")                                     },
    {OCI_SFC_ALTER_PROCEDURE,          MT("ALTER PROCEDURE")                                      },
    {OCI_SFC_ALTER_TABLE,              MT("ALTER TABLE")                                          },
    {OCI_SFC_EXPLAIN,                  MT("EXPLAIN")                                              },
    {OCI_SFC_GRANT,                    MT("GRANT")                                                },
    {OCI_SFC_REVOKE,                   MT("REVOKE")                                               },
    {OCI_SFC_CREATE_SYNONYM,           MT("CREATE SYNONYM")                                       },
    {OCI_SFC_DROP_SYNONYM,             MT("DROP SYNONYM")                                         },
    {OCI_SFC_ALTER_SYSTEM_SWITCHLOG,   MT("ALTER SYSTEM SWITCHLOG")                               },
    {OCI_SFC_SET_TRANSACTION,          MT("SET TRANSACTION")                                      },
    {OCI_SFC_PLSQL_EXECUTE,            MT("PL/SQL EXECUTE")                                       },
    {OCI_SFC_LOCK,                     MT("LOCK")                                                 },
    {OCI_SFC_NOOP,                     MT("NOOP")                                                 },
    {OCI_SFC_RENAME,                   MT("RENAME")                                               },
    {OCI_SFC_COMMENT,                  MT("COMMENT")                                              },
    {OCI_SFC_AUDIT,                    MT("AUDIT")                                                },
    {OCI_SFC_NO_AUDIT,                 MT("NO AUDIT")                                             },
    {OCI_SFC_ALTER_INDEX,              MT("ALTER INDEX")                                          },
    {OCI_SFC_CREATE_EXTERNAL_DATABASE, MT("CREATE EXTERNAL DATABASE")                             },
    {OCI_SFC_DROP_EXTERNALDATABASE,    MT("DROP EXTERNALDATABASE")                                },
    {OCI_SFC_CREATE_DATABASE,          MT("CREATE DATABASE")                                      },
    {OCI_SFC_ALTER_DATABASE,           MT("ALTER DATABASE")                                       },
    {OCI_SFC_CREATE_ROLLBACK_SEGMENT,  MT("CREATE ROLLBACK SEGMENT")                              },
    {OCI_SFC_ALTER_ROLLBACK_SEGMENT,   MT("ALTER ROLLBACK SEGMENT")                               },
    {OCI_SFC_DROP_ROLLBACK_SEGMENT,    MT("DROP ROLLBACK SEGMENT")                                },
    {OCI_SFC_CREATE_TABLESPACE,        MT("CREATE TABLESPACE")                                    },
    {OCI_SFC_ALTER_TABLESPACE,         MT("ALTER TABLESPACE")                                     },
    {OCI_SFC_DROP_TABLESPACE,          MT("DROP TABLESPACE")                                      },
    {OCI_SFC_ALTER_SESSION,            MT("ALTER SESSION")                                        },
    {OCI_SFC_ALTER_USER,               MT("ALTER USER")                                           },
    {OCI_SFC_COMMIT_WORK,              MT("COMMIT (WORK)")                                        },
    {OCI_SFC_ROLLBACK,                 MT("ROLLBACK")                                             },
    {OCI_SFC_SAVEPOINT,                MT("SAVEPOINT")                                            },
    {OCI_SFC_CREATE_CONTROL_FILE,      MT("CREATE CONTROL FILE")                                  },
    {OCI_SFC_ALTER_TRACING,            MT("ALTER TRACING")                                        },
    {OCI_SFC_CREATE_TRIGGER,           MT("CREATE TRIGGER")                                       },
    {OCI_SFC_ALTER_TRIGGER,            MT("ALTER TRIGGER")                                        },
    {OCI_SFC_DROP_TRIGGER,             MT("DROP TRIGGER")                                         },
    {OCI_SFC_ANALYZE_TABLE,            MT("ANALYZE TABLE")                                        },
    {OCI_SFC_ANALYZE_INDEX,            MT("ANALYZE INDEX")                                        },
    {OCI_SFC_ANALYZE_CLUSTER,          MT("ANALYZE CLUSTER")                                      },
    {OCI_SFC_CREATE_PROFILE,           MT("CREATE PROFILE")                                       },
    {OCI_SFC_DROP_PROFILE,             MT("DROP PROFILE")                                         },
    {OCI_SFC_ALTER_PROFILE,            MT("ALTER PROFILE")                                        },
    {OCI_SFC_DROP_PROCEDURE,           MT("DROP PROCEDURE")                                       },
    {OCI_SFC_ALTER_RESOURCE_COST,      MT("ALTER RESOURCE COST")                                  },
    {OCI_SFC_CREATE_SNAPSHOT_LOG,      MT("CREATE SNAPSHOT LOG")                                  },
    {OCI_SFC_ALTER_SNAPSHOT_LOG,       MT("ALTER SNAPSHOT LOG")                                   },
    {OCI_SFC_DROP_SNAPSHOT_LOG,        MT("DROP SNAPSHOT LOG")                                    },
    {OCI_SFC_DROP_SUMMARY,             MT("DROP SUMMARY")                                         },
    {OCI_SFC_CREATE_SNAPSHOT,          MT("CREATE SNAPSHOT")                                      },
    {OCI_SFC_ALTER_SNAPSHOT,           MT("ALTER SNAPSHOT")                                       },
    {OCI_SFC_DROP_SNAPSHOT,            MT("DROP SNAPSHOT")                                        },
    {OCI_SFC_CREATE_TYPE,              MT("CREATE TYPE")                                          },
    {OCI_SFC_DROP_TYPE,                MT("DROP TYPE")                                            },
    {OCI_SFC_ALTER_ROLE,               MT("ALTER ROLE")                                           },
    {OCI_SFC_ALTER_TYPE,               MT("ALTER TYPE")                                           },
    {OCI_SFC_CREATE_TYPE_BODY,         MT("CREATE TYPE BODY")                                     },
    {OCI_SFC_ALTER_TYPE_BODY,          MT("ALTER TYPE BODY")                                      },
    {OCI_SFC_DROP_TYPE_BODY,           MT("DROP TYPE BODY")                                       },
    {OCI_SFC_DROP_LIBRARY,             MT("DROP LIBRARY")                                         },
    {OCI_SFC_TRUNCATE_TABLE,           MT("TRUNCATE TABLE")                                       },
    {OCI_SFC_TRUNCATE_CLUSTER,         MT("TRUNCATE CLUSTER")                                     },
    {OCI_SFC_CREATE_BITMAPFILE,        MT("CREATE BITMAPFILE")                                    },
    {OCI_SFC_ALTER_VIEW,               MT("ALTER VIEW")                                           },
    {OCI_SFC_DROP_BITMAPFILE,          MT("DROP BITMAPFILE")                                      },
    {OCI_SFC_SET_CONSTRAINTS,          MT("SET CONSTRAINTS")                                      },
    {OCI_SFC_CREATE_FUNCTION,          MT("CREATE FUNCTION")                                      },
    {OCI_SFC_ALTER_FUNCTION,           MT("ALTER FUNCTION")                                       },
    {OCI_SFC_DROP_FUNCTION,            MT("DROP FUNCTION")                                        },
    {OCI_SFC_CREATE_PACKAGE,           MT("CREATE PACKAGE")                                       },
    {OCI_SFC_ALTER_PACKAGE,            MT("ALTER PACKAGE")                                        },
    {OCI_SFC_DROP_PACKAGE,             MT("DROP PACKAGE")                                         },
    {OCI_SFC_CREATE_PACKAGE_BODY,      MT("CREATE PACKAGE BODY")                                  },
    {OCI_SFC_ALTER_PACKAGE_BODY,       MT("ALTER PACKAGE BODY")                                   },
    {OCI_SFC_DROP_PACKAGE_BODY,        MT("DROP PACKAGE BODY")                                    },
    {OCI_SFC_CREATE_DIRECTORY,         MT("CREATE DIRECTORY")                                     },
    {OCI_SFC_DROP_DIRECTORY,           MT("DROP DIRECTORY")                                       },
    {OCI_SFC_CREATE_LIBRARY,           MT("CREATE LIBRARY")                                       },
    {OCI_SFC_CREATE_JAVA,              MT("CREATE JAVA")                                          },
    {OCI_SFC_ALTER_JAVA,               MT("ALTER JAVA")                                           },
    {OCI_SFC_DROP_JAVA,                MT("DROP JAVA")                                            },
    {OCI_SFC_CREATE_OPERATOR,          MT("CREATE OPERATOR")                                      },
    {OCI_SFC_CREATE_INDEXTYPE,         MT("CREATE INDEXTYPE")                                     },
    {OCI_SFC_DROP_INDEXTYPE,           MT("DROP INDEXTYPE")                                       },
    {OCI_SFC_ALTER_INDEXTYPE,          MT("ALTER INDEXTYPE")                                      },
    {OCI_SFC_DROP_OPERATOR,            MT("DROP OPERATOR")                                        },
    {OCI_SFC_ASSOCIATE_STATISTICS,     MT("ASSOCIATE STATISTICS")                                 },
    {OCI_SFC_DISASSOCIATE_STATISTICS,  MT("DISASSOCIATE STATISTICS")                              },
    {OCI_SFC_CALL_METHOD,              MT("CALL METHOD")                                          },
    {OCI_SFC_CREATE_SUMMARY,           MT("CREATE SUMMARY")                                       },
    {OCI_SFC_ALTER_SUMMARY,            MT("ALTER SUMMARY")                                        },
    {OCI_SFC_CREATE_DIMENSION,         MT("CREATE DIMENSION")                                     },
    {OCI_SFC_ALTER_DIMENSION,          MT("ALTER DIMENSION")                                      },
    {OCI_SFC_DROP_DIMENSION,           MT("DROP DIMENSION")                                       },
    {OCI_SFC_CREATE_CONTEXT,           MT("CREATE CONTEXT")                                       },
    {OCI_SFC_DROP_CONTEXT,             MT("DROP CONTEXT")                                         },
    {OCI_SFC_ALTER_OUTLINE,            MT("ALTER OUTLINE")                                        },
    {OCI_SFC_CREATE_OUTLINE,           MT("CREATE OUTLINE")                                       },
    {OCI_SFC_DROP_OUTLINE,             MT("DROP OUTLINE")                                         },
    {OCI_SFC_UPDATE_INDEXES,           MT("UPDATE INDEXES")                                       },
    {OCI_SFC_ALTER_OPERATOR,           MT("ALTER OPERATOR")                                       }
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
OCITYPEBYNAME                OCITypeByName                = NULL;
OCINUMBERTOINT               OCINumberToInt               = NULL;
OCINUMBERFROMINT             OCINumberFromInt             = NULL;
OCINUMBERTOREAL              OCINumberToReal              = NULL;
OCINUMBERFROMREAL            OCINumberFromReal            = NULL;
OCINUMBERTOTEXT              OCINumberToText              = NULL;
OCINUMBERFROMTEXT            OCINumberFromText            = NULL;
OCISTRINGPTR                 OCIStringPtr                 = NULL;
OCISTRINGASSIGNTEXT          OCIStringAssignText          = NULL;
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

  #ifdef ORAXB8_DEFINED

OCILOBCOPY2                  OCILobCopy2                  = NULL;
OCILOBERASE2                 OCILobErase2                 = NULL;
OCILOBGETLENGTH2             OCILobGetLength2             = NULL;
OCILOBLOADFROMFILE2          OCILobLoadFromFile2          = NULL;
OCILOBREAD2                  OCILobRead2                  = NULL;
OCILOBTRIM2                  OCILobTrim2                  = NULL;
OCILOBWRITE2                 OCILobWrite2                = NULL;
OCILOBWRITEAPPEND2           OCILobWriteAppend2          = NULL;

  #endif

#endif

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

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
    int i, n, nb_err = 0;

    OCI_CHECK(OCILib.key_map == NULL, TRUE)

    n = OCI_HashGetSize(OCILib.key_map);

    for (i = 0; i < n; i++)
    {
        e = OCI_HashGetEntry(OCILib.key_map, i);

        while (e != NULL)
        {
            v = e->values;

            while (v != NULL)
            {
                if (FALSE == OCI_ThreadKeyFree((OCI_ThreadKey *) (v->value.p_void)))
                    nb_err++;

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
 * OCI_SetStatus
 * --------------------------------------------------------------------------------------------- */

void OCI_SetStatus
(
    boolean res
)
{
    OCI_Error *err = OCI_ErrorGet(FALSE, FALSE);

    if (err != NULL)
    {
        err->raise = (res == FALSE);
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
    const mtext *lib_path,
    unsigned int mode
)
{
    boolean res  = TRUE;
    ub4 oci_mode = OCI_ENV_MODE | OCI_OBJECT;

    #ifdef OCI_IMPORT_RUNTIME

    char path[OCI_SIZE_BUFFER+1];

    size_t len = (size_t) 0;

    #endif

    /* check if it was already initialized */

    if (OCILib.loaded == TRUE)
        return TRUE;

    memset(&OCILib, 0, sizeof(OCI_Library));

    OCILib.error_handler = err_handler;

    OCILib.version_compile = OCI_VERSION_COMPILE;
    OCILib.version_runtime = OCI_VERSION_RUNTIME;

    OCILib.env_mode = mode;

    #ifdef OCI_CHARSET_ANSI

    /* test for UTF8 environment */

    {
        char *str = getenv("NLS_LANG");

        if (str != NULL)
        {
            char nls_lang[OCI_SIZE_OBJ_NAME+1] = "";

            strncat(nls_lang, str, OCI_SIZE_OBJ_NAME);

            for (str = nls_lang; *str != 0; str++)
                *str = (char) toupper(*str);

            OCILib.nls_utf8 = (strstr(nls_lang, "UTF8") != NULL);
        }
    }

    #endif

    #ifdef OCI_IMPORT_LINKAGE

    OCI_NOT_USED(lib_path);

    #if defined(OCI_BIG_UINT_ENABLED)

    OCILib.use_lob_ub8 = TRUE;

    #endif

    #if defined(OCI_STMT_SCROLLABLE_READONLY)

    OCILib.use_scrollable_cursors = TRUE;

    #endif

    #else

    memset(path, 0, sizeof(path));

    #if defined(OCI_CHARSET_WIDE)

    if (lib_path != NULL && lib_path[0] != 0)
        len = wcstombs(path, lib_path, sizeof(path));

    #else

    if (lib_path != NULL && lib_path[0] != 0)
    {
        strncat(path, lib_path, sizeof(path));

        len = strlen(path);
    }

    #endif

    if ((len > (size_t) 0)   &&
        (len < sizeof(path)) &&
        (path[len - (size_t) 1] != OCI_CHAR_SLASH))
    {
        path[len] = OCI_CHAR_SLASH;
        len++;
    }

    strncat(path, OCI_DL_ANSI_NAME, sizeof(path) - len);

    OCILib.lib_handle = LIB_OPEN(path);

    if (OCILib.lib_handle != NULL)
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

        LIB_SYMBOL(OCILib.lib_handle, "OCITypeByName", OCITypeByName,
                   OCITYPEBYNAME);

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

        LIB_SYMBOL(OCILib.lib_handle, "OCIStringPtr", OCIStringPtr,
                   OCISTRINGPTR);
        LIB_SYMBOL(OCILib.lib_handle, "OCIStringAssignText", OCIStringAssignText,
                   OCISTRINGASSIGNTEXT);

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

        /* API Version checking */

        if (OCIArrayDescriptorFree != NULL)
        {
            OCILib.version_runtime = OCI_11_1;
        }
        else if (OCIClientVersion != NULL)
        {
            OCILib.version_runtime = OCI_10_2;
        }
        else if (OCILobWrite2 != NULL)
        {
            OCILib.version_runtime = OCI_10_1;
        }
        else if (OCIStmtPrepare2  != NULL)
        {
            OCILib.version_runtime = OCI_9_2;
        }
        else if (OCIDateTimeGetTimeZoneName != NULL)
        {
            OCILib.version_runtime = OCI_9_0;
        }
        else if (OCIThreadProcessInit != NULL)
        {
            OCILib.version_runtime = OCI_8_1;
        }
        else if (OCIEnvCreate != NULL)
        {
            OCILib.version_runtime = OCI_8_0;
        }
        else
        {
            LIB_CLOSE(OCILib.lib_handle);

            OCI_ExceptionLoadingSymbols();

            res = FALSE;
        }
    }
    else
    {
        OCI_ExceptionLoadingSharedLib();

        res = FALSE;
    }

    if (res == TRUE)
    {

        #if defined(OCI_BIG_UINT_ENABLED)

        if ((OCILib.version_runtime >= OCI_10_1) && (OCILobCopy2 != NULL))
        {
            OCILib.use_lob_ub8 = TRUE;
        }

        #endif

        #if defined(OCI_STMT_SCROLLABLE_READONLY)

        if ((OCILib.version_runtime >= OCI_9_0) && (OCIStmtFetch2 != NULL))
        {
            OCILib.use_scrollable_cursors = TRUE;
        }

        #endif

    }

    #endif

    #if defined(OCI_CHARSET_WIDE)

    /* Oracle 8i does not support full Unicode mode */

    if ((res == TRUE) && (OCILib.version_runtime < OCI_9_0))
    {
        OCI_ExceptionNotAvailable(NULL, OCI_FEATURE_WIDE_USERDATA);

        res = FALSE;
    }

    #endif

    /* Initialize OCI environment */

    if (res == TRUE)
    {
        /* check modes */

        if (mode & OCI_ENV_THREADED)
            oci_mode |= OCI_THREADED;

        if (mode & OCI_ENV_EVENTS)
            oci_mode |= OCI_EVENTS;

        /* create environment on success */

        res = res && (OCI_SUCCESS == OCIEnvCreate(&OCILib.env, oci_mode,
                                                  (dvoid *) NULL, NULL, NULL, NULL,
                                                  (size_t) 0, (dvoid **) NULL));

        if (res == FALSE)
        {
            OCI_ExceptionOCIEnvironment();
        }

        /*  allocate error handle */

        res = res && (OCI_SUCCESS == OCI_HandleAlloc((dvoid *) OCILib.env,
                                                     (dvoid **) (void *) &OCILib.err,
                                                     (ub4) OCI_HTYPE_ERROR,
                                                     (size_t) 0, (dvoid **) NULL));
    }

    /* on success, we need to initialize OCIThread object support */

    if (res == TRUE)
    {
        if (OCI_LIB_THREADED)
        {
            OCIThreadProcessInit();

            res = (OCI_SUCCESS == OCIThreadInit(OCILib.env, OCILib.err));
        }

        /* create thread key for thread errors */

        OCILib.key_errs = OCI_ThreadKeyCreateInternal((POCI_THREADKEYDEST) OCI_ErrorFree);

        /* allocate connections internal list */

        if (res == TRUE)
        {
            OCILib.cons = OCI_ListCreate(OCI_IPC_CONNECTION);

            res = (OCILib.cons != NULL);
        }

        /* allocate pools internal list */

        if (res == TRUE)
        {

            OCILib.pools = OCI_ListCreate(OCI_IPC_POOL);

            res = (OCILib.pools != NULL);
        }

        #if OCI_VERSION_COMPILE >= OCI_10_2

        /* allocate connection pools internal list */

        if (res == TRUE)
        {

            OCILib.subs = OCI_ListCreate(OCI_IPC_NOTIFY);

            res = (OCILib.subs != NULL);
        }
        #endif

        if (res == TRUE)
        {

            OCILib.arrs = OCI_ListCreate(OCI_IPC_ARRAY);

            res = (OCILib.arrs != NULL);
        }
    }

    if (res == TRUE)
        OCILib.loaded = TRUE;

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Cleanup
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Cleanup
(
    void
)
{
    boolean res = TRUE;

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

    /* finalize OCIThread object support */

    if (OCI_LIB_THREADED)
    {
        OCI_CALL0
        (
            res, OCILib.err,

            OCIThreadTerm(OCILib.env, OCILib.err)
        )
    }

    /* free error thread key */

    if (OCILib.key_errs != NULL)
    {
        OCI_ThreadKey *key = OCILib.key_errs;
        OCI_Error *err     = OCI_ErrorGet(FALSE, FALSE);

        OCILib.key_errs = NULL;

        OCI_ErrorFree(err);
        OCI_ThreadKeySet(key, NULL);
        OCI_ThreadKeyFree(key);
    }

    /* set unloaded flag */

    OCILib.loaded = FALSE;

    /* close error handle */

    if (OCILib.err != NULL)
        OCI_HandleFree(OCILib.err, OCI_HTYPE_ERROR);

    /* close environment handle
       => direct OCIHandleFree() because this handle was not allocated
       with OCI_HandleAlloc()
    */

    if (OCILib.env != NULL)
        OCIHandleFree(OCILib.env, OCI_HTYPE_ENV);

    #ifdef OCI_IMPORT_RUNTIME

    if (OCILib.lib_handle != NULL)
        LIB_CLOSE(OCILib.lib_handle);

    #endif

    /* checks for unfreed handles */

    if (OCILib.nb_hndlp > 0)
    {
        OCI_ExceptionUnfreedData(OCI_HDLE_HANDLE, OCILib.nb_hndlp);
        res = FALSE;
    }

    /* checks for unfreed descriptors */

    if (OCILib.nb_descp > 0)
    {
        OCI_ExceptionUnfreedData(OCI_HDLE_DESCRIPTOR, OCILib.nb_descp);
        res = FALSE;
    }

    /* checks for unfreed objects */

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
    return (unsigned int) OCILib.version_compile;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetOCIRuntimeVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetOCIRuntimeVersion
(
    void
)
{
    return (unsigned int) OCILib.version_runtime;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetImportMode
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetImportMode
(
    void
)
{
    return (unsigned int) OCI_IMPORT_MODE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetCharsetMetaData
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetCharsetMetaData
(
    void
)
{
    return (unsigned int) OCI_CHAR_MTEXT;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetCharsetUserData
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetCharsetUserData
(
    void
)
{
    return (unsigned int) OCI_CHAR_DTEXT;
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

    if ((OCILib.loaded == FALSE) || (OCI_LIB_CONTEXT))
    {
        err = OCI_ErrorGet(TRUE, FALSE);

        if (err != NULL)
        {
            if (err->raise == FALSE)
                err = NULL;
        }
    }

    return err;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EnableWarnings
 * --------------------------------------------------------------------------------------------- */

void OCI_API OCI_EnableWarnings
(
    boolean value
)
{
    OCILib.warnings_on = value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetErrorHandler
 * --------------------------------------------------------------------------------------------- */

void OCI_API OCI_SetErrorHandler
(
    POCI_ERROR handler
)
{
    OCILib.error_handler = handler;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DatabaseStartup
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DatabaseStartup
(
    const mtext *db,
    const mtext *user,
    const mtext *pwd,
    unsigned int sess_mode,
    unsigned int start_mode,
    unsigned int start_flag,
    const mtext *spfile
)
{
    boolean res         = TRUE;
    OCI_Connection *con = NULL;

    OCI_CHECK_REMOTE_DBS_CONTROL_ENABLED(FALSE);

    #if OCI_VERSION_COMPILE >= OCI_10_2

    if (start_mode & OCI_DB_SPM_START)
    {
        OCIAdmin *adm = NULL;

        /* connect with prelim authenfication mode */

        con = OCI_ConnectionCreate(db, user, pwd, sess_mode | OCI_PRELIM_AUTH);

        if (con != NULL)
        {
            if ((res == TRUE) && (spfile != NULL) && (spfile[0] != 0))
            {
                void *ostr = NULL;
                int osize  = -1;

                /* allocate admin handle */

                res = (OCI_SUCCESS == OCI_HandleAlloc((dvoid *) OCILib.env,
                                                      (dvoid **) (void *) &adm,
                                                      (ub4) OCI_HTYPE_ADMIN,
                                                      (size_t) 0, (dvoid **) NULL));

                /* set client spfile if provided */

                ostr = OCI_GetInputMetaString(spfile, &osize);

                OCI_CALL2
                (
                    res, con,

                    OCIAttrSet((dvoid *) adm, (ub4) OCI_HTYPE_ADMIN,
                               (dvoid *) ostr, (ub4) osize,
                               (ub4) OCI_ATTR_ADMIN_PFILE, con->err)
                )

                OCI_ReleaseMetaString(ostr);
            }

            /* startup DB */

            OCI_CALL2
            (
                res, con,

                OCIDBStartup(con->cxt, con->err, (OCIAdmin *) adm,
                             OCI_DEFAULT, start_flag)
            )

            /* release security admin handle */

            if (adm != NULL)
            {
                OCI_HandleFree(OCILib.err, OCI_HTYPE_ADMIN);
            }

            /* disconnect */

            OCI_ConnectionFree(con);
        }
        else
            res = FALSE;
    }

    if (res == TRUE)
    {
        /* connect without prelim mode */

        con = OCI_ConnectionCreate(db, user, pwd, sess_mode);

        /* alter database */

        if (con != NULL)
        {
            OCI_Statement *stmt = OCI_StatementCreate(con);

            /* mount database */

            if (start_mode & OCI_DB_SPM_MOUNT)
                res = (res && OCI_ExecuteStmt(stmt, MT("ALTER DATABASE MOUNT")));

            /* open database */

            if (start_mode & OCI_DB_SPM_OPEN)
                res = (res && OCI_ExecuteStmt(stmt, MT("ALTER DATABASE OPEN")));

            OCI_StatementFree(stmt);

            /* disconnect */

            OCI_ConnectionFree(con);
        }
        else
            res = FALSE;
    }

    #else

    res = FALSE;

    OCI_NOT_USED(db);
    OCI_NOT_USED(user);
    OCI_NOT_USED(pwd);
    OCI_NOT_USED(sess_mode);
    OCI_NOT_USED(start_mode);
    OCI_NOT_USED(start_flag);
    OCI_NOT_USED(spfile);
    OCI_NOT_USED(con);

    #endif

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DatabaseShutdown
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DatabaseShutdown
(
    const mtext *db,
    const mtext *user,
    const mtext *pwd,
    unsigned int sess_mode,
    unsigned int shut_mode,
    unsigned int shut_flag
)
{
    boolean res         = TRUE;
    OCI_Connection *con = NULL;

    OCI_CHECK_REMOTE_DBS_CONTROL_ENABLED(FALSE);

    #if OCI_VERSION_COMPILE >= OCI_10_2

    /* connect to server */

    con = OCI_ConnectionCreate(db, user, pwd, sess_mode);

    if (con != NULL)
    {
        /* delete current transaction before the abort */

        if ((con->trs != NULL) && (shut_flag == OCI_DB_SDF_ABORT))
        {
            OCI_TransactionFree(con->trs);

            con->trs = NULL;
        }

        /* start shutdown */

        if (shut_mode & OCI_DB_SDM_SHUTDOWN)
        {
            /* start shutdown process */

            OCI_CALL2
            (
                res, con,

                OCIDBShutdown(con->cxt, con->err, (OCIAdmin *) NULL, shut_flag)
            )
        }

        /* alter database if we are not in abort mode */

        if ((res == TRUE) && (shut_flag != OCI_DB_SDF_ABORT))
        {
            OCI_Statement *stmt = OCI_StatementCreate(con);

            /* close database */

            if (shut_mode & OCI_DB_SDM_CLOSE)
                res = (res && OCI_ExecuteStmt(stmt, MT("ALTER DATABASE CLOSE NORMAL")));

            /* unmount database */

            if (shut_mode & OCI_DB_SDM_DISMOUNT)
                res = (res && OCI_ExecuteStmt(stmt,MT( "ALTER DATABASE DISMOUNT")));

            OCI_StatementFree(stmt);

            /* delete current transaction before the shutdown */

            if (con->trs != NULL)
            {
                OCI_TransactionFree(con->trs);

                con->trs = NULL;
            }

            /* do the final shutdown if we are not in abort mode */

            OCI_CALL2
            (
                res, con,

                OCIDBShutdown(con->cxt, con->err, (OCIAdmin *) 0, OCI_DBSHUTDOWN_FINAL)
            )
        }

        /* disconnect */

        OCI_ConnectionFree(con);
    }
    else
        res = FALSE;

    #else

    res = FALSE;

    OCI_NOT_USED(db);
    OCI_NOT_USED(user);
    OCI_NOT_USED(pwd);
    OCI_NOT_USED(sess_mode);
    OCI_NOT_USED(shut_mode);
    OCI_NOT_USED(shut_flag);
    OCI_NOT_USED(con);

    #endif

    OCI_RESULT(res);

    return res;
}
