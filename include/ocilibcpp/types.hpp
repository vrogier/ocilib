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

#pragma once

#include <cstddef>
#include <iterator>
#include <vector>

#include "ocilibcpp/core.hpp"
#include "ocilibcpp/support.hpp"

 // ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
 // ReSharper disable CppClangTidyModernizeUseNodiscard
 // ReSharper disable CppClangTidyHicppSpecialMemberFunctions
 // ReSharper disable CppClangTidyReadabilityRedundantDeclaration
 // ReSharper disable CppFunctionIsNotImplemented
 // ReSharper disable CppClangTidyCppcoreguidelinesSpecialMemberFunctions
 // ReSharper disable CppClassCanBeFinal

namespace ocilib
{

    /**
    * @brief
    * Oracle Version enumerated values
    *
    * @note
    * This list is NOT exhaustive and contains only a list of well known released versions
    *
    */
    enum OracleVersionValues
    {
        Oracle80 = OCI_8_0,
        Oracle8i = OCI_8_1,
        Oracle9iR1 = OCI_9_0,
        Oracle9iR2 = OCI_9_2,
        Oracle10gR1 = OCI_10_1,
        Oracle10gR2 = OCI_10_2,
        Oracle11gR1 = OCI_11_1,
        Oracle11gR2 = OCI_11_2,
        Oracle12cR1 = OCI_12_1,
        Oracle12cR2 = OCI_12_2,
        Oracle18cR1 = OCI_18_1,
        Oracle18cR2 = OCI_18_2,
        Oracle18cR3 = OCI_18_3,
        Oracle18cR4 = OCI_18_4,
        Oracle18cR5 = OCI_18_5,
        Oracle19cR3 = OCI_19_3,
        Oracle19cR5 = OCI_19_5,
        Oracle21cR3 = OCI_21_3
    };

    /**
    * @brief
    * Oracle Version
    *
    * Possible values are DataTypeValues
    *
    */
    typedef core::Enum<OracleVersionValues> OracleVersion;

    /**
    * @brief
    * Data type enumerated values
    *
    */
    enum DataTypeValues
    {
        /** NUMERIC */
        TypeNumeric = OCI_CDT_NUMERIC,
        /** DATE */
        TypeDate = OCI_CDT_DATETIME,
        /** TEXT */
        TypeString = OCI_CDT_TEXT,
        /** LONG / LONG RAW */
        TypeLong = OCI_CDT_LONG,
        /** SQL CURSOR */
        TypeStatement = OCI_CDT_CURSOR,
        /** CLOB or BLOB */
        TypeLob = OCI_CDT_LOB,
        /** FILE */
        TypeFile = OCI_CDT_FILE,
        /** TIMESTAMP */
        TypeTimestamp = OCI_CDT_TIMESTAMP,
        /** INTERVAL */
        TypeInterval = OCI_CDT_INTERVAL,
        /** RAW */
        TypeRaw = OCI_CDT_RAW,
        /** Object type */
        TypeObject = OCI_CDT_OBJECT,
        /** Collection  */
        TypeCollection = OCI_CDT_COLLECTION,
        /** Object REF */
        TypeReference = OCI_CDT_REF,
        /** PL/SQL boolean */
        TypeBoolean = OCI_CDT_BOOLEAN,
        /** XMLTYPE */
        TypeXmlType = OCI_CDT_XMLTYPE,
        /** VECTOR */
        TypeVector = OCI_CDT_VECTOR,
        /** JSON */
        TypeJson = OCI_CDT_JSON
    };

    /**
    * @brief
    * Column data type
    *
    * Possible values are DataTypeValues
    *
    */
    typedef core::Enum<DataTypeValues> DataType;

    /**
    * @brief
    * Numeric type enumerated values
    *
    */
    enum NumericTypeValues
    {
        /** Signed Int16 */
        NumericShort = OCI_NUM_SHORT,
        /** Signed Integer */
        NumericInt = OCI_NUM_INT,
        /** Signed Big Integer (Integer or Long Integer depending on the compiler )*/
        NumericBigInt = OCI_NUM_BIGINT,
        /** Unsigned Int16 */
        NumericUnsignedShort = OCI_NUM_USHORT,
        /** Unsigned Integer */
        NumericUnsignedInt = OCI_NUM_UINT,
        /** Unsigned Big Integer (Integer or Long Integer depending on the compiler )*/
        NumericUnsignedBigInt = OCI_NUM_BIGUINT,
        /** Double  */
        NumericDouble = OCI_NUM_DOUBLE,
        /** Float */
        NumericFloat = OCI_NUM_FLOAT,
        /** Number */
        NumericNumber = OCI_NUM_NUMBER
    };

    /**
    * @brief
    * Numeric data type
    *
    * Possible values are NumericTypeValues
    *
    */
    typedef core::Enum<NumericTypeValues> NumericType;

    /**
    * @brief
    * Charset form enumerated values
    *
    */
    enum CharsetFormValues
    {
        /** Database character set will be converted to the server national character set */
        CharsetFormDefault = OCI_CSF_DEFAULT,
        /** Client national character set will be converted to the server national character set */
        CharsetFormNational = OCI_CSF_NATIONAL
    };

    /**
    * @brief
    * Type of charsetForm
    *
    * Possible values are CharsetFormValues
    *
    */
    typedef core::Enum<CharsetFormValues> CharsetForm;

    /**
    * @brief
    * Collation ID enumerated values
    *
    */
    enum CollationIDValues
    {
        /** None */
        CollationIDNone = OCI_CCI_NONE,
        /**  Compare */
        CollationIDNlsComp = OCI_CCI_NLS_COMP,
        /** Sort */
        CollationIDNlsSort = OCI_CCI_NLS_SORT,
        /** Sort CI */
        CollationIDNlsSortCI = OCI_CCI_NLS_SORT_CI,
        /** Sort AI */
        CollationIDNlsSortAI = OCI_CCI_NLS_SORT_AI,
        /** Sort CS */
        CollationIDNlsSortCS = OCI_CCI_NLS_SORT_CS,
        /** Sort VAR 1 */
        CollationIDNlsSortVar1 = OCI_CCI_NLS_SORT_VAR1,
        /** Sort VAR 1 CI */
        CollationIDNlsSortVar1CI = OCI_CCI_NLS_SORT_VAR1_CI,
        /** Sort VAR 1 AI */
        CollationIDNlsSortVar1AI = OCI_CCI_NLS_SORT_VAR1_AI,
        /** Sort VAR 1 CS */
        CollationIDNlsSortVar1CS = OCI_CCI_NLS_SORT_VAR1_CS,
        /** Binary */
        CollationIDBinary = OCI_CCI_BINARY,
        /** Binary CI */
        CollationIDBinaryCI = OCI_CCI_BINARY_CI,
        /** Binary AI */
        CollationIDBinaryAI = OCI_CCI_BINARY_AI
    };

    /**
    * @brief
    * Type of Collation ID
    *
    * Possible values are CollationIDValues
    *
    */
    typedef core::Enum<CollationIDValues> CollationID;


    /**
    * @brief
    * Seek Modes enumerated values
    *
    */
    enum SeekModeValues
    {
        /** Set the current position within the stream to the given position */
        SeekSet = OCI_SEEK_SET,
        /** Set the current position within the stream begin position */
        SeekEnd = OCI_SEEK_END,
        /** Set the current position within the stream to the given value offset from the current position */
        SeekOffset = OCI_SEEK_CUR
    };

    /**
    * @brief
    * Seek Modes
    *
    * Possible values are SeekModeValues
    *
    */
    typedef core::Enum<SeekModeValues> SeekMode;

    /**
    * @brief
    * Open Modes enumerated values
    *
    */
    enum OpenModeValues
    {
        /** Stream is opened for read access */
        ReadOnly = OCI_LOB_READONLY,
        /** Stream is opened for read / write access */
        ReadWrite = OCI_LOB_READWRITE
    };

    /**
    * @brief
    * Open Modes
    *
    * Possible values are OpenModeValues
    *
    */
    typedef core::Enum<OpenModeValues> OpenMode;

    /**
    * @brief
    * Lob types enumerated values
    *
    */
    enum LobTypeValues
    {
        /** Binary Large Object (BLOB) */
        LobBinary = OCI_BLOB,
        /** Character Large Object (CLOB) */
        LobCharacter = OCI_CLOB,
        /** National Character Large Object (NCLOB) */
        LobNationalCharacter = OCI_NCLOB
    };

    /**
    * @brief
    * Type of Lob
    *
    * Possible values are LobTypeValues
    *
    */
    typedef core::Enum<LobTypeValues> LobType;

    /**
    * @brief
    * Long types enumerated values
    *
    */
    enum LongTypeValues
    {
        /** Binary LONG RAW */
        LongBinary = OCI_BLONG,
        /** Character LONG */
        LongCharacter = OCI_CLONG
    };

    /**
    * @brief
    * Type of Long
    *
    * Possible values are LongTypeValues
    *
    */
    typedef core::Enum<LongTypeValues> LongType;

    /**
    * @brief
    * Format type enumerated values
    *
    */
    enum FormatTypeValues
    {
        /** Date */
        FormatDate = OCI_FMT_DATE,
        /** Timestamp */
        FormatTimestamp = OCI_FMT_TIMESTAMP,
        /** All numeric types */
        FormatNumeric = OCI_FMT_NUMERIC,
        /** BinaryDouble */
        FormatBinaryDouble = OCI_FMT_BINARY_DOUBLE,
        /** Binary Float */
        FormatBinaryFloat = OCI_FMT_BINARY_FLOAT
    };

    /**
    * @brief
    * Format type
    *
    * Possible values are FormatTypeValues
    *
    */
    typedef core::Enum<FormatTypeValues> FormatType;

    /**
     *
     * @brief
     * Exception class handling all OCILIB errors
     *
     * This class wraps the OCILIB object handle OCI_Error and its related methods
     *
     */
    class Exception : public std::exception
    {
        template<class T>
        friend T core::Check(T result);
        friend class Statement;

    public:

        /**
        * @brief
        * Exception type enumerated values
        *
        */
        enum ExceptionTypeValues
        {
            /** Exception caused by an Oracle error */
            OracleError = OCI_ERR_ORACLE,
            /** Exception caused by an Ocilib error */
            OcilibError = OCI_ERR_OCILIB,
            /** Exception caused by an Oracle Warning */
            OracleWarning = OCI_ERR_WARNING
        };

        /**
        * @brief
        * Type of Exception
        *
        * Possible values are Exception::ExceptionTypeValues
        *
        */
        typedef core::Enum<ExceptionTypeValues>  ExceptionType;

        /**
         * @brief
         * Retrieve the error message
         *
         */
        ostring GetMessage() const;

        /**
         *
         * @brief
         * Return the Exception type
         *
         */
        ExceptionType GetType() const;

        /**
         * @brief
         * Return the Oracle error code
         *
         */
        Int32 GetOracleErrorCode() const;

        /**
         * @brief
         * Return the OCILIB error code
         *
         */
        Int32 GetInternalErrorCode() const;

        /**
         * @brief
         * Return the statement within the error occurred
         *
         */
        Statement GetStatement() const;

        /**
         * @brief
         * Return the connection within the error occurred
         *
         */
        Connection GetConnection() const;

        /**
         * @brief
         * Return the row index which caused an error during statement execution
         *
         * @warning
         * Row index start at 1.
         *
         * @return
         *  - 0 if the error is not related to array DML
         *  - otherwise the index of the given row which caused the error
         *
         */
        UInt32 GetRow() const;

        /**
        * @brief
        * Override the std::exception::what() method
        *
        * @return
        *  - The same content as GetMessage() but as using const char * type
        *
        */
        const char* what() const noexcept override;

        Exception(const Exception& other) noexcept;

        Exception& operator = (const Exception& other) noexcept;

        /**
        * @brief
        * Virtual destructor required for deriving from std::exception
        *
        */
        virtual ~Exception() noexcept;

    private:

        Exception() noexcept;
        Exception(OCI_Error* err) noexcept;

        void SetWhat(const otext* value) noexcept;
        void CopyWhat(const char* value) noexcept;

        char* _what;
        OCI_Statement* _pStatement;
        OCI_Connection* _pConnection;
        UInt32 _row;
        ExceptionType _type;
        Int32 _errLib;
        Int32 _errOracle;
    };

    /**
     * @brief
     * Static class in charge of library initialization / cleanup
     *
     */
    class Environment
    {
        friend class Connection;
        friend class Pool;
        friend class TypeInfo;
        friend class Subscription;
        friend class Dequeue;
        friend class Date;
        friend class Timestamp;
        friend class Number;
        friend class Interval;
        friend class Agent;
        friend class Transaction;
        friend class Exception;
        friend class File;
        friend class Log;
        friend class Statement;
        friend class Object;
        friend class Reference;
        friend class Message;
        friend class Event;
        friend class Column;
        template<class, Int32>
        friend class Lob;

        friend class core::HandleStore;

    public:

        /**
        * @brief
        * Environment Flags enumerated values
        *
        */
        enum EnvironmentFlagsValues
        {
            /** Default mode */
            Default = OCI_ENV_DEFAULT,
            /** Enable support for multi-threading */
            Threaded = OCI_ENV_THREADED,
            /** Enable support for events related to subscriptions, HA and AQ notifications */
            Events = OCI_ENV_EVENTS
        };

        /**
        * @brief
        * Environment Flags
        *
        * Possible values are Environment::EnvironmentFlagsValues
        *
        */
        typedef core::Flags<EnvironmentFlagsValues> EnvironmentFlags;



        Environment(EnvironmentFlags mode = Environment::Default, const ostring& libpath = OTEXT(""));
        ~Environment();


        /**
        * @brief
        * HA Event sources enumerated values
        *
        */
        enum HAEventSourceValues
        {
            /** The event has been generated by an Oracle instance */
            SourceInstance = OCI_HES_INSTANCE,
            /** The event has been generated by an Oracle database */
            SourceDatabase = OCI_HES_DATABASE,
            /** The event has been generated by an Oracle Node */
            SourceNode = OCI_HES_NODE,
            /** The event has been generated by an Oracle service */
            SourceService = OCI_HES_SERVICE,
            /** The event has been generated by an Oracle service member */
            SourceServiceMember = OCI_HES_SERVICE_MEMBER,
            /** The event has been generated by an Oracle ASM instance */
            SourceASMInstance = OCI_HES_ASM_INSTANCE,
            /** The event has been generated at service pre connecting stage */
            SourcePreConnect = OCI_HES_PRECONNECT
        };

        /**
        * @brief
        * Source of HA events
        *
        * Possible values are Environment::HAEventSourceValues
        *
        */
        typedef core::Enum<HAEventSourceValues>  HAEventSource;

        /**
        * @brief
        * HA Event types enumerated values
        *
        */
        enum HAEventTypeValues
        {
            /** The source of the event is down */
            EventDown = OCI_HET_DOWN,
            /** The source of the event is up (not generated yet by Oracle) */
            EventUp = OCI_HET_UP
        };

        /**
        * @brief
        * Type of HA events
        *
        * Possible values are Environment::HAEventTypeValues
        *
        */
        typedef core::Enum<HAEventTypeValues>  HAEventType;

        /**
        * @brief
        * OCI libraries import mode enumerated values
        *
        */
        enum ImportModeValues
        {
            /** OCI libraries are linked at compile time */
            ImportLinkage = OCI_IMPORT_MODE_LINKAGE,
            /** OCI libraries are dynamically loaded at runtime */
            ImportRuntime = OCI_IMPORT_MODE_RUNTIME
        };

        /**
        * @brief
        * OCI libraries import mode
        *
        * Possible values are Environment::ImportMode
        *
        */
        typedef core::Enum<ImportModeValues> ImportMode;

        /**
        * @brief
        * Charset mode enumerated values
        *
        */
        enum CharsetModeValues
        {
            /** Environment is ANSI string or UTF8 string */
            CharsetAnsi = OCI_CHAR_ANSI,
            /** Environment is UNICODE using wide character string  */
            CharsetWide = OCI_CHAR_WIDE
        };

        /**
        * @brief
        * Environment charset mode
        *
        * Possible values are Environment::CharsetModeValues
        *
        */
        typedef core::Enum<CharsetModeValues> CharsetMode;

        /**
        * @brief
        * Session flags enumerated values
        *
        */
        enum SessionFlagsValues
        {
            /** Default session mode */
            SessionDefault = OCI_SESSION_DEFAULT,
            /** the user is authenticated for SYSDBA access. */
            SessionSysDba = OCI_SESSION_SYSDBA,
            /** the user is authenticated for SYSOPER access */
            SessionSysOper = OCI_SESSION_SYSOPER,
            /** the user is authenticated for SYSASM access */
            SessionSysAsm = OCI_SESSION_SYSASM,
            /** the user is authenticated for SYSBKP access */
            SessionSysBkp = OCI_SESSION_SYSBKP,
            /** the user is authenticated for SYSDGD access */
            SessionSysDgD = OCI_SESSION_SYSDGD,
            /** the user is authenticated for SYSKMT access */
            SessionSysKmt = OCI_SESSION_SYSKMT,
            /** the user is authenticated for SYSRAC access */
            SessionSysRac = OCI_SESSION_SYSRAC,
            /** the user is authenticated for PRELIM_AUTH access */
            SessionPreAuth = OCI_SESSION_PRELIM_AUTH
        };

        /**
        * @brief
        * Sessions flags
        *
        * Possible values are Environment::SessionFlagsValues
        *
        */
        typedef core::Flags<SessionFlagsValues> SessionFlags;

        /**
        * @brief
        * Oracle instance start modes enumerated values
        *
        */
        enum StartModeValues
        {
            /** Start the instance without mounting and opening it */
            StartOnly = OCI_DB_SPM_START,
            /** Mount (only) the instance */
            StartMount = OCI_DB_SPM_MOUNT,
            /** Open (only)  the instance */
            StartOpen = OCI_DB_SPM_OPEN,
            /** Start, mount and open the instance */
            StartFull = OCI_DB_SPM_FULL
        };

        /**
        * @brief
        * Oracle instance start modes
        *
        * Possible values are Environment::StartModeValues
        *
        */
        typedef core::Flags<StartModeValues> StartMode;

        /**
        * @brief
        * Oracle instance start flags enumerated values
        *
        */
        enum StartFlagsValues
        {
            /** Default start flags */
            StartDefault = OCI_DB_SPF_DEFAULT,
            /** Shuts down a running instance (if needed) using ABORT command and starts a new instance */
            StartForce = OCI_DB_SPF_FORCE,
            /** Allows database access only to users with both CREATE SESSION and RESTRICTED SESSION privileges */
            StartRestrict = OCI_DB_SPF_RESTRICT
        };

        /**
        * @brief
        * Oracle instance start flags
        *
        * Possible values are Environment::StartFlagsValues
        *
        */
        typedef core::Flags<StartFlagsValues> StartFlags;

        /**
        * @brief
        * Oracle instance shutdown modes enumerated values
        *
        */
        enum ShutdownModeValues
        {
            /** Shutdown the instance */
            ShutdownOnly = OCI_DB_SDM_SHUTDOWN,
            /** Close (only) the instance */
            ShutdownClose = OCI_DB_SDM_CLOSE,
            /** Dismount (only)  the instance */
            ShutdownDismount = OCI_DB_SDM_DISMOUNT,
            /** Shutdown, close and dismount the instance */
            ShutdownFull = OCI_DB_SDM_FULL
        };

        /**
        * @brief
        * Oracle instance shutdown modes
        *
        * Possible values are Environment::ShutdownModeValues
        *
        */
        typedef core::Flags<ShutdownModeValues> ShutdownMode;

        /**
        * @brief
        * Oracle instance shutdown flags enumerated values
        *
        */
        enum ShutdownFlagsValues
        {
            /**  - Further connects are prohibited.
              *  - Waits for users to disconnect from the database */
            ShutdowntDefault = OCI_DB_SDF_DEFAULT,
            /**  - Further connects are prohibited
              *  - No new transactions are allowed. */
            ShutdowTrans = OCI_DB_SDF_TRANS,
            /**  - Further connects are prohibited
              *  - No new transactions are allowed.
              *  - Waits for active transactions to complete */
            ShutdownTransLocal = OCI_DB_SDF_TRANS_LOCAL,
            /**  - Does not wait for current calls to complete or users to disconnect from the database.
              *  - All uncommitted transactions are terminated and rolled back */
            ShutdownImmediate = OCI_DB_SDF_IMMEDIATE,
            /**  - Does not wait for current calls to complete or users to disconnect from the database.
              *  - All uncommitted transactions are terminated and are not rolled back.
              *  - This is the fastest possible way to shut down the database, but the next
              *    database startup may require instance recovery.
              *  - Therefore, this option should be used only in unusual circumstances */
            ShutdownAbort = OCI_DB_SDF_ABORT
        };

        /**
        * @brief
        * Oracle instance shutdown flags
        *
        * Possible values are Environment::ShutdownFlagsValues
        *
        */
        typedef core::Flags<ShutdownFlagsValues> ShutdownFlags;

        /**
        * @brief
        * Locales string enumerated values
        *
        */
        enum LocaleCodeValues
        {
            LocaleDayname1 = OCI_LSI_DAYNAME1,
            LocaleDayname2 = OCI_LSI_DAYNAME2,
            LocaleDayname3 = OCI_LSI_DAYNAME3,
            LocaleDayname4 = OCI_LSI_DAYNAME4,
            LocaleDayname5 = OCI_LSI_DAYNAME5,
            LocaleDayname6 = OCI_LSI_DAYNAME6,
            LocaleDayname7 = OCI_LSI_DAYNAME7,
            LocaleAbdayname1 = OCI_LSI_ABDAYNAME1,
            LocaleAbdayname2 = OCI_LSI_ABDAYNAME2,
            LocaleAbdayname3 = OCI_LSI_ABDAYNAME3,
            LocaleAbdayname4 = OCI_LSI_ABDAYNAME4,
            LocaleAbdayname5 = OCI_LSI_ABDAYNAME5,
            LocaleAbdayname6 = OCI_LSI_ABDAYNAME6,
            LocaleAbdayname7 = OCI_LSI_ABDAYNAME7,
            LocaleMonthname1 = OCI_LSI_MONTHNAME1,
            LocaleMonthname2 = OCI_LSI_MONTHNAME2,
            LocaleMonthname3 = OCI_LSI_MONTHNAME3,
            LocaleMonthname4 = OCI_LSI_MONTHNAME4,
            LocaleMonthname5 = OCI_LSI_MONTHNAME5,
            LocaleMonthname6 = OCI_LSI_MONTHNAME6,
            LocaleMonthname7 = OCI_LSI_MONTHNAME7,
            LocaleMonthname8 = OCI_LSI_MONTHNAME8,
            LocaleMonthname9 = OCI_LSI_MONTHNAME9,
            LocaleMonthname10 = OCI_LSI_MONTHNAME10,
            LocaleMonthname11 = OCI_LSI_MONTHNAME11,
            LocaleMonthname12 = OCI_LSI_MONTHNAME12,
            LocaleAbmonthname1 = OCI_LSI_ABMONTHNAME1,
            LocaleAbmonthname2 = OCI_LSI_ABMONTHNAME2,
            LocaleAbmonthname3 = OCI_LSI_ABMONTHNAME3,
            LocaleAbmonthname4 = OCI_LSI_ABMONTHNAME4,
            LocaleAbmonthname5 = OCI_LSI_ABMONTHNAME5,
            LocaleAbmonthname6 = OCI_LSI_ABMONTHNAME6,
            LocaleAbmonthname7 = OCI_LSI_ABMONTHNAME7,
            LocaleAbmonthname8 = OCI_LSI_ABMONTHNAME8,
            LocaleAbmonthname9 = OCI_LSI_ABMONTHNAME9,
            LocaleAbmonthname10 = OCI_LSI_ABMONTHNAME10,
            LocaleAbmonthname11 = OCI_LSI_ABMONTHNAME11,
            LocaleAbmonthname12 = OCI_LSI_ABMONTHNAME12,
            LocaleYes = OCI_LSI_YES,
            LocaleNo = OCI_LSI_NO,
            LocaleAm = OCI_LSI_AM,
            LocalePm = OCI_LSI_PM,
            LocaleAd = OCI_LSI_AD,
            LocaleBc = OCI_LSI_BC,
            LocaleDecimal = OCI_LSI_DECIMAL,
            LocaleGroup = OCI_LSI_GROUP,
            LocaleDebit = OCI_LSI_DEBIT,
            LocaleCredit = OCI_LSI_CREDIT,
            LocaleDateformat = OCI_LSI_DATEFORMAT,
            LocaleInt = OCI_LSI_INT_CURRENCY,
            LocaleLoc = OCI_LSI_LOC_CURRENCY,
            LocaleLanguage = OCI_LSI_LANGUAGE,
            LocaleAblanguage = OCI_LSI_ABLANGUAGE,
            LocaleTerritory = OCI_LSI_TERRITORY,
            LocaleCharacterSEt = OCI_LSI_CHARACTER_SET,
            LocaleLinguisticName = OCI_LSI_LINGUISTIC_NAME,
            LocaleCalendar = OCI_LSI_CALENDAR,
            LocaleDualCurrency = OCI_LSI_DUAL_CURRENCY,
            LocaleWritingdir = OCI_LSI_WRITINGDIR,
            LocaleAbterritory = OCI_LSI_ABTERRITORY,
            LocaleDdateformat = OCI_LSI_DDATEFORMAT,
            LocaleDtimeformat = OCI_LSI_DTIMEFORMAT,
            LocaleSfdateformat = OCI_LSI_SFDATEFORMAT,
            LocaleSftimeformat = OCI_LSI_SFTIMEFORMAT,
            LocaleNumgrouping = OCI_LSI_NUMGROUPING,
            LocaleListsep = OCI_LSI_LISTSEP,
            LocaleMondecimal = OCI_LSI_MONDECIMAL,
            LocaleMongroup = OCI_LSI_MONGROUP,
            LocaleMongrouping = OCI_LSI_MONGROUPING,
            LocaleIntCurrenySep = OCI_LSI_INT_CURRENCYSEP
        };

        /**
        * @brief
        * Locales string
        *
        * Possible values are Environment::AllocatedBytesValues
        *
        */
        typedef core::Enum<LocaleCodeValues> LocaleCode;
        /**
        * @typedef HAHandlerProc
        *
        * @brief
        * User callback for HA event notifications
        *
        */
        typedef void(*HAHandlerProc) (Connection& con, HAEventSource eventSource, HAEventType eventType, Timestamp& time);

        /**
         * @brief
         * Initialize the OCILIB environment
         *
         * @param mode         - Environment mode
         * @param libpath      - Oracle shared library path (optional)
         *
         * @note
         * An Environment instance must be created before any other OCILIB library function.
         *
         * @warning
         * - It should be called <b>ONCE</b> per application
         * - It is not thread safe
         *
         * @note
         * - The parameter 'libpath' is only used if OCILIB has been built with the option OCI_IMPORT_RUNTIME
         * - If the parameter 'lib_path' is NULL, the Oracle library is loaded from system environment variables
         *
         */
/**
         * @brief
         * Clean up all resources allocated by the environment
         *
         * @note
         * This function must be the last OCILIB library function call.
         * - It deallocates library managed objects not explicitly freed by the program (connections, statements, ...)
         * - It unloads the Oracle shared library if it has been dynamically loaded
         *
         * @warning
         * - It should be called <b>ONCE</b> per application
         * - It is not thread safe
         *
         */
/**
         * @brief
         * Return the Environment mode flags
         *
         * @note
         * It returns the value of the parameter 'mode' passed to the constructor
         *
         */
        Environment::EnvironmentFlags GetMode() const;

        /**
         * @brief
         * Return the Oracle shared library import mode
         *
         */
        Environment::ImportMode GetImportMode() const;

        /**
         * @brief
         * Return the OCILIB charset type
         *
         */
        Environment::CharsetMode GetCharset() const;

        /**
         * @brief
         * Return maximum size for a character
         *
         * @note
         * When Charset is Environment::CharsetAnsi and NLS_LANG is UTF8 based, return value is 4
         * Otherwise, it is sizeof(ostring::value_type)
         *
         */
        UInt32 GetCharMaxSize() const;

        /**
        * @brief
        * Return the requested internal environment statistic
        *
        * @param stat : statistic to retrieve (see OCI_UST_*)
        *
        */
        UInt64 GetStats(OCI_Stat stat) const;

        /**
         * @brief
         * Return the version of OCI used for compiling OCILIB
         *
         * @note
         * - with linkage build option, the version is determined from the oci.h header through different ways
         * - with runtime loading build option, the version is set to the highest version
         *   of OCI needed by OCILIB, not necessarily the real OCI version
         *
         */
        OracleVersion GetCompileVersion() const;

        /**
        * @brief
        * Return the major version number of OCI used for compiling OCILIB
        *
        */
        UInt32 GetCompileMajorVersion() const;

        /**
        * @brief
        * Return the minor version number of OCI used for compiling OCILIB
        *
        */
        UInt32 GetCompileMinorVersion() const;

        /**
        * @brief
        * Return the revision version number of OCI used for compiling OCILIB
        *
        */
        UInt32 GetCompileRevisionVersion() const;

        /**
         * @brief
         * Return the version of OCI used at runtime
         *
         * @note
         * - with linkage build option, the version is determined from the oci.h header through different ways
         * - with runtime loading build option, the version determined from the symbols dynamically loaded.
         *
         */
        OracleVersion GetRuntimeVersion() const;

        /**
        * @brief
        * Return the major version number of OCI used at runtime
        *
        */
        UInt32 GetRuntimeMajorVersion() const;

        /**
        * @brief
        * Return the minor version number of OCI used at runtime
        *
        */
        UInt32 GetRuntimeMinorVersion() const;

        /**
        * @brief
        * Return the revision version number of OCI used at runtime
        *
        */
        UInt32 GetRuntimeRevisionVersion() const;

        /**
         * @brief
         * Enable or disable Oracle warning notifications
         *
         * @param value  - enable/disable warnings
         *
         * @note
         * Default value is false
         *
         */
        void EnableWarnings(bool value);

        /**
        * @brief
        * Set the format string for implicit string conversions of the given type
        *
        * @param formatType - Type of format
        * @param format     - Format string
        *
        * Formats can set at 2 levels:
        * - Environment level
        * - Connection level
        *
        *
        * @note
        * Default format values are :
        * - FormatType::FormatDate          : constant OCI_STRING_FORMAT_DATE
        * - FormatType::FormatTimestamp     : constant OCI_STRING_FORMAT_TIMESTAMP
        * - FormatType::FormatNumeric       : constant OCI_STRING_FORMAT_NUMERIC
        * - FormatType::FormatBinaryDouble  : constant OCI_STRING_FORMAT_BINARY_DOUBLE
        * - FormatType::FormatBinaryFloat   : constant OCI_STRING_FORMAT_BINARY_FLOAT
        *
        * @note
        * Conversions are performed by Oracle built-in functions whenever possible.
        * For DATE, TIMESTAMP and numeric types, see documentation of Oracle SQL to_char() function for more details
        * For BINARY_DOUBLE and BINARY_FLOAT, refer to the C Standard Library printf() family documentation
        *
        */
        bool SetFormat(FormatType formatType, const ostring& format);

        /**
        * @brief
        * Return the format string for implicit string conversions of the given type
        *
        * @param formatType   - Type of format
        *
        * @note
        *  See SetFormat() for possible values
        *
        */
        ostring GetFormat(FormatType formatType) const;

        /**
        * @brief
        * Return the Localized string for the given LocalCode value
        *
        * @param code - Locale Code
        *
        */
        ostring GetLocaleString(LocaleCode code) const;

        /**
         * @brief
         * Start a database instance
         *
         * @param db           - Oracle Service Name
         * @param user         - Oracle User name
         * @param pwd          - Oracle User password
         * @param startFlags   - Start flags
         * @param startMode    - Start mode
         * @param sessionFlags - Session Flags
         * @param spfile       - Client-side spfile to start up the database (optional)
         *
         * @note
         * The only valid modes for sessionFlags are SessionSysDba and SessionSysOper.
         *
         * @note
         * Start modes and flags parameters values can be combined.
         *
         * @note
         * External credentials are supported by supplying a empty strings for the 'user' and 'pwd' parameters
         * If the parameter 'db' is empty then a connection to the default local DB is done.
         *
         * @note
         * If the client side spfile is not provided, the database is started with its server-side spfile.
         *
         */
        void StartDatabase(const ostring& db, const ostring& user, const ostring& pwd,
            Environment::StartFlags startFlags,
            Environment::StartMode startMode,
            Environment::SessionFlags sessionFlags = SessionSysDba,
            const ostring& spfile = OTEXT(""));

        /**
         * @brief
         * Shutdown a database instance
         *
         * @param db            - Oracle Service Name
         * @param user          - Oracle User name
         * @param pwd           - Oracle User password
         * @param shutdownFlags - Shutdown flags
         * @param shutdownMode  - Shutdown mode
         * @param sessionFlags  - Session flags
         *
         * @note
         * The only valid modes for sessionFlags are SessionSysDba and SessionSysOper.
         *
         * @note
         * Shutdown modes values can be combined.
         *
         * @note
         * Shutdown flag values are exclusive.
         *
         * @note
         * External credentials are supported by supplying a empty strings for the 'user' and 'pwd' parameters
         * If the param 'db' is empty then a connection to the default local DB is done.
         *
         */
        void ShutdownDatabase(const ostring& db, const ostring& user, const ostring& pwd,
            Environment::ShutdownFlags shutdownFlags,
            Environment::ShutdownMode shutdownMode,
            Environment::SessionFlags sessionFlags = SessionSysDba);

        /**
         * @brief
         * Change the password of the given user on the given database
         *
         * @param db      - Oracle Service Name
         * @param user    - Oracle User name
         * @param pwd     - Oracle User password
         * @param newPwd  - Oracle User New password
         *
         */
        void ChangeUserPassword(const ostring& db, const ostring& user, const ostring& pwd, const ostring& newPwd);

        /**
         * @brief
         * Set the High availability (HA) user handler
         *
         * @param handler - HA handler procedure
         *
         * @note
         * See HAHandlerProc documentation for more details
         *
         * @note
         * EnvMode::Events flag must be passed to the Environment constructor to be able to use HA events
         *
         * @warning
         * This call is supported from Oracle 10gR2.
         *
         */
        void SetHAHandler(HAHandlerProc handler);

        /**
         * @brief
         * Set the port number used for all notifications in this environment
         *
         * @param port - Port number to use (0 for automatic port assignment)
         *
         * @note
         * This call must be made before the first subscription is registered.
         * All subscriptions within the same environment share the same port.
         *
         * @warning
         * This call is supported from Oracle 10gR2.
         *
         */
        void SetSubscriptionPort(UInt32 port);

        /**
         * @brief
         * Return the port number used for notifications in this environment
         *
         * @return
         * The port number, or 0 on failure.
         *
         */
        UInt32 GetSubscriptionPort() const;

    private:

        class EnvironmentHandle : core::HandleHolder<OCI_Environment*>
        {
            friend class Environment;
        };

        static void HAHandler(OCI_Connection* pConnection, UInt32 source, UInt32 event, OCI_Timestamp* pTimestamp);
        static UInt32 TAFHandler(OCI_Connection* pConnection, UInt32 type, UInt32 event);
        static void NotifyHandler(OCI_Event* pEvent);
        static void NotifyHandlerAQ(OCI_Dequeue* pDequeue);

        static Environment* GetInstance(OCI_Environment* env);

        class CallbackBase
        {
        public:
            virtual ~CallbackBase() = default;
        };

        template<class Sig>
        class Callback : public CallbackBase
        {
        public:
            std::function<Sig> func;
        };

        template<class T>
        std::function<typename std::remove_pointer<T>::type> GetUserCallback(AnyPointer ptr);

        template<class T>
        void SetUserCallback(AnyPointer ptr, T callback);

    public:

        OCI_Environment* GetHandle() const;

    private:

        core::Handle* GetEnvironmentHandle() const;

        std::mutex _callbacksMutex;
        std::unordered_map<AnyPointer, std::unique_ptr<CallbackBase>> _callbacks;
        EnvironmentHandle _handle;
        EnvironmentFlags _mode;
        UInt32 _charMaxSize;
        bool _initialized;
        std::shared_ptr<core::HandleStore> _store;
    };

    /**
    /**
      * @brief
      * A connection or session Pool.
      *
      * This class wraps the OCILIB object handle OCI_Pool and its related methods
      *
      */
    class Pool : public core::HandleHolder<OCI_Pool*>
    {
    public:

        /**
         * @brief
         * Pool type enumerated values
         *
         */
        enum PoolTypeValues
        {
            /** Pool of Connections */
            ConnectionPool = OCI_POOL_CONNECTION,
            /** Pool of stateless sessions */
            SessionPool = OCI_POOL_SESSION
        };

        /**
        * @brief
        * Type of Pool
        *
        * Possible values are Pool::PoolTypeValues
        *
        */
        typedef core::Enum<PoolTypeValues> PoolType;

        /**
         * @brief
         * Default constructor
         *
         */
        Pool();

        /**
         * @brief
         * Constructor that creates an underlying pool with the given information
         *
         * @param db           - Oracle Service Name
         * @param user         - Oracle User name
         * @param pwd          - Oracle User password
         * @param poolType     - Type of pool
         * @param sessionFlags - session Flags
         * @param minSize      - minimum number of  connections/sessions that can be opened.
         * @param maxSize      - maximum number of  connections/sessions that can be opened.
         * @param increment    - next increment for connections/sessions to be opened
         *
         *  @note
         * it calls Open() with the given parameters
         *
         */
        Pool(const Environment& env, const ostring& db, const ostring& user, const ostring& pwd, Pool::PoolType poolType,
            UInt32 minSize, UInt32 maxSize, UInt32 increment = 1,
            Environment::SessionFlags sessionFlags = Environment::SessionDefault);

        /**
         * @brief
         * Create an Oracle pool of connections or sessions
         *
         * @param db           - Oracle Service Name
         * @param user         - Oracle User name
         * @param pwd          - Oracle User password
         * @param poolType     - Type of pool
         * @param sessionFlags - session Flags
         * @param minSize      - minimum number of  connections/sessions that can be opened.
         * @param maxSize      - maximum number of  connections/sessions that can be opened.
         * @param increment    - next increment for connections/sessions to be opened
         *
         * @note
         * External credentials are supported by supplying an empty string for the 'user' and 'pwd' parameters
         * If the param 'db' is empty then a connection to the default local DB is done
         *
         */
        void Open(const Environment& env, const ostring& db, const ostring& user, const ostring& pwd, Pool::PoolType poolType,
            UInt32 minSize, UInt32 maxSize, UInt32 increment = 1,
            Environment::SessionFlags sessionFlags = Environment::SessionDefault);

        /**
         * @brief
         * Destroy the current Oracle pool of connections or sessions
         *
         */
        void Close();

        /**
         * @brief
         * Get a connection from the pool
         *
         * @param sessionTag  - Session user tag string
         *
         * @par Session tagging
         *
         * Session pools have a nice feature called 'session tagging'
         * It's possible to tag a session with a string identifier
         * when the session is returned to the pool, it keeps its tags.
         * When requesting a connection from the session pool, it's
         * possible to request a session that has the given 'tag' parameter
         * If one exists, it is returned. If not and if an untagged session
         * is available, it is then returned. So check the connection tag
         * property with GetSessionTag() to find out if the returned
         * connection is tagged or not.
         *
         * This features is described in the OCI developer guide as the following :
         *
         *  "The tags provide a way for users to customize sessions in the pool.
         *   A client may get a default or untagged session from a pool, set certain
         *   attributes on the session (such as NLS settings), and return the session
         *   to the pool, labeling it with an appropriate tag.
         *   The user may request a session with the same tags in order to have a
         *   session with the same attributes"
         *
         */
        Connection GetConnection(const ostring& sessionTag = OTEXT(""));

        /**
         * @brief
         * Get the idle timeout for connections/sessions in the pool
         *
         * @note
         * Connections/sessions idled for more than this time value (in seconds) are terminated
         *
         * @note
         * Timeout is not available for internal pooling implementation (client < 9i)
         *
         */
        UInt32 GetTimeout() const;

        /**
         * @brief
         * Set the connections/sessions idle timeout
         *
         * @param value - Timeout value
         *
         * @note
         * connections/sessions idle for more than this time value (in seconds) are terminated
         *
         * @note
         * This call has no effect if pooling is internally implemented (client < 9i)
         *
         */
        void SetTimeout(UInt32 value);

        /**
         * @brief
         * Get the waiting mode used when no more connections/sessions are available from the pool
         *
         * @return
         * - true to wait for an available object if the pool is saturated
         * - false to not wait for an available object
         *
         */
        bool GetNoWait() const;

        /**
         * @brief
         * Set the waiting mode used when no more connections/sessions are available from the pool
         *
         * @param value - wait mode for object
         *
         * @note
         * For parameter value, pass :
         * - false to wait for an available object if the pool is saturated
         * - true to not wait for an available object
         *
         * @note
         * Default pool behavior is to wait for available connections
         *
         */
        void SetNoWait(bool value);

        /**
         * @brief
         * Return the current number of busy connections/sessions
         *
         */
        UInt32 GetBusyConnectionsCount() const;

        /**
         * @brief
         * Return the current number of opened connections/sessions
         *
         */
        UInt32 GetOpenedConnectionsCount() const;

        /**
         * @brief
         * Return the minimum number of connections/sessions that can be opened to the database
         *
         */
        UInt32 GetMinSize() const;

        /**
         * @brief
         * Return the maximum number of connections/sessions that can be opened to the database
         *
         */
        UInt32 GetMaxSize() const;

        /**
         * @brief
         * Return the increment for connections/sessions to be opened to the database when the pool is not full
         *
         */
        UInt32 GetIncrement() const;

        /**
         * @brief
         * Return the maximum number of statements to keep in the pool's statement cache
         *
         * @note
         * Default value is 20 (value from Oracle Documentation)
         *
         */
        UInt32 GetStatementCacheSize() const;

        /**
         * @brief
         * Set the maximum number of statements to keep in the pool's statement cache
         *
         * @param value - maximum number of statements in the cache
         *
         */
        void SetStatementCacheSize(UInt32 value);
    };

    /**
     * @brief
     * A connection or session with a specific database.
     *
     * This class wraps the OCILIB object handle OCI_Connection and its related methods
     *
     */
    class Connection : public core::HandleHolder<OCI_Connection*>
    {
        friend class Environment;
        friend class Exception;
        friend class Statement;
        friend class File;
        friend class Timestamp;
        friend class Pool;
        friend class Object;
        friend class TypeInfo;
        friend class Reference;
        friend class Resultset;
        friend class Subscription;
        friend class Agent;
        friend class Dequeue;
        friend class Enqueue;
        friend class Column;
        friend class Message;
        friend class Transaction;
        friend class DirectPath;
        friend class Vector;
        friend class SodaDb;
        friend class SodaColl;

        template<class, Int32>
        friend class Lob;
        template<class>
        friend class Collection;
    public:

        /**
        * @brief
        * Timeout enumerated values
        *
        */
        enum TimeoutTypeValues
        {
            /**
             *   - Time to wait for send operations completion to the database server
             *   - Requires Oracle 12cR1 client
             */
            TimeoutSend = OCI_NTO_SEND,
            /**
             *   - Time to wait for read operations completion from the database server
             *   - Requires Oracle 12cR1 client
             */
            TimeoutReceive = OCI_NTO_RECEIVE,
            /**
             *   - Time to wait for a database round-trip to complete ( Client processing is not taken into account)
             *   - Requires Oracle 18c client
             */
            TimeoutCall = OCI_NTO_CALL
        };

        /**
        * @brief
        * Timeout Types
        *
        * Possible values are Connection::TimeoutValues
        *
        */
        typedef core::Enum<TimeoutTypeValues> TimeoutType;

        /**
        * @brief
        * Failover request enumerated values
        *
        */
        enum FailoverRequestValues
        {
            /** User has requested only session failover */
            FailoverRequestSession = OCI_FOT_SESSION,
            /** User has requested select failover as well */
            FailoverRequestSelect = OCI_FOT_SELECT
        };

        /**
        * @brief
        * Failover requests
        *
        * Possible values are Connection::FailoverRequestValues
        *
        */
        typedef core::Enum<FailoverRequestValues> FailoverRequest;

        /**
        * @brief
        * Failover events enumerated values
        *
        */
        enum FailoverEventValues
        {
            /** Successful completion of failover */
            FailoverEventEnd = OCI_FOE_END,
            /** Failover was unsuccessful. Retrying is not allowed */
            FailoverEventAbort = OCI_FOE_ABORT,
            /** Multiple authentication and failover has occurred after the original authentication. User has been re authenticated */
            FailoverEventReauthentificate = OCI_FOE_REAUTH,
            /** Failover has detected a lost connection and failover is starting */
            FailoverEventBegin = OCI_FOE_BEGIN,
            /** Failover was unsuccessful. Retrying is allowed */
            FailoverEventError = OCI_FOE_ERROR
        };

        /**
        * @brief
        * Failover events
        *
        * Possible values are Connection::FailoverEventValues
        *
        */
        typedef core::Enum<FailoverEventValues> FailoverEvent;

        /**
        * @brief
        * Failover callback results enumerated values
        *
        */
        enum FailoverResultValues
        {
            /** Default acknowledgment of a failover event */
            FailoverOk = OCI_FOC_OK,
            /** Triggers a new failover attempt */
            FailoverRetry = OCI_FOC_RETRY
        };

        /**
        * @brief
        * Failover callback results
        *
        * Possible values are Connection::FailoverResultValues
        *
        */
        typedef core::Enum<FailoverResultValues> FailoverResult;

        /**
         * @brief
         * Session trace enumerated values
         *
         */
        enum SessionTraceValues
        {
            /** Specifies the user defined identifier in the session. It's recorded in the column CLIENT_IDENTIFIER of the system view V$SESSION */
            TraceIdentity = OCI_TRC_IDENTITY,
            /** Name of the current module in the client application. It's recorded in the column MODULE of the system view V$SESSION */
            TraceModule = OCI_TRC_MODULE,
            /** Name of the current action within the current module. It's recorded in the column ACTION of the system view V$SESSION */
            TraceAction = OCI_TRC_ACTION,
            /** Client application additional information. It's recorded in the column CLIENT_INFO of the system view V$SESSION */
            TraceDetail = OCI_TRC_DETAIL
        };

        /**
        * @brief
        * Session traces
        *
        * Possible values are Connection::SessionTraceValues
        *
        */
        typedef core::Enum<SessionTraceValues> SessionTrace;

        /**
        * @typedef TAFHandlerProc
        *
        * @brief
        * User callback for TAF event notifications
        *
        */
        typedef FailoverResult(*TAFHandlerProc) (Connection& con, FailoverRequest failoverRequest, FailoverEvent failoverEvent);

        /**
         * @brief
         * Default constructor
         *
         */
        Connection();

        /**
         * @brief
         * Constructor that creates an opens an underlying DB connection with the given information
         *
         * @param db           - Oracle Service Name
         * @param user         - Oracle User name
         * @param pwd          - Oracle User password
         * @param sessionFlags - Session Flags
         *
         *  @note
         * it calls Open() with the given parameters
         *
         */
        Connection(const Environment& env, const ostring& db, const ostring& user, const ostring& pwd, Environment::SessionFlags sessionFlags = Environment::SessionDefault);

        /**
         * @brief
         * Create a physical connection to an Oracle database server
         *
         * @param db           - Oracle Service Name
         * @param user         - Oracle User name
         * @param pwd          - Oracle User password
         * @param sessionFlags - Session Flags
         * *
         * @note
         * External credentials are supported by supplying an empty string for the 'user' and 'pwd' parameters
         * If the param 'db' is empty then a connection to the default local DB is done
         *
         * @note
         * On success, a local transaction is automatically created and started ONLY for regular
         * standalone connections and connections retrieved from connection pools.
         * No transaction is created for connections retrieved from session pools.
         *
         */
        void Open(const Environment& env, const ostring& db, const ostring& user, const ostring& pwd, Environment::SessionFlags sessionFlags = Environment::SessionDefault);

        /**
         * @brief
         * Close the physical connection to the DB server
         *
         */
        void Close();

        /**
         * @brief
         * Commit current pending changes
         *
         */
        void Commit();

        /**
          * @brief
          * Cancel current pending changes
          *
          */
        void Rollback();

        /**
         * @brief
         * Perform an immediate abort of any currently Oracle OCI call on the given connection
         *
         * @note
         * The current call will abort and will raise an exception
         *
         */
        void Break();

        /**
         * @brief
         * Enable or disable auto commit mode (implicit commits after every SQL execution)
         *
         * @param enabled - auto commit new status
         *
         */
        void SetAutoCommit(bool enabled);

        /**
         * @brief
         * Indicates if auto commit is currently activated
         *
         */
        bool GetAutoCommit() const;

        /**
        * @brief
        * Indicate if the connection is still connected to the server
        *
        * @note
        * the returned value is not real-time and is  based on client library last heart beat status
        *
        */
        bool IsServerAlive() const;

        /**
         * @brief
         * Performs a round trip call to the server to confirm that the connection to the server is still valid.
         *
         * @warning
         * This call is supported from Oracle 10g.
         *
         */
        bool PingServer() const;

        /**
         * @brief
         * Return the name of the connected database/service name
         *
         * @note
         * The returned value is the value of the 'db' parameter of the Open() method
         *
         */
        ostring GetConnectionString() const;

        /**
         * @brief
         * Return the current logged user name
         *
         * @note
         * The returned value is the value of the 'user' parameter of the Open() method
         *
         */
        ostring GetUserName() const;

        /**
         * @brief
         * Return the current logged user password
         *
         * @note
         * The returned value is the value of the 'pwd' parameter of the Open() method
         *
         */
        ostring GetPassword() const;

        /**
         * @brief
         * Return the connected database server string version
         *
         * @note
         * The returned value is the server version banner displayed by SQL*PLUS when connected to a DB server
         *
         */
        ostring GetServerVersion() const;

        /**
         * @brief
         * Return the Oracle version supported by the connection
         *
         * @note
         * The supported version is the lower version between client and server:
         *
         */
        OracleVersion GetVersion() const;

        /**
         * @brief
         * Return the major version number of the connected database server
         *
         */
        UInt32 GetServerMajorVersion() const;

        /**
         * @brief
         * Return the minor version number of the connected database server
         *
         */
        UInt32 GetServerMinorVersion() const;

        /**
         * @brief
         * Return the revision version number of the connected database server
         *
         */
        UInt32 GetServerRevisionVersion() const;

        /**
         * @brief
         * Change the password of the logged user
         *
         * @param newPwd - New password
         *
         */
        void ChangePassword(const ostring& newPwd);

        /**
         * @brief
         * Return the tag associated with the given connection
         *
         */
        ostring GetSessionTag() const;

        /**
         * @brief
         * Associate a tag to the given connection/session
         *
         * @param tag - user tag string
         *
         * @note
         * Use this call only for connections retrieved from a session pool
         * See Pool::GetConnection() for more details
         *
         * @note
         * To untag a session, call SetSessionTag() with an empty 'tag' parameter
         *
         */
        void SetSessionTag(const ostring& tag);

        /**
         * @brief
         * Return the current transaction of the connection
         *
         */
        Transaction GetTransaction() const;

        /**
         * @brief
         * Set a transaction to a connection
         *
         * @param transaction - Transaction to assign
         *
         * @note
         * The current transaction (if any) is automatically stopped but the newly assigned is not started or resumed
         *
         * @warning
         * Do not set a transaction to a connection retrieved from a session pool
         *
         */
        void SetTransaction(const Transaction& transaction);

        /**
         * @brief
         * Enable the server output
         *
         * @param bufsize - server buffer max size (server side)
         * @param arrsize - number of lines to retrieve per server round-trip
         * @param lnsize  - maximum size of one line
         *
         * @note
         * This call is equivalent to the command 'set serveroutput on' in SQL*PLUS
         *
         * @note
         *  - 'bufsize' minimum value is 2000, maximum 1000000 with Oracle < 10.2g and can be unlimited above
         *  - 'lnsize' maximum value is 255 with Oracle < 10g R2 and 32767 above
         *
         * @warning
         * If EnableServerOutput() is not called, GetServerOutput() will return false
         *
         */
        void EnableServerOutput(UInt32 bufsize, UInt32 arrsize, UInt32 lnsize);

        /**
         * @brief
         * Disable the server output
         *
         * @note
         * After this call, GetServerOutput() will return false.
         *
         */
        void DisableServerOutput();

        /**
         * @brief
         * Retrieve one line of the server buffer
         *
         * @return
         * true if a line has been retrieved otherwise false (server buffer is empty or all lines have been retrieved)
         *
         */
        bool GetServerOutput(ostring& line) const;

        /**
         * @brief
         * Retrieve all remaining lines of the server buffer
         *
         */
        void GetServerOutput(std::vector<ostring>& lines) const;

        /**
         * @brief
         * Set tracing information for the session
         *
         * @param trace - trace type
         * @param value - trace content
         *
         * Store current trace information to the given connection handle.
         * These information:
         *
         * - is stored in the system view V$SESSION
         * - can be retrieved from the connection property of an Error object
         *
         * system view V$SESSION
         *
         * @warning
         * The system view V$SESSION is updated on Oracle versions >= 10g
         *
         * @warning
         * Oracle limits the size of these traces content and thus OCILIB will truncate
         * the given values if needed :
         *
         * - TraceIdentity : 64 bytes
         * - TraceModule   : 48 bytes
         * - TraceAction   : 32 bytes
         * - TraceDetail   : 64 bytes
         *
         */
        void SetTrace(SessionTrace trace, const ostring& value);

        /**
         * @brief
         * Get the current trace for the trace type from the given connection.
         *
         * @param trace - trace type
         *
         * @note
         * See SetTrace() for more details.
         *
         */
        ostring GetTrace(SessionTrace trace) const;

        /**
         * @brief
         * Return the Oracle server database name of the connected database/service name
         *
         * @warning
         * This call is supported from Oracle 10gR2.
         *
         */
        ostring GetDatabase() const;

        /**
         * @brief
         * Return the Oracle server Instance name of the connected database/service name
         *
         * @warning
         * This call is supported from Oracle 10gR2.
         *
         */
        ostring GetInstance() const;

        /**
         * @brief
         * Return the Oracle server Service name of the connected database/service name
         *
         * @warning
         * This call is supported from Oracle 10gR2.
         *
         */
        ostring GetService() const;

        /**
         * @brief
         * Return the Oracle server Hos name of the connected database/service name
         *
         * @warning
         * This call is supported from Oracle 10gR2.
         *
         */
        ostring GetServer() const;

        /**
         * @brief
         * Return the Oracle server Domain name of the connected database/service name
         *
         * @warning
         * This call is supported from Oracle 10gR2.
         *
         */
        ostring GetDomain() const;

        /**
         * @brief
         * Return the date and time (Timestamp) server instance start of the
         *
         * @warning
         * This call is supported from Oracle 10gR2.
         *
         */
        Timestamp GetInstanceStartTime() const;

        /**
         * @brief
         * Return the maximum number of statements to keep in the statement cache
         *
         * @note
         * Default value is 20 (value from Oracle Documentation)
         *
         * @warning
         * Requires Oracle Client 9.2 or above
         *
         */
        UInt32 GetStatementCacheSize() const;

        /**
         * @brief
         * Set the maximum number of statements to keep in the statement cache
         *
         * @param value - maximum number of statements in the cache
         *
         * @warning
         * Requires Oracle Client 9.2 or above
         *
         */
        void SetStatementCacheSize(UInt32 value);

        /**
         * @brief
         * Return the default LOB prefetch buffer size for the connection
         *
         * @warning
         * Requires Oracle Client AND Server 11gR1 or above
         *
         * @note
         * Prefetch size is:
         * - number of bytes for BLOBs (Blob) and BFILEs (File)
         * - number of characters for CLOBs (Clob / NClob).
         *
         * @note
         * Default is 0 (pre-fetching disabled)
         *
         */
        UInt32 GetDefaultLobPrefetchSize() const;

        /**
         * @brief
         * Enable or disable pre-fetching for all LOBs fetched in the connection
         *
         * @param value - default prefetch buffer size
         *
         * @note
         * If parameter 'value':
         * - is == 0, it disables pre-fetching for all LOBs fetched in the connection.
         * - is >  0, it enables pre-fetching for all LOBs fetched in the connection
         * and the given buffer size is used for pre-fetching LOBs
         *
         * @note
         * LOBs pre-fetching is disabled by default
         *
         * @warning
         * Requires Oracle Client AND Server 11gR1 or above.
         *
         * @note
         * Prefetch size is:
         * - number of bytes for BLOBs (Blob) and BFILEs (File)
         * - number of characters for CLOBs (Clob / NClob).
         *
         */
        void SetDefaultLobPrefetchSize(UInt32 value);

        /**
        * @brief
        * Return the maximum number of SQL statements that can be opened in one session
        *
        * @warning
        * Requires Oracle Client AND Server 12cR1 or above
        *
        * @note
        * the returned value is the same as the db parameter 'open_cursors' from server's parameter file
        *
        * @note
        * Return 0 if the client and server version are < 12cR1
        *
        */
        UInt32 GetMaxCursors() const;

        /**
         * @brief
         * Verify if the connection support TAF events
         *
         * @warning
         * This call is supported from Oracle 10gR2.
         *
         */
        bool IsTAFCapable() const;

        /**
         * @brief
         * Set the Transparent Application Failover (TAF) user handler
         *
         * @param handler - TAF handler procedure
         *
         * @note
         * See TAFHandlerProc documentation for more details
         *
         * @warning
         * This call is supported from Oracle 10gR2.
         *
         */
        void SetTAFHandler(TAFHandlerProc handler);

        /**
         * @brief
         * Return the pointer to user data previously associated with the connection
         *
         */
        AnyPointer GetUserData();

        /**
         * @brief
         * Associate a pointer to user data to the given connection
         *
         * @param value - User data pointer
         *
         */
        void SetUserData(AnyPointer value);

        /**
         * @brief
         * Returns the requested timeout value for OCI calls that require server round-trips to the given database
         *
         * @param timeout  - Type of timeout
         *
         * @note:
         * See Connection::SetTimeout() for more information
         *
         * @return
         * The given timeout value if supported, otherwise 0
         *
         */
        UInt32 GetTimeout(TimeoutType timeout);

        /**
         * @brief
         * Set a given timeout for OCI calls that require server round-trips to the given database
         *
         * @param timeout - Type of timeout to set
         * @param value   - Timeout in milliseconds
         *
         * OCI client raises an timeout type related error when a given timeout is reached.
         *
         * @note
         * To disable a given timeout, pass the value 0
         *
         * @warning
         * OCI client is using the following precedence rules when applying timeouts:
         *   - 1 - Timeout set using TimeoutCall (all other timeouts are discarded)
         *   - 2 - Timeouts set using TimeoutSend and/or TimeoutReceive
         *   - 3 - Timeouts set in sqlnet.ora file
         *
         * Here is a summary:
         *
         * FLAG            | Min. Version | OCI Error raised | OCI Error description          | sqlnet.ora equivalent |
         * --------------- | ------------ | ---------------- | ------------------------------ | --------------------- |
         * TimeoutSend     | OCI_12_1     | ORA-12608        | TNS: Send timeout occurred     | SQLNET.SEND_TIMEOUT   |
         * TimeoutReceive  | OCI_12_1     | ORA-12609        | TNS: Receive timeout occurred  | SQLNET.RECV_TIMEOUT   |
         * TimeoutCall     | OCI_18_1     | ORA-03136        | inbound connection timed out   |         ---           |
         *
         */
        void SetTimeout(TimeoutType timeout, UInt32 value);

    private:

        Connection(OCI_Connection* con, core::Handle* parent, bool allocated);
    };

    /**
     * @brief
     * Oracle Transaction object
     *
     * This class wraps the OCILIB object handle OCI_Transaction and its related methods
     *
     */
    class Transaction : public core::HandleHolder<OCI_Transaction*>
    {
        friend class Connection;

    public:

        /**
         * @brief
         * Transaction flags enumerated values
         *
         */
        enum TransactionFlagsValues
        {
            Unknown = OCI_UNKNOWN,
            /** (Global) Specifies tightly coupled and migratable branch */
            New = OCI_TRS_NEW,
            /** (Global) Specifies a tightly coupled branch */
            Tight = OCI_TRS_TIGHT,
            /** (Global) Specifies a loosely coupled branch */
            Loose = OCI_TRS_LOOSE,
            /** (Global and local) start a read-only transaction */
            ReadOnly = OCI_TRS_READONLY,
            /** (Global and local) start a read-write transaction */
            ReadWrite = OCI_TRS_READWRITE,
            /** (Global and local) start a serializable transaction */
            Serializable = OCI_TRS_SERIALIZABLE
        };

        /**
        * @brief
        * Transaction flags
        *
        * Possible values are Transaction::TransactionFlagsValues
        *
        */
        typedef core::Flags<TransactionFlagsValues> TransactionFlags;

        /**
         * @brief
         * Create a new global transaction or a serializable/read-only local transaction
         *
         * @param connection - Connection
         * @param timeout    - Time that a transaction stays inactive after being stopped
         * @param flags      - Transaction flags
         * @param pxid       - pointer to a global transaction identifier structure
         *
         * @note
         * For local transaction,  don't use the 'pxid' parameter
         *
         */
        Transaction(const Connection& connection, UInt32 timeout, TransactionFlags flags, OCI_XID* pxid = nullptr);

        /**
          * @brief
          * Prepare a global transaction validation
          *
          */
        void Prepare();

        /**
          * @brief
          * Start global transaction
          *
          */
        void Start();

        /**
         * @brief
         * Stop current global transaction
         *
         */
        void Stop();

        /**
         * @brief
         * Resume a stopped global transaction
         *
         */
        void Resume();

        /**
         * @brief
         * Cancel the prepared global transaction validation
         *
         */
        void Forget();

        /**
         * @brief
         * Return the transaction mode.
         *
         * @note:
         * see Transaction() for possible values
         *
         */
        TransactionFlags GetFlags() const;

        /**
          * @brief
          * Return the transaction Timeout
          *
          */
        UInt32 GetTimeout() const;

    private:

        Transaction(OCI_Transaction* trans);
    };

    /**
    * @brief
    * Object identifying the SQL data type NUMBER.
    *
    * This class wraps the OCILIB object handle OCI_Number and its related methods
    *
    * NUMBERs can be handled using native scalar types
    * This class is optional and exposes some special NUMBER properties
    *
    */
    class Number : public core::HandleHolder<OCI_Number*>, public core::Streamable
    {
        friend class Statement;
        friend class Resultset;
        friend class support::BindArray;
        friend class JsonNode;
        friend class Object;
        template<class>
        friend class Collection;

    public:

        /**
        * @brief
        * Create an empty null number object
        *
        * @param create - Indicates if a Oracle Number object must be must be created
        *
        */
        Number(const Environment& env, bool create = false);

        /**
        * @brief
        * Create an empty null number object (internal usage)
        *
        */
        Number();

        /**
        * @brief
        * Create a number object initialized with given numerical value
        *
        * @param value - value to assign
        *
        */
        template<class T, core::SupportedNumericT<T>* = nullptr>
        Number(const Environment& env, const T& value);


        /**
        * @brief
        * For value types, overrides HandleHolder copy constructor to perform assignment rather
        * than ref counting management
        *
        * @param other - value to assign
        *
        */
        Number(const Number& other);

        /**
         * @brief
         * For value types, overrides HandleHolder assignment operator to perform assignment rather
         * than ref counting management
         *
         * @param other - value to assign
         *
         */
        Number& operator= (const Number& other);

        /**
        * @brief
        * Create a Number object with the value provided by the input Number string
        *
        * @param str    - String Number
        * @param format - format of the Number provided in parameter 'data'
        *
        * @note
        * For Number formats, refer to the Oracle SQL documentation
        *
        * @note
        * Default connection Number format is computed from Environment::GetFormat()
        *
        */
        Number(const Environment& env, const otext* str, const otext* format = OTEXT(""));

        /**
        * @brief
        * Create a Number object with the value provided by the input Number string
        *
        * @param str    - String Number
        * @param format - format of the Number provided in parameter 'data'
        *
        * @note
        * For Number formats, refer to the Oracle SQL documentation
        *
        * @note
        * Default connection Number format is computed from Environment::GetFormat()
        *
        */
        Number(const Environment& env, const ostring& str, const ostring& format = OTEXT(""));

        /**
        * @brief
        * Assign to the number object the value provided by the input number time string
        *
        * @param str    - String number
        * @param format - format of the number provided in parameter 'data'
        *
        * @note
        * For number formats, refer to the Oracle SQL documentation
        *
        * @note
        * Default connection number format is computed from Environment::GetFormat()
        *
        */
        void FromString(const ostring& str, const ostring& format = OTEXT("")) const;

        /**
        * @brief
        * Convert the number value to a string using the given format
        *
        * @param format - number time format to use
        *
        * @note
        * For number formats, refer to the Oracle SQL documentation
        *
        */
        ostring ToString(const ostring& format) const;

        /**
        * @brief
        * Convert the number value to a string using default format OCI_STRING_FORMAT_NUMERIC
        *
        * @note
        * For number formats, refer to the Oracle SQL documentation
        *
        */
        ostring ToString() const override;

        /**
        * @brief
        * Clone the current instance to a new one performing deep copy
        *
        */
        Number Clone() const;

        Number& operator ++ ();
        Number& operator -- ();
        Number operator ++ (Int32);
        Number operator -- (Int32);

        bool operator == (const Number& other) const;
        bool operator != (const Number& other) const;
        bool operator > (const Number& other) const;
        bool operator < (const Number& other) const;
        bool operator >= (const Number& other) const;
        bool operator <= (const Number& other) const;

        template<class T, core::SupportedNumericT<T>* = nullptr>
        Number& operator = (const T& lhs);

        template<class T, core::SupportedNumericT<T>* = nullptr>
        operator T() const;

        template<class T, core::SupportedNumericT<T>* = nullptr>
        Number operator - (const T& value);

        template<class T, core::SupportedNumericT<T>* = nullptr>
        Number operator + (const T& value);

        template<class T, core::SupportedNumericT<T>* = nullptr>
        Number operator * (const T& value);

        template<class T, core::SupportedNumericT<T>* = nullptr>
        Number operator / (const T& value);

        template<class T, core::SupportedNumericT<T>* = nullptr>
        Number& operator += (const T& value);

        template<class T, core::SupportedNumericT<T>* = nullptr>
        Number& operator -= (const T& value);

        template<class T, core::SupportedNumericT<T>* = nullptr>
        Number& operator *= (const T& value);

        template<class T, core::SupportedNumericT<T>* = nullptr>
        Number& operator /= (const T& value);

    private:

        template<class T>
        static AnyPointer GetNativeValue(const T& value);

        Number(OCI_Number* pNumber, core::Handle* parent = nullptr);

        void Allocate(OCI_Environment* env, core::Handle* parent);

        Int32 Compare(const Number& other) const;

        template<class T>
        inline T GetValue() const;

        template<class T>
        Number& SetValue(const T& value);

        template<class T>
        void Add(const T& value);

        template<class T>
        void Sub(const T& value);

        template<class T>
        void Multiply(const T& value);

        template<class T>
        void Divide(const T& value);

        Number& operator = (OCI_Number*& lhs);
    };

    /**
     * @brief
     * Object identifying the SQL data type DATE.
     *
     * This class wraps the OCILIB object handle OCI_Date and its related methods
     *
     */
    class Date : public core::HandleHolder<OCI_Date*>, public core::Streamable
    {
        friend class Statement;
        friend class Resultset;
        friend class support::BindArray;
        friend class JsonNode;
        friend class Object;
        template<class>
        friend class Collection;
        friend class Message;

    public:

        /**
        * @brief
        * Return the current system date time
        *
        */
        static Date SysDate(const Environment& env);

        Date();

        /**
         * @brief
         * Create an empty null Date object
         *
         * @param create - Indicates if a Oracle Date object must be must be created
         *
         */
        Date(const Environment& env, bool create = false);

        /**
         * @brief
         * For value types, overrides HandleHolder copy constructor to perform assignment rather
         * than ref counting management
         *
         * @param other - value to assign
         *
         */
        Date(const Date& other);

        /**
        * @brief
        * For value types, overrides HandleHolder assignment operator to perform assignment rather
        * than ref counting management
        *
        * @param other - value to assign
        *
        */
        Date& operator= (const Date& other);

        /**
        * @brief
        * Create a date object with the value provided by the input date time string
        *
        * @param str    - String date time
        * @param format - format of the date time provided in parameter 'data'
        *
        * @note
        * For date time formats, refer to the Oracle SQL documentation
        *
        * @note
        * Default connection date format is computed from Environment::GetFormat()
        *
        */
        Date(const Environment& env, const otext* str, const otext* format = OTEXT(""));

        /**
        * @brief
        * Create a date object with the value provided by the input date time string
        *
        * @param str    - String date time
        * @param format - format of the date time provided in parameter 'data'
        *
        * @note
        * For date time formats, refer to the Oracle SQL documentation
        *
        * @note
        * Default connection date format is computed from Environment::GetFormat()
        *
        */

        Date(const Environment& env, const ostring& str, const ostring& format = OTEXT(""));

        /**
         * @brief
         * Check if the given date is valid
         *
         */
        bool IsValid() const;

        /**
         * @brief
         * Return the date year value
         *
         */
        Int32 GetYear() const;

        /**
         * @brief
         * Set the date year value
         *
         */
        void SetYear(Int32 value);

        /**
         * @brief
         * Return the date month value
         *
         */
        Int32 GetMonth() const;

        /**
         * @brief
         * Set the date month value
         *
         */
        void SetMonth(Int32 value);

        /**
         * @brief
         * Return the date day value
         *
         */
        Int32 GetDay() const;

        /**
         * @brief
         * Set the date day value
         *
         */
        void SetDay(Int32 value);

        /**
         * @brief
         * Return the date hours value
         *
         */
        Int32 GetHours() const;

        /**
         * @brief
         * Set the date hours value
         *
         */
        void SetHours(Int32 value);

        /**
         * @brief
         * Return the date minutes value
         *
         */
        Int32 GetMinutes() const;

        /**
         * @brief
         * Set the date minutes value
         *
         */
        void SetMinutes(Int32 value);

        /**
         * @brief
         * Return the date seconds value
         *
         */
        Int32 GetSeconds() const;

        /**
         * @brief
         * Set the date seconds value
         *
         */
        void SetSeconds(Int32 value);

        /**
         * @brief
         * Return the number of days with the given date
         *
         * @param other - date to compare
         *
         */
        Int32 DaysBetween(const Date& other) const;

        /**
         * @brief
         * Set the date part
         *
         * @param year  - Year value
         * @param month - Month value
         * @param day   - Day value
         *
         */
        void SetDate(Int32 year, Int32 month, Int32 day);

        /**
         * @brief
         * Set the time part
         *
         * @param hour  - Hour value
         * @param min   - Minute value
         * @param sec   - Second value
         *
         */
        void SetTime(Int32 hour, Int32 min, Int32 sec);

        /**
         * @brief
         * Set the date and time part
         *
         * @param year  - Year value
         * @param month - Month value
         * @param day   - Day value
         * @param hour  - Hour value
         * @param min   - Minute value
         * @param sec   - Second value
         *
         */
        void SetDateTime(Int32 year, Int32 month, Int32 day, Int32 hour, Int32 min, Int32 sec);

        /**
         * @brief
         * Extract the date parts
         *
         * @param year  - Place holder for year value
         * @param month - Place holder for month value
         * @param day   - Place holder for day value
         *
         */
        void GetDate(Int32& year, Int32& month, Int32& day) const;

        /**
         * @brief
         * Extract time parts
         *
         * @param hour  - Place holder for hour value
         * @param min   - Place holder for minute value
         * @param sec   - Place holder for second value
         *
         */
        void GetTime(Int32& hour, Int32& min, Int32& sec) const;

        /**
         * @brief
         * Extract the date and time parts
         *
         * @param year  - Place holder for year value
         * @param month - Place holder for month value
         * @param day   - Place holder for day value
         * @param hour  - Place holder for hour value
         * @param min   - Place holder for minute value
         * @param sec   - Place holder for second value
         *
         */
        void GetDateTime(Int32& year, Int32& month, Int32& day, Int32& hour, Int32& min, Int32& sec) const;

        /**
         * @brief
         * Add or substract days
         *
         * @param days   - Number of days to add/remove
         *
         */
        void AddDays(Int32 days);

        /**
         * @brief
         * Add or substract months
         *
         * @param months   - Number of months to add/remove
         *
         */
        void AddMonths(Int32 months);

        /**
         * @brief
         *  Return the date of next day of the week, after the current date object
         *
         * @param day - Day of the week
         *
         */
        Date NextDay(const ostring& day) const;

        /**
         * @brief
         * Return the last day of month from the current date object
         *
         */
        Date LastDay() const;

        /**
         * @brief
         * Convert the date from one zone to another zone
         *
         * @param tzSrc - Source zone
         * @param tzDst - Destination zone
         *
         */
        void ChangeTimeZone(const ostring& tzSrc, const ostring& tzDst);

        /**
        * @brief
        * Assign to the date object the value provided by the input date time string
        *
        * @param str    - String date time
        * @param format - format of the date time provided in parameter 'data'
        *
        * @note
        * For date time formats, refer to the Oracle SQL documentation
        *
        * @note
        * Default connection date format is computed from Environment::GetFormat()
        *
        */
        void FromString(const ostring& str, const ostring& format = OTEXT(""));

        /**
        * @brief
        * Convert the date value to a string using the given format
        *
        * @param format - date time format to use
        *
        * @note
        * For date time formats, refer to the Oracle SQL documentation
        *
        */
        ostring ToString(const ostring& format) const;

        /**
        * @brief
        * Convert the date value to a string using default format OCI_STRING_FORMAT_DATE
        *
        * @note
        * For date time formats, refer to the Oracle SQL documentation
        *
        */
        ostring ToString() const override;

        /**
        * @brief
        * Clone the current instance to a new one performing deep copy
        *
        */
        Date Clone() const;

        /**
         * @brief
         * Increment the date by 1 day
         *
         */
        Date& operator ++ ();

        /**
         * @brief
         * Increment the date by 1 day
         *
         */
        Date operator ++ (Int32);

        /**
         * @brief
         * Decrement the date by 1 day
         *
         */
        Date& operator -- ();

        /**
         * @brief
         * Decrement the date by 1 day
         *
         */
        Date operator -- (Int32);

        /**
         * @brief
         * Return a new date holding the current date value incremented by the given number of days
         *
         */
        Date operator + (Int32 value) const;

        /**
         * @brief
         * Return a new date holding the current date value decremented by the given number of days
         *
         */
        Date operator - (Int32 value) const;

        /**
         * @brief
         * Increment the date by the given number of days
         *
         */
        Date& operator += (Int32 value);

        /**
         * @brief
         * Decrement the date by the given number of days
         *
         */
        Date& operator -= (Int32 value);

        /**
        * @brief
        * Indicates if the current date value is equal to the given date value
        *
        */
        bool operator == (const Date& other) const;

        /**
        * @brief
        * Indicates if the current date value is not equal the given date value
        *
        */
        bool operator != (const Date& other) const;

        /**
        * @brief
        * Indicates if the current date value is superior to the given date value
        *
        */
        bool operator > (const Date& other) const;

        /**
        * @brief
        * Indicates if the current date value is inferior to the given date value
        *
        */
        bool operator < (const Date& other) const;

        /**
        * @brief
        * Indicates if the current date value is superior or equal to the given date value
        *
        */
        bool operator >= (const Date& other) const;

        /**
        * @brief
        * Indicates if the current date value is inferior or equal to the given date value
        *
        */
        bool operator <= (const Date& other) const;

    private:

        Int32 Compare(const Date& other) const;

        Date(OCI_Date* pDate, core::Handle* parent = nullptr);

        void Allocate(OCI_Environment* env, core::Handle* parent);
    };

    /**
     * @brief
     * Object identifying the SQL data type INTERVAL.
     *
     * This class wraps the OCILIB object handle OCI_Interval and its related methods
     *
     */
    class Interval : public core::HandleHolder<OCI_Interval*>, public core::Streamable
    {
        friend class Environment;
        friend class Statement;
        friend class Resultset;
        friend class support::BindArray;
        friend class JsonNode;
        friend class Object;
        template<class>
        friend class Collection;

    public:

        /**
        * @brief
        * Interval types enumerated values
        *
        */
        enum IntervalTypeValues
        {
            /** Interval unit range is months -> years */
            YearMonth = OCI_INTERVAL_YM,
            /** Interval unit range is seconds -> days */
            DaySecond = OCI_INTERVAL_DS
        };

        /**
        * @brief
        * Interval types
        *
        * Possible values are Interval::IntervalTypeValues
        *
        */
        typedef core::Enum<IntervalTypeValues> IntervalType;

        /**
        * @brief
        * Create an empty null Interval instance
        *
        */
        Interval();

        /**
        * @brief
        * Create a new instance of the given type
        *
        * @param type - Interval type to create
        *
        */
        Interval(const Environment& env, IntervalType type);
        Interval(OCI_Environment* env, IntervalTypeValues type);

        /**
         * @brief
         * For value types, overrides HandleHolder copy constructor to perform assignment rather
         * than ref counting management
         *
         * @param other - value to assign
         *
         */
        Interval(const Interval& other);

        /**
         * @brief
         * For value types, overrides HandleHolder assignment operator to perform assignment rather
         * than ref counting management
         *
         * @param other - value to assign
         *
         */
        Interval& operator= (const Interval& other);

        /**
        * @brief
        * Create an interval object with the value provided by the input interval string
        *
        * @param type - Interval type to create
        * @param data - String interval
        *
        * @note
        * For interval format, refer to the Oracle SQL documentation
        *
        */
        Interval(const Environment& env, IntervalType type, const ostring& data);

        /**
        * @brief
        * Return the type of the given interval object
        *
        */
        IntervalType GetType() const;

        /**
        * @brief
        * Check if the given interval is valid
        *
        */
        bool IsValid() const;

        /**
        * @brief
        * Return the interval year value
        *
        * @warning
        * this call is only permitted if the current interval type is  Interval::YearMonth
        *
        */
        Int32 GetYear() const;

        /**
        * @brief
        * Set the interval year value
        *
        * @warning
        * this call is only permitted if the current interval type is  Interval::YearMonth
        *
        */
        void SetYear(Int32 value);

        /**
        * @brief
        * Return the interval month value
        *
        * @warning
        * this call is only permitted if the current interval type is  Interval::YearMonth
        *
        */
        Int32 GetMonth() const;

        /**
        * @brief
        * Set the interval month value
        *
        * @warning
        * this call is only permitted if the current interval type is  Interval::YearMonth
        *
        */
        void SetMonth(Int32 value);

        /**
        * @brief
        * Return the interval day value
        *
        * @warning
        * this call is only permitted if the current interval type is  Interval::DaySecond
        *
        */
        Int32 GetDay() const;

        /**
        * @brief
        * Set the interval day value
        *
        * @warning
        * this call is only permitted if the current interval type is  Interval::DaySecond
        *
        */
        void SetDay(Int32 value);

        /**
        * @brief
        * Return the interval hours value
        *
        * @warning
        * this call is only permitted if the current interval type is  Interval::DaySecond
        *
        */
        Int32 GetHours() const;

        /**
        * @brief
        * Set the interval hours value
        *
        * @warning
        * this call is only permitted if the current interval type is  Interval::DaySecond
        *
        */
        void SetHours(Int32 value);

        /**
        * @brief
        * Return the interval minutes value
        *
        * @warning
        * this call is only permitted if the current interval type is  Interval::DaySecond
        *
        */
        Int32 GetMinutes() const;

        /**
        * @brief
        * Set the interval minutes value
        *
        * @warning
        * this call is only permitted if the current interval type is  Interval::DaySecond
        *
        */
        void SetMinutes(Int32 value);

        /**
        * @brief
        * Return the interval seconds value
        *
        * @warning
        * this call is only permitted if the current interval type is  Interval::DaySecond
        *
        */
        Int32 GetSeconds() const;

        /**
        * @brief
        * Set the interval seconds value
        *
        * @warning
        * this call is only permitted if the current interval type is  Interval::DaySecond
        *
        */
        void SetSeconds(Int32 value);

        /**
        * @brief
        * Return the interval seconds value
        *
        */
        Int32 GetMilliSeconds() const;

        /**
        * @brief
        * Set the interval milliseconds value
        *
        */
        void SetMilliSeconds(Int32 value);

        /**
        * @brief
        * Return the interval nano seconds value
        *
        */
        Int32 GetNanoSeconds() const;

        /**
        * @brief
        * Set the interval nano seconds value
        *
        */
        void SetNanoSeconds(Int32 value);

        /**
        * @brief
        * Extract the date / second parts from the interval value
        *
        * @param day  - Place holder for Day value
        * @param hour - Place holder for Hour value
        * @param min  - Place holder for Minutes value
        * @param sec  - Place holder for Seconds value
        * @param nsec - Place holder for NanoSeconds value
        *
        * @warning
        * this call is only permitted if the current interval type is  Interval::DaySecond
        *
        */
        void GetDaySecond(Int32& day, Int32& hour, Int32& min, Int32& sec, Int32& nsec) const;

        /**
        * @brief
        * Set the Day / Second parts
        *
        * @param day  - Day value
        * @param hour - Hour value
        * @param min  - Minutes value
        * @param sec  - Seconds value
        * @param nsec - NanoSeconds value
        *
        * @warning
        * this call is only permitted if the current interval type is Interval::DaySecond
        *
        */
        void SetDaySecond(Int32 day, Int32 hour, Int32 min, Int32 sec, Int32 nsec);

        /**
        * @brief
        * Extract the year / month parts from the interval value
        *
        * @param year  - Place holder for year value
        * @param month - Place holder for month value
        *
        * @warning
        * this call is only permitted if the current interval type is  Interval::YearMonth
        *
        */
        void GetYearMonth(Int32& year, Int32& month) const;

        /**
        * @brief
        * Set the Year / Month parts
        *
        * @param year  - Year value
        * @param month - Month value
        *
        * @warning
        * this call is only permitted if the current interval type is  Interval::YearMonth
        *
        */
        void SetYearMonth(Int32 year, Int32 month);

        /**
        * @brief
        * Update the interval value with the given time zone
        *
        * @param timeZone - Time zone name
        *
        */
        void UpdateTimeZone(const ostring& timeZone);

        /**
        * @brief
        * Assign to the interval object the value provided by the input interval string
        *
        * @param data - String interval
        *
        * @note
        * For interval format, refer to the Oracle SQL documentation
        *
        */
        void FromString(const ostring& data);

        /**
        * @brief
        * Convert the interval value to a string using the given precisions
        *
        * @param leadingPrecision  - leading precision
        * @param fractionPrecision - fraction precision
        *
        */
        ostring ToString(Int32 leadingPrecision, Int32 fractionPrecision) const;

        /**
        * @brief
        * Convert the interval value to a string using the default precisions of 10
        *
        */
        ostring ToString() const override;

        /**
        * @brief
        * Clone the current instance to a new one performing deep copy
        *
        */
        Interval Clone() const;

        /**
        * @brief
        * Return a new Interval holding the sum of the current Interval value and the given Interval value
        *
        */
        Interval operator + (const Interval& other) const;

        /**
        * @brief
        * Return a new Interval holding the difference of the current Interval value and the given Interval value
        *
        */
        Interval operator - (const Interval& other) const;

        /**
        * @brief
        * Increment the current Value with the given Interval value
        *
        */
        Interval& operator += (const Interval& other);

        /**
        * @brief
        * Decrement the current Value with the given Interval value
        *
        */
        Interval& operator -= (const Interval& other);

        /**
        * @brief
        * Indicates if the current Interval value is equal to the given Interval value
        *
        */
        bool operator == (const Interval& other) const;

        /**
        * @brief
        * Indicates if the current Interval value is not equal the given Interval value
        *
        */
        bool operator != (const Interval& other) const;

        /**
        * @brief
        * Indicates if the current Interval value is superior to the given Interval value
        *
        */
        bool operator > (const Interval& other) const;

        /**
        * @brief
        * Indicates if the current Interval value is inferior to the given Interval value
        *
        */
        bool operator < (const Interval& other) const;

        /**
        * @brief
        * Indicates if the current Interval value is superior or equal to the given Interval value
        *
        */
        bool operator >= (const Interval& other) const;

        /**
        * @brief
        * Indicates if the current Interval value is inferior or equal to the given Interval value
        *
        */
        bool operator <= (const Interval& other) const;

    private:

        Int32 Compare(const Interval& other) const;

        void Allocate(OCI_Environment* env, IntervalTypeValues type, core::Handle* parent);

        Interval(OCI_Interval* pInterval, core::Handle* parent = nullptr);
    };

    /**
     *
     * @brief
     * Object identifying the SQL data type TIMESTAMP.
     *
     * This class wraps the OCILIB object handle OCI_Timestamp and its related methods
     *
     */
    class Timestamp : public core::HandleHolder<OCI_Timestamp*>, public core::Streamable
    {
        friend class Environment;
        friend class Statement;
        friend class Resultset;
        friend class support::BindArray;
        friend class JsonNode;
        friend class Object;
        friend class Connection;
        template<class>
        friend class Collection;

    public:

        /**
        * @brief
        * Interval types enumerated values
        *
        */
        enum TimestampTypeValues
        {
            /** Timestamp does not contains any time zone information */
            NoTimeZone = OCI_TIMESTAMP,
            /** Timestamp contains a given time zone */
            WithTimeZone = OCI_TIMESTAMP_TZ,
            /** Timestamp contains the user's local session time zone */
            WithLocalTimeZone = OCI_TIMESTAMP_LTZ
        };

        /**
        * @brief
        * Type of timestamp
        *
        * Possible values are Timestamp::TimestampTypeValues
        *
        */
        typedef core::Enum<TimestampTypeValues> TimestampType;

        /**
        * @brief
        * return the current system timestamp
        *
        * @param type - Timestamp type to create
        *
        */
        static Timestamp SysTimestamp(const Environment& env, TimestampType type = NoTimeZone);

        /**
        * @brief
        * Create an empty null timestamp instance
        *
        */
        Timestamp();

        /**
        * @brief
        * For value types, overrides HandleHolder copy constructor to perform assignment rather
        * than ref counting management
        *
        * @param other - value to assign
        *
        */
        Timestamp(const Timestamp& other);

        /**
         * @brief
         * For value types, overrides HandleHolder assignment operator to perform assignment rather
         * than ref counting management
         *
         * @param other - value to assign
         *
         */
        Timestamp& operator= (const Timestamp& other);

        /**
        * @brief
        * Create a new instance of the given type
        *
        * @param type - Timestamp type to create
        *
        */
        Timestamp(const Environment& env, TimestampType type);

        /**
        * @brief
        * Creates a timestamp object with the value provided by the input date time string
        *
        * @param type   - Timestamp type to create
        * @param data   - String date time
        * @param format - format of the date time provided in parameter 'data'
        *
        * @note
        * For date time formats, refer to the Oracle SQL documentation
        *
        */
        Timestamp(const Environment& env, TimestampType type, const ostring& data, const ostring& format = OTEXT(""));

        /**
        * @brief
        * Return the type of the given timestamp object
        *
        */
        TimestampType GetType() const;

        /**
        * @brief
        * Convert the current timestamp to the type of the given timestamp.
        *
        * @param other - Timestamp to use for the type conversion
        *
        */
        void Convert(const Timestamp& other);

        /**
        * @brief
        * Check if the given timestamp is valid
        *
        */
        bool IsValid() const;

        /**
        * @brief
        * Return the timestamp year value
        *
        */
        Int32 GetYear() const;

        /**
        * @brief
        * Set the timestamp year value
        *
        */
        void SetYear(Int32 value);

        /**
        * @brief
        * Return the timestamp month value
        *
        */
        Int32 GetMonth() const;

        /**
        * @brief
        * Set the timestamp month value
        *
        */
        void SetMonth(Int32 value);

        /**
        * @brief
        * Return the timestamp day value
        *
        */
        Int32 GetDay() const;

        /**
        * @brief
        * Set the timestamp day value
        *
        */
        void SetDay(Int32 value);

        /**
        * @brief
        * Return the timestamp hours value
        *
        */
        Int32 GetHours() const;

        /**
        * @brief
        * Set the timestamp hours value
        *
        */
        void SetHours(Int32 value);

        /**
        * @brief
        * Return the timestamp minutes value
        *
        */
        Int32 GetMinutes() const;

        /**
        * @brief
        * Set the timestamp minutes value
        *
        */
        void SetMinutes(Int32 value);

        /**
        * @brief
        * Return the timestamp seconds value
        *
        */
        Int32 GetSeconds() const;

        /**
        * @brief
        * Set the timestamp seconds value
        *
        */
        void SetSeconds(Int32 value);

        /**
        * @brief
        * Return the timestamp seconds value
        *
        */
        Int32 GetMilliSeconds() const;

        /**
        * @brief
        * Set the timestamp milliseconds value
        *
        */
        void SetMilliSeconds(Int32 value);

        /**
        * @brief
        * Extract the date parts
        *
        * @param year  - Place holder for year value
        * @param month - Place holder for month value
        * @param day   - Place holder for day value
        *
        */
        void GetDate(Int32& year, Int32& month, Int32& day) const;

        /**
        * @brief
        * Extract time parts
        *
        * @param hour  - Place holder for hour value
        * @param min   - Place holder for minute value
        * @param sec   - Place holder for second value
        * @param fsec  - Place holder for fractional part of the seconds
        *
        */
        void GetTime(Int32& hour, Int32& min, Int32& sec, Int32& fsec) const;

        /**
        * @brief
        * Set the date part
        *
        * @param year  - Year value
        * @param month - Month value
        * @param day   - Day value
        *
        */
        void SetDate(Int32 year, Int32 month, Int32 day);

        /**
        * @brief
        * Set the time part
        *
        * @param hour  - Hour value
        * @param min   - Minute value
        * @param sec   - Second value
        * @param fsec  - Place holder for fractional part of the seconds
        *
        */
        void SetTime(Int32 hour, Int32 min, Int32 sec, Int32 fsec);

        /**
        * @brief
        * Extract date and time parts
        *
        * @param year     - Place holder for year value
        * @param month    - Place holder for month value
        * @param day      - Place holder for day value
        * @param hour     - Place holder for hour value
        * @param min      - Place holder for minute value
        * @param sec      - Place holder for second value
        * @param fsec     - Place holder for fractional part of the seconds
        *
        */
        void GetDateTime(Int32& year, Int32& month, Int32& day, Int32& hour, Int32& min, Int32& sec, Int32& fsec) const;

        /**
        * @brief
        * Set the timestamp value from given date time parts
        *
        * @param year       - Year value
        * @param month      - Month value
        * @param day        - Day value
        * @param hour       - Hour value
        * @param min        - Minutes value
        * @param sec        - Seconds value
        * @param fsec       - Fractional part of seconds value
        * @param timeZone   - name of a time zone to use [optional]
        *
        */
        void SetDateTime(Int32 year, Int32 month, Int32 day, Int32 hour, Int32 min, Int32 sec, Int32 fsec, const ostring& timeZone = OTEXT(""));

        /**
        * @brief
        * Return the name of the current time zone
        *
        * @warning
        * Returns an empty string if the timestamp type is TimeStamp::NoTimeZone
        *
        */
        ostring GetTimeZone() const;

        /**
        * @brief
        * Set the given time zone to the timestamp
        *
        * @warning
        * - The timestamp must have a valid value before setting the timezone
        * - Applies to TimeStamp::WithTimeZone only
        *
        */
        void SetTimeZone(const ostring& timeZone);

        /**
        * @brief
        * Return the time zone (hour, minute) offsets
        *
        * @param hour  - Place holder for hour value
        * @param min   - Place holder for min value
        *
        */
        void GetTimeZoneOffset(Int32& hour, Int32& min) const;

        /**
        * @brief
        * Substract the given two timestamp and store the result into the given Interval
        *
        * @param lsh    - Timestamp value
        * @param rsh    - Timestamp to substract
        * @param result - result difference
        *
        * @note
        * the Interval object type must match the type of range resulting from the timestamp difference
        *
        */
        static void Substract(const Timestamp& lsh, const Timestamp& rsh, Interval& result);

        /**
        * @brief
        * Assign to the timestamp object the value provided by the input date time string
        *
        * @param data   - String date time
        * @param format - format of the date time provided in parameter 'data'
        *
        * @note
        * For date time formats, refer to the Oracle SQL documentation
        *
        */
        void FromString(const ostring& data, const ostring& format = OCI_STRING_FORMAT_DATE);

        /**
        * @brief
        * Convert the timestamp value to a string using the given format and precision
        *
        * @param format    - date time / timestamp format to use
        * @param precision - precision for milliseconds
        *
        * @note
        * For date time / timestamp formats, refer to the Oracle SQL documentation
        *
        */
        ostring ToString(const ostring& format, Int32 precision) const;

        /**
        * @brief
        * Convert the timestamp value to a string using default date format and no precision
        *
        */
        ostring ToString() const override;

        /**
        * @brief
        * Clone the current instance to a new one performing deep copy
        *
        */
        Timestamp Clone() const;

        /**
        * @brief
        * Increment the timestamp by 1 day
        *
        */
        Timestamp& operator ++ ();

        /**
        * @brief
        * Increment the timestamp by 1 day
        *
        */
        Timestamp operator ++ (Int32);

        /**
        * @brief
        * Decrement the Timestamp by 1 day
        *
        */
        Timestamp& operator -- ();

        /**
        * @brief
        * Decrement the Timestamp by 1 day
        *
        */
        Timestamp operator -- (Int32);

        /**
        * @brief
        * Return a new Timestamp holding the current Timestamp value incremented by the given number of days
        *
        */
        Timestamp operator + (Int32 value) const;

        /**
        * @brief
        * Return a new Timestamp holding the current Timestamp value decremented by the given number of days
        *
        */
        Timestamp operator - (Int32 value) const;

        /**
        * @brief
        * Return an interval storing the difference between the current timestamp and the given one
        *
        */
        Interval operator - (const Timestamp& other);

        /**
        * @brief
        * Increment the Timestamp by the given number of days
        *
        */
        Timestamp& operator += (Int32 value);

        /**
        * @brief
        * Decrement the Timestamp by the given number of days
        *
        */
        Timestamp& operator -= (Int32 value);

        /**
        * @brief
        * Return a new Timestamp holding the sum of the current Timestamp value and the given Interval value
        *
        */
        Timestamp operator + (const Interval& other) const;

        /**
        * @brief
        * Return a new Timestamp holding the difference of the current Timestamp value and the given Interval value
        *
        */
        Timestamp operator - (const Interval& other) const;

        /**
        * @brief
        * Increment the current Value with the given Interval value
        *
        */
        Timestamp& operator += (const Interval& other);

        /**
        * @brief
        * Decrement the current Value with the given Interval value
        *
        */
        Timestamp& operator -= (const Interval& other);

        /**
        * @brief
        * Indicates if the current Timestamp value is equal to the given Timestamp value
        *
        */
        bool operator == (const Timestamp& other) const;

        /**
        * @brief
        * Indicates if the current Timestamp value is not equal the given Timestamp value
        *
        */
        bool operator != (const Timestamp& other) const;

        /**
        * @brief
        * Indicates if the current Timestamp value is superior to the given Timestamp value
        *
        */
        bool operator > (const Timestamp& other) const;

        /**
        * @brief
        * Indicates if the current Timestamp value is inferior to the given Timestamp value
        *
        */
        bool operator < (const Timestamp& other) const;

        /**
        * @brief
        * Indicates if the current Timestamp value is superior or equal to the given Timestamp value
        *
        */
        bool operator >= (const Timestamp& other) const;

        /**
        * @brief
        * Indicates if the current Timestamp value is inferior or equal to the given Timestamp value
        *
        */
        bool operator <= (const Timestamp& other) const;

    private:

        Int32 Compare(const Timestamp& other) const;

        void Allocate(OCI_Environment* env, TimestampType type, core::Handle* parent);

        Timestamp(OCI_Timestamp* pTimestamp, core::Handle* parent = nullptr);
    };

    /**
     * @brief
     * Object identifying the SQL data type LOB (CLOB, NCLOB and BLOB)
     *
     * This class wraps the OCILIB object handle OCI_Lob and its related methods
     *
     */
    template<class T, Int32 U>
    class Lob : public core::HandleHolder<OCI_Lob*>
    {
        friend class Statement;
        friend class Resultset;
        friend class support::BindArray;
        friend class Object;
        template<class>
        friend class Collection;

    public:

        /**
        * @brief
        * Create an empty null Lob instance
        *
        */
        Lob();

        /**
        * @brief
        * Parametrized constructor
        *
        * @param connection - Parent connection
        *
        * @note
        * the lob object must not be accessed anymore once the parent connection object gets out of scope
        *
        */
        Lob(const Connection& connection);

        /**
        * @brief
        * Read a portion of a lob
        *
        * @param length - Maximum number of characters or bytes to read
        *
        * @return
        * The content read from the lob
        *
        */
        T Read(UInt32 length);

        /**
        * @brief
        * Write the given content at the current position within the lob
        *
        * @param content - content to write
        *
        * @return
        * Number of character or bytes written into the lob
        *
        */
        UInt32 Write(const T& content);

        /**
        * @brief
        * Append the given content to the lob
        *
        * @param content - content to write
        *
        * @return
        * Number of character or bytes written into the lob
        *
        */
        UInt32 Append(const T& content);

        /**
        * @brief
        * Append the given lob content to the lob
        *
        * @param other - source lob
        *
        */
        void Append(const Lob& other);

        /**
        * @brief
        * Move the current position within the lob for read/write operations
        *
        * @param seekMode - Seek mode
        * @param offset   - offset from current position
        *
        * @note
        * Positions start at 0.
        *
        * @return
        * true on success otherwise false
        *
        */
        bool Seek(SeekMode seekMode, UInt64 offset);

        /**
        * @brief
        * return the type of lob
        *
        */
        LobType GetType() const;

        /**
        * @brief
        * Returns the current R/W offset within the lob
        *
        */
        UInt64 GetOffset() const;

        /**
        * @brief
        * Returns the number of characters or bytes contained in the lob
        *
        */
        UInt64 GetLength() const;

        /**
        * @brief
        * Returns the lob maximum possible size
        *
        */
        UInt64 GetMaxSize() const;

        /**
        * @brief
        * Returns the current lob chunk size
        *
        * @note
        * This chunk size corresponds to the chunk size used by the LOB data layer
        * when accessing and modifying the LOB value. According to Oracle
        * documentation, performance will be improved if the application issues
        * read or write requests using a multiple of this chunk size
        *
        */
        UInt64 GetChunkSize() const;

        /**
        * @brief
        * Return the lob parent connection
        *
        */
        Connection GetConnection() const;

        /**
        * @brief
        * Truncate the lob to a shorter length
        *
        * @param length - New length in characters or bytes
        *
        */
        void Truncate(UInt64 length);

        /**
        * @brief
        * Erase a portion of the lob at a given position
        *
        * @param offset - Absolute position in source lob
        * @param length - Number of bytes or characters to erase
        *
        * @note
        * Absolute position starts at 0.
        * Erasing means that space or null values overwrite the existing LOB value.
        *
        * @return
        * Number of characters or bytes erased
        *
        */
        UInt64 Erase(UInt64 offset, UInt64 length);

        /**
        * @brief
        * Copy the given portion of the lob content to another one
        *
        * @param dest        - Destination lob
        * @param offset      - Absolute position in the lob
        * @param offsetDest  - Absolute position in the destination lob
        * @param length      - Number of characters or bytes to copy
        *
        * @note
        * Absolute position starts at 0.
        *
        */
        void Copy(Lob& dest, UInt64 offset, UInt64 offsetDest, UInt64 length) const;

        /**
        * @brief
        * Check if the given lob is a temporary lob
        *
        */
        bool IsTemporary() const;

        /**
        * @brief
        * Check if the given lob is a remote lob
        *
        */
        bool IsRemote() const;

        /**
        * @brief
        * Open explicitly a Lob
        *
        * @param mode - open mode
        *
        * @note
        * - A call to Open() is not necessary to manipulate a Lob.
        * - If a lob has not been opened explicitly, triggers are fired and
        *   indexes updated at every read/write/append operation
        *
        */
        void Open(OpenMode mode);

        /**
        * @brief
        * Flush the lob content to the server (if applicable)
        *
        */
        void Flush();

        /**
        * @brief
        * Close explicitly a Lob
        *
        * @note
        * - A call to Close() is not necessary to manipulate a Lob.
        *
        */
        void Close();

        /**
        * @brief
        * Enable / disable buffering mode on the given lob object
        *
        * @param value  - Enable/disable buffering mode
        *
        * @note
        * Oracle "LOB Buffering Subsystem" allows client applications
        * to speedup read/write of small buffers on Lobs Objects.
        * Check Oracle Documentation for more details on "LOB Buffering Subsystem".
        * This reduces the number of network round trips and LOB versions, thereby
        * improving LOB performance significantly.
        *
        * @warning
        * According to Oracle documentation the following operations are not permitted
        * on Lobs when buffering is on : Copy(), Append(), Erase(), GetLength(), Truncate()
        *
        */
        void EnableBuffering(bool value);

        /**
        * @brief
        * Clone the current instance to a new one performing deep copy
        *
        */
        Lob Clone() const;

        /**
        * @brief
        * Appending the given lob content to the current lob content
        *
        */
        Lob& operator += (const Lob& other);

        /**
        * @brief
        * Indicates if the current lob value is equal to the given lob value
        *
        */
        bool operator == (const Lob& other) const;

        /**
        * @brief
        * Indicates if the current lob value is not equal the given lob value
        *
        */
        bool operator != (const Lob& other) const;

    private:

        bool Equals(const Lob& other) const;

        Lob(OCI_Lob* pLob, core::Handle* parent = nullptr);

    };

    /**
    *
    * @brief
    * Class handling CLOB oracle type
    *
    * @note
    * Length and size arguments / returned values are expressed in number of characters
    *
    */
    typedef Lob<ostring, LobCharacter> Clob;

    /**
    *
    * @brief
    * Class handling NCLOB oracle type
    *
    * @note
    * Length and size arguments / returned values are expressed in number of characters
    *
    */
    typedef Lob<ostring, LobNationalCharacter> NClob;

    /**
    *
    * @brief
    * Class handling BLOB oracle type
    *
    * @note
    * Length and size arguments / returned values are expressed in number of bytes
    *
    */
    typedef Lob<Raw, LobBinary> Blob;

    /**
     *
     * @brief
     * Object identifying the SQL data type BFILE.
     *
     * This class wraps the OCILIB object handle OCI_File and its related methods
     *
     */
    class File : public core::HandleHolder<OCI_File*>
    {
        friend class Statement;
        friend class Resultset;
        friend class support::BindArray;
        friend class Object;
        template<class>
        friend class Collection;

    public:

        /**
        * @brief
        * Create an empty null File instance
        *
        */
        File();

        /**
        * @brief
        * Parametrized constructor
        *
        * @param connection - Parent connection
        *
        * @note
        * the file object must not be accessed anymore once the parent connection object gets out of scope
        *
        */
        File(const Connection& connection);

        /**
        * @brief
        * Parametrized constructor
        *
        * @param connection - Parent connection
        * @param directory  - File directory
        * @param name       - File name
        *
        * this convenient constructor calls File::SetInfos()
        *
        * @note
        * the file object must not be accessed anymore once the parent connection object gets out of scope
        *
        */
        File(const Connection& connection, const ostring& directory, const ostring& name);

        /**
        * @brief
        * Read a portion of a file
        *
        * @param size - Maximum number of bytes to read
        *
        * @return
        * The content read from the file
        *
        */
        Raw Read(UInt32 size);

        /**
        * @brief
        * Move the current position within the file for read/write operations
        *
        * @param seekMode  - Seek mode
        * @param offset    - offset from current position
        *
        * @note
        * Positions start at 0.
        *
        * @return
        * true on success otherwise false
        *
        */
        bool Seek(SeekMode seekMode, UInt64 offset);

        /**
        * @brief
        * Check if the given file exists on server
        *
        * @note
        * For local FILEs object, SetInfos() must be called before calling Exists()
        *
        */
        bool Exists() const;

        /**
        * @brief
        * Returns the current R/W offset within the file
        *
        */
        UInt64 GetOffset() const;

        /**
        * @brief
        * Returns the number of bytes contained in the file
        *
        */
        UInt64 GetLength() const;

        /**
        * @brief
        * Return the file parent connection
        *
        */
        Connection GetConnection() const;

        /**
        * @brief
        * Set the directory and file name of our file object
        *
        * @param directory - File directory
        * @param name      - File name
        *
        * @note
        * - For local FILEs only
        * - Files fetched from resultset can't be assigned a new directory and name
        *
        */
        void SetInfos(const ostring& directory, const ostring& name);

        /**
        * @brief
        * Return the file name
        *
        */
        ostring GetName() const;

        /**
        * @brief
        * Return the file directory
        *
        */
        ostring GetDirectory() const;

        /**
        * @brief
        * Open a file for reading on the server
        *
        */
        void Open();

        /**
        * @brief
        * Close the file on the server
        *
        */
        void Close();

        /**
        * @brief
        * Check if the specified file is currently opened on the server by our object
        *
        */
        bool IsOpened() const;

        /**
        * @brief
        * Clone the current instance to a new one performing deep copy
        *
        */
        File Clone() const;

        /**
        * @brief
        * Indicates if the current file value is equal the given file value
        *
        */
        bool operator == (const File& other) const;

        /**
        * @brief
        * Indicates if the current file value is not equal the given file value
        *
        */
        bool operator != (const File& other) const;

    private:

        bool Equals(const File& other) const;

        File(OCI_File* pFile, core::Handle* parent = nullptr);
    };

    /**
     * @brief
     * Provides type information on Oracle Database objects
     *
     * This class wraps the OCILIB object handle OCI_TypeInfo and its related methods
     *
     */
    class TypeInfo : public core::HandleHolder<OCI_TypeInfo*>
    {
        friend class Object;
        friend class Reference;
        template<class>
        friend class Collection;
        friend class Column;
    public:

        /**
        * @brief
        * Type of object information enumerated values
        *
        */
        enum TypeInfoTypeValues
        {
            /** Database Table information */
            Table = OCI_TIF_TABLE,
            /** Database View information */
            View = OCI_TIF_VIEW,
            /** Database type information */
            Type = OCI_TIF_TYPE
        };

        /**
        * @brief
        * Type of object information
        *
        * Possible values are TypeInfo::TypeInfoTypeValues
        *
        */
        typedef core::Enum<TypeInfoTypeValues> TypeInfoType;

        /**
        * @brief
        * Default constructor
        *
        * @note
        * The type info is not usable until it is assigned from a call to
        * TypeInfo(const Connection&, const ostring&, TypeInfoType) or
        * TypeInfo::GetTypeInfo(). Call IsNull() to check whether it wraps a
        * valid handle.
        *
        */
        TypeInfo();

        /**
        * @brief
        * Parametrized constructor
        *
        * @param connection - Parent connection
        * @param name       - Type name
        * @param type       - Kind of type to retrieve
        *
        * @note
        * the TypeInfo object must not be accessed anymore once the parent connection object gets out of scope
        *
        */
        TypeInfo(const Connection& connection, const ostring& name, TypeInfoType type);

        /**
        * @brief
        * Return the type of the given TypeInfo object
        *
        */
        TypeInfoType GetType() const;

        /**
        * @brief
        * Return the type info name
        *
        */
        ostring GetName() const;

        /**
        * @brief
        * Return the connection associated with a statement
        *
        */
        Connection GetConnection() const;

        /**
        * @brief
        * Return the number of columns contained in the type
        *
        * @note
        * - For table and views, it is the number of columns
        * - For types, it is the number of member properties
        *
        */
        UInt32 GetColumnCount() const;

        /**
        * @brief
        * Return the column from its index in the resultset
        *
        * @param index  - Column index
        *
        * @note
        * Column position starts at 1.
        *
        * @note
        * - For table and views, the returned column object describes a table/view column
        * - For types, the returned column object describes an object property
        *
        */
        Column GetColumn(UInt32 index) const;

        /**
        * @brief
        * Indicate if the given UDT type is final
        *
        * @note
        * Non-final types are virtual UDT type that can be inherited
        *
        * @return
        * - For table and views, it returns true
        * - For types, it returns true if the type is final and false if it is virtual
        *
        */
        OCI_Boolean IsFinalType() const;

        /**
        * @brief
        * Return the super type of the given type (e.g. parent type for a derived ORACLE UDT type)
        *
        * @note
        *  This property is only valid for types
        *
        * @return
        *  - For table and views, it return a null TypeInfo object
        *  - For types:
        *       - returns a valid TypeInfo object wrapping the parent super type
        *       - returns a null TypeInfo object if the given type is NOT deriving from a base type
        *
        */
        TypeInfo GetSuperType() const;

    private:

        TypeInfo(OCI_TypeInfo* pTypeInfo, core::Handle* parent);
    };

    /**
     * @brief
     * Object identifying the SQL data type OBJECT.
     *
     * This class wraps the OCILIB object handle OCI_Object and its related methods
     *
     */
    class Object : public core::HandleHolder<OCI_Object*>, public core::Streamable
    {
        friend class Statement;
        friend class Resultset;
        friend class support::BindArray;
        friend class Reference;
        template<class>
        friend class Collection;
        friend class Message;

    public:

        /**
        * @brief
        * Object Type enumerated values
        *
        */
        enum ObjectTypeValues
        {
            /** The object is persistent in the database */
            Persistent = OCI_OBJ_PERSISTENT,
            /** The object is Transient */
            Transient = OCI_OBJ_TRANSIENT,
            /** The object is a value member of another object */
            Value = OCI_OBJ_VALUE
        };

        /**
        * @brief
        * Object Type
        *
        * Possible values are Object::ObjectTypeValues
        *
        */
        typedef core::Enum<ObjectTypeValues> ObjectType;

        /**
        * @brief
        * Create an empty null Object instance
        *
        */
        Object();

        /**
        * @brief
        * Parametrized constructor
        *
        * @param typeInfo - type info descriptor
        *
        */
        Object(const TypeInfo& typeInfo);

        /**
        * @brief
        * Check if an object attribute is null
        *
        * @param name - Attribute name
        *
        */
        bool IsAttributeNull(const ostring& name) const;

        /**
        * @brief
        * Set the given object attribute to null
        *
        * @param name - Attribute name
        *
        */
        void SetAttributeNull(const ostring& name);

        /**
        * @brief
        * Return the TypeInfo object describing the object
        *
        */
        TypeInfo GetTypeInfo() const;

        /**
        * @brief
        * Creates a reference on the current object
        *
        */
        Reference GetReference() const;

        /**
        * @brief
        * Return the type of the given object
        *
        */
        ObjectType GetType() const;

        /**
        * @brief
        * Return the given object attribute value
        *
        * @tparam T - C++ object type to retrieve
        *
        * @param name - Attribute name
        *
        * @note
        * Specialized version of this template function are provided for all supported types
        *
        */
        template<class T>
        T Get(const ostring& name) const;

        /**
        * @brief
        * Assign the given value with the given object attribute value
        *
        * @tparam T - C++ object type to retrieve
        *
        * @param name  - Attribute name
        * @param value - value to assign
        *
        * @note
        * Specialized version of this template function are provided for all supported types
        *
        */
        template<class T>
        void Get(const ostring& name, T& value) const;

        /**
        * @brief
        * Assign the given collection with the given object attribute value of type collection
        *
        * @tparam T - C++ object type of the collection to retrieve
        *
        * @param name  - Attribute name
        * @param value - value to assign
        *
        * @note
        * Specialized version of this template function are provided for all supported types
        *
        */
        template<class T>
        void Get(const ostring& name, Collection<T>& value) const;

        /**
        * @brief
        * Set the given object attribute value
        *
        * @tparam T - C++ object type to set
        *
        * @param name  - Attribute name
        * @param value - Attribute value
        *
        * @note
        * Specialized version of this template function are provided for all supported types
        *
        */
        template<class T>
        void Set(const ostring& name, const T& value);

        /**
        * @brief
        * Clone the current instance to a new one performing deep copy
        *
        */
        Object Clone() const;

        /**
        * @brief
        * return a string representation of the current object
        *
        */
        ostring ToString() const override;

    private:

        Object(OCI_Object* pObject, core::Handle* parent = nullptr);
    };

    /**
     * @brief
     * Object identifying the SQL data type REF.
     *
     * This class wraps the OCILIB object handle OCI_Ref and its related methods
     *
     */
    class Reference : public core::HandleHolder<OCI_Ref*>, public core::Streamable
    {
        friend class Statement;
        friend class Resultset;
        friend class support::BindArray;
        friend class Object;
        template<class>
        friend class Collection;

    public:

        /**
        * @brief
        * Create an empty null Reference instance
        *
        */
        Reference();

        /**
        * @brief
        * Parametrized constructor
        *
        * @param typeInfo - type info descriptor
        *
        */
        Reference(const TypeInfo& typeInfo);

        /**
        * @brief
        * Return the TypeInfo object describing the referenced object
        *
        */
        TypeInfo GetTypeInfo() const;

        /**
        * @brief
        * Returns the object pointed by the reference
        *
        * @return
        * The object may be null if the current reference value is null
        *
        */
        Object GetObject() const;

        /**
        * @brief
        * Check if the reference points to an object or not.
        *
        */
        bool IsReferenceNull() const;

        /**
        * @brief
        * Nullify the given Ref handle
        *
        * @note
        * this call clears the reference to object pointed by the reference object.
        *
        */
        void SetReferenceNull();

        /**
        * @brief
        * Clone the current instance to a new one performing deep copy
        *
        */
        Reference Clone() const;

        /**
        * @brief
        * return a string representation of the current reference
        *
        */
        ostring ToString() const override;

    private:

        Reference(OCI_Ref* pRef, core::Handle* parent = nullptr);
    };

    /**
     * @brief
     * Object identifying the SQL data type XMLTYPE.
     *
     * This class wraps the OCILIB object handle OCI_XmlType and its related methods
     *
     */
    class XmlType : public core::HandleHolder<OCI_XmlType*>, public core::Streamable
    {
        friend class Resultset;

    public:

        /**
        * @brief
        * return a string representation of the current XmlType
        *
        */
        ostring ToString() const override;

    private:

        XmlType(OCI_XmlType* pXmlType, core::Handle* parent = nullptr);
    };

    /**
      * @brief
      * Object identifying the SQL data type VECTOR.
      *
      * This class wraps the OCILIB object handle OCI_Vector and its related methods
      *
      */
    class Vector : public core::HandleHolder<OCI_Vector*>, public core::Streamable
    {
        friend class Resultset;

    public:

        /**
        * @brief
        * Vector Format types enumerated values
        *
        */
        enum VectorFormatValues
        {
            /** Flexible format */
            Flex = OCI_VEC_FLEX,
            /** 8-bit integer format */
            Int8 = OCI_VEC_INT8,
            /** 32-bit floating point format */
            Float32 = OCI_VEC_FLOAT32,
            /** 64-bit floating point format */
            Float64 = OCI_VEC_FLOAT64,
            /** Binary format */
            Binary = OCI_VEC_BINARY
        };

        /**
        * @brief
        * Type of Format
        *
        * Possible values are Vector::VectorFormatValues
        *
        */
        typedef core::Enum<VectorFormatValues> VectorFormat;

        /**
        * @brief
        * Create an empty null Vector instance
        *
        */
        Vector();

        /**
        * @brief
        * Parametrized constructor
        *
        * @param connection - Parent connection
        *
        * @note
        * - The Vector object must not be accessed anymore once the parent connection object gets out of scope
        *
        */
        Vector(const Connection& connection);

        /**
         * @brief
         * Set the Vector content from a raw array of values
         *
         * @param values - values to set
         * @param size   - number of element in the input buffer array
         *
        */
        template<class T, core::SupportedVectorNumericT<T>* = nullptr>
        void Set(T* values, UInt32 size);

        /**
         * @brief
         * Fills the given raw array with the Vector content
         *
         * @param values - input array buffer to fill
         *
         * @warning
         * The input buffer must be large enough to receive the values.
         * Use GetDimensionCount() to know the required number of elements.
         *
         * @warning
         * T Type must match the format of the Vector current content :
         * - VectorFormat::Int8 => char
         * - VectorFormat::Float32 => Float
         * - VectorFormat::Float64 => Double
         * - VectorFormat::Binary => unsigned char
         *
         */
        template<class T, core::SupportedVectorNumericT<T>* = nullptr>
        void Get(T* values);

        /**
         * @brief
         * Set the Vector content from a std::vector<T>
         *
         * @param values - values to set
         *
         */
        template<class T, core::SupportedVectorNumericT<T>* = nullptr>
        void Set(const std::vector<T>& values);

        /**
         * @brief
         * Returns the Vector content as a std::vector<T>
         *
         * @warning
         * T Type must match the format of the Vector current content :
         * - VectorFormat::Int8 => char
         * - VectorFormat::Float32 => Float
         * - VectorFormat::Float64 => Double
         * - VectorFormat::Binary => unsigned char
         *
         */
        template<class T, core::SupportedVectorNumericT<T>* = nullptr>
        std::vector<T> Get();

        /**
        * @brief
        * Return the dimension count of the vector
        *
        */
        UInt32 GetDimensionCount();

        /**
        * @brief
        * Return the format of the vector
        *
        */
        VectorFormat GetFormat();

        /**
         * @brief
         * Assign to the Vector object the value provided by the given string
         *
         * @param str        - textual representation of the vector
         * @param format     - format of the vector contained in the input string
         * @param dimensions - number of dimensions of the vector contained in the input string
         *
         *
         */
        void FromString(const ostring& str, VectorFormat format, UInt32 dimensions);

        /**
        * @brief
        * return a string representation of the current Vector
        *
        */
        ostring ToString() const override;

    private:

        Vector(OCI_Vector* pVector, core::Handle* parent = nullptr);
    };

    /**
    * @brief
    * Forward declaration of JsonNode class
    *
    */
    class JsonNode;

    /**
    * @brief
    * Object identifying the SQL data type JSON.
    *
    * This class wraps the OCILIB object handle OCI_Json and its related methods
    *
    */
    class Json : public core::HandleHolder<OCI_Json*>, public core::Streamable
    {
        friend class Statement;
        friend class Resultset;
        friend class support::BindArray;
        friend class JsonNode;
        friend class SodaDoc;

    public:

        /**
        * @brief
        * JSON node type enumerated values
        *
        */
        enum JsonTypeValues
        {
            /** Null JSON value */
            NodeNull = OCI_JNT_NULL,
            /** JSON object */
            NodeObject = OCI_JNT_OBJECT,
            /** JSON array */
            NodeArray = OCI_JNT_ARRAY,
            /** JSON string */
            NodeString = OCI_JNT_STRING,
            /** JSON number */
            NodeNumber = OCI_JNT_NUMBER,
            /** JSON boolean */
            NodeBoolean = OCI_JNT_BOOLEAN,
            /** JSON binary */
            NodeBinary = OCI_JNT_BINARY,
            /** JSON date */
            NodeDate = OCI_JNT_DATE,
            /** JSON timestamp */
            NodeTimestamp = OCI_JNT_TIMESTAMP,
            /** JSON interval */
            NodeInterval = OCI_JNT_INTERVAL
        };

        /**
        * @brief
        * JSON node type
        *
        * Possible values are Json::JsonTypeValues
        *
        */
        typedef core::Enum<JsonTypeValues> JsonType;

        /**
        * @brief
        * JSON text parsing validation options
        *
        */
        enum JsonValidationValues
        {
            AllowNone = OCI_JV_ALLOW_NONE,
            AllowUnquotedNames = OCI_JV_ALLOW_UNQUOTED_NAMES,
            AllowSingleQuotes = OCI_JV_ALLOW_SINGLE_QUOTES,
            AllowUnquotedUnicode = OCI_JV_ALLOW_UNQUOTED_UNICODE,
            AllowNumericQuirks = OCI_JV_ALLOW_NUMERIC_QUIRKS,
            AllowControlsWhitespace = OCI_JV_ALLOW_CONTROLS_WHITESPACE,
            AllowTrailingCommas = OCI_JV_ALLOW_TRAILING_COMMAS,
            AllowMixedcaseKeywords = OCI_JV_ALLOW_MIXEDCASE_KEYWORDS,
            AllowScalarDocuments = OCI_JV_ALLOW_SCALAR_DOCUMENTS,
            AllowLeadingBom = OCI_JV_ALLOW_LEADING_BOM,
            AllowAll = OCI_JV_ALLOW_ALL
        };

        /**
        * @brief
        * JSON text validation options
        *
        * Possible values are Json::JsonValidationValues
        *
        */
        typedef core::Flags<JsonValidationValues> JsonValidation;

        /**
        * @brief
        * JSON text encoding options
        *
        */
        enum JsonEncodingValues
        {
            Detect = OCI_JE_DETECT,
            Direct = OCI_JE_DIRECT,
            Convert = OCI_JE_CONVERT,
            Utf16 = OCI_JE_UTF16,
            Utf16Swap = OCI_JE_UTF16_SWAP,
            Latin = OCI_JE_LATIN,
            Windows = OCI_JE_WINDOWS,
            Utf8 = OCI_JE_UTF8
        };

        /**
        * @brief
        * JSON text encoding
        *
        * Possible values are Json::JsonEncodingValues
        *
        */
        typedef core::Enum<JsonEncodingValues> JsonEncoding;

        /**
        * @brief
        * JSON text printing options
        *
        */
        enum JsonPrintValues
        {
            Pretty = OCI_JP_PRETTY,
            Ascii = OCI_JP_ASCII,
            NumFormat = OCI_JP_NUMFORMAT,
            InternalBuf = OCI_JP_INTERNAL_BUF,
            NlsCharset = OCI_JP_NLS_CHARSET,
            Replacement = OCI_JP_REPLACEMENT,
            Ejson = OCI_JP_EJSON,
            BsonPatterns = OCI_JP_BSON_PATTERNS,
            Test = OCI_JP_TEST
        };

        /**
        * @brief
        * JSON text printing options
        *
        * Possible values are Json::JsonPrintValues
        *
        */
        typedef core::Flags<JsonPrintValues> JsonPrint;

        /**
        * @brief
        * Create an empty null Json object
        *
        */
        Json();

        /**
        * @brief
        * Create a local Json object
        *
        * @param connection - Connection to use
        *
        */
        Json(const Connection& connection);

        /**
        * @brief
        * Parse a JSON text buffer and load it into the Json object
        *
        * @param text       - JSON text
        * @param validation - Parsing validation options
        * @param encoding   - Text encoding option
        *
        */
        void LoadText(const ostring& text, JsonValidation validation = JsonValidation(AllowAll), JsonEncoding encoding = JsonEncoding(Detect));

        /**
        * @brief
        * Parse a JSON text buffer and load it into the Json object
        *
        * @param text       - JSON text buffer
        * @param length     - Length of the text buffer (in characters)
        * @param validation - Parsing validation options
        * @param encoding   - Text encoding option
        *
        */
        void LoadText(const otext* text, UInt32 length, JsonValidation validation = JsonValidation(AllowAll), JsonEncoding encoding = JsonEncoding(Detect));

        /**
        * @brief
        * Load an OSON binary buffer into the Json object
        *
        * @param data   - OSON binary buffer
        * @param length - Buffer length in bytes
        *
        */
        void LoadBinary(const void* data, UInt32 length);

        /**
        * @brief
        * Load an OSON binary buffer into the Json object
        *
        * @param data - OSON binary data
        *
        */
        void LoadBinary(const Raw& data);

        /**
        * @brief
        * Serialize the Json object to a JSON text string
        *
        * @param print - Printing options
        *
        * @return The serialized JSON text
        *
        */
        ostring ToText(JsonPrint print = JsonPrint()) const;

        /**
        * @brief
        * Serialize the Json object to an OSON binary buffer
        *
        * @return The serialized OSON binary data
        *
        */
        Raw ToBinary() const;

        /**
        * @brief
        * Clone the current Json object into another one
        *
        * @param dest - Destination Json object
        *
        * @return TRUE on success, FALSE otherwise
        *
        */
        bool Clone(Json& dest) const;

        /**
        * @brief
        * Set the mutability status of the Json object
        *
        * @param value - New mutability value
        *
        */
        void SetMutable(bool value);

        /**
        * @brief
        * Check if the Json object is mutable
        *
        * @return TRUE if mutable, FALSE otherwise
        *
        */
        bool IsMutable() const;

        /**
        * @brief
        * Return the root node of the JSON document
        *
        * @return The root JsonNode
        *
        */
        JsonNode GetRoot();

        /**
        * @brief
        * Set the root node of the JSON document
        *
        * @param node - New root node
        *
        */
        void SetRoot(const JsonNode& node);

        /**
        * @brief
        * Create a new JSON node of the given type
        *
        * @param type - Node type
        *
        * @return The new JsonNode
        *
        */
        JsonNode CreateNode(JsonType type);

        /**
        * @brief
        * Create a new empty JSON object node
        *
        * @return The new JsonNode
        *
        */
        JsonNode CreateObject();

        /**
        * @brief
        * Create a new empty JSON array node
        *
        * @return The new JsonNode
        *
        */
        JsonNode CreateArray();

        /**
        * @brief
        * Create a new JSON null node
        *
        * @return The new JsonNode
        *
        */
        JsonNode CreateNull();

        /**
        * @brief
        * Create a new JSON string node
        *
        * @param value - String value
        *
        * @return The new JsonNode
        *
        */
        JsonNode CreateString(const ostring& value);

        /**
        * @brief
        * Create a new JSON number node
        *
        * @param value - Number value
        *
        * @return The new JsonNode
        *
        */
        JsonNode CreateNumber(const Number& value);

        /**
        * @brief
        * Create a new JSON boolean node
        *
        * @param value - Boolean value
        *
        * @return The new JsonNode
        *
        */
        JsonNode CreateBoolean(bool value);

        /**
        * @brief
        * Create a new JSON binary node
        *
        * @param data   - Binary data
        * @param length - Data length in bytes
        *
        * @return The new JsonNode
        *
        */
        JsonNode CreateBinary(const void* data, UInt32 length);

        /**
        * @brief
        * Create a new JSON binary node
        *
        * @param data - Binary data
        *
        * @return The new JsonNode
        *
        */
        JsonNode CreateBinary(const Raw& data);

        /**
        * @brief
        * Create a new JSON date node
        *
        * @param value - Date value
        *
        * @return The new JsonNode
        *
        */
        JsonNode CreateDate(const Date& value);

        /**
        * @brief
        * Create a new JSON timestamp node
        *
        * @param value - Timestamp value
        *
        * @return The new JsonNode
        *
        */
        JsonNode CreateTimestamp(const Timestamp& value);

        /**
        * @brief
        * Create a new JSON interval node
        *
        * @param value - Interval value
        *
        * @return The new JsonNode
        *
        */
        JsonNode CreateInterval(const Interval& value);

        /**
        * @brief
        * Return a string representation of the current Json object
        *
        * @return The serialized JSON text
        *
        */
        ostring ToString() const override;

    private:

        Json& operator=(OCI_Json*& lhs);
        Json(OCI_Json* pJson, core::Handle* parent = nullptr);
    };

    /**
    * @brief
    * Object identifying a node within a JSON document.
    *
    * This class wraps the OCILIB object handle OCI_JsonNode and its related methods
    *
    */
    class JsonNode : public core::HandleHolder<OCI_JsonNode*>
    {
        friend class Json;

    public:

        /**
        * @brief
        * Create an empty null JsonNode object
        *
        */
        JsonNode();

        /**
        * @brief
        * JSON node type
        *
        */
        typedef Json::JsonType JsonType;

        /**
        * @brief
        * Return the type of the JSON node
        *
        * @return The node type
        *
        */
        JsonType GetType() const;

        /**
        * @brief
        * Check if the JSON node value is null
        *
        * @return TRUE if the node is null, FALSE otherwise
        *
        */
        bool IsNull() const;

        /**
        * @brief
        * Return the number of fields in a JSON object node
        *
        * @return Number of fields
        *
        */
        UInt32 GetFieldCount() const;

        /**
        * @brief
        * Return the name of a JSON object field at the given index
        *
        * @param index - Field index (starts at 1)
        *
        * @return Field name
        *
        */
        ostring GetFieldName(UInt32 index) const;

        /**
        * @brief
        * Return a JSON object field by name
        *
        * @param name - Field name
        *
        * @return The field JsonNode
        *
        */
        JsonNode GetFieldByName(const ostring& name) const;

        /**
        * @brief
        * Return a JSON object field by index
        *
        * @param index - Field index (starts at 1)
        *
        * @return The field JsonNode
        *
        */
        JsonNode GetFieldByIndex(UInt32 index) const;

        /**
        * @brief
        * Return a JSON object field by name or index
        *
        */
        JsonNode GetField(const ostring& name) const;
        JsonNode GetField(UInt32 index) const;

        /**
        * @brief
        * Return the number of items in a JSON array node
        *
        * @return Number of items
        *
        */
        UInt32 GetItemCount() const;

        /**
        * @brief
        * Return a JSON array item by index
        *
        * @param index - Item index (starts at 1)
        *
        * @return The item JsonNode
        *
        */
        JsonNode GetItem(UInt32 index) const;

        /**
        * @brief
        * Return the string value of a JSON string node
        *
        * @return The string value
        *
        */
        ostring GetString() const;

        /**
        * @brief
        * Return the number value of a JSON number node
        *
        * @return The number value
        *
        */
        Number GetNumber() const;

        /**
        * @brief
        * Return the boolean value of a JSON boolean node
        *
        * @return The boolean value
        *
        */
        bool GetBoolean() const;

        /**
        * @brief
        * Return the binary value of a JSON binary node
        *
        * @return The binary value
        *
        */
        Raw GetBinary() const;

        /**
        * @brief
        * Return the date value of a JSON date node
        *
        * @return The date value
        *
        */
        Date GetDate() const;

        /**
        * @brief
        * Return the timestamp value of a JSON timestamp node
        *
        * @return The timestamp value
        *
        */
        Timestamp GetTimestamp() const;

        /**
        * @brief
        * Return the interval value of a JSON interval node
        *
        * @return The interval value
        *
        */
        Interval GetInterval() const;

        /**
        * @brief
        * Set the string value of a JSON string node
        *
        * @param value - New string value
        *
        */
        void SetString(const ostring& value);

        /**
        * @brief
        * Set the number value of a JSON number node
        *
        * @param value - New number value
        *
        */
        void SetNumber(const Number& value);

        /**
        * @brief
        * Set the boolean value of a JSON boolean node
        *
        * @param value - New boolean value
        *
        */
        void SetBoolean(bool value);

        /**
        * @brief
        * Set the binary value of a JSON binary node
        *
        * @param data   - Binary data
        * @param length - Data length in bytes
        *
        */
        void SetBinary(const void* data, UInt32 length);

        /**
        * @brief
        * Set the binary value of a JSON binary node
        *
        * @param data - Binary data
        *
        */
        void SetBinary(const Raw& data);

        /**
        * @brief
        * Set the JSON node to null
        *
        */
        void SetNull();

        /**
        * @brief
        * Set the date value of a JSON date node
        *
        * @param value - New date value
        *
        */
        void SetDate(const Date& value);

        /**
        * @brief
        * Set the timestamp value of a JSON timestamp node
        *
        * @param value - New timestamp value
        *
        */
        void SetTimestamp(const Timestamp& value);

        /**
        * @brief
        * Set the interval value of a JSON interval node
        *
        * @param value - New interval value
        *
        */
        void SetInterval(const Interval& value);

        /**
        * @brief
        * Set a field of a JSON object node
        *
        * @param name  - Field name
        * @param value - Field value
        *
        */
        void SetField(const ostring& name, const JsonNode& value);

        /**
        * @brief
        * Set an item of a JSON array node
        *
        * @param index - Item index (starts at 1)
        * @param value - New item value
        *
        */
        void SetItem(UInt32 index, const JsonNode& value);

        /**
        * @brief
        * Append an item to a JSON array node
        *
        * @param value - Item value to append
        *
        */
        void AppendItem(const JsonNode& value);

    private:

        JsonNode(OCI_JsonNode* pNode, core::Handle* parent = nullptr);
    };

    /**
    * @brief
    * Class used for handling transient collection value.
    * it is used internally by the Collection<T> class:
    * - the Collection<> indexer operator in order to provide lvalue for read/write access
    * - the CollectionIterator class
    *
    */
    template<class T>
    class CollectionElement
    {
        template<class>
        friend class CollectionIterator;

    public:

        typedef T ValueType;
        typedef Collection<ValueType> CollectionType;

        CollectionElement();
        CollectionElement(CollectionType* coll, UInt32 pos);
        operator T() const;
        CollectionElement& operator = (const ValueType& other);
        CollectionElement& operator = (const CollectionElement& other);
        bool IsNull() const;
        void SetNull();

    private:

        CollectionType* _coll;
        UInt32 _pos;
    };

    /**
    * @brief
    * STL compliant Collection Random iterator class
    *
    * @warning this iterator does not support the operator[]
    *
    */
    template<class T>
    class CollectionIterator
    {
    public:

        template<class>
        friend class Collection;

        typedef typename T::ValueType value_type;
        typedef Collection<value_type> CollectionType;

        typedef std::random_access_iterator_tag iterator_category;
        typedef ptrdiff_t difference_type;
        typedef ptrdiff_t distance_type;
        typedef value_type* pointer;
        typedef value_type& reference;

        CollectionIterator();
        CollectionIterator(const CollectionIterator& other);

        CollectionIterator& operator = (const CollectionIterator& other);

        CollectionIterator& operator += (difference_type value);
        CollectionIterator& operator -= (difference_type value);

        T& operator*();
        T* operator->();

        CollectionIterator& operator++();
        CollectionIterator& operator--();

        CollectionIterator operator++(Int32);
        CollectionIterator operator--(Int32);

        CollectionIterator operator + (difference_type value);
        CollectionIterator operator - (difference_type value);

        difference_type operator - (const CollectionIterator& other);

        bool operator == (const CollectionIterator& other);
        bool operator != (const CollectionIterator& other);
        bool operator >  (const CollectionIterator& other);
        bool operator <  (const CollectionIterator& other);
        bool operator >= (const CollectionIterator& other);
        bool operator <= (const CollectionIterator& other);

    protected:

        CollectionIterator(CollectionType* collection, UInt32 pos);

        T _elem;
    };

    /**
     * @brief
     * Object identifying the SQL data types VARRAY and NESTED TABLE.
     *
     * This class wraps the OCILIB object handle OCI_Coll and its related methods
     *
     */
    template<class T>
    class Collection : public core::HandleHolder<OCI_Coll*>, public core::Streamable
    {
        friend class Statement;
        friend class Resultset;
        friend class support::BindArray;
        friend class Object;
        template<class>
        friend class CollectionIterator;
        template<class>
        friend class Collection;

    public:

        /**
         * @brief
         * Collection type enumerated values
         *
         */
        enum CollectionTypeValues
        {
            /** Collection is a VARRAY */
            Varray = OCI_COLL_VARRAY,
            /** Collection is a NESTED TABLE */
            NestedTable = OCI_COLL_NESTED_TABLE,
            /** Collection is a PL/SQL TABLE INDEX BY */
            IndexedTable = OCI_COLL_INDEXED_TABLE
        };

        /**
        * @brief
        * Collection type
        *
        * Possible values are Collection::CollectionTypeValues
        *
        */
        typedef core::Enum<CollectionTypeValues> CollectionType;

        /**
        * @brief
        * Create an empty null Collection instance
        *
        */
        Collection();

        /**
        * @brief
        * Parametrized constructor
        *
        * @param typeInfo - type info descriptor
        *
        */
        Collection(const TypeInfo& typeInfo);

        /**
        * @brief
        * Return the type of the collection
        *
        */
        CollectionType GetType() const;

        /**
        * @brief
        * Returns the maximum number of elements for the collection
        *
        */
        UInt32 GetMax() const;

        /**
        * @brief
        * Returns the total number of elements in the collection
        *
        */
        UInt32 GetSize() const;

        /**
        * @brief
        * Returns the current number of elements in the collection
        *
        * @note
        * - For VARRAYs, it returns the same value than GetSize() as VARRAYs cannot contains holes
        * - For Nested Tables that are spare collections that can have holes, it returns the total number
        *   of elements minus the total of deleted elements
        *
        */
        UInt32 GetCount() const;

        /**
        * @brief
        * Trim the given number of elements from the end of the collection
        *
        * @param size - Number of elements to trim
        *
        */
        void Truncate(UInt32 size);

        /**
        * @brief
        * Clear all items of the collection
        *
        */

        void Clear();

        /**
        * @brief
        * check if the element at the given index is null
        *
        * @param index - Index of the element
        *
        *  @note
        * Collection Index start at 1
        *
        */
        bool IsElementNull(UInt32 index) const;

        /**
        * @brief
        * Nullify the element at the given index
        *
        * @param index - Index of the element
        *
        *  @note
        * Collection Index start at 1
        *
        */
        void SetElementNull(UInt32 index);

        /**
        * @brief
        * Delete the element at the given position in the Nested Table Collection
        *
        * @param index - Index of the element to delete
        *
        * @note
        * Collection indexes start at position 1.
        *
        * @warning
        * Delete() is only valid for nested tables (e.g. when collection type is Collection::NestedTable
        *
        * @return
        * - if the input collection is a nested table, it returns true if the element  is successfully deleted
        * - if the input collection is a VARRAY, it always returns false
        *
        */
        bool Delete(UInt32 index) const;

        /**
        * @brief
        * Return the collection element value at the given position
        *
        * @param index - Index of the element
        *
        */
        T Get(UInt32 index) const;

        /**
        * @brief
        * Set the collection element value at the given position
        *
        * @param index - Index of the element
        * @param value - Value to set
        *
        */
        void Set(UInt32 index, const T& value);

        /**
        * @brief
        * Append the given element value at the end of the collection
        *
        * @param value - Value to add
        *
        *
        */
        void Append(const T& value);

        /**
        * @brief
        * Return the type information object associated to the collection
        *
        */
        TypeInfo GetTypeInfo() const;

        /**
        * @brief
        * Clone the current instance to a new one performing deep copy
        *
        */
        Collection Clone() const;

        /**
        * @brief
        * return a string representation of the current collection
        *
        */
        ostring ToString() const override;

        /**
        * @brief
        * Common iterator declaration
        *
        */
        typedef CollectionIterator<CollectionElement<T> > iterator;

        /**
        * @brief
        * Common const iterator declaration
        *
        */
        typedef CollectionIterator<const CollectionElement<T> > const_iterator;

        /**
        * @brief
        * Returns an iterator pointing to the first element in the collection
        *
        */
        iterator begin();

        /**
        * @brief
        * Returns a const iterator pointing to the first element in the collection
        *
        */
        const_iterator begin() const;

        /**
        * @brief
        * Returns an iterator referring to the past-the-end element in the collection
        *
        */
        iterator end();

        /**
        * @brief
        * Returns a const iterator referring to the past-the-end element in the collection
        *
        */
        const_iterator end() const;

        /**
        * @brief
        * Returns the element at a given position in the collection.
        *
        */
        CollectionElement<T> operator [] (UInt32 index);

        /**
        * @brief
        * Returns the element at a given position in the collection.
        *
        */
        CollectionElement<T> operator [](UInt32 index) const;

    private:

        static T GetElem(OCI_Elem* elem, core::Handle* parent);

        static void SetElem(OCI_Elem* elem, const T& value, core::Handle* parent);

        Collection(OCI_Coll* pColl, core::Handle* parent = nullptr);
    };

    /**
     * @brief
     * Object identifying the SQL data type LONG.
     *
     * This class wraps the OCILIB object handle OCI_LONG of type OCI_CLONG and its related methods
     *
     */
    template<class T, Int32 U>
    class Long : public core::HandleHolder<OCI_Long*>
    {
        friend class Statement;
        friend class Resultset;
        friend class support::BindArray;

    public:

        /**
        * @brief
        * Create an empty null Long instance
        *
        */
        Long();

        /**
        * @brief
        * Constructor
        *
        * @param statement - statement object that will handle operations on the long buffer
        *
        */
        Long(const Statement& statement);

        /**
        * @brief
        * Write the given string into the long Object
        *
        * @param content - string to write
        *
        * @return
        * Number of character written
        *
        */
        UInt32 Write(const T& content);

        /**
        * @brief
        * Return the buffer length
        *
        */
        UInt32 GetLength() const;

        /**
        * @brief
        * Return the string read from a fetch sequence
        *
        */
        T GetContent() const;

    private:

        Long(OCI_Long* pLong, core::Handle* parent = nullptr);
    };

    /**
    *
    * @brief
    * Class handling LONG oracle type
    *
    * @note
    * Length and size arguments / returned values are expressed in number of characters
    *
    */
    typedef Long<ostring, LongCharacter> Clong;

    /**
    *
    * @brief
    * Class handling LONG RAW oracle type
    *
    * @note
    * Length and size arguments / returned values are expressed in number of bytes
    *
    */
    typedef Long<Raw, LongBinary> Blong;

    /**
     * @brief
     * Provides SQL bind information
     *
     * This class wraps the OCILIB object handle OCI_Bind and its related methods
     *
     */
    class BindInfo : public core::HandleHolder<OCI_Bind*>
    {
        friend class Statement;

    public:

        /**
        * @brief
        * Bind direction enumerated values
        *
        */
        enum BindDirectionValues
        {
            /** Input bind variable (will be not modified as it is an input value for the server) */
            In = OCI_BDM_IN,
            /** Output bind variable (will be modified  as it is an output value by the server ) */
            Out = OCI_BDM_OUT,
            /** Input / Output bind variable (can be modified as it is an input value that can be modified by the server) */
            InOut = OCI_BDM_IN_OUT
        };

        /**
        * @brief
        * Bind direction
        *
        * Possible values are BindInfo::BindDirectionValues
        *
        */
        typedef core::Enum<BindDirectionValues> BindDirection;

        /**
        * @brief
        * Vector type values
        *
        */
        enum VectorTypeValues
        {
            /** Vector is bound as an array in a regular DML array operation */
            AsArray = 1,
            /** Vector is bound as a PL/SQL index by table */
            AsPlSqlTable = 2
        };

        /**
        * @brief
        * Vector type
        *
        * Possible values are BindInfo::VectorTypeValues
        *
        */
        typedef core::Enum<VectorTypeValues> VectorType;

        /**
        * @brief
        * Return the name of the bind object
        *
        */
        ostring GetName() const;

        /**
        * @brief
        * Return the OCILIB type of the data associated with the bind object
        *
        */
        DataType GetType() const;

        /**
        * @brief
        * Return the OCILIB object subtype of a column
        *
        * @note
        * This call is valid for the following OCILIB types:
        * - Scalar numeric types
        * - Clong and Blong
        * - Clob, NClob and Blob
        * - File
        * - Timestamp
        * - Interval
        *
        * @warning
        * - Cast returned values to the matching C++ class GetType() property type.
        * - For scalar numeric types, cast returned value to ocilib::NumericType values.
        * - For a non valid type, it returns 0.
        *
        */
        UInt32 GetSubType() const;

        /**
        * @brief
        * Return the number of elements associated with the bind object
        *
        * @return
        * - For single binds, it returns 1
        * - For vector binds, it returns the number of element in the bind array
        *
        */
        UInt32 GetDataCount() const;

        /**
        * @brief
        * Return the statement associated with the bind object
        *
        */
        Statement GetStatement() const;

        /**
        * @brief
        * Mark as null or not null the current bind real value(s) used in SQL statements
        *
        * @param value - null status value
        * @param index - Only valid for vectors : index of the element to check
        *
        * @note
        * There is no notion of null value in C C++ for scalar types.
        * It's necessary to explicitly tell Oracle that the bind has a null value.
        * It must be done before an Execute() call
        *
        * @warning
        * For handled based data types (non scalar types), OCILIB performs an extra
        * check on handles and set the bind status to null is the handle is null
        *
        * @warning
        * Index starts with 1
        *
        */
        void SetDataNull(bool value, UInt32 index = 1);

        /**
        * @brief
        * Check if the current bind value(s) used in SQL statements is marked as NULL
        *
        * @param index - Only valid for vectors : index of the element to check
        *
        * @note
        * If the bind is related to a single host variable, don't use the parameter index
        * If the bind is related to host vectors, the parameter 'index' refers to the index in the vector
        *
        * @warning
        * Index starts with 1
        *
        */
        bool IsDataNull(UInt32 index = 1) const;

        /**
        * @brief
        * Set the charset form of the given character based bind object
        *
        * @param value - charset form
        *
        * @note
        * This call has to be made after Statement::Prepare() but before Statement::Execute()
        *
        * @warning
        * This call does nothing the bind data type is not ocilib::TypeString or ocilib::TypeLong
        *
        */
        void SetCharsetForm(CharsetForm value);

        /**
        * @brief
        * Set the direction mode
        *
        * @param value - direction mode
        *
        * @note
        * This call has to be made after Statement::Prepare() but before Statement::Execute()
        *
        */
        void SetDirection(BindDirection value);

        /**
        * @brief
        * Get the direction mode
          *
        */
        BindDirection GetDirection() const;

    private:

        BindInfo(OCI_Bind* pBind, core::Handle* parent);
    };

    /**
    * @brief
    * Object used for executing SQL or PL/SQL statement and returning the produced results.
    *
    * This class wraps the OCILIB object handle OCI_Statement and its related methods
    *
    */
    class Statement : public core::HandleHolder<OCI_Statement*>
    {
        friend class Exception;
        friend class Resultset;
        template<class, Int32>
        friend class Long;
        friend class BindInfo;
        friend class BindObject;
        friend class Subscription;

    public:

        /**
        * @brief
        * Statement Type enumerated values
        *
        */
        enum StatementTypeValues
        {
            /** SELECT statement */
            TypeSelect = OCI_CST_SELECT,
            /** UPDATE statement */
            TypeUpdate = OCI_CST_UPDATE,
            /** DELETE statement */
            TypeDelete = OCI_CST_DELETE,
            /** INSERT statement */
            TypeInsert = OCI_CST_INSERT,
            /** CREATE statement */
            TypeCreate = OCI_CST_CREATE,
            /** DROP statement */
            TypeDrop = OCI_CST_DROP,
            /** ALTER statement */
            TypeAlter = OCI_CST_ALTER,
            /** BEGIN statement */
            TypeBegin = OCI_CST_BEGIN,
            /** DECLARE statement */
            TypeDeclare = OCI_CST_DECLARE,
            /** CALL statement */
            TypeCall = OCI_CST_CALL,
            /** MERGE statement */
            TypeMerge = OCI_CST_MERGE
        };

        /**
        * @brief
        * Statement Type
        *
        * Possible values are Statement::StatementTypeValues
        *
        */
        typedef core::Enum<StatementTypeValues> StatementType;

        /**
        * @brief
        * Fetch Modes enumerated values
        *
        */
        enum FetchModeValues
        {
            /** Statement is forward only */
            FetchForward = OCI_SFM_DEFAULT,
            /** Statement is scrollable */
            FetchScrollable = OCI_SFM_SCROLLABLE
        };

        /**
        * @brief
        * Fetch Modes
        *
        * Possible values are Statement::FetchModeValues
        *
        */
        typedef core::Enum<FetchModeValues> FetchMode;

        /**
        * @brief
        * Bind Modes enumerated values
        *
        */
        enum BindModeValues
        {
            /** Bind variables are bound by name */
            BindByPosition = OCI_BIND_BY_POS,
            /** Bind variables are bound by position */
            BindByName = OCI_BIND_BY_NAME
        };

        /**
        * @brief
        * Bind Modes
        *
        * Possible values are Statement::BindModeValues
        *
        */
        typedef core::Enum<BindModeValues> BindMode;

        /**
        * @brief
        * LONG data type mapping modes enumerated values
        *
        */
        enum LongModeValues
        {
            /** LONG and LONG RAW are mapped to Long objects */
            LongExplicit = OCI_LONG_EXPLICIT,
            /** LONG and LONG RAW are mapped to ostring objects */
            LongImplicit = OCI_LONG_IMPLICIT
        };

        /**
        * @brief
        * LONG data type mapping modes
        *
        * Possible values are Statement::LongModeValues
        *
        */
        typedef core::Enum<LongModeValues> LongMode;

        /**
        * @brief
        * Create an empty null Statement instance
        *
        */
        Statement();

        /**
        * @brief
        * Parametrized constructor
        *
        * @param connection - Parent connection
        *
        * @note
        * - The statement object must not be accessed anymore once the parent connection object gets out of scope
        *
        */
        Statement(const Connection& connection);

        /**
        * @brief
        * Return the connection associated with a statement
        *
        */
        Connection GetConnection() const;

        /**
        * @brief
        * Describe the select list of a SQL select statement.
        *
        * @param sql  - SELECT sql statement
        *
        * @note
        * This call sends the SELECT SQL order to the server for retrieving the
        * description of the select order only.
        * The command is not executed.
        * This call is only useful to retrieve information on the associated resultset
        * Call GetResultset() after Describe() to access to SELECT list information
        *
        * @note
        * This call prepares the statement (internal call to Prepare()) and ask
        * the Oracle server to describe the output SELECT list.
        * Execute() can be called after Describe() in order to execute the
        * statement, which means that the server will parse, and describe again the SQL
        * order.
        *
        * @warning
        * Do not use Describe() unless you're only interested in the resultset
        * information because the statement will be parsed again when executed and thus
        * leading to unnecessary server round trips and less performance
        *
        */
        void Describe(const ostring& sql);

        /**
        * @brief
        * Parse a SQL statement or PL/SQL block.
        *
        * @param sql  - SQL order - PL/SQL block
        *
        * @note
        * This call sends the SQL or PL/SQL command to the server for parsing only.
        * The command is not executed.
        * This call is only useful to check is a command is valid or not.
        *
        * @note
        * This call prepares the statement (internal call to Prepare()) and ask
        * the Oracle server to parse its SQL or PL/SQL command.
        * Execute() can be call after Parse() in order to execute the
        * statement, which means that the server will parse again the command.
        *
        * @warning
        * Do not use Parse() unless you're only interested in the parsing result
        * because the statement will be parsed again when executed and thus leading to
        * unnecessary server round trips and less performance
        *
        */
        void Parse(const ostring& sql);

        /**
        * @brief
        * Prepare a SQL statement or PL/SQL block.
        *
        * @param sql  - SQL order or PL/SQL block
        *
        * @note
        * With version 1.3.0 and above, do not call this function for fetched statements (REF cursors)
        *
        */
        void Prepare(const ostring& sql);

        /**
        * @brief
        * Execute a prepared SQL statement or PL/SQL block.
        *
        */
        void ExecutePrepared();

        /**
        * @brief
        * Prepare and execute a SQL statement or PL/SQL block.
        *
        * @param sql  - SQL order - PL/SQL block
        *
        */
        void Execute(const ostring& sql);

        /**
        * @brief
        * Execute a prepared SQL statement or PL/SQL block and return its resultset.
        *
        * @note
        * This call is equivalent to calling ExecutePrepared() followed by GetResultset().
        *
        */
        Resultset ExecuteAndGetResultset();

        /**
        * @brief
        * Prepare and execute a SQL statement or PL/SQL block and return its resultset.
        *
        * @param sql  - SQL order - PL/SQL block
        *
        * @note
        * This call is equivalent to calling Execute() followed by GetResultset().
        *
        */
        Resultset ExecuteAndGetResultset(const ostring& sql);

        /**
        * @brief
        * Execute the prepared statement, retrieve all resultsets, and call the given callback for each row of each resultsets
        *
        * @tparam TFetchCallback -  type of the fetch callback
        *
        * @param callback -  User defined callback
        *
        * @note
        * The user defined callback function must conform to the following prototype:
        * bool callback(const Resultset &)
        * It shall return true to continue fetching the resultset or false to stop the fetch
        *
        * @return
        * The number of rows fetched
        *
        */
        template<class T>
        UInt32 ExecutePrepared(T callback);

        /**
        * @brief
        * Execute the prepared statement, retrieve all resultsets, and call the given callback
        * with adapted type for each row of each resultsets
        *
        * @tparam TAdapter       -  type of the adapter callback
        * @tparam TFetchCallback -  type of the fetch callback
        *
        * @param callback -  User defined callback
        * @param adapter  -  User defined adapter function
        *
        * @note
        * The user defined callback function must conform to the following prototype:
        * bool callback(const Resultset &)
        * It shall return true to continue fetching the resultset or false to stop the fetch
        *
        * @return
        * The number of rows fetched
        *
        */
        template<class T, class U>
        UInt32 ExecutePrepared(T callback, U adapter);

        /**
        * @brief
        * Execute the given SQL statement, retrieve all resultsets, and call the given callback for each row of each resultsets
        *
        * @tparam TFetchCallback -  type of the fetch callback
        *
        * @param sql      - SQL order - PL/SQL block
        * @param callback -  User defined callback
        *
        * @note
        * The user defined callback function must conform to the following prototype:
        * bool callback(const Resultset &)
        * It shall return true to continue fetching the resultset or false to stop the fetch
        *
        * @return
        * The number of rows fetched
        *
        */
        template<class T>
        UInt32 Execute(const ostring& sql, T callback);

        /**
        * @brief
        * Execute the given SQL statement, retrieve all resultsets, and call the given callback
        * with adapted type for each row of each resultsets
        *
        * @tparam TAdapter       -  type of the adapter callback
        * @tparam TFetchCallback -  type of the fetch callback
        *
        * @param sql      - SQL order - PL/SQL block
        * @param callback -  User defined callback
        * @param adapter  -  User defined adapter function
        *
        * @note
        * The user defined callback function must conform to the following prototype:
        * bool callback(const Resultset &)
        * It shall return true to continue fetching the resultset or false to stop the fetch
        *
        * @return
        * The number of rows fetched
        *
        */
        template<class T, class U>
        UInt32 Execute(const ostring& sql, T callback, U adapter);

        /**
        * @brief
        * Return the number of rows affected by the SQL statement
        *
        *
        * The returned value is :
        *  - For UPDATEs : number of rows updated
        *  - For INSERTs : number of rows inserted
        *  - For DELETEs : number of rows deleted
        *
        * @note
        * For SELECTs statements, use GetRowCount() instead
        *
        */
        UInt32 GetAffectedRows() const;

        /**
        * @brief
        * Return the last SQL or PL/SQL statement prepared or executed by the statement
        *
        */
        ostring GetSql()  const;

        /**
        * @brief
        * Return the server SQL_ID of the last SQL or PL/SQL statement prepared or executed by the statement
        *
        */
        ostring GetSqlIdentifier()  const;

        /**
        * @brief
        * Retrieve the resultset from an executed statement
        *
        * @note
        * See @ref OcilibCApiFetching for more details about what statements can return resultsets
        *
        * @warning
        * If the statement has not been prepared and executed, null resultset object will be returned
        *
        */
        Resultset GetResultset();

        /**
        * @brief
        * Retrieve the next available resultset
        *
        * @note
        * it is only valid for the following statements:
        * - Statements executing SQL UPDATE/DELETE using a RETURNING INTO clause
        * - Statements implicitly returned from PL/SQL procedure or blocks (new feature in Oracle 12cR1) using
        *   DBMS_SQL.RETURN_RESULT()
        *
        * @note
        * SQL statements with a 'returning' clause can return multiple resultsets.
        * When arrays of program variables are bound to the statement, Oracle will
        * execute the statement for every row (iteration).
        * Each iteration generates a resultset that can be fetched like regular ones.
        *
        * @note
        * Starting with Oracle 12cR1, PL/SQL procedure and blocks can return multiple implicit resultsets
        * Refer to  Oracle documentation for more information.
        *
        */
        Resultset GetNextResultset();

        /**
        * @brief
        * Set the input array size for bulk operations
        *
        * @param size - Array size
        *
        * @warning
        * Do not use SetBindArraySize() for PL/SQL tables binding
        *
        * @note
        * SetBindArraySize() is used to set the size of input bind array when using
        * arrays for DML statements.
        * SetBindArraySize() MUST be called to set the maximum size of the arrays
        * to bind to the statement before any of its execution. This initial call must
        * be bone AFTER OPrepare() and BEFORE any Bind() call taking vectors as parameter.
        *
        * @note
        * SetBindArraySize() can optionally be called before any later Execute()
        * call in order to notify the statement of the exact number of elements
        * populating the input arrays for the next execution. The array size passed to
        * later SetBindArraySize() calls cannot be greater than the initial size
        * otherwise an exception will be thrown.
        *
        */
        void SetBindArraySize(UInt32 size);

        /**
        * @brief
        * Return the current input array size for bulk operations
        *
        */
        UInt32 GetBindArraySize() const;

        /**
        * @brief
        * Allow different host variables to be bound using the same bind name or
        * position between executions of a prepared statement
        *
        * @param value - Rebinding mode allowed
        *
        * @note
        * Default value is false
        *
        * @warning
        * When using rebinding feature, host variable re-bound to a previously allocated
        * bind MUST be of the same data type !
        *
        */
        void AllowRebinding(bool value);

        /**
        * @brief
        * Indicate if rebinding is allowed on the statement
        *
        * @note
        * See AllowRebinding() for more details
        *
        */
        bool IsRebindingAllowed() const;

        /**
        * @brief
        * Return the index of the bind from its name belonging to the statement
        *
        * @param name - Bind variable name
        *
        * @warning
        * The bind name is case insensitive
        *
        * @note
        * Bind indexes start with 1 in OCILIB
        *
        * @return
        * Bind index on success or zero if the bind does not exists
        *
        */
        UInt32 GetBindIndex(const ostring& name) const;

        /**
        * @brief
        * Return the number of binds currently associated to a statement
        *
        */
        UInt32 GetBindCount() const;

        /**
        * @brief
        * Return the bind at the given index in the internal array of bind objects
        *
        * @param index - Bind position
        *
        * @warning
        * Index starts at 1.
        *
        * @note
        * Bind handle are created sequentially. For example, the third call to a
        * Bind() call generates a bind object of index 3.
        *
        */
        BindInfo GetBind(UInt32 index) const;

        /**
        * @brief
        * Return a bind handle from its name
        *
        * @param name - Bind variable name
        *
        * @note
        * Bind names must include a semicolon at the beginning.
        *
        */
        BindInfo GetBind(const ostring& name) const;

        /**
        * @brief
        * Bind an host variable
        *
        * @tparam T - C++ type of the host variable
        *
        * @param name  - Bind name
        * @param value - Host variable
        * @param mode  - bind direction mode
        *
        * @warning
        * This method has built-in specialized versions for all supported types except ostring, Clong and Blong variables.
        * For binding ostring, Clong and Blong variables, use the version with an extra parameter.
        *
        * @note
        * It is not necessary to specify the template data type in the bind call as all possible specializations can be resolved
        * automatically from the arguments.
        *
        */
        template<class T>
        BindInfo Bind(const ostring& name, T& value, BindInfo::BindDirection mode);

        /**
        * @brief
        * Bind an host variable with more information
        *
        * @tparam T  - C++ type of the host variable
        * @tparam TExtraInfo - C++ type if the extra information needed for the bind call
        *
        * @param name      - Bind name
        * @param value     - Host variable
        * @param extraInfo - Extra information needed for the bind call
        * @param mode      - bind direction mode
        *
        * @warning
        * This method has built-in specialized versions for ostring, Clong and Blong variables.
        * Pass the maximum length/size of the variable in the parameter extraInfo
        *
        * @note
        * It is not necessary to specify the template data type in the bind call as all possible specializations can be resolved
        * automatically from the arguments.
        *
        */
        template<class T, class U>
        BindInfo Bind(const ostring& name, T& value, U extraInfo, BindInfo::BindDirection mode);

        /**
        * @brief
        * Bind a host collection variable
        *
        * @tparam T - C++ type of the host collection variable
        *
        * @param name  - Bind name
        * @param value - Host Collection variable
        * @param mode  - bind direction mode
        *
        */
        template<class T>
        BindInfo Bind(const ostring& name, Collection<T>& value, BindInfo::BindDirection mode);

        /**
        * @brief
        * Bind a vector of host variables
        *
        * @tparam T - C++ type of the host variable
        *
        * @param name   - Bind name
        * @param values - Vector of host variables
        * @param mode   - bind direction mode
        * @param type   - vector type (regular array or PL/SQL table)
        *
        * @warning
        * This method has built-in specialized versions for all C++ native scalar types, Date time and Statement objects.
        * For others types (ostring, Clong, Blong, Raw, Object, Reference, Collection, Timestamp, Interval), use versions with extra parameters.
        *
        * @note
        * It is not necessary to specify the template data type in the bind call as all possible specializations can be resolved
        * automatically from the arguments.
        *
        */
        template<class T>
        BindInfo Bind(const ostring& name, std::vector<T>& values, BindInfo::BindDirection mode, BindInfo::VectorType type = BindInfo::AsArray);

        /**
        * @brief
        * Bind a vector of host variables with Oracle type information
        *
        * @tparam T  - C++ type of the host variable
        *
        * @param name     - Bind name
        * @param values   - Vector of host variables
        * @param typeInfo - Object type information
        * @param mode     - bind direction mode
        * @param type     - vector type (regular array or PL/SQL table)
        *
        * @warning
        * This method has built-in specialized versions for Object, Reference.
        *
        * @note
        * It is not necessary to specify the template data type in the bind call as all possible specializations can be resolved
        * automatically from the arguments.
        *
        */
        template<class T>
        BindInfo Bind(const ostring& name, std::vector<T>& values, TypeInfo& typeInfo, BindInfo::BindDirection mode, BindInfo::VectorType type = BindInfo::AsArray);

        /**
        * @brief
        * Bind a vector of host collection variables with Oracle type information
        *
        * @tparam T  - collection C++ type of the host collection variables
        *
        * @param name     - Bind name
        * @param values   - Vector of host collection variables
        * @param typeInfo - Object type information
        * @param mode     - bind direction mode
        * @param type     - vector type (regular array or PL/SQL table)
        *
        */
        template<class T>
        BindInfo Bind(const ostring& name, std::vector<Collection<T> >& values, TypeInfo& typeInfo, BindInfo::BindDirection mode, BindInfo::VectorType type = BindInfo::AsArray);

        /**
        * @brief
        * Bind a vector of host variables with more information
        *
        * @tparam T  - C++ type of the host variable
        * @tparam TExtraInfo - C++ type if the extra information needed for the bind call
        *
        * @param name      - Bind name
        * @param values    - Vector of host variables
        * @param extraInfo - Extra information needed for the bind call
        * @param mode      - bind direction mode
        * @param type      - vector type (regular array or PL/SQL table)
        *
        * @warning
        * This method has built-in specialized versions for ostring, Raw , Clong, Blong, Timestamp, Interval variables.
        * - For ostring, Clong, Blong, Raw : Pass the maximum length/size of variables in the parameter extraInfo
        * - For Timestamp, Interval : Pass a value of the matching C++ class GetType() property type OR the underlying enumeration type.
        *
        * @note
        * It is not necessary to specify the template data type in the bind call as all possible specializations can be resolved
        * automatically from the arguments.
        *
        */
        template<class T, class U>
        BindInfo Bind(const ostring& name, std::vector<T>& values, U extraInfo, BindInfo::BindDirection mode, BindInfo::VectorType type = BindInfo::AsArray);

        /**
        * @brief
        * Register a host variable as an output for a column present in a SQL RETURNING INTO  clause
        *
        * @tparam T - C++ type of the host variable
        *
        * @param name  - Bind name
        *
        * @warning
        * This method has built-in specialized versions for all supported types except:
        * - Timestamp and Interval, Object and Reference : use the version that takes a TypeInfo parameter
        * - ostring and Raw : use the version that takes an extraInfo parameter
        *
        * @note
        * Statement, Blong and Clong are not supported for register calls
        *
        * @warning
        * It is necessary to specify the template data type in the register call
        *
        */
        template<class T>
        void Register(const ostring& name);

        /**
        * @brief
        * Register a host variable with Oracle type information as an output for a column present in a SQL RETURNING INTO  clause
        *
        * @tparam T  - C++ type of the host variable
        * @tparam TExtraInfo - C++ type if the extra information needed for the bind call
        *
        * @param name     - Bind name
        * @param extraInfo - Extra information needed for the bind call
        *
        * @warning
        * This method has built-in specialized versions for Object and Reference.
        *
        * @warning
        * It is necessary to specify the template data type in the register call
        *
        */
        template<class T, class U>
        void Register(const ostring& name, U& extraInfo);

        /**
        * @brief
        * Register a host variable with more information as an output for a column present in a SQL RETURNING INTO  clause
        *
        * @tparam T  - C++ type of the host variable
        * @tparam TExtraInfo - C++ type if the extra information needed for the bind call
        *
        * @param name      - Bind name
        * @param extraInfo - Extra information needed for the bind call
        *
        * @warning
        * This method has built-in specialized versions for ostring and Raw variables.
        *  Pass the maximum length/size of variables in the parameter extraInfo
        *
        * @note
        * It is necessary to specify the template data type in the register call
        *
        */
        template<class T, class U>
        void Register(const ostring& name, U extraInfo);

        /**
        * @brief
        * Return the type of a SQL statement
        *
        */
        StatementType GetStatementType() const;

        /**
        * @brief
        * Return the error position (in terms of characters) in the SQL statement
        * where the error occurred in case of SQL parsing error
        *
        * @note
        * Positions start at 1.
        *
        */
        UInt32 GetSqlErrorPos() const;

        /**
        * @brief
        * Set the fetch mode of a SQL statement
        *
        * @param value - fetch mode value
        *
        * @warning
        * SetFetchMode() MUST be called before any Execute() call
        *
        */
        void SetFetchMode(FetchMode value);

        /**
        * @brief
        * Return the fetch mode of a SQL statement
        *
        * @note
        * Default value is Statement::FetchForward
        *
        */
        FetchMode GetFetchMode() const;

        /**
        * @brief
        * Set the binding mode of a SQL statement
        *
        * @param value - binding mode value
        *
        */
        void SetBindMode(BindMode value);

        /**
        * @brief
        * Return the binding mode of a SQL statement
        *
        * @note
        * Default value is Statement::BindByName
        *
        */
        BindMode GetBindMode() const;

        /**
        * @brief
        * Set the number of rows fetched per internal server fetch call
        *
        * @param value - number of rows to fetch
        *
        */
        void SetFetchSize(UInt32 value);

        /**
        * @brief
        * Return the number of rows fetched per internal server fetch call
        *
        * @note
        * Default value is set to constant OCI_FETCH_SIZE
        *
        */
        UInt32 GetFetchSize() const;

        /**
        * @brief
        * Set the number of rows pre-fetched by OCI Client
        *
        * @param value - number of rows to pre-fetch
        *
        * @note
        * To turn off pre-fetching, set both attributes (size and memory) to 0.
        *
        */
        void SetPrefetchSize(UInt32 value);

        /**
        * @brief
        * Return the number of rows pre-fetched by OCI Client
        *
        * @note
        * Default value is set to constant OCI_PREFETCH_SIZE
        *
        */
        UInt32 GetPrefetchSize() const;

        /**
        * @brief
        * Set the amount of memory pre-fetched by OCI Client
        *
        * @param value - amount of memory to fetch
        *
        * @note
        * Default value is 0 and the pre-fetch size attribute is used instead.
        * When both attributes are set (pre-fetch size and memory) and pre-fetch memory
        * value can hold more rows than specified by pre-fetch size, OCI uses pre-fetch
        * size instead.
        *
        * @note
        * OCILIB set pre-fetch attribute to OCI_PREFETCH_SIZE when a statement is created.
        * To setup a big value for SetPrefetchMemory(), you must call
        * SetPrefetchSize() to 0 to make OCI consider this attribute.
        *
        */
        void SetPrefetchMemory(UInt32 value);

        /**
        * @brief
        * Return the amount of memory used to retrieve rows pre-fetched by OCI Client
        *
        * @note
        * Default value is 0
        *
        */
        UInt32 GetPrefetchMemory() const;

        /**
        * @brief
        * Set the piece size for dynamic fetch operations (XMLTYPE, LONGs)
        *
        * @param value - maximum size for long buffer
        *
        */
        void SetPieceSize(UInt32 value);

        /**
        * @brief
        * Return the piece size for dynamic fetch operations (XMLTYPE, LONGs)
        *
        * @note
        * Default value is set to constant OCI_SIZE_PIECE_DYNAMIC_FETCH
        *
        */
        UInt32 GetPieceSize() const;

        /**
        * @brief
        * Set the long data type handling mode of a SQL statement
        *
        * @param value - long mode value
        *
        * @note
        * LONG RAWs can't be handled with Statement::LongImplicit mode
        *
        */
        void SetLongMode(LongMode value);

        /**
        * @brief
        * Return the long data type handling mode of a SQL statement
        *
        */
        LongMode GetLongMode() const;

        /**
        * @brief
        * Return the Oracle SQL code the command held by the statement
        *
        * @warning
        * GetSQLCommand() must be called after the statement has be executed
        * because that's the server engine that computes the SQL command code
        *
        * @note
        * The SQL command list is available in Oracle documentations and guides
        *
        */
        UInt32 GetSQLCommand() const;

        /**
        * @brief
        * Return the verb of the SQL command held by the statement
        *
        * @warning
        * GetSQLVerb() must be called after the statement has been executed
        * because that's the server engine that computes the SQL verb
        *
        * @note
        * The SQL verb list is available in Oracle documentations and guides
        *
        */
        ostring GetSQLVerb() const;

        /**
        * @brief
        * Return the list of parsed bind names after an Prepare() Parse() call.
        *
        * @warning
        * Returned names are upper case and do not contain leading ':' character.
        *
        * @warning
        * GetParseBindNames() returns information computed by OCI client library, not by OCILIB.
        * Result is uncorrelated to any bind operation.
        *
        * @note
        * List of parsed bind on success or empty list if no bind found
        *
        */

        std::vector<ostring> GetParseBindNames() const;

        /**
        * @brief
        * Returns all errors that occurred within a DML array statement execution
        *
        */
        void GetBatchErrors(std::vector<Exception>& exceptions);

    private:

        static void OnFreeSmartHandle(core::Handle* smartHandle);

        Statement(OCI_Statement* stmt, core::Handle* parent = nullptr);

        support::BindsHolder* GetBindsHolder(bool create) const;

        void ReleaseResultsets() const;

        void SetInData() const;
        void SetOutData() const;
        void ClearBinds() const;

        template<typename M, class T>
        BindInfo Bind1(M& method, const ostring& name, T& value, BindInfo::BindDirection mode);

        template<typename M, class T>
        BindInfo Bind2(M& method, const ostring& name, T& value, BindInfo::BindDirection mode);

        template<typename M, class T>
        BindInfo BindVector1(M& method, const ostring& name, std::vector<T>& values, BindInfo::BindDirection mode, BindInfo::VectorType type);

        template<typename M, class T, class U>
        BindInfo BindVector2(M& method, const ostring& name, std::vector<T>& values, BindInfo::BindDirection mode, U subType, BindInfo::VectorType type);

        template<typename T>
        UInt32 Fetch(T callback);

        template<typename T, typename U>
        UInt32 Fetch(T callback, U adapter);
    };

    /**
     * @brief
     * Database resultset
     *
     * This class wraps the OCILIB object handle OCI_Resultset and its related methods
     *
     */
    class Resultset : public core::HandleHolder<OCI_Resultset*>
    {
        friend class Statement;
    public:

        /**
        * @brief
        * Seek Modes enumerated values
        *
        */
        enum SeekModeValues
        {
            /** Seek is performed using a given absolute offset. The statement must be scrollable */
            SeekAbsolute = OCI_SFD_ABSOLUTE,
            /** Seek is performed using a given relative offset from the current position. The statement must be scrollable */
            SeekRelative = OCI_SFD_RELATIVE
        };

        /**
        * @brief
        * Seek Modes
        *
        * Possible values are Resultset::SeekModeValues
        *
        */
        typedef core::Enum<SeekModeValues> SeekMode;
 
        /**
        * @brief
        * Default constructor
        *
        * @note
        * The resultset is not usable until it is assigned from a call to
        * Statement::GetResultset() or Statement::ExecuteAndGetResultset().
        * Call IsNull() to check whether it wraps a valid handle.
        *
        */
        Resultset();

        /**
        * @brief
        * Return the current value of the column at the given index in the resultset
        *
        * @tparam T - C++ type of the value to retrieve
        *
        * @param index - Column position
        *
        * @note
        * Column position starts at 1.
        *
        * @note
        * Get<ostring> performs an implicit text conversion for non-string columns
        * on every call, for every fetched row. On performance-critical fetch loops,
        * prefer the typed overloads and convert to text only where needed. The
        * produced text also depends on the session connection formats (see
        * Environment::SetFormat / Environment::GetFormat) - it is not a stable
        * serialized form.
        *
        */
        template<class T>
        T Get(UInt32 index) const;

        /**
        * @brief
        * Assign to the current value of the column at the given index in the resultset
        *
        * @tparam T - C++ type of the value to retrieve
        *
        * @param index - Column position
        * @param value - value to fill
        *
        * @note
        * Column position starts at 1.
        *
        */
        template<class T>
        void Get(UInt32 index, T& value) const;

        /**
        * @brief
        * Return the current value of the column from its name in the resultset
        *
        * @tparam T - C++ type of the value to retrieve
        *
        * @param name  - Column name
        *
        * @note
        * The column name is case insensitive.
        *
        */
        template<class T>
        T Get(const ostring& name) const;

        /**
        * @brief
        * Assign to the current value of the column from its name in the resultset
        *
        * @tparam T - C++ type of the value to retrieve
        *
        * @param name - Column name
        * @param value - value to fill
        *
        */
        template<class T>
        void Get(const ostring& name, T& value) const;

        /**
        * @brief
        * Return a given user type from the current fetched row.
        * It allow translating row column values returned by a query to a user defined type using the given adapter type
        *
         * @param value    - User defined type value to be filled by the adapter function
        * @param adapter -  User defined adapter function
        *
        * @note
        * The user defined adapter function must conform to the following prototype:
        * bool adapter (const Resultset &, T &)
        * It shall return true if it has filled the value from the resultset otherwise false
        *
        */
        template<class T, class U>
        bool Get(T& value, U adapter) const;

        /**
        * @brief
        * Fetch all rows in the resultset and call the given callback for row
        *
        * @param callback -  User defined callback
        *
        * @note
        * The user defined callback function must conform to the following prototype:
        * bool callback(const Resultset &)
        * It shall return true to continue fetching the resultset or false to stop the fetch
        *
        * @return
        * The number of rows fetched
        *
        */
        template<typename T>
        UInt32 ForEach(T callback);

        /**
        * @brief
        * Fetch all rows in the resultset and call the given callback for row
        * It allow translating row column values returned by a query to a user defined type using the given adapter type
        *
        * @param callback -  User defined callback
        * @param adapter  -  User defined adapter function
        *
         * @note
        * The user defined adapter function must conform to the following prototype:
        * [UserDefinedType] adapter (const Resultset &)
        * The return value type must match the parameter of the callback method
        *
        * @note
        * The user defined callback function must conform to the following prototype:
        * bool callback(const [UserDefinedType] &)
        * It shall return true to continue fetching the resultset or false to stop the fetch
        *
        * @return
        * The number of rows fetched
        *
        */
        template<typename T, typename U>
        UInt32 ForEach(T callback, U adapter);

        /**
        * @brief
        * Fetch the next row of the resultset
        *
        * @note
        * Next() works for normal and scrollable resultsets
        *
        * @return
        * true on success otherwise false if :
        * - Empty resultset
        * - Last row already fetched
        *
        */
        bool Next();

        /**
        * @brief
        * Fetch the previous row of the resultset
        *
        * @note
        * Prev() works ONLY for scrollable resultsets
        *
        * @return
        * true on success otherwise false if :
        * - Empty resultset
        * - First row already fetched
        *
        */
        bool Prev();

        /**
        * @brief
        * Fetch the first row of the resultset
        *
        * @note
        * First() works ONLY for scrollable resultsets
        *
        * @return
        * true on success otherwise false if  the resultset is empty
        *
        */
        bool First();

        /**
        * @brief
        * Fetch the last row of the resultset
        *
        * @note
        * Last() works ONLY for scrollable resultsets
        *
        * @return
        * true on success otherwise false if  the resultset is empty
        *
        */
        bool Last();

        /**
        * @brief
        * Custom Fetch of the resultset
        *
        * @param mode    - Fetch direction
        * @param offset  - Fetch offset
        *
        * @warning
        * Seek() works ONLY for scrollable resultsets
        *
        * @note
        * If you intend to use Seek() on a scrollable statement and if any of the
        * selected columns is a ref cursor or a nested table, you must set the fetching size
        * to 1 using Statement::SetFetchSize() before calling Statement::GetResultset()
        * Otherwise Seek() will fails with a OCI-10002 error
        *
        * @return
        * true on success otherwise false the resultset is empty or seek offset out of bounds
        *
        */
        bool Seek(SeekMode mode, Int32 offset);

        /**
        * @brief
        * Retrieve the number of rows fetched so far
        *
        */
        UInt32 GetCount() const;

        /**
        * @brief
        * Retrieve the current row index
        *
        * @note
        * - GetCurrentRow() returns the current row number starting from 1
        * - If the resultset has not been fetched or if the resultset is empty, it returns 0
        * - If the resultset has been fully fetched, it returns the last fetched row number
        *
        */
        UInt32 GetCurrentRow() const;

        /**
        * @brief
        * Return the index of the column in the result from its name
        *
        * @param name  - Column name
        *
        * @warning
        * The column name is case insensitive
        *
        * @note
        * Column position starts at 1.
        *
        */
        UInt32 GetColumnIndex(const ostring& name) const;

        /**
        * @brief
        * Return the number of columns in the resultset
        *
        */
        UInt32 GetColumnCount() const;

        /**
        * @brief
        * Return the column from its index in the resultset
        *
        * @param index  - Column index
        *
        * @note
        * Column position starts at 1.
        *
        */
        Column GetColumn(UInt32 index) const;

        /**
        * @brief
        * Return the column from its name in the resultset
        *
        * @param name  - Column name
        *
        * @note
        * The column name is case insensitive
        *
        */
        Column GetColumn(const ostring& name) const;

        /**
        * @brief
        * Check if the current row value is null for the column at the given index
        *
        * @param index  - Column index
        *
        * @note
        * Column position starts at 1.
        *
        */
        bool IsColumnNull(UInt32 index) const;

        /**
        * @brief
        * Check if the current row value is null for the column of the given name
        *
        * @param name  - Column name
        *
        */
        bool IsColumnNull(const ostring& name) const;

        /**
        * @brief
        * Return the statement associated with the resultset
        *
        */
        Statement GetStatement() const;

        /**
        * @brief
        * Convenient operator overloading that performs a call to Next()
        *
        */
        bool operator ++ (Int32);

        /**
        * @brief
        * Convenient operator overloading that performs a call to Prev()
        *
        */
        bool operator -- (Int32);

        /**
        * @brief
        * Convenient operator overloading that performs a call to Seek()
        * with Resultset::SeekRelative and the given offset
        *
        */
        bool operator += (Int32 offset);

        /**
        * @brief
        * Convenient operator overloading that performs a call to Seek()
        * with Resultset::SeekRelative and the given offset that is internally negated
        *
        */
        bool operator -= (Int32 offset);

    private:

        Resultset(OCI_Resultset* resultset, core::Handle* parent);
    };

    /**
     * @brief
     * Encapsulate a Resultset column or object member properties
     *
     * This class wraps the OCILIB object handle OCI_Column and its related methods
     *
     */
    class Column : public core::HandleHolder<OCI_Column*>
    {
        friend class TypeInfo;
        friend class Resultset;

    public:

        /**
        * @brief
        * Column properties flags values
        *
        */
        enum PropertyFlagsValues
        {
            /** The column has no flags or the OCI client does not support it */
            NoFlags = OCI_CPF_NONE,
            /** - If Set, the column is an IDENTITY column
            - Otherwise, it is not an IDENTITY column */
            IsIdentity = OCI_CPF_IS_IDENTITY,
            /** Only valid when IsIdentity is set:
            - If set, means that the value is "ALWAYS GENERATED"
            - Otherwise means that the value is "GENERATED BY" */
            IsGeneratedAlways = OCI_CPF_IS_GEN_ALWAYS,
            /** Only valid when IsIdentity is set:
            - If set, means that the value is generated by default on NULL */
            IsGeneratedByDefaultOnNull = OCI_CPF_IS_GEN_BY_DEFAULT_ON_NULL,
            /**  If set, Column is an implicitly generated logical partitioning column for container_map enabled object */
            IsLogicalPartitioning = OCI_CPF_IS_LPART,
            /**  If set, Column is a CON_ID column implicitly generated by CONTAINERS() or is an ORIGIN_CON_ID column implicitly generated for Extended Data Link */
            IsGeneratedByContainers = OCI_CPF_IS_CONID
        };

        /**
        * @brief
        * Column properties flags
        *
        * Possible values are Column::PropertyFlagsValues
        *
        */
        typedef core::Flags<PropertyFlagsValues> PropertyFlags;

        /**
        * @brief
        * Return the Column name
        *
        */
        ostring GetName() const;

        /**
        * @brief
        * Return the Oracle SQL type name of the column data type
        *
        * @note
        * For possible values, consults Oracle Documentation
        *
        */
        ostring GetSQLType() const;

        /**
        * @brief
        * Return the Oracle SQL Full name including precision and size of the
        * column data type
        *
        * @note
        * This function returns a description that matches the one given by SQL*Plus
        *
        */
        ostring GetFullSQLType() const;

        /**
        * @brief
        * Return the type of the given column
        *
        */
        DataType GetType() const;

        /**
        * @brief
        * Return the OCILIB object subtype of a column
        *
        * @note
        * This call is valid for the following OCILIB types:
        * - Clong and Blong
        * - Clob, NClob and Blob
        * - File
        * - Timestamp
        * - Interval
        *
        * @warning
        * - Cast returned values to the matching C++ class GetType() property type.
        * - For a non valid type, it returns 0.
        *
        */
        UInt32 GetSubType() const;

        /**
        * @brief
        * Return the charset form of the given column
        *
        */
        CharsetForm GetCharsetForm() const;

        /**
        * @brief
        * Return the collation ID of the given column
        *
        */
        CollationID GetCollationID() const;

        /**
        * @brief
        * Return the size of the column
        *
        * @note
        * For all types, the size is expressed is bytes, excepted for character
        * based columns that were created with a character based size or of type NCHAR/NVARCHAR
        *
        */
        UInt32 GetSize() const;

        /**
        * @brief
        * Return the scale of the column for numeric columns
        *
        */
        Int32 GetScale() const;

        /**
        * @brief
        * Return the precision of the column for numeric columns
        *
        */
        Int32 GetPrecision() const;

        /**
        * @brief
        * Return the fractional precision of the column for Timestamp and Interval columns
        *
        */
        Int32 GetFractionalPrecision() const;

        /**
        * @brief
        * Return the leading precision of the column for Interval columns
        *
        */
        Int32 GetLeadingPrecision() const;


        /**
        * @brief
        * Return the dimension of the column for Vector columns
        *
        */
        Int32 GetDimension() const;

        /**
        * @brief
        * Return the column property flags
        *
        * @note
        * This was introduced in Oracle 12cR1.
        * It is currently used for identifying Identity columns.
        * For earlier versions, it always return Columns::NoFlags
        *
        */
        PropertyFlags GetPropertyFlags() const;

        /**
        * @brief
        * Return true if the column is nullable otherwise false
        *
        */
        bool IsNullable() const;

        /**
        * @brief
        * Return true if the length of the column is character-length or  false if it is byte-length
        *
        * @note
        * This was introduced in Oracle 9i. So for version that are not supporting this
        * property, it always return false
        *
        */
        bool IsCharSemanticUsed() const;

        /**
        * @brief
        * Return the type information object associated to the column
        *
        * @note
        * This call is used only for Named Object typed and collection columns.
        * It returns a null object if the column is not a Named Object or a collection.
        *
        */
        TypeInfo GetTypeInfo() const;

    private:

        Column(OCI_Column* pColumn, core::Handle* parent);
    };

    /**
    * @brief
    * Subscription to database or objects changes
    *
    * This class wraps the OCILIB object handle OCI_Subscription and its related methods
    *
    * @warning
    * Environment::Events flag must be passed to the Environment constructor to be able to use subscriptions

    */
    class Subscription : public core::HandleHolder<OCI_Subscription*>
    {
        friend class Event;

    public:

        /**
         * @typedef NotifyHandlerProc
         *
         * @brief
         * User callback for subscriptions event notifications
         *
         */
        typedef void (*NotifyHandlerProc) (Event& evt);

        /**
        * @brief
        * Subscription changes flags values
        *
        */
        enum ChangeTypesValues
        {
            /**  Request for changes at objects (e.g. tables) level (DDL / DML) */
            ObjectChanges = OCI_CNT_OBJECTS,
            /**  Request for changes at rows level (DML) */
            RowChanges = OCI_CNT_ROWS,
            /**  Request for changes at database level (startup, shutdown) */
            DatabaseChanges = OCI_CNT_DATABASES,
            /**  Request for all changes */
            AllChanges = OCI_CNT_ALL
        };

        /**
        * @brief
        * Subscription changes flags
        *
        * Possible values are Subscription::ChangeTypesValues
        *
        */
        typedef core::Flags<ChangeTypesValues> ChangeTypes;

        /**
        * @brief
        * Subscription registration mode values
        *
        */
        enum ModeValues
        {
            /** Server initiates the notification listener connection */
            ModeDefault = OCI_SRM_DEFAULT,
            /** Client initiates the notification listener connection (Oracle client 12c and above) */
            ModeSecureNotification = OCI_SRM_SECURE_NOTIFICATION
        };

        /**
        * @brief
        * Subscription registration mode
        *
        * Possible values are Subscription::ModeValues
        *
        */
        typedef core::Enum<ModeValues> SubscriptionMode;

        /**
        * @brief
        * Default constructor
        *
        */
        Subscription();

        /**
        * @brief
        * Register a notification against the given database
        *
        * @param connection  - Connection handle
        * @param name        - Notification name
        * @param changeTypes - Subscription type
        * @param handler     - User handler callback
        * @param timeout     - notification timeout
        *
        * @note
        * The notification port is configured on the environment by calling
        * Environment::SetSubscriptionPort() before the first subscription is registered.
        *
        * @note
        * Requires Oracle Client 10gR2 or above
        *
        */
        void Register(const Connection& connection, const ostring& name, ChangeTypes changeTypes, NotifyHandlerProc handler, SubscriptionMode mode = ModeValues::ModeDefault, UInt32 timeout = 0);

        /**
        * @brief
        * Unregister a previously registered notification
        *
        * @note
        * Environment::Cleanup() will automatically unregister any non unregistered subscriptions
        *
        * @note
        * If the database connection passed to Register()
        * has been closed by the time that the application calls
        * Unregister(), the library internally reconnects
        * to the given database, performs the unregistration and then disconnects
        *
        */
        void Unregister();

        /**
        * @brief
        * Add a SQL query to monitor
        *
        */
        void Watch(const ostring& sql);

        /**
        * @brief
        * Return the name of the given registered subscription
        *
        */
        ostring GetName() const;

        /**
        * @brief
        * Return the timeout of the given registered subscription
        *
        */
        UInt32 GetTimeout() const;

        /**
        * @brief
        * Return the connection  associated with a subscription handle
        *
        * @note
        * It may return a null connection object if the connection used at Register() time has been closed
        *
        */
        Connection GetConnection() const;

    private:

        Subscription(OCI_Subscription* pSubscription);
    };

    /**
    * @brief
    * Subscription Event
    *
    * This class wraps the OCILIB object handle OCI_Event and its related methods
    *
    * @warning
    * Environment::Events flag must be passed to the Environment constructor to be able to use subscriptions
    *
    */
    class Event : public core::HandleHolder<OCI_Event*>
    {
        friend class Subscription;
        friend class Environment;

    public:

        /**
        * @brief
        * Event type enumerated values
        *
        */
        enum EventTypeValues
        {
            /** A database has been started up */
            DatabaseStart = OCI_ENT_STARTUP,
            /** A database has been shut down */
            DatabaseShutdown = OCI_ENT_SHUTDOWN,
            /** A database has been shut down (RAC) */
            DatabaseShutdownAny = OCI_ENT_SHUTDOWN_ANY,
            /** A database has been dropped */
            DatabaseDrop = OCI_ENT_DROP_DATABASE,
            /** The notification is timed out */
            Unregister = OCI_ENT_DEREGISTER,
            /** A database object has been modified */
            ObjectChanged = OCI_ENT_OBJECT_CHANGED
        };

        /**
        * @brief
        * Event type
        *
        * Possible values are Event::EventTypeValues
        *
        */
        typedef core::Enum<EventTypeValues> EventType;

        /**
        * @brief
        * Object events enumerated values
        *
        */
        enum ObjectEventValues
        {
            /** An insert has been performed */
            ObjectInserted = OCI_ONT_INSERT,
            /** An update has been performed */
            ObjectUpdated = OCI_ONT_UPDATE,
            /** A delete has been performed */
            ObjectDeleted = OCI_ONT_DELETE,
            /** An alter has been performed */
            ObjectAltered = OCI_ONT_ALTER,
            /** A drop has been performed */
            ObjectDropped = OCI_ONT_DROP,
            /** Generic undefined action * */
            ObjectGeneric = OCI_ONT_GENERIC
        };

        /**
        * @brief
        * Object events
        *
        * Possible values are Event::ObjectEventValues
        *
        */
        typedef core::Enum<ObjectEventValues> ObjectEvent;

        /**
        * @brief
        * Return the type of event reported by a notification
        *
        */
        EventType GetType() const;

        /**
        * @brief
        * Return the type of operation reported by a notification

        * @note
        * This call is only valid when GetType() reports the event type Event::ObjectChanged
        *
        */
        ObjectEvent GetObjectEvent() const;

        /**
        * @brief
        * Return the name of the database that generated the event
        *
        */
        ostring GetDatabaseName() const;

        /**
        * @brief
        * Return the name of the object that generated the event
        *
        * @note
        * Database object name follows the pattern "[schema_name].object_name"
        *
        */
        ostring GetObjectName() const;

        /**
        * @brief
        * Return the rowid of the altered database object row
        *
        */
        ostring GetRowID() const;

        /**
        * @brief
        * Return the subscription that generated this event
        *
        */
        Subscription GetSubscription() const;

    private:

        Event(OCI_Event* pEvent);
    };

    /**
    * @brief
    * AQ identified agent for messages delivery
    *
    * This class wraps the OCILIB object handle OCI_Agent and its related methods
    *
    */
    class Agent : public core::HandleHolder<OCI_Agent*>
    {
        friend class Message;
        friend class Dequeue;

    public:

        /**
        * @brief
        * Create an AQ agent object
        *
        * @param connection - Connection object
        * @param name       - Agent name
        * @param address    - Agent address
        *
        * @note
        * An AQ agent object is :
        * - used as recipient information when enqueuing a message
        * - used as sender information when dequeuing a message
        * - used for listening message only from identified senders
        *
        * @note
        * the AQ agent address can be any Oracle identifier, up to 128 bytes.
        * the AQ agent name    can be any Oracle identifier, up to 30  bytes.
        *
        * @note
        * the Agent object must not be accessed anymore once the parent connection object gets out of scope
        *
        */
        Agent(const Connection& connection, const ostring& name = OTEXT(""), const ostring& address = OTEXT(""));

        /**
        * @brief
        * Get the given AQ agent name
        *
        */
        ostring GetName() const;

        /**
        * @brief
        * Set the given AQ agent name
        *
        * @param value  - AQ agent name
        *
        * @note
        * the AQ agent name is used to identified an message send or recipient when enqueuing/dequeuing a message
        *
        * @note
        * the AQ agent name can be any Oracle identifier, up to 30 bytes.
        *
        */
        void SetName(const ostring& value);

        /**
        * @brief
        * Get the given AQ agent address
        *
        * @note
        * See SetAddress() for more details
        *
        */
        ostring GetAddress() const;

        /**
        * @brief
        * Set the given AQ agent address
        *
        * @param value - AQ agent address
        *
        * @note
        * the parameter 'address' must be of the form : [schema.]queue_name[\@dblink]
        *
        * @note
        * the AQ agent address can be any Oracle identifier, up to 128 bytes.
        *
        */
        void SetAddress(const ostring& value);

    private:

        Agent(OCI_Agent* pAgent, core::Handle* parent);
    };

    /**
    * @brief
    * AQ message
    *
    * This class wraps the OCILIB object handle OCI_Msg and its related methods
    *
    */
    class Message : public core::HandleHolder<OCI_Msg*>
    {
        friend class Dequeue;

    public:

        /**
        * @brief
        * Message state enumerated values
        *
        */
        enum MessageStateValues
        {
            /** The message is ready to be processed */
            Ready = OCI_AMS_READY,
            /** The message delay has not yet completed */
            Waiting = OCI_AMS_WAITING,
            /** The message has been processed */
            Processed = OCI_AMS_PROCESSED,
            /** The message has moved to exception queue */
            Expired = OCI_AMS_EXPIRED
        };

        /**
        * @brief
        * Message state
        *
        * Possible values are Message::MessageStateValues
        *
        */
        typedef core::Enum<MessageStateValues> MessageState;

        /**
        * @brief
        * Create a message object based on the given payload type
        *
        * @param typeInfo - Type info object
        *
        * @note
        * OCILIB supports 2 type of message payload :
        * - Oracle types (UDT)
        * - RAW data
        *
        * @note
        * Oracle Type AnyData is not supported in the current version of OCILIB
        *
        * @note
        * the parameter 'typinf' indicates the type of payload :
        * - For object payload, retrieve the object type information object from the given type name
        * - For RAW payload, you MUST pass the object type information object from the type name "SYS.RAW" as object type name
        *
        * @warning
        * Newly created Message handles have NULL payloads.
        * For Message handling Objects payloads, Get() returns a null Object until an object is assigned to the message.
        *
        * @note
        * When a local Message object handle is enqueued, it keeps its attributes. If it's enqueued again, another
        * identical message is posted into the queue.
        * To reset a message and empty all its properties, call Reset()
        *
        */
        Message(const TypeInfo& typeInfo);

        /**
        * @brief
        * Reset all attributes of the message
        *
        * @warning
        * Reset() clears the message payload and set it to NULL
        *
        */
        void Reset();

        /**
        * @brief
        * Get the object payload of the message
        *
        */
        template<class T>
        T GetPayload();

        /**
        * @brief
        * Set the object payload of the message
        *
        * @param value - Object payload
        *
        */
        template<class T>
        void SetPayload(const T& value);

        /**
        * @brief
        * return the time the message was enqueued
        *
        * @note
        * Only use this function for message dequeued from queues
        *
        */
        Date GetEnqueueTime() const;

        /**
        * @brief
        * Return the number of attempts that have been made to dequeue the message
        *
        */
        Int32 GetAttemptCount() const;

        /**
        * @brief
        * Return the state of the message at the time of the dequeue
        *
        */
        MessageState GetState() const;

        /**
        * @brief
        * Return the ID of the message
        *
        * @note
        * The message ID is :
        *  - generated when the message is enqueued in the queue
        *  - retrieved when the message is dequeued from the queue
        *
        */
        Raw GetID() const;

        /**
        * @brief
        * Return the duration that the message is available for dequeuing
        *
        * @note
        * see SetExpiration() for more details
        *
        */
        Int32 GetExpiration() const;

        /**
        * @brief
        * set the duration that the message is available for dequeuing
        *
        * @param value - duration in seconds
        *
        * @note
        * This parameter is an offset from the delay (see SetEnqueueDelay())
        * While waiting for expiration, the message state is set to Message::Ready.
        * If the message is not dequeued before it expires, it will be moved to the exception queue
        * with the state Message::Expired.
        *
        * @note
        * If parameter 'value' is set to -1 (default value), the message will not expire
        *
        * @warning
        * Expiration processing requires the queue monitor to be started.
        *
        */
        void SetExpiration(Int32 value);

        /**
        * @brief
        * Return the number of seconds that a message is delayed for dequeuing
        *
        * @note
        * see SetEnqueueDelay() for more details
        *
        */
        Int32 GetEnqueueDelay() const;

        /**
        * @brief
        * set the number of seconds to delay the enqueued message
        *
        * @param value - Delay in seconds
        *
        * @note
        * The delay represents the number of seconds after which a message is available for dequeuing.
        * When the message is enqueued, its state is set to Message::Waiting.
        * When the delay expires, its state is set to Message::Ready.
        *
        * @note
        * If parameter 'value' is set to zero (default value), the message will be immediately available
        * for dequeuing
        *
        * @warning
        * Dequeuing by Message ID overrides the delay specification.
        *
        * @warning
        * Delaying processing requires the queue monitor to be started.
        *
        */
        void SetEnqueueDelay(Int32 value);

        /**
        * @brief
        * Return the priority of the message
        *
        * @note
        * see SetPriority() for more details
        *
        */
        Int32 GetPriority() const;

        /**
        * @brief
        * Set the priority of the message
        *
        * @param value - Message priority
        *
        * @note
        *   - The priority can be any number, including negative numbers.
        *   - A smaller number indicates higher priority.
        *   - Default value is zero.
        *
        */
        void SetPriority(Int32 value);

        /**
        * @brief
        * Return the original ID of the message in the last queue that generated this message
        *
        * @warning
        * When a message is propagated from/to different queues, this ID is the one generated for the
        * message in the previous queue.
        *
        */
        Raw GetOriginalID() const;

        /**
        * @brief
        * Set the original ID of the message in the last queue that generated this message
        *
        * @param value - Message ID
        *
        * @warning
        * When a message is propagated from/to different queues, this ID is the one generated for the
        * message in the previous queue.
        *
        */
        void SetOriginalID(const Raw& value);

        /**
        * @brief
        * Get the correlation identifier of the message
        *
        * @note
        * see SetCorrelation() for more details
        *
        */
        ostring GetCorrelation() const;

        /**
        * @brief
        * Set the correlation identifier of the message
        *
        * @param value - Message correlation text
        *
        * @note
        * see Dequeue::SetCorrelation() for more details
        *
        */
        void SetCorrelation(const ostring& value);

        /**
        * @brief
        * Get the Exception queue name of the message
        *
        * @warning
        * When calling this function on a message retrieved with Dequeue::Get(), the returned value is
        * empty if the default exception queue associated with the current queue is used (e.g. no user
        * defined specified at enqueue time for the message)
        *
        * @note
        * see SetExceptionQueue() for more details
        *
        */
        ostring GetExceptionQueue() const;

        /**
        * @brief
        * Set the name of the queue to which the message is moved to if it cannot be
        * processed successfully
        *
        * @param value - Exception queue name
        *
        * @warning
        * From Oracle Documentation :
        *
        * "Messages are moved into exception queues in two cases :
        *  - If the number of unsuccessful dequeue attempts has exceeded the attribute 'max_retries' of
        *    given queue
        *  - if the message has expired.
        *
        * All messages in the exception queue are in the EXPIRED state.
        *
        * The default is the exception queue associated with the queue table.
        *
        * If the exception queue specified does not exist at the time of the move the message will be
        * moved to the default exception queue associated with the queue table and a warning will be
        * logged in the alert file.
        *
        * This attribute must refer to a valid queue name."
        *
        */
        void SetExceptionQueue(const ostring& value);

        /**
        * @brief
        * Return the original sender of the message
        *
        * @return
        * Valid agent object if set at enqueue time otherwise a null agent object
        *
        */
        Agent GetSender() const;

        /**
        * @brief
        * Set the original sender of the message
        *
        * @param agent - Message sender
        *
        */
        void SetSender(const Agent& agent);

        /**
        * @brief
        * Set the recipient list of a message to enqueue
        *
        * @param agents - Recipients list
        *
        * @warning
        * This function should only be used for queues which allow multiple consumers.
        * The default recipients are the queue subscribers.
        *
        */
        void SetConsumers(std::vector<Agent>& agents);

    private:

        Message(OCI_Msg* pMessage, core::Handle* parent);
    };

    /**
    * @brief
    * Enqueue object for queuing messages into an Oracle Queue
    *
    * This class wraps the OCILIB object handle OCI_Enqueue and its related methods
    *
    */
    class Enqueue : public core::HandleHolder<OCI_Enqueue*>
    {
    public:

        /**
        * @brief
        * Message enqueuing mode enumerated values
        *
        */
        enum EnqueueModeValues
        {
            /** Enqueue message before another message */
            Before = OCI_ASD_BEFORE,
            /** Enqueue message before all messages */
            OnTop = OCI_ASD_TOP
        };

        /**
        * @brief
        * Message enqueuing mode
        *
        * Possible values are Enqueue::EnqueueModeValues
        *
        */
        typedef core::Enum<EnqueueModeValues> EnqueueMode;

        /**
        * @brief
        * Message visibility enumerated values
        *
        */
        enum EnqueueVisibilityValues
        {
            /** Enqueue is an independent transaction */
            Immediate = OCI_AMV_IMMEDIATE,
            /** Enqueue is part of current transaction */
            OnCommit = OCI_AMV_ON_COMMIT
        };

        /**
        * @brief
        * Message visibility after begin queued
        *
        * Possible values are Enqueue::EnqueueVisibilityValues
        *
        */
        typedef core::Enum<EnqueueVisibilityValues> EnqueueVisibility;

        /**
        * @brief
        * Create a Enqueue object for the given queue
        *
        * @param typeInfo  - Type info handle
        * @param queueName - Queue name
        *
        * @note
        * OCILIB supports 2 types of message payload :
        * - Oracle types (UDT)
        * - RAW data
        *
        * @note
        * Oracle Type AnyData is not supported in the current version of OCILIB
        *
        * @note
        * the parameter 'typeInfo' indicates the type of payload to enqueue to the given queue :
        * - For object payload, retrieve the object type information object from the given type name
        * - For RAW payload, you MUST pass the object type information object from the type name "SYS.RAW" as object type name
        *
        */
        Enqueue(const TypeInfo& typeInfo, const ostring& queueName);

        /**
        * @brief
        * Enqueue a message the on queue associated to the Enqueue object
        *
        * @param message  - Message  to enqueue
        *
        */
        void Put(const Message& message);

        /**
        * @brief
        * Get the enqueuing/locking behavior
        *
        * @note
        * see SetVisibility() for more details
        *
        */
        EnqueueVisibility GetVisibility() const;

        /**
        * @brief
        * Set whether the new message is enqueued as part of the current transaction
        *
        * @param value - Enqueuing visibility
        *
        * @note
        * Default value is Enqueue::OnCommit
        *
        */
        void SetVisibility(EnqueueVisibility value);

        /**
        * @brief
        * Return the enqueuing mode of messages to enqueue
        *
        * @note
        * see SetMode() for more details
        *
        */
        EnqueueMode GetMode() const;

        /**
        * @brief
        * Set the enqueuing mode of messages to put in the queue
        *
        * @param value - enqueuing mode
        *
        * @note
        * Default value is Enqueue::OnTop
        *
        * @note
        * if the parameter 'value' is set to Enqueue::Before, the application must
        * call SetRelativeMsgID() before enqueuing the next message in the queue.
        *
        * @note
        * In order to stop enqueuing message using a sequence deviation, call
        * SetMode() with the value Enqueue::OnTop
        *
        */
        void SetMode(EnqueueMode value);

        /**
        * @brief
        * Get the current associated message identifier used for enqueuing messages
        * using a sequence deviation
        *
        * @note
        * see SetRelativeMsgID() for more details
        *
        */

        Raw GetRelativeMsgID() const;

        /**
        * @brief
        * Set a message identifier to use for enqueuing messages using a sequence deviation
        *
        * @param value    - message identifier
        *
        * @note
        * This call is only valid if SetMode() has been called with the value Enqueue::Before
        *
        * @note
        * see SetMode() for more details
        *
        */
        void SetRelativeMsgID(const Raw& value);
    };

    /**
    * @brief
    * Dequeue object for dequeuing messages into an Oracle Queue
    *
    * This class wraps the OCILIB object handle OCI_Dequeue and its related methods
    *
    */
    class Dequeue : public core::HandleHolder<OCI_Dequeue*>
    {
        friend class Environment;

    public:

        /**
         * @typedef NotifyAQHandlerProc
         *
         * @brief
         * User callback for dequeue event notifications
         *
         */
        typedef void (*NotifyAQHandlerProc) (Dequeue& dequeue);

        /**
        * @brief
        * Dequeue mode enumerated values
        *
        */
        enum DequeueModeValues
        {
            /** Read message without acquiring a lock */
            Browse = OCI_ADM_BROWSE,
            /** Read and obtain write lock on message */
            Locked = OCI_ADM_LOCKED,
            /** Read the message and delete it */
            Remove = OCI_ADM_REMOVE,
            /** Confirm receipt of the message, but do not deliver the actual message content */
            Confirm = OCI_ADM_REMOVE_NODATA
        };

        /**
        * @brief
        * Dequeue mode
        *
        * Possible values are Dequeue::DequeueModeValues
        *
        */
        typedef core::Enum<DequeueModeValues> DequeueMode;

        /**
        * @brief
        * Message visibility enumerated values
        *
        */
        enum DequeueVisibilityValues
        {
            /** Dequeue is an independent transaction */
            Immediate = OCI_AMV_IMMEDIATE,
            /** Dequeue is part of current transaction */
            OnCommit = OCI_AMV_ON_COMMIT
        };

        /**
        * @brief
        * Message visibility after begin dequeued
        *
        * Possible values are Dequeue::DequeueVisibilityValues
        *
        */
        typedef core::Enum<DequeueVisibilityValues> DequeueVisibility;

        /**
        * @brief
        * Navigation Mode enumerated values
        *
        */
        enum NavigationModeValues
        {
            /** Retrieves the first message which is available */
            FirstMessage = OCI_ADN_FIRST_MSG,
            /** Retrieves the next message which is available */
            NextMessage = OCI_ADN_NEXT_MSG,
            /** Skips the remainder of the current transaction  group (if any)
             and retrieves the first message of the next transaction group.*/
            NextTransaction = OCI_ADN_NEXT_TRANSACTION
        };

        /**
        * @brief
        * Navigation Mode
        *
        * Possible values are Dequeue::NavigationModeValues
        *
        */
        typedef core::Enum<NavigationModeValues> NavigationMode;

        /**
        * @brief
        * Parametrized constructor
        *
        * @param typeInfo  - Payload type info
        * @param queueName - Queue name
        *
        */
        Dequeue(const TypeInfo& typeInfo, const ostring& queueName);

        /**
        * @brief
        * Dequeue messages from the given queue
        *
        * @warning
        * When dequeuing from a multiple consumer queue, you need
        * to set the navigation mode to NavigationMode::FirstMessage using
        * SetNavigation()
        *
        * @return
        * A valid Message handle on success otherwise a null Message on timeout
        *
        */
        Message Get();

        /**
        * @brief
        * Listen for messages that match any recipient of the associated Agent list
        *
        * @param timeout - Timeout in second
        *
        * @note
        * If an Agent handle is returned, messages are available for this agent.
        * In order to retrieve its messages :
        * - call SetConsumer() with the name of agent using Agent::GetName()
        * - call Get() to dequeue it's pending messages
        *
        * @return
        * An Agent handle for who messages are available on success otherwise an null agent object
        */
        Agent Listen(Int32 timeout);

        /**
        * @brief
        * Get the current consumer name associated with the dequeuing process.
        *
        * @note
        * see SetConsumer() for more details
        *
        */
        ostring GetConsumer() const;

        /**
        * @brief
        * Set the current consumer name to retrieve message for.
        *
        * @param value - consumer name
        *
        * @warning
        * If a queue is not set up for multiple consumers, SetConsumer()
        * should not be called or called with an empty value
        *
        */
        void SetConsumer(const ostring& value);

        /**
        * @brief
        * Get the correlation identifier of the message to be dequeued
        *
        * @note
        * see SetCorrelation() for more details
        *
        */
        ostring GetCorrelation() const;

        /**
        * @brief
        * set the correlation identifier of the message to be dequeued
        *
        * @param value - correlation identifier
        *
        * @note
        * Special pattern matching characters, such as "%" or "_" can be used.
        * If more than one message satisfies the pattern, the order of dequeuing is undetermined.
        *
        */
        void SetCorrelation(const ostring& value);

        /**
        * @brief
        * Get the message identifier of the message to be dequeued
        *
        * @note
        * see SetRelativeMsgID() for more details
        *
        */
        Raw GetRelativeMsgID() const;

        /**
        * @brief
        * Set the message identifier of the message to be dequeued
        *
        * @param value    - message identifier
        *
        */
        void SetRelativeMsgID(const Raw& value);

        /**
        * @brief
        * Get the dequeuing/locking behavior
        *
        * @note
        * see SetVisibility() for more details
        *
        */
        DequeueVisibility GetVisibility() const;

        /**
        * @brief
        * Set whether the new message is dequeued as part of the current transaction
        *
        * @param value - dequeuing mode
        *
        * @warning
        * The visibility parameter is ignored when using the dequeuing
        * mode is Dequeue::Browse
        *
        * @note
        * Default value is Dequeue::OnCommit
        *
        */
        void SetVisibility(DequeueVisibility value);

        /**
        * @brief
        * Get the dequeuing/locking behavior
        *
        * @note
        * see SetMode() for more details
        *
        */
        DequeueMode GetMode() const;

        /**
        * @brief
        * Set the dequeuing/locking behavior
        *
        * @param value - dequeuing mode
        *
        * @note
        * Default value is Dequeue::Remove
        *
        */
        void SetMode(DequeueMode value);

        /**
        * @brief
        * Return the navigation position of messages to retrieve from the queue
        *
        * @note
        * see SetNavigation() for more details
        *
        */
        NavigationMode GetNavigation() const;

        /**
        * @brief
        * Set the position of messages to be retrieved.
        *
        * @param value - navigation position
        *
        * @note
        * The dequeuing uses the following sequence :
        *   - find messages using the navigation position
        *   - apply search criteria (message correlation)
        *   - get message
        *
        * @note
        * Default value is Dequeue::NextMessage
        *
        * @warning
        * Dequeue::NextTransaction can only be used if message grouping is enabled for the given queue.
        *
        */
        void SetNavigation(NavigationMode value);

        /**
        * @brief
        * Return the time that Get() waits for messages if no messages are currently available
        *
        * @note
        * see SetWaitTime() for more details
        *
        */
        Int32 GetWaitTime() const;

        /**
        * @brief
        * Set the time that Get() waits for messages if no messages are currently available
        *
        * @param value - timeout in seconds
        *
        *@note
        * - Any positive values in seconds are valid.
        * - The value 0  is accepted and means Get() does not wait for
        *   messages and returns immediately if no messages are available
        * - The value -1 is accepted and means Get() waits for ever (until
        *   a message is available in the queue)
        *
        * @note
        * Default value is -1 (wait for ever)
        *
        */
        void SetWaitTime(Int32 value);

        /**
         * @brief
         * Set the Agent list to listen to message for
         *
         * @param agents - vector of agents
         *
         */
        void SetAgents(std::vector<Agent>& agents);

        /**
         * @brief
         * Subscribe one or more dequeues for asynchronous messages notifications
         *
         * @param connection - Connection handle used for the subscription
         * @param dequeues   - Dequeues to subscribe
         * @param timeout    - Notification timeout
         * @param handler    - User handler callback fired when messages are ready to be dequeued
         *
         * @note
         * The notification port is configured on the environment by calling
         * Environment::SetSubscriptionPort() before the first subscription is registered.
         *
         * @note
         * Environment::Events flag must be passed to the Environment constructor to be able to use
         * asynchronous messages notifications
         *
         * @note
         * Each subscribed dequeue can later be unsubscribed with Unsubscribe().
         * Dequeue instances are automatically unsubscribed when destroyed.
         *
         * @note
         * Requires Oracle Client 10gR2 or above
         *
         */
        static void Subscribe(const Connection& connection, std::vector<Dequeue>& dequeues, UInt32 timeout, NotifyAQHandlerProc handler);

        /**
         * @brief
         * Unsubscribe for asynchronous messages notifications
         *
         * @note
         * Requires Oracle Client 10gR2 or above
         *
         */
        void Unsubscribe();

    private:

        Dequeue(OCI_Dequeue* pDequeue, core::Handle* parent);
    };

    /**
    * @brief
    * Class allowing the administration of Oracle Queues
    *
    */
    class Queue
    {
    public:

        /**
        * @brief
        * Queue Type enumerated values
        *
        */
        enum QueueTypeValues
        {
            /** Normal queue */
            NormalQueue = OCI_AQT_NORMAL,
            /** Exception queue */
            ExceptionQueue = OCI_AQT_EXCEPTION,
            /** Non persistent queue */
            NonPersistentQueue = OCI_AQT_NON_PERSISTENT
        };

        /**
        * @brief
        * Queue Type
        *
        * Possible values are Queue::QueueTypeValues
        *
        */
        typedef core::Enum<QueueTypeValues> QueueType;

        /**
         * @brief
         * Create a queue
         *
         * @param connection         - Database connection
         * @param queue              - Queue name
         * @param table              - Queue table name
         * @param type               - Queue type
         * @param maxRetries         - Maximum number of attempts to dequeue a message
         * @param retryDelay         - Number of seconds between attempts to dequeue a message
         * @param retentionTime      - number of seconds a message is retained in the queue table after
         *                             being dequeued from the queue
         * @param dependencyTracking - Parameter reserved for future use by Oracle (MUST be set to FALSE)
         * @param comment            - Description of the queue
         *
         * @note
         * Parameter 'queue' can specify the schema where to create to queue ([schema.]queue_name)
         * Queue names cannot be longer than 24 characters (Oracle limit for user queues)
         *
         * @note
         * this call wraps the PL/SQL procedure DBMS_AQADM.CREATE_QUEUE().
         * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
         *
         */
        static void Create(const Connection& connection, const ostring& queue, const ostring& table, QueueType type = NormalQueue,
            UInt32 maxRetries = 0, UInt32 retryDelay = 0, UInt32 retentionTime = 0,
            bool dependencyTracking = false, const ostring& comment = OTEXT(""));

        /**
         * @brief
         * Alter the given queue
         *
         * @param connection      - Database connection
         * @param queue           - Queue name
         * @param maxRetries      - Maximum number of attempts to dequeue a message
         * @param retryDelay      - Number of seconds between attempts to dequeue a message
         * @param retentionTime   - number of seconds a message is retained in the queue table after
         *                          being dequeued from the queue
         * @param comment         - Description of the queue
         *
         * @note
         * See Create() for more details
         *
         * @note
         * this call wraps the PL/SQL procedure DBMS_AQADM.ALTER_QUEUE().
         * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
         *
         */
        static void Alter(const Connection& connection, const ostring& queue,
            UInt32 maxRetries = 0, UInt32 retryDelay = 0,
            UInt32 retentionTime = 0, const ostring& comment = OTEXT(""));

        /**
         * @brief
         * Drop the given queue
         *
         * @param connection - Database connection
         * @param queue      - Queue name
         *
         * @warning
         * A queue can be dropped only if it has been stopped before.
         *
         * @note
         * this call wraps the PL/SQL procedure DBMS_AQADM.DROP_QUEUE().
         * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
         *
         */
        static void Drop(const Connection& connection, const ostring& queue);

        /**
         * @brief
         * Start the given queue
         *
         * @param connection     - Database connection
         * @param queue          - Queue name
         * @param enableEnqueue  - Enable enqueue
         * @param enableDequeue  - Enable dequeue
         *
         * @warning
         * For exception queues, only enqueuing is allowed
         *
         * @note
         * this call wraps the PL/SQL procedure DBMS_AQADM.START_QUEUE().
         * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
         *
         */
        static void Start(const Connection& connection, const ostring& queue, bool enableEnqueue = true, bool enableDequeue = true);

        /**
        * @brief
        * Stop enqueuing or dequeuing or both on the given queue
        *
        * @param connection  - Database connection
        * @param queue       - Queue name
        * @param stopEnqueue - Disable enqueue
        * @param stopDequeue - Disable dequeue
        * @param wait        - Wait for current pending enqueues/dequeues
        *
        * @warning
        * A queue cannot be stopped if there are pending transactions against the queue.
        *
        * @note
        * this call wraps the PL/SQL procedure DBMS_AQADM.STOP_QUEUE().
        * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
        *
        */
        static void Stop(const Connection& connection, const ostring& queue, bool stopEnqueue = true, bool stopDequeue = true, bool wait = true);
    };

    /**
    * @brief
    * Class allowing the administration of Oracle Queue tables
    *
    */
    class QueueTable
    {
    public:

        /**
         *
         * @brief
         * Grouping mode enumerated values
         *
         */
        enum GroupingModeValues
        {
            /** Each message is treated individually */
            None = OCI_AGM_NONE,
            /** All messages enqueued in one transaction are considered part of
                the same group and can be dequeued as a group of related messages */
            Transactionnal = OCI_AGM_TRANSACTIONNAL
        };

        /**
        * @brief
        * Grouping Mode
        *
        * Possible values are QueueTable::GroupingModeValues
        *
        */
        typedef core::Enum<GroupingModeValues> GroupingMode;

        /**
         *
         * @brief
         * Purge mode enumerated values
         *
         */
        enum PurgeModeValues
        {
            /** Purge only buffered   messages */
            Buffered = OCI_APM_BUFFERED,
            /** Purge only persistent messages */
            Persistent = OCI_APM_PERSISTENT,
            /** Purge all messages */
            All = OCI_APM_ALL
        };

        /**
        * @brief
        * Purge mode
        *
        * Possible values are QueueTable::PurgeModeValues
        *
        */
        typedef core::Enum<PurgeModeValues> PurgeMode;

        /**
         * @brief
         * Create a queue table for messages of the given type
         *
         * @param connection         - Database connection
         * @param table              - Queue table name
         * @param payloadType        - Message type name
         * @param multipleConsumers  - Enable multiple consumers for each messages
         * @param storageClause      - Optional Additional clauses for the table storage
         * @param sortList           - Optional Additional columns name to use for sorting
         * @param groupingMode       - Optional Specifies if messages are grouped within a transaction
         * @param comment            - Optional Description of the queue table
         * @param primaryInstance    - Optional primary owner (instance) of the queue table
         * @param secondaryInstance  - Optional Owner of the queue table if the primary instance is not available
         * @param compatible         - Optional lowest database version with which the queue table is compatible
         *
         * @note
         * Parameter 'table' can specify the schema where to create to queue table ([schema.]queue_table)
         * Queue table names cannot be longer than 24 characters (Oracle limit for user queue tables)
         *
         * @note
         * Possible values for parameter 'payloadType' :
         * - For Oracle types (UDT) : use the type name ([schema.]type_name)
         * - For RAW data           : use "SYS.RAW" or "RAW"
         *
         * @note
         * Possible values for parameter 'compatible' :
         * - "8.0", "8.1", "10.0"
         *
         * @note
         * this call wraps the PL/SQL procedure DBMS_AQADM.CREATE_QUEUE_TABLE().
         * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
         *
         */
        static void Create(const Connection& connection, const ostring& table, const ostring& payloadType, bool multipleConsumers,
            const ostring& storageClause = OTEXT(""), const ostring& sortList = OTEXT(""),
            GroupingMode groupingMode = None, const ostring& comment = OTEXT(""),
            UInt32 primaryInstance = 0, UInt32 secondaryInstance = 0,
            const ostring& compatible = OTEXT(""));

        /**
        * @brief
        * Alter the given queue table
        *
        * @param connection           - Database connection
        * @param table                - Queue table name
        * @param comment              - Optional Description of the queue table
        * @param primaryInstance      - Optional primary owner (instance) of the queue table
        * @param secondaryInstance    - Optional Owner of the queue table if the primary instance is not available
        *
        * @note
        * See Create() from more details
        *
        * @note
        * this call wraps the PL/SQL procedure DBMS_AQADM.ALTER_QUEUE_TABLE().
        * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
        *
        */
        static void Alter(const Connection& connection, const ostring& table, const ostring& comment, UInt32 primaryInstance = 0, UInt32 secondaryInstance = 0);

        /**
        * @brief
        * Drop the given queue table
        *
        * @param connection   - Database connection
        * @param table        - Queue table name
        * @param force        - Force the deletion of objects related to the queue table
        *
        * @note
        * Possible values for 'force' :
        *  - true  : all queues using the queue table and their associated propagation schedules are
        *            dropped automatically
        *  - false : All the queues using the given queue table must be stopped and dropped before the
        *            queue table can be dropped.
        *
        * @note
        * this call wraps the PL/SQL procedure DBMS_AQADM.DROP_QUEUE_TABLE().
        * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
        *
        */
        static void Drop(const Connection& connection, const ostring& table, bool force = true);

        /**
         * @brief
         * Purge messages from the given queue table
         *
         * @param connection - Database connection
         * @param table      - Queue table name
         * @param mode       - Type of message to purge
         * @param block      - Lock all queues using the queue table while doing the purge
         * @param condition  - Optional SQL based conditions (see notes)
         *
         * @note
         * For more information about the SQL purge conditions, refer to
         * Oracle Streams - Advanced Queuing User's Guide for more details
         *
         * @warning
         * This feature is only available from Oracle 10gR2.
         * This function does nothing and returns TRUE is the server version is < Oracle 10gR2
         *
         * @note
         * this call wraps the PL/SQL procedure DBMS_AQADM.PURGE_QUEUE_TABLE().
         * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
         *
         */
        static void Purge(const Connection& connection, const ostring& table, PurgeMode mode, const ostring& condition = OTEXT(""), bool block = true);

        /**
         * @brief
         * Migrate a queue table from one version to another
         *
         * @param connection - Database connection
         * @param table      - Queue table name
         * @param compatible - Optional database version with which the queue table has to migrate
         *
         * @note
         * Possible values for parameter 'compatible' :
         * - "8.0", "8.1", "10.0"
         *
         * @note
         * this call wraps the PL/SQL procedure DBMS_AQADM.MIGRATE_QUEUE_TABLE().
         * Refer to Oracle Streams - Advanced Queuing User's Guide for more details
         *
         */
        static void Migrate(const Connection& connection, const ostring& table, const ostring& compatible = OTEXT(""));
    };

    /**
    * @brief
    * Oracle Direct path loading encapsulation
    *
    * This class wraps the OCILIB object handle OCI_DirPath and its related methods
    *
    */
    class DirectPath : public core::HandleHolder<OCI_DirPath*>
    {
    public:

        /**
         * @brief
         * Conversion mode enumerated  values
         *
         */
        enum ConversionModeValues
        {
            /** Conversion fails on error */
            Default = OCI_DCM_DEFAULT,
            /** Conversion does not fail on error */
            Force = OCI_DCM_FORCE
        };

        /**
        * @brief
        * Conversion Mode
        *
        * Possible values are DirectPath::ConversionModeValues
        *
        */
        typedef core::Enum<ConversionModeValues> ConversionMode;

        /**
        * @brief
        * Direct Operation Result enumerated values
        *
        */
        enum ResultValues
        {
            /** Conversion/load has been successful */
            ResultComplete = OCI_DPR_COMPLETE,
            /** An error happened while converting/loading data */
            ResultError = OCI_DPR_ERROR,
            /** The internal stream is full*/
            ResultFull = OCI_DPR_FULL,
            /** A column has not been fully filled yet */
            ResultPartial = OCI_DPR_PARTIAL,
            /** No data was found to convert/load */
            ResultEmpty = OCI_DPR_EMPTY
        };

        /**
        * @brief
        * Direct Operation Result
        *
        * Possible values are DirectPath::ResultValues
        *
        */
        typedef core::Enum<ResultValues> Result;

        /**
         * @brief
         * Constructor
         *
         * @param typeInfo  - Table type info object
         * @param nbCols    - Number of columns to load
         * @param nbRows    - Maximum of rows to handle per load operation
         * @param partition - Partition name
         *
         * @note
         * The partition name is not mandatory
         *
         * @note
         * Parameter 'nbRows' is ignored for Oracle 8i. Prior to Oracle 9i, it's the
         * OCI client that decides of the number of rows to process per convert/load calls.
         * From Oracle 9i, OCI allows application to specify this value. Note that, the
         * OCI client might not accept the input value. After Prepare() has
         * been successfully called, GetMaxRows() returns the final number
         * of rows used for the given direct path operation.
         *
         */
        DirectPath(const TypeInfo& typeInfo, UInt32 nbCols, UInt32  nbRows, const ostring& partition = OTEXT(""));

        /**
         * @brief
         * Describe a column to load into the given table
         *
         * @param colIndex - Column index
         * @param name     - Column name
         * @param maxSize  - Maximum input value size for a column entry
         * @param format   - Date or numeric format to use
         *
         * @note
         * An error is thrown if :
         * - If the column specified by the 'name' parameter is not found in the table
         *   referenced by the type info handle passed to the constructor
         * - the index is out of bounds (= 0 or >= number of columns)
         *
         */
        void SetColumn(UInt32 colIndex, const ostring& name, UInt32 maxSize, const ostring& format = OTEXT(""));

        /**
         * @brief
         * Set the value of the given row/column array entry from the given string
         *
         * @tparam T - type of data to set (only supported types are ostring and Raw)
         *
         * @param rowIndex  - Row index
         * @param colIndex  - Column index
         * @param value     - Value to set
         * @param complete  - Is the entry content fully provided ?
         *
         * @note
         * Rows and columns indexes start at 1.
         *
         * @note
         * Direct path support piece loading for LONGs and LOBs columns. When filling
         * these columns, it's possible to provide input buffer piece by piece. In order
         * to do so :
         * - set the 'complete' parameter to false
         * - Repeat calls to SetEntry() until the data is totally provided
         * - The last call that set the last piece or an entry must specify the value
         *   true for the 'complete' parameter
         *
         * @warning
         * Current Direct Path OCILIB implementation DOES NOT support setting entry
         * content piece by piece as mentioned above. It was planned in the original design
         * but not supported yet. So, always set the complete parameter to true.
         * Setting entries content piece by piece may be supported in future releases
         *
         */
        template<class T>
        void SetEntry(UInt32 rowIndex, UInt32 colIndex, const T& value, bool complete = true);

        /**
         * @brief
         * Reset internal arrays and streams to prepare another load
         *
         * @note
         * Once some data have been converted or loaded, Reset() resets
         * internal OCI structures in order to prepare another load operation
         * (set entries, convert and load)
         *
         */
        void Reset();

        /**
         * @brief
         * Prepares the OCI direct path load interface before any rows can be converted or loaded
         *
         */
        void Prepare();

        /**
         * @brief
         * Convert provided user data to the direct path stream format
         *
         * @par Behavior
         * - When using conversion mode DirectPath::Default, Convert() stops when
         *   any error is encountered and returns ResultError
         * - When using conversion mode DirectPath::Force, Convert() does not stop
         *   on errors. Instead it discards any erred rows and returns ResultComplete once
         *   all rows are processed.
         *
         * @note
         * List of faulted rows and columns can be retrieved using GetErrorRow() and GetErrorColumn()
         *
         * @note
         * GetAffectedRows() returns the number of rows converted in the last call.
         *
         */
        DirectPath::Result Convert();

        /**
         * @brief
         * Loads the data converted to direct path stream format
         *
         * @note
         * List of faulted rows can be retrieved using GetErrorRow()
         *
         * @note
         * GetAffectedRows() returns the number of rows successfully loaded in the last call.
         *
         */
        DirectPath::Result Load();

        /**
         * @brief
         * Terminate a direct path operation and commit changes into the database
         *
         * @warning
         * The direct path object cannot be used anymore after this call for any more loading operations
         *
         */
        void Finish();

        /**
         * @brief
         * Terminate a direct path operation without committing changes
         *
         * @note
         * Any pending loaded data are canceled.
         * Any load completion operations, such as index maintenance operations, are not performed.
         *
         * @warning
         * The direct path object cannot be used anymore after this call for any more loading operations
         *
         */
        void Abort();

        /**
         * @brief
         * Execute a data save-point (server side)
         *
         * @note
         * Executing a data save-point is not allowed for LOBs
         *
         */
        void Save();

        /**
         * @brief
         * Flushes a partially loaded row from server
         *
         */
        void FlushRow();

        /**
         * @brief
         * Set the current number of rows to convert and load
         *
         * @param value - Number of row to process
         *
         * @warning
         * An OCILIB error will be thrown if the value exceeds the maximum number of
         * rows in the internals arrays
         *
         */
        void SetCurrentRows(UInt32 value);

        /**
         * @brief
         * Return the current number of rows used in the OCILIB internal arrays of rows
         *
         * @return
         * Internal current array size on SUCCESS otherwise 0
         *
         */
        UInt32 GetCurrentRows() const;

        /**
         * @brief
         * Return the maximum number of rows allocated in the OCI and OCILIB internal arrays of rows
         *
         * @return
         * Internal maximum array size on SUCCESS otherwise 0
         *
         */
        UInt32 GetMaxRows() const;

        /**
         * @brief
         * Return the number of rows successfully loaded into the database so far
         *
         * @note
         * Insertions are committed with Finish()
         *
         */
        UInt32 GetRowCount() const;

        /**
         * @brief
         * return the number of rows successfully processed during in the last
         * conversion or loading call
         *
         * @note
         * This function called after :
         *
         * - Convert(), returns the number of converted rows
         * - Load(), returns the number of loaded rows
         *
         */
        UInt32 GetAffectedRows() const;

        /**
         * @brief
         * Set the default date format string for input conversion
         *
         * @param format - date format
         *
         * @note
         * For string to date conversion, Oracle uses :
         * - Column date format
         * - Default date format (modified by this call)
         * - Default global support environment setting
         *
         */
        void SetDateFormat(const ostring& format);

        /**
         * @brief
         * Set the parallel loading mode
         *
         * @param value - enable/disable parallel mode
         *
         * @note
         * Default value is false.
         *
         * @note
         * Setting the value to TRUE allows multiple load sessions to load the same
         * segment concurrently
         *
         * @par Parallel loading mode (From Oracle documentation)
         *
         * A direct load operation requires that the object being loaded is locked to
         * prevent DML on the object.
         * Note that queries are lock-free and are allowed while the object is being loaded.
         * - For a table load, if the option is set to:
         *   - false, then the table DML X-Lock is acquired.
         *   - true, then the table DML S-Lock is acquired.
         * - For a partition load, if the option is set to:
         *   - false, then the table DML SX-Lock and partition DML X-Lock is acquired.
         *   - true, then the table DML SS-Lock and partition DML S-Lock is acquired.
         *
         */
        void SetParallel(bool value);

        /**
         * @brief
         * Set the logging mode for the loading operation
         *
         * @param value - enable/disable logging
         *
         * @par Logging mode (from Oracle Documentation)
         *
         * The NOLOG attribute of each segment determines whether image redo or
         * invalidation redo is generated:
         * - false : Use the attribute of the segment being loaded.
         * - true  : No logging. Overrides DDL statement, if necessary.
         *
         */
        void SetNoLog(bool value);

        /**
         * @brief
         * Set number of elements in the date cache
         *
         * @param value - Buffer size
         *
         * @note
         * Default value is 0.
         *
         * @note
         * Setting the value to 0 disables the cache
         *
         */
        void SetCacheSize(UInt32 value);

        /**
         * @brief
         * Set the size of the internal stream transfer buffer
         *
         * @param value - Buffer size
         *
         * @note
         * Default value is 64KB.
         *
         */
        void SetBufferSize(UInt32 value);

        /**
         * @brief
         * Set the direct path conversion mode
         *
         * @param value - Conversion mode
         *
         * @note
         * See Convert() for conversion mode details
         *
         * @note
         * Default value is ConversionMode::Default
         *
         */
        void SetConversionMode(ConversionMode value);

        /**
         * @brief
         * Return the index of a column which caused an error during data conversion
         *
         * @warning
         * Direct path column indexes start at 1.
         *
         * @note
         * Errors may happen while data is converted to direct path stream format
         * using Convert().
         *
         * @par Usage after a Convert() call
         * - When using conversion mode DirectPath::Default, Convert() returns
         * DirectPath::ResultError on error and GetErrorColumn() returns the column index that
         * caused the error.
         * - When using conversion mode DirectPath::Force, Convert() returns
         * DirectPath::ResultComplete even on errors. In order to retrieve the list of all column
         * indexes that have erred, the application can call GetErrorColumn()
         * repeatedly until it returns 0.
         *
         * @note
         * The internal value is reset to 0 when calling Convert()
         *
         * @return
         * 0 is no error occurs otherwise the index of the given column which caused an
         * error
         *
         */
        UInt32 GetErrorColumn();

        /**
         * @brief
         * Return the index of a row which caused an error during data conversion
         *
         * @warning
         * Direct path row indexes start at 1.
         *
         * @note
         * Errors may happen :
         * - while data is converted to direct path stream format using Convert()
         * - while data is loaded to database using Load()
         *
         * @par Usage after a Convert() call
         * - When using conversion mode DirectPath::Default, Convert() returns
         *   DirectPath::ResultError on error and GetErrorRow() returns the row index that
         *   caused the error.
         * - When using conversion mode DirectPath::Force, Convert() returns
         *   DirectPath::ResultComplete even on errors. In order to retrieve the list of all row
         *   indexes that have erred, the application can call GetErrorRow() repeatedly until it returns 0.
         *
         * @par  Usage after a Load() call
         * After a call to Load(), in order to retrieve the list of all faulted rows
         * indexes, the application can call GetErrorRow() repeatedly until it returns 0.
         *
         * @note
         * The internal value is reset to 0 when calling Convert(),Reset() or Load()
         *
         * @return
         * 0 is no error occurs otherwise the index of the given row which caused an
         * error
         *
         */

        UInt32 GetErrorRow();
    };


    /**
     * @brief
     * SODA (Simple Oracle Document Access) document
     *
     * This class wraps the OCILIB object handle OCI_SodaDoc and its related methods
     *
     */
    class SodaDoc : public core::HandleHolder<OCI_SodaDoc*>
    {
        friend class SodaDb;
        friend class SodaColl;
        friend class SodaDocCursor;

    public:

        /**
         * @brief
         * Default constructor (null handle)
         *
         */
        SodaDoc();
        SodaDoc(OCI_SodaDoc* doc, core::Handle* parent);

        /**
         * @brief
         * Document key
         *
         */
        ostring GetKey() const;

        /**
         * @brief
         * Document version
         *
         */
        ostring GetVersion() const;

        /**
         * @brief
         * Document creation timestamp
         *
         */
        ostring GetCreatedOn() const;

        /**
         * @brief
         * Document last modification timestamp
         *
         */
        ostring GetLastModified() const;

        /**
         * @brief
         * Document media type
         *
         */
        ostring GetMediaType() const;

        /**
         * @brief
         * Document content as an otext string
         *
         */
        ostring GetText() const;

        /**
         * @brief
         * Document content as an OCI_Json object
         *
         * @warning
         * Requires an Oracle client 23.4 or higher.
         *
         */
        Json GetJson() const;
    };

    /**
     * @brief
     * SODA operation options
     *
     * This class wraps the OCILIB object handle OCI_SodaOptions and its related methods
     *
     */
    class SodaOptions : public core::HandleHolder<OCI_SodaOptions*>
    {
        friend class SodaColl;

    public:

        /**
         * @brief
         * Default constructor (null handle)
         *
         */
        SodaOptions();

        /**
         * @brief
         * Create a SODA options object bound to a SODA database
         *
         */
        SodaOptions(const SodaDb& sdb);

        /**
         * @brief
         * Set a single document key on the options
         *
         */
        void SetKey(const ostring& key);

        /**
         * @brief
         * Set multiple document keys on the options
         *
         * @warning
         * Requires an Oracle client 18.3 or higher.
         *
         */
        void SetKeys(const std::vector<ostring>& keys);

        /**
         * @brief
         * Set a document version on the options
         *
         */
        void SetVersion(const ostring& version);

        /**
         * @brief
         * Set a query-by-example filter on the options
         *
         */
        void SetFilter(const ostring& filter);

        /**
         * @brief
         * Set the number of documents to skip on the options
         *
         */
        void SetSkip(UInt32 count);

        /**
         * @brief
         * Set the maximum number of documents to return on the options
         *
         */
        void SetLimit(UInt32 count);
    };

    /**
     * @brief
     * SODA database
     *
     * This class wraps the OCILIB object handle OCI_SodaDb and its related methods
     *
     * @warning
     * SODA support requires an Oracle client 18.1 or higher and an Oracle
     * Database 18.1 or higher.
     *
     */
    class SodaDb : public core::HandleHolder<OCI_SodaDb*>
    {
        friend class SodaOptions;

    public:

        /**
         * @brief
         * Default constructor (null handle)
         *
         */
        SodaDb();

        /**
         * @brief
         * Create a SODA database handle bound to a connection
         *
         */
        SodaDb(const Connection& connection);

        /**
         * @brief
         * Return the connection associated with the SODA database
         *
         */
        Connection GetConnection() const;

        /**
         * @brief
         * Create a SODA collection
         *
         */
        SodaColl CreateCollection(const ostring& name, const ostring& metadata = ostring());

        /**
         * @brief
         * Open an existing SODA collection
         *
         */
        SodaColl OpenCollection(const ostring& name);

        /**
         * @brief
         * Return a cursor over the SODA collections in the current schema
         *
         */
        SodaCollCursor GetCollections(const ostring& startName = OTEXT(""));

        /**
         * @brief
         * Create a SODA document from raw text
         *
         */
        SodaDoc CreateDocumentFromText(const ostring& content, const ostring& key = OTEXT(""));

        /**
         * @brief
         * Create a SODA document from a JSON handle
         *
         * @warning
         * Requires an Oracle client 23.4 or higher.
         *
         */
        SodaDoc CreateDocumentFromJson(const Json& json, const ostring& key = OTEXT(""));
    };

    /**
     * @brief
     * SODA collection
     *
     * This class wraps the OCILIB object handle OCI_SodaColl and its related methods
     *
     */
    class SodaColl : public core::HandleHolder<OCI_SodaColl*>
    {
        friend class SodaDb;
        friend class SodaCollCursor;

    public:

        /**
         * @brief
         * Default constructor (null handle)
         *
         */
        SodaColl();
        SodaColl(OCI_SodaColl* coll, core::Handle* parent);

        /**
         * @brief
         * Return the connection associated with the SODA collection
         *
         */
        Connection GetConnection() const;

        /**
         * @brief
         * Collection name
         *
         */
        ostring GetName() const;

        /**
         * @brief
         * Collection metadata descriptor (JSON)
         *
         */
        ostring GetMetadata() const;

        /**
         * @brief
         * Check whether the collection is mapped to an existing table
         *
         * @warning
         * Requires an Oracle client 23.4 or higher.
         *
         */
        bool IsMapped() const;

        /**
         * @brief
         * Drop the collection
         *
         */
        bool Drop();

        /**
         * @brief
         * Truncate the collection (delete all documents)
         *
         * @warning
         * Requires an Oracle client 20.1 or higher.
         *
         */
        bool Truncate();

        /**
         * @brief
         * Insert a document into the collection
         *
         */
        void Insert(SodaDoc& doc);

        /**
         * @brief
         * Find documents in the collection
         *
         */
        SodaDocCursor Find(const SodaOptions& options);

        /**
         * @brief
         * Find one document in the collection
         *
         */
        SodaDoc FindOne(const SodaOptions& options);

        /**
         * @brief
         * Find one document by key in the collection
         *
         */
        SodaDoc FindOne(const ostring& key);

        /**
         * @brief
         * Return the number of documents in the collection
         *
         */
        UInt64 GetCount(const SodaOptions& options);

        /**
         * @brief
         * Return the number of documents matching a filter in the collection
         *
         */
        UInt64 GetCount(const ostring& filter);

        /**
         * @brief
         * Replace one document in the collection
         *
         */
        bool ReplaceOne(const SodaOptions& options, const SodaDoc& doc);

        /**
         * @brief
         * Replace one document in the collection and return the new document
         *
         */
        SodaDoc ReplaceOneAndGet(const SodaOptions& options, SodaDoc& doc);

        /**
         * @brief
         * Replace one document by key in the collection
         *
         */
        bool ReplaceOne(const ostring& key, const SodaDoc& doc);

        /**
         * @brief
         * Replace one document by key in the collection and return the new document
         *
         */
        SodaDoc ReplaceOneAndGet(const ostring& key, SodaDoc& doc);

        /**
         * @brief
         * Remove documents from the collection
         *
         */
        bool Remove(const SodaOptions& options, UInt64* count = nullptr);

        /**
         * @brief
         * Remove one document by key from the collection
         *
         */
        bool RemoveOne(const ostring& key);
    };

    /**
     * @brief
     * SODA collection cursor
     *
     * This class wraps the OCILIB object handle OCI_SodaCollCursor and its related methods
     *
     */
    class SodaCollCursor : public core::HandleHolder<OCI_SodaCollCursor*>
    {
        friend class SodaDb;

    public:

        /**
         * @brief
         * Default constructor (null handle)
         *
         */
        SodaCollCursor();
        SodaCollCursor(OCI_SodaCollCursor* cursor, core::Handle* parent);

        /**
         * @brief
         * Return the next collection of the cursor
         *
         */
        SodaColl GetNext();
    };

    /**
     * @brief
     * SODA document cursor
     *
     * This class wraps the OCILIB object handle OCI_SodaDocCursor and its related methods
     *
     */
    class SodaDocCursor : public core::HandleHolder<OCI_SodaDocCursor*>
    {
        friend class SodaColl;

    public:

        /**
         * @brief
         * Default constructor (null handle)
         *
         */
        SodaDocCursor();
        SodaDocCursor(OCI_SodaDocCursor* cursor, core::Handle* parent);

        /**
         * @brief
         * Return the next document of the cursor
         *
         */
        SodaDoc GetNext();
    };


#define OCILIB_DEFINE_FLAG_OPERATORS(T) \
        inline core::Flags<T> operator | (T a, T b) { return core::Flags<T>(a) | core::Flags<T>(b); } \

        OCILIB_DEFINE_FLAG_OPERATORS(Environment::EnvironmentFlagsValues)
        OCILIB_DEFINE_FLAG_OPERATORS(Environment::SessionFlagsValues)
        OCILIB_DEFINE_FLAG_OPERATORS(Environment::StartFlagsValues)
        OCILIB_DEFINE_FLAG_OPERATORS(Environment::StartModeValues)
        OCILIB_DEFINE_FLAG_OPERATORS(Environment::ShutdownModeValues)
        OCILIB_DEFINE_FLAG_OPERATORS(Environment::ShutdownFlagsValues)
        OCILIB_DEFINE_FLAG_OPERATORS(Transaction::TransactionFlagsValues)
        OCILIB_DEFINE_FLAG_OPERATORS(Column::PropertyFlagsValues)
        OCILIB_DEFINE_FLAG_OPERATORS(Subscription::ChangeTypesValues)
}
