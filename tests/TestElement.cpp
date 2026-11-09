#include "ocilib_tests.h"
#include "NullArgs.h"

namespace TestCApi
{
    TEST(TestElement, GetAndSet)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_StrVarray AS VARRAY(10) OF VARCHAR2(50)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_StrVarray('alpha', 'beta') FROM DUAL")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 0);
        ASSERT_NE(nullptr, coll);

        const auto elem1 = OCI_CollGetElem(coll, 0);
        ASSERT_NE(nullptr, elem1);
        const otext* str = OCI_ElemGetString(elem1);
        ASSERT_NE(nullptr, str);
        ASSERT_EQ(String(OTEXT("alpha")), String(str));
        ASSERT_FALSE(OCI_ElemIsNull(elem1));

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestElement_StrVarray"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto local = OCI_ElemCreate(typinf);
        ASSERT_NE(nullptr, local);

        ASSERT_TRUE(OCI_ElemSetString(local, OTEXT("local value")));
        ASSERT_EQ(String(OTEXT("local value")), String(OCI_ElemGetString(local)));

        ASSERT_TRUE(OCI_ElemSetNull(local));
        ASSERT_TRUE(OCI_ElemIsNull(local));

        ASSERT_TRUE(OCI_ElemFree(local));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TYPE TestElement_StrVarray"));
    }

    /* -------------------------------------------------------------------------
       Parameterized: element get/set round-trip per collection element type
       ------------------------------------------------------------------------- */

    struct ElemCase
    {
        const char* name;
        void (*run)(OCI_Environment*, OCI_Connection*, OCI_Statement*);
    };

    void PrintTo(const ElemCase& c, std::ostream* os) { *os << c.name; }

    static void ElemNumVarray(OCI_Environment* env, OCI_Connection*, OCI_Statement* stmt)
    {
        ExecDML(OTEXT("create or replace type TestElement_AllNumVarray as varray(3) of number"));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_AllNumVarray(3.14, 2.71) FROM DUAL")));
        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 0);
        ASSERT_NE(nullptr, coll);
        ASSERT_EQ((OCI_UInt32) OCI_COLL_VARRAY, OCI_CollGetType(coll));

        const auto elem = OCI_CollGetElem(coll, 0);
        ASSERT_NE(nullptr, elem);
        ASSERT_FALSE(OCI_ElemIsNull(elem));

        const auto num = OCI_ElemGetNumber(elem);
        ASSERT_NE(nullptr, num);
        OCI_Double d = 0.0;
        ASSERT_TRUE(OCI_NumberGetValue(num, OCI_NUM_DOUBLE, &d));
        EXPECT_DOUBLE_EQ(3.14, d);
        EXPECT_EQ(3, OCI_ElemGetInt(elem));

        const auto local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
        ASSERT_NE(nullptr, local);
        ASSERT_TRUE(OCI_ElemSetInt(local, 5));
        EXPECT_EQ(5, OCI_ElemGetInt(local));
        ASSERT_TRUE(OCI_ElemSetDouble(local, 6.5));
        EXPECT_DOUBLE_EQ(6.5, OCI_ElemGetDouble(local));

        const auto num2 = OCI_NumberCreate(env);
        OCI_Int32 n = 7;
        ASSERT_TRUE(OCI_NumberSetValue(num2, OCI_NUM_INT, &n));
        ASSERT_TRUE(OCI_ElemSetNumber(local, num2));
        EXPECT_EQ(7, OCI_ElemGetInt(local));
        ASSERT_TRUE(OCI_NumberFree(num2));

        ASSERT_TRUE(OCI_ElemSetNull(local));
        EXPECT_TRUE(OCI_ElemIsNull(local));
        ASSERT_TRUE(OCI_ElemFree(local));

        ExecDML(OTEXT("drop type TestElement_AllNumVarray"));
    }

    static void ElemStrVarray(OCI_Environment*, OCI_Connection*, OCI_Statement* stmt)
    {
        ExecDML(OTEXT("create or replace type TestElement_AllStrVarray as varray(3) of varchar2(50)"));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_AllStrVarray('alpha', 'beta') FROM DUAL")));
        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 0);
        const auto elem = OCI_CollGetElem(coll, 0);
        EXPECT_EQ(String(OTEXT("alpha")), String(OCI_ElemGetString(elem)));

        const auto local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
        ASSERT_TRUE(OCI_ElemSetString(local, OTEXT("local value")));
        EXPECT_EQ(String(OTEXT("local value")), String(OCI_ElemGetString(local)));
        ASSERT_TRUE(OCI_ElemSetString(local, nullptr));
        EXPECT_TRUE(OCI_ElemIsNull(local));
        ASSERT_TRUE(OCI_ElemFree(local));

        ExecDML(OTEXT("drop type TestElement_AllStrVarray"));
    }

    static void ElemDateVarray(OCI_Environment* env, OCI_Connection*, OCI_Statement* stmt)
    {
        ExecDML(OTEXT("create or replace type TestElement_AllDateVarray as varray(2) of date"));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_AllDateVarray(DATE '2023-06-15') FROM DUAL")));
        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 0);
        const auto elem = OCI_CollGetElem(coll, 0);
        const auto date = OCI_ElemGetDate(elem);
        ASSERT_NE(nullptr, date);
        OCI_Int32 y = 0, m = 0, d = 0;
        ASSERT_TRUE(OCI_DateGetDate(date, &y, &m, &d));
        EXPECT_EQ(2023, y);
        EXPECT_EQ(6, m);
        EXPECT_EQ(15, d);

        const auto local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
        const auto d2 = OCI_DateCreate(env);
        ASSERT_TRUE(OCI_DateSetDateTime(d2, 2024, 1, 2, 3, 4, 5));
        ASSERT_TRUE(OCI_ElemSetDate(local, d2));
        const auto d3 = OCI_ElemGetDate(local);
        ASSERT_NE(nullptr, d3);
        EXPECT_EQ(0, OCI_DateCompare(d2, d3));
        ASSERT_TRUE(OCI_DateFree(d2));
        ASSERT_TRUE(OCI_ElemFree(local));

        ExecDML(OTEXT("drop type TestElement_AllDateVarray"));
    }

    static void ElemTsVarray(OCI_Environment* env, OCI_Connection*, OCI_Statement* stmt)
    {
        ExecDML(OTEXT("create or replace type TestElement_AllTsVarray as varray(2) of timestamp"));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_AllTsVarray(TIMESTAMP '2023-06-15 12:30:00') FROM DUAL")));
        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 0);
        const auto elem = OCI_CollGetElem(coll, 0);
        const auto ts = OCI_ElemGetTimestamp(elem);
        ASSERT_NE(nullptr, ts);

        const auto local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
        const auto ts2 = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        ASSERT_TRUE(OCI_TimestampConstruct(ts2, 2024, 1, 2, 3, 4, 5, 0, nullptr));
        ASSERT_TRUE(OCI_ElemSetTimestamp(local, ts2));
        const auto ts3 = OCI_ElemGetTimestamp(local);
        ASSERT_NE(nullptr, ts3);
        EXPECT_EQ(0, OCI_TimestampCompare(ts2, ts3));
        ASSERT_TRUE(OCI_TimestampFree(ts2));
        ASSERT_TRUE(OCI_ElemFree(local));

        ExecDML(OTEXT("drop type TestElement_AllTsVarray"));
    }

    static void ElemItvVarray(OCI_Environment* env, OCI_Connection*, OCI_Statement* stmt)
    {
        ExecDML(OTEXT("create or replace type TestElement_AllItvVarray as varray(2) of interval day to second"));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_AllItvVarray(INTERVAL '1 02:03:04' DAY TO SECOND) FROM DUAL")));
        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 0);
        const auto elem = OCI_CollGetElem(coll, 0);
        const auto itv = OCI_ElemGetInterval(elem);
        ASSERT_NE(nullptr, itv);
        OCI_Int32 day = 0, hour = 0, min = 0, sec = 0, nsec = 0;
        ASSERT_TRUE(OCI_IntervalGetDaySecond(itv, &day, &hour, &min, &sec, &nsec));
        EXPECT_EQ(1, day);
        EXPECT_EQ(2, hour);

        const auto local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
        const auto itv2 = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        ASSERT_TRUE(OCI_IntervalSetDaySecond(itv2, 5, 6, 7, 8, 0));
        ASSERT_TRUE(OCI_ElemSetInterval(local, itv2));
        const auto itv3 = OCI_ElemGetInterval(local);
        ASSERT_NE(nullptr, itv3);
        EXPECT_EQ(0, OCI_IntervalCompare(itv2, itv3));
        ASSERT_TRUE(OCI_IntervalFree(itv2));
        ASSERT_TRUE(OCI_ElemFree(local));

        ExecDML(OTEXT("drop type TestElement_AllItvVarray"));
    }

    static void ElemRawVarray(OCI_Environment*, OCI_Connection*, OCI_Statement* stmt)
    {
        ExecDML(OTEXT("create or replace type TestElement_AllRawVarray as varray(2) of raw(20)"));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_AllRawVarray(HEXTORAW('00010203'), HEXTORAW('0405')) FROM DUAL")));
        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 0);
        const auto elem = OCI_CollGetElem(coll, 0);
        unsigned char out[20] = {};
        ASSERT_EQ(4u, OCI_ElemGetRaw(elem, out, sizeof(out)));
        EXPECT_EQ(4u, OCI_ElemGetRawSize(elem));
        unsigned char expected[] = { 0x00, 0x01, 0x02, 0x03 };
        EXPECT_EQ(0, memcmp(expected, out, 4));

        const auto local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
        unsigned char data[] = { 0xAA, 0xBB, 0xCC };
        ASSERT_TRUE(OCI_ElemSetRaw(local, data, sizeof(data)));
        unsigned char out2[20] = {};
        EXPECT_EQ(3u, OCI_ElemGetRaw(local, out2, sizeof(out2)));
        EXPECT_EQ(0, memcmp(data, out2, 3));
        ASSERT_TRUE(OCI_ElemFree(local));

        ExecDML(OTEXT("drop type TestElement_AllRawVarray"));
    }

    static void ElemObjVarray(OCI_Environment*, OCI_Connection* conn, OCI_Statement* stmt)
    {
        ExecDML(OTEXT("create or replace type TestElement_AllObjT as object(code int, name varchar2(20))"));
        ExecDML(OTEXT("create or replace type TestElement_AllObjVarray as varray(2) of TestElement_AllObjT"));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_AllObjVarray(TestElement_AllObjT(1,'a'), TestElement_AllObjT(2,'b')) FROM DUAL")));
        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 0);
        const auto elem = OCI_CollGetElem(coll, 0);
        const auto obj = OCI_ElemGetObject(elem);
        ASSERT_NE(nullptr, obj);
        EXPECT_EQ(1, OCI_ObjectGetInt(obj, OTEXT("CODE")));

        const auto local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
        const auto objType = OCI_TypeInfoGet(conn, OTEXT("TestElement_AllObjT"), OCI_TIF_TYPE);
        const auto obj2 = OCI_ObjectCreate(objType);
        ASSERT_TRUE(OCI_ObjectSetInt(obj2, OTEXT("CODE"), 9));
        ASSERT_TRUE(OCI_ObjectSetString(obj2, OTEXT("NAME"), OTEXT("local")));
        ASSERT_TRUE(OCI_ElemSetObject(local, obj2));
        const auto obj3 = OCI_ElemGetObject(local);
        ASSERT_NE(nullptr, obj3);
        EXPECT_EQ(9, OCI_ObjectGetInt(obj3, OTEXT("CODE")));
        ASSERT_TRUE(OCI_ObjectFree(obj2));
        ASSERT_TRUE(OCI_TypeInfoFree(objType));
        ASSERT_TRUE(OCI_ElemFree(local));

        ExecDML(OTEXT("drop type TestElement_AllObjVarray"));
        ExecDML(OTEXT("drop type TestElement_AllObjT"));
    }

    static void ElemCollVarray(OCI_Environment*, OCI_Connection* conn, OCI_Statement* stmt)
    {
        ExecDML(OTEXT("create or replace type TestElement_AllNumVarray as varray(3) of number"));
        ExecDML(OTEXT("create or replace type TestElement_AllCollVarray as varray(2) of TestElement_AllNumVarray"));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_AllCollVarray(TestElement_AllNumVarray(1,2), TestElement_AllNumVarray(3,4)) FROM DUAL")));
        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 0);
        const auto elem = OCI_CollGetElem(coll, 0);
        const auto inner = OCI_ElemGetColl(elem);
        ASSERT_NE(nullptr, inner);
        EXPECT_EQ(2u, OCI_CollGetCount(inner));

        const auto local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
        const auto innerType = OCI_TypeInfoGet(conn, OTEXT("TestElement_AllNumVarray"), OCI_TIF_TYPE);
        const auto innerColl = OCI_CollCreate(innerType);
        const auto e = OCI_ElemCreate(innerType);
        ASSERT_TRUE(OCI_ElemSetInt(e, 5));
        ASSERT_TRUE(OCI_CollAppend(innerColl, e));
        ASSERT_TRUE(OCI_ElemFree(e));
        ASSERT_TRUE(OCI_ElemSetColl(local, innerColl));
        const auto inner2 = OCI_ElemGetColl(local);
        ASSERT_NE(nullptr, inner2);
        EXPECT_EQ(1u, OCI_CollGetCount(inner2));
        ASSERT_TRUE(OCI_CollFree(innerColl));
        ASSERT_TRUE(OCI_TypeInfoFree(innerType));
        ASSERT_TRUE(OCI_ElemFree(local));

        ExecDML(OTEXT("drop type TestElement_AllCollVarray"));
        ExecDML(OTEXT("drop type TestElement_AllNumVarray"));
    }


    static void ElemLobColl(OCI_Environment*, OCI_Connection* conn, OCI_Statement* stmt)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_LobColl AS VARRAY(2) OF CLOB"));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_LobColl(TO_CLOB('hello'), TO_CLOB('world')) FROM DUAL")));
        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 0);
        ASSERT_NE(nullptr, coll);
        const auto elem = OCI_CollGetElem(coll, 0);
        ASSERT_NE(nullptr, elem);
        const auto lob = OCI_ElemGetLob(elem);
        ASSERT_NE(nullptr, lob);
        EXPECT_EQ((OCI_UInt64) 5, OCI_LobGetLength(lob));

        const auto local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
        ASSERT_NE(nullptr, local);
        const auto lob2 = OCI_LobCreate(conn, OCI_CLOB);
        ASSERT_NE(nullptr, lob2);
        OCI_UInt32 lob_cc = 5, lob_bc = 0;
        ASSERT_TRUE(OCI_LobWrite(lob2, (void*) OTEXT("tests"), &lob_cc, &lob_bc));
        ASSERT_TRUE(OCI_ElemSetLob(local, lob2));
        const auto lob3 = OCI_ElemGetLob(local);
        ASSERT_NE(nullptr, lob3);
        EXPECT_EQ((OCI_UInt64) 5, OCI_LobGetLength(lob3));
        ASSERT_TRUE(OCI_ElemSetLob(local, nullptr));
        EXPECT_TRUE(OCI_ElemIsNull(local));

        ASSERT_TRUE(OCI_LobFree(lob2));
        ASSERT_TRUE(OCI_ElemFree(local));

        ExecDML(OTEXT("DROP TYPE TestElement_LobColl"));
    }

    static void ElemFileColl(OCI_Environment*, OCI_Connection* conn, OCI_Statement* stmt)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_FileColl AS VARRAY(1) OF BFILE"));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_FileColl(BFILENAME('FAKE_DIR', 'a.txt')) FROM DUAL")));
        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 0);
        ASSERT_NE(nullptr, coll);
        const auto elem = OCI_CollGetElem(coll, 0);
        ASSERT_NE(nullptr, elem);
        const auto file = OCI_ElemGetFile(elem);
        ASSERT_NE(nullptr, file);
        EXPECT_EQ(String(OTEXT("FAKE_DIR")), String(OCI_FileGetDirectory(file)));
        EXPECT_EQ(String(OTEXT("a.txt")), String(OCI_FileGetName(file)));

        const auto local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
        ASSERT_NE(nullptr, local);
        const auto file2 = OCI_FileCreate(conn, OCI_BFILE);
        ASSERT_NE(nullptr, file2);
        ASSERT_TRUE(OCI_FileSetName(file2, OTEXT("FAKE_DIR"), OTEXT("b.txt")));
        ASSERT_TRUE(OCI_ElemSetFile(local, file2));
        const auto file3 = OCI_ElemGetFile(local);
        ASSERT_NE(nullptr, file3);
        EXPECT_EQ(String(OTEXT("b.txt")), String(OCI_FileGetName(file3)));
        ASSERT_TRUE(OCI_ElemSetFile(local, nullptr));
        EXPECT_TRUE(OCI_ElemIsNull(local));

        ASSERT_TRUE(OCI_FileFree(file2));
        ASSERT_TRUE(OCI_ElemFree(local));

        ExecDML(OTEXT("DROP TYPE TestElement_FileColl"));
    }

    static void ElemRefColl(OCI_Environment*, OCI_Connection* conn, OCI_Statement* stmt)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_RefT AS OBJECT(code int)"));
        ExecDML(OTEXT("CREATE TABLE TestElement_RefTable OF TestElement_RefT"));
        ExecDML(OTEXT("INSERT INTO TestElement_RefTable VALUES (1)"));
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_RefColl AS VARRAY(1) OF REF TestElement_RefT"));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_RefColl(REF(t)) FROM TestElement_RefTable t")));
        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 0);
        ASSERT_NE(nullptr, coll);
        const auto elem = OCI_CollGetElem(coll, 0);
        ASSERT_NE(nullptr, elem);
        const auto ref = OCI_ElemGetRef(elem);
        ASSERT_NE(nullptr, ref);
        EXPECT_FALSE(OCI_RefIsNull(ref));

        const auto local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
        ASSERT_NE(nullptr, local);
        const auto objType = OCI_TypeInfoGet(conn, OTEXT("TestElement_RefT"), OCI_TIF_TYPE);
        const auto ref2 = OCI_RefCreate(objType);
        ASSERT_NE(nullptr, ref2);
        ASSERT_TRUE(OCI_RefAssign(ref2, ref));
        ASSERT_TRUE(OCI_ElemSetRef(local, ref2));
        const auto ref3 = OCI_ElemGetRef(local);
        ASSERT_NE(nullptr, ref3);
        EXPECT_FALSE(OCI_RefIsNull(ref3));
        ASSERT_TRUE(OCI_ElemSetRef(local, nullptr));
        EXPECT_TRUE(OCI_ElemIsNull(local));

        ASSERT_TRUE(OCI_RefFree(ref2));
        ASSERT_TRUE(OCI_ElemFree(local));
        ASSERT_TRUE(OCI_TypeInfoFree(objType));

        ExecDML(OTEXT("DROP TYPE TestElement_RefColl"));
        ExecDML(OTEXT("DROP TABLE TestElement_RefTable"));
        ExecDML(OTEXT("DROP TYPE TestElement_RefT"));
    }
    class TestElemGetSet : public ::testing::TestWithParam<ElemCase>
    {
    protected:
        OCI_Environment* env = nullptr;
        OCI_Connection* conn = nullptr;
        OCI_Statement* stmt = nullptr;

        void SetUp() override
        {
            env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);
            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
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

    TEST_P(TestElemGetSet, FetchedElemRoundTripsAndLocalElemAcceptsType)
    {
        GetParam().run(env, conn, stmt);
    }

    INSTANTIATE_TEST_CASE_P(ElemType, TestElemGetSet,
        ::testing::Values(
            ElemCase{ "NumVarray",  ElemNumVarray },
            ElemCase{ "StrVarray",  ElemStrVarray },
            ElemCase{ "DateVarray", ElemDateVarray },
            ElemCase{ "TsVarray",   ElemTsVarray },
            ElemCase{ "ItvVarray",  ElemItvVarray },
            ElemCase{ "RawVarray",  ElemRawVarray },
            ElemCase{ "ObjVarray",  ElemObjVarray },
            ElemCase{ "CollVarray", ElemCollVarray },
            ElemCase{ "LobColl",    ElemLobColl },
            ElemCase{ "FileColl",   ElemFileColl },
            ElemCase{ "RefColl",    ElemRefColl }),
        ::testing::PrintToStringParamName());

    /* -------------------------------------------------------------------------
       Parameterized: numeric elem setter/getter round-trip per subtype
       ------------------------------------------------------------------------- */

    struct ElemNumCase
    {
        const char* name;
        void (*run)(OCI_Elem*);
    };

    void PrintTo(const ElemNumCase& c, std::ostream* os) { *os << c.name; }

    static void ElemNumSetShort(OCI_Elem* e)
    {
        ASSERT_TRUE(OCI_ElemSetShort(e, 5));
        EXPECT_EQ(5, OCI_ElemGetInt(e));
    }
    static void ElemNumSetUShort(OCI_Elem* e)
    {
        ASSERT_TRUE(OCI_ElemSetUnsignedShort(e, 6));
        EXPECT_EQ(6, OCI_ElemGetInt(e));
    }
    static void ElemNumSetInt(OCI_Elem* e)
    {
        ASSERT_TRUE(OCI_ElemSetInt(e, 7));
        EXPECT_EQ(7, OCI_ElemGetInt(e));
    }
    static void ElemNumSetUInt(OCI_Elem* e)
    {
        ASSERT_TRUE(OCI_ElemSetUnsignedInt(e, 8u));
        EXPECT_EQ(8, OCI_ElemGetInt(e));
    }
    static void ElemNumSetBigInt(OCI_Elem* e)
    {
        ASSERT_TRUE(OCI_ElemSetBigInt(e, static_cast<OCI_Int64>(9)));
        EXPECT_EQ(9, OCI_ElemGetInt(e));
    }
    static void ElemNumSetUBigInt(OCI_Elem* e)
    {
        ASSERT_TRUE(OCI_ElemSetUnsignedBigInt(e, static_cast<OCI_UInt64>(10)));
        EXPECT_EQ(10, OCI_ElemGetInt(e));
    }
    static void ElemNumSetFloat(OCI_Elem* e)
    {
        ASSERT_TRUE(OCI_ElemSetFloat(e, 1.5f));
        EXPECT_NEAR(1.5, OCI_ElemGetDouble(e), 0.01);
    }
    static void ElemNumSetDouble(OCI_Elem* e)
    {
        ASSERT_TRUE(OCI_ElemSetDouble(e, 2.5));
        EXPECT_NEAR(2.5, OCI_ElemGetDouble(e), 0.01);
    }

    class TestElemNum : public ::testing::TestWithParam<ElemNumCase>
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_Statement* stmt = nullptr;
        OCI_Coll* coll = nullptr;
        OCI_Elem* local = nullptr;

        void SetUp() override
        {
            ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NumVar2 AS VARRAY(2) OF NUMBER"));

            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);
            ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_NumVar2(3.14, -2) FROM DUAL")));
            const auto rs = OCI_GetResultset(stmt);
            ASSERT_NE(nullptr, rs);
            ASSERT_TRUE(OCI_FetchNext(rs));
            coll = OCI_GetColl(rs, 0);
            ASSERT_NE(nullptr, coll);
            local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
            ASSERT_NE(nullptr, local);
        }

        void TearDown() override
        {
            if (local) OCI_ElemFree(local);
            if (stmt) OCI_StatementFree(stmt);
            if (conn) OCI_ConnectionFree(conn);

            ExecDML(OTEXT("DROP TYPE TestElement_NumVar2"));
        }
    };

    TEST_P(TestElemNum, SetThenGetOnLocalElem_RoundTripsValue)
    {
        GetParam().run(local);
    }

    INSTANTIATE_TEST_CASE_P(NumSubtype, TestElemNum,
        ::testing::Values(
            ElemNumCase{ "Short",   ElemNumSetShort },
            ElemNumCase{ "UShort",  ElemNumSetUShort },
            ElemNumCase{ "Int",     ElemNumSetInt },
            ElemNumCase{ "UInt",    ElemNumSetUInt },
            ElemNumCase{ "BigInt",  ElemNumSetBigInt },
            ElemNumCase{ "UBigInt", ElemNumSetUBigInt },
            ElemNumCase{ "Float",   ElemNumSetFloat },
            ElemNumCase{ "Double",  ElemNumSetDouble }),
        ::testing::PrintToStringParamName());

    TEST_F(TestElemNum, FetchedNumElem_CoercesToAllNumericGetters)
    {
        const auto elem = OCI_CollGetElem(coll, 0);
        ASSERT_NE(nullptr, elem);

        EXPECT_EQ(3, OCI_ElemGetShort(elem));
        EXPECT_EQ(3u, OCI_ElemGetUnsignedShort(elem));
        EXPECT_EQ(3, OCI_ElemGetInt(elem));
        EXPECT_EQ(3u, OCI_ElemGetUnsignedInt(elem));
        EXPECT_EQ(3, OCI_ElemGetBigInt(elem));
        EXPECT_EQ(3u, OCI_ElemGetUnsignedBigInt(elem));
        EXPECT_NEAR(3.14f, OCI_ElemGetFloat(elem), 0.01f);
        EXPECT_NEAR(3.14, OCI_ElemGetDouble(elem), 0.01);
    }




    /* -------------------------------------------------------------------------
       Parameterized: typed NULL setter marks local elem null per elem type
       ------------------------------------------------------------------------- */

    struct ElemNullCase
    {
        const char* name;
        void (*run)(OCI_Connection*);
    };

    void PrintTo(const ElemNullCase& c, std::ostream* os) { *os << c.name; }

    static void ElemNullSetObject(OCI_Connection* conn)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NullObjT AS OBJECT(code int, name varchar2(20))"));
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NullObjColl AS VARRAY(1) OF TestElement_NullObjT"));
        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestElement_NullObjColl"), OCI_TIF_TYPE);
        const auto local = OCI_ElemCreate(typinf);
        ASSERT_TRUE(OCI_ElemSetObject(local, nullptr));
        EXPECT_TRUE(OCI_ElemIsNull(local));
        ASSERT_TRUE(OCI_ElemFree(local));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ExecDML(OTEXT("DROP TYPE TestElement_NullObjColl"));
        ExecDML(OTEXT("DROP TYPE TestElement_NullObjT"));
    }
    static void ElemNullSetColl(OCI_Connection* conn)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NullNumColl AS VARRAY(1) OF NUMBER"));
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NullCollColl AS VARRAY(1) OF TestElement_NullNumColl"));
        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestElement_NullCollColl"), OCI_TIF_TYPE);
        const auto local = OCI_ElemCreate(typinf);
        ASSERT_TRUE(OCI_ElemSetColl(local, nullptr));
        EXPECT_TRUE(OCI_ElemIsNull(local));
        ASSERT_TRUE(OCI_ElemFree(local));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ExecDML(OTEXT("DROP TYPE TestElement_NullCollColl"));
        ExecDML(OTEXT("DROP TYPE TestElement_NullNumColl"));
    }
    static void ElemNullSetString(OCI_Connection* conn)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NullStrColl AS VARRAY(1) OF VARCHAR2(20)"));
        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestElement_NullStrColl"), OCI_TIF_TYPE);
        const auto local = OCI_ElemCreate(typinf);
        ASSERT_TRUE(OCI_ElemSetString(local, nullptr));
        EXPECT_TRUE(OCI_ElemIsNull(local));
        ASSERT_TRUE(OCI_ElemFree(local));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ExecDML(OTEXT("DROP TYPE TestElement_NullStrColl"));
    }
    static void ElemNullSetRaw(OCI_Connection* conn)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NullRawColl AS VARRAY(1) OF RAW(10)"));
        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestElement_NullRawColl"), OCI_TIF_TYPE);
        const auto local = OCI_ElemCreate(typinf);
        ASSERT_TRUE(OCI_ElemSetRaw(local, nullptr, 0));
        EXPECT_TRUE(OCI_ElemIsNull(local));
        ASSERT_TRUE(OCI_ElemFree(local));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ExecDML(OTEXT("DROP TYPE TestElement_NullRawColl"));
    }
    static void ElemNullSetNumber(OCI_Connection* conn)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NullNumColl AS VARRAY(1) OF NUMBER"));
        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestElement_NullNumColl"), OCI_TIF_TYPE);
        const auto local = OCI_ElemCreate(typinf);
        ASSERT_TRUE(OCI_ElemSetNumber(local, nullptr));
        EXPECT_TRUE(OCI_ElemIsNull(local));
        ASSERT_TRUE(OCI_ElemFree(local));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ExecDML(OTEXT("DROP TYPE TestElement_NullNumColl"));
    }
    static void ElemNullSetDate(OCI_Connection* conn)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NullDateColl AS VARRAY(1) OF DATE"));
        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestElement_NullDateColl"), OCI_TIF_TYPE);
        const auto local = OCI_ElemCreate(typinf);
        ASSERT_TRUE(OCI_ElemSetDate(local, nullptr));
        EXPECT_TRUE(OCI_ElemIsNull(local));
        ASSERT_TRUE(OCI_ElemFree(local));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ExecDML(OTEXT("DROP TYPE TestElement_NullDateColl"));
    }
    static void ElemNullSetTimestamp(OCI_Connection* conn)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NullTsColl AS VARRAY(1) OF TIMESTAMP"));
        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestElement_NullTsColl"), OCI_TIF_TYPE);
        const auto local = OCI_ElemCreate(typinf);
        ASSERT_TRUE(OCI_ElemSetTimestamp(local, nullptr));
        EXPECT_TRUE(OCI_ElemIsNull(local));
        ASSERT_TRUE(OCI_ElemFree(local));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ExecDML(OTEXT("DROP TYPE TestElement_NullTsColl"));
    }
    static void ElemNullSetInterval(OCI_Connection* conn)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NullItvColl AS VARRAY(1) OF INTERVAL DAY TO SECOND"));
        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestElement_NullItvColl"), OCI_TIF_TYPE);
        const auto local = OCI_ElemCreate(typinf);
        ASSERT_TRUE(OCI_ElemSetInterval(local, nullptr));
        EXPECT_TRUE(OCI_ElemIsNull(local));
        ASSERT_TRUE(OCI_ElemFree(local));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ExecDML(OTEXT("DROP TYPE TestElement_NullItvColl"));
    }

    class TestElemNull : public ::testing::TestWithParam<ElemNullCase>
    {
    protected:
        OCI_Connection* conn = nullptr;

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

    TEST_P(TestElemNull, SetNullOnLocalElem_MarksElemNull)
    {
        GetParam().run(conn);
    }

    INSTANTIATE_TEST_CASE_P(ElemType, TestElemNull,
        ::testing::Values(
            ElemNullCase{ "Object",    ElemNullSetObject },
            ElemNullCase{ "Coll",      ElemNullSetColl },
            ElemNullCase{ "String",    ElemNullSetString },
            ElemNullCase{ "Raw",       ElemNullSetRaw },
            ElemNullCase{ "Number",    ElemNullSetNumber },
            ElemNullCase{ "Date",      ElemNullSetDate },
            ElemNullCase{ "Timestamp", ElemNullSetTimestamp },
            ElemNullCase{ "Interval",  ElemNullSetInterval }),
        ::testing::PrintToStringParamName());

    TEST(TestElement, GetElem2NullObject)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_ObjForNullT AS OBJECT(code int)"));
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_ObjForNullColl AS VARRAY(1) OF TestElement_ObjForNullT"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestElement_ObjForNullColl"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto coll = OCI_CollCreate(typinf);
        ASSERT_NE(nullptr, coll);

        const auto elem = OCI_CollGetElem(coll, 0);
        ASSERT_EQ(nullptr, elem);
        ASSERT_TRUE(OCI_CollFree(coll));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TYPE TestElement_ObjForNullColl"));
        ExecDML(OTEXT("DROP TYPE TestElement_ObjForNullT"));
    }

    class TestElementBool : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_BoolColl AS VARRAY(2) OF BOOLEAN"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("DROP TYPE TestElement_BoolColl"));
        }
    };

    TEST_F(TestElementBool, GetBooleanOnFetchedElem_ReturnsValue)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        if (OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_BoolColl(TRUE, FALSE) FROM DUAL")))
        {
            const auto rs = OCI_GetResultset(stmt);
            ASSERT_NE(nullptr, rs);
            ASSERT_TRUE(OCI_FetchNext(rs));

            const auto coll = OCI_GetColl(rs, 0);
            ASSERT_NE(nullptr, coll);

            const auto elem = OCI_CollGetElem(coll, 0);
            ASSERT_NE(nullptr, elem);
            ASSERT_TRUE(OCI_ElemGetBoolean(elem));
        }

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST_F(TestElementBool, SetBooleanOnLocalElem_RoundTrips)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestElement_BoolColl"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto local = OCI_ElemCreate(typinf);
        ASSERT_NE(nullptr, local);

        ASSERT_TRUE(OCI_ElemSetBoolean(local, TRUE));
        ASSERT_TRUE(OCI_ElemGetBoolean(local));

        ASSERT_TRUE(OCI_ElemFree(local));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestElement, GetIntFromTextElement)
    {
        /* text element read as a number goes through the string conversion arm */

        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_TextColl AS VARRAY(2) OF VARCHAR2(30)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestElement_TextColl"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto elem = OCI_ElemCreate(typinf);
        ASSERT_NE(nullptr, elem);

        ASSERT_TRUE(OCI_ElemSetString(elem, OTEXT("123")));
        ASSERT_EQ(123, OCI_ElemGetInt(elem));

        ASSERT_TRUE(OCI_ElemFree(elem));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TYPE TestElement_TextColl"));
    }

    TEST(TestElement, GetIntFromIncompatibleType)
    {
        /* a non numeric / non text element read as a number throws TypeNotCompatible */

        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_DateColl AS VARRAY(2) OF DATE"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestElement_DateColl"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto elem = OCI_ElemCreate(typinf);
        ASSERT_NE(nullptr, elem);

        const auto dt = OCI_DateCreate(env);
        ASSERT_NE(nullptr, dt);
        ASSERT_TRUE(OCI_DateSysDate(dt));
        ASSERT_TRUE(OCI_ElemSetDate(elem, dt));

        ASSERT_EQ(0, OCI_ElemGetInt(elem));
        ASSERT_NE(nullptr, OCI_GetLastError());

        ASSERT_TRUE(OCI_DateFree(dt));
        ASSERT_TRUE(OCI_ElemFree(elem));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TYPE TestElement_DateColl"));
    }

    class TestElementNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestElementNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestElementNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "ElemCreate", [] { return nullptr == OCI_ElemCreate(nullptr); } },
            { "ElemFree", [] { return !OCI_ElemFree(nullptr); } },
            { "ElemGetBigInt", [] { return 0 == OCI_ElemGetBigInt(nullptr); } },
            { "ElemGetBoolean", [] { return !OCI_ElemGetBoolean(nullptr); } },
            { "ElemGetColl", [] { return nullptr == OCI_ElemGetColl(nullptr); } },
            { "ElemGetDate", [] { return nullptr == OCI_ElemGetDate(nullptr); } },
            { "ElemGetDouble", [] { return 0.0 == OCI_ElemGetDouble(nullptr); } },
            { "ElemGetFile", [] { return nullptr == OCI_ElemGetFile(nullptr); } },
            { "ElemGetFloat", [] { return 0.0f == OCI_ElemGetFloat(nullptr); } },
            { "ElemGetInt", [] { return 0 == OCI_ElemGetInt(nullptr); } },
            { "ElemGetInterval", [] { return nullptr == OCI_ElemGetInterval(nullptr); } },
            { "ElemGetLob", [] { return nullptr == OCI_ElemGetLob(nullptr); } },
            { "ElemGetNumber", [] { return nullptr == OCI_ElemGetNumber(nullptr); } },
            { "ElemGetObject", [] { return nullptr == OCI_ElemGetObject(nullptr); } },
            { "ElemGetRaw", [] { return 0 == OCI_ElemGetRaw(nullptr, NullArgBuffer(), 1); } },
            { "ElemGetRawSize", [] { return 0 == OCI_ElemGetRawSize(nullptr); } },
            { "ElemGetRef", [] { return nullptr == OCI_ElemGetRef(nullptr); } },
            { "ElemGetShort", [] { return 0 == OCI_ElemGetShort(nullptr); } },
            { "ElemGetString", [] { return nullptr == OCI_ElemGetString(nullptr); } },
            { "ElemGetTimestamp", [] { return nullptr == OCI_ElemGetTimestamp(nullptr); } },
            { "ElemGetUnsignedBigInt", [] { return 0 == OCI_ElemGetUnsignedBigInt(nullptr); } },
            { "ElemGetUnsignedInt", [] { return 0 == OCI_ElemGetUnsignedInt(nullptr); } },
            { "ElemGetUnsignedShort", [] { return 0 == OCI_ElemGetUnsignedShort(nullptr); } },
            { "ElemIsNull", [] { return !OCI_ElemIsNull(nullptr); } },
            { "ElemSetBigInt", [] { return !OCI_ElemSetBigInt(nullptr, 1); } },
            { "ElemSetBoolean", [] { return !OCI_ElemSetBoolean(nullptr, FALSE); } },
            { "ElemSetColl", [] { return !OCI_ElemSetColl(nullptr, nullptr); } },
            { "ElemSetDate", [] { return !OCI_ElemSetDate(nullptr, nullptr); } },
            { "ElemSetDouble", [] { return !OCI_ElemSetDouble(nullptr, 1); } },
            { "ElemSetFile", [] { return !OCI_ElemSetFile(nullptr, nullptr); } },
            { "ElemSetFloat", [] { return !OCI_ElemSetFloat(nullptr, 1); } },
            { "ElemSetInt", [] { return !OCI_ElemSetInt(nullptr, 1); } },
            { "ElemSetInterval", [] { return !OCI_ElemSetInterval(nullptr, nullptr); } },
            { "ElemSetLob", [] { return !OCI_ElemSetLob(nullptr, nullptr); } },
            { "ElemSetNull", [] { return !OCI_ElemSetNull(nullptr); } },
            { "ElemSetNumber", [] { return !OCI_ElemSetNumber(nullptr, nullptr); } },
            { "ElemSetObject", [] { return !OCI_ElemSetObject(nullptr, nullptr); } },
            { "ElemSetRaw", [] { return !OCI_ElemSetRaw(nullptr, NullArgBuffer(), 1); } },
            { "ElemSetRef", [] { return !OCI_ElemSetRef(nullptr, nullptr); } },
            { "ElemSetShort", [] { return !OCI_ElemSetShort(nullptr, 1); } },
            { "ElemSetString", [] { return !OCI_ElemSetString(nullptr, OTEXT("x")); } },
            { "ElemSetTimestamp", [] { return !OCI_ElemSetTimestamp(nullptr, nullptr); } },
            { "ElemSetUnsignedBigInt", [] { return !OCI_ElemSetUnsignedBigInt(nullptr, 1); } },
            { "ElemSetUnsignedInt", [] { return !OCI_ElemSetUnsignedInt(nullptr, 1); } },
            { "ElemSetUnsignedShort", [] { return !OCI_ElemSetUnsignedShort(nullptr, 1); } },
        }),
        &NullArgCaseName);

}
