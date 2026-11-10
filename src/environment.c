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

#include "environment.h"

#include "array.h"
#include "callback.h"
#include "connection.h"
#include "error.h"
#include "hash.h"
#include "list.h"
#include "log.h"
#include "macros.h"
#include "pool.h"
#include "stringutils.h"
#include "subscription.h"

WARNING_DISABLE_CAST_FUNC_TYPE

const char * EnvironmentVarNames[OCI_VARS_COUNT] =
{
    VAR_OCILIB_WORKAROUND_UTF16_COLUMN_NAME
};

const OCI_SQLCmdInfo SQLCmds[OCI_SQLCMD_COUNT] =
{
    { OCI_UNKNOWN, OTEXT("UNKNOWN") },
    { OCI_SFC_CREATE_TABLE, OTEXT("CREATE TABLE") },
    { OCI_SFC_INSERT, OTEXT("INSERT") },
    { OCI_SFC_SELECT, OTEXT("SELECT") },
    { OCI_SFC_CREATE_CLUSTER, OTEXT("CREATE CLUSTER") },
    { OCI_SFC_ALTER_CLUSTER, OTEXT("ALTER CLUSTER") },
    { OCI_SFC_UPDATE, OTEXT("UPDATE") },
    { OCI_SFC_DELETE, OTEXT("DELETE") },
    { OCI_SFC_DROP_CLUSTER, OTEXT("DROP CLUSTER") },
    { OCI_SFC_CREATE_INDEX, OTEXT("CREATE INDEX") },
    { OCI_SFC_DROP_INDEX, OTEXT("DROP INDEX") },
    { OCI_SFC_ALTER_INDEX, OTEXT("ALTER INDEX") },
    { OCI_SFC_DROP_TABLE, OTEXT("DROP TABLE") },
    { OCI_SFC_CREATE_SEQUENCE, OTEXT("CREATE SEQUENCE") },
    { OCI_SFC_ALTER_SEQUENCE, OTEXT("ALTER SEQUENCE") },
    { OCI_SFC_ALTER_TABLE, OTEXT("ALTER TABLE") },
    { OCI_SFC_DROP_SEQUENCE, OTEXT("DROP SEQUENCE") },
    { OCI_SFC_GRANT_OBJECT, OTEXT("GRANT OBJECT") },
    { OCI_SFC_REVOKE_OBJECT, OTEXT("REVOKE OBJECT") },
    { OCI_SFC_CREATE_SYNONYM, OTEXT("CREATE SYNONYM") },
    { OCI_SFC_DROP_SYNONYM, OTEXT("DROP SYNONYM") },
    { OCI_SFC_CREATE_VIEW, OTEXT("CREATE VIEW") },
    { OCI_SFC_DROP_VIEW, OTEXT("DROP VIEW") },
    { OCI_SFC_VALIDATE_INDEX, OTEXT("VALIDATE INDEX") },
    { OCI_SFC_CREATE_PROCEDURE, OTEXT("CREATE PROCEDURE") },
    { OCI_SFC_ALTER_PROCEDURE, OTEXT("ALTER PROCEDURE") },
    { OCI_SFC_LOCK, OTEXT("LOCK") },
    { OCI_SFC_NO_OP, OTEXT("NO-OP") },
    { OCI_SFC_RENAME, OTEXT("RENAME") },
    { OCI_SFC_COMMENT, OTEXT("COMMENT") },
    { OCI_SFC_AUDIT_OBJECT, OTEXT("AUDIT OBJECT") },
    { OCI_SFC_NOAUDIT_OBJECT, OTEXT("NOAUDIT OBJECT") },
    { OCI_SFC_CREATE_DATABASE_LINK, OTEXT("CREATE DATABASE LINK") },
    { OCI_SFC_DROP_DATABASE_LINK, OTEXT("DROP DATABASE LINK") },
    { OCI_SFC_CREATE_DATABASE, OTEXT("CREATE DATABASE") },
    { OCI_SFC_ALTER_DATABASE, OTEXT("ALTER DATABASE") },
    { OCI_SFC_CREATE_ROLLBACK_SEG, OTEXT("CREATE ROLLBACK SEG") },
    { OCI_SFC_ALTER_ROLLBACK_SEG, OTEXT("ALTER ROLLBACK SEG") },
    { OCI_SFC_DROP_ROLLBACK_SEG, OTEXT("DROP ROLLBACK SEG") },
    { OCI_SFC_CREATE_TABLESPACE, OTEXT("CREATE TABLESPACE") },
    { OCI_SFC_ALTER_TABLESPACE, OTEXT("ALTER TABLESPACE") },
    { OCI_SFC_DROP_TABLESPACE, OTEXT("DROP TABLESPACE") },
    { OCI_SFC_ALTER_SESSION, OTEXT("ALTER SESSION") },
    { OCI_SFC_ALTER_USER, OTEXT("ALTER USER") },
    { OCI_SFC_COMMIT, OTEXT("COMMIT") },
    { OCI_SFC_ROLLBACK, OTEXT("ROLLBACK") },
    { OCI_SFC_SAVEPOINT, OTEXT("SAVEPOINT") },
    { OCI_SFC_PL_SQL_EXECUTE, OTEXT("PL/SQL EXECUTE") },
    { OCI_SFC_SET_TRANSACTION, OTEXT("SET TRANSACTION") },
    { OCI_SFC_ALTER_SYSTEM, OTEXT("ALTER SYSTEM") },
    { OCI_SFC_EXPLAIN, OTEXT("EXPLAIN") },
    { OCI_SFC_CREATE_USER, OTEXT("CREATE USER") },
    { OCI_SFC_CREATE_ROLE, OTEXT("CREATE ROLE") },
    { OCI_SFC_DROP_USER, OTEXT("DROP USER") },
    { OCI_SFC_DROP_ROLE, OTEXT("DROP ROLE") },
    { OCI_SFC_SET_ROLE, OTEXT("SET ROLE") },
    { OCI_SFC_CREATE_SCHEMA, OTEXT("CREATE SCHEMA") },
    { OCI_SFC_CREATE_CONTROL_FILE, OTEXT("CREATE CONTROL FILE") },
    { OCI_SFC_ALTER_TRACING, OTEXT("ALTER TRACING") },
    { OCI_SFC_CREATE_TRIGGER, OTEXT("CREATE TRIGGER") },
    { OCI_SFC_ALTER_TRIGGER, OTEXT("ALTER TRIGGER") },
    { OCI_SFC_DROP_TRIGGER, OTEXT("DROP TRIGGER") },
    { OCI_SFC_ANALYZE_TABLE, OTEXT("ANALYZE TABLE") },
    { OCI_SFC_ANALYZE_INDEX, OTEXT("ANALYZE INDEX") },
    { OCI_SFC_ANALYZE_CLUSTER, OTEXT("ANALYZE CLUSTER") },
    { OCI_SFC_CREATE_PROFILE, OTEXT("CREATE PROFILE") },
    { OCI_SFC_DROP_PROFILE, OTEXT("DROP PROFILE") },
    { OCI_SFC_ALTER_PROFILE, OTEXT("ALTER PROFILE") },
    { OCI_SFC_DROP_PROCEDURE, OTEXT("DROP PROCEDURE") },
    { OCI_SFC_ALTER_RESOURCE_COST, OTEXT("ALTER RESOURCE COST") },
    { OCI_SFC_CREATE_MATERIALIZED_VIEW_LOG, OTEXT("CREATE MATERIALIZED VIEW LOG") },
    { OCI_SFC_ALTER_MATERIALIZED_VIEW_LOG, OTEXT("ALTER MATERIALIZED VIEW LOG") },
    { OCI_SFC_DROP_MATERIALIZED_VIEW_LOG, OTEXT("DROP MATERIALIZED VIEW LOG") },
    { OCI_SFC_CREATE_MATERIALIZED_VIEW, OTEXT("CREATE MATERIALIZED VIEW") },
    { OCI_SFC_ALTER_MATERIALIZED_VIEW, OTEXT("ALTER MATERIALIZED VIEW") },
    { OCI_SFC_DROP_MATERIALIZED_VIEW, OTEXT("DROP MATERIALIZED VIEW") },
    { OCI_SFC_CREATE_TYPE, OTEXT("CREATE TYPE") },
    { OCI_SFC_DROP_TYPE, OTEXT("DROP TYPE") },
    { OCI_SFC_ALTER_ROLE, OTEXT("ALTER ROLE") },
    { OCI_SFC_ALTER_TYPE, OTEXT("ALTER TYPE") },
    { OCI_SFC_CREATE_TYPE_BODY, OTEXT("CREATE TYPE BODY") },
    { OCI_SFC_ALTER_TYPE_BODY, OTEXT("ALTER TYPE BODY") },
    { OCI_SFC_DROP_TYPE_BODY, OTEXT("DROP TYPE BODY") },
    { OCI_SFC_DROP_LIBRARY, OTEXT("DROP LIBRARY") },
    { OCI_SFC_TRUNCATE_TABLE, OTEXT("TRUNCATE TABLE") },
    { OCI_SFC_TRUNCATE_CLUSTER, OTEXT("TRUNCATE CLUSTER") },
    { OCI_SFC_ALTER_VIEW, OTEXT("ALTER VIEW") },
    { OCI_SFC_SET_CONSTRAINTS, OTEXT("SET CONSTRAINTS") },
    { OCI_SFC_CREATE_FUNCTION, OTEXT("CREATE FUNCTION") },
    { OCI_SFC_ALTER_FUNCTION, OTEXT("ALTER FUNCTION") },
    { OCI_SFC_DROP_FUNCTION, OTEXT("DROP FUNCTION") },
    { OCI_SFC_CREATE_PACKAGE, OTEXT("CREATE PACKAGE") },
    { OCI_SFC_ALTER_PACKAGE, OTEXT("ALTER PACKAGE") },
    { OCI_SFC_DROP_PACKAGE, OTEXT("DROP PACKAGE") },
    { OCI_SFC_CREATE_PACKAGE_BODY, OTEXT("CREATE PACKAGE BODY") },
    { OCI_SFC_ALTER_PACKAGE_BODY, OTEXT("ALTER PACKAGE BODY") },
    { OCI_SFC_DROP_PACKAGE_BODY, OTEXT("DROP PACKAGE BODY") },
    { OCI_SFC_LOGON, OTEXT("LOGON") },
    { OCI_SFC_LOGOFF, OTEXT("LOGOFF") },
    { OCI_SFC_LOGOFF_BY_CLEANUP, OTEXT("LOGOFF BY CLEANUP") },
    { OCI_SFC_SESSION_REC, OTEXT("SESSION REC") },
    { OCI_SFC_SYSTEM_AUDIT, OTEXT("SYSTEM AUDIT") },
    { OCI_SFC_SYSTEM_NOAUDIT, OTEXT("SYSTEM NOAUDIT") },
    { OCI_SFC_AUDIT_DEFAULT, OTEXT("AUDIT DEFAULT") },
    { OCI_SFC_NOAUDIT_DEFAULT, OTEXT("NOAUDIT DEFAULT") },
    { OCI_SFC_SYSTEM_GRANT, OTEXT("SYSTEM GRANT") },
    { OCI_SFC_SYSTEM_REVOKE, OTEXT("SYSTEM REVOKE") },
    { OCI_SFC_CREATE_PUBLIC_SYNONYM, OTEXT("CREATE PUBLIC SYNONYM") },
    { OCI_SFC_DROP_PUBLIC_SYNONYM, OTEXT("DROP PUBLIC SYNONYM") },
    { OCI_SFC_CREATE_PUBLIC_DATABASE_LINK, OTEXT("CREATE PUBLIC DATABASE LINK") },
    { OCI_SFC_DROP_PUBLIC_DATABASE_LINK, OTEXT("DROP PUBLIC DATABASE LINK") },
    { OCI_SFC_GRANT_ROLE, OTEXT("GRANT ROLE") },
    { OCI_SFC_REVOKE_ROLE, OTEXT("REVOKE ROLE") },
    { OCI_SFC_EXECUTE_PROCEDURE, OTEXT("EXECUTE PROCEDURE") },
    { OCI_SFC_USER_COMMENT, OTEXT("USER COMMENT") },
    { OCI_SFC_ENABLE_TRIGGER, OTEXT("ENABLE TRIGGER") },
    { OCI_SFC_DISABLE_TRIGGER, OTEXT("DISABLE TRIGGER") },
    { OCI_SFC_ENABLE_ALL_TRIGGERS, OTEXT("ENABLE ALL TRIGGERS") },
    { OCI_SFC_DISABLE_ALL_TRIGGERS, OTEXT("DISABLE ALL TRIGGERS") },
    { OCI_SFC_NETWORK_ERROR, OTEXT("NETWORK ERROR") },
    { OCI_SFC_EXECUTE_TYPE, OTEXT("EXECUTE TYPE") },
    { OCI_SFC_READ_DIRECTORY, OTEXT("READ DIRECTORY") },
    { OCI_SFC_WRITE_DIRECTORY, OTEXT("WRITE DIRECTORY") },
    { OCI_SFC_FLASHBACK, OTEXT("FLASHBACK") },
    { OCI_SFC_BECOME_USER, OTEXT("BECOME USER") },
    { OCI_SFC_ALTER_MINING_MODEL, OTEXT("ALTER MINING MODEL") },
    { OCI_SFC_SELECT_MINING_MODEL, OTEXT("SELECT MINING MODEL") },
    { OCI_SFC_USE_EDITION, OTEXT("USE EDITION") },
    { OCI_SFC_CREATE_MINING_MODEL, OTEXT("CREATE MINING MODEL") },
    { OCI_SFC_ALTER_PUBLIC_SYNONYM, OTEXT("ALTER PUBLIC SYNONYM") },
    { OCI_SFC_EXECUTE_DIRECTORY, OTEXT("EXECUTE DIRECTORY") },
    { OCI_SFC_SQL_LOADER_DIRECT_PATH_LOAD, OTEXT("SQL*LOADER DIRECT PATH LOAD") },
    { OCI_SFC_DATAPUMP_DIRECT_PATH_UNLOAD, OTEXT("DATAPUMP DIRECT PATH UNLOAD") },
    { OCI_SFC_DATABASE_STARTUP, OTEXT("DATABASE STARTUP") },
    { OCI_SFC_DATABASE_SHUTDOWN, OTEXT("DATABASE SHUTDOWN") },
    { OCI_SFC_CREATE_SQL_TXLN_PROFILE, OTEXT("CREATE SQL TXLN PROFILE") },
    { OCI_SFC_ALTER_SQL_TXLN_PROFILE, OTEXT("ALTER SQL TXLN PROFILE") },
    { OCI_SFC_USE_SQL_TXLN_PROFILE, OTEXT("USE SQL TXLN PROFILE") },
    { OCI_SFC_DROP_SQL_TXLN_PROFILE, OTEXT("DROP SQL TXLN PROFILE") },
    { OCI_SFC_CREATE_MEASURE_FOLDER, OTEXT("CREATE MEASURE FOLDER") },
    { OCI_SFC_ALTER_MEASURE_FOLDER, OTEXT("ALTER MEASURE FOLDER") },
    { OCI_SFC_DROP_MEASURE_FOLDER, OTEXT("DROP MEASURE FOLDER") },
    { OCI_SFC_CREATE_CUBE_BUILD_PROCESS, OTEXT("CREATE CUBE BUILD PROCESS") },
    { OCI_SFC_ALTER_CUBE_BUILD_PROCESS, OTEXT("ALTER CUBE BUILD PROCESS") },
    { OCI_SFC_DROP_CUBE_BUILD_PROCESS, OTEXT("DROP CUBE BUILD PROCESS") },
    { OCI_SFC_CREATE_CUBE, OTEXT("CREATE CUBE") },
    { OCI_SFC_ALTER_CUBE, OTEXT("ALTER CUBE") },
    { OCI_SFC_DROP_CUBE, OTEXT("DROP CUBE") },
    { OCI_SFC_CREATE_CUBE_DIMENSION, OTEXT("CREATE CUBE DIMENSION") },
    { OCI_SFC_ALTER_CUBE_DIMENSION, OTEXT("ALTER CUBE DIMENSION") },
    { OCI_SFC_DROP_CUBE_DIMENSION, OTEXT("DROP CUBE DIMENSION") },
    { OCI_SFC_CREATE_DIRECTORY, OTEXT("CREATE DIRECTORY") },
    { OCI_SFC_DROP_DIRECTORY, OTEXT("DROP DIRECTORY") },
    { OCI_SFC_CREATE_LIBRARY, OTEXT("CREATE LIBRARY") },
    { OCI_SFC_CREATE_JAVA, OTEXT("CREATE JAVA") },
    { OCI_SFC_ALTER_JAVA, OTEXT("ALTER JAVA") },
    { OCI_SFC_DROP_JAVA, OTEXT("DROP JAVA") },
    { OCI_SFC_CREATE_OPERATOR, OTEXT("CREATE OPERATOR") },
    { OCI_SFC_CREATE_INDEXTYPE, OTEXT("CREATE INDEXTYPE") },
    { OCI_SFC_DROP_INDEXTYPE, OTEXT("DROP INDEXTYPE") },
    { OCI_SFC_ALTER_INDEXTYPE, OTEXT("ALTER INDEXTYPE") },
    { OCI_SFC_DROP_OPERATOR, OTEXT("DROP OPERATOR") },
    { OCI_SFC_ASSOCIATE_STATISTICS, OTEXT("ASSOCIATE STATISTICS") },
    { OCI_SFC_DISASSOCIATE_STATISTICS, OTEXT("DISASSOCIATE STATISTICS") },
    { OCI_SFC_CALL_METHOD, OTEXT("CALL METHOD") },
    { OCI_SFC_CREATE_SUMMARY, OTEXT("CREATE SUMMARY") },
    { OCI_SFC_ALTER_SUMMARY, OTEXT("ALTER SUMMARY") },
    { OCI_SFC_DROP_SUMMARY, OTEXT("DROP SUMMARY") },
    { OCI_SFC_CREATE_DIMENSION, OTEXT("CREATE DIMENSION") },
    { OCI_SFC_ALTER_DIMENSION, OTEXT("ALTER DIMENSION") },
    { OCI_SFC_DROP_DIMENSION, OTEXT("DROP DIMENSION") },
    { OCI_SFC_CREATE_CONTEXT, OTEXT("CREATE CONTEXT") },
    { OCI_SFC_DROP_CONTEXT, OTEXT("DROP CONTEXT") },
    { OCI_SFC_ALTER_OUTLINE, OTEXT("ALTER OUTLINE") },
    { OCI_SFC_CREATE_OUTLINE, OTEXT("CREATE OUTLINE") },
    { OCI_SFC_DROP_OUTLINE, OTEXT("DROP OUTLINE") },
    { OCI_SFC_UPDATE_INDEXES, OTEXT("UPDATE INDEXES") },
    { OCI_SFC_ALTER_OPERATOR, OTEXT("ALTER OPERATOR") },
    { OCI_SFC_CREATE_SPFILE, OTEXT("CREATE SPFILE") },
    { OCI_SFC_CREATE_PFILE, OTEXT("CREATE PFILE") },
    { OCI_SFC_MERGE, OTEXT("MERGE") },
    { OCI_SFC_PASSWORD_CHANGE, OTEXT("PASSWORD CHANGE") },
    { OCI_SFC_ALTER_SYNONYM, OTEXT("ALTER SYNONYM") },
    { OCI_SFC_ALTER_DISKGROUP, OTEXT("ALTER DISKGROUP") },
    { OCI_SFC_CREATE_DISKGROUP, OTEXT("CREATE DISKGROUP") },
    { OCI_SFC_DROP_DISKGROUP, OTEXT("DROP DISKGROUP") },
    { OCI_SFC_PURGE_RECYCLEBIN, OTEXT("PURGE RECYCLEBIN") },
    { OCI_SFC_PURGE_DBA_RECYCLEBIN, OTEXT("PURGE DBA_RECYCLEBIN") },
    { OCI_SFC_PURGE_TABLESPACE, OTEXT("PURGE TABLESPACE") },
    { OCI_SFC_PURGE_TABLE, OTEXT("PURGE TABLE") },
    { OCI_SFC_PURGE_INDEX, OTEXT("PURGE INDEX") },
    { OCI_SFC_UNDROP_OBJECT, OTEXT("UNDROP OBJECT") },
    { OCI_SFC_DROP_DATABASE, OTEXT("DROP DATABASE") },
    { OCI_SFC_FLASHBACK_DATABASE, OTEXT("FLASHBACK DATABASE") },
    { OCI_SFC_FLASHBACK_TABLE, OTEXT("FLASHBACK TABLE") },
    { OCI_SFC_CREATE_RESTORE_POINT, OTEXT("CREATE RESTORE POINT") },
    { OCI_SFC_DROP_RESTORE_POINT, OTEXT("DROP RESTORE POINT") },
    { OCI_SFC_PROXY_AUTHENTICATION_ONLY, OTEXT("PROXY AUTHENTICATION ONLY") },
    { OCI_SFC_DECLARE_REWRITE_EQUIVALENCE, OTEXT("DECLARE REWRITE EQUIVALENCE") },
    { OCI_SFC_ALTER_REWRITE_EQUIVALENCE, OTEXT("ALTER REWRITE EQUIVALENCE") },
    { OCI_SFC_DROP_REWRITE_EQUIVALENCE, OTEXT("DROP REWRITE EQUIVALENCE") },
    { OCI_SFC_CREATE_EDITION, OTEXT("CREATE EDITION") },
    { OCI_SFC_ALTER_EDITION, OTEXT("ALTER EDITION") },
    { OCI_SFC_DROP_EDITION, OTEXT("DROP EDITION") },
    { OCI_SFC_DROP_ASSEMBLY, OTEXT("DROP ASSEMBLY") },
    { OCI_SFC_CREATE_ASSEMBLY, OTEXT("CREATE ASSEMBLY") },
    { OCI_SFC_ALTER_ASSEMBLY, OTEXT("ALTER ASSEMBLY") },
    { OCI_SFC_CREATE_FLASHBACK_ARCHIVE, OTEXT("CREATE FLASHBACK ARCHIVE") },
    { OCI_SFC_ALTER_FLASHBACK_ARCHIVE, OTEXT("ALTER FLASHBACK ARCHIVE") },
    { OCI_SFC_DROP_FLASHBACK_ARCHIVE, OTEXT("DROP FLASHBACK ARCHIVE") },
    { OCI_SFC_DEBUG_CONNECT, OTEXT("DEBUG CONNECT") },
    { OCI_SFC_DEBUG_PROCEDURE, OTEXT("DEBUG PROCEDURE") },
    { OCI_SFC_ALTER_DATABASE_LINK, OTEXT("ALTER DATABASE LINK") },
    { OCI_SFC_CREATE_PLUGGABLE_DATABASE, OTEXT("CREATE PLUGGABLE DATABASE") },
    { OCI_SFC_ALTER_PLUGGABLE_DATABASE, OTEXT("ALTER PLUGGABLE DATABASE") },
    { OCI_SFC_DROP_PLUGGABLE_DATABASE, OTEXT("DROP PLUGGABLE DATABASE") },
    { OCI_SFC_CREATE_AUDIT_POLICY, OTEXT("CREATE AUDIT POLICY") },
    { OCI_SFC_ALTER_AUDIT_POLICY, OTEXT("ALTER AUDIT POLICY") },
    { OCI_SFC_DROP_AUDIT_POLICY, OTEXT("DROP AUDIT POLICY") },
    { OCI_SFC_CODE_BASED_GRANT, OTEXT("CODE-BASED GRANT") },
    { OCI_SFC_CODE_BASED_REVOKE, OTEXT("CODE-BASED REVOKE") },
    { OCI_SFC_CREATE_LOCKDOWN_PROFILE, OTEXT("CREATE LOCKDOWN PROFILE") },
    { OCI_SFC_DROP_LOCKDOWN_PROFILE, OTEXT("DROP LOCKDOWN PROFILE") },
    { OCI_SFC_ALTER_LOCKDOWN_PROFILE, OTEXT("ALTER LOCKDOWN PROFILE") },
    { OCI_SFC_TRANSLATE_SQL, OTEXT("TRANSLATE SQL") },
    { OCI_SFC_ADMINISTER_KEY_MANAGEMENT, OTEXT("ADMINISTER KEY MANAGEMENT") },
    { OCI_SFC_CREATE_MATERIALIZED_ZONEMAP, OTEXT("CREATE MATERIALIZED ZONEMAP") },
    { OCI_SFC_ALTER_MATERIALIZED_ZONEMAP, OTEXT("ALTER MATERIALIZED ZONEMAP") },
    { OCI_SFC_DROP_MATERIALIZED_ZONEMAP, OTEXT("DROP MATERIALIZED ZONEMAP") },
    { OCI_SFC_DROP_MINING_MODEL, OTEXT("DROP MINING MODEL") },
    { OCI_SFC_CREATE_ATTRIBUTE_DIMENSION, OTEXT("CREATE ATTRIBUTE DIMENSION") },
    { OCI_SFC_ALTER_ATTRIBUTE_DIMENSION, OTEXT("ALTER ATTRIBUTE DIMENSION") },
    { OCI_SFC_DROP_ATTRIBUTE_DIMENSION, OTEXT("DROP ATTRIBUTE DIMENSION") },
    { OCI_SFC_CREATE_HIERARCHY, OTEXT("CREATE HIERARCHY") },
    { OCI_SFC_ALTER_HIERARCHY, OTEXT("ALTER HIERARCHY") },
    { OCI_SFC_DROP_HIERARCHY, OTEXT("DROP HIERARCHY") },
    { OCI_SFC_CREATE_ANALYTIC_VIEW, OTEXT("CREATE ANALYTIC VIEW") },
    { OCI_SFC_ALTER_ANALYTIC_VIEW, OTEXT("ALTER ANALYTIC VIEW") },
    { OCI_SFC_DROP_ANALYTIC_VIEW, OTEXT("DROP ANALYTIC VIEW") },
    { OCI_SFC_CREATE_MLE_MODULE, OTEXT("CREATE MLE MODULE") },
    { OCI_SFC_DROP_MLE_MODULE, OTEXT("DROP MLE MODULE") },
    { OCI_SFC_CREATE_MLE_ENV, OTEXT("CREATE MLE ENV") },
    { OCI_SFC_DROP_MLE_ENV, OTEXT("DROP MLE ENV") },
    { OCI_SFC_CREATE_LANGUAGE, OTEXT("CREATE LANGUAGE") },
    { OCI_SFC_DROP_LANGUAGE, OTEXT("DROP LANGUAGE") },
    { OCI_SFC_CREATE_PROPERTY_GRAPH, OTEXT("CREATE PROPERTY GRAPH") },
    { OCI_SFC_ALTER_PROPERTY_GRAPH, OTEXT("ALTER PROPERTY GRAPH") },
    { OCI_SFC_DROP_PROPERTY_GRAPH, OTEXT("DROP PROPERTY GRAPH") },
    { OCI_SFC_CREATE_TRUE_CACHE, OTEXT("CREATE TRUE CACHE") },
    { OCI_SFC_ALTER_MLE_MODULE, OTEXT("ALTER MLE MODULE") },
    { OCI_SFC_ALTER_MLE_ENV, OTEXT("ALTER MLE ENV") },
    { OCI_SFC_CREATE_DOMAIN, OTEXT("CREATE DOMAIN") },
    { OCI_SFC_ALTER_DOMAIN, OTEXT("ALTER DOMAIN") },
    { OCI_SFC_DROP_DOMAIN, OTEXT("DROP DOMAIN") },
    { OCI_SFC_COLLECT_DEBUG_INFO, OTEXT("COLLECT DEBUG INFO") },
    { OCI_SFC_ALTER_PUBLIC_DATABASE_LINK, OTEXT("ALTER PUBLIC DATABASE LINK") },
};

static const OCI_UInt32 FormatTypeValues[] =
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

/* Immutable template for the per-environment locale string cache.
   Each OCI_Environment gets its own copy in env->locale_strings; only the
   'code' fields below matter here, the 'str' slots always start out NULL. */

static const OCI_LocaleStringInfo LocaleStringDefaults[OCI_LSI_COUNT] =
{
    { OCI_LSI_DAYNAME1,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                },
    { OCI_LSI_DAYNAME2,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              },
    { OCI_LSI_DAYNAME3,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           },
    { OCI_LSI_DAYNAME4,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        },
    { OCI_LSI_DAYNAME5,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     },
    { OCI_LSI_DAYNAME6,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  },
    { OCI_LSI_DAYNAME7,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               },
    { OCI_LSI_ABDAYNAME1,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            },
    { OCI_LSI_ABDAYNAME2,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         },
    { OCI_LSI_ABDAYNAME3,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      },
    { OCI_LSI_ABDAYNAME4,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   },
    { OCI_LSI_ABDAYNAME5,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                },
    { OCI_LSI_ABDAYNAME6,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             },
    { OCI_LSI_ABDAYNAME7,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
    { OCI_LSI_MONTHNAME1,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       },
    { OCI_LSI_MONTHNAME2,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    },
    { OCI_LSI_MONTHNAME3,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 },
    { OCI_LSI_MONTHNAME4,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              },
    { OCI_LSI_MONTHNAME5,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           },
    { OCI_LSI_MONTHNAME6,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        },
    { OCI_LSI_MONTHNAME7,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     },
    { OCI_LSI_MONTHNAME8,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  },
    { OCI_LSI_MONTHNAME9,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               },
    { OCI_LSI_MONTHNAME10,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            },
    { OCI_LSI_MONTHNAME11,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         },
    { OCI_LSI_MONTHNAME12,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      },
    { OCI_LSI_ABMONTHNAME1,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   },
    { OCI_LSI_ABMONTHNAME2,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                },
    { OCI_LSI_ABMONTHNAME3,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             },
    { OCI_LSI_ABMONTHNAME4,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
    { OCI_LSI_ABMONTHNAME5,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       },
    { OCI_LSI_ABMONTHNAME6,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    },
    { OCI_LSI_ABMONTHNAME7,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 },
    { OCI_LSI_ABMONTHNAME8,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              },
    { OCI_LSI_ABMONTHNAME9,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           },
    { OCI_LSI_ABMONTHNAME10,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        },
    { OCI_LSI_ABMONTHNAME11,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     },
    { OCI_LSI_ABMONTHNAME12,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  },
    { OCI_LSI_YES,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               },
    { OCI_LSI_NO,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            },
    { OCI_LSI_AM,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         },
    { OCI_LSI_PM,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      },
    { OCI_LSI_AD,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   },
    { OCI_LSI_BC,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             },
    { OCI_LSI_DECIMAL,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       },
    { OCI_LSI_GROUP,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 },
    { OCI_LSI_DEBIT,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
    { OCI_LSI_CREDIT,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  },
    { OCI_LSI_DATEFORMAT,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
    { OCI_LSI_INT_CURRENCY,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  },
    { OCI_LSI_LOC_CURRENCY,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
    { OCI_LSI_LANGUAGE,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  },
    { OCI_LSI_ABLANGUAGE,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
    { OCI_LSI_TERRITORY,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  },
    { OCI_LSI_CHARACTER_SET,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
    { OCI_LSI_LINGUISTIC_NAME,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  },
    { OCI_LSI_CALENDAR,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
    { OCI_LSI_DUAL_CURRENCY,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  },
    { OCI_LSI_WRITINGDIR,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
    { OCI_LSI_ABTERRITORY,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  },
    { OCI_LSI_DDATEFORMAT,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
    { OCI_LSI_DTIMEFORMAT,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  },
    { OCI_LSI_SFDATEFORMAT,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
    { OCI_LSI_SFTIMEFORMAT,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  },
    { OCI_LSI_NUMGROUPING,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
    { OCI_LSI_LISTSEP,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 },
    { OCI_LSI_MONDECIMAL,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        },
    { OCI_LSI_MONGROUP,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            },
    { OCI_LSI_MONGROUPING,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            },
    { OCI_LSI_INT_CURRENCYSEP,
      NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            }
};

static void OcilibEnvironmentClearSymbolsAbove
(
    OCI_Environment *env,
    OCI_UInt32       version
);

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentComputeRuntimeVersion
 * --------------------------------------------------------------------------------------------- */

static OCI_UInt32 OcilibEnvironmentComputeRuntimeVersion
(
    OCI_Environment *env,
    OCI_UInt32       version
)
{
    /* version != 0 requests an emulated (lowered) client: mask symbols
       introduced above the requested version and return it as-is */

    if (OCI_UNKNOWN != version)
    {
        OcilibEnvironmentClearSymbolsAbove(env, version);
        return version;
    }

    /* detect the real client version from the loaded API surface */

    if (env->api->OCIClientVersion)
    {
        sword maj = 0;
        sword min = 0;
        sword rev = 0;

        env->api->OCIClientVersion(&maj, &min, &rev, NULL, NULL);

        return (OCI_UInt32) OCI_VER_MAKE(maj, min, rev);
    }
    else if (env->api->OCIVectorToArray)             return OCI_23_4;
    else if (env->api->OCIServerDataLengthGet)       return OCI_21_3;
    else if (env->api->OCIJsonTextBufferParse)       return OCI_21_1;
    else if (env->api->OCISodaBulkInsert)            return OCI_19_3;
    else if (env->api->OCISodaOperKeysSet)           return OCI_18_3;
    else if (env->api->OCIServerRelease2)            return OCI_18_1;
    else if (env->api->OCIStmtGetNextResult)         return OCI_12_1;
    else if (env->api->OCILobGetContentType)         return OCI_11_2;
    else if (env->api->OCIArrayDescriptorFree)       return OCI_11_1;
    else if (env->api->OCILobWrite2)                 return OCI_10_1;
    else if (env->api->OCIStmtPrepare2)              return OCI_9_2;
    else if (env->api->OCIDateTimeGetTimeZoneName)   return OCI_9_0;
    else if (env->api->OCIEnvInit)                   return OCI_8_0;

    return 0;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentClearSymbolsAbove
 * --------------------------------------------------------------------------------------------- */

static void OcilibEnvironmentClearSymbolsAbove
(
    OCI_Environment *env,
    OCI_UInt32       version
)
{
    if (version < OCI_23_4)
    {
        env->api->OCIVectorToArray      = NULL;
        env->api->OCIVectorFromArray    = NULL;
        env->api->OCIVectorToText       = NULL;
        env->api->OCIVectorFromText     = NULL;
        env->api->OCIJsonSchemaValidate = NULL;
    }

    if (version < OCI_21_1)
    {
        env->api->OCIJsonTextBufferParse  = NULL;
        env->api->OCIJsonToTextBuffer     = NULL;
        env->api->OCIJsonBinaryBufferLoad = NULL;
        env->api->OCIJsonToBinaryBuffer   = NULL;
        env->api->OCIJsonDomDocGet        = NULL;
        env->api->OCIJsonDomDocSet        = NULL;
        env->api->OCIJsonClone            = NULL;
        env->api->OCIJsonBinaryLengthGet  = NULL;
    }

    if (version < OCI_20_1)
    {
        env->api->OCISodaCollTruncate = NULL;
    }

    if (version < OCI_19_3)
    {
        env->api->OCIServerDataLengthGet = NULL;
    }

    if (version < OCI_18_5)
    {
        env->api->OCISodaBulkInsertWithCtnt       = NULL;
        env->api->OCISodaBulkInsert               = NULL;
        env->api->OCISodaBulkInsertAndGet         = NULL;
        env->api->OCISodaBulkInsertAndGetWithCtnt = NULL;
        env->api->OCISodaBulkInsertAndGetWithOpts = NULL;
    }

    if (version < OCI_18_3)
    {
        env->api->OCISodaOperKeysSet = NULL;
    }

    if (version < OCI_18_1)
    {
        env->api->OCIServerRelease2               = NULL;
        env->api->OCISodaCollCreate               = NULL;
        env->api->OCISodaMetadataBuild            = NULL;
        env->api->OCISodaCollCreateWithMetadata   = NULL;
        env->api->OCISodaCollOpen                 = NULL;
        env->api->OCISodaDocCreate                = NULL;
        env->api->OCISodaDocCreateWithKey         = NULL;
        env->api->OCISodaDocCreateWithKeyAndMType = NULL;
        env->api->OCISodaInsert                   = NULL;
        env->api->OCISodaInsertAndGet             = NULL;
        env->api->OCISodaInsertWithCtnt           = NULL;
        env->api->OCISodaInsertAndGetWithCtnt     = NULL;
        env->api->OCISodaFind                     = NULL;
        env->api->OCISodaFindOne                  = NULL;
        env->api->OCISodaFindOneWithKey           = NULL;
        env->api->OCISodaDocGetNext               = NULL;
        env->api->OCISodaCollList                 = NULL;
        env->api->OCISodaCollGetNext              = NULL;
        env->api->OCISodaDocCount                 = NULL;
        env->api->OCISodaDocCountWithFilter       = NULL;
        env->api->OCISodaReplOne                  = NULL;
        env->api->OCISodaReplOneAndGet            = NULL;
        env->api->OCISodaReplOneWithKey           = NULL;
        env->api->OCISodaReplOneAndGetWithKey     = NULL;
        env->api->OCISodaReplOneWithCtnt          = NULL;
        env->api->OCISodaReplOneAndGetWithCtnt    = NULL;
        env->api->OCISodaRemove                   = NULL;
        env->api->OCISodaRemoveOneWithKey         = NULL;
        env->api->OCISodaCollDrop                 = NULL;
        env->api->OCISodaIndexCreate              = NULL;
        env->api->OCISodaIndexDrop                = NULL;
        env->api->OCISodaSave                     = NULL;
        env->api->OCISodaSaveAndGet               = NULL;
        env->api->OCISodaIndexList                = NULL;
        env->api->OCISodaDataGuideGet             = NULL;
    }

    if (version < OCI_12_2)
    {
        env->api->OCIShardingKeyColumnAdd = NULL;
        env->api->OCIShardingKeyReset     = NULL;
        env->api->OCIShardInstancesGet    = NULL;
    }

    if (version < OCI_12_1)
    {
        env->api->OCIStmtGetNextResult = NULL;
    }

    if (version < OCI_11_2)
    {
        env->api->OCILobGetContentType = NULL;
    }

    if (version < OCI_11_1)
    {
        env->api->OCIArrayDescriptorAlloc = NULL;
        env->api->OCIArrayDescriptorFree  = NULL;
    }

    if (version < OCI_10_2)
    {
        env->api->OCIClientVersion = NULL;
        env->api->OCIDBStartup     = NULL;
        env->api->OCIDBShutdown    = NULL;
        env->api->OCIPing          = NULL;
    }

    if (version < OCI_10_1)
    {
        env->api->OCILobGetStorageLimit = NULL;
        env->api->OCILobCopy2           = NULL;
        env->api->OCILobErase2          = NULL;
        env->api->OCILobGetLength2      = NULL;
        env->api->OCILobLoadFromFile2   = NULL;
        env->api->OCILobRead2           = NULL;
        env->api->OCILobTrim2           = NULL;
        env->api->OCILobWrite2          = NULL;
        env->api->OCILobWriteAppend2    = NULL;
    }

    if (version < OCI_9_2)
    {
        env->api->OCINlsGetInfo   = NULL;
        env->api->OCIEnvNlsCreate = NULL;
        env->api->OCIStmtPrepare2 = NULL;
        env->api->OCIStmtRelease  = NULL;
    }

    if (version < OCI_9_1)
    {
        env->api->OCIStmtFetch2 = NULL;
    }

    if (version < OCI_8_1)
    {
        env->api->OCIEnvCreate               = NULL;
        env->api->OCIDirPathAbort            = NULL;
        env->api->OCIDirPathDataSave         = NULL;
        env->api->OCIDirPathFinish           = NULL;
        env->api->OCIDirPathPrepare          = NULL;
        env->api->OCIDirPathLoadStream       = NULL;
        env->api->OCIDirPathColArrayEntrySet = NULL;
        env->api->OCIDirPathColArrayReset    = NULL;
        env->api->OCIDirPathColArrayToStream = NULL;
        env->api->OCIDirPathStreamReset      = NULL;
        env->api->OCIDirPathFlushRow         = NULL;
        env->api->OCICacheFree               = NULL;
        env->api->OCISubscriptionRegister    = NULL;
        env->api->OCISubscriptionUnRegister  = NULL;
    }

}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentLoadSymbols
 * --------------------------------------------------------------------------------------------- */

static void OcilibEnvironmentLoadSymbols
(
    OCI_Environment *env
)
{
    WARNING_DISABLE_UNSAFE_CONVERT

#ifdef OCI_IMPORT_LINKAGE
    /* In linkage mode, OCI functions are resolved at build time. */

#if OCI_VERSION_COMPILE >= OCI_8_0
    env->api->OCIAQDeq                     = OCIAQDeq;
    env->api->OCIAQEnq                     = OCIAQEnq;
    env->api->OCIAQListen                  = OCIAQListen;
    env->api->OCIAttrGet                   = OCIAttrGet;
    env->api->OCIAttrSet                   = OCIAttrSet;
    env->api->OCIBindByName                = OCIBindByName;
    env->api->OCIBindByPos                 = OCIBindByPos;
    env->api->OCIBindDynamic               = OCIBindDynamic;
    env->api->OCIBindObject                = OCIBindObject;
    env->api->OCIBreak                     = OCIBreak;
    env->api->OCICollAppend                = OCICollAppend;
    env->api->OCICollAssign                = OCICollAssign;
    env->api->OCICollAssignElem            = OCICollAssignElem;
    env->api->OCICollGetElem               = OCICollGetElem;
    env->api->OCICollMax                   = OCICollMax;
    env->api->OCICollSize                  = OCICollSize;
    env->api->OCICollTrim                  = OCICollTrim;
    env->api->OCIConnectionPoolCreate      = OCIConnectionPoolCreate;
    env->api->OCIConnectionPoolDestroy     = OCIConnectionPoolDestroy;
    env->api->OCIDateAddDays               = OCIDateAddDays;
    env->api->OCIDateAddMonths             = OCIDateAddMonths;
    env->api->OCIDateAssign                = OCIDateAssign;
    env->api->OCIDateCheck                 = OCIDateCheck;
    env->api->OCIDateCompare               = OCIDateCompare;
    env->api->OCIDateDaysBetween           = OCIDateDaysBetween;
    env->api->OCIDateFromText              = OCIDateFromText;
    env->api->OCIDateLastDay               = OCIDateLastDay;
    env->api->OCIDateNextDay               = OCIDateNextDay;
    env->api->OCIDateSysDate               = OCIDateSysDate;
    env->api->OCIDateTimeAssign            = OCIDateTimeAssign;
    env->api->OCIDateTimeCheck             = OCIDateTimeCheck;
    env->api->OCIDateTimeCompare           = OCIDateTimeCompare;
    env->api->OCIDateTimeConstruct         = OCIDateTimeConstruct;
    env->api->OCIDateTimeConvert           = OCIDateTimeConvert;
    env->api->OCIDateTimeFromArray         = OCIDateTimeFromArray;
    env->api->OCIDateTimeFromText          = OCIDateTimeFromText;
    env->api->OCIDateTimeGetDate           = OCIDateTimeGetDate;
    env->api->OCIDateTimeGetTime           = OCIDateTimeGetTime;
    env->api->OCIDateTimeGetTimeZoneName   = OCIDateTimeGetTimeZoneName;
    env->api->OCIDateTimeGetTimeZoneOffset = OCIDateTimeGetTimeZoneOffset;
    env->api->OCIDateTimeIntervalAdd       = OCIDateTimeIntervalAdd;
    env->api->OCIDateTimeIntervalSub       = OCIDateTimeIntervalSub;
    env->api->OCIDateTimeSubtract          = OCIDateTimeSubtract;
    env->api->OCIDateTimeSysTimeStamp      = OCIDateTimeSysTimeStamp;
    env->api->OCIDateTimeToArray           = OCIDateTimeToArray;
    env->api->OCIDateTimeToText            = OCIDateTimeToText;
    env->api->OCIDateToText                = OCIDateToText;
    env->api->OCIDateZoneToZone            = OCIDateZoneToZone;
    env->api->OCIDefineByPos               = OCIDefineByPos;
    env->api->OCIDefineDynamic             = OCIDefineDynamic;
    env->api->OCIDefineObject              = OCIDefineObject;
    env->api->OCIDescribeAny               = OCIDescribeAny;
    env->api->OCIDescriptorAlloc           = OCIDescriptorAlloc;
    env->api->OCIDescriptorFree            = OCIDescriptorFree;
    env->api->OCIEnvInit                   = OCIEnvInit;
    env->api->OCIErrorGet                  = OCIErrorGet;
    env->api->OCIHandleAlloc               = OCIHandleAlloc;
    env->api->OCIHandleFree                = OCIHandleFree;
    env->api->OCIInitialize                = OCIInitialize;
    env->api->OCIIntervalAdd               = OCIIntervalAdd;
    env->api->OCIIntervalAssign            = OCIIntervalAssign;
    env->api->OCIIntervalCheck             = OCIIntervalCheck;
    env->api->OCIIntervalCompare           = OCIIntervalCompare;
    env->api->OCIIntervalFromTZ            = OCIIntervalFromTZ;
    env->api->OCIIntervalFromText          = OCIIntervalFromText;
    env->api->OCIIntervalGetDaySecond      = OCIIntervalGetDaySecond;
    env->api->OCIIntervalGetYearMonth      = OCIIntervalGetYearMonth;
    env->api->OCIIntervalSetDaySecond      = OCIIntervalSetDaySecond;
    env->api->OCIIntervalSetYearMonth      = OCIIntervalSetYearMonth;
    env->api->OCIIntervalSubtract          = OCIIntervalSubtract;
    env->api->OCIIntervalToText            = OCIIntervalToText;
    env->api->OCIIterCreate                = OCIIterCreate;
    env->api->OCIIterDelete                = OCIIterDelete;
    env->api->OCIIterInit                  = OCIIterInit;
    env->api->OCIIterNext                  = OCIIterNext;
    env->api->OCIIterPrev                  = OCIIterPrev;
    env->api->OCILobAppend                 = OCILobAppend;
    env->api->OCILobAssign                 = OCILobAssign;
    env->api->OCILobClose                  = OCILobClose;
    env->api->OCILobCopy                   = OCILobCopy;
    env->api->OCILobCreateTemporary        = OCILobCreateTemporary;
    env->api->OCILobDisableBuffering       = OCILobDisableBuffering;
    env->api->OCILobEnableBuffering        = OCILobEnableBuffering;
    env->api->OCILobErase                  = OCILobErase;
    env->api->OCILobFileClose              = OCILobFileClose;
    env->api->OCILobFileCloseAll           = OCILobFileCloseAll;
    env->api->OCILobFileExists             = OCILobFileExists;
    env->api->OCILobFileGetName            = OCILobFileGetName;
    env->api->OCILobFileIsOpen             = OCILobFileIsOpen;
    env->api->OCILobFileOpen               = OCILobFileOpen;
    env->api->OCILobFileSetName            = OCILobFileSetName;
    env->api->OCILobFlushBuffer            = OCILobFlushBuffer;
    env->api->OCILobFreeTemporary          = OCILobFreeTemporary;
    env->api->OCILobGetChunkSize           = OCILobGetChunkSize;
    env->api->OCILobGetLength              = OCILobGetLength;
    env->api->OCILobIsEqual                = OCILobIsEqual;
    env->api->OCILobIsTemporary            = OCILobIsTemporary;
    env->api->OCILobLoadFromFile           = OCILobLoadFromFile;
    env->api->OCILobLocatorAssign          = OCILobLocatorAssign;
    env->api->OCILobOpen                   = OCILobOpen;
    env->api->OCILobRead                   = OCILobRead;
    env->api->OCILobTrim                   = OCILobTrim;
    env->api->OCILobWrite                  = OCILobWrite;
    env->api->OCILobWriteAppend            = OCILobWriteAppend;
    env->api->OCINumberAdd                 = OCINumberAdd;
    env->api->OCINumberAssign              = OCINumberAssign;
    env->api->OCINumberCmp                 = OCINumberCmp;
    env->api->OCINumberDiv                 = OCINumberDiv;
    env->api->OCINumberFromInt             = OCINumberFromInt;
    env->api->OCINumberFromReal            = OCINumberFromReal;
    env->api->OCINumberFromText            = OCINumberFromText;
    env->api->OCINumberMul                 = OCINumberMul;
    env->api->OCINumberSub                 = OCINumberSub;
    env->api->OCINumberToInt               = OCINumberToInt;
    env->api->OCINumberToReal              = OCINumberToReal;
    env->api->OCINumberToText              = OCINumberToText;
    env->api->OCIObjectCopy                = OCIObjectCopy;
    env->api->OCIObjectFree                = OCIObjectFree;
    env->api->OCIObjectGetAttr             = OCIObjectGetAttr;
    env->api->OCIObjectGetInd              = OCIObjectGetInd;
    env->api->OCIObjectGetObjectRef        = OCIObjectGetObjectRef;
    env->api->OCIObjectGetProperty         = OCIObjectGetProperty;
    env->api->OCIObjectGetTypeRef          = OCIObjectGetTypeRef;
    env->api->OCIObjectNew                 = OCIObjectNew;
    env->api->OCIObjectPin                 = OCIObjectPin;
    env->api->OCIObjectSetAttr             = OCIObjectSetAttr;
    env->api->OCIObjectUnpin               = OCIObjectUnpin;
    env->api->OCIParamGet                  = OCIParamGet;
    env->api->OCIParamSet                  = OCIParamSet;
    env->api->OCIPasswordChange            = OCIPasswordChange;
    env->api->OCIRawAllocSize              = OCIRawAllocSize;
    env->api->OCIRawAssignBytes            = OCIRawAssignBytes;
    env->api->OCIRawPtr                    = OCIRawPtr;
    env->api->OCIRawResize                 = OCIRawResize;
    env->api->OCIRawSize                   = OCIRawSize;
    env->api->OCIRefAssign                 = OCIRefAssign;
    env->api->OCIRefClear                  = OCIRefClear;
    env->api->OCIRefHexSize                = OCIRefHexSize;
    env->api->OCIRefIsNull                 = OCIRefIsNull;
    env->api->OCIRefToHex                  = OCIRefToHex;
    env->api->OCIRowidToChar               = OCIRowidToChar;
    env->api->OCIServerAttach              = OCIServerAttach;
    env->api->OCIServerDetach              = OCIServerDetach;
    env->api->OCIServerVersion             = OCIServerVersion;
    env->api->OCISessionBegin              = OCISessionBegin;
    env->api->OCISessionEnd                = OCISessionEnd;
    env->api->OCISessionGet                = OCISessionGet;
    env->api->OCISessionPoolCreate         = OCISessionPoolCreate;
    env->api->OCISessionPoolDestroy        = OCISessionPoolDestroy;
    env->api->OCISessionRelease            = OCISessionRelease;
    env->api->OCIStmtExecute               = OCIStmtExecute;
    env->api->OCIStmtFetch                 = OCIStmtFetch;
    env->api->OCIStmtGetBindInfo           = OCIStmtGetBindInfo;
    env->api->OCIStmtGetPieceInfo          = OCIStmtGetPieceInfo;
    env->api->OCIStmtPrepare               = OCIStmtPrepare;
    env->api->OCIStmtSetPieceInfo          = OCIStmtSetPieceInfo;
    env->api->OCIStringAssignText          = OCIStringAssignText;
    env->api->OCIStringPtr                 = OCIStringPtr;
    env->api->OCIStringResize              = OCIStringResize;
    env->api->OCIStringSize                = OCIStringSize;
    env->api->OCITableDelete               = OCITableDelete;
    env->api->OCITableSize                 = OCITableSize;
    env->api->OCITransCommit               = OCITransCommit;
    env->api->OCITransDetach               = OCITransDetach;
    env->api->OCITransForget               = OCITransForget;
    env->api->OCITransPrepare              = OCITransPrepare;
    env->api->OCITransRollback             = OCITransRollback;
    env->api->OCITransStart                = OCITransStart;
    env->api->OCITypeByName                = OCITypeByName;
    env->api->OCITypeByRef                 = OCITypeByRef;
#endif
#if OCI_VERSION_COMPILE >= OCI_8_1
    env->api->OCICacheFree               = OCICacheFree;
    env->api->OCIDirPathAbort            = OCIDirPathAbort;
    env->api->OCIDirPathColArrayEntrySet = OCIDirPathColArrayEntrySet;
    env->api->OCIDirPathColArrayReset    = OCIDirPathColArrayReset;
    env->api->OCIDirPathColArrayToStream = OCIDirPathColArrayToStream;
    env->api->OCIDirPathDataSave         = OCIDirPathDataSave;
    env->api->OCIDirPathFinish           = OCIDirPathFinish;
    env->api->OCIDirPathFlushRow         = OCIDirPathFlushRow;
    env->api->OCIDirPathLoadStream       = OCIDirPathLoadStream;
    env->api->OCIDirPathPrepare          = OCIDirPathPrepare;
    env->api->OCIDirPathStreamReset      = OCIDirPathStreamReset;
    env->api->OCIEnvCreate               = OCIEnvCreate;
    env->api->OCISubscriptionRegister    = OCISubscriptionRegister;
    env->api->OCISubscriptionUnRegister  = OCISubscriptionUnRegister;
#endif
#if OCI_VERSION_COMPILE >= OCI_9_2
    env->api->OCIStmtFetch2   = OCIStmtFetch2;
    env->api->OCIEnvNlsCreate = OCIEnvNlsCreate;
    env->api->OCINlsGetInfo   = OCINlsGetInfo;
    env->api->OCIStmtPrepare2 = OCIStmtPrepare2;
    env->api->OCIStmtRelease  = OCIStmtRelease;
#endif
#if OCI_VERSION_COMPILE >= OCI_10_1
    env->api->OCILobGetStorageLimit = OCILobGetStorageLimit;
  #ifdef ORAXB8_DEFINED
    env->api->OCILobCopy2         = OCILobCopy2;
    env->api->OCILobErase2        = OCILobErase2;
    env->api->OCILobGetLength2    = OCILobGetLength2;
    env->api->OCILobLoadFromFile2 = OCILobLoadFromFile2;
    env->api->OCILobRead2         = OCILobRead2;
    env->api->OCILobTrim2         = OCILobTrim2;
    env->api->OCILobWrite2        = OCILobWrite2;
    env->api->OCILobWriteAppend2  = OCILobWriteAppend2;
  #endif
#endif
#if OCI_VERSION_COMPILE >= OCI_10_2
    env->api->OCIClientVersion = OCIClientVersion;
    env->api->OCIDBShutdown    = OCIDBShutdown;
    env->api->OCIDBStartup     = OCIDBStartup;
    env->api->OCIPing          = OCIPing;
#endif
#if OCI_VERSION_COMPILE >= OCI_11_1
    env->api->OCIArrayDescriptorAlloc = OCIArrayDescriptorAlloc;
    env->api->OCIArrayDescriptorFree  = OCIArrayDescriptorFree;
#endif
#if OCI_VERSION_COMPILE >= OCI_11_2
    env->api->OCILobGetContentType = OCILobGetContentType;
#endif
#if OCI_VERSION_COMPILE >= OCI_12_1
    env->api->OCIStmtGetNextResult = OCIStmtGetNextResult;
#endif
#if OCI_VERSION_COMPILE >= OCI_12_2
    env->api->OCIShardingKeyColumnAdd = OCIShardingKeyColumnAdd;
    env->api->OCIShardingKeyReset     = OCIShardingKeyReset;
    env->api->OCIShardInstancesGet    = OCIShardInstancesGet;
#endif
#if OCI_VERSION_COMPILE >= OCI_18_1
    env->api->OCIServerRelease2               = OCIServerRelease2;
    env->api->OCISodaCollCreate               = OCISodaCollCreate;
    env->api->OCISodaCollCreateWithMetadata   = OCISodaCollCreateWithMetadata;
    env->api->OCISodaCollDrop                 = OCISodaCollDrop;
    env->api->OCISodaCollGetNext              = OCISodaCollGetNext;
    env->api->OCISodaCollList                 = OCISodaCollList;
    env->api->OCISodaCollOpen                 = OCISodaCollOpen;
    env->api->OCISodaDataGuideGet             = OCISodaDataGuideGet;
    env->api->OCISodaDocCount                 = OCISodaDocCount;
    env->api->OCISodaDocCountWithFilter       = OCISodaDocCountWithFilter;
    env->api->OCISodaDocCreate                = OCISodaDocCreate;
    env->api->OCISodaDocCreateWithKey         = OCISodaDocCreateWithKey;
    env->api->OCISodaDocCreateWithKeyAndMType = OCISodaDocCreateWithKeyAndMType;
    env->api->OCISodaDocGetNext               = OCISodaDocGetNext;
    env->api->OCISodaFind                     = OCISodaFind;
    env->api->OCISodaFindOne                  = OCISodaFindOne;
    env->api->OCISodaFindOneWithKey           = OCISodaFindOneWithKey;
    env->api->OCISodaIndexCreate              = OCISodaIndexCreate;
    env->api->OCISodaIndexDrop                = OCISodaIndexDrop;
    env->api->OCISodaInsert                   = OCISodaInsert;
    env->api->OCISodaInsertAndGet             = OCISodaInsertAndGet;
    env->api->OCISodaInsertAndGetWithCtnt     = OCISodaInsertAndGetWithCtnt;
    env->api->OCISodaInsertWithCtnt           = OCISodaInsertWithCtnt;
    env->api->OCISodaSave                     = OCISodaSave;
    env->api->OCISodaSaveAndGet               = OCISodaSaveAndGet;
    env->api->OCISodaIndexList                = OCISodaIndexList;
    env->api->OCISodaMetadataBuild            = OCISodaMetadataBuild;
    env->api->OCISodaRemove                   = OCISodaRemove;
    env->api->OCISodaRemoveOneWithKey         = OCISodaRemoveOneWithKey;
    env->api->OCISodaReplOne                  = OCISodaReplOne;
    env->api->OCISodaReplOneAndGet            = OCISodaReplOneAndGet;
    env->api->OCISodaReplOneAndGetWithCtnt    = OCISodaReplOneAndGetWithCtnt;
    env->api->OCISodaReplOneAndGetWithKey     = OCISodaReplOneAndGetWithKey;
    env->api->OCISodaReplOneWithCtnt          = OCISodaReplOneWithCtnt;
    env->api->OCISodaReplOneWithKey           = OCISodaReplOneWithKey;
#endif
#if OCI_VERSION_COMPILE >= OCI_18_3
    env->api->OCISodaOperKeysSet = OCISodaOperKeysSet;
#endif
#if OCI_VERSION_COMPILE >= OCI_18_5
    env->api->OCISodaBulkInsert               = OCISodaBulkInsert;
    env->api->OCISodaBulkInsertAndGet         = OCISodaBulkInsertAndGet;
    env->api->OCISodaBulkInsertAndGetWithCtnt = OCISodaBulkInsertAndGetWithCtnt;
    env->api->OCISodaBulkInsertAndGetWithOpts = OCISodaBulkInsertAndGetWithOpts;
    env->api->OCISodaBulkInsertWithCtnt       = OCISodaBulkInsertWithCtnt;
#endif
#if OCI_VERSION_COMPILE >= OCI_19_3
    env->api->OCIServerDataLengthGet = OCIServerDataLengthGet;
#endif
#if OCI_VERSION_COMPILE >= OCI_20_1
    env->api->OCISodaCollTruncate = OCISodaCollTruncate;
#endif
#if OCI_VERSION_COMPILE >= OCI_21_1
    env->api->OCIJsonBinaryBufferLoad = OCIJsonBinaryBufferLoad;
    env->api->OCIJsonBinaryLengthGet  = OCIJsonBinaryLengthGet;
    env->api->OCIJsonClone            = OCIJsonClone;
    env->api->OCIJsonDomDocGet        = OCIJsonDomDocGet;
    env->api->OCIJsonDomDocSet        = OCIJsonDomDocSet;
    env->api->OCIJsonTextBufferParse  = OCIJsonTextBufferParse;
    env->api->OCIJsonToBinaryBuffer   = OCIJsonToBinaryBuffer;
    env->api->OCIJsonToTextBuffer     = OCIJsonToTextBuffer;
#endif
#if OCI_VERSION_COMPILE >= OCI_23_4
    env->api->OCIVectorFromArray    = OCIVectorFromArray;
    env->api->OCIVectorFromText     = OCIVectorFromText;
    env->api->OCIVectorToArray      = OCIVectorToArray;
    env->api->OCIVectorToText       = OCIVectorToText;
    env->api->OCIJsonSchemaValidate = OCIJsonSchemaValidate;
#endif
#else /* OCI_IMPORT_RUNTIME */

    /* Now loading all symbols - no check is performed on each function,
        Basic checks will be done to ensure we're loading an
        Oracle and compatible library ...
    */

    LIB_SYMBOL(env->lib_handle, "OCIInitialize", env->api->OCIInitialize, OCIINITIALIZE);
    LIB_SYMBOL(env->lib_handle, "OCIEnvInit", env->api->OCIEnvInit, OCIENVINIT);
    LIB_SYMBOL(env->lib_handle, "OCIEnvCreate", env->api->OCIEnvCreate, OCIENVCREATE);
    LIB_SYMBOL(env->lib_handle, "OCIServerAttach", env->api->OCIServerAttach, OCISERVERATTACH);
    LIB_SYMBOL(env->lib_handle, "OCIServerDetach", env->api->OCIServerDetach, OCISERVERDETACH);
    LIB_SYMBOL(env->lib_handle, "OCIHandleAlloc", env->api->OCIHandleAlloc, OCIHANDLEALLOC);
    LIB_SYMBOL(env->lib_handle, "OCIHandleFree", env->api->OCIHandleFree, OCIHANDLEFREE);
    LIB_SYMBOL(env->lib_handle, "OCIDescriptorAlloc", env->api->OCIDescriptorAlloc, OCIDESCRIPTORALLOC);
    LIB_SYMBOL(env->lib_handle, "OCIDescriptorFree", env->api->OCIDescriptorFree, OCIDESCRIPTORFREE);
    LIB_SYMBOL(env->lib_handle, "OCISessionBegin", env->api->OCISessionBegin, OCISESSIONBEGIN);
    LIB_SYMBOL(env->lib_handle, "OCISessionEnd", env->api->OCISessionEnd, OCISESSIONEND);
    LIB_SYMBOL(env->lib_handle, "OCIPasswordChange", env->api->OCIPasswordChange, OCIPASSWORDCHANGE);
    LIB_SYMBOL(env->lib_handle, "OCIBindByPos", env->api->OCIBindByPos, OCIBINDBYPOS);
    LIB_SYMBOL(env->lib_handle, "OCIBindByName", env->api->OCIBindByName, OCIBINDBYNAME);
    LIB_SYMBOL(env->lib_handle, "OCIBindDynamic", env->api->OCIBindDynamic, OCIBINDDYNAMIC);
    LIB_SYMBOL(env->lib_handle, "OCIBindObject", env->api->OCIBindObject, OCIBINDOBJECT);
    LIB_SYMBOL(env->lib_handle, "OCIDefineByPos", env->api->OCIDefineByPos, OCIDEFINEBYPOS);
    LIB_SYMBOL(env->lib_handle, "OCIDefineDynamic", env->api->OCIDefineDynamic, OCIDEFINEDYNAMIC);
    LIB_SYMBOL(env->lib_handle, "OCIDefineObject", env->api->OCIDefineObject, OCIDEFINEOBJECT);
    LIB_SYMBOL(env->lib_handle, "OCIStmtPrepare", env->api->OCIStmtPrepare, OCISTMTPREPARE);
    LIB_SYMBOL(env->lib_handle, "OCIStmtExecute", env->api->OCIStmtExecute, OCISTMTEXECUTE);
    LIB_SYMBOL(env->lib_handle, "OCIStmtFetch", env->api->OCIStmtFetch, OCISTMTFETCH);
    LIB_SYMBOL(env->lib_handle, "OCIStmtFetch2", env->api->OCIStmtFetch2, OCISTMTFETCH2);
    LIB_SYMBOL(env->lib_handle, "OCIStmtGetPieceInfo", env->api->OCIStmtGetPieceInfo, OCISTMTGETPIECEINFO);
    LIB_SYMBOL(env->lib_handle, "OCIStmtSetPieceInfo", env->api->OCIStmtSetPieceInfo, OCISTMTSETPIECEINFO);
    LIB_SYMBOL(env->lib_handle, "OCIParamGet", env->api->OCIParamGet, OCIPARAMGET);
    LIB_SYMBOL(env->lib_handle, "OCIParamSet", env->api->OCIParamSet, OCIPARAMSET);
    LIB_SYMBOL(env->lib_handle, "OCITransStart", env->api->OCITransStart, OCITRANSSTART);
    LIB_SYMBOL(env->lib_handle, "OCITransDetach", env->api->OCITransDetach, OCITRANSDETACH);
    LIB_SYMBOL(env->lib_handle, "OCITransPrepare", env->api->OCITransPrepare, OCITRANSPREPARE);
    LIB_SYMBOL(env->lib_handle, "OCITransForget", env->api->OCITransForget, OCITRANSFORGET);
    LIB_SYMBOL(env->lib_handle, "OCITransCommit", env->api->OCITransCommit, OCITRANSCOMMIT);
    LIB_SYMBOL(env->lib_handle, "OCITransRollback", env->api->OCITransRollback, OCITRANSROLLBACK);
    LIB_SYMBOL(env->lib_handle, "OCIErrorGet", env->api->OCIErrorGet, OCIERRORGET);
    LIB_SYMBOL(env->lib_handle, "OCILobCreateTemporary", env->api->OCILobCreateTemporary, OCILOBCREATETEMPORARY);
    LIB_SYMBOL(env->lib_handle, "OCILobFreeTemporary", env->api->OCILobFreeTemporary, OCILOBFREETEMPORARY);
    LIB_SYMBOL(env->lib_handle, "OCILobIsTemporary", env->api->OCILobIsTemporary, OCILOBISTEMPORARY);
    LIB_SYMBOL(env->lib_handle, "OCILobAppend", env->api->OCILobAppend, OCILOBAPPEND);
    LIB_SYMBOL(env->lib_handle, "OCILobCopy", env->api->OCILobCopy, OCILOBCOPY);
    LIB_SYMBOL(env->lib_handle, "OCILobGetLength", env->api->OCILobGetLength, OCILOBGETLENGTH);
    LIB_SYMBOL(env->lib_handle, "OCILobGetChunkSize", env->api->OCILobGetChunkSize, OCILOBGETCHUNKSIZE);
    LIB_SYMBOL(env->lib_handle, "OCILobRead", env->api->OCILobRead, OCILOBREAD);
    LIB_SYMBOL(env->lib_handle, "OCILobWrite", env->api->OCILobWrite, OCILOBWRITE);
    LIB_SYMBOL(env->lib_handle, "OCILobTrim", env->api->OCILobTrim, OCILOBTRIM);
    LIB_SYMBOL(env->lib_handle, "OCILobErase", env->api->OCILobErase, OCILOBERASE);
    LIB_SYMBOL(env->lib_handle, "OCILobOpen", env->api->OCILobOpen, OCILOBOPEN);
    LIB_SYMBOL(env->lib_handle, "OCILobClose", env->api->OCILobClose, OCILOBCLOSE);
    LIB_SYMBOL(env->lib_handle, "OCILobLocatorAssign", env->api->OCILobLocatorAssign, OCILOBLOCATORASSIGN);
    LIB_SYMBOL(env->lib_handle, "OCILobAssign", env->api->OCILobAssign, OCILOBASSIGN);
    LIB_SYMBOL(env->lib_handle, "OCILobIsEqual", env->api->OCILobIsEqual, OCILOBISEQUAL);
    LIB_SYMBOL(env->lib_handle, "OCILobFlushBuffer", env->api->OCILobFlushBuffer, OCILOBFLUSHBUFFER);
    LIB_SYMBOL(env->lib_handle, "OCILobGetStorageLimit", env->api->OCILobGetStorageLimit, OCILOBGETSTORAGELIMIT);
    LIB_SYMBOL(env->lib_handle, "OCILobEnableBuffering", env->api->OCILobEnableBuffering, OCILOBENABLEBUFFERING);
    LIB_SYMBOL(env->lib_handle, "OCILobDisableBuffering", env->api->OCILobDisableBuffering, OCILOBDISABLEBUFFERING);
    LIB_SYMBOL(env->lib_handle, "OCILobFileOpen", env->api->OCILobFileOpen, OCILOBFILEOPEN);
    LIB_SYMBOL(env->lib_handle, "OCILobFileClose", env->api->OCILobFileClose, OCILOBFILECLOSE);
    LIB_SYMBOL(env->lib_handle, "OCILobFileCloseAll", env->api->OCILobFileCloseAll, OCILOBFILECLOSEALL);
    LIB_SYMBOL(env->lib_handle, "OCILobFileIsOpen", env->api->OCILobFileIsOpen, OCILOBFILEISOPEN);
    LIB_SYMBOL(env->lib_handle, "OCILobFileExists", env->api->OCILobFileExists, OCILOBFILEEXISTS);
    LIB_SYMBOL(env->lib_handle, "OCILobFileGetName", env->api->OCILobFileGetName, OCILOBFIELGETNAME);
    LIB_SYMBOL(env->lib_handle, "OCILobFileSetName", env->api->OCILobFileSetName, OCILOBFILESETNAME);
    LIB_SYMBOL(env->lib_handle, "OCILobLoadFromFile", env->api->OCILobLoadFromFile, OCILOBLOADFROMFILE);
    LIB_SYMBOL(env->lib_handle, "OCILobWriteAppend", env->api->OCILobWriteAppend, OCILOBWRITEAPPEND);
    LIB_SYMBOL(env->lib_handle, "OCIServerVersion", env->api->OCIServerVersion, OCISERVERVERSION);
    LIB_SYMBOL(env->lib_handle, "OCIBreak", env->api->OCIBreak, OCIBREAK);
    LIB_SYMBOL(env->lib_handle, "OCIAttrGet", env->api->OCIAttrGet, OCIATTRGET);
    LIB_SYMBOL(env->lib_handle, "OCIAttrSet", env->api->OCIAttrSet, OCIATTRSET);
    LIB_SYMBOL(env->lib_handle, "OCIDateAssign", env->api->OCIDateAssign, OCIDATEASSIGN);
    LIB_SYMBOL(env->lib_handle, "OCIDateToText", env->api->OCIDateToText, OCIDATETOTEXT);
    LIB_SYMBOL(env->lib_handle, "OCIDateFromText", env->api->OCIDateFromText, OCIDATEFROMTEXT);
    LIB_SYMBOL(env->lib_handle, "OCIDateCompare", env->api->OCIDateCompare, OCIDATECOMPARE);
    LIB_SYMBOL(env->lib_handle, "OCIDateAddMonths", env->api->OCIDateAddMonths, OCIDATEADDMONTHS);
    LIB_SYMBOL(env->lib_handle, "OCIDateAddDays", env->api->OCIDateAddDays, OCIDATEADDDAYS);
    LIB_SYMBOL(env->lib_handle, "OCIDateLastDay", env->api->OCIDateLastDay, OCIDATELASTDAY);
    LIB_SYMBOL(env->lib_handle, "OCIDateDaysBetween", env->api->OCIDateDaysBetween, OCIDATEDAYSBETWEEN);
    LIB_SYMBOL(env->lib_handle, "OCIDateZoneToZone", env->api->OCIDateZoneToZone, OCIDATEZONETOZONE);
    LIB_SYMBOL(env->lib_handle, "OCIDateNextDay", env->api->OCIDateNextDay, OCIDATENEXTDAY);
    LIB_SYMBOL(env->lib_handle, "OCIDateCheck", env->api->OCIDateCheck, OCIDATECHECK);
    LIB_SYMBOL(env->lib_handle, "OCIDateSysDate", env->api->OCIDateSysDate, OCIDATESYSDATE);
    LIB_SYMBOL(env->lib_handle, "OCIDescribeAny", env->api->OCIDescribeAny, OCIDESCRIBEANY);
    LIB_SYMBOL(env->lib_handle, "OCIIntervalAssign", env->api->OCIIntervalAssign, OCIINTERVALASSIGN);
    LIB_SYMBOL(env->lib_handle, "OCIIntervalCheck", env->api->OCIIntervalCheck, OCIINTERVALCHECK);
    LIB_SYMBOL(env->lib_handle, "OCIIntervalCompare", env->api->OCIIntervalCompare, OCIINTERVALCOMPARE);
    LIB_SYMBOL(env->lib_handle, "OCIIntervalFromText", env->api->OCIIntervalFromText, OCIINTERVALFROMTEXT);
    LIB_SYMBOL(env->lib_handle, "OCIIntervalToText", env->api->OCIIntervalToText, OCIINTERVALTOTEXT);
    LIB_SYMBOL(env->lib_handle, "OCIIntervalFromTZ", env->api->OCIIntervalFromTZ, OCIINTERVALFROMTZ);
    LIB_SYMBOL(env->lib_handle, "OCIIntervalGetDaySecond", env->api->OCIIntervalGetDaySecond, OCIINTERVALGETDAYSECOND);
    LIB_SYMBOL(env->lib_handle, "OCIIntervalGetYearMonth", env->api->OCIIntervalGetYearMonth, OCIINTERVALGETYEARMONTH);
    LIB_SYMBOL(env->lib_handle, "OCIIntervalSetDaySecond", env->api->OCIIntervalSetDaySecond, OCIINTERVALSETDAYSECOND);
    LIB_SYMBOL(env->lib_handle, "OCIIntervalSetYearMonth", env->api->OCIIntervalSetYearMonth, OCIINTERVALSETYEARMONTH);
    LIB_SYMBOL(env->lib_handle, "OCIIntervalSubtract", env->api->OCIIntervalSubtract, OCIINTERVALSUBTRACT);
    LIB_SYMBOL(env->lib_handle, "OCIIntervalAdd", env->api->OCIIntervalAdd, OCIINTERVALADD);
    LIB_SYMBOL(env->lib_handle, "OCIDateTimeAssign", env->api->OCIDateTimeAssign, OCIDATETIMEASSIGN);
    LIB_SYMBOL(env->lib_handle, "OCIDateTimeCheck", env->api->OCIDateTimeCheck, OCIDATETIMECHECK);
    LIB_SYMBOL(env->lib_handle, "OCIDateTimeCompare", env->api->OCIDateTimeCompare, OCIDATETIMECOMPARE);
    LIB_SYMBOL(env->lib_handle, "OCIDateTimeConstruct", env->api->OCIDateTimeConstruct, OCIDATETIMECONSTRUCT);
    LIB_SYMBOL(env->lib_handle, "OCIDateTimeConvert", env->api->OCIDateTimeConvert, OCIDATETIMECONVERT);
    LIB_SYMBOL(env->lib_handle, "OCIDateTimeFromArray", env->api->OCIDateTimeFromArray, OCIDATETIMEFROMARRAY);
    LIB_SYMBOL(env->lib_handle, "OCIDateTimeToArray", env->api->OCIDateTimeToArray, OCIDATETIMETOARRAY);
    LIB_SYMBOL(env->lib_handle, "OCIDateTimeFromText", env->api->OCIDateTimeFromText, OCIDATETIMEFROMTEXT);
    LIB_SYMBOL(env->lib_handle, "OCIDateTimeToText", env->api->OCIDateTimeToText, OCIDATETIMETOTEXT);
    LIB_SYMBOL(env->lib_handle, "OCIDateTimeGetDate", env->api->OCIDateTimeGetDate, OCIDATETIMEGETDATE);
    LIB_SYMBOL(env->lib_handle, "OCIDateTimeGetTime", env->api->OCIDateTimeGetTime, OCIDATETIMEGETTIME);
    LIB_SYMBOL(env->lib_handle, "OCIDateTimeGetTimeZoneName", env->api->OCIDateTimeGetTimeZoneName, OCIDATETIMEGETTIMEZONENAME);
    LIB_SYMBOL(env->lib_handle, "OCIDateTimeGetTimeZoneOffset", env->api->OCIDateTimeGetTimeZoneOffset, OCIDATETIMEGETTIMEZONEOFFSET);
    LIB_SYMBOL(env->lib_handle, "OCIDateTimeIntervalAdd", env->api->OCIDateTimeIntervalAdd, OCIDATETIMEINTERVALADD);
    LIB_SYMBOL(env->lib_handle, "OCIDateTimeIntervalSub", env->api->OCIDateTimeIntervalSub, OCIDATETIMEINTERVALSUB);
    LIB_SYMBOL(env->lib_handle, "OCIDateTimeSubtract", env->api->OCIDateTimeSubtract, OCIDATETIMESUBTRACT);
    LIB_SYMBOL(env->lib_handle, "OCIDateTimeSysTimeStamp", env->api->OCIDateTimeSysTimeStamp, OCIDATETIMESYSTIMESTAMP);
    LIB_SYMBOL(env->lib_handle, "OCIArrayDescriptorAlloc", env->api->OCIArrayDescriptorAlloc, OCIARRAYDESCRIPTORALLOC);
    LIB_SYMBOL(env->lib_handle, "OCIArrayDescriptorFree", env->api->OCIArrayDescriptorFree, OCIARRAYDESCRIPTORFREE);
    LIB_SYMBOL(env->lib_handle, "OCIClientVersion", env->api->OCIClientVersion, OCICLIENTVERSION);
    LIB_SYMBOL(env->lib_handle, "OCITypeByRef", env->api->OCITypeByRef, OCITYPEBYREF);
    LIB_SYMBOL(env->lib_handle, "OCITypeByName", env->api->OCITypeByName, OCITYPEBYNAME);
    LIB_SYMBOL(env->lib_handle, "OCINumberToInt", env->api->OCINumberToInt, OCINUMBERTOINT);
    LIB_SYMBOL(env->lib_handle, "OCINumberFromInt", env->api->OCINumberFromInt, OCINUMBERFROMINT);
    LIB_SYMBOL(env->lib_handle, "OCINumberToReal", env->api->OCINumberToReal, OCINUMBERTOREAL);
    LIB_SYMBOL(env->lib_handle, "OCINumberFromReal", env->api->OCINumberFromReal, OCINUMBERFROMREAL);
    LIB_SYMBOL(env->lib_handle, "OCINumberToText", env->api->OCINumberToText, OCINUMBERTOTEXT);
    LIB_SYMBOL(env->lib_handle, "OCINumberFromText", env->api->OCINumberFromText, OCINUMBERFROMTEXT);
    LIB_SYMBOL(env->lib_handle, "OCINumberAssign", env->api->OCINumberAssign, OCINUMBERASSIGN);
    LIB_SYMBOL(env->lib_handle, "OCINumberAdd", env->api->OCINumberAdd, OCINUMBERADD);
    LIB_SYMBOL(env->lib_handle, "OCINumberSub", env->api->OCINumberSub, OCINUMBERSUB);
    LIB_SYMBOL(env->lib_handle, "OCINumberMul", env->api->OCINumberMul, OCINUMBERMUL);
    LIB_SYMBOL(env->lib_handle, "OCINumberDiv", env->api->OCINumberDiv, OCINUMBERDIV);
    LIB_SYMBOL(env->lib_handle, "OCINumberCmp", env->api->OCINumberCmp, OCINUMBERCMP);
    LIB_SYMBOL(env->lib_handle, "OCIStringPtr", env->api->OCIStringPtr, OCISTRINGPTR);
    LIB_SYMBOL(env->lib_handle, "OCIStringSize", env->api->OCIStringSize, OCISTRINGSIZE);
    LIB_SYMBOL(env->lib_handle, "OCIStringAssignText", env->api->OCIStringAssignText, OCISTRINGASSIGNTEXT);
    LIB_SYMBOL(env->lib_handle, "OCIStringResize", env->api->OCIStringResize, OCISTRINGRESIZE);
    LIB_SYMBOL(env->lib_handle, "OCIRawPtr", env->api->OCIRawPtr, OCIRAWPTR);
    LIB_SYMBOL(env->lib_handle, "OCIRawAssignBytes", env->api->OCIRawAssignBytes, OCIRAWASSIGNBYTES);
    LIB_SYMBOL(env->lib_handle, "OCIRawResize", env->api->OCIRawResize, OCIRAWRESIZE);
    LIB_SYMBOL(env->lib_handle, "OCIRawAllocSize", env->api->OCIRawAllocSize, OCIRAWALLOCSIZE);
    LIB_SYMBOL(env->lib_handle, "OCIRawSize", env->api->OCIRawSize, OCIRAWSIZE);
    LIB_SYMBOL(env->lib_handle, "OCIObjectNew", env->api->OCIObjectNew, OCIOBJECTNEW);
    LIB_SYMBOL(env->lib_handle, "OCIObjectFree", env->api->OCIObjectFree, OCIOBJECTFREE);
    LIB_SYMBOL(env->lib_handle, "OCIObjectSetAttr", env->api->OCIObjectSetAttr, OCIOBJECTSETATTR);
    LIB_SYMBOL(env->lib_handle, "OCIObjectGetAttr", env->api->OCIObjectGetAttr, OCIOBJECTGETATTR);
    LIB_SYMBOL(env->lib_handle, "OCIObjectPin", env->api->OCIObjectPin, OCIOBJECTPIN);
    LIB_SYMBOL(env->lib_handle, "OCIObjectUnpin", env->api->OCIObjectUnpin, OCIOBJECTUNPIN);
    LIB_SYMBOL(env->lib_handle, "OCIObjectCopy", env->api->OCIObjectCopy, OCIOBJECTCOPY);
    LIB_SYMBOL(env->lib_handle, "OCIObjectGetObjectRef", env->api->OCIObjectGetObjectRef, OCIOBJECTGETOBJECTREF);
    LIB_SYMBOL(env->lib_handle, "OCIObjectGetTypeRef", env->api->OCIObjectGetTypeRef, OCIOBJECTGETTYPEREF);
    LIB_SYMBOL(env->lib_handle, "OCIObjectGetProperty", env->api->OCIObjectGetProperty, OCIOBJECTGETPROPERTY);
    LIB_SYMBOL(env->lib_handle, "OCIObjectGetInd", env->api->OCIObjectGetInd, OCIOBJECTGETIND);
    LIB_SYMBOL(env->lib_handle, "OCIRefAssign", env->api->OCIRefAssign, OCIREFASSIGN);
    LIB_SYMBOL(env->lib_handle, "OCIRefIsNull", env->api->OCIRefIsNull, OCIREFISNULL);
    LIB_SYMBOL(env->lib_handle, "OCIRefClear", env->api->OCIRefClear, OCIREFCLEAR);
    LIB_SYMBOL(env->lib_handle, "OCIRefToHex", env->api->OCIRefToHex, OCIREFTOHEX);
    LIB_SYMBOL(env->lib_handle, "OCIRefHexSize", env->api->OCIRefHexSize, OCIREFHEXSIZE);
    LIB_SYMBOL(env->lib_handle, "OCIConnectionPoolCreate", env->api->OCIConnectionPoolCreate, OCICONNECTIONPOOLCREATE);
    LIB_SYMBOL(env->lib_handle, "OCIConnectionPoolDestroy", env->api->OCIConnectionPoolDestroy, OCICONNECTIONPOOLDESTROY);
    LIB_SYMBOL(env->lib_handle, "OCISessionPoolCreate", env->api->OCISessionPoolCreate, OCISESSIONPOOLCREATE);
    LIB_SYMBOL(env->lib_handle, "OCISessionPoolDestroy", env->api->OCISessionPoolDestroy, OCISESSIONPOOLDESTROY);
    LIB_SYMBOL(env->lib_handle, "OCISessionGet", env->api->OCISessionGet, OCISESSIONGET);
    LIB_SYMBOL(env->lib_handle, "OCISessionRelease", env->api->OCISessionRelease, OCISESSIONRELEASE);
    LIB_SYMBOL(env->lib_handle, "OCICollSize", env->api->OCICollSize, OCICOLLSIZE);
    LIB_SYMBOL(env->lib_handle, "OCICollMax", env->api->OCICollMax, OCICOLLMAX);
    LIB_SYMBOL(env->lib_handle, "OCICollGetElem", env->api->OCICollGetElem, OCICOLLGETITEM);
    LIB_SYMBOL(env->lib_handle, "OCICollAssignElem", env->api->OCICollAssignElem, OCICOLLASSIGNELEM);
    LIB_SYMBOL(env->lib_handle, "OCICollAssign", env->api->OCICollAssign, OCICOLLASSIGN);
    LIB_SYMBOL(env->lib_handle, "OCICollAppend", env->api->OCICollAppend, OCICOLLAPPEND);
    LIB_SYMBOL(env->lib_handle, "OCICollTrim", env->api->OCICollTrim, OCICOLLTRIM);
    LIB_SYMBOL(env->lib_handle, "OCITableSize", env->api->OCITableSize, OCITABLESIZE);
    LIB_SYMBOL(env->lib_handle, "OCITableDelete", env->api->OCITableDelete, OCITABLEDELETE);
    LIB_SYMBOL(env->lib_handle, "OCIIterCreate", env->api->OCIIterCreate, OCIITERCREATE);
    LIB_SYMBOL(env->lib_handle, "OCIIterDelete", env->api->OCIIterDelete, OCIITERDELETE);
    LIB_SYMBOL(env->lib_handle, "OCIIterInit", env->api->OCIIterInit, OCIITERINIT);
    LIB_SYMBOL(env->lib_handle, "OCIIterNext", env->api->OCIIterNext, OCIITERNEXT);
    LIB_SYMBOL(env->lib_handle, "OCIIterPrev", env->api->OCIIterPrev, OCIITERPREV);
    LIB_SYMBOL(env->lib_handle, "OCIDirPathAbort", env->api->OCIDirPathAbort, OCIDIRPATHABORT);
    LIB_SYMBOL(env->lib_handle, "OCIDirPathDataSave", env->api->OCIDirPathDataSave, OCIDIRPATHDATASAVE);
    LIB_SYMBOL(env->lib_handle, "OCIDirPathFinish", env->api->OCIDirPathFinish, OCIDIRPATHFINISH);
    LIB_SYMBOL(env->lib_handle, "OCIDirPathPrepare", env->api->OCIDirPathPrepare, OCIDIRPATHPREPARE);
    LIB_SYMBOL(env->lib_handle, "OCIDirPathLoadStream", env->api->OCIDirPathLoadStream, OCIDIRPATHLOADSTREAM);
    LIB_SYMBOL(env->lib_handle, "OCIDirPathColArrayEntrySet", env->api->OCIDirPathColArrayEntrySet, OCIDIRPATHCOLARRAYENTRYSET);
    LIB_SYMBOL(env->lib_handle, "OCIDirPathColArrayReset", env->api->OCIDirPathColArrayReset, OCIDIRPATHCOLARRAYRESET);
    LIB_SYMBOL(env->lib_handle, "OCIDirPathColArrayToStream", env->api->OCIDirPathColArrayToStream, OCIDIRPATHCOLARRAYTOSTREAM);
    LIB_SYMBOL(env->lib_handle, "OCIDirPathStreamReset", env->api->OCIDirPathStreamReset, OCIDIRPATHSTREAMRESET);
    LIB_SYMBOL(env->lib_handle, "OCIDirPathFlushRow", env->api->OCIDirPathFlushRow, OCIDIRPATHFLUSHROW);
    LIB_SYMBOL(env->lib_handle, "OCICacheFree", env->api->OCICacheFree, OCICACHEFREE);
    LIB_SYMBOL(env->lib_handle, "OCIPing", env->api->OCIPing, OCIPING);
    LIB_SYMBOL(env->lib_handle, "OCIAQEnq", env->api->OCIAQEnq, OCIAQENQ);
    LIB_SYMBOL(env->lib_handle, "OCIAQDeq", env->api->OCIAQDeq, OCIAQDEQ);
    LIB_SYMBOL(env->lib_handle, "OCIAQListen", env->api->OCIAQListen, OCIAQLISTEN);
    LIB_SYMBOL(env->lib_handle, "OCIDBStartup", env->api->OCIDBStartup, OCIDBSTARTUP);
    LIB_SYMBOL(env->lib_handle, "OCIDBShutdown", env->api->OCIDBShutdown, OCIDBSHUTDOWN);
    LIB_SYMBOL(env->lib_handle, "OCIEnvNlsCreate", env->api->OCIEnvNlsCreate, OCIENVNLSCREATE);
    LIB_SYMBOL(env->lib_handle, "OCINlsGetInfo", env->api->OCINlsGetInfo, OCINLSGETINFO);
    LIB_SYMBOL(env->lib_handle, "OCIStmtPrepare2", env->api->OCIStmtPrepare2, OCISTMTPREPARE2);
    LIB_SYMBOL(env->lib_handle, "OCIStmtRelease", env->api->OCIStmtRelease, OCISTMTRELEASE);
    LIB_SYMBOL(env->lib_handle, "OCISubscriptionRegister", env->api->OCISubscriptionRegister, OCISUBSCRIPTIONREGISTER);
    LIB_SYMBOL(env->lib_handle, "OCISubscriptionUnRegister", env->api->OCISubscriptionUnRegister, OCISUBSCRIPTIONUNREGISTER);
    LIB_SYMBOL(env->lib_handle, "OCILobGetContentType", env->api->OCILobGetContentType, OCILOBGETCONTENTTYPE);
    LIB_SYMBOL(env->lib_handle, "OCIStmtGetNextResult", env->api->OCIStmtGetNextResult, OCISTMTGETNEXTRESULT);
    LIB_SYMBOL(env->lib_handle, "OCIServerRelease2", env->api->OCIServerRelease2, OCISERVERRELEASE2);
    LIB_SYMBOL(env->lib_handle, "OCISodaOperKeysSet", env->api->OCISodaOperKeysSet, OCISODAOPERKEYSSET);
    LIB_SYMBOL(env->lib_handle, "OCIRowidToChar", env->api->OCIRowidToChar, OCIROWIDTOCHAR);
    LIB_SYMBOL(env->lib_handle, "OCIStmtGetBindInfo", env->api->OCIStmtGetBindInfo, OCISTMTGETBINDINFO);
    LIB_SYMBOL(env->lib_handle, "OCISodaBulkInsert", env->api->OCISodaBulkInsert, OCISODABULKINSERT);

    LIB_SYMBOL(env->lib_handle, "OCISodaCollCreate", env->api->OCISodaCollCreate, OCISODACOLLCREATE);
    LIB_SYMBOL(env->lib_handle, "OCISodaMetadataBuild", env->api->OCISodaMetadataBuild, OCISODAMETADATABUILD);
    LIB_SYMBOL(env->lib_handle, "OCISodaCollCreateWithMetadata", env->api->OCISodaCollCreateWithMetadata, OCISODACOLLCREATEWITHMETADATA);
    LIB_SYMBOL(env->lib_handle, "OCISodaCollOpen", env->api->OCISodaCollOpen, OCISODACOLLOPEN);
    LIB_SYMBOL(env->lib_handle, "OCISodaDocCreate", env->api->OCISodaDocCreate, OCISODADOCCREATE);
    LIB_SYMBOL(env->lib_handle, "OCISodaDocCreateWithKey", env->api->OCISodaDocCreateWithKey, OCISODADOCCREATEWITHKEY);
    LIB_SYMBOL(env->lib_handle, "OCISodaDocCreateWithKeyAndMType", env->api->OCISodaDocCreateWithKeyAndMType,
               OCISODADOCCREATEWITHKEYANDMTYPE);
    LIB_SYMBOL(env->lib_handle, "OCISodaInsert", env->api->OCISodaInsert, OCISODAINSERT);
    LIB_SYMBOL(env->lib_handle, "OCISodaInsertAndGet", env->api->OCISodaInsertAndGet, OCISODAINSERTANDGET);
    LIB_SYMBOL(env->lib_handle, "OCISodaInsertWithCtnt", env->api->OCISodaInsertWithCtnt, OCISODAINSERTWITHCTNT);
    LIB_SYMBOL(env->lib_handle, "OCISodaInsertAndGetWithCtnt", env->api->OCISodaInsertAndGetWithCtnt, OCISODAINSERTANDGETWITHCTNT);
    LIB_SYMBOL(env->lib_handle, "OCISodaFind", env->api->OCISodaFind, OCISODAFIND);
    LIB_SYMBOL(env->lib_handle, "OCISodaFindOne", env->api->OCISodaFindOne, OCISODAFINDONE);
    LIB_SYMBOL(env->lib_handle, "OCISodaFindOneWithKey", env->api->OCISodaFindOneWithKey, OCISODAFINDONEWITHKEY);
    LIB_SYMBOL(env->lib_handle, "OCISodaDocGetNext", env->api->OCISodaDocGetNext, OCISODADOCGETNEXT);
    LIB_SYMBOL(env->lib_handle, "OCISodaCollList", env->api->OCISodaCollList, OCISODACOLLLIST);
    LIB_SYMBOL(env->lib_handle, "OCISodaCollGetNext", env->api->OCISodaCollGetNext, OCISODACOLLGETNEXT);
    LIB_SYMBOL(env->lib_handle, "OCISodaDocCount", env->api->OCISodaDocCount, OCISODADOCCOUNT);
    LIB_SYMBOL(env->lib_handle, "OCISodaDocCountWithFilter", env->api->OCISodaDocCountWithFilter, OCISODADOCCOUNTWITHFILTER);
    LIB_SYMBOL(env->lib_handle, "OCISodaReplOne", env->api->OCISodaReplOne, OCISODAREPLONE);
    LIB_SYMBOL(env->lib_handle, "OCISodaReplOneAndGet", env->api->OCISodaReplOneAndGet, OCISODAREPLONEANDGET);
    LIB_SYMBOL(env->lib_handle, "OCISodaReplOneWithKey", env->api->OCISodaReplOneWithKey, OCISODAREPLONEWITHKEY);
    LIB_SYMBOL(env->lib_handle, "OCISodaReplOneAndGetWithKey", env->api->OCISodaReplOneAndGetWithKey, OCISODAREPLONEANDGETWITHKEY);
    LIB_SYMBOL(env->lib_handle, "OCISodaReplOneWithCtnt", env->api->OCISodaReplOneWithCtnt, OCISODAREPLONEWITHCTNT);
    LIB_SYMBOL(env->lib_handle, "OCISodaReplOneAndGetWithCtnt", env->api->OCISodaReplOneAndGetWithCtnt, OCISODAREPLONEANDGETWITHCTNT);
    LIB_SYMBOL(env->lib_handle, "OCISodaRemove", env->api->OCISodaRemove, OCISODAREMOVE);
    LIB_SYMBOL(env->lib_handle, "OCISodaRemoveOneWithKey", env->api->OCISodaRemoveOneWithKey, OCISODAREMOVEONEWITHKEY);
    LIB_SYMBOL(env->lib_handle, "OCISodaSave", env->api->OCISodaSave, OCISODASAVE);
    LIB_SYMBOL(env->lib_handle, "OCISodaSaveAndGet", env->api->OCISodaSaveAndGet, OCISODASAVEANDGET);
    LIB_SYMBOL(env->lib_handle, "OCISodaIndexList", env->api->OCISodaIndexList, OCISODAINDEXLIST);
    LIB_SYMBOL(env->lib_handle, "OCISodaCollDrop", env->api->OCISodaCollDrop, OCISODACOLLDROP);
    LIB_SYMBOL(env->lib_handle, "OCISodaIndexCreate", env->api->OCISodaIndexCreate, OCISODAINDEXCREATE);
    LIB_SYMBOL(env->lib_handle, "OCISodaIndexDrop", env->api->OCISodaIndexDrop, OCISODAINDEXDROP);
    LIB_SYMBOL(env->lib_handle, "OCISodaDataGuideGet", env->api->OCISodaDataGuideGet, OCISODADATAGUIDEGET);
    LIB_SYMBOL(env->lib_handle, "OCISodaBulkInsertWithCtnt", env->api->OCISodaBulkInsertWithCtnt, OCISODABULKINSERTWITHCTNT);
    LIB_SYMBOL(env->lib_handle, "OCISodaBulkInsertAndGet", env->api->OCISodaBulkInsertAndGet, OCISODABULKINSERTANDGET);
    LIB_SYMBOL(env->lib_handle, "OCISodaBulkInsertAndGetWithCtnt", env->api->OCISodaBulkInsertAndGetWithCtnt,
               OCISODABULKINSERTANDGETWITHCTNT);
    LIB_SYMBOL(env->lib_handle, "OCISodaBulkInsertAndGetWithOpts", env->api->OCISodaBulkInsertAndGetWithOpts,
               OCISODABULKINSERTANDGETWITHOPTS);
    LIB_SYMBOL(env->lib_handle, "OCISodaCollTruncate", env->api->OCISodaCollTruncate, OCISODACOLLTRUNCATE);

    LIB_SYMBOL(env->lib_handle, "OCIServerDataLengthGet", env->api->OCIServerDataLengthGet, OCISERVERDATALENGTHGET);
    LIB_SYMBOL(env->lib_handle, "OCIVectorToArray", env->api->OCIVectorToArray, OCIVECTORTOARRAY);
    LIB_SYMBOL(env->lib_handle, "OCIVectorFromArray", env->api->OCIVectorFromArray, OCIVECTORFROMARRAY);
    LIB_SYMBOL(env->lib_handle, "OCIVectorToText", env->api->OCIVectorToText, OCIVECTORTOTEXT);
    LIB_SYMBOL(env->lib_handle, "OCIVectorFromText", env->api->OCIVectorFromText, OCIVECTORFROMTEXT);
    LIB_SYMBOL(env->lib_handle, "OCIJsonSchemaValidate", env->api->OCIJsonSchemaValidate, OCIJSONSCHEMAVALIDATE);

    LIB_SYMBOL(env->lib_handle, "OCIJsonTextBufferParse", env->api->OCIJsonTextBufferParse, OCIJSONTEXTBUFFERPARSE);
    LIB_SYMBOL(env->lib_handle, "OCIJsonToTextBuffer",    env->api->OCIJsonToTextBuffer,    OCIJSONTOTEXTBUFFER);
    LIB_SYMBOL(env->lib_handle, "OCIJsonBinaryBufferLoad",env->api->OCIJsonBinaryBufferLoad, OCIJSONBINARYBUFFERLOAD);
    LIB_SYMBOL(env->lib_handle, "OCIJsonToBinaryBuffer",  env->api->OCIJsonToBinaryBuffer,  OCIJSONTOBINARYBUFFER);
    LIB_SYMBOL(env->lib_handle, "OCIJsonDomDocGet",       env->api->OCIJsonDomDocGet,       OCIJSONDOMDOCGET);
    LIB_SYMBOL(env->lib_handle, "OCIJsonDomDocSet",       env->api->OCIJsonDomDocSet,       OCIJSONDOMDOCSET);
    LIB_SYMBOL(env->lib_handle, "OCIJsonClone",           env->api->OCIJsonClone,           OCIJSONCLONE);
    LIB_SYMBOL(env->lib_handle, "OCIJsonBinaryLengthGet", env->api->OCIJsonBinaryLengthGet, OCIJSONBINARYLENGTHGET);

    LIB_SYMBOL(env->lib_handle, "OCIShardingKeyColumnAdd", env->api->OCIShardingKeyColumnAdd, OCISHARDINGKEYCOLUMNADD);
    LIB_SYMBOL(env->lib_handle, "OCIShardingKeyReset",     env->api->OCIShardingKeyReset,     OCISHARDINGKEYRESET);
    LIB_SYMBOL(env->lib_handle, "OCIShardInstancesGet",    env->api->OCIShardInstancesGet,    OCISHARDINSTANCESGET);

  #ifdef ORAXB8_DEFINED
    LIB_SYMBOL(env->lib_handle, "OCILobCopy2", env->api->OCILobCopy2, OCILOBCOPY2);
    LIB_SYMBOL(env->lib_handle, "OCILobErase2", env->api->OCILobErase2, OCILOBERASE2);
    LIB_SYMBOL(env->lib_handle, "OCILobGetLength2", env->api->OCILobGetLength2, OCILOBGETLENGTH2);
    LIB_SYMBOL(env->lib_handle, "OCILobLoadFromFile2", env->api->OCILobLoadFromFile2, OCILOBLOADFROMFILE2);
    LIB_SYMBOL(env->lib_handle, "OCILobRead2", env->api->OCILobRead2, OCILOBREAD2);
    LIB_SYMBOL(env->lib_handle, "OCILobTrim2", env->api->OCILobTrim2, OCILOBTRIM2);
    LIB_SYMBOL(env->lib_handle, "OCILobWrite2", env->api->OCILobWrite2, OCILOBWRITE2);
    LIB_SYMBOL(env->lib_handle, "OCILobWriteAppend2", env->api->OCILobWriteAppend2, OCILOBWRITEAPPEND2);
  #endif

#endif /* OCI_IMPORT_LINKAGE */

    WARNING_RESTORE_UNSAFE_CONVERT
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentGetVariable
 *
 * @note
 * Values are allocated with MemoryAlloc() and need to be freed by the caller using MemoryFree()
 *
 * --------------------------------------------------------------------------------------------- */

char * OcilibEnvironmentGetVariable
(
    OCI_Environment *env,
    const char      *name
)
{
    ENTER_FUNC
    (
        /* returns */ char*, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);
    CHECK_PTR(OCI_IPC_STRING, name);

    char* value = NULL;
    char *str   = getenv(name);

    if (NULL != str)
    {
        value = OcilibStringAnsiDuplicate(env, str);
    }

#if defined(_WINDOWS)

    else
    {
        /* Variables set using SetEnvironmentVariable() on Windows are not seen by MSVC
         * implementation of getenv() !! */

        /* Thus, let's check if they can be retrieved using GetEnvironmentVariable() */
        OCI_UInt32 size = GetEnvironmentVariableA(name, NULL, 0);
        if (size > 0)
        {
            value = OcilibMemoryAlloc(env, OCI_IPC_STRING, size, 1, TRUE);
            size  = GetEnvironmentVariableA(name, value, size);
            if (size == 0)
            {
                OcilibMemoryFree(env, value);
            }
        }
    }

#endif

    SET_RETVAL(value);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Environment * OcilibEnvironmentCreate
(
    POCI_ERROR   err_handler,
    const otext *lib_path,
    OCI_EnvMode  mode,
    OCI_UInt32   version
)
{
    OCI_Environment *env = (OCI_Environment *) calloc(1, sizeof(*env));

    if (env == NULL)
    {
        return NULL;
    }

    env->env   = env;
    env->htype = OCI_IPC_ENVIRONMENT;

    memcpy(env->locale_strings, LocaleStringDefaults, sizeof(env->locale_strings));

    env->api = (OCI_Symbols *) calloc(1, sizeof(OCI_Symbols));

    if (env->api == NULL)
    {
        free(env);
        return NULL;
    }

    /* Initialize logging and error thread-local storage from environment
       variables before any OCI or memory helper call. */
    OcilibLogInitialize();

    if (!OcilibErrorInitialize())
    {
        free(env->api);
        free(env);
        return NULL;
    }

    ENTER_FUNC
    (
        /* returns */ OCI_Environment *, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    OCI_UInt32 i = 0;

    sword ret = OCI_SUCCESS;

    ub4 oci_mode = OCI_ENV_MODE | OCI_OBJECT;

#ifdef OCI_IMPORT_RUNTIME

    char path[OCI_SIZE_BUFFER+1];

    OCI_Size len = (OCI_Size) 0;

#endif

#if defined(OCI_CHARSET_WIDE) && (OCI_VERSION_COMPILE >= OCI_10_2)

    /* activate support for N' substitution (nchar replacement in SQL statement strings) */

    oci_mode |= OCI_NCHAR_LITERAL_REPLACE_ON;

#endif

    env->error_handler      = err_handler;
    env->version_compile    = OCI_VERSION_COMPILE;
    env->env_mode           = mode;
    env->charset            = (sizeof(otext) == sizeof(wchar_t)) ? OCI_CHAR_WIDE : OCI_CHAR_ANSI;
    env->use_wide_char_conv = (IS_WIDE_CHAR_ENV() && (WCHAR_MAX == WCHAR_4_BYTES));

    for (i = 0; i < OCI_FMT_COUNT; i++)
    {
        env->formats[i] = OcilibStringDuplicate(env, FormatDefaultValues[i]);
    }

    /* load any specific environment variable */

    for (i = 0; i < OCI_VARS_COUNT; i++)
    {
        char *value = OcilibEnvironmentGetVariable(env, EnvironmentVarNames[i]);

        env->env_vars[i] = value && (OcilibStringAnsiCaseCompare(value, OCI_VARS_TRUE_VALUE) == 0 || atoi(value) == 1);

        FREE(value);
    }

    /* test for UTF8 environment */

    if (IS_ANSI_ENV())
    {
        char *str = OcilibEnvironmentGetVariable(env, "NLS_LANG");

        if (str)
        {
            char nls_lang[OCI_SIZE_OBJ_NAME + 1] = "";

            strncat(nls_lang, str, OCI_SIZE_OBJ_NAME);

            for (char* ptr = nls_lang; *ptr; ptr++)
            {
                *ptr = (char) toupper(*ptr);
            }

            env->nls_utf8 = (NULL != strstr(nls_lang, "UTF8"));

            FREE(str);
        }
    }

#ifdef OCI_IMPORT_RUNTIME

    memset(path, 0, sizeof(path));

  #ifdef OCI_CHARSET_WIDE

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

    if ((len > (OCI_Size) 0) && (len < sizeof(path) -1) && (OCI_CHAR_SLASH != path[len - (OCI_Size) 1]))
    {
        path[len] = OCI_CHAR_SLASH;
        len++;
    }

    strncat(path, OCI_DL_ANSI_NAME, sizeof(path) - len - 1);

    env->lib_handle = LIB_OPEN(path);

    if (env->lib_handle == NULL)
    {
        THROW_NO_ARGS(OcilibExceptionLoadingSharedLib);
    }

#else

    OCI_NOT_USED(lib_path);

#endif

    OcilibEnvironmentLoadSymbols(env);

    /* API Version checking */

    env->version_runtime = OcilibEnvironmentComputeRuntimeVersion(env, version);

    if (OCI_UNKNOWN == env->version_runtime)
    {
#ifdef OCI_IMPORT_RUNTIME
        if (env->lib_handle)
        {
            LIB_CLOSE(env->lib_handle);
        }
#endif
        THROW_NO_ARGS(OcilibExceptionLoadingSymbols);
    }

    if ((env->version_runtime < OCI_8_1) && (mode & OCI_ENV_THREADED))
    {
        THROW(OcilibExceptionNotAvailable, OCI_FEATURE_MULTITHREADING);
    }

#if defined(OCI_BIG_UINT_ENABLED)

    if ((env->version_runtime >= OCI_10_1) && env->api->OCILobCopy2)
    {
        env->use_lob_ub8 = TRUE;
    }

#endif

#if defined(OCI_STMT_SCROLLABLE_READONLY)

    if ((env->version_runtime >= OCI_9_0) && env->api->OCIStmtFetch2)
    {
        env->use_scrollable_cursors = TRUE;
    }

#endif

#if defined(OCI_CHARSET_WIDE)

    /* Oracle 8i does not support full Unicode mode */

    if (env->version_runtime < OCI_9_0)
    {
        THROW(OcilibExceptionNotAvailable, OCI_FEATURE_WIDE_USERDATA);
    }

#endif

    /* EnvironmentInitialize OCI environment */

    if (mode & OCI_ENV_THREADED)
    {
        oci_mode |= OCI_THREADED;
    }

    if (mode & OCI_ENV_EVENTS)
    {
        oci_mode |= OCI_EVENTS;
    }

    /* create environment on success */

    if (env->version_runtime >= OCI_9_2)
    {
        ub4 nls_mode = (ub4) oci_mode;
        ub2 charset  = OCI_DEFAULT;

#ifdef OCI_CHARSET_WIDE

        nls_mode = (ub4) nls_mode &~OCI_ENV_MODE;
        charset  = OCI_UTF16ID;

#endif

        OCI_CALL(ret, OCIEnvNlsCreate, &env->handle, nls_mode,
                 (dvoid *) env,
                 OcilibMemoryAllocOracleCallback,
                 OcilibMemoryReallocOracleCallback,
                 OcilibMemoryFreeOracleCallback,
                 (OCI_Size) 0, (dvoid **) NULL,
                 charset,
                 charset);

    }
    else if (env->version_runtime >= OCI_8_1)
    {
        OCI_CALL(ret, OCIEnvCreate, &env->handle, oci_mode,
                 (dvoid *) env,
                 OcilibMemoryAllocOracleCallback,
                 OcilibMemoryReallocOracleCallback,
                 OcilibMemoryFreeOracleCallback,
                 (OCI_Size) 0, (dvoid **) NULL);
    }
    else if (env->version_runtime == OCI_8_0)
    {
        OCI_CALL(ret, OCIInitialize, oci_mode,
                 (dvoid *) env,
                 OcilibMemoryAllocOracleCallback,
                 OcilibMemoryReallocOracleCallback,
                 OcilibMemoryFreeOracleCallback);

        if (OCI_SUCCESSFUL(ret))
        {
            OCI_CALL(ret, OCIEnvInit, &env->handle, OCI_DEFAULT, (OCI_Size)0, (dvoid**)NULL);
        }
    }
    else
    {
        ret = OCI_ERROR;
    }

    /*  allocate error handle */
    if (OCI_SUCCESSFUL(ret))
    {
        CHECK
        (
            OcilibMemoryAllocHandle
            (
                env,
                (dvoid *)env->handle,
                (dvoid **) (void *) &env->err,
                OCI_HTYPE_ERROR
            )
        );
    }
    else
    {
        THROW_NO_ARGS(OcilibExceptionOCIEnvironment);
    }

    /* create the environment mutex when running in threaded mode.
       It is recursive because guarded sections (e.g. the locale string
       lazy fill) call back into the memory layer, which locks it again
       through MUTEXED_CALL. */

    if (env->env_mode & OCI_ENV_THREADED)
    {
        if (mtx_init(&env->mutex, mtx_recursive) != thrd_success)
        {
            THROW(OcilibExceptionMemory, OCI_IPC_ENVIRONMENT, 0);
        }
    }

    /* allocate connections internal list */

    env->cons = OcilibListCreate(env, OCI_IPC_CONNECTION);
    CHECK_NULL(env->cons);

    /* allocate pools internal list */

    env->pools = OcilibListCreate(env, OCI_IPC_POOL);
    CHECK_NULL(env->pools);

    /* allocate array internal list */

    env->arrs = OcilibListCreate(env, OCI_IPC_ARRAY);
    CHECK_NULL(env->arrs);

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* allocate connection pools internal list */

    env->subs = OcilibListCreate(env, OCI_IPC_NOTIFY);
    CHECK_NULL(env->subs);

#endif

    SET_RETVAL(env);

    CLEANUP_AND_EXIT_FUNC
    (
        if (!ocilib_call_status && env)
        {
            free(env->api);
            free(env);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibEnvironmentFree
(
    OCI_Environment *env
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    OCI_Boolean success = FALSE;

    OCI_UInt32 i = 0;

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);

    success = TRUE;

    /* dispose list items; objects whose dispose failed stay in their list
       (and were logged) so the failures are reported below instead of being
       silently cleared */

    LIST_ATOMIC_REMOVE_ALL(env->arrs,  OcilibArrayDispose);
    LIST_ATOMIC_REMOVE_ALL(env->subs,  OcilibSubscriptionDispose);
    LIST_ATOMIC_REMOVE_ALL(env->cons,  OcilibConnectionDispose);
    LIST_ATOMIC_REMOVE_ALL(env->pools, OcilibPoolDispose);

    if (env->arrs->count || env->subs->count || env->cons->count || env->pools->count)
    {
        success = FALSE;

        OCILIB_LOG(OCI_LOG_CAT_LIB, OCI_LOG_WARN,
                   OTEXT("Memory Leak: arrays=%u, subscriptions=%u, connections=%u, pools=%u"),
                   (OCI_UInt32)env->arrs->count, (OCI_UInt32)env->subs->count,
                   (OCI_UInt32)env->cons->count, (OCI_UInt32)env->pools->count);
    }

    /* free all lists */

    OcilibListFree(env->arrs);
    OcilibListFree(env->subs);
    OcilibListFree(env->cons);
    OcilibListFree(env->pools);

    env->cons  = NULL;
    env->pools = NULL;
    env->subs  = NULL;

    for (i = 0; i < OCI_FMT_COUNT; i++)
    {
        FREE(env->formats[i]);
    }

    for (i = 0; i < OCI_LSI_COUNT; i++)
    {
        FREE(env->locale_strings[i].str);
    }

    /* free the environment mutex */

    if (env->env_mode & OCI_ENV_THREADED)
    {
        mtx_destroy(&env->mutex);
    }

    /* close error handle */

    if (NULL != env->err)
    {
        OcilibMemoryFreeHandle(env, env->err, OCI_HTYPE_ERROR);
    }

    /* close environment handle */

    if (env->handle)
    {
        env->api->OCIHandleFree(env->handle, OCI_HTYPE_ENV);
    }

#ifdef OCI_IMPORT_RUNTIME

    if (NULL != env->lib_handle)
    {
        LIB_CLOSE(env->lib_handle);
    }

#endif

    /* checks for non freed handles */

    if (env->nb_hndlp > 0)
    {
        OcilibExceptionUnfreedData(&ocilib_call_context, OCI_HDLE_HANDLE, (OCI_Int32) env->nb_hndlp);
        success = FALSE;
    }

    /* checks for non freed descriptors */

    if (env->nb_descp > 0)
    {
        OcilibExceptionUnfreedData(&ocilib_call_context, OCI_HDLE_DESCRIPTOR, (OCI_Int32) env->nb_descp);
        success = FALSE;
    }

    /* checks for non freed objects */

    if (env->nb_objinst > 0)
    {
        OcilibExceptionUnfreedData(&ocilib_call_context, OCI_HDLE_OBJECT, (OCI_Int32) env->nb_objinst);
        success = FALSE;
    }

    /* checks for non freed library memory */

    if (env->mem_bytes_lib > 0)
    {
        OcilibExceptionUnfreedBytes(&ocilib_call_context, env->mem_bytes_lib);
        success = FALSE;
    }

    /* reset environment object */

    free(env->api);
    env->api = NULL;

    OcilibLogCleanup();
    OcilibErrorCleanup();

    free(env);

    CHECK(TRUE);

    CLEANUP_AND_EXIT_FUNC
    (
        SET_RETVAL(success);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentGetOCICompileVersion
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibEnvironmentGetOCICompileVersion
(
    OCI_Environment *env
)
{
    GET_LIB_PROP
    (
        env,
        /* result */ OCI_UInt32, OCI_UNKNOWN,
        /* value  */ env->version_compile
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentGetOCIRuntimeVersion
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibEnvironmentGetOCIRuntimeVersion
(
    OCI_Environment *env
)
{
    GET_LIB_PROP
    (
        env,
        /* result */ OCI_UInt32, OCI_UNKNOWN,
        /* value  */ env->version_runtime
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentGetOCIRuntimeImportMode
 * --------------------------------------------------------------------------------------------- */

OCI_ImportMode OcilibEnvironmentGetOCIRuntimeImportMode
(
    OCI_Environment *env
)
{
    GET_LIB_PROP
    (
        env,
        /* result */ OCI_ImportMode, OCI_UNKNOWN,
        /* value  */ OCI_IMPORT_MODE
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentGetCharset
 * --------------------------------------------------------------------------------------------- */

OCI_Charset OcilibEnvironmentGetCharset
(
    OCI_Environment *env
)
{
    GET_LIB_PROP
    (
        env,
        /* result */ OCI_Charset, OCI_UNKNOWN,
        /* value  */ OCI_CHAR_TEXT
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentGetStats
 * --------------------------------------------------------------------------------------------- */

OCI_UInt64 OcilibEnvironmentGetStats
(
    OCI_Environment *env,
    OCI_Stat         stat
)
{
    OCI_UInt64 value = 0;

    if (NULL != env)
    {
        switch (stat)
        {
            case OCI_UST_BYTES:
            {
                value = env->mem_bytes_lib;
                break;
            }
            case OCI_UST_HANDLES:
            {
                value = env->nb_hndlp;
                break;
            }
            case OCI_UST_DESCRIPTORS:
            {
                value = env->nb_descp;
                break;
            }
            case OCI_UST_OBJECTS:
            {
                value = env->nb_objinst;
                break;
            }
            default:
            {
                value = 0;
                break;
            }
        }
    }

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentEnableWarnings
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibEnvironmentEnableWarnings
(
    OCI_Environment *env,
    OCI_Boolean      value
)
{
    SET_LIB_PROP
    (
        env,
        /* member */ env->warnings_on,
        /* value  */ value
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentSetErrorHandler
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibEnvironmentSetErrorHandler
(
    OCI_Environment *env,
    POCI_ERROR       handler
)
{
    SET_LIB_PROP
    (
        env,
        /* member */ env->error_handler,
        /* value  */ handler
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentSetHAHandler
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibEnvironmentSetHAHandler
(
    OCI_Environment *env,
    POCI_HA_HANDLER  handler
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    void *callback = NULL;

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);
    CHECK_HIGH_AVAILABILITY_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* On MSVC, casting a function pointer to a data pointer generates a warning.
       As there is no other way to do regarding the OCI API, let's disable this
       warning just the time to set the callback attribute to the environment handle */

    WARNING_DISABLE_CAST_FUNC_TYPE
        WARNING_DISABLE_PEDANTIC

    if (NULL != handler)
    {
        callback = (void*)OcilibCallbackHAEvent;
    }

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_ENV, OCI_ATTR_EVTCBK,
        env->handle, callback, 0,
        env->err
    );

    env->ha_handler = handler;

    WARNING_RESTORE_CAST_FUNC_TYPE
    WARNING_RESTORE_PEDANTIC

#else

    OCI_NOT_USED
    (
        callback
    );

#endif

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentSetSubscriptionPort
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibEnvironmentSetSubscriptionPort
(
    OCI_Environment *env,
    OCI_UInt32       port
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);

    if (port > 0)
    {
        ub4 oci_port = (ub4) port;

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_ENV, OCI_ATTR_SUBSCR_PORTNO,
            env->handle, &oci_port, sizeof(oci_port),
            env->err
        );
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentGetSubscriptionPort
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibEnvironmentGetSubscriptionPort
(
    OCI_Environment *env
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);

    ub4 port = 0;
    ub4 size = sizeof(port);

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_ENV, OCI_ATTR_SUBSCR_PORTNO,
        env->handle, &port, &size,
        env->err
    );

    SET_RETVAL((OCI_UInt32)port);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
* OcilibGetLocaleString
* --------------------------------------------------------------------------------------------- */

const otext* OcilibGetLocaleString
(
    OCI_Environment *env,
    OCI_LocaleCode   code
)
{
    ENTER_FUNC
    (
        /* returns */ const otext *, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    dbtext   *dbstr  = NULL;
    OCI_Int32 dbsize = OCI_SIZE_BUFFER_LSI;

    otext      *lsi_str    = NULL;
    OCI_Boolean mtx_locked = FALSE;

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);

#if OCI_VERSION_COMPILE >= OCI_9_2

    OCI_Size i = 0;
    for (; i < OCI_LSI_COUNT; i++)
    {
        if (code == env->locale_strings[i].code)
        {
            break;
        }
    }

    if (i >= OCI_LSI_COUNT)
    {
        THROW(OcilibExceptionArgInvalidValue, OTEXT("Locale Code"), code);
    }

    if (env->locale_strings[i].str == NULL)
    {
        /* the lazy fill is guarded per environment so that two threads
           cannot allocate the same slot twice */

        if (LIB_THREADED(env))
        {
            mtx_lock(&env->mutex);
            mtx_locked = TRUE;
        }

        /* re-test now that the slot is protected */

        if (env->locale_strings[i].str == NULL)
        {
            lsi_str = (otext*)OcilibMemoryAlloc(env, OCI_IPC_STRING, (OCI_Size)OCI_SIZE_BUFFER_LSI + 1, (OCI_Size)1, TRUE);

            CHECK_NULL(lsi_str);

            dbstr = OcilibStringGetDBString(env, lsi_str, &dbsize);

            CHECK_NULL(dbstr);

            CHECK_OCI
            (
                env->err,
                OCINlsGetInfo,
                env->handle,
                env->err,
                (OraText*)dbstr,
                (OCI_Size)dbsize,
                (ub2)code
            );

            OcilibStringCopyDBStringToNativeString(env, dbstr, lsi_str, OCI_SIZE_BUFFER_LSI);

            /* publish the slot only on success so a failed fill can be retried */

            env->locale_strings[i].str = lsi_str;
            lsi_str                    = NULL;
        }

        if (mtx_locked)
        {
            mtx_unlock(&env->mutex);
            mtx_locked = FALSE;
        }
    }

    SET_RETVAL(env->locale_strings[i].str);

#else

    OCI_NOT_USED(type);

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (mtx_locked)
        {
            mtx_unlock(&env->mutex);
        }

        FREE(lsi_str);

        OcilibStringReleaseDBString(env, dbstr);
    );
}

/* --------------------------------------------------------------------------------------------- *
* OcilibEnvironmentSetFormat
* --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibEnvironmentSetFormat
(
    OCI_Environment *env,
    OCI_FormatType   type,
    const otext     *format
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    otext **value = NULL;

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);
    CHECK_ENUM_VALUE(type, FormatTypeValues, OTEXT("Format Type"));

    value = &env->formats[type - 1];

    FREE(*value);

    *value = OcilibStringDuplicate(env, format ? format : FormatDefaultValues[type-1]);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
* OcilibEnvironmentGetFormat
* --------------------------------------------------------------------------------------------- */

const otext * OcilibEnvironmentGetFormat
(
    OCI_Environment *env,
    OCI_FormatType   type
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    otext **value = NULL;

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);
    CHECK_ENUM_VALUE(type, FormatTypeValues, OTEXT("Format Type"));

    value = &env->formats[type-1];

    if (NULL == *value)
    {
        CHECK(OcilibEnvironmentSetFormat(env, type, NULL));
    }

    SET_RETVAL(*value);

    EXIT_FUNC();
}

WARNING_RESTORE_CAST_FUNC_TYPE
