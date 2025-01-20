/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2025 Vincent ROGIER <vince.rogier@ocilib.net>
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
#include "macros.h"
#include "mutex.h"
#include "pool.h"
#include "stringutils.h"
#include "subscription.h"
#include "threadkey.h"

WARNING_DISABLE_CAST_FUNC_TYPE

OCI_Environment Env;

const char * EnvironmentVarNames[OCI_VARS_COUNT] =
{
    VAR_OCILIB_WORKAROUND_UTF16_COLUMN_NAME
};

const OCI_SQLCmdInfo SQLCmds[OCI_SQLCMD_COUNT] =
{
    {OCI_UNKNOWN                             , OTEXT("UNKNOWN")                        },
    {OCI_SFC_CREATE_TABLE                    , OTEXT("CREATE TABLE")                   },
    {OCI_SFC_INSERT                          , OTEXT("INSERT")                         },
    {OCI_SFC_SELECT                          , OTEXT("SELECT")                         },
    {OCI_SFC_CREATE_CLUSTER                  , OTEXT("CREATE CLUSTER")                 },
    {OCI_SFC_ALTER_CLUSTER                   , OTEXT("ALTER CLUSTER")                  },
    {OCI_SFC_UPDATE                          , OTEXT("UPDATE")                         },
    {OCI_SFC_DELETE                          , OTEXT("DELETE")                         },
    {OCI_SFC_DROP_CLUSTER                    , OTEXT("DROP CLUSTER")                   },
    {OCI_SFC_CREATE_INDEX                    , OTEXT("CREATE INDEX")                   },
    {OCI_SFC_DROP_INDEX                      , OTEXT("DROP INDEX")                     },
    {OCI_SFC_ALTER_INDEX                     , OTEXT("ALTER INDEX")                    },
    {OCI_SFC_DROP_TABLE                      , OTEXT("DROP TABLE")                     },
    {OCI_SFC_CREATE_SEQUENCE                 , OTEXT("CREATE SEQUENCE")                },
    {OCI_SFC_ALTER_SEQUENCE                  , OTEXT("ALTER SEQUENCE")                 },
    {OCI_SFC_ALTER_TABLE                     , OTEXT("ALTER TABLE")                    },
    {OCI_SFC_DROP_SEQUENCE                   , OTEXT("DROP SEQUENCE")                  },
    {OCI_SFC_GRANT_OBJECT                    , OTEXT("GRANT OBJECT")                   },
    {OCI_SFC_REVOKE_OBJECT                   , OTEXT("REVOKE OBJECT")                  },
    {OCI_SFC_CREATE_SYNONYM                  , OTEXT("CREATE SYNONYM")                 },
    {OCI_SFC_DROP_SYNONYM                    , OTEXT("DROP SYNONYM")                   },
    {OCI_SFC_CREATE_VIEW                     , OTEXT("CREATE VIEW")                    },
    {OCI_SFC_DROP_VIEW                       , OTEXT("DROP VIEW")                      },
    {OCI_SFC_VALIDATE_INDEX                  , OTEXT("VALIDATE INDEX")                 },
    {OCI_SFC_CREATE_PROCEDURE                , OTEXT("CREATE PROCEDURE")               },
    {OCI_SFC_ALTER_PROCEDURE                 , OTEXT("ALTER PROCEDURE")                },
    {OCI_SFC_LOCK                            , OTEXT("LOCK")                           },
    {OCI_SFC_NO_OP                           , OTEXT("NO-OP")                          },
    {OCI_SFC_RENAME                          , OTEXT("RENAME")                         },
    {OCI_SFC_COMMENT                         , OTEXT("COMMENT")                        },
    {OCI_SFC_AUDIT_OBJECT                    , OTEXT("AUDIT OBJECT")                   },
    {OCI_SFC_NOAUDIT_OBJECT                  , OTEXT("NOAUDIT OBJECT")                 },
    {OCI_SFC_CREATE_DATABASE_LINK            , OTEXT("CREATE DATABASE LINK")           },
    {OCI_SFC_DROP_DATABASE_LINK              , OTEXT("DROP DATABASE LINK")             },
    {OCI_SFC_CREATE_DATABASE                 , OTEXT("CREATE DATABASE")                },
    {OCI_SFC_ALTER_DATABASE                  , OTEXT("ALTER DATABASE")                 },
    {OCI_SFC_CREATE_ROLLBACK_SEG             , OTEXT("CREATE ROLLBACK SEG")            },
    {OCI_SFC_ALTER_ROLLBACK_SEG              , OTEXT("ALTER ROLLBACK SEG")             },
    {OCI_SFC_DROP_ROLLBACK_SEG               , OTEXT("DROP ROLLBACK SEG")              },
    {OCI_SFC_CREATE_TABLESPACE               , OTEXT("CREATE TABLESPACE")              },
    {OCI_SFC_ALTER_TABLESPACE                , OTEXT("ALTER TABLESPACE")               },
    {OCI_SFC_DROP_TABLESPACE                 , OTEXT("DROP TABLESPACE")                },
    {OCI_SFC_ALTER_SESSION                   , OTEXT("ALTER SESSION")                  },
    {OCI_SFC_ALTER_USER                      , OTEXT("ALTER USER")                     },
    {OCI_SFC_COMMIT                          , OTEXT("COMMIT")                         },
    {OCI_SFC_ROLLBACK                        , OTEXT("ROLLBACK")                       },
    {OCI_SFC_SAVEPOINT                       , OTEXT("SAVEPOINT")                      },
    {OCI_SFC_PL_SQL_EXECUTE                  , OTEXT("PL/SQL EXECUTE")                 },
    {OCI_SFC_SET_TRANSACTION                 , OTEXT("SET TRANSACTION")                },
    {OCI_SFC_ALTER_SYSTEM                    , OTEXT("ALTER SYSTEM")                   },
    {OCI_SFC_EXPLAIN                         , OTEXT("EXPLAIN")                        },
    {OCI_SFC_CREATE_USER                     , OTEXT("CREATE USER")                    },
    {OCI_SFC_CREATE_ROLE                     , OTEXT("CREATE ROLE")                    },
    {OCI_SFC_DROP_USER                       , OTEXT("DROP USER")                      },
    {OCI_SFC_DROP_ROLE                       , OTEXT("DROP ROLE")                      },
    {OCI_SFC_SET_ROLE                        , OTEXT("SET ROLE")                       },
    {OCI_SFC_CREATE_SCHEMA                   , OTEXT("CREATE SCHEMA")                  },
    {OCI_SFC_CREATE_CONTROL_FILE             , OTEXT("CREATE CONTROL FILE")            },
    {OCI_SFC_ALTER_TRACING                   , OTEXT("ALTER TRACING")                  },
    {OCI_SFC_CREATE_TRIGGER                  , OTEXT("CREATE TRIGGER")                 },
    {OCI_SFC_ALTER_TRIGGER                   , OTEXT("ALTER TRIGGER")                  },
    {OCI_SFC_DROP_TRIGGER                    , OTEXT("DROP TRIGGER")                   },
    {OCI_SFC_ANALYZE_TABLE                   , OTEXT("ANALYZE TABLE")                  },
    {OCI_SFC_ANALYZE_INDEX                   , OTEXT("ANALYZE INDEX")                  },
    {OCI_SFC_ANALYZE_CLUSTER                 , OTEXT("ANALYZE CLUSTER")                },
    {OCI_SFC_CREATE_PROFILE                  , OTEXT("CREATE PROFILE")                 },
    {OCI_SFC_DROP_PROFILE                    , OTEXT("DROP PROFILE")                   },
    {OCI_SFC_ALTER_PROFILE                   , OTEXT("ALTER PROFILE")                  },
    {OCI_SFC_DROP_PROCEDURE                  , OTEXT("DROP PROCEDURE")                 },
    {OCI_SFC_ALTER_RESOURCE_COST             , OTEXT("ALTER RESOURCE COST")            },
    {OCI_SFC_CREATE_MATERIALIZED_VIEW_LOG    , OTEXT("CREATE MATERIALIZED VIEW LOG")   },
    {OCI_SFC_ALTER_MATERIALIZED_VIEW_LOG     , OTEXT("ALTER MATERIALIZED VIEW LOG")    },
    {OCI_SFC_DROP_MATERIALIZED_VIEW_LOG      , OTEXT("DROP MATERIALIZED VIEW LOG")     },
    {OCI_SFC_CREATE_MATERIALIZED_VIEW        , OTEXT("CREATE MATERIALIZED VIEW")       },
    {OCI_SFC_ALTER_MATERIALIZED_VIEW         , OTEXT("ALTER MATERIALIZED VIEW")        },
    {OCI_SFC_DROP_MATERIALIZED_VIEW          , OTEXT("DROP MATERIALIZED VIEW")         },
    {OCI_SFC_CREATE_TYPE                     , OTEXT("CREATE TYPE")                    },
    {OCI_SFC_DROP_TYPE                       , OTEXT("DROP TYPE")                      },
    {OCI_SFC_ALTER_ROLE                      , OTEXT("ALTER ROLE")                     },
    {OCI_SFC_ALTER_TYPE                      , OTEXT("ALTER TYPE")                     },
    {OCI_SFC_CREATE_TYPE_BODY                , OTEXT("CREATE TYPE BODY")               },
    {OCI_SFC_ALTER_TYPE_BODY                 , OTEXT("ALTER TYPE BODY")                },
    {OCI_SFC_DROP_TYPE_BODY                  , OTEXT("DROP TYPE BODY")                 },
    {OCI_SFC_DROP_LIBRARY                    , OTEXT("DROP LIBRARY")                   },
    {OCI_SFC_TRUNCATE_TABLE                  , OTEXT("TRUNCATE TABLE")                 },
    {OCI_SFC_TRUNCATE_CLUSTER                , OTEXT("TRUNCATE CLUSTER")               },
    {OCI_SFC_ALTER_VIEW                      , OTEXT("ALTER VIEW")                     },
    {OCI_SFC_SET_CONSTRAINTS                 , OTEXT("SET CONSTRAINTS")                },
    {OCI_SFC_CREATE_FUNCTION                 , OTEXT("CREATE FUNCTION")                },
    {OCI_SFC_ALTER_FUNCTION                  , OTEXT("ALTER FUNCTION")                 },
    {OCI_SFC_DROP_FUNCTION                   , OTEXT("DROP FUNCTION")                  },
    {OCI_SFC_CREATE_PACKAGE                  , OTEXT("CREATE PACKAGE")                 },
    {OCI_SFC_ALTER_PACKAGE                   , OTEXT("ALTER PACKAGE")                  },
    {OCI_SFC_DROP_PACKAGE                    , OTEXT("DROP PACKAGE")                   },
    {OCI_SFC_CREATE_PACKAGE_BODY             , OTEXT("CREATE PACKAGE BODY")            },
    {OCI_SFC_ALTER_PACKAGE_BODY              , OTEXT("ALTER PACKAGE BODY")             },
    {OCI_SFC_DROP_PACKAGE_BODY               , OTEXT("DROP PACKAGE BODY")              },
    {OCI_SFC_LOGON                           , OTEXT("LOGON")                          },
    {OCI_SFC_LOGOFF                          , OTEXT("LOGOFF")                         },
    {OCI_SFC_LOGOFF_BY_CLEANUP               , OTEXT("LOGOFF BY CLEANUP")              },
    {OCI_SFC_SESSION_REC                     , OTEXT("SESSION REC")                    },
    {OCI_SFC_SYSTEM_AUDIT                    , OTEXT("SYSTEM AUDIT")                   },
    {OCI_SFC_SYSTEM_NOAUDIT                  , OTEXT("SYSTEM NOAUDIT")                 },
    {OCI_SFC_AUDIT_DEFAULT                   , OTEXT("AUDIT DEFAULT")                  },
    {OCI_SFC_NOAUDIT_DEFAULT                 , OTEXT("NOAUDIT DEFAULT")                },
    {OCI_SFC_SYSTEM_GRANT                    , OTEXT("SYSTEM GRANT")                   },
    {OCI_SFC_SYSTEM_REVOKE                   , OTEXT("SYSTEM REVOKE")                  },
    {OCI_SFC_CREATE_PUBLIC_SYNONYM           , OTEXT("CREATE PUBLIC SYNONYM")          },
    {OCI_SFC_DROP_PUBLIC_SYNONYM             , OTEXT("DROP PUBLIC SYNONYM")            },
    {OCI_SFC_CREATE_PUBLIC_DATABASE_LINK     , OTEXT("CREATE PUBLIC DATABASE LINK")    },
    {OCI_SFC_DROP_PUBLIC_DATABASE_LINK       , OTEXT("DROP PUBLIC DATABASE LINK")      },
    {OCI_SFC_GRANT_ROLE                      , OTEXT("GRANT ROLE")                     },
    {OCI_SFC_REVOKE_ROLE                     , OTEXT("REVOKE ROLE")                    },
    {OCI_SFC_EXECUTE_PROCEDURE               , OTEXT("EXECUTE PROCEDURE")              },
    {OCI_SFC_USER_COMMENT                    , OTEXT("USER COMMENT")                   },
    {OCI_SFC_ENABLE_TRIGGER                  , OTEXT("ENABLE TRIGGER")                 },
    {OCI_SFC_DISABLE_TRIGGER                 , OTEXT("DISABLE TRIGGER")                },
    {OCI_SFC_ENABLE_ALL_TRIGGERS             , OTEXT("ENABLE ALL TRIGGERS")            },
    {OCI_SFC_DISABLE_ALL_TRIGGERS            , OTEXT("DISABLE ALL TRIGGERS")           },
    {OCI_SFC_NETWORK_ERROR                   , OTEXT("NETWORK ERROR")                  },
    {OCI_SFC_EXECUTE_TYPE                    , OTEXT("EXECUTE TYPE")                   },
    {OCI_SFC_READ_DIRECTORY                  , OTEXT("READ DIRECTORY")                 },
    {OCI_SFC_WRITE_DIRECTORY                 , OTEXT("WRITE DIRECTORY")                },
    {OCI_SFC_FLASHBACK                       , OTEXT("FLASHBACK")                      },
    {OCI_SFC_BECOME_USER                     , OTEXT("BECOME USER")                    },
    {OCI_SFC_ALTER_MINING_MODEL              , OTEXT("ALTER MINING MODEL")             },
    {OCI_SFC_SELECT_MINING_MODEL             , OTEXT("SELECT MINING MODEL")            },
    {OCI_SFC_CREATE_MINING_MODEL             , OTEXT("CREATE MINING MODEL")            },
    {OCI_SFC_ALTER_PUBLIC_SYNONYM            , OTEXT("ALTER PUBLIC SYNONYM")           },
    {OCI_SFC_EXECUTE_DIRECTORY               , OTEXT("EXECUTE DIRECTORY")              },
    {OCI_SFC_SQL_LOADER_DIRECT_PATH_LOAD     , OTEXT("SQL*LOADER DIRECT PATH LOAD")    },
    {OCI_SFC_DATAPUMP_DIRECT_PATH_UNLOAD     , OTEXT("DATAPUMP DIRECT PATH UNLOAD")    },
    {OCI_SFC_DATABASE_STARTUP                , OTEXT("DATABASE STARTUP")               },
    {OCI_SFC_DATABASE_SHUTDOWN               , OTEXT("DATABASE SHUTDOWN")              },
    {OCI_SFC_CREATE_SQL_TXLN_PROFILE         , OTEXT("CREATE SQL TXLN PROFILE")        },
    {OCI_SFC_ALTER_SQL_TXLN_PROFILE          , OTEXT("ALTER SQL TXLN PROFILE")         },
    {OCI_SFC_USE_SQL_TXLN_PROFILE            , OTEXT("USE SQL TXLN PROFILE")           },
    {OCI_SFC_DROP_SQL_TXLN_PROFILE           , OTEXT("DROP SQL TXLN PROFILE")          },
    {OCI_SFC_CREATE_MEASURE_FOLDER           , OTEXT("CREATE MEASURE FOLDER")          },
    {OCI_SFC_ALTER_MEASURE_FOLDER            , OTEXT("ALTER MEASURE FOLDER")           },
    {OCI_SFC_DROP_MEASURE_FOLDER             , OTEXT("DROP MEASURE FOLDER")            },
    {OCI_SFC_CREATE_CUBE_BUILD_PROCESS       , OTEXT("CREATE CUBE BUILD PROCESS")      },
    {OCI_SFC_ALTER_CUBE_BUILD_PROCESS        , OTEXT("ALTER CUBE BUILD PROCESS")       },
    {OCI_SFC_DROP_CUBE_BUILD_PROCESS         , OTEXT("DROP CUBE BUILD PROCESS")        },
    {OCI_SFC_CREATE_CUBE                     , OTEXT("CREATE CUBE")                    },
    {OCI_SFC_ALTER_CUBE                      , OTEXT("ALTER CUBE")                     },
    {OCI_SFC_DROP_CUBE                       , OTEXT("DROP CUBE")                      },
    {OCI_SFC_CREATE_CUBE_DIMENSION           , OTEXT("CREATE CUBE DIMENSION")          },
    {OCI_SFC_ALTER_CUBE_DIMENSION            , OTEXT("ALTER CUBE DIMENSION")           },
    {OCI_SFC_DROP_CUBE_DIMENSION             , OTEXT("DROP CUBE DIMENSION")            },
    {OCI_SFC_CREATE_DIRECTORY                , OTEXT("CREATE DIRECTORY")               },
    {OCI_SFC_DROP_DIRECTORY                  , OTEXT("DROP DIRECTORY")                 },
    {OCI_SFC_CREATE_LIBRARY                  , OTEXT("CREATE LIBRARY")                 },
    {OCI_SFC_CREATE_JAVA                     , OTEXT("CREATE JAVA")                    },
    {OCI_SFC_ALTER_JAVA                      , OTEXT("ALTER JAVA")                     },
    {OCI_SFC_DROP_JAVA                       , OTEXT("DROP JAVA")                      },
    {OCI_SFC_CREATE_OPERATOR                 , OTEXT("CREATE OPERATOR")                },
    {OCI_SFC_CREATE_INDEXTYPE                , OTEXT("CREATE INDEXTYPE")               },
    {OCI_SFC_DROP_INDEXTYPE                  , OTEXT("DROP INDEXTYPE")                 },
    {OCI_SFC_ALTER_INDEXTYPE                 , OTEXT("ALTER INDEXTYPE")                },
    {OCI_SFC_DROP_OPERATOR                   , OTEXT("DROP OPERATOR")                  },
    {OCI_SFC_ASSOCIATE_STATISTICS            , OTEXT("ASSOCIATE STATISTICS")           },
    {OCI_SFC_DISASSOCIATE_STATISTICS         , OTEXT("DISASSOCIATE STATISTICS")        },
    {OCI_SFC_CALL_METHOD                     , OTEXT("CALL METHOD")                    },
    {OCI_SFC_CREATE_SUMMARY                  , OTEXT("CREATE SUMMARY")                 },
    {OCI_SFC_ALTER_SUMMARY                   , OTEXT("ALTER SUMMARY")                  },
    {OCI_SFC_DROP_SUMMARY                    , OTEXT("DROP SUMMARY")                   },
    {OCI_SFC_CREATE_DIMENSION                , OTEXT("CREATE DIMENSION")               },
    {OCI_SFC_ALTER_DIMENSION                 , OTEXT("ALTER DIMENSION")                },
    {OCI_SFC_DROP_DIMENSION                  , OTEXT("DROP DIMENSION")                 },
    {OCI_SFC_CREATE_CONTEXT                  , OTEXT("CREATE CONTEXT")                 },
    {OCI_SFC_DROP_CONTEXT                    , OTEXT("DROP CONTEXT")                   },
    {OCI_SFC_ALTER_OUTLINE                   , OTEXT("ALTER OUTLINE")                  },
    {OCI_SFC_CREATE_OUTLINE                  , OTEXT("CREATE OUTLINE")                 },
    {OCI_SFC_DROP_OUTLINE                    , OTEXT("DROP OUTLINE")                   },
    {OCI_SFC_UPDATE_INDEXES                  , OTEXT("UPDATE INDEXES")                 },
    {OCI_SFC_ALTER_OPERATOR                  , OTEXT("ALTER OPERATOR")                 },
    {OCI_SFC_CREATE_SPFILE                   , OTEXT("CREATE SPFILE")                  },
    {OCI_SFC_CREATE_PFILE                    , OTEXT("CREATE PFILE")                   },
    {OCI_SFC_MERGE                           , OTEXT("MERGE")                          },
    {OCI_SFC_PASSWORD_CHANGE                 , OTEXT("PASSWORD CHANGE")                },
    {OCI_SFC_ALTER_SYNONYM                   , OTEXT("ALTER SYNONYM")                  },
    {OCI_SFC_ALTER_DISKGROUP                 , OTEXT("ALTER DISKGROUP")                },
    {OCI_SFC_CREATE_DISKGROUP                , OTEXT("CREATE DISKGROUP")               },
    {OCI_SFC_DROP_DISKGROUP                  , OTEXT("DROP DISKGROUP")                 },
    {OCI_SFC_PURGE_RECYCLEBIN                , OTEXT("PURGE RECYCLEBIN")               },
    {OCI_SFC_PURGE_DBA_RECYCLEBIN            , OTEXT("PURGE DBA_RECYCLEBIN")           },
    {OCI_SFC_PURGE_TABLESPACE                , OTEXT("PURGE TABLESPACE")               },
    {OCI_SFC_PURGE_TABLE                     , OTEXT("PURGE TABLE")                    },
    {OCI_SFC_PURGE_INDEX                     , OTEXT("PURGE INDEX")                    },
    {OCI_SFC_UNDROP_OBJECT                   , OTEXT("UNDROP OBJECT")                  },
    {OCI_SFC_DROP_DATABASE                   , OTEXT("DROP DATABASE")                  },
    {OCI_SFC_FLASHBACK_DATABASE              , OTEXT("FLASHBACK DATABASE")             },
    {OCI_SFC_FLASHBACK_TABLE                 , OTEXT("FLASHBACK TABLE")                },
    {OCI_SFC_CREATE_RESTORE_POINT            , OTEXT("CREATE RESTORE POINT")           },
    {OCI_SFC_DROP_RESTORE_POINT              , OTEXT("DROP RESTORE POINT")             },
    {OCI_SFC_PROXY_AUTHENTICATION_ONLY       , OTEXT("PROXY AUTHENTICATION ONLY")      },
    {OCI_SFC_DECLARE_REWRITE_EQUIVALENCE     , OTEXT("DECLARE REWRITE EQUIVALENCE")    },
    {OCI_SFC_ALTER_REWRITE_EQUIVALENCE       , OTEXT("ALTER REWRITE EQUIVALENCE")      },
    {OCI_SFC_DROP_REWRITE_EQUIVALENCE        , OTEXT("DROP REWRITE EQUIVALENCE")       },
    {OCI_SFC_CREATE_EDITION                  , OTEXT("CREATE EDITION")                 },
    {OCI_SFC_ALTER_EDITION                   , OTEXT("ALTER EDITION")                  },
    {OCI_SFC_DROP_EDITION                    , OTEXT("DROP EDITION")                   },
    {OCI_SFC_DROP_ASSEMBLY                   , OTEXT("DROP ASSEMBLY")                  },
    {OCI_SFC_CREATE_ASSEMBLY                 , OTEXT("CREATE ASSEMBLY")                },
    {OCI_SFC_ALTER_ASSEMBLY                  , OTEXT("ALTER ASSEMBLY")                 },
    {OCI_SFC_CREATE_FLASHBACK_ARCHIVE        , OTEXT("CREATE FLASHBACK ARCHIVE")       },
    {OCI_SFC_ALTER_FLASHBACK_ARCHIVE         , OTEXT("ALTER FLASHBACK ARCHIVE")        },
    {OCI_SFC_DROP_FLASHBACK_ARCHIVE          , OTEXT("DROP FLASHBACK ARCHIVE")         },
    {OCI_SFC_DEBUG_CONNECT                   , OTEXT("DEBUG CONNECT")                  },
    {OCI_SFC_DEBUG_PROCEDURE                 , OTEXT("DEBUG PROCEDURE")                },
    {OCI_SFC_ALTER_DATABASE_LINK             , OTEXT("ALTER DATABASE LINK")            },
    {OCI_SFC_CREATE_PLUGGABLE_DATABASE       , OTEXT("CREATE PLUGGABLE DATABASE")      },
    {OCI_SFC_ALTER_PLUGGABLE_DATABASE        , OTEXT("ALTER PLUGGABLE DATABASE")       },
    {OCI_SFC_DROP_PLUGGABLE_DATABASE         , OTEXT("DROP PLUGGABLE DATABASE")        },
    {OCI_SFC_CREATE_AUDIT_POLICY             , OTEXT("CREATE AUDIT POLICY")            },
    {OCI_SFC_ALTER_AUDIT_POLICY              , OTEXT("ALTER AUDIT POLICY")             },
    {OCI_SFC_DROP_AUDIT_POLICY               , OTEXT("DROP AUDIT POLICY")              },
    {OCI_SFC_CODE_BASED_GRANT                , OTEXT("CODE-BASED GRANT")               },
    {OCI_SFC_CODE_BASED_REVOKE               , OTEXT("CODE-BASED REVOKE")              },
    {OCI_SFC_CREATE_LOCKDOWN_PROFILE         , OTEXT("CREATE LOCKDOWN PROFILE")        },
    {OCI_SFC_DROP_LOCKDOWN_PROFILE           , OTEXT("DROP LOCKDOWN PROFILE")          },
    {OCI_SFC_ALTER_LOCKDOWN_PROFILE          , OTEXT("ALTER LOCKDOWN PROFILE")         },
    {OCI_SFC_TRANSLATE_SQL                   , OTEXT("TRANSLATE SQL")                  },
    {OCI_SFC_ADMINISTER_KEY_MANAGEMENT       , OTEXT("ADMINISTER KEY MANAGEMENT")      },
    {OCI_SFC_CREATE_MATERIALIZED_ZONEMAP     , OTEXT("CREATE MATERIALIZED ZONEMAP")    },
    {OCI_SFC_ALTER_MATERIALIZED_ZONEMAP      , OTEXT("ALTER MATERIALIZED ZONEMAP")     },
    {OCI_SFC_DROP_MATERIALIZED_ZONEMAP       , OTEXT("DROP MATERIALIZED ZONEMAP")      },
    {OCI_SFC_DROP_MINING_MODEL               , OTEXT("DROP MINING MODEL")              },
    {OCI_SFC_CREATE_ATTRIBUTE_DIMENSION      , OTEXT("CREATE ATTRIBUTE DIMENSION")     },
    {OCI_SFC_ALTER_ATTRIBUTE_DIMENSION       , OTEXT("ALTER ATTRIBUTE DIMENSION")      },
    {OCI_SFC_DROP_ATTRIBUTE_DIMENSION        , OTEXT("DROP ATTRIBUTE DIMENSION")       },
    {OCI_SFC_CREATE_HIERARCHY                , OTEXT("CREATE HIERARCHY")               },
    {OCI_SFC_ALTER_HIERARCHY                 , OTEXT("ALTER HIERARCHY")                },
    {OCI_SFC_DROP_HIERARCHY                  , OTEXT("DROP HIERARCHY")                 },
    {OCI_SFC_CREATE_ANALYTIC_VIEW            , OTEXT("CREATE ANALYTIC VIEW")           },
    {OCI_SFC_ALTER_ANALYTIC_VIEW             , OTEXT("ALTER ANALYTIC VIEW")            },
    {OCI_SFC_DROP_ANALYTIC_VIEW              , OTEXT("DROP ANALYTIC VIEW")             },
    {OCI_SFC_CREATE_LANGUAGE                 , OTEXT("CREATE LANGUAGE")                },
    {OCI_SFC_DROP_LANGUAGE                   , OTEXT("DROP LANGUAGE")                  },
    {OCI_SFC_ALTER_PUBLIC_DATABASE_LINK      , OTEXT("ALTER PUBLIC DATABASE LINK")     }
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

#define OCI_LSI_COUNT 70

OCI_LocaleStringInfo LocaleStrings[OCI_LSI_COUNT] =
{
    { OCI_LSI_DAYNAME1,         NULL },
    { OCI_LSI_DAYNAME2,         NULL },
    { OCI_LSI_DAYNAME3,         NULL },
    { OCI_LSI_DAYNAME4,         NULL },
    { OCI_LSI_DAYNAME5,         NULL },
    { OCI_LSI_DAYNAME6,         NULL },
    { OCI_LSI_DAYNAME7,         NULL },
    { OCI_LSI_ABDAYNAME1,       NULL },
    { OCI_LSI_ABDAYNAME2,       NULL },
    { OCI_LSI_ABDAYNAME3,       NULL },
    { OCI_LSI_ABDAYNAME4,       NULL },
    { OCI_LSI_ABDAYNAME5,       NULL },
    { OCI_LSI_ABDAYNAME6,       NULL },
    { OCI_LSI_ABDAYNAME7,       NULL },
    { OCI_LSI_MONTHNAME1,       NULL },
    { OCI_LSI_MONTHNAME2,       NULL },
    { OCI_LSI_MONTHNAME3,       NULL },
    { OCI_LSI_MONTHNAME4,       NULL },
    { OCI_LSI_MONTHNAME5,       NULL },
    { OCI_LSI_MONTHNAME6,       NULL },
    { OCI_LSI_MONTHNAME7,       NULL },
    { OCI_LSI_MONTHNAME8,       NULL },
    { OCI_LSI_MONTHNAME9,       NULL },
    { OCI_LSI_MONTHNAME10,      NULL },
    { OCI_LSI_MONTHNAME11,      NULL },
    { OCI_LSI_MONTHNAME12,      NULL },
    { OCI_LSI_ABMONTHNAME1,     NULL },
    { OCI_LSI_ABMONTHNAME2,     NULL },
    { OCI_LSI_ABMONTHNAME3,     NULL },
    { OCI_LSI_ABMONTHNAME4,     NULL },
    { OCI_LSI_ABMONTHNAME5,     NULL },
    { OCI_LSI_ABMONTHNAME6,     NULL },
    { OCI_LSI_ABMONTHNAME7,     NULL },
    { OCI_LSI_ABMONTHNAME8,     NULL },
    { OCI_LSI_ABMONTHNAME9,     NULL },
    { OCI_LSI_ABMONTHNAME10,    NULL },
    { OCI_LSI_ABMONTHNAME11,    NULL },
    { OCI_LSI_ABMONTHNAME12,    NULL },
    { OCI_LSI_YES,              NULL },
    { OCI_LSI_NO,               NULL },
    { OCI_LSI_AM,               NULL },
    { OCI_LSI_PM,               NULL },
    { OCI_LSI_AD,               NULL },
    { OCI_LSI_BC,               NULL },
    { OCI_LSI_DECIMAL,          NULL },
    { OCI_LSI_GROUP,            NULL },
    { OCI_LSI_DEBIT,            NULL },
    { OCI_LSI_CREDIT,           NULL },
    { OCI_LSI_DATEFORMAT,       NULL },
    { OCI_LSI_INT_CURRENCY,     NULL },
    { OCI_LSI_LOC_CURRENCY,     NULL },
    { OCI_LSI_LANGUAGE,         NULL },
    { OCI_LSI_ABLANGUAGE,       NULL },
    { OCI_LSI_TERRITORY,        NULL },
    { OCI_LSI_CHARACTER_SET,    NULL },
    { OCI_LSI_LINGUISTIC_NAME,  NULL },
    { OCI_LSI_CALENDAR,         NULL },
    { OCI_LSI_DUAL_CURRENCY,    NULL },
    { OCI_LSI_WRITINGDIR,       NULL },
    { OCI_LSI_ABTERRITORY,      NULL },
    { OCI_LSI_DDATEFORMAT,      NULL },
    { OCI_LSI_DTIMEFORMAT,      NULL },
    { OCI_LSI_SFDATEFORMAT,     NULL },
    { OCI_LSI_SFTIMEFORMAT,     NULL },
    { OCI_LSI_NUMGROUPING,      NULL },
    { OCI_LSI_LISTSEP,          NULL },
    { OCI_LSI_MONDECIMAL,       NULL },
    { OCI_LSI_MONGROUP,         NULL },
    { OCI_LSI_MONGROUPING,      NULL },
    { OCI_LSI_INT_CURRENCYSEP,  NULL }
};

#ifdef OCI_IMPORT_RUNTIME

/* OCI function pointers */

OCIINITIALIZE                OCIInitialize                = NULL;
OCIENVINIT                   OCIEnvInit                   = NULL;
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
OCIDEFINEDYNAMIC             OCIDefineDynamic             = NULL;
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
OCITYPEBYNAME                OCITypeByName                = NULL;
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
OCIENVNLSCREATE              OCIEnvNlsCreate              = NULL;
OCINLSGETINFO                OCINlsGetInfo                = NULL;
OCISTMTPREPARE2              OCIStmtPrepare2              = NULL;
OCISTMTRELEASE               OCIStmtRelease               = NULL;
OCISUBSCRIPTIONREGISTER      OCISubscriptionRegister      = NULL;
OCISUBSCRIPTIONUNREGISTER    OCISubscriptionUnRegister    = NULL;
OCIAQENQ                     OCIAQEnq                     = NULL;
OCIAQDEQ                     OCIAQDeq                     = NULL;
OCIAQLISTEN                  OCIAQListen                  = NULL;
OCILOBGETCONTENTTYPE         OCILobGetContentType         = NULL;
OCISTMTGETNEXTRESULT         OCIStmtGetNextResult         = NULL;
OCISERVERRELEASE2            OCIServerRelease2            = NULL;
OCISODAOPERKEYSSET           OCISodaOperKeysSet           = NULL;
OCIROWIDTOCHAR               OCIRowidToChar               = NULL;
OCISODABULKINSERT            OCISodaBulkInsert            = NULL;    
OCISERVERDATALENGTHGET       OCIServerDataLengthGet       = NULL;
OCIVECTORTOARRAY             OCIVectorToArray             = NULL;
OCIVECTORFROMARRAY           OCIVectorFromArray           = NULL;
OCIVECTORTOTEXT              OCIVectorToText              = NULL;
OCIVECTORFROMTEXT            OCIVectorFromText            = NULL;

XAOSVCCTX xaoSvcCtx = NULL;
XAOENV    xaoEnv    = NULL;

#ifdef ORAXB8_DEFINED

OCILOBCOPY2         OCILobCopy2         = NULL;
OCILOBERASE2        OCILobErase2        = NULL;
OCILOBGETLENGTH2    OCILobGetLength2    = NULL;
OCILOBLOADFROMFILE2 OCILobLoadFromFile2 = NULL;
OCILOBREAD2         OCILobRead2         = NULL;
OCILOBTRIM2         OCILobTrim2         = NULL;
OCILOBWRITE2        OCILobWrite2        = NULL;
OCILOBWRITEAPPEND2  OCILobWriteAppend2  = NULL;

#endif /* ORAXB8_DEFINED */

#endif /* OCI_IMPORT_RUNTIME */

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentFreeErrors
 * --------------------------------------------------------------------------------------------- */

static void OcilibEnvironmentFreeError
(
    OCI_Error *err
)
{
    if (err != Env.lib_err)
    {
        OcilibErrorFree(err);
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentKeyMapFree
 * --------------------------------------------------------------------------------------------- */

static void OcilibEnvironmentKeyMapFree
(
    void
)
{
    OCI_HashEntry *e = NULL;
    OCI_HashValue *v = NULL;

    int nb_err = 0;

    unsigned int count = 0;

    if (NULL == Env.key_map)
    {
        return;
    }

    count = OcilibHashGetSize(Env.key_map);

    for (unsigned int i = 0; i < count; i++)
    {
        e = OcilibHashGetEntry(Env.key_map, i);

        while (e)
        {
            v = e->values;

            while (v)
            {
                if (!OcilibThreadKeyFree((OCI_ThreadKey *) (v->value.p_void)))
                {
                    nb_err++;
                }

                v = v->next;
            }

            e = e->next;
        }
    }

    OcilibHashFree(Env.key_map);

    Env.key_map = NULL;
}

#ifndef OCI_IMPORT_LINKAGE

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentLoadSymbols
 * --------------------------------------------------------------------------------------------- */

static void OcilibEnvironmentLoadSymbols()
{
    WARNING_DISABLE_UNSAFE_CONVERT

    /* Now loading all symbols - no check is performed on each function,
        Basic checks will be done to ensure we're loading an
        Oracle and compatible library ...
    */

    LIB_SYMBOL(Env.lib_handle, "OCIInitialize",         OCIInitialize,
                OCIINITIALIZE);

    LIB_SYMBOL(Env.lib_handle, "OCIEnvInit",            OCIEnvInit,
                OCIENVINIT);

    LIB_SYMBOL(Env.lib_handle, "OCIEnvCreate",           OCIEnvCreate,
                OCIENVCREATE);

    LIB_SYMBOL(Env.lib_handle, "OCIServerAttach",       OCIServerAttach,
                OCISERVERATTACH);
    LIB_SYMBOL(Env.lib_handle, "OCIServerDetach",       OCIServerDetach,
                OCISERVERDETACH);

    LIB_SYMBOL(Env.lib_handle, "OCIHandleAlloc",        OCIHandleAlloc,
                OCIHANDLEALLOC);
    LIB_SYMBOL(Env.lib_handle, "OCIHandleFree",         OCIHandleFree,
                OCIHANDLEFREE);

    LIB_SYMBOL(Env.lib_handle, "OCIDescriptorAlloc",    OCIDescriptorAlloc,
                OCIDESCRIPTORALLOC);
    LIB_SYMBOL(Env.lib_handle, "OCIDescriptorFree",     OCIDescriptorFree,
                OCIDESCRIPTORFREE);

    LIB_SYMBOL(Env.lib_handle, "OCIAttrSet",            OCIAttrSet,
                OCIATTRSET);
    LIB_SYMBOL(Env.lib_handle, "OCIAttrGet",            OCIAttrGet,
                OCIATTRGET);

    LIB_SYMBOL(Env.lib_handle, "OCIParamSet",           OCIParamSet,
                OCIPARAMSET);
    LIB_SYMBOL(Env.lib_handle, "OCIParamGet",           OCIParamGet,
                OCIPARAMGET);

    LIB_SYMBOL(Env.lib_handle, "OCISessionBegin",       OCISessionBegin,
                OCISESSIONBEGIN);
    LIB_SYMBOL(Env.lib_handle, "OCISessionEnd",         OCISessionEnd,
                OCISESSIONEND);

    LIB_SYMBOL(Env.lib_handle, "OCIPasswordChange",     OCIPasswordChange,
                OCIPASSWORDCHANGE);

    LIB_SYMBOL(Env.lib_handle, "OCITransStart",         OCITransStart,
                OCITRANSSTART);
    LIB_SYMBOL(Env.lib_handle, "OCITransDetach",        OCITransDetach,
                OCITRANSDETACH);
    LIB_SYMBOL(Env.lib_handle, "OCITransPrepare",       OCITransPrepare,
                OCITRANSPREPARE);
    LIB_SYMBOL(Env.lib_handle, "OCITransForget",        OCITransForget,
                OCITRANSFORGET);
    LIB_SYMBOL(Env.lib_handle, "OCITransCommit",        OCITransCommit,
                OCITRANSCOMMIT);
    LIB_SYMBOL(Env.lib_handle, "OCITransRollback",      OCITransRollback,
                OCITRANSROLLBACK);

    LIB_SYMBOL(Env.lib_handle, "OCIErrorGet",           OCIErrorGet,
                OCIERRORGET);
    LIB_SYMBOL(Env.lib_handle, "OCIServerVersion",      OCIServerVersion,
                OCISERVERVERSION);
    LIB_SYMBOL(Env.lib_handle, "OCIBreak",              OCIBreak,
                OCIBREAK);

    LIB_SYMBOL(Env.lib_handle, "OCIBindByPos",          OCIBindByPos,
                OCIBINDBYPOS);
    LIB_SYMBOL(Env.lib_handle, "OCIBindByName",         OCIBindByName,
                OCIBINDBYNAME);
    LIB_SYMBOL(Env.lib_handle, "OCIBindDynamic",        OCIBindDynamic,
                OCIBINDDYNAMIC);
    LIB_SYMBOL(Env.lib_handle, "OCIBindObject",         OCIBindObject,
                OCIBINDOBJECT);

    LIB_SYMBOL(Env.lib_handle, "OCIDefineByPos",        OCIDefineByPos,
                OCIDEFINEBYPOS);
    LIB_SYMBOL(Env.lib_handle, "OCIDefineDynamic",      OCIDefineDynamic,
                OCIDEFINEDYNAMIC);
    LIB_SYMBOL(Env.lib_handle, "OCIDefineObject",       OCIDefineObject,
                OCIDEFINEOBJECT);

    LIB_SYMBOL(Env.lib_handle, "OCIStmtPrepare",        OCIStmtPrepare,
                OCISTMTPREPARE);
    LIB_SYMBOL(Env.lib_handle, "OCIStmtExecute",        OCIStmtExecute,
                OCISTMTEXECUTE);
    LIB_SYMBOL(Env.lib_handle, "OCIStmtFetch",          OCIStmtFetch,
                OCISTMTFETCH);
    LIB_SYMBOL(Env.lib_handle, "OCIStmtFetch2",         OCIStmtFetch2,
                OCISTMTFETCH2);

    LIB_SYMBOL(Env.lib_handle, "OCIStmtGetPieceInfo",   OCIStmtGetPieceInfo,
                OCISTMTGETPIECEINFO);
    LIB_SYMBOL(Env.lib_handle, "OCIStmtSetPieceInfo",   OCIStmtSetPieceInfo,
                OCISTMTSETPIECEINFO);

    LIB_SYMBOL(Env.lib_handle, "OCILobCreateTemporary", OCILobCreateTemporary,
                OCILOBCREATETEMPORARY);
    LIB_SYMBOL(Env.lib_handle, "OCILobFreeTemporary",   OCILobFreeTemporary,
                OCILOBFREETEMPORARY);
    LIB_SYMBOL(Env.lib_handle, "OCILobIsTemporary",     OCILobIsTemporary,
                OCILOBISTEMPORARY);
    LIB_SYMBOL(Env.lib_handle, "OCILobRead",            OCILobRead,
                OCILOBREAD);
    LIB_SYMBOL(Env.lib_handle, "OCILobWrite",           OCILobWrite,
                OCILOBWRITE);
    LIB_SYMBOL(Env.lib_handle, "OCILobCopy",            OCILobCopy,
                OCILOBCOPY);
    LIB_SYMBOL(Env.lib_handle, "OCILobTrim",            OCILobTrim,
                OCILOBTRIM);
    LIB_SYMBOL(Env.lib_handle, "OCILobErase",           OCILobErase,
                OCILOBERASE);
    LIB_SYMBOL(Env.lib_handle, "OCILobAppend",          OCILobAppend,
                OCILOBAPPEND);
    LIB_SYMBOL(Env.lib_handle, "OCILobGetLength",       OCILobGetLength,
                OCILOBGETLENGTH);
    LIB_SYMBOL(Env.lib_handle, "OCILobGetChunkSize",    OCILobGetChunkSize,
                OCILOBGETCHUNKSIZE);
    LIB_SYMBOL(Env.lib_handle, "OCILobOpen",            OCILobOpen,
                OCILOBOPEN);
    LIB_SYMBOL(Env.lib_handle, "OCILobClose",           OCILobClose,
                OCILOBCLOSE);

#ifdef ORAXB8_DEFINED

    LIB_SYMBOL(Env.lib_handle, "OCILobCopy2",         OCILobCopy2,
                OCILOBCOPY2);
    LIB_SYMBOL(Env.lib_handle, "OCILobErase2",        OCILobErase2,
                OCILOBERASE2);
    LIB_SYMBOL(Env.lib_handle, "OCILobGetLength2",    OCILobGetLength2,
                OCILOBGETLENGTH2);
    LIB_SYMBOL(Env.lib_handle, "OCILobLoadFromFile2", OCILobLoadFromFile2,
                OCILOBLOADFROMFILE2);
    LIB_SYMBOL(Env.lib_handle, "OCILobRead2",         OCILobRead2,
                OCILOBREAD2);
    LIB_SYMBOL(Env.lib_handle, "OCILobTrim2",         OCILobTrim2,
                OCILOBTRIM2);
    LIB_SYMBOL(Env.lib_handle, "OCILobWrite2",        OCILobWrite2,
                OCILOBWRITE2);
    LIB_SYMBOL(Env.lib_handle, "OCILobWriteAppend2",    OCILobWriteAppend2,
                OCILOBWRITEAPPEND2);

#endif

    LIB_SYMBOL(Env.lib_handle, "OCILobFileOpen",               OCILobFileOpen,
                OCILOBFILEOPEN);
    LIB_SYMBOL(Env.lib_handle, "OCILobFileClose",              OCILobFileClose,
                OCILOBFILECLOSE);
    LIB_SYMBOL(Env.lib_handle, "OCILobFileCloseAll",           OCILobFileCloseAll,
                OCILOBFILECLOSEALL);
    LIB_SYMBOL(Env.lib_handle, "OCILobFileIsOpen",             OCILobFileIsOpen,
                OCILOBFILEISOPEN);
    LIB_SYMBOL(Env.lib_handle, "OCILobFileExists",             OCILobFileExists,
                OCILOBFILEEXISTS);
    LIB_SYMBOL(Env.lib_handle, "OCILobFileGetName",            OCILobFileGetName,
                OCILOBFIELGETNAME);
    LIB_SYMBOL(Env.lib_handle, "OCILobFileSetName",            OCILobFileSetName,
                OCILOBFILESETNAME);
    LIB_SYMBOL(Env.lib_handle, "OCILobLoadFromFile",           OCILobLoadFromFile,
                OCILOBLOADFROMFILE);
    LIB_SYMBOL(Env.lib_handle, "OCILobWriteAppend",            OCILobWriteAppend,
                OCILOBWRITEAPPEND);
    LIB_SYMBOL(Env.lib_handle, "OCILobIsEqual",                OCILobIsEqual,
                OCILOBISEQUAL);
    LIB_SYMBOL(Env.lib_handle, "OCILobAssign",                 OCILobAssign,
                OCILOBASSIGN);
    LIB_SYMBOL(Env.lib_handle, "OCILobLocatorAssign",          OCILobLocatorAssign,
                OCILOBLOCATORASSIGN);
    LIB_SYMBOL(Env.lib_handle, "OCILobFlushBuffer",            OCILobFlushBuffer,
                OCILOBFLUSHBUFFER);
    LIB_SYMBOL(Env.lib_handle, "OCILobGetStorageLimit",        OCILobGetStorageLimit,
                OCILOBGETSTORAGELIMIT);
    LIB_SYMBOL(Env.lib_handle, "OCILobEnableBuffering",        OCILobEnableBuffering,
                OCILOBENABLEBUFFERING);
    LIB_SYMBOL(Env.lib_handle, "OCILobDisableBuffering",       OCILobDisableBuffering,
                OCILOBDISABLEBUFFERING);

    LIB_SYMBOL(Env.lib_handle, "OCIDateAssign",                OCIDateAssign,
                OCIDATEASSIGN);
    LIB_SYMBOL(Env.lib_handle, "OCIDateToText",                OCIDateToText,
                OCIDATETOTEXT);
    LIB_SYMBOL(Env.lib_handle, "OCIDateFromText",              OCIDateFromText,
                OCIDATEFROMTEXT);
    LIB_SYMBOL(Env.lib_handle, "OCIDateCompare",               OCIDateCompare,
                OCIDATECOMPARE);
    LIB_SYMBOL(Env.lib_handle, "OCIDateAddMonths",             OCIDateAddMonths,
                OCIDATEADDMONTHS);
    LIB_SYMBOL(Env.lib_handle, "OCIDateAddDays",               OCIDateAddDays,
                OCIDATEADDDAYS);
    LIB_SYMBOL(Env.lib_handle, "OCIDateLastDay",               OCIDateLastDay,
                OCIDATELASTDAY);
    LIB_SYMBOL(Env.lib_handle, "OCIDateDaysBetween",           OCIDateDaysBetween,
                OCIDATEDAYSBETWEEN);
    LIB_SYMBOL(Env.lib_handle, "OCIDateZoneToZone",            OCIDateZoneToZone,
                OCIDATEZONETOZONE);
    LIB_SYMBOL(Env.lib_handle, "OCIDateNextDay",               OCIDateNextDay,
                OCIDATENEXTDAY);
    LIB_SYMBOL(Env.lib_handle, "OCIDateCheck",                 OCIDateCheck,
                OCIDATECHECK);
    LIB_SYMBOL(Env.lib_handle, "OCIDateSysDate",               OCIDateSysDate,
                OCIDATESYSDATE);
    LIB_SYMBOL(Env.lib_handle, "OCIDescribeAny",               OCIDescribeAny,
                OCIDESCRIBEANY);

    LIB_SYMBOL(Env.lib_handle, "OCIIntervalAssign",            OCIIntervalAssign,
                OCIINTERVALASSIGN);
    LIB_SYMBOL(Env.lib_handle, "OCIIntervalCheck",             OCIIntervalCheck,
                OCIINTERVALCHECK);
    LIB_SYMBOL(Env.lib_handle, "OCIIntervalCompare",           OCIIntervalCompare,
                OCIINTERVALCOMPARE);
    LIB_SYMBOL(Env.lib_handle, "OCIIntervalFromText",          OCIIntervalFromText,
                OCIINTERVALFROMTEXT);
    LIB_SYMBOL(Env.lib_handle, "OCIIntervalToText",            OCIIntervalToText,
                OCIINTERVALTOTEXT);
    LIB_SYMBOL(Env.lib_handle, "OCIIntervalFromTZ",            OCIIntervalFromTZ,
                OCIINTERVALFROMTZ);
    LIB_SYMBOL(Env.lib_handle, "OCIIntervalGetDaySecond",      OCIIntervalGetDaySecond,
                OCIINTERVALGETDAYSECOND);
    LIB_SYMBOL(Env.lib_handle, "OCIIntervalGetYearMonth",      OCIIntervalGetYearMonth,
                OCIINTERVALGETYEARMONTH);
    LIB_SYMBOL(Env.lib_handle, "OCIIntervalSetDaySecond",      OCIIntervalSetDaySecond,
                OCIINTERVALSETDAYSECOND);
    LIB_SYMBOL(Env.lib_handle, "OCIIntervalSetYearMonth",      OCIIntervalSetYearMonth,
                OCIINTERVALSETYEARMONTH);
    LIB_SYMBOL(Env.lib_handle, "OCIIntervalSubtract",          OCIIntervalSubtract,
                OCIINTERVALSUBTRACT);
    LIB_SYMBOL(Env.lib_handle, "OCIIntervalAdd",               OCIIntervalAdd,
                OCIINTERVALADD);

    LIB_SYMBOL(Env.lib_handle, "OCIDateTimeAssign",            OCIDateTimeAssign,
                OCIDATETIMEASSIGN);
    LIB_SYMBOL(Env.lib_handle, "OCIDateTimeCheck",             OCIDateTimeCheck,
                OCIDATETIMECHECK);
    LIB_SYMBOL(Env.lib_handle, "OCIDateTimeCompare",           OCIDateTimeCompare,
                OCIDATETIMECOMPARE);
    LIB_SYMBOL(Env.lib_handle, "OCIDateTimeConstruct",         OCIDateTimeConstruct,
                OCIDATETIMECONSTRUCT);
    LIB_SYMBOL(Env.lib_handle, "OCIDateTimeConvert",           OCIDateTimeConvert,
                OCIDATETIMECONVERT);
    LIB_SYMBOL(Env.lib_handle, "OCIDateTimeFromArray",         OCIDateTimeFromArray,
                OCIDATETIMEFROMARRAY);
    LIB_SYMBOL(Env.lib_handle, "OCIDateTimeToArray",           OCIDateTimeToArray,
                OCIDATETIMETOARRAY);
    LIB_SYMBOL(Env.lib_handle, "OCIDateTimeFromText",          OCIDateTimeFromText,
                OCIDATETIMEFROMTEXT);
    LIB_SYMBOL(Env.lib_handle, "OCIDateTimeToText",            OCIDateTimeToText,
                OCIDATETIMETOTEXT);
    LIB_SYMBOL(Env.lib_handle, "OCIDateTimeGetDate",           OCIDateTimeGetDate,
                OCIDATETIMEGETDATE);
    LIB_SYMBOL(Env.lib_handle, "OCIDateTimeGetTime",           OCIDateTimeGetTime,
                OCIDATETIMEGETTIME);
    LIB_SYMBOL(Env.lib_handle, "OCIDateTimeGetTimeZoneName",   OCIDateTimeGetTimeZoneName,
                OCIDATETIMEGETTIMEZONENAME);
    LIB_SYMBOL(Env.lib_handle, "OCIDateTimeGetTimeZoneOffset", OCIDateTimeGetTimeZoneOffset,
                OCIDATETIMEGETTIMEZONEOFFSET);
    LIB_SYMBOL(Env.lib_handle, "OCIDateTimeIntervalAdd",       OCIDateTimeIntervalAdd,
                OCIDATETIMEINTERVALADD);
    LIB_SYMBOL(Env.lib_handle, "OCIDateTimeIntervalSub",       OCIDateTimeIntervalSub,
                OCIDATETIMEINTERVALSUB);
    LIB_SYMBOL(Env.lib_handle, "OCIDateTimeSubtract",          OCIDateTimeSubtract,
                OCIDATETIMESUBTRACT);
    LIB_SYMBOL(Env.lib_handle, "OCIDateTimeSysTimeStamp",      OCIDateTimeSysTimeStamp,
                OCIDATETIMESYSTIMESTAMP);

    LIB_SYMBOL(Env.lib_handle, "OCITypeByRef",                 OCITypeByRef,
                OCITYPEBYREF);
    LIB_SYMBOL(Env.lib_handle, "OCITypeByName",                OCITypeByName,
                OCITYPEBYNAME);

    LIB_SYMBOL(Env.lib_handle, "OCINumberToInt",               OCINumberToInt,
                OCINUMBERTOINT);
    LIB_SYMBOL(Env.lib_handle, "OCINumberFromInt",             OCINumberFromInt,
                OCINUMBERFROMINT);
    LIB_SYMBOL(Env.lib_handle, "OCINumberToReal",              OCINumberToReal,
                OCINUMBERTOREAL);
    LIB_SYMBOL(Env.lib_handle, "OCINumberFromReal",            OCINumberFromReal,
                OCINUMBERFROMREAL);
    LIB_SYMBOL(Env.lib_handle, "OCINumberToText",              OCINumberToText,
                OCINUMBERTOTEXT);
    LIB_SYMBOL(Env.lib_handle, "OCINumberFromText",            OCINumberFromText,
                OCINUMBERFROMTEXT);
    LIB_SYMBOL(Env.lib_handle, "OCINumberAssign",              OCINumberAssign,
                OCINUMBERASSIGN);
    LIB_SYMBOL(Env.lib_handle, "OCINumberAdd",                 OCINumberAdd,
                OCINUMBERADD);
    LIB_SYMBOL(Env.lib_handle, "OCINumberSub",                 OCINumberSub,
                OCINUMBERSUB);
    LIB_SYMBOL(Env.lib_handle, "OCINumberMul",                 OCINumberMul,
                OCINUMBERMUL);
    LIB_SYMBOL(Env.lib_handle, "OCINumberDiv",                 OCINumberDiv,
                OCINUMBERDIV);
    LIB_SYMBOL(Env.lib_handle, "OCINumberCmp",                 OCINumberCmp,
                OCINUMBERCMP);

    LIB_SYMBOL(Env.lib_handle, "OCIStringPtr",                 OCIStringPtr,
                OCISTRINGPTR);
    LIB_SYMBOL(Env.lib_handle, "OCIStringSize",                OCIStringSize,
                OCISTRINGSIZE);
    LIB_SYMBOL(Env.lib_handle, "OCIStringAssignText",          OCIStringAssignText,
                OCISTRINGASSIGNTEXT);
    LIB_SYMBOL(Env.lib_handle, "OCIStringResize",              OCIStringResize,
                OCISTRINGRESIZE);
    LIB_SYMBOL(Env.lib_handle, "OCIRawPtr",                    OCIRawPtr,
                OCIRAWPTR);
    LIB_SYMBOL(Env.lib_handle, "OCIRawAssignBytes",            OCIRawAssignBytes,
                OCIRAWASSIGNBYTES);
    LIB_SYMBOL(Env.lib_handle, "OCIRawResize",                 OCIRawResize,
                OCIRAWRESIZE);
    LIB_SYMBOL(Env.lib_handle, "OCIRawAllocSize",              OCIRawAllocSize,
                OCIRAWALLOCSIZE);
    LIB_SYMBOL(Env.lib_handle, "OCIRawSize",                   OCIRawSize,
                OCIRAWSIZE);

    LIB_SYMBOL(Env.lib_handle, "OCIObjectNew",                 OCIObjectNew,
                OCIOBJECTNEW);
    LIB_SYMBOL(Env.lib_handle, "OCIObjectFree",                OCIObjectFree,
                OCIOBJECTFREE);
    LIB_SYMBOL(Env.lib_handle, "OCIObjectSetAttr",             OCIObjectSetAttr,
                OCIOBJECTSETATTR);
    LIB_SYMBOL(Env.lib_handle, "OCIObjectGetAttr",             OCIObjectGetAttr,
                OCIOBJECTGETATTR);
    LIB_SYMBOL(Env.lib_handle, "OCIObjectPin",                 OCIObjectPin,
                OCIOBJECTPIN);
    LIB_SYMBOL(Env.lib_handle, "OCIObjectUnpin",               OCIObjectUnpin,
                OCIOBJECTUNPIN);
    LIB_SYMBOL(Env.lib_handle, "OCIObjectCopy",                OCIObjectCopy,
                OCIOBJECTCOPY);
    LIB_SYMBOL(Env.lib_handle, "OCIObjectGetObjectRef",        OCIObjectGetObjectRef,
                OCIOBJECTGETOBJECTREF);
    LIB_SYMBOL(Env.lib_handle, "OCIObjectGetProperty",         OCIObjectGetProperty,
                OCIOBJECTGETPROPERTY);
    LIB_SYMBOL(Env.lib_handle, "OCIObjectGetInd",              OCIObjectGetInd,
                OCIOBJECTGETIND);
    LIB_SYMBOL(Env.lib_handle, "OCIObjectGetTypeRef",          OCIObjectGetTypeRef,
                OCIOBJECTGETOBJECTREF);

    LIB_SYMBOL(Env.lib_handle, "OCIRefAssign",                 OCIRefAssign,
                OCIREFASSIGN);
    LIB_SYMBOL(Env.lib_handle, "OCIRefIsNull",                 OCIRefIsNull,
                OCIREFISNULL);
    LIB_SYMBOL(Env.lib_handle, "OCIRefClear",                  OCIRefClear,
                OCIREFCLEAR);
    LIB_SYMBOL(Env.lib_handle, "OCIRefToHex",                  OCIRefToHex,
                OCIREFTOHEX);
    LIB_SYMBOL(Env.lib_handle, "OCIRefHexSize",                OCIRefHexSize,
                OCIREFHEXSIZE);

    LIB_SYMBOL(Env.lib_handle, "OCIArrayDescriptorAlloc",      OCIArrayDescriptorAlloc,
                OCIARRAYDESCRIPTORALLOC);
    LIB_SYMBOL(Env.lib_handle, "OCIArrayDescriptorFree",       OCIArrayDescriptorFree,
                OCIARRAYDESCRIPTORFREE);

    LIB_SYMBOL(Env.lib_handle, "OCIClientVersion",             OCIClientVersion,
                OCICLIENTVERSION);

    LIB_SYMBOL(Env.lib_handle, "OCIThreadProcessInit",         OCIThreadProcessInit,
                OCITHREADPROCESSINIT);
    LIB_SYMBOL(Env.lib_handle, "OCIThreadInit",                OCIThreadInit,
                OCITHREADINIT);
    LIB_SYMBOL(Env.lib_handle, "OCIThreadTerm",                OCIThreadTerm,
                OCITHREADTERM);

    LIB_SYMBOL(Env.lib_handle, "OCIThreadIdInit",              OCIThreadIdInit,
                OCITHREADIDINIT);
    LIB_SYMBOL(Env.lib_handle, "OCIThreadIdDestroy",           OCIThreadIdDestroy,
                OCITHREADIDDESTROY);
    LIB_SYMBOL(Env.lib_handle, "OCIThreadHndInit",             OCIThreadHndInit,
                OCITHREADHNDINIT);
    LIB_SYMBOL(Env.lib_handle, "OCIThreadHndDestroy",          OCIThreadHndDestroy,
                OCITHREADHNDDESTROY);
    LIB_SYMBOL(Env.lib_handle, "OCIThreadCreate",              OCIThreadCreate,
                OCITHREADCREATE);
    LIB_SYMBOL(Env.lib_handle, "OCIThreadJoin",                OCIThreadJoin,
                OCITHREADJOIN);
    LIB_SYMBOL(Env.lib_handle, "OCIThreadClose",               OCIThreadClose,
                OCITHREADCLOSE);

    LIB_SYMBOL(Env.lib_handle, "OCIThreadMutexInit",           OCIThreadMutexInit,
                OCITHREADMUTEXINIT);
    LIB_SYMBOL(Env.lib_handle, "OCIThreadMutexDestroy",        OCIThreadMutexDestroy,
                OCITHREADMUTEXDESTROY);
    LIB_SYMBOL(Env.lib_handle, "OCIThreadMutexAcquire",        OCIThreadMutexAcquire,
                OCITHREADMUTEXACQUIRE);
    LIB_SYMBOL(Env.lib_handle, "OCIThreadMutexRelease",        OCIThreadMutexRelease,
                OCITHREADMUTEXRELEASE);

    LIB_SYMBOL(Env.lib_handle, "OCIThreadKeyInit",             OCIThreadKeyInit,
                OCITHREADKEYINIT);
    LIB_SYMBOL(Env.lib_handle, "OCIThreadKeyDestroy",          OCIThreadKeyDestroy,
                OCITHREADKEYDESTROY);
    LIB_SYMBOL(Env.lib_handle, "OCIThreadKeySet",              OCIThreadKeySet,
                OCITHREADKEYSET);
    LIB_SYMBOL(Env.lib_handle, "OCIThreadKeyGet",              OCIThreadKeyGet,
                OCITHREADKEYGET);

    LIB_SYMBOL(Env.lib_handle, "OCIConnectionPoolCreate",      OCIConnectionPoolCreate,
                OCICONNECTIONPOOLCREATE);
    LIB_SYMBOL(Env.lib_handle, "OCIConnectionPoolDestroy",     OCIConnectionPoolDestroy,
                OCICONNECTIONPOOLDESTROY);

    LIB_SYMBOL(Env.lib_handle, "OCISessionPoolCreate",         OCISessionPoolCreate,
                OCISESSIONPOOLCREATE);
    LIB_SYMBOL(Env.lib_handle, "OCISessionPoolDestroy",        OCISessionPoolDestroy,
                OCISESSIONPOOLDESTROY);

    LIB_SYMBOL(Env.lib_handle, "OCISessionGet",                OCISessionGet,
                OCISESSIONGET);
    LIB_SYMBOL(Env.lib_handle, "OCISessionRelease",            OCISessionRelease,
                OCISESSIONRELEASE);

    LIB_SYMBOL(Env.lib_handle, "OCICollSize",                  OCICollSize,
                OCICOLLSIZE);
    LIB_SYMBOL(Env.lib_handle, "OCICollMax",                   OCICollMax,
                OCICOLLMAX);
    LIB_SYMBOL(Env.lib_handle, "OCICollGetElem",               OCICollGetElem,
                OCICOLLGETITEM);
    LIB_SYMBOL(Env.lib_handle, "OCICollAssignElem",            OCICollAssignElem,
                OCICOLLASSIGNELEM);
    LIB_SYMBOL(Env.lib_handle, "OCICollAssign",                OCICollAssign,
                OCICOLLASSIGN);
    LIB_SYMBOL(Env.lib_handle, "OCICollAppend",                OCICollAppend,
                OCICOLLAPPEND);
    LIB_SYMBOL(Env.lib_handle, "OCICollTrim",                  OCICollTrim,
                OCICOLLTRIM);
    LIB_SYMBOL(Env.lib_handle, "OCITableDelete",               OCITableDelete,
                OCITABLEDELETE);
    LIB_SYMBOL(Env.lib_handle, "OCITableSize",                 OCITableSize,
                OCITABLESIZE);

    LIB_SYMBOL(Env.lib_handle, "OCIIterCreate",                OCIIterCreate,
                OCIITERCREATE);
    LIB_SYMBOL(Env.lib_handle, "OCIIterDelete",                OCIIterDelete,
                OCIITERDELETE);
    LIB_SYMBOL(Env.lib_handle, "OCIIterInit",                  OCIIterInit,
                OCIITERINIT);
    LIB_SYMBOL(Env.lib_handle, "OCIIterNext",                  OCIIterNext,
                OCIITERNEXT);
    LIB_SYMBOL(Env.lib_handle, "OCIIterPrev",                  OCIIterPrev,
                OCIITERPREV);

    LIB_SYMBOL(Env.lib_handle, "OCIDirPathAbort",              OCIDirPathAbort,
                OCIDIRPATHABORT);
    LIB_SYMBOL(Env.lib_handle, "OCIDirPathDataSave",           OCIDirPathDataSave,
                OCIDIRPATHDATASAVE);
    LIB_SYMBOL(Env.lib_handle, "OCIDirPathFinish",             OCIDirPathFinish,
                OCIDIRPATHFINISH);
    LIB_SYMBOL(Env.lib_handle, "OCIDirPathPrepare",            OCIDirPathPrepare,
                OCIDIRPATHPREPARE);
    LIB_SYMBOL(Env.lib_handle, "OCIDirPathLoadStream",         OCIDirPathLoadStream,
                OCIDIRPATHLOADSTREAM);
    LIB_SYMBOL(Env.lib_handle, "OCIDirPathColArrayEntrySet",   OCIDirPathColArrayEntrySet,
                OCIDIRPATHCOLARRAYENTRYSET);
    LIB_SYMBOL(Env.lib_handle, "OCIDirPathColArrayReset",      OCIDirPathColArrayReset,
                OCIDIRPATHCOLARRAYRESET);
    LIB_SYMBOL(Env.lib_handle, "OCIDirPathColArrayToStream",   OCIDirPathColArrayToStream,
                OCIDIRPATHCOLARRAYTOSTREAM);
    LIB_SYMBOL(Env.lib_handle, "OCIDirPathStreamReset",        OCIDirPathStreamReset,
                OCIDIRPATHSTREAMRESET);
    LIB_SYMBOL(Env.lib_handle, "OCIDirPathFlushRow",           OCIDirPathFlushRow,
                OCIDIRPATHFLUSHROW);

    LIB_SYMBOL(Env.lib_handle, "OCICacheFree",                 OCICacheFree,
                OCICACHEFREE);
    LIB_SYMBOL(Env.lib_handle, "OCIPing",                      OCIPing,
                OCIPING);

    LIB_SYMBOL(Env.lib_handle, "OCIDBStartup",                 OCIDBStartup,
                OCIDBSTARTUP);
    LIB_SYMBOL(Env.lib_handle, "OCIDBShutdown",                OCIDBShutdown,
                OCIDBSHUTDOWN);

    LIB_SYMBOL(Env.lib_handle, "OCIEnvNlsCreate",              OCIEnvNlsCreate,
                OCIENVNLSCREATE);
    LIB_SYMBOL(Env.lib_handle, "OCINlsGetInfo",              OCINlsGetInfo,
                OCINLSGETINFO);
    LIB_SYMBOL(Env.lib_handle, "OCIStmtPrepare2",              OCIStmtPrepare2,
                OCISTMTPREPARE2);
    LIB_SYMBOL(Env.lib_handle, "OCIStmtRelease",               OCIStmtRelease,
                OCISTMTRELEASE);

    LIB_SYMBOL(Env.lib_handle, "OCISubscriptionRegister",      OCISubscriptionRegister,
                OCISUBSCRIPTIONREGISTER);
    LIB_SYMBOL(Env.lib_handle, "OCISubscriptionUnRegister",    OCISubscriptionUnRegister,
                OCISUBSCRIPTIONUNREGISTER);

    LIB_SYMBOL(Env.lib_handle, "OCIAQEnq",                     OCIAQEnq,
                OCIAQENQ);
    LIB_SYMBOL(Env.lib_handle, "OCIAQDeq",                     OCIAQDeq,
                OCIAQDEQ);
    LIB_SYMBOL(Env.lib_handle, "OCIAQListen",                  OCIAQListen,
                OCIAQLISTEN);
        
    LIB_SYMBOL(Env.lib_handle, "OCIRowidToChar",               OCIRowidToChar,
                OCIROWIDTOCHAR);
                   
    LIB_SYMBOL(Env.lib_handle, "xaoSvcCtx",                    xaoSvcCtx,
                XAOSVCCTX);

    LIB_SYMBOL(Env.lib_handle, "xaoEnv",                       xaoEnv,
                XAOENV);

    LIB_SYMBOL(Env.lib_handle, "OCILobGetContentType",         OCILobGetContentType,
                OCILOBGETCONTENTTYPE);

    LIB_SYMBOL(Env.lib_handle, "OCIStmtGetNextResult",         OCIStmtGetNextResult,
                OCISTMTGETNEXTRESULT);

    LIB_SYMBOL(Env.lib_handle, "OCIServerRelease2",            OCIServerRelease2,
                OCISERVERRELEASE2);

    LIB_SYMBOL(Env.lib_handle, "OCISodaOperKeysSet",           OCISodaOperKeysSet,
                OCISODAOPERKEYSSET);

    LIB_SYMBOL(Env.lib_handle, "OCISodaBulkInsert",            OCISodaBulkInsert,
                OCISODABULKINSERT);

    LIB_SYMBOL(Env.lib_handle, "OCIServerDataLengthGet",       OCIServerDataLengthGet,
                OCISERVERDATALENGTHGET);

    LIB_SYMBOL(Env.lib_handle, "OCIVectorToArray",             OCIVectorToArray,
                OCIVECTORTOARRAY);

    LIB_SYMBOL(Env.lib_handle, "OCIVectorFromArray",           OCIVectorFromArray,
                OCIVECTORFROMARRAY);

    LIB_SYMBOL(Env.lib_handle, "OCIVectorToText",              OCIVectorToText,
                OCIVECTORTOTEXT);

    LIB_SYMBOL(Env.lib_handle, "OCIVectorFromText",            OCIVectorFromText,
                OCIVECTORFROMTEXT);

    WARNING_RESTORE_UNSAFE_CONVERT
}

#endif


/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentGetVariable
 *
 * @note
 * Values are allocated with MemoryAlloc() and need to be freed by the caller using MemoryFree()
 *
 * --------------------------------------------------------------------------------------------- */

char * OcilibEnvironmentGetVariable
(
    const char *name
)
{
    ENTER_FUNC
    (
        /* returns */ char*, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_PTR(OCI_IPC_STRING, name)

    char* value = NULL;
    char *str = getenv(name);

    if (NULL != str)
    {
        value = OcilibStringAnsiDuplicate(str);
    }

#if defined(_WINDOWS)

    else
    {
        /* Variables set using SetEnvironmentVariable() on Windows are not seen by MSVC
         * implementation of getenv() !! */

        /* Thus, let's check if they can be retrieved using GetEnvironmentVariable() */
        unsigned int size = GetEnvironmentVariableA(name, NULL, 0);
        if (size > 0)
        {
            value = OcilibMemoryAlloc(OCI_IPC_STRING, size, 1, TRUE);
            size  = GetEnvironmentVariableA(name, value, size);
            if (size == 0)
            {
                OcilibMemoryFree(value);
            }
        }
    }

#endif

    SET_RETVAL(value)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentInitialize
 * --------------------------------------------------------------------------------------------- */

boolean OcilibEnvironmentInitialize
(
    POCI_ERROR   err_handler,
    const otext *lib_path,
    unsigned int mode
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    unsigned int i = 0;

    sword ret = OCI_SUCCESS;

    ub4 oci_mode = OCI_ENV_MODE | OCI_OBJECT;

#ifdef OCI_IMPORT_RUNTIME

    char path[OCI_SIZE_BUFFER+1];

    size_t len = (size_t) 0;

#endif

    /* check if it was already initialized */

    if (Env.loaded)
    {
        SET_SUCCESS()
        JUMP_EXIT()
    }

#if defined(OCI_CHARSET_WIDE) && (OCI_VERSION_COMPILE >= OCI_10_2)

    /* activate support for N' substitution (nchar replacement in SQL statement strings) */

    oci_mode |= OCI_NCHAR_LITERAL_REPLACE_ON;

#endif

    memset(&Env, 0, sizeof(struct OCI_Environment));

    Env.error_handler = err_handler;

    Env.version_compile = OCI_VERSION_COMPILE;
    Env.version_runtime = OCI_VERSION_RUNTIME;

    Env.env_mode           = mode;
    Env.charset            = (sizeof(otext) == sizeof(wchar_t)) ? OCI_CHAR_WIDE : OCI_CHAR_ANSI;
    Env.use_wide_char_conv = (IS_WIDE_CHAR_ENV() && (WCHAR_MAX == WCHAR_4_BYTES));

    /* create environment error */

    Env.lib_err = OcilibErrorCreate();

    for (i = 0; i < OCI_FMT_COUNT; i++)
    {
        Env.formats[i] = OcilibStringDuplicate(FormatDefaultValues[i]);
    }

    /* load any specific environment variable */

    for (i = 0; i < OCI_VARS_COUNT; i++)
    {
        char *value = OcilibEnvironmentGetVariable(EnvironmentVarNames[i]);

        Env.env_vars[i] = value && (OcilibStringAnsiCaseCompare(value, OCI_VARS_TRUE_VALUE) == 0 || atoi(value) == 1);

        FREE(value);
    }

    /* test for UTF8 environment */

    if (IS_ANSI_ENV())
    {
        char *str = OcilibEnvironmentGetVariable("NLS_LANG");

        if (str)
        {
            char nls_lang[OCI_SIZE_OBJ_NAME + 1] = "";

            strncat(nls_lang, str, OCI_SIZE_OBJ_NAME);

            for (char* ptr = nls_lang; *ptr; ptr++)
            {
                *ptr = (char) toupper(*ptr);
            }

            Env.nls_utf8 = (NULL != strstr(nls_lang, "UTF8"));

            FREE(str);
        }
    }

#ifdef OCI_IMPORT_LINKAGE

    OCI_NOT_USED(lib_path)

  #if defined(OCI_BIG_UINT_ENABLED)

    Env.use_lob_ub8 = TRUE;

  #endif

  #if defined(OCI_STMT_SCROLLABLE_READONLY)

    Env.use_scrollable_cursors = TRUE;

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

    Env.lib_handle = LIB_OPEN(path);

    if (Env.lib_handle)
    {
        OcilibEnvironmentLoadSymbols();
            
        /* API Version checking */

        if (OCIClientVersion)
        {
            sword maj = 0;
            sword min = 0;
            sword rev = 0;

            OCIClientVersion(&maj, &min, &rev, NULL, NULL);

            Env.version_runtime = OCI_VER_MAKE(maj, min, rev);
        }
        else if (OCIVectorToArray)
        {
            Env.version_runtime = OCI_23_4;
        }
        else if (OCIServerDataLengthGet)
        {
            Env.version_runtime = OCI_21_3;
        }
         else if (OCISodaBulkInsert)
        {
            Env.version_runtime = OCI_19_3;
        }
        else if (OCISodaOperKeysSet)
        {
            Env.version_runtime = OCI_18_3;
        }
        else if (OCIServerRelease2)
        {
            Env.version_runtime = OCI_18_1;
        }
        else if (OCIStmtGetNextResult)
        {
            Env.version_runtime = OCI_12_1;
        }
        else if (OCILobGetContentType)
        {
            Env.version_runtime = OCI_11_2;
        }
        else if (OCIArrayDescriptorFree)
        {
            Env.version_runtime = OCI_11_1;
        }
        else if (OCIClientVersion)
        {
            Env.version_runtime = OCI_10_2;
        }
        else if (OCILobWrite2)
        {
            Env.version_runtime = OCI_10_1;
        }
        else if (OCIStmtPrepare2)
        {
            Env.version_runtime = OCI_9_2;
        }
        else if (OCIDateTimeGetTimeZoneName)
        {
            Env.version_runtime = OCI_9_0;
        }
        else if (OCIThreadCreate)
        {
            Env.version_runtime = OCI_8_1;
        }
        else if (OCIEnvInit)
        {
            Env.version_runtime = OCI_8_0;

            if (mode & OCI_ENV_THREADED)
            {
                THROW(OcilibExceptionNotAvailable, OCI_FEATURE_MULTITHREADING)
            }
        }
        else
        {
            LIB_CLOSE(Env.lib_handle);
            THROW_NO_ARGS(OcilibExceptionLoadingSymbols)
        }
    }
    else
    {
        THROW_NO_ARGS(OcilibExceptionLoadingSharedLib)
    }

  #if defined(OCI_BIG_UINT_ENABLED)

    if ((Env.version_runtime >= OCI_10_1) && OCILobCopy2)
    {
        Env.use_lob_ub8 = TRUE;
    }

  #endif

  #if defined(OCI_STMT_SCROLLABLE_READONLY)

    if ((Env.version_runtime >= OCI_9_0) && OCIStmtFetch2)
    {
        Env.use_scrollable_cursors = TRUE;
    }

  #endif

#endif

#if defined(OCI_CHARSET_WIDE)

    /* Oracle 8i does not support full Unicode mode */

    if (Env.version_runtime < OCI_9_0)
    {
        THROW(OcilibExceptionNotAvailable, OCI_FEATURE_WIDE_USERDATA)
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

    if (Env.version_runtime >= OCI_9_2)
    {
        ub4 nls_mode    = (ub4) oci_mode;
        ub2 charset = OCI_DEFAULT;
        
    #ifdef OCI_CHARSET_WIDE

        nls_mode    = (ub4) nls_mode &~OCI_ENV_MODE;
        charset     = OCI_UTF16ID;

    #endif

        ret = OCIEnvNlsCreate(&Env.env, nls_mode,
                           (dvoid *) &Env,
                           OcilibMemoryAllocOracleCallback,
                           OcilibMemoryReallocOracleCallback,
                           OcilibMemoryFreeOracleCallback,
                           (size_t) 0, (dvoid **) NULL,
                            charset,
                            charset);

    }
    else if (Env.version_runtime >= OCI_8_1)
    {
        ret = OCIEnvCreate(&Env.env, oci_mode,
                           (dvoid *) &Env,
                           OcilibMemoryAllocOracleCallback,
                           OcilibMemoryReallocOracleCallback,
                           OcilibMemoryFreeOracleCallback,
                           (size_t) 0, (dvoid **) NULL);
    }
    else if (Env.version_runtime == OCI_8_0)
    {
        ret = OCIInitialize(oci_mode, 
                           (dvoid *) &Env,
                            OcilibMemoryAllocOracleCallback,
                            OcilibMemoryReallocOracleCallback,
                            OcilibMemoryFreeOracleCallback);

        if (OCI_SUCCESSFUL(ret))
        {
            ret = OCIEnvInit(&Env.env, OCI_DEFAULT, (size_t)0, (dvoid**)NULL);
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
                (dvoid *)Env.env,
                (dvoid **) (void *) &Env.err,
                OCI_HTYPE_ERROR
            )
        )
    }
    else
    {
        THROW_NO_ARGS(OcilibExceptionOCIEnvironment)
    }

    /* on success, we need to initialize OCIThread object support */

    if (LIB_THREADED)
    {
        OCIThreadProcessInit();

        CHECK_OCI
        (
            Env.err,
            OCIThreadInit, Env.env, Env.err
        )

        Env.mem_mutex= OcilibMutexCreateInternal();
        CHECK_NULL(Env.mem_mutex)
    }

    /* create thread key for thread errors */
 
    WARNING_DISABLE_UNSAFE_CONVERT

    if (LIB_THREADED)
    {
        Env.key_errs = OcilibThreadKeyCreateInternal((POCI_THREADKEYDEST)OcilibEnvironmentFreeError);
        CHECK_NULL(Env.key_errs)
    }

    WARNING_RESTORE_UNSAFE_CONVERT

    /* allocate connections internal list */

    Env.cons = OcilibListCreate(OCI_IPC_CONNECTION);
    CHECK_NULL(Env.cons)

    /* allocate pools internal list */

    Env.pools = OcilibListCreate(OCI_IPC_POOL);
    CHECK_NULL(Env.pools)

    /* allocate array internal list */

    Env.arrs = OcilibListCreate(OCI_IPC_ARRAY);
    CHECK_NULL(Env.arrs)

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* allocate connection pools internal list */

    Env.subs = OcilibListCreate(OCI_IPC_NOTIFY);
    CHECK_NULL(Env.subs)

#endif

    Env.loaded = TRUE;

    /* test for XA support */

#ifdef _WINDOWS
  #if OCI_VERSION_COMPILE >= OCI_10_1
    Env.use_xa = (NULL != xaoEnv);
  #else
    Env.use_xa = FALSE;
  #endif
#else
    Env.use_xa = TRUE;
#endif

    SET_RETVAL(Env.loaded)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentCleanup
 * --------------------------------------------------------------------------------------------- */

boolean OcilibEnvironmentCleanup
(
    void
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    boolean success = FALSE;

    unsigned int i = 0;

    /* exit if the environment is not loaded */

    if (!Env.loaded)
    {
        JUMP_EXIT()
    }

    success = TRUE;

    /* dispose list items */

    LIST_ATOMIC_REMOVE_ALL(Env.arrs,  OcilibArrayDispose)
    LIST_ATOMIC_REMOVE_ALL(Env.subs,  OcilibSubscriptionDispose)
    LIST_ATOMIC_REMOVE_ALL(Env.cons,  OcilibConnectionDispose)
    LIST_ATOMIC_REMOVE_ALL(Env.pools, OcilibPoolDispose)

    /* free all lists */

    OcilibListFree(Env.arrs);
    OcilibListFree(Env.subs);
    OcilibListFree(Env.cons);
    OcilibListFree(Env.pools);

    /* free key map */

    OcilibEnvironmentKeyMapFree();

    Env.cons    = NULL;
    Env.pools   = NULL;
    Env.subs    = NULL;
    Env.key_map = NULL;

    for (i = 0; i < OCI_FMT_COUNT; i++)
    {
        FREE(Env.formats[i])
    }

    for (i = 0; i < OCI_FMT_COUNT; i++)
    {
        FREE(LocaleStrings[i].str)
    }

    /* finalize OCIThread object support */

    if (LIB_THREADED)
    {
        /* free the memory mutex. We set its reference in the library structure to NULL first otherwise
           it would generate an OCI error when calling MemoryAllocHandle() for freeing the mutex object error handle
        */

        OCI_Mutex * mutex = Env.mem_mutex;

        Env.mem_mutex = NULL;

        if (NULL != mutex)
        {
            OcilibMutexFree(mutex);
        }

        success = OCI_SUCCESSFUL(OCIThreadTerm(Env.env, Env.err));

        /* free error thread key */

        if (NULL != Env.key_errs)
        {
            OCI_ThreadKey *key = Env.key_errs;
            OCI_Error     *err = OcilibErrorGet(FALSE, FALSE);

            Env.key_errs = NULL;

            OcilibEnvironmentFreeError(err);

            OcilibThreadKeySet(key, NULL);
            OcilibThreadKeyFree(key);
        }
    }

    /* set unloaded flag */

    Env.loaded = FALSE;

    /* close error handle */

    if (NULL != Env.err)
    {
        OcilibMemoryFreeHandle(Env.err, OCI_HTYPE_ERROR);
    }

    /* close environment handle */

    if (Env.env)
    {
        OCIHandleFree(Env.env, OCI_HTYPE_ENV);
    }

#ifdef OCI_IMPORT_RUNTIME

    if (NULL != Env.lib_handle)
    {
        LIB_CLOSE(Env.lib_handle);
    }

#endif

    /* checks for non freed handles */

    if (Env.nb_hndlp > 0)
    {
        OcilibExceptionUnfreedData(&call_context, OCI_HDLE_HANDLE, (int) Env.nb_hndlp);
        success = FALSE;
    }

    /* checks for non freed descriptors */

    if (Env.nb_descp > 0)
    {
        OcilibExceptionUnfreedData(&call_context, OCI_HDLE_DESCRIPTOR, (int) Env.nb_descp);
        success = FALSE;
    }

    /* checks for non freed objects */

    if (Env.nb_objinst > 0)
    {
        OcilibExceptionUnfreedData(&call_context, OCI_HDLE_OBJECT, (int) Env.nb_objinst);
        success = FALSE;
    }

    /* checks for non freed library memory */

    if (Env.mem_bytes_lib > 0)
    {
        OcilibExceptionUnfreedBytes(&call_context, Env.mem_bytes_lib);
        success = FALSE;
    }

    /* free environment errors */

    if (NULL != Env.lib_err)
    {
        OcilibErrorFree(Env.lib_err);
    }

    /* reset environment object */

    memset(&Env, 0, sizeof(Env));

    CHECK(TRUE)

    CLEANUP_AND_EXIT_FUNC
    (
        SET_RETVAL(success)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentGetOCICompileVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibEnvironmentGetOCICompileVersion
(
    void
)
{
    GET_LIB_PROP
    (
        /* result */ unsigned int, OCI_UNKNOWN,
        /* value  */ Env.version_compile
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentGetOCIRuntimeVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibEnvironmentGetOCIRuntimeVersion
(
    void
)
{
    GET_LIB_PROP
    (
        /* result */ unsigned int, OCI_UNKNOWN,
        /* value  */ Env.version_runtime
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentGetOCIRuntimeImportMode
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibEnvironmentGetOCIRuntimeImportMode
(
    void
)
{
    GET_LIB_PROP
    (
        /* result */ unsigned int, OCI_UNKNOWN,
        /* value  */ OCI_IMPORT_MODE
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentGetCharset
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibEnvironmentGetCharset
(
    void
)
{
    GET_LIB_PROP
    (
        /* result */ unsigned int, OCI_UNKNOWN,
        /* value  */ OCI_CHAR_TEXT
    )
}

/* --------------------------------------------------------------------------------------------- *
* OcilibEnvironmentGetAllocatedBytes
* --------------------------------------------------------------------------------------------- */

big_uint OcilibEnvironmentGetAllocatedBytes
(
    unsigned int mem_type
)
{
    ENTER_FUNC
    (
        /* returns */ big_uint, 0,
        /* context */ OCI_IPC_VOID, &Env
    )

    big_uint bytes_count = 0;

    CHECK_INITIALIZED()

    if (mem_type & OCI_MEM_ORACLE)
    {
        bytes_count += Env.mem_bytes_oci;
    }

    if (mem_type & OCI_MEM_OCILIB)
    {
        bytes_count += Env.mem_bytes_lib;
    }

    SET_RETVAL(bytes_count)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentGetLastError
 * --------------------------------------------------------------------------------------------- */

OCI_Error * OcilibEnvironmentGetLastError
(
    void
)
{
    OCI_Error *err = NULL;

    if (!Env.loaded || Env.env_mode & OCI_ENV_CONTEXT)
    {
        err = OcilibErrorGet(TRUE, FALSE);

        if (NULL != err && OCI_UNKNOWN == err->code)
        {
            err = NULL;
        }

    }

    return err;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentEnableWarnings
 * --------------------------------------------------------------------------------------------- */

boolean OcilibEnvironmentEnableWarnings
(
    boolean value
)
{
    SET_LIB_PROP
    (
        /* member */ Env.warnings_on,
        /* value  */ value
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentSetErrorHandler
 * --------------------------------------------------------------------------------------------- */

boolean OcilibEnvironmentSetErrorHandler
(
    POCI_ERROR handler
)
{
    SET_LIB_PROP
    (
        /* member */ Env.error_handler,
        /* value  */ handler
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEnvironmentSetHAHandler
 * --------------------------------------------------------------------------------------------- */

boolean OcilibEnvironmentSetHAHandler
(
    POCI_HA_HANDLER handler
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    void *callback = NULL;

    CHECK_INITIALIZED()
    CHECK_HIGH_AVAILABILITY_ENABLED()

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* On MSVC, casting a function pointer to a data pointer generates a warning.
       As there is no other to way to do regarding the OCI API, let's disable this
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
        Env.env, callback, 0,
        Env.err
    )

    Env.ha_handler = handler;

    WARNING_RESTORE_CAST_FUNC_TYPE
    WARNING_RESTORE_PEDANTIC

#else

    OCI_NOT_USED(callback)

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
* OcilibGetLocaleString
* --------------------------------------------------------------------------------------------- */

const otext* OcilibGetLocaleString
(
    unsigned int code
)
{
    ENTER_FUNC
    (
        /* returns */ const otext *, NULL,
        /* context */ OCI_IPC_VOID, &Env
    )

    dbtext *dbstr = NULL;
    int     dbsize = OCI_SIZE_BUFFER_LSI;

    CHECK_INITIALIZED()


#if OCI_VERSION_COMPILE >= OCI_9_2

    size_t i = 0, n = sizeof(LocaleStrings) / sizeof((LocaleStrings)[0]);
    for (; i < n; i++)
    {
        if (code == LocaleStrings[i].code)
        {
            break;
        }
    } 
     
    if (i >= n)
    {
        THROW(OcilibExceptionArgInvalidValue, OTEXT("Locale Code"), code)
    }

    if (LocaleStrings[i].str == NULL)
    {
        LocaleStrings[i].str = (otext*)OcilibMemoryAlloc(OCI_IPC_STRING, (size_t)OCI_SIZE_BUFFER_LSI + 1, (size_t)1, TRUE);

        dbstr = OcilibStringGetDBString(LocaleStrings[i].str, &dbsize);

        CHECK_OCI
        (
            Env.err,
            OCINlsGetInfo,
            Env.env,
            Env.err,
            (OraText*)dbstr,
            (size_t)dbsize,
            (ub2)code
        )

        OcilibStringCopyDBStringToNativeString(dbstr, LocaleStrings[i].str, OCI_SIZE_BUFFER_LSI);
    }

    SET_RETVAL(LocaleStrings[i].str)

#else

    OCI_NOT_USED(type)

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(dbstr);
    )
}


/* --------------------------------------------------------------------------------------------- *
* OcilibEnvironmentSetFormat
* --------------------------------------------------------------------------------------------- */

boolean OcilibEnvironmentSetFormat
(
    OCI_Connection *con,
    unsigned int    type,
    const otext    *format
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    otext **value = NULL;

    CHECK_INITIALIZED()
    CHECK_ENUM_VALUE(type, FormatTypeValues, OTEXT("Format Type"))

    value = con ? &con->formats[type - 1] : &Env.formats[type - 1];

    FREE(*value)

    *value = OcilibStringDuplicate(format ? format : FormatDefaultValues[type-1]);

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
* OcilibEnvironmentGetFormat
* --------------------------------------------------------------------------------------------- */

const otext * OcilibEnvironmentGetFormat
(
    OCI_Connection *con,
    unsigned int    type
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_VOID, &Env
    )

    otext **value = NULL;

    CHECK_INITIALIZED()
    CHECK_ENUM_VALUE(type, FormatTypeValues, OTEXT("Format Type"))

    value = con ? &con->formats[type-1] : &Env.formats[type-1];

    if (NULL == *value)
    {
        CHECK(OcilibEnvironmentSetFormat(con, type, NULL))
    }

    SET_RETVAL(*value)

    EXIT_FUNC()
}

WARNING_RESTORE_CAST_FUNC_TYPE
