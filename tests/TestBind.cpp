#include "ocilib_tests.h"
#include "NullArgs.h"


namespace TestCApi
{
    static Context context;

    static inline void AddError(OCI_Error* err)
    {
        context.Errs.emplace_back(Error{ OCI_ErrorGetCode(err), OCI_ErrorGetCategory(err) });
    }

    /* -------------------------------------------------------------------------
       Shared fixture: prepared insert with int + string scalar binds
       ------------------------------------------------------------------------- */

    class TestBindBasic : public ::testing::Test
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_Statement* stmt = nullptr;
        OCI_Int32 id = 42;
        otext name[64] = OTEXT("hello");

    public:
        static void SetUpTestCase()
        {
            ExecDML(OTEXT("create table TestBind_Table (id int, name varchar2(50))"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestBind_Table"));
        }

    protected:
        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);
            ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into TestBind_Table values (:id, :name)")));
            ASSERT_NE(nullptr, OCI_StatementBindInt32(stmt, OTEXT(":id"), &id));
            ASSERT_NE(nullptr, OCI_StatementBindString(stmt, OTEXT(":name"), name, 50));
        }

        void TearDown() override
        {
            if (stmt) OCI_StatementFree(stmt);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_F(TestBindBasic, BoundHandle_ReportsNameAndStatement)
    {
        const auto bnd_id = OCI_StatementGetBind(stmt, 0);
        const auto bnd_name = OCI_StatementGetBindByName(stmt, OTEXT(":name"));

        ASSERT_NE(nullptr, bnd_id);
        ASSERT_NE(nullptr, bnd_name);
        EXPECT_EQ(String(OTEXT(":id")), String(OCI_BindGetName(bnd_id)));
        EXPECT_EQ(String(OTEXT(":name")), String(OCI_BindGetName(bnd_name)));
        EXPECT_EQ(stmt, OCI_BindGetStatement(bnd_id));
    }

    TEST_F(TestBindBasic, BoundHandle_ReportsTypeDirectionAndDataCount)
    {
        const auto bnd_id = OCI_StatementGetBind(stmt, 0);
        const auto bnd_name = OCI_StatementGetBindByName(stmt, OTEXT(":name"));

        ASSERT_NE(nullptr, bnd_id);
        ASSERT_NE(nullptr, bnd_name);
        EXPECT_EQ((OCI_UInt32) OCI_BDM_IN_OUT, OCI_BindGetDirection(bnd_id));
        EXPECT_EQ((OCI_UInt32) OCI_CDT_NUMERIC, OCI_BindGetType(bnd_id));
        EXPECT_EQ((OCI_UInt32) OCI_CDT_TEXT, OCI_BindGetType(bnd_name));
        EXPECT_EQ(1u, OCI_BindGetDataCount(bnd_id));
    }

    TEST_F(TestBindBasic, BoundHandle_GetDataReturnsBoundBuffers)
    {
        const auto bnd_id = OCI_StatementGetBind(stmt, 0);
        const auto bnd_name = OCI_StatementGetBindByName(stmt, OTEXT(":name"));

        ASSERT_NE(nullptr, bnd_id);
        ASSERT_NE(nullptr, bnd_name);
        EXPECT_EQ(id, *static_cast<OCI_Int32*>(OCI_BindGetData(bnd_id)));
        EXPECT_EQ(String(name), String(static_cast<otext*>(OCI_BindGetData(bnd_name))));
    }

    TEST_F(TestBindBasic, ExecuteInsertThenSelect_ReturnsBoundValues)
    {
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select name from TestBind_Table where id = 42")));

        const auto rset = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rset);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rset));
        EXPECT_EQ(String(OTEXT("hello")), String(OCI_ResultsetGetString(rset, 0)));
    }

    /* -------------------------------------------------------------------------
       Shared fixture: prepared PL/SQL block with 3 OUT binds
       ------------------------------------------------------------------------- */

    class TestBindOut : public ::testing::Test
    {
    protected:
        OCI_Environment* env = nullptr;
        OCI_Connection* conn = nullptr;
        OCI_Statement* stmt = nullptr;
        OCI_Int32 iout = 0;
        otext sout[64] = OTEXT("");
        OCI_Date* dout = nullptr;

        void SetUp() override
        {
            env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);
            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);
            dout = OCI_DateCreate(env);
            ASSERT_NE(nullptr, dout);
            ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("BEGIN :i := 123; :s := 'hello'; :d := TO_DATE('2023-06-15', 'YYYY-MM-DD'); END;")));
            ASSERT_NE(nullptr, OCI_StatementBindInt32(stmt, OTEXT(":i"), &iout));
            ASSERT_NE(nullptr, OCI_StatementBindString(stmt, OTEXT(":s"), sout, 50));
            ASSERT_NE(nullptr, OCI_StatementBindDate(stmt, OTEXT(":d"), dout));
        }

        void TearDown() override
        {
            if (dout) OCI_DateFree(dout);
            if (stmt) OCI_StatementFree(stmt);
            if (conn) OCI_ConnectionFree(conn);
        }

        void SetAllDirectionOut()
        {
            ASSERT_TRUE(OCI_BindSetDirection(OCI_StatementGetBindByName(stmt, OTEXT(":i")), OCI_BDM_OUT));
            ASSERT_TRUE(OCI_BindSetDirection(OCI_StatementGetBindByName(stmt, OTEXT(":s")), OCI_BDM_OUT));
            ASSERT_TRUE(OCI_BindSetDirection(OCI_StatementGetBindByName(stmt, OTEXT(":d")), OCI_BDM_OUT));
        }
    };

    TEST_F(TestBindOut, SetDirectionOut_ReportsOutDirectionAndMetadata)
    {
        const auto bnd_i = OCI_StatementGetBindByName(stmt, OTEXT(":i"));
        const auto bnd_s = OCI_StatementGetBindByName(stmt, OTEXT(":s"));
        const auto bnd_d = OCI_StatementGetBindByName(stmt, OTEXT(":d"));
        ASSERT_NE(nullptr, bnd_i);
        ASSERT_NE(nullptr, bnd_s);
        ASSERT_NE(nullptr, bnd_d);

        SetAllDirectionOut();

        EXPECT_EQ((OCI_UInt32) OCI_BDM_OUT, OCI_BindGetDirection(bnd_i));
        EXPECT_EQ((OCI_UInt32) OCI_BDM_OUT, OCI_BindGetDirection(bnd_s));
        EXPECT_EQ((OCI_UInt32) OCI_CDT_NUMERIC, OCI_BindGetType(bnd_i));
        EXPECT_EQ((OCI_UInt32) OCI_CDT_TEXT, OCI_BindGetType(bnd_s));
        EXPECT_EQ((OCI_UInt32) OCI_CDT_DATETIME, OCI_BindGetType(bnd_d));
        EXPECT_EQ(1u, OCI_BindGetDataCount(bnd_i));
        EXPECT_EQ(stmt, OCI_BindGetStatement(bnd_i));
        EXPECT_EQ(static_cast<void*>(&iout), OCI_BindGetData(bnd_i));
    }

    TEST_F(TestBindOut, ExecutePlsql_ReturnsAssignedOutValues)
    {
        SetAllDirectionOut();

        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        EXPECT_EQ(123, iout);
        EXPECT_EQ(String(OTEXT("hello")), String(sout));
        OCI_Int32 year = 0, month = 0, day = 0;
        ASSERT_TRUE(OCI_DateGetDate(dout, &year, &month, &day));
        EXPECT_EQ(2023, year);
        EXPECT_EQ(6, month);
        EXPECT_EQ(15, day);
    }

    TEST(TestBind, NullAndDataSize)
    {
        ExecDML(OTEXT("create table TestBind_Null (id int, name varchar2(50))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        OCI_Int32 id = 1;
        otext name[64] = OTEXT("OCI_Int16 extra");

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into TestBind_Null values (:id, :name)")));
        ASSERT_NE(nullptr, OCI_StatementBindInt32(stmt, OTEXT(":id"), &id));
        ASSERT_NE(nullptr, OCI_StatementBindString(stmt, OTEXT(":name"), name, 50));
        const auto bnd_id = OCI_StatementGetBindByName(stmt, OTEXT(":id"));
        const auto bnd_name = OCI_StatementGetBindByName(stmt, OTEXT(":name"));
        ASSERT_NE(nullptr, bnd_id);
        ASSERT_NE(nullptr, bnd_name);

        ASSERT_TRUE(OCI_BindSetNull(bnd_name));
        ASSERT_TRUE(OCI_BindIsNull(bnd_name));
        ASSERT_FALSE(OCI_BindIsNull(bnd_id));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        id = 2;
        ASSERT_TRUE(OCI_BindSetNotNull(bnd_name));
        ASSERT_TRUE(OCI_BindSetDataSize(bnd_name, 5));
        ASSERT_EQ(5u, OCI_BindGetDataSize(bnd_name));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        OCI_Int32 count = 0;
        ASSERT_TRUE(OCI_ConnectionImmediate(conn, OTEXT("select count(*) from TestBind_Null where id = 1 and name is null"), OCI_ARG_INT32, &count));
        ASSERT_EQ(1, count);

        count = 0;
        ASSERT_TRUE(OCI_ConnectionImmediate(conn, OTEXT("select count(*) from TestBind_Null where id = 2 and name like 'OCI_Int16%'"), OCI_ARG_INT32, &count));
        ASSERT_EQ(1, count);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestBind_Null"));
    }

    /* -------------------------------------------------------------------------
       Parameterized: each OCI_BindX call registers a valid bind handle
       ------------------------------------------------------------------------- */

    struct BindTypeCase
    {
        const char* name;
        void (*run)(OCI_Environment*, OCI_Connection*, OCI_Statement*);
    };

    void PrintTo(const BindTypeCase& c, std::ostream* os) { *os << c.name; }

    static void BindNumber(OCI_Environment* env, OCI_Connection*, OCI_Statement* stmt)
    {
        const auto number = OCI_NumberCreate(env);
        OCI_Int32 num = 42;
        ASSERT_TRUE(OCI_NumberSetValue(number, OCI_NUM_INT32, &num));

        ASSERT_NE(nullptr, OCI_StatementBindNumber(stmt, OTEXT(":n"), number));

        const auto bnd = OCI_StatementGetBindByName(stmt, OTEXT(":n"));
        ASSERT_NE(nullptr, bnd);
        EXPECT_EQ((OCI_UInt32) OCI_NUM_NUMBER, OCI_BindGetSubtype(bnd));
        EXPECT_EQ(static_cast<void*>(number), OCI_BindGetData(bnd));
        EXPECT_EQ((OCI_UInt32) OCI_BAM_EXTERNAL, OCI_BindGetAllocationMode(bnd));

        ASSERT_TRUE(OCI_NumberFree(number));
    }
    static void BindString(OCI_Environment*, OCI_Connection*, OCI_Statement* stmt)
    {
        otext str[64] = OTEXT("typed bind");

        ASSERT_NE(nullptr, OCI_StatementBindString(stmt, OTEXT(":s"), str, 50));

        const auto bnd = OCI_StatementGetBindByName(stmt, OTEXT(":s"));
        ASSERT_NE(nullptr, bnd);
        EXPECT_EQ((OCI_UInt32) OCI_UNKNOWN, OCI_BindGetSubtype(bnd));
    }
    static void BindDate(OCI_Environment* env, OCI_Connection*, OCI_Statement* stmt)
    {
        OCI_Date* date = OCI_DateCreate(env);
        ASSERT_NE(nullptr, date);
        ASSERT_TRUE(OCI_DateSetDateTime(date, 2024, 1, 2, 3, 4, 5));

        ASSERT_NE(nullptr, OCI_StatementBindDate(stmt, OTEXT(":d"), date));

        const auto bnd = OCI_StatementGetBindByName(stmt, OTEXT(":d"));
        ASSERT_NE(nullptr, bnd);
        EXPECT_EQ(static_cast<void*>(date), OCI_BindGetData(bnd));

        ASSERT_TRUE(OCI_DateFree(date));
    }
    static void BindRaw(OCI_Environment*, OCI_Connection*, OCI_Statement* stmt)
    {
        unsigned char raw[20] = { 0x00, 0x01, 0x02, 0x03, 0x04 };

        ASSERT_NE(nullptr, OCI_StatementBindRaw(stmt, OTEXT(":r"), raw, 20));

        const auto bnd = OCI_StatementGetBindByName(stmt, OTEXT(":r"));
        ASSERT_NE(nullptr, bnd);
        ASSERT_TRUE(OCI_BindSetDataSize(bnd, 5));
        EXPECT_EQ(5u, OCI_BindGetDataSize(bnd));
    }
    static void BindLob(OCI_Environment*, OCI_Connection* conn, OCI_Statement* stmt)
    {
        OCI_Lob* lob = OCI_LobCreate(conn, OCI_CLOB);
        ASSERT_NE(nullptr, lob);
        OCI_UInt32 lob_cc = (OCI_UInt32)ostrlen(OTEXT("clob data"));
        OCI_UInt32 lob_bc = 0;
        ASSERT_TRUE(OCI_LobWrite(lob, (void*)OTEXT("clob data"), &lob_cc, &lob_bc));

        ASSERT_NE(nullptr, OCI_StatementBindLob(stmt, OTEXT(":l"), lob));

        const auto bnd = OCI_StatementGetBindByName(stmt, OTEXT(":l"));
        ASSERT_NE(nullptr, bnd);
        EXPECT_EQ(static_cast<void*>(lob), OCI_BindGetData(bnd));

        ASSERT_TRUE(OCI_LobFree(lob));
    }
    static void BindObject(OCI_Environment*, OCI_Connection* conn, OCI_Statement* stmt)
    {
        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestBind_ObjType"), OCI_TIF_TYPE);
        OCI_Object* obj = OCI_ObjectCreate(typinf);
        ASSERT_NE(nullptr, obj);
        ASSERT_TRUE(OCI_ObjectSetInt32(obj, OTEXT("CODE"), 7));
        ASSERT_TRUE(OCI_ObjectSetString(obj, OTEXT("NAME"), OTEXT("obj")));

        ASSERT_NE(nullptr, OCI_StatementBindObject(stmt, OTEXT(":o"), obj));

        const auto bnd = OCI_StatementGetBindByName(stmt, OTEXT(":o"));
        ASSERT_NE(nullptr, bnd);
        EXPECT_EQ(static_cast<void*>(obj), OCI_BindGetData(bnd));

        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
    }

    class TestBindTypes : public ::testing::TestWithParam<BindTypeCase>
    {
    protected:
        OCI_Environment* env = nullptr;
        OCI_Connection* conn = nullptr;
        OCI_Statement* stmt = nullptr;

    public:
        static void SetUpTestCase()
        {
            ExecDML(OTEXT("create type TestBind_ObjType as object(code int, name varchar2(30))"));
            ExecDML(OTEXT("create table TestBind_Types (n number, s varchar2(50), d date, r raw(20), l clob, o TestBind_ObjType)"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestBind_Types"));
            ExecDML(OTEXT("drop type TestBind_ObjType"));
        }

    protected:
        void SetUp() override
        {
            env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);
            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);
            ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into TestBind_Types values (:n, :s, :d, :r, :l, :o)")));
        }

        void TearDown() override
        {
            if (stmt) OCI_StatementFree(stmt);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_P(TestBindTypes, BindXOnPreparedInsert_ReturnsValidHandle)
    {
        GetParam().run(env, conn, stmt);
    }

    INSTANTIATE_TEST_CASE_P(BindKind, TestBindTypes,
        ::testing::Values(
            BindTypeCase{ "Number", BindNumber },
            BindTypeCase{ "String", BindString },
            BindTypeCase{ "Date",   BindDate },
            BindTypeCase{ "Raw",    BindRaw },
            BindTypeCase{ "Lob",    BindLob },
            BindTypeCase{ "Object", BindObject }),
        ::testing::PrintToStringParamName());

    TEST_F(TestBindTypes, ExecuteInsertWithAllTypedBinds_InsertsOneRow)
    {
        const auto number = OCI_NumberCreate(env);
        OCI_Int32 num = 42;
        ASSERT_TRUE(OCI_NumberSetValue(number, OCI_NUM_INT32, &num));
        otext str[64] = OTEXT("typed bind");
        OCI_Date* date = OCI_DateCreate(env);
        ASSERT_TRUE(OCI_DateSetDateTime(date, 2024, 1, 2, 3, 4, 5));
        unsigned char raw[20] = { 0x00, 0x01, 0x02, 0x03, 0x04 };
        OCI_Lob* lob = OCI_LobCreate(conn, OCI_CLOB);
        OCI_UInt32 lob_cc = (OCI_UInt32)ostrlen(OTEXT("clob data"));
        OCI_UInt32 lob_bc = 0;
        ASSERT_TRUE(OCI_LobWrite(lob, (void*)OTEXT("clob data"), &lob_cc, &lob_bc));
        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestBind_ObjType"), OCI_TIF_TYPE);
        OCI_Object* obj = OCI_ObjectCreate(typinf);
        ASSERT_TRUE(OCI_ObjectSetInt32(obj, OTEXT("CODE"), 7));
        ASSERT_TRUE(OCI_ObjectSetString(obj, OTEXT("NAME"), OTEXT("obj")));
        ASSERT_NE(nullptr, OCI_StatementBindNumber(stmt, OTEXT(":n"), number));
        ASSERT_NE(nullptr, OCI_StatementBindString(stmt, OTEXT(":s"), str, 50));
        ASSERT_NE(nullptr, OCI_StatementBindDate(stmt, OTEXT(":d"), date));
        ASSERT_NE(nullptr, OCI_StatementBindRaw(stmt, OTEXT(":r"), raw, 20));
        ASSERT_NE(nullptr, OCI_StatementBindLob(stmt, OTEXT(":l"), lob));
        ASSERT_NE(nullptr, OCI_StatementBindObject(stmt, OTEXT(":o"), obj));

        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        OCI_Int32 count = 0;
        ASSERT_TRUE(OCI_ConnectionImmediate(conn, OTEXT("select count(*) from TestBind_Types"), OCI_ARG_INT32, &count));
        EXPECT_EQ(1, count);

        ASSERT_TRUE(OCI_NumberFree(number));
        ASSERT_TRUE(OCI_DateFree(date));
        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
    }

    TEST(TestBind, ArrayBind)
    {
        ExecDML(OTEXT("create table TestBind_Array (id int)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        OCI_Int32 ids[] = { 10, 20, 30 };

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into TestBind_Array values (:id)")));
        ASSERT_TRUE(OCI_StatementBindArraySetSize(stmt, 3));
        ASSERT_EQ(3u, OCI_StatementBindArrayGetSize(stmt));

        ASSERT_NE(nullptr, OCI_StatementBindInt32Array(stmt, OTEXT(":id"), ids, 0));
        const auto bnd = OCI_StatementGetBindByName(stmt, OTEXT(":id"));
        ASSERT_NE(nullptr, bnd);
        ASSERT_EQ(3u, OCI_BindGetDataCount(bnd));
        ASSERT_EQ((OCI_UInt32) OCI_CDT_NUMERIC, OCI_BindGetType(bnd));
        ASSERT_EQ((OCI_UInt32) OCI_NUM_INT32, OCI_BindGetSubtype(bnd));
        ASSERT_EQ(static_cast<void*>(ids), OCI_BindGetData(bnd));
        ASSERT_EQ((OCI_UInt32) OCI_BAM_EXTERNAL, OCI_BindGetAllocationMode(bnd));

        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        OCI_Int32 count = 0;
        ASSERT_TRUE(OCI_ConnectionImmediate(conn, OTEXT("select count(*) from TestBind_Array where id in (10, 20, 30)"), OCI_ARG_INT32, &count));
        ASSERT_EQ(3, count);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestBind_Array"));
    }

    /* -------------------------------------------------------------------------
       Parameterized: OCI_RegisterX succeeds per scalar out-bind type
       ------------------------------------------------------------------------- */

    struct BindRegCase
    {
        const char* name;
        void (*run)(OCI_Statement*);
    };

    void PrintTo(const BindRegCase& c, std::ostream* os) { *os << c.name; }

    static void BindRegPrep(OCI_Statement* s)
    {
        ASSERT_TRUE(OCI_StatementSetBindAllocation(s, OCI_BAM_INTERNAL));
        ASSERT_TRUE(OCI_StatementPrepare(s, OTEXT("BEGIN :v := :v; END;")));
    }

    static void BindRegInt(OCI_Statement* s)       { BindRegPrep(s); EXPECT_TRUE(OCI_StatementRegisterInt32(s, OTEXT(":v"))); }
    static void BindRegBigInt(OCI_Statement* s)    { BindRegPrep(s); EXPECT_TRUE(OCI_StatementRegisterInt64(s, OTEXT(":v"))); }
    static void BindRegNumber(OCI_Statement* s)    { BindRegPrep(s); EXPECT_TRUE(OCI_StatementRegisterNumber(s, OTEXT(":v"))); }
    static void BindRegString(OCI_Statement* s)    { BindRegPrep(s); EXPECT_TRUE(OCI_StatementRegisterString(s, OTEXT(":v"), 20)); }
    static void BindRegRaw(OCI_Statement* s)       { BindRegPrep(s); EXPECT_TRUE(OCI_StatementRegisterRaw(s, OTEXT(":v"), 16)); }
    static void BindRegDouble(OCI_Statement* s)    { BindRegPrep(s); EXPECT_TRUE(OCI_StatementRegisterDouble(s, OTEXT(":v"))); }
    static void BindRegFloat(OCI_Statement* s)     { BindRegPrep(s); EXPECT_TRUE(OCI_StatementRegisterFloat(s, OTEXT(":v"))); }
    static void BindRegDate(OCI_Statement* s)      { BindRegPrep(s); EXPECT_TRUE(OCI_StatementRegisterDate(s, OTEXT(":v"))); }
    static void BindRegTimestamp(OCI_Statement* s) { BindRegPrep(s); EXPECT_TRUE(OCI_StatementRegisterTimestamp(s, OTEXT(":v"), OCI_TIMESTAMP)); }
    static void BindRegInterval(OCI_Statement* s)  { BindRegPrep(s); EXPECT_TRUE(OCI_StatementRegisterInterval(s, OTEXT(":v"), OCI_INTERVAL_DS)); }

    class TestBindReg : public ::testing::TestWithParam<BindRegCase>
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

    TEST_P(TestBindReg, RegisterByName_ReturnsTrue)
    {
        GetParam().run(stmt);
    }

    INSTANTIATE_TEST_CASE_P(RegBind, TestBindReg,
        ::testing::Values(
            BindRegCase{ "Int",       BindRegInt },
            BindRegCase{ "BigInt",    BindRegBigInt },
            BindRegCase{ "Number",    BindRegNumber },
            BindRegCase{ "String",    BindRegString },
            BindRegCase{ "Raw",       BindRegRaw },
            BindRegCase{ "Double",    BindRegDouble },
            BindRegCase{ "Float",     BindRegFloat },
            BindRegCase{ "Date",      BindRegDate },
            BindRegCase{ "Timestamp", BindRegTimestamp },
            BindRegCase{ "Interval",  BindRegInterval }),
        ::testing::PrintToStringParamName());

    TEST_F(TestBindReg, BindNullDataInInternalMode_BindsAndReportsIndex)
    {
        ASSERT_TRUE(OCI_StatementSetBindAllocation(stmt, OCI_BAM_INTERNAL));
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("BEGIN :d2 := :d2; END;")));

        ASSERT_NE(nullptr, OCI_StatementBindDouble(stmt, OTEXT(":d2"), NULL));

        EXPECT_EQ(0, OCI_StatementGetBindIndex(stmt, OTEXT(":d2")));
    }

    TEST_F(TestBindReg, GetBindIndexOnRegisteredBinds_ReturnsMinusOne)
    {
        ASSERT_TRUE(OCI_StatementSetBindAllocation(stmt, OCI_BAM_INTERNAL));
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("BEGIN :i := :i; :bi := :bi; :n := :n; :s := :s; END;")));
        ASSERT_TRUE(OCI_StatementRegisterInt32(stmt, OTEXT(":i")));
        ASSERT_TRUE(OCI_StatementRegisterInt64(stmt, OTEXT(":bi")));
        ASSERT_TRUE(OCI_StatementRegisterNumber(stmt, OTEXT(":n")));
        ASSERT_TRUE(OCI_StatementRegisterString(stmt, OTEXT(":s"), 20));

        /* register binds are not binds: OCI_StatementGetBindIndex shall not see them */

        EXPECT_EQ(-1, OCI_StatementGetBindIndex(stmt, OTEXT(":i")));
        EXPECT_EQ(-1, OCI_StatementGetBindIndex(stmt, OTEXT(":bi")));
        EXPECT_EQ(-1, OCI_StatementGetBindIndex(stmt, OTEXT(":n")));
        EXPECT_EQ(-1, OCI_StatementGetBindIndex(stmt, OTEXT(":s")));
        EXPECT_EQ(-1, OCI_StatementGetBindIndex(stmt, OTEXT(":missing")));
    }

    /* -------------------------------------------------------------------------
       Parameterized: OCI_RegisterX succeeds per complex out-bind type
       ------------------------------------------------------------------------- */

    struct BindRegCxCase
    {
        const char* name;
        void (*run)(OCI_Statement*, OCI_TypeInfo*);
    };

    void PrintTo(const BindRegCxCase& c, std::ostream* os) { *os << c.name; }

    static void BindRegCxPrep(OCI_Statement* s)
    {
        ASSERT_TRUE(OCI_StatementSetBindAllocation(s, OCI_BAM_INTERNAL));
        ASSERT_TRUE(OCI_StatementPrepare(s, OTEXT("BEGIN :v := :v; END;")));
    }

    static void BindRegCxLob(OCI_Statement* s, OCI_TypeInfo*)    { BindRegCxPrep(s); EXPECT_TRUE(OCI_StatementRegisterLob(s, OTEXT(":v"), OCI_CLOB)); }
    static void BindRegCxFile(OCI_Statement* s, OCI_TypeInfo*)   { BindRegCxPrep(s); EXPECT_TRUE(OCI_StatementRegisterFile(s, OTEXT(":v"), OCI_BFILE)); }
    static void BindRegCxObject(OCI_Statement* s, OCI_TypeInfo* t){ BindRegCxPrep(s); EXPECT_TRUE(OCI_StatementRegisterObject(s, OTEXT(":v"), t)); }
    static void BindRegCxRef(OCI_Statement* s, OCI_TypeInfo* t)  { BindRegCxPrep(s); EXPECT_TRUE(OCI_StatementRegisterReference(s, OTEXT(":v"), t)); }
    static void BindRegCxVector(OCI_Statement* s, OCI_TypeInfo*) { BindRegCxPrep(s); EXPECT_TRUE(OCI_StatementRegisterVector(s, OTEXT(":v"))); }
    static void BindRegCxJson(OCI_Statement* s, OCI_TypeInfo*)   { BindRegCxPrep(s); EXPECT_TRUE(OCI_StatementRegisterJson(s, OTEXT(":v"))); }

    class TestBindRegCx : public ::testing::TestWithParam<BindRegCxCase>
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_Statement* stmt = nullptr;
        OCI_TypeInfo* typinf = nullptr;

    public:
        static void SetUpTestCase()
        {
            ExecDML(OTEXT("create or replace type TestBind_RegCxObj as object(code int)"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop type TestBind_RegCxObj"));
        }

    protected:
        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);
            typinf = OCI_TypeInfoGet(conn, OTEXT("TestBind_RegCxObj"), OCI_TIF_TYPE);
            ASSERT_NE(nullptr, typinf);
        }

        void TearDown() override
        {
            if (stmt) OCI_StatementFree(stmt);
            if (typinf) OCI_TypeInfoFree(typinf);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_P(TestBindRegCx, RegisterByName_ReturnsTrue)
    {
        GetParam().run(stmt, typinf);
    }

    INSTANTIATE_TEST_CASE_P(RegCx, TestBindRegCx,
        ::testing::Values(
            BindRegCxCase{ "Lob",    BindRegCxLob },
            BindRegCxCase{ "File",   BindRegCxFile },
            BindRegCxCase{ "Object", BindRegCxObject },
            BindRegCxCase{ "Ref",    BindRegCxRef },
            BindRegCxCase{ "Vector", BindRegCxVector },
            BindRegCxCase{ "Json",   BindRegCxJson }),
        ::testing::PrintToStringParamName());

    /* -------------------------------------------------------------------------
       Parameterized: each OCI_BindArrayOfX call registers an internal-alloc
       array bind handle
       ------------------------------------------------------------------------- */

    struct BindArrCase
    {
        const char* name;
        void (*run)(OCI_Statement*, OCI_TypeInfo*);
    };

    void PrintTo(const BindArrCase& c, std::ostream* os) { *os << c.name; }

    static void ArrInts(OCI_Statement* s, OCI_TypeInfo*)
    {
        EXPECT_NE(nullptr, OCI_StatementBindInt32Array(s, OTEXT(":n"), NULL, 0));
    }
    static void ArrNumbers(OCI_Statement* s, OCI_TypeInfo*)
    {
        EXPECT_NE(nullptr, OCI_StatementBindNumberArray(s, OTEXT(":nn"), NULL, 0));
    }
    static void ArrBigInts(OCI_Statement* s, OCI_TypeInfo*)
    {
        EXPECT_NE(nullptr, OCI_StatementBindInt64Array(s, OTEXT(":bi"), NULL, 0));
    }
    static void ArrStrings(OCI_Statement* s, OCI_TypeInfo*)
    {
        EXPECT_NE(nullptr, OCI_StatementBindStringArray(s, OTEXT(":s"), NULL, 20, 0));
    }
    static void ArrRaws(OCI_Statement* s, OCI_TypeInfo*)
    {
        EXPECT_NE(nullptr, OCI_StatementBindRawArray(s, OTEXT(":r"), NULL, 16, 0));
    }
    static void ArrDates(OCI_Statement* s, OCI_TypeInfo*)
    {
        EXPECT_NE(nullptr, OCI_StatementBindDateArray(s, OTEXT(":d"), NULL, 0));
    }
    static void ArrTimestamps(OCI_Statement* s, OCI_TypeInfo*)
    {
        EXPECT_NE(nullptr, OCI_StatementBindTimestampArray(s, OTEXT(":ts"), NULL, OCI_TIMESTAMP, 0));
    }
    static void ArrIntervals(OCI_Statement* s, OCI_TypeInfo*)
    {
        EXPECT_NE(nullptr, OCI_StatementBindIntervalArray(s, OTEXT(":iv"), NULL, OCI_INTERVAL_DS, 0));
    }
    static void ArrLobs(OCI_Statement* s, OCI_TypeInfo*)
    {
        EXPECT_NE(nullptr, OCI_StatementBindLobArray(s, OTEXT(":bl"), NULL, OCI_BLOB, 0));
    }
    static void ArrObjects(OCI_Statement* s, OCI_TypeInfo* t)
    {
        EXPECT_NE(nullptr, OCI_StatementBindObjectArray(s, OTEXT(":o"), NULL, t, 0));
    }
    static void ArrJsons(OCI_Statement* s, OCI_TypeInfo*)
    {
        EXPECT_NE(nullptr, OCI_StatementBindJsonArray(s, OTEXT(":j"), NULL, 0));
    }

    class TestBindArr : public ::testing::TestWithParam<BindArrCase>
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_TypeInfo* typinf_obj = nullptr;
        OCI_TypeInfo* typinf_nt = nullptr;
        OCI_Statement* stmt = nullptr;

    public:
        static void SetUpTestCase()
        {
            ExecDML(OTEXT("create or replace type TestBind_ArrObj as object(code int)"));
            ExecDML(OTEXT("create or replace type TestBind_ArrNt as table of number"));
            ExecDML(OTEXT("create table TestBind_Arr (id number, nn number, bi number, s varchar2(30), r raw(16), d date, ts timestamp, iv interval day to second, bl blob, o TestBind_ArrObj, c TestBind_ArrNt, rf ref TestBind_ArrObj, fl bfile, v vector, j json) nested table c store as TestBind_ArrNtt"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestBind_Arr"));
            ExecDML(OTEXT("drop type TestBind_ArrNt"));
            ExecDML(OTEXT("drop type TestBind_ArrObj"));
        }

    protected:
        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            typinf_obj = OCI_TypeInfoGet(conn, OTEXT("TestBind_ArrObj"), OCI_TIF_TYPE);
            typinf_nt = OCI_TypeInfoGet(conn, OTEXT("TestBind_ArrNt"), OCI_TIF_TYPE);
            ASSERT_NE(nullptr, typinf_obj);
            ASSERT_NE(nullptr, typinf_nt);
            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);
            ASSERT_TRUE(OCI_StatementSetBindAllocation(stmt, OCI_BAM_INTERNAL));
            ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT(
                "insert into TestBind_Arr values(:n, :nn, :bi, :s, :r, :d, :ts, :iv, :bl, :o, :c, :rf, :fl, :v, :j)")));
            ASSERT_TRUE(OCI_StatementBindArraySetSize(stmt, 3));
        }

        void TearDown() override
        {
            if (stmt) OCI_StatementFree(stmt);
            if (typinf_obj) OCI_TypeInfoFree(typinf_obj);
            if (typinf_nt) OCI_TypeInfoFree(typinf_nt);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_P(TestBindArr, BindArrayOfX_ReturnsValidHandle)
    {
        GetParam().run(stmt, typinf_obj);
    }

    INSTANTIATE_TEST_CASE_P(ArrBind, TestBindArr,
        ::testing::Values(
            BindArrCase{ "Ints",       ArrInts },
            BindArrCase{ "Numbers",    ArrNumbers },
            BindArrCase{ "BigInts",    ArrBigInts },
            BindArrCase{ "Strings",    ArrStrings },
            BindArrCase{ "Raws",       ArrRaws },
            BindArrCase{ "Dates",      ArrDates },
            BindArrCase{ "Timestamps", ArrTimestamps },
            BindArrCase{ "Intervals",  ArrIntervals },
            BindArrCase{ "Lobs",       ArrLobs },
            BindArrCase{ "Objects",    ArrObjects },
            BindArrCase{ "Jsons",      ArrJsons }),
        ::testing::PrintToStringParamName());

    TEST_F(TestBindArr, PrepareScalarCollInsert_Succeeds)
    {
        /* scalar collection bind: no public API can create it with NULL data */

        const auto stmt2 = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt2);
        ASSERT_TRUE(OCI_StatementSetBindAllocation(stmt2, OCI_BAM_INTERNAL));

        EXPECT_TRUE(OCI_StatementPrepare(stmt2, OTEXT("insert into TestBind_Arr (c) values(:c)")));

        ASSERT_TRUE(OCI_StatementFree(stmt2));
    }

    TEST(TestBind, PlsqlTableBind)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementSetBindAllocation(stmt, OCI_BAM_INTERNAL));

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("BEGIN :t := :t; END;")));
        ASSERT_NE(nullptr, OCI_StatementBindInt32Array(stmt, OTEXT(":t"), NULL, 5));

        const auto bnd = OCI_StatementGetBindByName(stmt, OTEXT(":t"));
        ASSERT_NE(nullptr, bnd);
        ASSERT_EQ(5u, OCI_BindGetDataCount(bnd));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestBind, BindByPosition)
    {
        ExecDML(OTEXT("create table TestBind_Pos (id int, val varchar2(30))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementSetBindMode(stmt, OCI_BIND_BY_POS));

        OCI_Int32 id = 7;
        otext val[32] = OTEXT("pos bind");

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into TestBind_Pos values(:1, :2)")));
        ASSERT_NE(nullptr, OCI_StatementBindInt32(stmt, OTEXT(":1"), &id));
        ASSERT_NE(nullptr, OCI_StatementBindString(stmt, OTEXT(":2"), val, 30));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        /* out of range positional bind name */

        ASSERT_EQ(nullptr, OCI_StatementBindInt32(stmt, OTEXT(":0"), &id));

        OCI_Int32 count = 0;
        ASSERT_TRUE(OCI_ConnectionImmediate(conn, OTEXT("select count(*) from TestBind_Pos where id = 7 and val = 'pos bind'"), OCI_ARG_INT32, &count));
        ASSERT_EQ(1, count);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestBind_Pos"));
    }

    TEST(TestBind, Rebinding)
    {
        ExecDML(OTEXT("create table TestBind_Rebind (id number)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into TestBind_Rebind values(:x)")));

        /* rebinding is off by default: binding the same name twice fails */

        OCI_Int64 bi = 1;
        ASSERT_NE(nullptr, OCI_StatementBindInt64(stmt, OTEXT(":x"), &bi));
        ASSERT_EQ(nullptr, OCI_StatementBindInt64(stmt, OTEXT(":x"), &bi));

        ASSERT_TRUE(OCI_StatementAllowRebinding(stmt, TRUE));

        /* same name + same type: bind is reused, internal buffer reallocated */

        ASSERT_NE(nullptr, OCI_StatementBindInt64(stmt, OTEXT(":x"), &bi));
        ASSERT_EQ(0, OCI_StatementGetBindIndex(stmt, OTEXT(":x")));

        /* same name + another numeric code: reuse path without allocated buffer */

        OCI_Int32 iv = 0;
        ASSERT_NE(nullptr, OCI_StatementBindInt32(stmt, OTEXT(":x"), &iv));

        /* same name + different datatype: an error is raised and the
           previous bind is returned unchanged */

        otext sbuf[16] = OTEXT("");
        OCI_StatementBindString(stmt, OTEXT(":x"), sbuf, 10);
        const auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestBind_Rebind"));
    }

    TEST(TestBind, LongAndNClobBind)
    {
        ExecDML(OTEXT("create table TestBind_Long (l clob, n nclob)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into TestBind_Long values(:l, :n)")));

        const auto lg = OCI_LongCreate(stmt, OCI_CLONG);
        ASSERT_NE(nullptr, lg);
        ASSERT_NE(nullptr, OCI_StatementBindLong(stmt, OTEXT(":l"), lg, 100));

        const auto nclob = OCI_LobCreate(conn, OCI_NCLOB);
        ASSERT_NE(nullptr, nclob);
        ASSERT_NE(nullptr, OCI_StatementBindLob(stmt, OTEXT(":n"), nclob));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_LongFree(lg));
        ASSERT_TRUE(OCI_LobFree(nclob));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestBind_Long"));
    }

    TEST(TestBind, TextBindSizeAndCharset)
    {
        ExecDML(OTEXT("create table TestBind_Sz (id int, s varchar2(30))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        OCI_Int32 iv = 1;
        otext buf[64] = OTEXT("abc");

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into TestBind_Sz values(:i, :s)")));
        ASSERT_NE(nullptr, OCI_StatementBindInt32(stmt, OTEXT(":i"), &iv));
        ASSERT_NE(nullptr, OCI_StatementBindString(stmt, OTEXT(":s"), buf, 20));

        const auto bnd = OCI_StatementGetBindByName(stmt, OTEXT(":s"));
        const auto bnd_i = OCI_StatementGetBindByName(stmt, OTEXT(":i"));
        ASSERT_NE(nullptr, bnd);
        ASSERT_NE(nullptr, bnd_i);

        /* freshly bound text: stored length equals the bind buffer size minus padding */

        const auto fresh_size = OCI_BindGetDataSizeAtPos(bnd, 0);
        ASSERT_EQ(20u, fresh_size);

        /* setting the data size to the full buffer size hits the text padding branch */

        ASSERT_TRUE(OCI_BindSetDataSizeAtPos(bnd, 0, OCI_BindGetDataSize(bnd)));

        ASSERT_TRUE(OCI_BindSetDataSize(bnd, 3));
        ASSERT_EQ(3u, OCI_BindGetDataSize(bnd));

        /* charset form is only supported on text binds */

        ASSERT_TRUE(OCI_BindSetCharsetForm(bnd, OCI_CSF_NATIONAL));
        ASSERT_TRUE(OCI_BindSetCharsetForm(bnd, OCI_CSF_DEFAULT));
        ASSERT_TRUE(OCI_BindSetCharsetForm(bnd_i, OCI_CSF_NATIONAL));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestBind_Sz"));
    }

    /* --- public-API coverage of the dropped OcilibBind* / OcilibStatement*
       internals (InternalAlloc* / BindCreate / BufferSize / DbTextSize):
       OCI_StatementSetBindAllocation(OCI_BAM_INTERNAL) + OCI_Bind* register +
       OCI_StatementExecutePrepared drives the internal-alloc bind creation path
       transitively — no introspection on unallocated bind internals --- */

    TEST(TestBind, InternalAllocBindOps)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        ExecDML(OTEXT("drop table TestBind_InternalAllocBindOps"));
        ExecDML(OTEXT("create table TestBind_InternalAllocBindOps (c int, s varchar2(50))"));

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        /* OcilibStatementSetBindAllocation + OcilibBindAlloc /
           OcilibBindCreate / OcilibBindRegister internals */
        ASSERT_TRUE(OCI_StatementSetBindAllocation(stmt, OCI_BAM_INTERNAL));
        ASSERT_EQ((OCI_UInt32)OCI_BAM_INTERNAL, OCI_StatementGetBindAllocation(stmt));

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("insert into TestBind_InternalAllocBindOps values (:c, :s)")));

        /* internal allocation mode requires NULL data: OCILIB allocates the
           bind buffers, content is populated through OCI_BindGetData() */
        const auto bnd_c = OCI_StatementBindInt32(stmt, OTEXT(":c"), nullptr);
        ASSERT_NE(nullptr, bnd_c);
        const auto bnd_s = OCI_StatementBindString(stmt, OTEXT(":s"), nullptr, 20);
        ASSERT_NE(nullptr, bnd_s);

        const auto cdata = static_cast<OCI_Int32*>(OCI_BindGetData(bnd_c));
        const auto sdata = static_cast<otext*>(OCI_BindGetData(bnd_s));
        ASSERT_NE(nullptr, cdata);
        ASSERT_NE(nullptr, sdata);

        *cdata = 7;
        memcpy(sdata, OTEXT("internal-alloc"), sizeof(otext) * 15);

        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestBind_InternalAllocBindOps"));
    }

    TEST(TestBind, InternalAllocRejectsExternalData)
    {
        Guard guard(context.Lock);
        context.Errs.clear();

        auto env = GetEnvironment(AddError);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        /* passing user data to a bind in internal allocation mode must be
           rejected, not accepted and later freed as library memory */
        ASSERT_TRUE(OCI_StatementSetBindAllocation(stmt, OCI_BAM_INTERNAL));
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("select :c, :s from dual")));

        OCI_Int32 cval = 7;
        otext sval[20] = OTEXT("external");

        ASSERT_EQ(nullptr, OCI_StatementBindInt32(stmt, OTEXT(":c"), &cval));
        ASSERT_EQ(nullptr, OCI_StatementBindString(stmt, OTEXT(":s"), sval, 20));

        ASSERT_EQ(2, context.Errs.size());
        ASSERT_EQ(OCI_ERR_BIND_EXTERNAL_NOT_ALLOWED, context.Errs[0].Code);
        ASSERT_EQ(OCI_ERR_CAT_LIB, context.Errs[0].Category);
        ASSERT_EQ(OCI_ERR_BIND_EXTERNAL_NOT_ALLOWED, context.Errs[1].Code);
        ASSERT_EQ(OCI_ERR_CAT_LIB, context.Errs[1].Category);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }







    TEST(TestBind, Execute_PlsqlRaise_ReturnsFalseWithOciError)
    {
        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);
        int value = 0;
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("begin raise_application_error(-20001, 'injected'); end;")));

        ASSERT_FALSE(OCI_StatementExecutePrepared(stmt));

        const auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);
        ASSERT_NE(0, OCI_ErrorGetCode(err));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestBind, GetBind_OutOfRangeIndex_ReturnsNull)
    {
        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);
        int value = 0;
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("begin :i := :i; end;")));
        ASSERT_NE(nullptr, OCI_StatementBindInt32(stmt, OTEXT(":i"), &value));

        ASSERT_EQ(nullptr, OCI_StatementGetBind(stmt, 99));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    class TestBindNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestBindNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestBindNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "BindArrayGetSize", [] { return 0 == OCI_StatementBindArrayGetSize(nullptr); } },
            { "BindArrayOfBigInts", [] { return nullptr == OCI_StatementBindInt64Array(nullptr, OTEXT("x"), NullArgOut<OCI_Int64>(), 1); } },
            { "BindArrayOfColls", [] { return nullptr == OCI_StatementBindCollectionArray(nullptr, OTEXT("x"), nullptr, nullptr, 1); } },
            { "BindArrayOfDates", [] { return nullptr == OCI_StatementBindDateArray(nullptr, OTEXT("x"), nullptr, 1); } },
            { "BindArrayOfDoubles", [] { return nullptr == OCI_StatementBindDoubleArray(nullptr, OTEXT("x"), NullArgOut<OCI_Double>(), 1); } },
            { "BindArrayOfFiles", [] { return nullptr == OCI_StatementBindFileArray(nullptr, OTEXT("x"), nullptr, 1, 1); } },
            { "BindArrayOfFloats", [] { return nullptr == OCI_StatementBindFloatArray(nullptr, OTEXT("x"), NullArgOut<OCI_Float>(), 1); } },
            { "BindArrayOfIntervals", [] { return nullptr == OCI_StatementBindIntervalArray(nullptr, OTEXT("x"), nullptr, 1, 1); } },
            { "BindArrayOfInts", [] { return nullptr == OCI_StatementBindInt32Array(nullptr, OTEXT("x"), NullArgOut<OCI_Int32>(), 1); } },
            { "BindArrayOfJsons", [] { return nullptr == OCI_StatementBindJsonArray(nullptr, OTEXT("x"), nullptr, 1); } },
            { "BindArrayOfLobs", [] { return nullptr == OCI_StatementBindLobArray(nullptr, OTEXT("x"), nullptr, 1, 1); } },
            { "BindArrayOfNumbers", [] { return nullptr == OCI_StatementBindNumberArray(nullptr, OTEXT("x"), nullptr, 1); } },
            { "BindArrayOfObjects", [] { return nullptr == OCI_StatementBindObjectArray(nullptr, OTEXT("x"), nullptr, nullptr, 1); } },
            { "BindArrayOfRaws", [] { return nullptr == OCI_StatementBindRawArray(nullptr, OTEXT("x"), NullArgBuffer(), 1, 1); } },
            { "BindArrayOfRefs", [] { return nullptr == OCI_StatementBindReferenceArray(nullptr, OTEXT("x"), nullptr, nullptr, 1); } },
            { "BindArrayOfShorts", [] { return nullptr == OCI_StatementBindInt16Array(nullptr, OTEXT("x"), NullArgOut<OCI_Int16>(), 1); } },
            { "BindArrayOfStrings", [] { return nullptr == OCI_StatementBindStringArray(nullptr, OTEXT("x"), NullArgText(), 1, 1); } },
            { "BindArrayOfTimestamps", [] { return nullptr == OCI_StatementBindTimestampArray(nullptr, OTEXT("x"), nullptr, 1, 1); } },
            { "BindArrayOfUnsignedBigInts", [] { return nullptr == OCI_StatementBindUInt64Array(nullptr, OTEXT("x"), NullArgOut<OCI_UInt64>(), 1); } },
            { "BindArrayOfUnsignedInts", [] { return nullptr == OCI_StatementBindUInt32Array(nullptr, OTEXT("x"), NullArgOut<OCI_UInt32>(), 1); } },
            { "BindArrayOfUnsignedShorts", [] { return nullptr == OCI_StatementBindUInt16Array(nullptr, OTEXT("x"), NullArgOut<OCI_UInt16>(), 1); } },
            { "BindArrayOfVectors", [] { return nullptr == OCI_StatementBindVectorArray(nullptr, OTEXT("x"), nullptr, 1); } },
            { "BindArraySetSize", [] { return !OCI_StatementBindArraySetSize(nullptr, 1); } },
            { "BindBigInt", [] { return nullptr == OCI_StatementBindInt64(nullptr, OTEXT("x"), NullArgOut<OCI_Int64>()); } },
            { "BindBoolean", [] { return nullptr == OCI_StatementBindBoolean(nullptr, OTEXT("x"), NullArgOut<OCI_Boolean>()); } },
            { "BindColl", [] { return nullptr == OCI_StatementBindCollection(nullptr, OTEXT("x"), nullptr); } },
            { "BindDate", [] { return nullptr == OCI_StatementBindDate(nullptr, OTEXT("x"), nullptr); } },
            { "BindDouble", [] { return nullptr == OCI_StatementBindDouble(nullptr, OTEXT("x"), NullArgOut<OCI_Double>()); } },
            { "BindFile", [] { return nullptr == OCI_StatementBindFile(nullptr, OTEXT("x"), nullptr); } },
            { "BindFloat", [] { return nullptr == OCI_StatementBindFloat(nullptr, OTEXT("x"), NullArgOut<OCI_Float>()); } },
            { "BindGetAllocationMode", [] { return OCI_UNKNOWN == OCI_BindGetAllocationMode(nullptr); } },
            { "BindGetData", [] { return nullptr == OCI_BindGetData(nullptr); } },
            { "BindGetDataCount", [] { return 0 == OCI_BindGetDataCount(nullptr); } },
            { "BindGetDataSize", [] { return 0 == OCI_BindGetDataSize(nullptr); } },
            { "BindGetDataSizeAtPos", [] { return 0 == OCI_BindGetDataSizeAtPos(nullptr, 1); } },
            { "BindGetDirection", [] { return OCI_UNKNOWN == OCI_BindGetDirection(nullptr); } },
            { "BindGetName", [] { return nullptr == OCI_BindGetName(nullptr); } },
            { "BindGetStatement", [] { return nullptr == OCI_BindGetStatement(nullptr); } },
            { "BindGetSubtype", [] { return OCI_UNKNOWN == OCI_BindGetSubtype(nullptr); } },
            { "BindGetType", [] { return OCI_UNKNOWN == OCI_BindGetType(nullptr); } },
            { "BindInt", [] { return nullptr == OCI_StatementBindInt32(nullptr, OTEXT("x"), NullArgOut<OCI_Int32>()); } },
            { "BindInterval", [] { return nullptr == OCI_StatementBindInterval(nullptr, OTEXT("x"), nullptr); } },
            { "BindIsNull", [] { return OCI_BindIsNull(nullptr); } }, // NULL handle -> null status indeterminate -> TRUE
            { "BindIsNullAtPos", [] { return OCI_BindIsNullAtPos(nullptr, 1); } }, // NULL handle -> null status indeterminate -> TRUE
            { "BindJson", [] { return nullptr == OCI_StatementBindJson(nullptr, OTEXT("x"), nullptr); } },
            { "BindLob", [] { return nullptr == OCI_StatementBindLob(nullptr, OTEXT("x"), nullptr); } },
            { "BindLong", [] { return nullptr == OCI_StatementBindLong(nullptr, OTEXT("x"), nullptr, 1); } },
            { "BindNumber", [] { return nullptr == OCI_StatementBindNumber(nullptr, OTEXT("x"), nullptr); } },
            { "BindObject", [] { return nullptr == OCI_StatementBindObject(nullptr, OTEXT("x"), nullptr); } },
            { "BindRaw", [] { return nullptr == OCI_StatementBindRaw(nullptr, OTEXT("x"), NullArgBuffer(), 1); } },
            { "BindRef", [] { return nullptr == OCI_StatementBindReference(nullptr, OTEXT("x"), nullptr); } },
            { "BindSetCharsetForm", [] { return !OCI_BindSetCharsetForm(nullptr, 1); } },
            { "BindSetDataSize", [] { return !OCI_BindSetDataSize(nullptr, 1); } },
            { "BindSetDataSizeAtPos", [] { return !OCI_BindSetDataSizeAtPos(nullptr, 1, 1); } },
            { "BindSetDirection", [] { return !OCI_BindSetDirection(nullptr, 1); } },
            { "BindSetNotNull", [] { return !OCI_BindSetNotNull(nullptr); } },
            { "BindSetNotNullAtPos", [] { return !OCI_BindSetNotNullAtPos(nullptr, 1); } },
            { "BindSetNull", [] { return !OCI_BindSetNull(nullptr); } },
            { "BindSetNullAtPos", [] { return !OCI_BindSetNullAtPos(nullptr, 1); } },
            { "BindShort", [] { return nullptr == OCI_StatementBindInt16(nullptr, OTEXT("x"), NullArgOut<OCI_Int16>()); } },
            { "BindStatement", [] { return nullptr == OCI_StatementBindCursor(nullptr, OTEXT("x"), nullptr); } },
            { "BindString", [] { return nullptr == OCI_StatementBindString(nullptr, OTEXT("x"), NullArgText(), 1); } },
            { "BindTimestamp", [] { return nullptr == OCI_StatementBindTimestamp(nullptr, OTEXT("x"), nullptr); } },
            { "BindUnsignedBigInt", [] { return nullptr == OCI_StatementBindUInt64(nullptr, OTEXT("x"), NullArgOut<OCI_UInt64>()); } },
            { "BindUnsignedInt", [] { return nullptr == OCI_StatementBindUInt32(nullptr, OTEXT("x"), NullArgOut<OCI_UInt32>()); } },
            { "BindUnsignedShort", [] { return nullptr == OCI_StatementBindUInt16(nullptr, OTEXT("x"), NullArgOut<OCI_UInt16>()); } },
            { "BindVector", [] { return nullptr == OCI_StatementBindVector(nullptr, OTEXT("x"), nullptr); } },
        }),
        &NullArgCaseName);

}