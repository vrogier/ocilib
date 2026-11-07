#include "ocilib_tests.h"

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

    TEST(TestElement, AllGetAndSetTypes)
    {
        ExecDML(OTEXT("create or replace type TestElement_AllObjT as object(code int, name varchar2(20))"));
        ExecDML(OTEXT("create or replace type TestElement_AllNumVarray as varray(3) of number"));
        ExecDML(OTEXT("create or replace type TestElement_AllStrVarray as varray(3) of varchar2(50)"));
        ExecDML(OTEXT("create or replace type TestElement_AllDateVarray as varray(2) of date"));
        ExecDML(OTEXT("create or replace type TestElement_AllTsVarray as varray(2) of timestamp"));
        ExecDML(OTEXT("create or replace type TestElement_AllItvVarray as varray(2) of interval day to second"));
        ExecDML(OTEXT("create or replace type TestElement_AllRawVarray as varray(2) of raw(20)"));
        ExecDML(OTEXT("create or replace type TestElement_AllObjVarray as varray(2) of TestElement_AllObjT"));
        ExecDML(OTEXT("create or replace type TestElement_AllCollVarray as varray(2) of TestElement_AllNumVarray"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        // ---- number varray ----
        {
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
            ASSERT_DOUBLE_EQ(3.14, d);
            ASSERT_EQ(3, OCI_ElemGetInt(elem));

            const auto local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
            ASSERT_NE(nullptr, local);
            ASSERT_TRUE(OCI_ElemSetInt(local, 5));
            ASSERT_EQ(5, OCI_ElemGetInt(local));
            ASSERT_TRUE(OCI_ElemSetDouble(local, 6.5));
            ASSERT_DOUBLE_EQ(6.5, OCI_ElemGetDouble(local));

            const auto num2 = OCI_NumberCreate(env);
            OCI_Int32 n = 7;
            ASSERT_TRUE(OCI_NumberSetValue(num2, OCI_NUM_INT, &n));
            ASSERT_TRUE(OCI_ElemSetNumber(local, num2));
            ASSERT_EQ(7, OCI_ElemGetInt(local));
            ASSERT_TRUE(OCI_NumberFree(num2));

            ASSERT_TRUE(OCI_ElemSetNull(local));
            ASSERT_TRUE(OCI_ElemIsNull(local));
            ASSERT_TRUE(OCI_ElemFree(local));
        }

        // ---- string varray ----
        {
            ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_AllStrVarray('alpha', 'beta') FROM DUAL")));
            const auto rs = OCI_GetResultset(stmt);
            ASSERT_NE(nullptr, rs);
            ASSERT_TRUE(OCI_FetchNext(rs));

            const auto coll = OCI_GetColl(rs, 0);
            const auto elem = OCI_CollGetElem(coll, 0);
            ASSERT_EQ(String(OTEXT("alpha")), String(OCI_ElemGetString(elem)));

            const auto local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
            ASSERT_TRUE(OCI_ElemSetString(local, OTEXT("local value")));
            ASSERT_EQ(String(OTEXT("local value")), String(OCI_ElemGetString(local)));
            ASSERT_TRUE(OCI_ElemSetString(local, nullptr));
            ASSERT_TRUE(OCI_ElemIsNull(local));
            ASSERT_TRUE(OCI_ElemFree(local));
        }

        // ---- date varray ----
        {
            ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_AllDateVarray(DATE '2023-06-15') FROM DUAL")));
            const auto rs = OCI_GetResultset(stmt);
            ASSERT_TRUE(OCI_FetchNext(rs));

            const auto coll = OCI_GetColl(rs, 0);
            const auto elem = OCI_CollGetElem(coll, 0);
            const auto date = OCI_ElemGetDate(elem);
            ASSERT_NE(nullptr, date);
            OCI_Int32 y = 0, m = 0, d = 0;
            ASSERT_TRUE(OCI_DateGetDate(date, &y, &m, &d));
            ASSERT_EQ(2023, y);
            ASSERT_EQ(6, m);
            ASSERT_EQ(15, d);

            const auto local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
            const auto d2 = OCI_DateCreate(env);
            ASSERT_TRUE(OCI_DateSetDateTime(d2, 2024, 1, 2, 3, 4, 5));
            ASSERT_TRUE(OCI_ElemSetDate(local, d2));
            const auto d3 = OCI_ElemGetDate(local);
            ASSERT_NE(nullptr, d3);
            ASSERT_EQ(0, OCI_DateCompare(d2, d3));
            ASSERT_TRUE(OCI_DateFree(d2));
            ASSERT_TRUE(OCI_ElemFree(local));
        }

        // ---- timestamp varray ----
        {
            ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_AllTsVarray(TIMESTAMP '2023-06-15 12:30:00') FROM DUAL")));
            const auto rs = OCI_GetResultset(stmt);
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
            ASSERT_EQ(0, OCI_TimestampCompare(ts2, ts3));
            ASSERT_TRUE(OCI_TimestampFree(ts2));
            ASSERT_TRUE(OCI_ElemFree(local));
        }

        // ---- interval varray ----
        {
            ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_AllItvVarray(INTERVAL '1 02:03:04' DAY TO SECOND) FROM DUAL")));
            const auto rs = OCI_GetResultset(stmt);
            ASSERT_TRUE(OCI_FetchNext(rs));

            const auto coll = OCI_GetColl(rs, 0);
            const auto elem = OCI_CollGetElem(coll, 0);
            const auto itv = OCI_ElemGetInterval(elem);
            ASSERT_NE(nullptr, itv);
            OCI_Int32 day = 0, hour = 0, min = 0, sec = 0, nsec = 0;
            ASSERT_TRUE(OCI_IntervalGetDaySecond(itv, &day, &hour, &min, &sec, &nsec));
            ASSERT_EQ(1, day);
            ASSERT_EQ(2, hour);

            const auto local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
            const auto itv2 = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
            ASSERT_TRUE(OCI_IntervalSetDaySecond(itv2, 5, 6, 7, 8, 0));
            ASSERT_TRUE(OCI_ElemSetInterval(local, itv2));
            const auto itv3 = OCI_ElemGetInterval(local);
            ASSERT_NE(nullptr, itv3);
            ASSERT_EQ(0, OCI_IntervalCompare(itv2, itv3));
            ASSERT_TRUE(OCI_IntervalFree(itv2));
            ASSERT_TRUE(OCI_ElemFree(local));
        }

        // ---- raw varray ----
        {
            ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_AllRawVarray(HEXTORAW('00010203'), HEXTORAW('0405')) FROM DUAL")));
            const auto rs = OCI_GetResultset(stmt);
            ASSERT_TRUE(OCI_FetchNext(rs));

            const auto coll = OCI_GetColl(rs, 0);
            const auto elem = OCI_CollGetElem(coll, 0);
            unsigned char out[20] = {};
            ASSERT_EQ(4u, OCI_ElemGetRaw(elem, out, sizeof(out)));
            ASSERT_EQ(4u, OCI_ElemGetRawSize(elem));
            unsigned char expected[] = { 0x00, 0x01, 0x02, 0x03 };
            ASSERT_EQ(0, memcmp(expected, out, 4));

            const auto local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
            unsigned char data[] = { 0xAA, 0xBB, 0xCC };
            ASSERT_TRUE(OCI_ElemSetRaw(local, data, sizeof(data)));
            unsigned char out2[20] = {};
            ASSERT_EQ(3u, OCI_ElemGetRaw(local, out2, sizeof(out2)));
            ASSERT_EQ(0, memcmp(data, out2, 3));
            ASSERT_TRUE(OCI_ElemFree(local));
        }

        // ---- object varray ----
        {
            ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_AllObjVarray(TestElement_AllObjT(1,'a'), TestElement_AllObjT(2,'b')) FROM DUAL")));
            const auto rs = OCI_GetResultset(stmt);
            ASSERT_TRUE(OCI_FetchNext(rs));

            const auto coll = OCI_GetColl(rs, 0);
            const auto elem = OCI_CollGetElem(coll, 0);
            const auto obj = OCI_ElemGetObject(elem);
            ASSERT_NE(nullptr, obj);
            ASSERT_EQ(1, OCI_ObjectGetInt(obj, OTEXT("CODE")));

            const auto local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
            const auto objType = OCI_TypeInfoGet(conn, OTEXT("TestElement_AllObjT"), OCI_TIF_TYPE);
            const auto obj2 = OCI_ObjectCreate(objType);
            ASSERT_TRUE(OCI_ObjectSetInt(obj2, OTEXT("CODE"), 9));
            ASSERT_TRUE(OCI_ObjectSetString(obj2, OTEXT("NAME"), OTEXT("local")));
            ASSERT_TRUE(OCI_ElemSetObject(local, obj2));
            const auto obj3 = OCI_ElemGetObject(local);
            ASSERT_NE(nullptr, obj3);
            ASSERT_EQ(9, OCI_ObjectGetInt(obj3, OTEXT("CODE")));
            ASSERT_TRUE(OCI_ObjectFree(obj2));
            ASSERT_TRUE(OCI_TypeInfoFree(objType));
            ASSERT_TRUE(OCI_ElemFree(local));
        }

        // ---- nested collection varray ----
        {
            ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_AllCollVarray(TestElement_AllNumVarray(1,2), TestElement_AllNumVarray(3,4)) FROM DUAL")));
            const auto rs = OCI_GetResultset(stmt);
            ASSERT_TRUE(OCI_FetchNext(rs));

            const auto coll = OCI_GetColl(rs, 0);
            const auto elem = OCI_CollGetElem(coll, 0);
            const auto inner = OCI_ElemGetColl(elem);
            ASSERT_NE(nullptr, inner);
            ASSERT_EQ(2u, OCI_CollGetCount(inner));

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
            ASSERT_EQ(1u, OCI_CollGetCount(inner2));
            ASSERT_TRUE(OCI_CollFree(innerColl));
            ASSERT_TRUE(OCI_TypeInfoFree(innerType));
            ASSERT_TRUE(OCI_ElemFree(local));
        }

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop type TestElement_AllCollVarray"));
        ExecDML(OTEXT("drop type TestElement_AllObjVarray"));
        ExecDML(OTEXT("drop type TestElement_AllRawVarray"));
        ExecDML(OTEXT("drop type TestElement_AllItvVarray"));
        ExecDML(OTEXT("drop type TestElement_AllTsVarray"));
        ExecDML(OTEXT("drop type TestElement_AllDateVarray"));
        ExecDML(OTEXT("drop type TestElement_AllStrVarray"));
        ExecDML(OTEXT("drop type TestElement_AllNumVarray"));
        ExecDML(OTEXT("drop type TestElement_AllObjT"));
    }

    TEST(TestElement, NumericVariations)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NumVar2 AS VARRAY(2) OF NUMBER"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_NumVar2(3.14, -2) FROM DUAL")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto coll = OCI_GetColl(rs, 0);
        ASSERT_NE(nullptr, coll);

        const auto elem = OCI_CollGetElem(coll, 0);
        ASSERT_NE(nullptr, elem);

        ASSERT_EQ(3, OCI_ElemGetShort(elem));
        ASSERT_EQ(3u, OCI_ElemGetUnsignedShort(elem));
        ASSERT_EQ(3, OCI_ElemGetInt(elem));
        ASSERT_EQ(3u, OCI_ElemGetUnsignedInt(elem));
        ASSERT_EQ(3, OCI_ElemGetBigInt(elem));
        ASSERT_EQ(3u, OCI_ElemGetUnsignedBigInt(elem));
        ASSERT_NEAR(3.14f, OCI_ElemGetFloat(elem), 0.01f);
        ASSERT_NEAR(3.14, OCI_ElemGetDouble(elem), 0.01);

        const auto local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
        ASSERT_NE(nullptr, local);

        ASSERT_TRUE(OCI_ElemSetShort(local, 5));
        ASSERT_EQ(5, OCI_ElemGetInt(local));

        ASSERT_TRUE(OCI_ElemSetUnsignedShort(local, 6));
        ASSERT_EQ(6, OCI_ElemGetInt(local));

        ASSERT_TRUE(OCI_ElemSetInt(local, 7));
        ASSERT_EQ(7, OCI_ElemGetInt(local));

        ASSERT_TRUE(OCI_ElemSetUnsignedInt(local, 8u));
        ASSERT_EQ(8, OCI_ElemGetInt(local));

        ASSERT_TRUE(OCI_ElemSetBigInt(local, static_cast<OCI_Int64>(9)));
        ASSERT_EQ(9, OCI_ElemGetInt(local));

        ASSERT_TRUE(OCI_ElemSetUnsignedBigInt(local, static_cast<OCI_UInt64>(10)));
        ASSERT_EQ(10, OCI_ElemGetInt(local));

        ASSERT_TRUE(OCI_ElemSetFloat(local, 1.5f));
        ASSERT_NEAR(1.5, OCI_ElemGetDouble(local), 0.01);

        ASSERT_TRUE(OCI_ElemSetDouble(local, 2.5));
        ASSERT_NEAR(2.5, OCI_ElemGetDouble(local), 0.01);

        ASSERT_TRUE(OCI_ElemFree(local));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TYPE TestElement_NumVar2"));
    }

    TEST(TestElement, RawNull)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_RawVar AS VARRAY(1) OF RAW(10)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestElement_RawVar"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto local = OCI_ElemCreate(typinf);
        ASSERT_NE(nullptr, local);

        ASSERT_TRUE(OCI_ElemSetRaw(local, nullptr, 0));
        ASSERT_TRUE(OCI_ElemIsNull(local));

        ASSERT_TRUE(OCI_ElemFree(local));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TYPE TestElement_RawVar"));
    }

    TEST(TestElement, LobAndFile)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_LobColl AS VARRAY(2) OF CLOB"));
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_FileColl AS VARRAY(1) OF BFILE"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        // CLOB collection
        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_LobColl(TO_CLOB('hello'), TO_CLOB('world')) FROM DUAL")));

        auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        auto coll = OCI_GetColl(rs, 0);
        ASSERT_NE(nullptr, coll);

        auto elem = OCI_CollGetElem(coll, 0);
        ASSERT_NE(nullptr, elem);

        const auto lob = OCI_ElemGetLob(elem);
        ASSERT_NE(nullptr, lob);
        ASSERT_EQ((OCI_UInt64) 5, OCI_LobGetLength(lob));

        auto typinf = OCI_CollGetTypeInfo(coll);
        ASSERT_NE(nullptr, typinf);

        auto local = OCI_ElemCreate(typinf);
        ASSERT_NE(nullptr, local);

        const auto lob2 = OCI_LobCreate(conn, OCI_CLOB);
        ASSERT_NE(nullptr, lob2);
        OCI_UInt32 lob_cc = 5, lob_bc = 0;
        ASSERT_TRUE(OCI_LobWrite(lob2, (void*) OTEXT("tests"), &lob_cc, &lob_bc));
        ASSERT_EQ(5u, lob_cc);
        ASSERT_TRUE(OCI_ElemSetLob(local, lob2));

        const auto lob3 = OCI_ElemGetLob(local);
        ASSERT_NE(nullptr, lob3);
        ASSERT_EQ((OCI_UInt64) 5, OCI_LobGetLength(lob3));

        ASSERT_TRUE(OCI_ElemSetLob(local, nullptr));
        ASSERT_TRUE(OCI_ElemIsNull(local));

        ASSERT_TRUE(OCI_LobFree(lob2));
        ASSERT_TRUE(OCI_ElemFree(local));

        // BFILE collection
        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT TestElement_FileColl(BFILENAME('FAKE_DIR', 'a.txt')) FROM DUAL")));

        rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        coll = OCI_GetColl(rs, 0);
        ASSERT_NE(nullptr, coll);

        elem = OCI_CollGetElem(coll, 0);
        ASSERT_NE(nullptr, elem);

        const auto file = OCI_ElemGetFile(elem);
        ASSERT_NE(nullptr, file);
        ASSERT_EQ(String(OTEXT("FAKE_DIR")), String(OCI_FileGetDirectory(file)));
        ASSERT_EQ(String(OTEXT("a.txt")), String(OCI_FileGetName(file)));

        typinf = OCI_CollGetTypeInfo(coll);
        ASSERT_NE(nullptr, typinf);

        local = OCI_ElemCreate(typinf);
        ASSERT_NE(nullptr, local);

        const auto file2 = OCI_FileCreate(conn, OCI_BFILE);
        ASSERT_NE(nullptr, file2);
        ASSERT_TRUE(OCI_FileSetName(file2, OTEXT("FAKE_DIR"), OTEXT("b.txt")));
        ASSERT_TRUE(OCI_ElemSetFile(local, file2));

        const auto file3 = OCI_ElemGetFile(local);
        ASSERT_NE(nullptr, file3);
        ASSERT_EQ(String(OTEXT("b.txt")), String(OCI_FileGetName(file3)));

        ASSERT_TRUE(OCI_ElemSetFile(local, nullptr));
        ASSERT_TRUE(OCI_ElemIsNull(local));

        ASSERT_TRUE(OCI_FileFree(file2));
        ASSERT_TRUE(OCI_ElemFree(local));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TYPE TestElement_FileColl"));
        ExecDML(OTEXT("DROP TYPE TestElement_LobColl"));
    }

    TEST(TestElement, Reference)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_RefT AS OBJECT(code int)"));
        ExecDML(OTEXT("CREATE TABLE TestElement_RefTable OF TestElement_RefT"));
        ExecDML(OTEXT("INSERT INTO TestElement_RefTable VALUES (1)"));
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_RefColl AS VARRAY(1) OF REF TestElement_RefT"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

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
        ASSERT_FALSE(OCI_RefIsNull(ref));

        const auto objType = OCI_TypeInfoGet(conn, OTEXT("TestElement_RefT"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, objType);

        const auto local = OCI_ElemCreate(OCI_CollGetTypeInfo(coll));
        ASSERT_NE(nullptr, local);

        const auto ref2 = OCI_RefCreate(objType);
        ASSERT_NE(nullptr, ref2);
        ASSERT_TRUE(OCI_RefAssign(ref2, ref));
        ASSERT_TRUE(OCI_ElemSetRef(local, ref2));

        const auto ref3 = OCI_ElemGetRef(local);
        ASSERT_NE(nullptr, ref3);
        ASSERT_FALSE(OCI_RefIsNull(ref3));

        ASSERT_TRUE(OCI_ElemSetRef(local, nullptr));
        ASSERT_TRUE(OCI_ElemIsNull(local));

        ASSERT_TRUE(OCI_RefFree(ref2));
        ASSERT_TRUE(OCI_ElemFree(local));
        ASSERT_TRUE(OCI_TypeInfoFree(objType));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TYPE TestElement_RefColl"));
        ExecDML(OTEXT("DROP TABLE TestElement_RefTable"));
        ExecDML(OTEXT("DROP TYPE TestElement_RefT"));
    }

    TEST(TestElement, NullSetters)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NullObjT AS OBJECT(code int, name varchar2(20))"));
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NullObjColl AS VARRAY(1) OF TestElement_NullObjT"));
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NullNumColl AS VARRAY(1) OF NUMBER"));
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NullStrColl AS VARRAY(1) OF VARCHAR2(20)"));
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NullDateColl AS VARRAY(1) OF DATE"));
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NullTsColl AS VARRAY(1) OF TIMESTAMP"));
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NullItvColl AS VARRAY(1) OF INTERVAL DAY TO SECOND"));
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NullRawColl AS VARRAY(1) OF RAW(10)"));
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestElement_NullCollColl AS VARRAY(1) OF TestElement_NullNumColl"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        // Object null setter
        {
            const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestElement_NullObjColl"), OCI_TIF_TYPE);
            const auto local = OCI_ElemCreate(typinf);
            ASSERT_TRUE(OCI_ElemSetObject(local, nullptr));
            ASSERT_TRUE(OCI_ElemIsNull(local));
            ASSERT_TRUE(OCI_ElemFree(local));
            ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        }

        // Collection null setter
        {
            const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestElement_NullCollColl"), OCI_TIF_TYPE);
            const auto local = OCI_ElemCreate(typinf);
            ASSERT_TRUE(OCI_ElemSetColl(local, nullptr));
            ASSERT_TRUE(OCI_ElemIsNull(local));
            ASSERT_TRUE(OCI_ElemFree(local));
            ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        }

        // String, raw, number null setters
        {
            const auto strTyp = OCI_TypeInfoGet(conn, OTEXT("TestElement_NullStrColl"), OCI_TIF_TYPE);
            auto local = OCI_ElemCreate(strTyp);
            ASSERT_TRUE(OCI_ElemSetString(local, nullptr));
            ASSERT_TRUE(OCI_ElemIsNull(local));
            ASSERT_TRUE(OCI_ElemFree(local));

            const auto rawTyp = OCI_TypeInfoGet(conn, OTEXT("TestElement_NullRawColl"), OCI_TIF_TYPE);
            local = OCI_ElemCreate(rawTyp);
            ASSERT_TRUE(OCI_ElemSetRaw(local, nullptr, 0));
            ASSERT_TRUE(OCI_ElemIsNull(local));
            ASSERT_TRUE(OCI_ElemFree(local));

            const auto numTyp = OCI_TypeInfoGet(conn, OTEXT("TestElement_NullNumColl"), OCI_TIF_TYPE);
            local = OCI_ElemCreate(numTyp);
            ASSERT_TRUE(OCI_ElemSetNumber(local, nullptr));
            ASSERT_TRUE(OCI_ElemIsNull(local));
            ASSERT_TRUE(OCI_ElemFree(local));

            ASSERT_TRUE(OCI_TypeInfoFree(strTyp));
            ASSERT_TRUE(OCI_TypeInfoFree(rawTyp));
            ASSERT_TRUE(OCI_TypeInfoFree(numTyp));
        }

        // Date, timestamp, interval null setters
        {
            const auto dateTyp = OCI_TypeInfoGet(conn, OTEXT("TestElement_NullDateColl"), OCI_TIF_TYPE);
            auto local = OCI_ElemCreate(dateTyp);
            ASSERT_TRUE(OCI_ElemSetDate(local, nullptr));
            ASSERT_TRUE(OCI_ElemIsNull(local));
            ASSERT_TRUE(OCI_ElemFree(local));

            const auto tsTyp = OCI_TypeInfoGet(conn, OTEXT("TestElement_NullTsColl"), OCI_TIF_TYPE);
            local = OCI_ElemCreate(tsTyp);
            ASSERT_TRUE(OCI_ElemSetTimestamp(local, nullptr));
            ASSERT_TRUE(OCI_ElemIsNull(local));
            ASSERT_TRUE(OCI_ElemFree(local));

            const auto itvTyp = OCI_TypeInfoGet(conn, OTEXT("TestElement_NullItvColl"), OCI_TIF_TYPE);
            local = OCI_ElemCreate(itvTyp);
            ASSERT_TRUE(OCI_ElemSetInterval(local, nullptr));
            ASSERT_TRUE(OCI_ElemIsNull(local));
            ASSERT_TRUE(OCI_ElemFree(local));

            ASSERT_TRUE(OCI_TypeInfoFree(dateTyp));
            ASSERT_TRUE(OCI_TypeInfoFree(tsTyp));
            ASSERT_TRUE(OCI_TypeInfoFree(itvTyp));
        }

        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("DROP TYPE TestElement_NullCollColl"));
        ExecDML(OTEXT("DROP TYPE TestElement_NullRawColl"));
        ExecDML(OTEXT("DROP TYPE TestElement_NullItvColl"));
        ExecDML(OTEXT("DROP TYPE TestElement_NullTsColl"));
        ExecDML(OTEXT("DROP TYPE TestElement_NullDateColl"));
        ExecDML(OTEXT("DROP TYPE TestElement_NullStrColl"));
        ExecDML(OTEXT("DROP TYPE TestElement_NullNumColl"));
        ExecDML(OTEXT("DROP TYPE TestElement_NullObjColl"));
        ExecDML(OTEXT("DROP TYPE TestElement_NullObjT"));
    }

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

    TEST_F(TestElementBool, GetBooleanFromFetchedElement)
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

    TEST_F(TestElementBool, SetBooleanOnLocalElement)
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
}
