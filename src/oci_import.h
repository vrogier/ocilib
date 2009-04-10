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
 * $Id: oci_import.h, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#ifndef OCILIB_OCI_IMPORT_H_INCLUDED
#define OCILIB_OCI_IMPORT_H_INCLUDED

#ifdef OCI_IMPORT_LINKAGE


/* this is necessary because some Oracle headers do NOT include C++ checks !
   By example, the file orid.h is not protected !
*/

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include <oci.h>
#include <orid.h>
#include <oci8dp.h>

#ifdef  __cplusplus
}
#endif

#ifdef _MSC_VER
#pragma comment(lib, "oci.lib")
#endif

#else

#include "oci_loader.h"
#include "oci_api.h"

/* Setup Oracle shared library name if not provided */

#ifndef OCI_DL
  #if defined(_WINDOWS)
    #define OCI_DL                   oci.dll
  #elif defined(__APPLE__)
    #define OCI_DL                   libclntsh.dylib
  #elif defined(__hppa)
    #define OCI_DL                   libclntsh.sl
  #else
    #define OCI_DL                   libclntsh.so
  #endif
#endif

#define OCI_DL_GET(s) OCI_DL_CVT(s)
#define OCI_DL_CVT(s) #s

#define OCI_DL_NAME OCI_DL_GET(OCI_DL)

/* symbol list */

extern OCIENVCREATE                 OCIEnvCreate;
extern OCISERVERATTACH              OCIServerAttach;
extern OCISERVERDETACH              OCIServerDetach;
extern OCIHANDLEALLOC               OCIHandleAlloc;
extern OCIHANDLEFREE                OCIHandleFree;
extern OCIDESCRIPTORALLOC           OCIDescriptorAlloc;
extern OCIDESCRIPTORFREE            OCIDescriptorFree;
extern OCISESSIONBEGIN              OCISessionBegin;
extern OCISESSIONEND                OCISessionEnd;
extern OCIPASSWORDCHANGE            OCIPasswordChange;
extern OCIBINDBYPOS                 OCIBindByPos;
extern OCIBINDBYNAME                OCIBindByName;
extern OCIBINDDYNAMIC               OCIBindDynamic;
extern OCIBINDOBJECT                OCIBindObject;
extern OCIDEFINEBYPOS               OCIDefineByPos;
extern OCIDEFINEOBJECT              OCIDefineObject;
extern OCISTMTPREPARE               OCIStmtPrepare;
extern OCISTMTEXECUTE               OCIStmtExecute;
extern OCISTMTFETCH                 OCIStmtFetch;
extern OCISTMTFETCH2                OCIStmtFetch2;
extern OCISTMTGETPIECEINFO          OCIStmtGetPieceInfo;
extern OCISTMTSETPIECEINFO          OCIStmtSetPieceInfo;
extern OCIPARAMGET                  OCIParamGet;
extern OCIPARAMSET                  OCIParamSet;
extern OCITRANSSTART                OCITransStart;
extern OCITRANSDETACH               OCITransDetach;
extern OCITRANSPREPARE              OCITransPrepare;
extern OCITRANSFORGET               OCITransForget;
extern OCITRANSCOMMIT               OCITransCommit;
extern OCITRANSROLLBACK             OCITransRollback;
extern OCIERRORGET                  OCIErrorGet;
extern OCILOBCREATETEMPORARY        OCILobCreateTemporary;
extern OCILOBFREETEMPORARY          OCILobFreeTemporary;
extern OCILOBISTEMPORARY            OCILobIsTemporary;
extern OCILOBAPPEND                 OCILobAppend;
extern OCILOBCOPY                   OCILobCopy;
extern OCILOBGETLENGTH              OCILobGetLength;
extern OCILOBREAD                   OCILobRead;
extern OCILOBWRITE                  OCILobWrite;
extern OCILOBTRIM                   OCILobTrim;
extern OCILOBERASE                  OCILobErase;
extern OCILOBOPEN                   OCILobOpen;
extern OCILOBCLOSE                  OCILobClose;
extern OCILOBLOCATORASSIGN          OCILobLocatorAssign;
extern OCILOBASSIGN                 OCILobAssign;
extern OCILOBISEQUAL                OCILobIsEqual;
extern OCILOBFILEOPEN               OCILobFileOpen;
extern OCILOBFILECLOSE              OCILobFileClose;
extern OCILOBFILECLOSEALL           OCILobFileCloseAll;
extern OCILOBFILEISOPEN             OCILobFileIsOpen;
extern OCILOBFILEEXISTS             OCILobFileExists;
extern OCILOBFIELGETNAME            OCILobFileGetName;
extern OCILOBFILESETNAME            OCILobFileSetName;
extern OCILOBLOADFROMFILE           OCILobLoadFromFile;
extern OCILOBWRITEAPPEND            OCILobWriteAppend;
extern OCISERVERVERSION             OCIServerVersion;
extern OCIBREAK                     OCIBreak;
extern OCIATTRGET                   OCIAttrGet;
extern OCIATTRSET                   OCIAttrSet;
extern OCIDATEASSIGN                OCIDateAssign;
extern OCIDATETOTEXT                OCIDateToText;
extern OCIDATEFROMTEXT              OCIDateFromText;
extern OCIDATECOMPARE               OCIDateCompare;
extern OCIDATEADDMONTHS             OCIDateAddMonths;
extern OCIDATEADDDAYS               OCIDateAddDays;
extern OCIDATELASTDAY               OCIDateLastDay;
extern OCIDATEDAYSBETWEEN           OCIDateDaysBetween;
extern OCIDATEZONETOZONE            OCIDateZoneToZone;
extern OCIDATENEXTDAY               OCIDateNextDay;
extern OCIDATECHECK                 OCIDateCheck;
extern OCIDATESYSDATE               OCIDateSysDate;
extern OCIDESCRIBEANY               OCIDescribeAny;
extern OCIINTERVALASSIGN            OCIIntervalAssign;
extern OCIINTERVALCHECK             OCIIntervalCheck;
extern OCIINTERVALCOMPARE           OCIIntervalCompare;
extern OCIINTERVALFROMTEXT          OCIIntervalFromText;
extern OCIINTERVALTOTEXT            OCIIntervalToText;
extern OCIINTERVALFROMTZ            OCIIntervalFromTZ;
extern OCIINTERVALGETDAYSECOND      OCIIntervalGetDaySecond;
extern OCIINTERVALGETYEARMONTH      OCIIntervalGetYearMonth;
extern OCIINTERVALSETDAYSECOND      OCIIntervalSetDaySecond;
extern OCIINTERVALSETYEARMONTH      OCIIntervalSetYearMonth;
extern OCIINTERVALSUBTRACT          OCIIntervalSubtract;
extern OCIINTERVALADD               OCIIntervalAdd;
extern OCIDATETIMEASSIGN            OCIDateTimeAssign;
extern OCIDATETIMECHECK             OCIDateTimeCheck;
extern OCIDATETIMECOMPARE           OCIDateTimeCompare;
extern OCIDATETIMECONSTRUCT         OCIDateTimeConstruct;
extern OCIDATETIMECONVERT           OCIDateTimeConvert;
extern OCIDATETIMEFROMARRAY         OCIDateTimeFromArray;
extern OCIDATETIMETOARRAY           OCIDateTimeToArray;
extern OCIDATETIMEFROMTEXT          OCIDateTimeFromText;
extern OCIDATETIMETOTEXT            OCIDateTimeToText;
extern OCIDATETIMEGETDATE           OCIDateTimeGetDate;
extern OCIDATETIMEGETTIME           OCIDateTimeGetTime;
extern OCIDATETIMEGETTIMEZONENAME   OCIDateTimeGetTimeZoneName;
extern OCIDATETIMEGETTIMEZONEOFFSET OCIDateTimeGetTimeZoneOffset;
extern OCIDATETIMEINTERVALADD       OCIDateTimeIntervalAdd;
extern OCIDATETIMEINTERVALSUB       OCIDateTimeIntervalSub;
extern OCIDATETIMESUBTRACT          OCIDateTimeSubtract;
extern OCIDATETIMESYSTIMESTAMP      OCIDateTimeSysTimeStamp;
extern OCIARRAYDESCRIPTORFREE       OCIArrayDescriptorFree;
extern OCICLIENTVERSION             OCIClientVersion;;
extern OCITYPEBYNAME                OCITypeByName;
extern OCINUMBERTOINT               OCINumberToInt;
extern OCINUMBERFROMINT             OCINumberFromInt;
extern OCINUMBERTOREAL              OCINumberToReal;
extern OCINUMBERFROMREAL            OCINumberFromReal;
extern OCINUMBERTOTEXT              OCINumberToText;
extern OCINUMBERFROMTEXT            OCINumberFromText;
extern OCISTRINGPTR                 OCIStringPtr;
extern OCISTRINGASSIGNTEXT          OCIStringAssignText;
extern OCIRAWPTR                    OCIRawPtr;
extern OCIRAWASSIGNBYTES            OCIRawAssignBytes;
extern OCIRAWALLOCSIZE              OCIRawAllocSize;
extern OCIOBJECTNEW                 OCIObjectNew;
extern OCIOBJECTFREE                OCIObjectFree;
extern OCIOBJECTSETATTR             OCIObjectSetAttr;
extern OCIOBJECTGETATTR             OCIObjectGetAttr;
extern OCIOBJECTPIN                 OCIObjectPin;
extern OCIOBJECTUNPIN               OCIObjectUnpin;
extern OCIOBJECTCOPY                OCIObjectCopy;
extern OCIOBJECTGETOBJECTREF        OCIObjectGetObjectRef;
extern OCIOBJECTGETPROPERTY         OCIObjectGetProperty;
extern OCIOBJECTGETIND              OCIObjectGetInd;
extern OCIREFASSIGN                 OCIRefAssign;
extern OCIREFISNULL                 OCIRefIsNull;
extern OCIREFCLEAR                  OCIRefClear;
extern OCIREFTOHEX                  OCIRefToHex;
extern OCIREFHEXSIZE                OCIRefHexSize;
extern OCITHREADPROCESSINIT         OCIThreadProcessInit;
extern OCITHREADINIT                OCIThreadInit;
extern OCITHREADTERM                OCIThreadTerm;
extern OCITHREADIDINIT              OCIThreadIdInit;
extern OCITHREADIDDESTROY           OCIThreadIdDestroy;
extern OCITHREADHNDINIT             OCIThreadHndInit;
extern OCITHREADHNDDESTROY          OCIThreadHndDestroy;
extern OCITHREADCREATE              OCIThreadCreate;
extern OCITHREADJOIN                OCIThreadJoin;
extern OCITHREADCLOSE               OCIThreadClose;
extern OCITHREADMUTEXINIT           OCIThreadMutexInit;
extern OCITHREADMUTEXDESTROY        OCIThreadMutexDestroy;
extern OCITHREADMUTEXACQUIRE        OCIThreadMutexAcquire;
extern OCITHREADMUTEXRELEASE        OCIThreadMutexRelease;
extern OCITHREADKEYINIT             OCIThreadKeyInit;
extern OCITHREADKEYDESTROY          OCIThreadKeyDestroy;
extern OCITHREADKEYSET              OCIThreadKeySet;
extern OCITHREADKEYGET              OCIThreadKeyGet;
extern OCICONNECTIONPOOLCREATE      OCIConnectionPoolCreate;
extern OCICONNECTIONPOOLDESTROY     OCIConnectionPoolDestroy;
extern OCICOLLSIZE                  OCICollSize;
extern OCICOLLMAX                   OCICollMax;
extern OCICOLLGETITEM               OCICollGetElem;
extern OCICOLLASSIGNELEM            OCICollAssignElem;
extern OCICOLLASSIGN                OCICollAssign;
extern OCICOLLAPPEND                OCICollAppend;
extern OCICOLLTRIM                  OCICollTrim;
extern OCIITERCREATE                OCIIterCreate;
extern OCIITERDELETE                OCIIterDelete;
extern OCIITERINIT                  OCIIterInit;
extern OCIITERNEXT                  OCIIterNext;
extern OCIITERPREV                  OCIIterPrev;

extern OCIDIRPATHABORT              OCIDirPathAbort;
extern OCIDIRPATHDATASAVE           OCIDirPathDataSave;
extern OCIDIRPATHFINISH             OCIDirPathFinish;
extern OCIDIRPATHPREPARE            OCIDirPathPrepare;
extern OCIDIRPATHLOADSTREAM         OCIDirPathLoadStream;
extern OCIDIRPATHCOLARRAYENTRYSET   OCIDirPathColArrayEntrySet;
extern OCIDIRPATHCOLARRAYRESET      OCIDirPathColArrayReset;
extern OCIDIRPATHCOLARRAYTOSTREAM   OCIDirPathColArrayToStream;
extern OCIDIRPATHSTREAMRESET        OCIDirPathStreamReset;
extern OCIDIRPATHFLUSHROW           OCIDirPathFlushRow;

extern OCICACHEFREE                 OCICacheFree;

#ifdef ORAXB8_DEFINED

extern OCILOBCOPY2                  OCILobCopy2;
extern OCILOBERASE2                 OCILobErase2;
extern OCILOBGETLENGTH2             OCILobGetLength2;
extern OCILOBLOADFROMFILE2          OCILobLoadFromFile2;
extern OCILOBREAD2                  OCILobRead2;
extern OCILOBTRIM2                  OCILobTrim2;
extern OCILOBWRITE2                 OCILobWrite2;
extern OCILOBWRITEAPPEND2           OCILobWriteAppend2;

#endif

#define OCIDateGetTime(date, hour, min, sec) \
  { \
     *hour = (date)->OCIDateTime.OCITimeHH; \
     *min = (date)->OCIDateTime.OCITimeMI; \
     *sec = (date)->OCIDateTime.OCITimeSS; \
  }

#define OCIDateGetDate(date, year, month, day) \
  { \
     *year = (date)->OCIDateYYYY; \
     *month = (date)->OCIDateMM; \
     *day = (date)->OCIDateDD; \
  }

#define OCIDateSetTime(date, hour, min, sec) \
  { \
     (date)->OCIDateTime.OCITimeHH = hour; \
     (date)->OCIDateTime.OCITimeMI = min; \
     (date)->OCIDateTime.OCITimeSS = sec; \
  }

#define OCIDateSetDate(date, year, month, day) \
  { \
     (date)->OCIDateYYYY = year; \
     (date)->OCIDateMM = month; \
     (date)->OCIDateDD = day; \
  }

#endif

#endif    /* OCILIB_OCI_IMPORT_H_INCLUDED */

