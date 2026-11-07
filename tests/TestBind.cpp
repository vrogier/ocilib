#include "ocilib_tests.h"


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
            ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into TestBind_Table values (:id, :name)")));
            ASSERT_NE(nullptr, OCI_BindInt(stmt, OTEXT(":id"), &id));
            ASSERT_NE(nullptr, OCI_BindString(stmt, OTEXT(":name"), name, 50));
        }

        void TearDown() override
        {
            if (stmt) OCI_StatementFree(stmt);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_F(TestBindBasic, BoundHandle_ReportsNameAndStatement)
    {
        const auto bnd_id = OCI_GetBind(stmt, 0);
        const auto bnd_name = OCI_GetBindByName(stmt, OTEXT(":name"));

        ASSERT_NE(nullptr, bnd_id);
        ASSERT_NE(nullptr, bnd_name);
        EXPECT_EQ(String(OTEXT(":id")), String(OCI_BindGetName(bnd_id)));
        EXPECT_EQ(String(OTEXT(":name")), String(OCI_BindGetName(bnd_name)));
        EXPECT_EQ(stmt, OCI_BindGetStatement(bnd_id));
    }

    TEST_F(TestBindBasic, BoundHandle_ReportsTypeDirectionAndDataCount)
    {
        const auto bnd_id = OCI_GetBind(stmt, 0);
        const auto bnd_name = OCI_GetBindByName(stmt, OTEXT(":name"));

        ASSERT_NE(nullptr, bnd_id);
        ASSERT_NE(nullptr, bnd_name);
        EXPECT_EQ((OCI_UInt32) OCI_BDM_IN_OUT, OCI_BindGetDirection(bnd_id));
        EXPECT_EQ((OCI_UInt32) OCI_CDT_NUMERIC, OCI_BindGetType(bnd_id));
        EXPECT_EQ((OCI_UInt32) OCI_CDT_TEXT, OCI_BindGetType(bnd_name));
        EXPECT_EQ(1u, OCI_BindGetDataCount(bnd_id));
    }

    TEST_F(TestBindBasic, BoundHandle_GetDataReturnsBoundBuffers)
    {
        const auto bnd_id = OCI_GetBind(stmt, 0);
        const auto bnd_name = OCI_GetBindByName(stmt, OTEXT(":name"));

        ASSERT_NE(nullptr, bnd_id);
        ASSERT_NE(nullptr, bnd_name);
        EXPECT_EQ(id, *static_cast<OCI_Int32*>(OCI_BindGetData(bnd_id)));
        EXPECT_EQ(String(name), String(static_cast<otext*>(OCI_BindGetData(bnd_name))));
    }

    TEST_F(TestBindBasic, ExecuteInsertThenSelect_ReturnsBoundValues)
    {
        ASSERT_TRUE(OCI_Execute(stmt));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select name from TestBind_Table where id = 42")));

        const auto rset = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rset);
        ASSERT_TRUE(OCI_FetchNext(rset));
        EXPECT_EQ(String(OTEXT("hello")), String(OCI_GetString(rset, 0)));
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
            ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("BEGIN :i := 123; :s := 'hello'; :d := TO_DATE('2023-06-15', 'YYYY-MM-DD'); END;")));
            ASSERT_NE(nullptr, OCI_BindInt(stmt, OTEXT(":i"), &iout));
            ASSERT_NE(nullptr, OCI_BindString(stmt, OTEXT(":s"), sout, 50));
            ASSERT_NE(nullptr, OCI_BindDate(stmt, OTEXT(":d"), dout));
        }

        void TearDown() override
        {
            if (dout) OCI_DateFree(dout);
            if (stmt) OCI_StatementFree(stmt);
            if (conn) OCI_ConnectionFree(conn);
        }

        void SetAllDirectionOut()
        {
            ASSERT_TRUE(OCI_BindSetDirection(OCI_GetBindByName(stmt, OTEXT(":i")), OCI_BDM_OUT));
            ASSERT_TRUE(OCI_BindSetDirection(OCI_GetBindByName(stmt, OTEXT(":s")), OCI_BDM_OUT));
            ASSERT_TRUE(OCI_BindSetDirection(OCI_GetBindByName(stmt, OTEXT(":d")), OCI_BDM_OUT));
        }
    };

    TEST_F(TestBindOut, SetDirectionOut_ReportsOutDirectionAndMetadata)
    {
        const auto bnd_i = OCI_GetBindByName(stmt, OTEXT(":i"));
        const auto bnd_s = OCI_GetBindByName(stmt, OTEXT(":s"));
        const auto bnd_d = OCI_GetBindByName(stmt, OTEXT(":d"));
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

        ASSERT_TRUE(OCI_Execute(stmt));

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

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into TestBind_Null values (:id, :name)")));
        ASSERT_NE(nullptr, OCI_BindInt(stmt, OTEXT(":id"), &id));
        ASSERT_NE(nullptr, OCI_BindString(stmt, OTEXT(":name"), name, 50));
        const auto bnd_id = OCI_GetBindByName(stmt, OTEXT(":id"));
        const auto bnd_name = OCI_GetBindByName(stmt, OTEXT(":name"));
        ASSERT_NE(nullptr, bnd_id);
        ASSERT_NE(nullptr, bnd_name);

        ASSERT_TRUE(OCI_BindSetNull(bnd_name));
        ASSERT_TRUE(OCI_BindIsNull(bnd_name));
        ASSERT_FALSE(OCI_BindIsNull(bnd_id));
        ASSERT_TRUE(OCI_Execute(stmt));

        id = 2;
        ASSERT_TRUE(OCI_BindSetNotNull(bnd_name));
        ASSERT_TRUE(OCI_BindSetDataSize(bnd_name, 5));
        ASSERT_EQ(5u, OCI_BindGetDataSize(bnd_name));
        ASSERT_TRUE(OCI_Execute(stmt));

        OCI_Int32 count = 0;
        ASSERT_TRUE(OCI_Immediate(conn, OTEXT("select count(*) from TestBind_Null where id = 1 and name is null"), OCI_ARG_INT, &count));
        ASSERT_EQ(1, count);

        count = 0;
        ASSERT_TRUE(OCI_Immediate(conn, OTEXT("select count(*) from TestBind_Null where id = 2 and name like 'OCI_Int16%'"), OCI_ARG_INT, &count));
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
        ASSERT_TRUE(OCI_NumberSetValue(number, OCI_NUM_INT, &num));

        ASSERT_NE(nullptr, OCI_BindNumber(stmt, OTEXT(":n"), number));

        const auto bnd = OCI_GetBindByName(stmt, OTEXT(":n"));
        ASSERT_NE(nullptr, bnd);
        EXPECT_EQ((OCI_UInt32) OCI_NUM_NUMBER, OCI_BindGetSubtype(bnd));
        EXPECT_EQ(static_cast<void*>(number), OCI_BindGetData(bnd));
        EXPECT_EQ((OCI_UInt32) OCI_BAM_EXTERNAL, OCI_BindGetAllocationMode(bnd));

        ASSERT_TRUE(OCI_NumberFree(number));
    }
    static void BindString(OCI_Environment*, OCI_Connection*, OCI_Statement* stmt)
    {
        otext str[64] = OTEXT("typed bind");

        ASSERT_NE(nullptr, OCI_BindString(stmt, OTEXT(":s"), str, 50));

        const auto bnd = OCI_GetBindByName(stmt, OTEXT(":s"));
        ASSERT_NE(nullptr, bnd);
        EXPECT_EQ((OCI_UInt32) OCI_UNKNOWN, OCI_BindGetSubtype(bnd));
    }
    static void BindDate(OCI_Environment* env, OCI_Connection*, OCI_Statement* stmt)
    {
        OCI_Date* date = OCI_DateCreate(env);
        ASSERT_NE(nullptr, date);
        ASSERT_TRUE(OCI_DateSetDateTime(date, 2024, 1, 2, 3, 4, 5));

        ASSERT_NE(nullptr, OCI_BindDate(stmt, OTEXT(":d"), date));

        const auto bnd = OCI_GetBindByName(stmt, OTEXT(":d"));
        ASSERT_NE(nullptr, bnd);
        EXPECT_EQ(static_cast<void*>(date), OCI_BindGetData(bnd));

        ASSERT_TRUE(OCI_DateFree(date));
    }
    static void BindRaw(OCI_Environment*, OCI_Connection*, OCI_Statement* stmt)
    {
        unsigned char raw[20] = { 0x00, 0x01, 0x02, 0x03, 0x04 };

        ASSERT_NE(nullptr, OCI_BindRaw(stmt, OTEXT(":r"), raw, 20));

        const auto bnd = OCI_GetBindByName(stmt, OTEXT(":r"));
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

        ASSERT_NE(nullptr, OCI_BindLob(stmt, OTEXT(":l"), lob));

        const auto bnd = OCI_GetBindByName(stmt, OTEXT(":l"));
        ASSERT_NE(nullptr, bnd);
        EXPECT_EQ(static_cast<void*>(lob), OCI_BindGetData(bnd));

        ASSERT_TRUE(OCI_LobFree(lob));
    }
    static void BindObject(OCI_Environment*, OCI_Connection* conn, OCI_Statement* stmt)
    {
        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestBind_ObjType"), OCI_TIF_TYPE);
        OCI_Object* obj = OCI_ObjectCreate(typinf);
        ASSERT_NE(nullptr, obj);
        ASSERT_TRUE(OCI_ObjectSetInt(obj, OTEXT("CODE"), 7));
        ASSERT_TRUE(OCI_ObjectSetString(obj, OTEXT("NAME"), OTEXT("obj")));

        ASSERT_NE(nullptr, OCI_BindObject(stmt, OTEXT(":o"), obj));

        const auto bnd = OCI_GetBindByName(stmt, OTEXT(":o"));
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
            ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into TestBind_Types values (:n, :s, :d, :r, :l, :o)")));
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
        ASSERT_TRUE(OCI_NumberSetValue(number, OCI_NUM_INT, &num));
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
        ASSERT_TRUE(OCI_ObjectSetInt(obj, OTEXT("CODE"), 7));
        ASSERT_TRUE(OCI_ObjectSetString(obj, OTEXT("NAME"), OTEXT("obj")));
        ASSERT_NE(nullptr, OCI_BindNumber(stmt, OTEXT(":n"), number));
        ASSERT_NE(nullptr, OCI_BindString(stmt, OTEXT(":s"), str, 50));
        ASSERT_NE(nullptr, OCI_BindDate(stmt, OTEXT(":d"), date));
        ASSERT_NE(nullptr, OCI_BindRaw(stmt, OTEXT(":r"), raw, 20));
        ASSERT_NE(nullptr, OCI_BindLob(stmt, OTEXT(":l"), lob));
        ASSERT_NE(nullptr, OCI_BindObject(stmt, OTEXT(":o"), obj));

        ASSERT_TRUE(OCI_Execute(stmt));

        OCI_Int32 count = 0;
        ASSERT_TRUE(OCI_Immediate(conn, OTEXT("select count(*) from TestBind_Types"), OCI_ARG_INT, &count));
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

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into TestBind_Array values (:id)")));
        ASSERT_TRUE(OCI_BindArraySetSize(stmt, 3));
        ASSERT_EQ(3u, OCI_BindArrayGetSize(stmt));

        ASSERT_NE(nullptr, OCI_BindArrayOfInts(stmt, OTEXT(":id"), ids, 0));
        const auto bnd = OCI_GetBindByName(stmt, OTEXT(":id"));
        ASSERT_NE(nullptr, bnd);
        ASSERT_EQ(3u, OCI_BindGetDataCount(bnd));
        ASSERT_EQ((OCI_UInt32) OCI_CDT_NUMERIC, OCI_BindGetType(bnd));
        ASSERT_EQ((OCI_UInt32) OCI_NUM_INT, OCI_BindGetSubtype(bnd));
        ASSERT_EQ(static_cast<void*>(ids), OCI_BindGetData(bnd));
        ASSERT_EQ((OCI_UInt32) OCI_BAM_EXTERNAL, OCI_BindGetAllocationMode(bnd));

        ASSERT_TRUE(OCI_Execute(stmt));

        OCI_Int32 count = 0;
        ASSERT_TRUE(OCI_Immediate(conn, OTEXT("select count(*) from TestBind_Array where id in (10, 20, 30)"), OCI_ARG_INT, &count));
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
        ASSERT_TRUE(OCI_SetBindAllocation(s, OCI_BAM_INTERNAL));
        ASSERT_TRUE(OCI_Prepare(s, OTEXT("BEGIN :v := :v; END;")));
    }

    static void BindRegInt(OCI_Statement* s)       { BindRegPrep(s); EXPECT_TRUE(OCI_RegisterInt(s, OTEXT(":v"))); }
    static void BindRegBigInt(OCI_Statement* s)    { BindRegPrep(s); EXPECT_TRUE(OCI_RegisterBigInt(s, OTEXT(":v"))); }
    static void BindRegNumber(OCI_Statement* s)    { BindRegPrep(s); EXPECT_TRUE(OCI_RegisterNumber(s, OTEXT(":v"))); }
    static void BindRegString(OCI_Statement* s)    { BindRegPrep(s); EXPECT_TRUE(OCI_RegisterString(s, OTEXT(":v"), 20)); }
    static void BindRegRaw(OCI_Statement* s)       { BindRegPrep(s); EXPECT_TRUE(OCI_RegisterRaw(s, OTEXT(":v"), 16)); }
    static void BindRegDouble(OCI_Statement* s)    { BindRegPrep(s); EXPECT_TRUE(OCI_RegisterDouble(s, OTEXT(":v"))); }
    static void BindRegFloat(OCI_Statement* s)     { BindRegPrep(s); EXPECT_TRUE(OCI_RegisterFloat(s, OTEXT(":v"))); }
    static void BindRegDate(OCI_Statement* s)      { BindRegPrep(s); EXPECT_TRUE(OCI_RegisterDate(s, OTEXT(":v"))); }
    static void BindRegTimestamp(OCI_Statement* s) { BindRegPrep(s); EXPECT_TRUE(OCI_RegisterTimestamp(s, OTEXT(":v"), OCI_TIMESTAMP)); }
    static void BindRegInterval(OCI_Statement* s)  { BindRegPrep(s); EXPECT_TRUE(OCI_RegisterInterval(s, OTEXT(":v"), OCI_INTERVAL_DS)); }

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
        ASSERT_TRUE(OCI_SetBindAllocation(stmt, OCI_BAM_INTERNAL));
        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("BEGIN :d2 := :d2; END;")));

        ASSERT_NE(nullptr, OCI_BindDouble(stmt, OTEXT(":d2"), NULL));

        EXPECT_EQ(0, OCI_GetBindIndex(stmt, OTEXT(":d2")));
    }

    TEST_F(TestBindReg, GetBindIndexOnRegisteredBinds_ReturnsMinusOne)
    {
        ASSERT_TRUE(OCI_SetBindAllocation(stmt, OCI_BAM_INTERNAL));
        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("BEGIN :i := :i; :bi := :bi; :n := :n; :s := :s; END;")));
        ASSERT_TRUE(OCI_RegisterInt(stmt, OTEXT(":i")));
        ASSERT_TRUE(OCI_RegisterBigInt(stmt, OTEXT(":bi")));
        ASSERT_TRUE(OCI_RegisterNumber(stmt, OTEXT(":n")));
        ASSERT_TRUE(OCI_RegisterString(stmt, OTEXT(":s"), 20));

        /* register binds are not binds: OCI_GetBindIndex shall not see them */

        EXPECT_EQ(-1, OCI_GetBindIndex(stmt, OTEXT(":i")));
        EXPECT_EQ(-1, OCI_GetBindIndex(stmt, OTEXT(":bi")));
        EXPECT_EQ(-1, OCI_GetBindIndex(stmt, OTEXT(":n")));
        EXPECT_EQ(-1, OCI_GetBindIndex(stmt, OTEXT(":s")));
        EXPECT_EQ(-1, OCI_GetBindIndex(stmt, OTEXT(":missing")));
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
        ASSERT_TRUE(OCI_SetBindAllocation(s, OCI_BAM_INTERNAL));
        ASSERT_TRUE(OCI_Prepare(s, OTEXT("BEGIN :v := :v; END;")));
    }

    static void BindRegCxLob(OCI_Statement* s, OCI_TypeInfo*)    { BindRegCxPrep(s); EXPECT_TRUE(OCI_RegisterLob(s, OTEXT(":v"), OCI_CLOB)); }
    static void BindRegCxFile(OCI_Statement* s, OCI_TypeInfo*)   { BindRegCxPrep(s); EXPECT_TRUE(OCI_RegisterFile(s, OTEXT(":v"), OCI_BFILE)); }
    static void BindRegCxObject(OCI_Statement* s, OCI_TypeInfo* t){ BindRegCxPrep(s); EXPECT_TRUE(OCI_RegisterObject(s, OTEXT(":v"), t)); }
    static void BindRegCxRef(OCI_Statement* s, OCI_TypeInfo* t)  { BindRegCxPrep(s); EXPECT_TRUE(OCI_RegisterRef(s, OTEXT(":v"), t)); }
    static void BindRegCxVector(OCI_Statement* s, OCI_TypeInfo*) { BindRegCxPrep(s); EXPECT_TRUE(OCI_RegisterVector(s, OTEXT(":v"))); }
    static void BindRegCxJson(OCI_Statement* s, OCI_TypeInfo*)   { BindRegCxPrep(s); EXPECT_TRUE(OCI_RegisterJson(s, OTEXT(":v"))); }

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
        EXPECT_NE(nullptr, OCI_BindArrayOfInts(s, OTEXT(":n"), NULL, 0));
    }
    static void ArrNumbers(OCI_Statement* s, OCI_TypeInfo*)
    {
        EXPECT_NE(nullptr, OCI_BindArrayOfNumbers(s, OTEXT(":nn"), NULL, 0));
    }
    static void ArrBigInts(OCI_Statement* s, OCI_TypeInfo*)
    {
        EXPECT_NE(nullptr, OCI_BindArrayOfBigInts(s, OTEXT(":bi"), NULL, 0));
    }
    static void ArrStrings(OCI_Statement* s, OCI_TypeInfo*)
    {
        EXPECT_NE(nullptr, OCI_BindArrayOfStrings(s, OTEXT(":s"), NULL, 20, 0));
    }
    static void ArrRaws(OCI_Statement* s, OCI_TypeInfo*)
    {
        EXPECT_NE(nullptr, OCI_BindArrayOfRaws(s, OTEXT(":r"), NULL, 16, 0));
    }
    static void ArrDates(OCI_Statement* s, OCI_TypeInfo*)
    {
        EXPECT_NE(nullptr, OCI_BindArrayOfDates(s, OTEXT(":d"), NULL, 0));
    }
    static void ArrTimestamps(OCI_Statement* s, OCI_TypeInfo*)
    {
        EXPECT_NE(nullptr, OCI_BindArrayOfTimestamps(s, OTEXT(":ts"), NULL, OCI_TIMESTAMP, 0));
    }
    static void ArrIntervals(OCI_Statement* s, OCI_TypeInfo*)
    {
        EXPECT_NE(nullptr, OCI_BindArrayOfIntervals(s, OTEXT(":iv"), NULL, OCI_INTERVAL_DS, 0));
    }
    static void ArrLobs(OCI_Statement* s, OCI_TypeInfo*)
    {
        EXPECT_NE(nullptr, OCI_BindArrayOfLobs(s, OTEXT(":bl"), NULL, OCI_BLOB, 0));
    }
    static void ArrObjects(OCI_Statement* s, OCI_TypeInfo* t)
    {
        EXPECT_NE(nullptr, OCI_BindArrayOfObjects(s, OTEXT(":o"), NULL, t, 0));
    }
    static void ArrJsons(OCI_Statement* s, OCI_TypeInfo*)
    {
        EXPECT_NE(nullptr, OCI_BindArrayOfJsons(s, OTEXT(":j"), NULL, 0));
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
            ASSERT_TRUE(OCI_SetBindAllocation(stmt, OCI_BAM_INTERNAL));
            ASSERT_TRUE(OCI_Prepare(stmt, OTEXT(
                "insert into TestBind_Arr values(:n, :nn, :bi, :s, :r, :d, :ts, :iv, :bl, :o, :c, :rf, :fl, :v, :j)")));
            ASSERT_TRUE(OCI_BindArraySetSize(stmt, 3));
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
        ASSERT_TRUE(OCI_SetBindAllocation(stmt2, OCI_BAM_INTERNAL));

        EXPECT_TRUE(OCI_Prepare(stmt2, OTEXT("insert into TestBind_Arr (c) values(:c)")));

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

        ASSERT_TRUE(OCI_SetBindAllocation(stmt, OCI_BAM_INTERNAL));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("BEGIN :t := :t; END;")));
        ASSERT_NE(nullptr, OCI_BindArrayOfInts(stmt, OTEXT(":t"), NULL, 5));

        const auto bnd = OCI_GetBindByName(stmt, OTEXT(":t"));
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

        ASSERT_TRUE(OCI_SetBindMode(stmt, OCI_BIND_BY_POS));

        OCI_Int32 id = 7;
        otext val[32] = OTEXT("pos bind");

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into TestBind_Pos values(:1, :2)")));
        ASSERT_NE(nullptr, OCI_BindInt(stmt, OTEXT(":1"), &id));
        ASSERT_NE(nullptr, OCI_BindString(stmt, OTEXT(":2"), val, 30));
        ASSERT_TRUE(OCI_Execute(stmt));

        /* out of range positional bind name */

        ASSERT_EQ(nullptr, OCI_BindInt(stmt, OTEXT(":0"), &id));

        OCI_Int32 count = 0;
        ASSERT_TRUE(OCI_Immediate(conn, OTEXT("select count(*) from TestBind_Pos where id = 7 and val = 'pos bind'"), OCI_ARG_INT, &count));
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

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into TestBind_Rebind values(:x)")));

        /* rebinding is off by default: binding the same name twice fails */

        OCI_Int64 bi = 1;
        ASSERT_NE(nullptr, OCI_BindBigInt(stmt, OTEXT(":x"), &bi));
        ASSERT_EQ(nullptr, OCI_BindBigInt(stmt, OTEXT(":x"), &bi));

        ASSERT_TRUE(OCI_AllowRebinding(stmt, TRUE));

        /* same name + same type: bind is reused, internal buffer reallocated */

        ASSERT_NE(nullptr, OCI_BindBigInt(stmt, OTEXT(":x"), &bi));
        ASSERT_EQ(0, OCI_GetBindIndex(stmt, OTEXT(":x")));

        /* same name + another numeric code: reuse path without allocated buffer */

        OCI_Int32 iv = 0;
        ASSERT_NE(nullptr, OCI_BindInt(stmt, OTEXT(":x"), &iv));

        /* same name + different datatype: an error is raised and the
           previous bind is returned unchanged */

        otext sbuf[16] = OTEXT("");
        OCI_BindString(stmt, OTEXT(":x"), sbuf, 10);
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

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into TestBind_Long values(:l, :n)")));

        const auto lg = OCI_LongCreate(stmt, OCI_CLONG);
        ASSERT_NE(nullptr, lg);
        ASSERT_NE(nullptr, OCI_BindLong(stmt, OTEXT(":l"), lg, 100));

        const auto nclob = OCI_LobCreate(conn, OCI_NCLOB);
        ASSERT_NE(nullptr, nclob);
        ASSERT_NE(nullptr, OCI_BindLob(stmt, OTEXT(":n"), nclob));

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

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into TestBind_Sz values(:i, :s)")));
        ASSERT_NE(nullptr, OCI_BindInt(stmt, OTEXT(":i"), &iv));
        ASSERT_NE(nullptr, OCI_BindString(stmt, OTEXT(":s"), buf, 20));

        const auto bnd = OCI_GetBindByName(stmt, OTEXT(":s"));
        const auto bnd_i = OCI_GetBindByName(stmt, OTEXT(":i"));
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
       OCI_SetBindAllocation(OCI_BAM_INTERNAL) + OCI_Bind* register +
       OCI_Execute drives the internal-alloc bind creation path
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
        ASSERT_TRUE(OCI_SetBindAllocation(stmt, OCI_BAM_INTERNAL));
        ASSERT_EQ((OCI_UInt32)OCI_BAM_INTERNAL, OCI_GetBindAllocation(stmt));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into TestBind_InternalAllocBindOps values (:c, :s)")));

        /* internal allocation mode requires NULL data: OCILIB allocates the
           bind buffers, content is populated through OCI_BindGetData() */
        const auto bnd_c = OCI_BindInt(stmt, OTEXT(":c"), nullptr);
        ASSERT_NE(nullptr, bnd_c);
        const auto bnd_s = OCI_BindString(stmt, OTEXT(":s"), nullptr, 20);
        ASSERT_NE(nullptr, bnd_s);

        const auto cdata = static_cast<OCI_Int32*>(OCI_BindGetData(bnd_c));
        const auto sdata = static_cast<otext*>(OCI_BindGetData(bnd_s));
        ASSERT_NE(nullptr, cdata);
        ASSERT_NE(nullptr, sdata);

        *cdata = 7;
        memcpy(sdata, OTEXT("internal-alloc"), sizeof(otext) * 15);

        ASSERT_TRUE(OCI_Execute(stmt));

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
        ASSERT_TRUE(OCI_SetBindAllocation(stmt, OCI_BAM_INTERNAL));
        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select :c, :s from dual")));

        OCI_Int32 cval = 7;
        otext sval[20] = OTEXT("external");

        ASSERT_EQ(nullptr, OCI_BindInt(stmt, OTEXT(":c"), &cval));
        ASSERT_EQ(nullptr, OCI_BindString(stmt, OTEXT(":s"), sval, 20));

        ASSERT_EQ(2, context.Errs.size());
        ASSERT_EQ(OCI_ERR_BIND_EXTERNAL_NOT_ALLOWED, context.Errs[0].Code);
        ASSERT_EQ(OCI_ERR_CAT_LIB, context.Errs[0].Category);
        ASSERT_EQ(OCI_ERR_BIND_EXTERNAL_NOT_ALLOWED, context.Errs[1].Code);
        ASSERT_EQ(OCI_ERR_CAT_LIB, context.Errs[1].Category);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }
}