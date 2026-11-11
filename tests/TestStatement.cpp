#include "ocilib_tests.h"
#include "NullArgs.h"

namespace TestCApi
{
    TEST(TestStatement, Metadata)
    {
        ExecDML(OTEXT("CREATE TABLE TestCApiCovMeta(id NUMBER)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        const otext* sql = OTEXT("INSERT INTO TestCApiCovMeta(id) VALUES(1)");
        ASSERT_TRUE(OCI_StatementExecute(stmt, sql));

        const otext* gotSql = OCI_StatementGetSql(stmt);
        ASSERT_NE(nullptr, gotSql);
        ASSERT_EQ(String(sql), String(gotSql));

        ASSERT_EQ(OCI_CST_INSERT, OCI_StatementGetType(stmt));
        ASSERT_EQ(1u, OCI_StatementGetAffectedRows(stmt));

        (void) OCI_StatementGetSqlIdentifier(stmt);
        ASSERT_EQ(nullptr, OCI_StatementGetBatchError(stmt));
        ASSERT_EQ(0u, OCI_StatementGetBatchErrorCount(stmt));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TABLE TestCApiCovMeta"));
    }

    /* -------------------------------------------------------------------------
       Parameterized: statement property setter/getter round-trips
       ------------------------------------------------------------------------- */

    struct StmtPropCase
    {
        const char* name;
        void (*run)(OCI_Statement*);
    };

    void PrintTo(const StmtPropCase& c, std::ostream* os) { *os << c.name; }

    static void StmtPropFetchMode(OCI_Statement* s)
    {
        ASSERT_TRUE(OCI_StatementSetFetchMode(s, OCI_SFM_DEFAULT));
        EXPECT_EQ((OCI_UInt32) OCI_SFM_DEFAULT, OCI_StatementGetFetchMode(s));
    }
    static void StmtPropBindMode(OCI_Statement* s)
    {
        ASSERT_TRUE(OCI_StatementSetBindMode(s, OCI_BIND_BY_NAME));
        EXPECT_EQ((OCI_UInt32) OCI_BIND_BY_NAME, OCI_StatementGetBindMode(s));
    }
    static void StmtPropBindAlloc(OCI_Statement* s)
    {
        ASSERT_TRUE(OCI_StatementSetBindAllocation(s, OCI_BAM_EXTERNAL));
        EXPECT_EQ((OCI_UInt32) OCI_BAM_EXTERNAL, OCI_StatementGetBindAllocation(s));
    }
    static void StmtPropFetchSize(OCI_Statement* s)
    {
        ASSERT_TRUE(OCI_StatementSetFetchSize(s, 10));
        EXPECT_EQ(10u, OCI_StatementGetFetchSize(s));
    }
    static void StmtPropPrefetchSize(OCI_Statement* s)
    {
        ASSERT_TRUE(OCI_StatementSetPrefetchSize(s, 100));
        EXPECT_EQ(100u, OCI_StatementGetPrefetchSize(s));
    }
    static void StmtPropPrefetchMemory(OCI_Statement* s)
    {
        ASSERT_TRUE(OCI_StatementSetPrefetchMemory(s, 1024));
        EXPECT_EQ(1024u, OCI_StatementGetPrefetchMemory(s));
    }
    static void StmtPropPieceSize(OCI_Statement* s)
    {
        ASSERT_TRUE(OCI_StatementSetPieceSize(s, 32000));
        EXPECT_EQ(32000u, OCI_StatementGetPieceSize(s));
    }
    static void StmtPropLongMode(OCI_Statement* s)
    {
        ASSERT_TRUE(OCI_StatementSetLongMode(s, OCI_LONG_IMPLICIT));
        EXPECT_EQ((OCI_UInt32) OCI_LONG_IMPLICIT, OCI_StatementGetLongMode(s));
    }
    static void StmtPropBindArraySize(OCI_Statement* s)
    {
        ASSERT_TRUE(OCI_StatementPrepare(s, OTEXT("select 1 from dual")));
        ASSERT_TRUE(OCI_StatementBindArraySetSize(s, 1));
        EXPECT_EQ(1u, OCI_StatementBindArrayGetSize(s));
    }
    static void StmtPropRebinding(OCI_Statement* s)
    {
        ASSERT_TRUE(OCI_StatementAllowRebinding(s, TRUE));
        EXPECT_TRUE(OCI_StatementIsRebindingAllowed(s));
    }

    class TestStmtProps : public ::testing::TestWithParam<StmtPropCase>
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_Statement* stmt = nullptr;

        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);
        }

        void TearDown() override
        {
            if (stmt) OCI_StatementFree(stmt);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_P(TestStmtProps, SetThenGet_RoundTripsValue)
    {
        GetParam().run(stmt);
    }

    INSTANTIATE_TEST_CASE_P(StmtProp, TestStmtProps,
        ::testing::Values(
            StmtPropCase{ "FetchMode",      StmtPropFetchMode },
            StmtPropCase{ "BindMode",       StmtPropBindMode },
            StmtPropCase{ "BindAllocation", StmtPropBindAlloc },
            StmtPropCase{ "FetchSize",      StmtPropFetchSize },
            StmtPropCase{ "PrefetchSize",   StmtPropPrefetchSize },
            StmtPropCase{ "PrefetchMemory", StmtPropPrefetchMemory },
            StmtPropCase{ "PieceSize",      StmtPropPieceSize },
            StmtPropCase{ "LongMode",       StmtPropLongMode },
            StmtPropCase{ "BindArraySize",  StmtPropBindArraySize },
            StmtPropCase{ "Rebinding",      StmtPropRebinding }),
        ::testing::PrintToStringParamName());

    TEST(TestStatement, ParseAndDescribe)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("select :id as id, :name as name from dual")));

        OCI_UInt32 count = 0;
        const otext** names = nullptr;
        ASSERT_TRUE(OCI_StatementGetParseBindNames(stmt, &count, &names));
        ASSERT_EQ(2u, count);
        ASSERT_NE(nullptr, names);

        ASSERT_TRUE(OCI_StatementParse(stmt, OTEXT("select 1 from dual")));
        ASSERT_TRUE(OCI_StatementDescribe(stmt, OTEXT("select 1 as c from dual")));

        const auto stmt2 = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt2);
        ASSERT_FALSE(OCI_StatementExecute(stmt2, OTEXT("select * from")));
        ASSERT_GT(OCI_StatementGetSqlErrorPos(stmt2), 0u);
        ASSERT_TRUE(OCI_StatementFree(stmt2));

        const auto stmt3 = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt3);
        ASSERT_TRUE(OCI_StatementExecute(stmt3, OTEXT("select 1 from dual")));
        ASSERT_EQ((OCI_UInt32) OCI_CST_SELECT, OCI_StatementGetType(stmt3));
        ASSERT_TRUE(OCI_StatementFree(stmt3));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestStatement, ExecuteStmtFmt)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementExecuteFormat(stmt, OTEXT("select %i as c from dual"), 123));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        ASSERT_EQ(123, OCI_ResultsetGetInt32(rs, 0));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestStatement, ParseAndDescribeFmt)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementParseFormat(stmt, OTEXT("select %i from dual"), 1));
        ASSERT_TRUE(OCI_StatementDescribeFormat(stmt, OTEXT("select %i as c from dual"), 1));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    /* -------------------------------------------------------------------------
       Parameterized: OCI_BindX returns a bind handle per datatype
       ------------------------------------------------------------------------- */

    struct StmtBindCase
    {
        const char* name;
        void (*run)(OCI_Environment*, OCI_Statement*);
    };

    void PrintTo(const StmtBindCase& c, std::ostream* os) { *os << c.name; }

    static void BindScalarPrep(OCI_Statement* s)
    {
        ASSERT_TRUE(OCI_StatementPrepare(s, OTEXT("select :v as v from dual")));
    }

    static void StmtBindInt(OCI_Environment*, OCI_Statement* s)
    {
        OCI_Int32 v = 1;
        BindScalarPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindInt32(s, OTEXT(":v"), &v));
    }
    static void StmtBindUInt(OCI_Environment*, OCI_Statement* s)
    {
        OCI_UInt32 v = 2;
        BindScalarPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindUInt32(s, OTEXT(":v"), &v));
    }
    static void StmtBindShort(OCI_Environment*, OCI_Statement* s)
    {
        OCI_Int16 v = 3;
        BindScalarPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindInt16(s, OTEXT(":v"), &v));
    }
    static void StmtBindUShort(OCI_Environment*, OCI_Statement* s)
    {
        OCI_UInt16 v = 4;
        BindScalarPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindUInt16(s, OTEXT(":v"), &v));
    }
    static void StmtBindBigInt(OCI_Environment*, OCI_Statement* s)
    {
        OCI_Int64 v = 5;
        BindScalarPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindInt64(s, OTEXT(":v"), &v));
    }
    static void StmtBindUBigInt(OCI_Environment*, OCI_Statement* s)
    {
        OCI_UInt64 v = 6;
        BindScalarPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindUInt64(s, OTEXT(":v"), &v));
    }
    static void StmtBindFloat(OCI_Environment*, OCI_Statement* s)
    {
        OCI_Float v = 7.5f;
        BindScalarPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindFloat(s, OTEXT(":v"), &v));
    }
    static void StmtBindDouble(OCI_Environment*, OCI_Statement* s)
    {
        OCI_Double v = 8.5;
        BindScalarPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindDouble(s, OTEXT(":v"), &v));
    }
    static void StmtBindNumber(OCI_Environment* env, OCI_Statement* s)
    {
        const auto num = OCI_NumberCreate(env);
        OCI_Int32 i = 1;
        ASSERT_TRUE(OCI_NumberSetValue(num, OCI_NUM_INT32, &i));
        BindScalarPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindNumber(s, OTEXT(":v"), num));
        ASSERT_TRUE(OCI_NumberFree(num));
    }
    static void StmtBindString(OCI_Environment*, OCI_Statement* s)
    {
        otext v[64] = OTEXT("text");
        BindScalarPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindString(s, OTEXT(":v"), v, 50));
    }
    static void StmtBindRaw(OCI_Environment*, OCI_Statement* s)
    {
        unsigned char v[20] = { 0x01, 0x02, 0x03 };
        BindScalarPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindRaw(s, OTEXT(":v"), v, 3));
    }
    static void StmtBindDate(OCI_Environment* env, OCI_Statement* s)
    {
        const auto date = OCI_DateCreate(env);
        BindScalarPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindDate(s, OTEXT(":v"), date));
        ASSERT_TRUE(OCI_DateFree(date));
    }
    static void StmtBindTimestamp(OCI_Environment* env, OCI_Statement* s)
    {
        const auto ts = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        BindScalarPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindTimestamp(s, OTEXT(":v"), ts));
        ASSERT_TRUE(OCI_TimestampFree(ts));
    }
    static void StmtBindInterval(OCI_Environment* env, OCI_Statement* s)
    {
        const auto iv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        BindScalarPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindInterval(s, OTEXT(":v"), iv));
        ASSERT_TRUE(OCI_IntervalFree(iv));
    }

    class TestStmtBind : public ::testing::TestWithParam<StmtBindCase>
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_Statement* stmt = nullptr;

        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);
        }

        void TearDown() override
        {
            if (stmt) OCI_StatementFree(stmt);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_P(TestStmtBind, BindByName_ReturnsBindHandle)
    {
        GetParam().run(GetEnvironment(nullptr), stmt);
    }

    INSTANTIATE_TEST_CASE_P(BindType, TestStmtBind,
        ::testing::Values(
            StmtBindCase{ "Int",       StmtBindInt },
            StmtBindCase{ "UInt",      StmtBindUInt },
            StmtBindCase{ "Short",     StmtBindShort },
            StmtBindCase{ "UShort",    StmtBindUShort },
            StmtBindCase{ "BigInt",    StmtBindBigInt },
            StmtBindCase{ "UBigInt",   StmtBindUBigInt },
            StmtBindCase{ "Float",     StmtBindFloat },
            StmtBindCase{ "Double",    StmtBindDouble },
            StmtBindCase{ "Number",    StmtBindNumber },
            StmtBindCase{ "String",    StmtBindString },
            StmtBindCase{ "Raw",       StmtBindRaw },
            StmtBindCase{ "Date",      StmtBindDate },
            StmtBindCase{ "Timestamp", StmtBindTimestamp },
            StmtBindCase{ "Interval",  StmtBindInterval }),
        ::testing::PrintToStringParamName());

    TEST_F(TestStmtBind, BoundStmt_ReportsBindCountAndHandles)
    {
        OCI_Int32 i = 1, u = 2;
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("select :i as i, :u as u from dual")));
        ASSERT_NE(nullptr, OCI_StatementBindInt32(stmt, OTEXT(":i"), &i));
        ASSERT_NE(nullptr, OCI_StatementBindInt32(stmt, OTEXT(":u"), &u));

        EXPECT_EQ(2u, OCI_StatementGetBindCount(stmt));
        EXPECT_NE(nullptr, OCI_StatementGetBind(stmt, 0));
        EXPECT_NE(nullptr, OCI_StatementGetBindByName(stmt, OTEXT(":i")));
        EXPECT_EQ(0, OCI_StatementGetBindIndex(stmt, OTEXT(":i")));
    }

    /* -------------------------------------------------------------------------
       Parameterized: OCI_BindArrayOfX returns a bind handle per datatype
       ------------------------------------------------------------------------- */

    struct StmtArrBindCase
    {
        const char* name;
        void (*run)(OCI_Environment*, OCI_Statement*);
    };

    void PrintTo(const StmtArrBindCase& c, std::ostream* os) { *os << c.name; }

    static void ArrBindPrep(OCI_Statement* s)
    {
        ASSERT_TRUE(OCI_StatementPrepare(s, OTEXT("select :v as v from dual")));
        ASSERT_TRUE(OCI_StatementBindArraySetSize(s, 1));
    }

    static void ArrBindInt(OCI_Environment*, OCI_Statement* s)
    {
        OCI_Int32 v[1] = { 1 };
        ArrBindPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindInt32Array(s, OTEXT(":v"), v, 0));
    }
    static void ArrBindUInt(OCI_Environment*, OCI_Statement* s)
    {
        OCI_UInt32 v[1] = { 2 };
        ArrBindPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindUInt32Array(s, OTEXT(":v"), v, 0));
    }
    static void ArrBindShort(OCI_Environment*, OCI_Statement* s)
    {
        OCI_Int16 v[1] = { 3 };
        ArrBindPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindInt16Array(s, OTEXT(":v"), v, 0));
    }
    static void ArrBindUShort(OCI_Environment*, OCI_Statement* s)
    {
        OCI_UInt16 v[1] = { 4 };
        ArrBindPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindUInt16Array(s, OTEXT(":v"), v, 0));
    }
    static void ArrBindBigInt(OCI_Environment*, OCI_Statement* s)
    {
        OCI_Int64 v[1] = { 5 };
        ArrBindPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindInt64Array(s, OTEXT(":v"), v, 0));
    }
    static void ArrBindUBigInt(OCI_Environment*, OCI_Statement* s)
    {
        OCI_UInt64 v[1] = { 6 };
        ArrBindPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindUInt64Array(s, OTEXT(":v"), v, 0));
    }
    static void ArrBindDouble(OCI_Environment*, OCI_Statement* s)
    {
        OCI_Double v[1] = { 8.5 };
        ArrBindPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindDoubleArray(s, OTEXT(":v"), v, 0));
    }
    static void ArrBindFloat(OCI_Environment*, OCI_Statement* s)
    {
        OCI_Float v[1] = { 7.5f };
        ArrBindPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindFloatArray(s, OTEXT(":v"), v, 0));
    }
    static void ArrBindNumber(OCI_Environment* env, OCI_Statement* s)
    {
        const auto num = OCI_NumberCreate(env);
        OCI_Int32 i = 1;
        ASSERT_TRUE(OCI_NumberSetValue(num, OCI_NUM_INT32, &i));
        OCI_Number* v[1] = { num };
        ArrBindPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindNumberArray(s, OTEXT(":v"), v, 0));
        ASSERT_TRUE(OCI_NumberFree(num));
    }
    static void ArrBindString(OCI_Environment*, OCI_Statement* s)
    {
        otext v[1][64] = { { OTEXT("text") } };
        ArrBindPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindStringArray(s, OTEXT(":v"), v[0], 64, 0));
    }
    static void ArrBindRaw(OCI_Environment*, OCI_Statement* s)
    {
        unsigned char v[1][20] = { { 0x01, 0x02, 0x03 } };
        ArrBindPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindRawArray(s, OTEXT(":v"), v[0], 20, 0));
    }
    static void ArrBindDate(OCI_Environment* env, OCI_Statement* s)
    {
        const auto date = OCI_DateCreate(env);
        OCI_Date* v[1] = { date };
        ArrBindPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindDateArray(s, OTEXT(":v"), v, 0));
        ASSERT_TRUE(OCI_DateFree(date));
    }
    static void ArrBindTimestamp(OCI_Environment* env, OCI_Statement* s)
    {
        const auto ts = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        OCI_Timestamp* v[1] = { ts };
        ArrBindPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindTimestampArray(s, OTEXT(":v"), v, OCI_TIMESTAMP, 0));
        ASSERT_TRUE(OCI_TimestampFree(ts));
    }
    static void ArrBindInterval(OCI_Environment* env, OCI_Statement* s)
    {
        const auto iv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        OCI_Interval* v[1] = { iv };
        ArrBindPrep(s);
        EXPECT_NE(nullptr, OCI_StatementBindIntervalArray(s, OTEXT(":v"), v, OCI_INTERVAL_DS, 0));
        ASSERT_TRUE(OCI_IntervalFree(iv));
    }

    class TestStmtArrBind : public ::testing::TestWithParam<StmtArrBindCase>
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_Statement* stmt = nullptr;

    public:
        static void SetUpTestCase()
        {
            ExecDML(OTEXT("create table TestStatementArr (i number, u number, sh number, us number, bi number, bui number, f number, db number, n number, str varchar2(50), raw_col raw(20), d date, ts timestamp, iv interval day to second)"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestStatementArr"));
        }

    protected:
        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);
        }

        void TearDown() override
        {
            if (stmt) OCI_StatementFree(stmt);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_P(TestStmtArrBind, BindArrayByName_ReturnsBindHandle)
    {
        GetParam().run(GetEnvironment(nullptr), stmt);
    }

    INSTANTIATE_TEST_CASE_P(ArrBindType, TestStmtArrBind,
        ::testing::Values(
            StmtArrBindCase{ "Int",       ArrBindInt },
            StmtArrBindCase{ "UInt",      ArrBindUInt },
            StmtArrBindCase{ "Short",     ArrBindShort },
            StmtArrBindCase{ "UShort",    ArrBindUShort },
            StmtArrBindCase{ "BigInt",    ArrBindBigInt },
            StmtArrBindCase{ "UBigInt",   ArrBindUBigInt },
            StmtArrBindCase{ "Double",    ArrBindDouble },
            StmtArrBindCase{ "Float",     ArrBindFloat },
            StmtArrBindCase{ "Number",    ArrBindNumber },
            StmtArrBindCase{ "String",    ArrBindString },
            StmtArrBindCase{ "Raw",       ArrBindRaw },
            StmtArrBindCase{ "Date",      ArrBindDate },
            StmtArrBindCase{ "Timestamp", ArrBindTimestamp },
            StmtArrBindCase{ "Interval",  ArrBindInterval }),
        ::testing::PrintToStringParamName());

    TEST_F(TestStmtArrBind, ExecuteInsertWithAllArrayBinds_InsertsOneRow)
    {
        const auto env = GetEnvironment(nullptr);
        const auto num = OCI_NumberCreate(env);
        const auto date = OCI_DateCreate(env);
        const auto ts = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        const auto iv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        OCI_Int32 i_arr[1] = { 1 };
        OCI_UInt32 u_arr[1] = { 2 };
        OCI_Int16 sh_arr[1] = { 3 };
        OCI_UInt16 us_arr[1] = { 4 };
        OCI_Int64 bi_arr[1] = { 5 };
        OCI_UInt64 bui_arr[1] = { 6 };
        OCI_Float f_arr[1] = { 7.5f };
        OCI_Double db_arr[1] = { 8.5 };
        OCI_Number* n_arr[1] = { num };
        OCI_Date* d_arr[1] = { date };
        OCI_Timestamp* ts_arr[1] = { ts };
        OCI_Interval* iv_arr[1] = { iv };
        otext str_arr[1][64] = { { OTEXT("text") } };
        unsigned char raw_arr[1][20] = { { 0x01, 0x02, 0x03 } };
        OCI_Int32 n = 1;
        ASSERT_TRUE(OCI_NumberSetValue(num, OCI_NUM_INT32, &n));
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into TestStatementArr (i, u, sh, us, bi, bui, f, db, n, str, raw_col, d, ts, iv) values (:i, :u, :sh, :us, :bi, :bui, :f, :db, :n, :str, :raw_col, :d, :ts, :iv)")));
        ASSERT_TRUE(OCI_StatementBindArraySetSize(stmt, 1));
        ASSERT_NE(nullptr, OCI_StatementBindInt32Array(stmt, OTEXT(":i"), i_arr, 0));
        ASSERT_NE(nullptr, OCI_StatementBindUInt32Array(stmt, OTEXT(":u"), u_arr, 0));
        ASSERT_NE(nullptr, OCI_StatementBindInt16Array(stmt, OTEXT(":sh"), sh_arr, 0));
        ASSERT_NE(nullptr, OCI_StatementBindUInt16Array(stmt, OTEXT(":us"), us_arr, 0));
        ASSERT_NE(nullptr, OCI_StatementBindInt64Array(stmt, OTEXT(":bi"), bi_arr, 0));
        ASSERT_NE(nullptr, OCI_StatementBindUInt64Array(stmt, OTEXT(":bui"), bui_arr, 0));
        ASSERT_NE(nullptr, OCI_StatementBindDoubleArray(stmt, OTEXT(":db"), db_arr, 0));
        ASSERT_NE(nullptr, OCI_StatementBindFloatArray(stmt, OTEXT(":f"), f_arr, 0));
        ASSERT_NE(nullptr, OCI_StatementBindNumberArray(stmt, OTEXT(":n"), n_arr, 0));
        ASSERT_NE(nullptr, OCI_StatementBindStringArray(stmt, OTEXT(":str"), str_arr[0], 64, 0));
        ASSERT_NE(nullptr, OCI_StatementBindRawArray(stmt, OTEXT(":raw_col"), raw_arr[0], 20, 0));
        ASSERT_NE(nullptr, OCI_StatementBindDateArray(stmt, OTEXT(":d"), d_arr, 0));
        ASSERT_NE(nullptr, OCI_StatementBindTimestampArray(stmt, OTEXT(":ts"), ts_arr, OCI_TIMESTAMP, 0));
        ASSERT_NE(nullptr, OCI_StatementBindIntervalArray(stmt, OTEXT(":iv"), iv_arr, OCI_INTERVAL_DS, 0));

        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        EXPECT_EQ(1u, OCI_StatementGetAffectedRows(stmt));

        ASSERT_TRUE(OCI_NumberFree(num));
        ASSERT_TRUE(OCI_DateFree(date));
        ASSERT_TRUE(OCI_TimestampFree(ts));
        ASSERT_TRUE(OCI_IntervalFree(iv));
    }

    /* -------------------------------------------------------------------------
       Parameterized: OCI_RegisterX succeeds per out-bind datatype
       ------------------------------------------------------------------------- */

    struct StmtRegCase
    {
        const char* name;
        void (*run)(OCI_Statement*);
    };

    void PrintTo(const StmtRegCase& c, std::ostream* os) { *os << c.name; }

    static void RegPrep(OCI_Statement* s, const otext* lit)
    {
        otext buf[160];
        otext* dst = buf;
        const otext* p = OTEXT("BEGIN :v := ");
        while (*p) *dst++ = *p++;
        p = lit;
        while (*p) *dst++ = *p++;
        p = OTEXT("; END;");
        while (*p) *dst++ = *p++;
        *dst = 0;
        ASSERT_TRUE(OCI_StatementPrepare(s, buf));
    }

    static void RegInt(OCI_Statement* s)
    {
        RegPrep(s, OTEXT("1"));
        EXPECT_TRUE(OCI_StatementRegisterInt32(s, OTEXT(":v")));
    }
    static void RegUInt(OCI_Statement* s)
    {
        RegPrep(s, OTEXT("1"));
        EXPECT_TRUE(OCI_StatementRegisterUInt32(s, OTEXT(":v")));
    }
    static void RegShort(OCI_Statement* s)
    {
        RegPrep(s, OTEXT("1"));
        EXPECT_TRUE(OCI_StatementRegisterInt16(s, OTEXT(":v")));
    }
    static void RegUShort(OCI_Statement* s)
    {
        RegPrep(s, OTEXT("1"));
        EXPECT_TRUE(OCI_StatementRegisterUInt16(s, OTEXT(":v")));
    }
    static void RegBigInt(OCI_Statement* s)
    {
        RegPrep(s, OTEXT("1"));
        EXPECT_TRUE(OCI_StatementRegisterInt64(s, OTEXT(":v")));
    }
    static void RegDouble(OCI_Statement* s)
    {
        RegPrep(s, OTEXT("1.5"));
        EXPECT_TRUE(OCI_StatementRegisterDouble(s, OTEXT(":v")));
    }
    static void RegFloat(OCI_Statement* s)
    {
        RegPrep(s, OTEXT("1.5"));
        EXPECT_TRUE(OCI_StatementRegisterFloat(s, OTEXT(":v")));
    }
    static void RegNumber(OCI_Statement* s)
    {
        RegPrep(s, OTEXT("1"));
        EXPECT_TRUE(OCI_StatementRegisterNumber(s, OTEXT(":v")));
    }
    static void RegString(OCI_Statement* s)
    {
        RegPrep(s, OTEXT("'x'"));
        EXPECT_TRUE(OCI_StatementRegisterString(s, OTEXT(":v"), 20));
    }
    static void RegDate(OCI_Statement* s)
    {
        RegPrep(s, OTEXT("sysdate"));
        EXPECT_TRUE(OCI_StatementRegisterDate(s, OTEXT(":v")));
    }
    static void RegTimestamp(OCI_Statement* s)
    {
        RegPrep(s, OTEXT("systimestamp"));
        EXPECT_TRUE(OCI_StatementRegisterTimestamp(s, OTEXT(":v"), OCI_TIMESTAMP));
    }
    static void RegInterval(OCI_Statement* s)
    {
        RegPrep(s, OTEXT("interval '1' day"));
        EXPECT_TRUE(OCI_StatementRegisterInterval(s, OTEXT(":v"), OCI_INTERVAL_DS));
    }

    class TestStmtReg : public ::testing::TestWithParam<StmtRegCase>
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_Statement* stmt = nullptr;

        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);
        }

        void TearDown() override
        {
            if (stmt) OCI_StatementFree(stmt);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_P(TestStmtReg, RegisterByName_ReturnsTrue)
    {
        GetParam().run(stmt);
    }

    INSTANTIATE_TEST_CASE_P(RegType, TestStmtReg,
        ::testing::Values(
            StmtRegCase{ "Int",       RegInt },
            StmtRegCase{ "UInt",      RegUInt },
            StmtRegCase{ "Short",     RegShort },
            StmtRegCase{ "UShort",    RegUShort },
            StmtRegCase{ "BigInt",    RegBigInt },
            StmtRegCase{ "Double",    RegDouble },
            StmtRegCase{ "Float",     RegFloat },
            StmtRegCase{ "Number",    RegNumber },
            StmtRegCase{ "String",    RegString },
            StmtRegCase{ "Date",      RegDate },
            StmtRegCase{ "Timestamp", RegTimestamp },
            StmtRegCase{ "Interval",  RegInterval }),
        ::testing::PrintToStringParamName());

    TEST(TestStatement, ExecuteAndGetResultset)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("select 1 as n from dual")));

        const auto rs = OCI_StatementExecutePreparedAndGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        ASSERT_EQ(1, OCI_ResultsetGetInt32(rs, 0));

        // re-executing a statement that holds a resultset releases it first
        const auto rs2 = OCI_StatementExecuteAndGetResultset(stmt, OTEXT("select 2 as n from dual"));
        ASSERT_NE(nullptr, rs2);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs2));
        ASSERT_EQ(2, OCI_ResultsetGetInt32(rs2, 0));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestStatement, PrepareFmt)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementPrepareFormat(stmt, OTEXT("select %i as n, %s as s from dual"), 7, OTEXT("txt")));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        ASSERT_EQ(7, OCI_ResultsetGetInt32(rs, 0));
        ASSERT_EQ(String(OTEXT("txt")), String(OCI_ResultsetGetString(rs, 1)));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestStatement, SqlCommandAndVerb)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        // not executed yet: getter must fail
        ASSERT_EQ(OCI_UNKNOWN, (OCI_Int32) OCI_StatementGetSqlCommand(stmt));

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select 1 from dual")));
        ASSERT_EQ(OCI_SFC_SELECT, OCI_StatementGetSqlCommand(stmt));
        ASSERT_EQ(String(OTEXT("SELECT")), String(OCI_StatementGetSqlVerb(stmt)));

        ExecDML(OTEXT("drop table TestStatementCmd"));
        ExecDML(OTEXT("create table TestStatementCmd (id int)"));

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("insert into TestStatementCmd values (1)")));
        ASSERT_EQ(OCI_SFC_INSERT, OCI_StatementGetSqlCommand(stmt));
        ASSERT_EQ(String(OTEXT("INSERT")), String(OCI_StatementGetSqlVerb(stmt)));

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("update TestStatementCmd set id = 2")));
        ASSERT_EQ(String(OTEXT("UPDATE")), String(OCI_StatementGetSqlVerb(stmt)));

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("drop table TestStatementCmd")));
        ASSERT_EQ(String(OTEXT("DROP TABLE")), String(OCI_StatementGetSqlVerb(stmt)));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    /* --- OCI_ConnectionImmediate argument types --- */

    class TestStatementImmediate : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("drop table TestImmediateTbl"));
            ExecDML(OTEXT("drop type TestImmediateColl"));
            ExecDML(OTEXT("drop type TestImmediateObj"));
            ExecDML(OTEXT("create type TestImmediateObj as object(code int, name varchar2(30))"));
            ExecDML(OTEXT("create type TestImmediateColl as varray(4) of number"));
            ExecDML(OTEXT("create table TestImmediateTbl of TestImmediateObj"));
            ExecDML(OTEXT("insert into TestImmediateTbl values (TestImmediateObj(7, 'seven'))"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestImmediateTbl"));
            ExecDML(OTEXT("drop type TestImmediateColl"));
            ExecDML(OTEXT("drop type TestImmediateObj"));
        }

        void SetUp() override
        {
            env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, con);
        }

        void TearDown() override
        {
            OCI_ConnectionFree(con);
        }

        OCI_Environment* env{ nullptr };
        OCI_Connection*  con{ nullptr };
    };

    /* -------------------------------------------------------------------------
       Parameterized: OCI_ConnectionImmediate fetches one column per OCI_ARG_* type
       ------------------------------------------------------------------------- */

    struct ImmCase
    {
        const char* name;
        void (*run)(OCI_Environment*, OCI_Connection*);
    };

    void PrintTo(const ImmCase& c, std::ostream* os) { *os << c.name; }

    static void ImmText(OCI_Environment*, OCI_Connection* con)
    {
        otext v[64] = {};
        ASSERT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select 'abc' from dual"), OCI_ARG_TEXT, v));
        EXPECT_EQ(String(OTEXT("abc")), String(v));
    }
    static void ImmShort(OCI_Environment*, OCI_Connection* con)
    {
        OCI_Int16 v = 0;
        ASSERT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select cast(1 as number(4)) from dual"), OCI_ARG_INT16, &v));
        EXPECT_EQ(1, v);
    }
    static void ImmUShort(OCI_Environment*, OCI_Connection* con)
    {
        OCI_UInt16 v = 0;
        ASSERT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select cast(2 as number(4)) from dual"), OCI_ARG_UINT16, &v));
        EXPECT_EQ(2, v);
    }
    static void ImmInt(OCI_Environment*, OCI_Connection* con)
    {
        OCI_Int32 v = 0;
        ASSERT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select cast(3 as number(9)) from dual"), OCI_ARG_INT32, &v));
        EXPECT_EQ(3, v);
    }
    static void ImmUInt(OCI_Environment*, OCI_Connection* con)
    {
        OCI_UInt32 v = 0;
        ASSERT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select cast(4 as number(9)) from dual"), OCI_ARG_UINT32, &v));
        EXPECT_EQ(4u, v);
    }
    static void ImmBigInt(OCI_Environment*, OCI_Connection* con)
    {
        OCI_Int64 v = 0;
        ASSERT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select cast(5 as number(19)) from dual"), OCI_ARG_INT64, &v));
        EXPECT_EQ(5, v);
    }
    static void ImmBigUInt(OCI_Environment*, OCI_Connection* con)
    {
        OCI_UInt64 v = 0;
        ASSERT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select cast(6 as number(19)) from dual"), OCI_ARG_UINT64, &v));
        EXPECT_EQ(6u, v);
    }
    static void ImmDouble(OCI_Environment*, OCI_Connection* con)
    {
        OCI_Double v = 0.0;
        ASSERT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select cast(1.5 as binary_double) from dual"), OCI_ARG_DOUBLE, &v));
        EXPECT_GT(v, 0.0);
    }
    static void ImmFloat(OCI_Environment*, OCI_Connection* con)
    {
        OCI_Float v = 0.0f;
        ASSERT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select cast(2.5 as binary_float) from dual"), OCI_ARG_FLOAT, &v));
        EXPECT_GT(v, 0.0f);
    }
    static void ImmNumber(OCI_Environment* env, OCI_Connection* con)
    {
        const auto v = OCI_NumberCreate(env);
        ASSERT_NE(nullptr, v);
        ASSERT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select cast(9 as number) from dual"), OCI_ARG_NUMBER, v));
        OCI_Double d = 0.0;
        ASSERT_TRUE(OCI_NumberGetValue(v, OCI_NUM_DOUBLE, &d));
        EXPECT_GT(d, 0.0);
        ASSERT_TRUE(OCI_NumberFree(v));
    }
    static void ImmDatetime(OCI_Environment* env, OCI_Connection* con)
    {
        const auto v = OCI_DateCreate(env);
        ASSERT_NE(nullptr, v);
        EXPECT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select sysdate from dual"), OCI_ARG_DATETIME, v));
        ASSERT_TRUE(OCI_DateFree(v));
    }
    static void ImmRaw(OCI_Environment*, OCI_Connection* con)
    {
        unsigned char v[64] = {};
        ASSERT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select hextoraw('AABB') from dual"), OCI_ARG_RAW, v));
        EXPECT_EQ(0xAA, v[0]);
    }
    static void ImmLob(OCI_Environment*, OCI_Connection* con)
    {
        const auto v = OCI_LobCreate(con, OCI_CLOB);
        ASSERT_NE(nullptr, v);
        ASSERT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select to_clob('lobdata') from dual"), OCI_ARG_LOB, v));
        EXPECT_GT(OCI_LobGetLength(v), 0u);
        ASSERT_TRUE(OCI_LobFree(v));
    }
    static void ImmFile(OCI_Environment*, OCI_Connection* con)
    {
        const auto v = OCI_FileCreate(con, OCI_BFILE);
        ASSERT_NE(nullptr, v);
        EXPECT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select bfilename('FAKE_DIR', 'f.txt') from dual"), OCI_ARG_FILE, v));
        ASSERT_TRUE(OCI_FileFree(v));
    }
    static void ImmTimestamp(OCI_Environment* env, OCI_Connection* con)
    {
        const auto v = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        ASSERT_NE(nullptr, v);
        EXPECT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select systimestamp from dual"), OCI_ARG_TIMESTAMP, v));
        ASSERT_TRUE(OCI_TimestampFree(v));
    }
    static void ImmInterval(OCI_Environment* env, OCI_Connection* con)
    {
        const auto v = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        ASSERT_NE(nullptr, v);
        EXPECT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select interval '1 02:03:04' day to second from dual"), OCI_ARG_INTERVAL, v));
        ASSERT_TRUE(OCI_IntervalFree(v));
    }
    static void ImmObject(OCI_Environment*, OCI_Connection* con)
    {
        const auto typinf = OCI_TypeInfoGet(con, OTEXT("TestImmediateObj"), OCI_TIF_TYPE);
        const auto v = OCI_ObjectCreate(typinf);
        ASSERT_NE(nullptr, v);
        ASSERT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select TestImmediateObj(1, 'o') from dual"), OCI_ARG_OBJECT, v));
        EXPECT_EQ(1, OCI_ObjectGetInt32(v, OTEXT("CODE")));
        ASSERT_TRUE(OCI_ObjectFree(v));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
    }
    static void ImmColl(OCI_Environment*, OCI_Connection* con)
    {
        const auto typinf = OCI_TypeInfoGet(con, OTEXT("TestImmediateColl"), OCI_TIF_TYPE);
        const auto v = OCI_CollectionCreate(typinf);
        ASSERT_NE(nullptr, v);
        ASSERT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select TestImmediateColl(1, 2) from dual"), OCI_ARG_COLLECTION, v));
        EXPECT_EQ(2u, OCI_CollectionGetCount(v));
        ASSERT_TRUE(OCI_CollectionFree(v));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
    }
    static void ImmRef(OCI_Environment*, OCI_Connection* con)
    {
        const auto typinf = OCI_TypeInfoGet(con, OTEXT("TestImmediateObj"), OCI_TIF_TYPE);
        const auto v = OCI_ReferenceCreate(typinf);
        ASSERT_NE(nullptr, v);
        ASSERT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select ref(t) from TestImmediateTbl t"), OCI_ARG_REF, v));
        EXPECT_FALSE(OCI_ReferenceIsNull(v));
        ASSERT_TRUE(OCI_ReferenceFree(v));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
    }

    class TestStmtImm : public ::testing::TestWithParam<ImmCase>
    {
    protected:
        OCI_Connection* conn = nullptr;

    public:
        static void SetUpTestCase()
        {
            ExecDML(OTEXT("drop table TestImmediateTbl"));
            ExecDML(OTEXT("drop type TestImmediateColl"));
            ExecDML(OTEXT("drop type TestImmediateObj"));
            ExecDML(OTEXT("create type TestImmediateObj as object(code int, name varchar2(30))"));
            ExecDML(OTEXT("create type TestImmediateColl as varray(4) of number"));
            ExecDML(OTEXT("create table TestImmediateTbl of TestImmediateObj"));
            ExecDML(OTEXT("insert into TestImmediateTbl values (TestImmediateObj(7, 'seven'))"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestImmediateTbl"));
            ExecDML(OTEXT("drop type TestImmediateColl"));
            ExecDML(OTEXT("drop type TestImmediateObj"));
        }

    protected:
        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
        }

        void TearDown() override
        {
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_P(TestStmtImm, ImmediateSelect_FetchesTypedValue)
    {
        GetParam().run(GetEnvironment(nullptr), conn);
    }

    INSTANTIATE_TEST_CASE_P(ArgType, TestStmtImm,
        ::testing::Values(
            ImmCase{ "Text",      ImmText },
            ImmCase{ "Short",     ImmShort },
            ImmCase{ "UShort",    ImmUShort },
            ImmCase{ "Int",       ImmInt },
            ImmCase{ "UInt",      ImmUInt },
            ImmCase{ "BigInt",    ImmBigInt },
            ImmCase{ "BigUInt",   ImmBigUInt },
            ImmCase{ "Double",    ImmDouble },
            ImmCase{ "Float",     ImmFloat },
            ImmCase{ "Number",    ImmNumber },
            ImmCase{ "Datetime",  ImmDatetime },
            ImmCase{ "Raw",       ImmRaw },
            ImmCase{ "Lob",       ImmLob },
            ImmCase{ "File",      ImmFile },
            ImmCase{ "Timestamp", ImmTimestamp },
            ImmCase{ "Interval",  ImmInterval },
            ImmCase{ "Object",    ImmObject },
            ImmCase{ "Coll",      ImmColl },
            ImmCase{ "Ref",       ImmRef }),
        ::testing::PrintToStringParamName());

    TEST_F(TestStatementImmediate, ImmediateWithTextSizeArg_FetchesSizedText)
    {
        otext txtn[8] = {};

        // size-carrying text arg
        ASSERT_TRUE(OCI_ConnectionImmediate(con, OTEXT("select 'bounded' from dual"),
                                  OCI_ARG_TEXT_N, txtn, (OCI_UInt32) 8));
        ASSERT_EQ(String(OTEXT("bounded")), String(txtn));
    }

    TEST_F(TestStatementImmediate, ImmediateWithJsonArg_FetchesJson)
    {
        const auto json = OCI_JsonCreate(con);
        ASSERT_NE(nullptr, json);

        ASSERT_TRUE(OCI_ConnectionImmediate(con,
            OTEXT("select json('{\"a\":1}') from dual"),
            OCI_ARG_JSON, json));

        OCI_UInt32 jsize = 0;
        ASSERT_TRUE(OCI_JsonToText(json, nullptr, &jsize, 0));
        ASSERT_GT(jsize, 0u);

        ASSERT_TRUE(OCI_JsonFree(json));
    }

    TEST_F(TestStatementImmediate, ImmediateWithVectorArg_FetchesVector)
    {
        const auto vect = OCI_VectorCreate(con);
        ASSERT_NE(nullptr, vect);

        ASSERT_TRUE(OCI_ConnectionImmediate(con,
            OTEXT("select vector('[1.5,2.5,3.5]', 3, float32) from dual"),
            OCI_ARG_VECTOR, vect));

        OCI_UInt32 dimensions = 0;
        OCI_UInt32 format     = OCI_VEC_FLEX;
        ASSERT_TRUE(OCI_VectorGetInfo(vect, &format, &dimensions));
        ASSERT_EQ(3u, dimensions);
        ASSERT_EQ(OCI_VEC_FLOAT32, format);

        ASSERT_TRUE(OCI_VectorFree(vect));
    }

    TEST_F(TestStatementImmediate, ImmediateWithUnknownArgTag_Fails)
    {
        OCI_Int32 dummy = 0;

        // unknown arg tag -> mapping error
        ASSERT_FALSE(OCI_ConnectionImmediate(con, OTEXT("select 1 from dual"), 4242, &dummy));
    }

    /* --- PL/SQL boolean and cursor binds --- */

    class TestStatementPlsqlBinds : public ::testing::Test
    {
    protected:

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, con);

            stmt = OCI_StatementCreate(con);
            ASSERT_NE(nullptr, stmt);

            sub = OCI_StatementCreate(con);
            ASSERT_NE(nullptr, sub);

            ASSERT_TRUE(OCI_StatementPrepare(stmt,
                OTEXT("begin :b := true; open :c for select 1 as n from dual; end;")));
            ASSERT_NE(nullptr, OCI_StatementBindBoolean(stmt, OTEXT(":b"), &b));
            ASSERT_NE(nullptr, OCI_StatementBindCursor(stmt, OTEXT(":c"), sub));
        }

        void TearDown() override
        {
            OCI_StatementFree(sub);
            OCI_StatementFree(stmt);
            OCI_ConnectionFree(con);
        }

        OCI_Connection* con{ nullptr };
        OCI_Statement*  stmt{ nullptr };
        OCI_Statement*  sub{ nullptr };
        OCI_Boolean     b{ FALSE };
    };

    TEST_F(TestStatementPlsqlBinds, PlsqlBooleanAndCursorBinds_RoundTrip)
    {
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));
        ASSERT_TRUE(b);

        const auto rs = OCI_StatementGetResultset(sub);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        ASSERT_EQ(1, OCI_ResultsetGetInt32(rs, 0));
    }

    TEST_F(TestStatementPlsqlBinds, ReexecuteAfterBind_ResetsStatement)
    {
        // second execute resets the bound statement (ALLOCATED_BIND_STMT path)
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));
    }

    /* --- complex bind arrays (file/ref/coll/vector) --- */

    class TestStatementComplexArrays : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("drop table TestStmtCArrTbl"));
            ExecDML(OTEXT("drop type TestStmtCArrColl"));
            ExecDML(OTEXT("drop type TestStmtCArrObj"));
            ExecDML(OTEXT("create type TestStmtCArrObj as object(code int)"));
            ExecDML(OTEXT("create type TestStmtCArrColl as varray(4) of number"));
            ExecDML(OTEXT("create table TestStmtCArrTbl (id int primary key, o TestStmtCArrObj)"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestStmtCArrTbl"));
            ExecDML(OTEXT("drop type TestStmtCArrColl"));
            ExecDML(OTEXT("drop type TestStmtCArrObj"));
        }

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, con);

            stmt = OCI_StatementCreate(con);
            ASSERT_NE(nullptr, stmt);
        }

        void TearDown() override
        {
            OCI_StatementFree(stmt);
            OCI_ConnectionFree(con);
        }

        void PrepareArrayBind(const otext* sql)
        {
            ASSERT_TRUE(OCI_StatementPrepare(stmt, sql));
            ASSERT_TRUE(OCI_StatementBindArraySetSize(stmt, 2));
        }

        OCI_Connection* con{ nullptr };
        OCI_Statement*  stmt{ nullptr };
    };

    TEST_F(TestStatementComplexArrays, BindFileOnInsert_BindsFile)
    {
        PrepareArrayBind(OTEXT("begin :f := :f; end;"));

        const auto file = OCI_FileCreate(con, OCI_BFILE);
        ASSERT_NE(nullptr, file);
        ASSERT_NE(nullptr, OCI_StatementBindFile(stmt, OTEXT(":f"), file));

        ASSERT_TRUE(OCI_FileFree(file));
    }

    TEST_F(TestStatementComplexArrays, BindArrayOfFilesOnInsert_BindsFiles)
    {
        PrepareArrayBind(OTEXT("begin :fa := :fa; end;"));

        const auto files = OCI_FileArrayCreate(con, OCI_BFILE, 2);
        ASSERT_NE(nullptr, files);
        ASSERT_NE(nullptr, OCI_StatementBindFileArray(stmt, OTEXT(":fa"), files, OCI_BFILE, 0));

        ASSERT_TRUE(OCI_FileArrayFree(files));
    }

    TEST_F(TestStatementComplexArrays, BindArrayOfRefsOnInsert_BindsRefs)
    {
        PrepareArrayBind(OTEXT("begin :r := :r; end;"));

        const auto typinf = OCI_TypeInfoGet(con, OTEXT("TestStmtCArrObj"), OCI_TIF_TYPE);
        const auto refs   = OCI_ReferenceArrayCreate(typinf, 2);
        ASSERT_NE(nullptr, refs);
        ASSERT_NE(nullptr, OCI_StatementBindReferenceArray(stmt, OTEXT(":r"), refs, typinf, 0));

        ASSERT_TRUE(OCI_ReferenceArrayFree(refs));
    }

    TEST_F(TestStatementComplexArrays, BindArrayOfCollsOnInsert_BindsColls)
    {
        PrepareArrayBind(OTEXT("begin :c := :c; end;"));

        const auto typinf = OCI_TypeInfoGet(con, OTEXT("TestStmtCArrColl"), OCI_TIF_TYPE);
        const auto colls  = OCI_CollectionArrayCreate(typinf, 2);
        ASSERT_NE(nullptr, colls);
        ASSERT_NE(nullptr, OCI_StatementBindCollectionArray(stmt, OTEXT(":c"), colls, typinf, 0));

        ASSERT_TRUE(OCI_CollectionArrayFree(colls));
    }

    TEST_F(TestStatementComplexArrays, BindArrayOfVectorsOnInsert_BindsVectors)
    {
        PrepareArrayBind(OTEXT("begin :v := :v; end;"));

        const auto vects = OCI_VectorArrayCreate(con, 2);
        ASSERT_NE(nullptr, vects);
        ASSERT_NE(nullptr, OCI_StatementBindVectorArray(stmt, OTEXT(":v"), vects, 0));

        ASSERT_TRUE(OCI_VectorArrayFree(vects));
    }

    /* --- bind error paths --- */

    class TestStatementBindErrors : public ::testing::Test
    {
    protected:

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, con);

            stmt = OCI_StatementCreate(con);
            ASSERT_NE(nullptr, stmt);
        }

        void TearDown() override
        {
            OCI_StatementFree(stmt);
            OCI_ConnectionFree(con);
        }

        OCI_Connection* con{ nullptr };
        OCI_Statement*  stmt{ nullptr };
    };

    TEST_F(TestStatementBindErrors, GrowArraySizeWithExistingBinds_Fails)
    {
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into t values (:x)")));

        ASSERT_TRUE(OCI_StatementBindArraySetSize(stmt, 5));
        OCI_Int32 i = 1;
        ASSERT_NE(nullptr, OCI_StatementBindInt32(stmt, OTEXT(":x"), &i));
        ASSERT_FALSE(OCI_StatementBindArraySetSize(stmt, 10));
    }

    TEST_F(TestStatementBindErrors, NullStringDataWithInternalAlloc_BindsNull)
    {
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into t values (:s)")));

        // NULL data with internal allocation and no usable length -> error
        ASSERT_TRUE(OCI_StatementSetBindAllocation(stmt, OCI_BAM_INTERNAL));
        ASSERT_EQ(nullptr, OCI_StatementBindString(stmt, OTEXT(":s"), NULL, 0));
    }

    TEST_F(TestStatementBindErrors, NullRawDataWithInternalAlloc_BindsNull)
    {
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into t values (:r)")));

        ASSERT_TRUE(OCI_StatementSetBindAllocation(stmt, OCI_BAM_INTERNAL));
        ASSERT_EQ(nullptr, OCI_StatementBindRaw(stmt, OTEXT(":r"), NULL, 0));
    }

    TEST_F(TestStatementBindErrors, GetBindByNameOnUnknown_ReturnsNull)
    {
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into t values (:x)")));
        ASSERT_EQ(nullptr, OCI_StatementGetBindByName(stmt, OTEXT(":nope")));
    }

    /* --- fetch mode on a legacy (9iR1) server version --- */





    /* --- autocommit and batch errors --- */

    class TestStatementBatch : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("drop table TestStatementBatch"));
            ExecDML(OTEXT("create table TestStatementBatch (id int not null)"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestStatementBatch"));
        }

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, con);

            stmt = OCI_StatementCreate(con);
            ASSERT_NE(nullptr, stmt);
        }

        void TearDown() override
        {
            OCI_StatementFree(stmt);
            OCI_ConnectionFree(con);
        }

        OCI_Connection* con{ nullptr };
        OCI_Statement*  stmt{ nullptr };
    };

    TEST_F(TestStatementBatch, ExecuteWithAutoCommit_CommitsBatch)
    {
        // autocommit path on DML execute
        ASSERT_TRUE(OCI_ConnectionSetAutoCommit(con, TRUE));
        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("insert into TestStatementBatch values (1)")));
        ASSERT_TRUE(OCI_ConnectionSetAutoCommit(con, FALSE));
    }

    TEST_F(TestStatementBatch, BatchExecuteWithErrors_ReportsErrorList)
    {
        // array DML with one failing row -> batch error list
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into TestStatementBatch values (:x)")));

        ASSERT_TRUE(OCI_StatementBindArraySetSize(stmt, 3));
        OCI_Int32 vals[3] = { 2, 0, 3 };
        ASSERT_NE(nullptr, OCI_StatementBindInt32Array(stmt, OTEXT(":x"), vals, 0));
        // NULL indicator on the second element
        ASSERT_TRUE(OCI_BindSetNullAtPos(OCI_StatementGetBind(stmt, 0), 1));

        ASSERT_FALSE(OCI_StatementExecutePrepared(stmt));
        ASSERT_GT(OCI_StatementGetBatchErrorCount(stmt), 0u);

        const auto err = OCI_StatementGetBatchError(stmt);
        ASSERT_NE(nullptr, err);
        ASSERT_EQ(nullptr, OCI_StatementGetBatchError(stmt));
    }


    TEST(TestStatement, PrefetchMemoryOnHandle)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("select 1 from dual")));

        // stmt->handle is allocated after prepare: hits the OCI_ATTR_PREFETCH_MEMORY set
        ASSERT_TRUE(OCI_StatementSetPrefetchMemory(stmt, 4096));
        ASSERT_EQ(4096u, OCI_StatementGetPrefetchMemory(stmt));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    /* --- public-API coverage of the dropped OcilibStatement* / OcilibBind* /
       OcilibResultset* internals (RegisterOutBindsFetch / prefetch arms):
       OCI_RegisterOut* / OCI_StatementRegisterDate / OCI_StatementRegisterString /
       OCI_StatementRegisterInt32 / OCI_StatementRegisterLob / OCI_StatementRegisterInterval /
       OCI_StatementRegisterUInt64 / OCI_StatementExecutePrepared / OCI_ResultsetGetStruct /
       OCI_ResultsetGetCursor / OCI_SetPrefetch* / OCI_GetPrefetch* --- */

    TEST(TestStatement, RegisterOutBindsFetchPublic)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        ExecDML(OTEXT("drop table TestRegisterOutTbl"));
        ExecDML(OTEXT("create table TestRegisterOutTbl (n number, s varchar2(50), d date, ub number)"));

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        /* OcilibStatement* out-bind registration + RETURNING INTO arms via
           OCI_Register* on a DML returning statement */
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into TestRegisterOutTbl values (1, 'one', sysdate, 2) returning n, s, d, ub into :n, :s, :d, :ub")));

        ASSERT_TRUE(OCI_StatementRegisterInt32(stmt, OTEXT(":n")));
        ASSERT_TRUE(OCI_StatementRegisterString(stmt, OTEXT(":s"), 50));
        ASSERT_TRUE(OCI_StatementRegisterDate(stmt, OTEXT(":d")));
        ASSERT_TRUE(OCI_StatementRegisterUInt64(stmt, OTEXT(":ub")));

        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestRegisterOutTbl"));
    }

    TEST(TestStatement, FetchModeScrollableGetSet)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("select 1 as n from dual")));

        /* OcilibStatement* / OcilibEnv* scrollable-cursor internals via
           OCI_StatementSetFetchMode / OCI_StatementGetFetchMode */
        ASSERT_TRUE(OCI_StatementSetFetchMode(stmt, OCI_SFM_SCROLLABLE));
        ASSERT_EQ((OCI_FetchMode)OCI_SFM_SCROLLABLE, OCI_StatementGetFetchMode(stmt));
        ASSERT_TRUE(OCI_StatementSetFetchMode(stmt, OCI_SFM_DEFAULT));
        ASSERT_EQ((OCI_FetchMode)OCI_SFM_DEFAULT, OCI_StatementGetFetchMode(stmt));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }






    TEST(TestStatement, Execute_InvalidSql_ReturnsFalseAndRaisesError)
    {
        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);
        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("select * from where bogus syntax x")));

        ASSERT_FALSE(OCI_StatementExecutePrepared(stmt));

        const auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);
        ASSERT_NE(0, OCI_ErrorGetCode(err));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestStatement, Execute_WithoutPrepare_ReturnsFalse)
    {
        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);
        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_FALSE(OCI_StatementExecutePrepared(stmt));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestStatement, GetBindByName_UnknownName_ReturnsNull)
    {
        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);
        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("begin :i := 42; end;")));

        ASSERT_EQ(nullptr, OCI_StatementGetBindByName(stmt, OTEXT(":does_not_exist")));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestStatement, GetNextResultset_PreClient12_Fails)
    {
        /* OCIStmtGetNextResult is a 12.1+ client feature */

        const auto env = GetSpecificEnvironment(nullptr, OCI_11_2);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);
        ASSERT_EQ(nullptr, OCI_StatementGetNextResultset(stmt));
        ASSERT_NE(nullptr, OCI_GetLastError());

        ASSERT_TRUE(OCI_StatementFree(stmt));

        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }


    TEST(TestStatement, Prepare_PreClient9_2_UsesPrepare1)
    {
        /* pre-9.2 clients have no OCIStmtPrepare2 -> Prepare falls back to
           OCIStmtPrepare and statements still work */

        const auto env = GetSpecificEnvironment(nullptr, OCI_9_0);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);
        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select 1 from dual")));
        ASSERT_NE(nullptr, OCI_StatementGetResultset(stmt));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }


    class TestStatementNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestStatementNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestStatementNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "AllowRebinding", [] { return !OCI_StatementAllowRebinding(nullptr, FALSE); } },
            { "Describe", [] { return !OCI_StatementDescribe(nullptr, OTEXT("x")); } },
            { "Execute", [] { return !OCI_StatementExecutePrepared(nullptr); } },
            { "ExecuteAndGetResultset", [] { return nullptr == OCI_StatementExecutePreparedAndGetResultset(nullptr); } },
            { "ExecuteStmt", [] { return !OCI_StatementExecute(nullptr, OTEXT("x")); } },
            { "ExecuteStmtAndGetResultset", [] { return nullptr == OCI_StatementExecuteAndGetResultset(nullptr, OTEXT("x")); } },
            { "GetAffectedRows", [] { return 0 == OCI_StatementGetAffectedRows(nullptr); } },
            { "GetBatchError", [] { return nullptr == OCI_StatementGetBatchError(nullptr); } },
            { "GetBatchErrorCount", [] { return 0 == OCI_StatementGetBatchErrorCount(nullptr); } },
            { "GetBind", [] { return nullptr == OCI_StatementGetBind(nullptr, 1); } },
            { "GetBindAllocation", [] { return OCI_UNKNOWN == OCI_StatementGetBindAllocation(nullptr); } },
            { "GetBindByName", [] { return nullptr == OCI_StatementGetBindByName(nullptr, OTEXT("x")); } },
            { "GetBindCount", [] { return 0 == OCI_StatementGetBindCount(nullptr); } },
            { "GetBindIndex", [] { return -1 == OCI_StatementGetBindIndex(nullptr, OTEXT("x")); } },
            { "GetBindMode", [] { return OCI_UNKNOWN == OCI_StatementGetBindMode(nullptr); } },
            { "GetFetchMode", [] { return OCI_UNKNOWN == OCI_StatementGetFetchMode(nullptr); } },
            { "GetFetchSize", [] { return 0 == OCI_StatementGetFetchSize(nullptr); } },
            { "GetLongMode", [] { return OCI_UNKNOWN == OCI_StatementGetLongMode(nullptr); } },
            { "GetNextResultset", [] { return nullptr == OCI_StatementGetNextResultset(nullptr); } },
            { "GetParseBindNames", [] { return !OCI_StatementGetParseBindNames(nullptr, NullArgOut<OCI_UInt32>(), nullptr /* otext *** */); } },
            { "GetPieceSize", [] { return 0 == OCI_StatementGetPieceSize(nullptr); } },
            { "GetPrefetchMemory", [] { return 0 == OCI_StatementGetPrefetchMemory(nullptr); } },
            { "GetPrefetchSize", [] { return 0 == OCI_StatementGetPrefetchSize(nullptr); } },
            { "GetResultset", [] { return nullptr == OCI_StatementGetResultset(nullptr); } },
            { "GetSQLCommand", [] { return OCI_UNKNOWN == OCI_StatementGetSqlCommand(nullptr); } },
            { "GetSQLVerb", [] { return nullptr == OCI_StatementGetSqlVerb(nullptr); } },
            { "GetSql", [] { return nullptr == OCI_StatementGetSql(nullptr); } },
            { "GetSqlErrorPos", [] { return 0 == OCI_StatementGetSqlErrorPos(nullptr); } },
            { "GetSqlIdentifier", [] { return nullptr == OCI_StatementGetSqlIdentifier(nullptr); } },
            { "GetStatementType", [] { return OCI_UNKNOWN == OCI_StatementGetType(nullptr); } },
            { "IsRebindingAllowed", [] { return !OCI_StatementIsRebindingAllowed(nullptr); } },
            { "Parse", [] { return !OCI_StatementParse(nullptr, OTEXT("x")); } },
            { "Prepare", [] { return !OCI_StatementPrepare(nullptr, OTEXT("x")); } },
            { "ReleaseResultsets", [] { return !OCI_StatementReleaseResultsets(nullptr); } },
            { "SetBindAllocation", [] { return !OCI_StatementSetBindAllocation(nullptr, 1); } },
            { "SetBindMode", [] { return !OCI_StatementSetBindMode(nullptr, 1); } },
            { "SetFetchMode", [] { return !OCI_StatementSetFetchMode(nullptr, 1); } },
            { "SetFetchSize", [] { return !OCI_StatementSetFetchSize(nullptr, 1); } },
            { "SetLongMode", [] { return !OCI_StatementSetLongMode(nullptr, 1); } },
            { "SetPieceSize", [] { return !OCI_StatementSetPieceSize(nullptr, 1); } },
            { "SetPrefetchMemory", [] { return !OCI_StatementSetPrefetchMemory(nullptr, 1); } },
            { "SetPrefetchSize", [] { return !OCI_StatementSetPrefetchSize(nullptr, 1); } },
            { "StatementFree", [] { return !OCI_StatementFree(nullptr); } },
            { "StatementGetConnection", [] { return nullptr == OCI_StatementGetConnection(nullptr); } },
        }),
        &NullArgCaseName);

}
