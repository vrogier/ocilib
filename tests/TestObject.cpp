#include "ocilib_tests.h"

using namespace ocilib;

namespace TestCApi
{
    TEST(TestObject, SetGetBasicProps)
    {
        ExecDML(OTEXT("create type TestObjectSetGetBasicPropsVendor as object(code number, name varchar2(30))"));
        ExecDML(OTEXT("create type TestObjectSetGetBasicPropsSale as object(code number, price float, name varchar2(30), ref varchar2(30), date_sale date, vendor TestObjectSetGetBasicPropsVendor)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto date = OCI_DateCreate(env);
        ASSERT_NE(nullptr, date);

        ASSERT_TRUE(OCI_DateSysDate(date));

        const auto vendor = OCI_ObjectCreate(OCI_TypeInfoGet(conn, OTEXT("TestObjectSetGetBasicPropsVendor"), OCI_TIF_TYPE));
        ASSERT_NE(nullptr, vendor);
        ASSERT_TRUE(OCI_ObjectSetInt(vendor, OTEXT("CODE"), 134));
        ASSERT_TRUE(OCI_ObjectSetString(vendor, OTEXT("NAME"), OTEXT("JOHN SMITH")));

        const auto sale = OCI_ObjectCreate(OCI_TypeInfoGet(conn, OTEXT("TestObjectSetGetBasicPropsSale"), OCI_TIF_TYPE));
        ASSERT_NE(nullptr, vendor);

        ASSERT_TRUE(OCI_ObjectSetInt(sale, OTEXT("CODE"), 1));
        ASSERT_TRUE(OCI_ObjectSetDouble(sale, OTEXT("PRICE"), 12.99));
        ASSERT_TRUE(OCI_ObjectSetString(sale, OTEXT("NAME"), OTEXT("USB KEY 2go")));
        ASSERT_TRUE(OCI_ObjectSetString(sale, OTEXT("REF"), OTEXT("A56547WSAA")));
        ASSERT_TRUE(OCI_ObjectSetObject(sale, OTEXT("VENDOR"), vendor));
        ASSERT_TRUE(OCI_ObjectSetDate(sale, OTEXT("DATE_SALE"), date));

        ASSERT_EQ(1, OCI_ObjectGetInt(sale, OTEXT("CODE")));
        ASSERT_EQ(12.99, OCI_ObjectGetDouble(sale, OTEXT("PRICE")));
        ASSERT_EQ(String(OTEXT("USB KEY 2go")), String(OCI_ObjectGetString(sale, OTEXT("NAME"))));
        ASSERT_EQ(String(OTEXT("A56547WSAA")), String(OCI_ObjectGetString(sale, OTEXT("REF"))));

        const auto vendor2 = OCI_ObjectGetObject(sale, OTEXT("VENDOR"));
        ASSERT_NE(nullptr, vendor2);

        ASSERT_EQ(134, OCI_ObjectGetInt(vendor2, OTEXT("CODE")));
        ASSERT_EQ(String(OTEXT("JOHN SMITH")), String(OCI_ObjectGetString(vendor2, OTEXT("NAME"))));

        const auto date2 = OCI_ObjectGetDate(sale, OTEXT("DATE_SALE"));
        ASSERT_NE(nullptr, date2);
        ASSERT_EQ(0, OCI_DateCompare(date, date2));

        ASSERT_TRUE(OCI_ObjectFree(sale));
        ASSERT_TRUE(OCI_ObjectFree(vendor));
        ASSERT_TRUE(OCI_DateFree(date));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

       ExecDML(OTEXT("drop type TestObjectSetGetBasicPropsSale"));
       ExecDML(OTEXT("drop type TestObjectSetGetBasicPropsVendor"));
    }

    TEST(TestObject, AllAttributeGettersAndSetters)
    {
        ExecDML(OTEXT("create or replace type TestObject_TgtType as object(code int, name varchar2(30))"));
        ExecDML(OTEXT("create or replace type TestObject_NumVarray as varray(3) of number"));
        ExecDML(OTEXT("create or replace type TestObject_FullType as object(n number, i int, s varchar2(30), r raw(20), d date, ts timestamp, iv interval day to second, lob clob, coll TestObject_NumVarray, ref_val ref TestObject_TgtType, f bfile, nested TestObject_TgtType)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto fullType = OCI_TypeInfoGet(conn, OTEXT("TestObject_FullType"), OCI_TIF_TYPE);
        const auto obj = OCI_ObjectCreate(fullType);
        ASSERT_NE(nullptr, obj);
        ASSERT_NE((OCI_UInt32) OCI_UNKNOWN, OCI_ObjectGetType(obj));

        const auto tgtType = OCI_TypeInfoGet(conn, OTEXT("TestObject_TgtType"), OCI_TIF_TYPE);
        const auto nested = OCI_ObjectCreate(tgtType);
        ASSERT_NE(nullptr, nested);
        ASSERT_TRUE(OCI_ObjectSetInt(nested, OTEXT("CODE"), 7));
        ASSERT_TRUE(OCI_ObjectSetString(nested, OTEXT("NAME"), OTEXT("nested")));

        const auto number = OCI_NumberCreate(env);
        OCI_Int32 num = 42;
        ASSERT_TRUE(OCI_NumberSetValue(number, OCI_NUM_INT, &num));
        ASSERT_TRUE(OCI_ObjectSetNumber(obj, OTEXT("N"), number));
        const auto num2 = OCI_ObjectGetNumber(obj, OTEXT("N"));
        ASSERT_NE(nullptr, num2);
        OCI_Int32 numOut = 0;
        ASSERT_TRUE(OCI_NumberGetValue(num2, OCI_NUM_INT, &numOut));
        ASSERT_EQ(42, numOut);

        ASSERT_TRUE(OCI_ObjectSetInt(obj, OTEXT("I"), 99));
        ASSERT_EQ(99, OCI_ObjectGetInt(obj, OTEXT("I")));

        ASSERT_TRUE(OCI_ObjectSetString(obj, OTEXT("S"), OTEXT("full")));
        ASSERT_EQ(String(OTEXT("full")), String(OCI_ObjectGetString(obj, OTEXT("S"))));

        unsigned char raw[] = { 0x00, 0x01, 0x02, 0x03, 0x04 };
        ASSERT_TRUE(OCI_ObjectSetRaw(obj, OTEXT("R"), raw, sizeof(raw)));
        ASSERT_EQ(sizeof(raw), OCI_ObjectGetRawSize(obj, OTEXT("R")));
        unsigned char rawOut[20] = {};
        ASSERT_EQ((OCI_Int32)sizeof(raw), OCI_ObjectGetRaw(obj, OTEXT("R"), rawOut, sizeof(rawOut)));
        ASSERT_EQ(0, memcmp(raw, rawOut, sizeof(raw)));

        const auto date = OCI_DateCreate(env);
        ASSERT_NE(nullptr, date);
        ASSERT_TRUE(OCI_DateSetDateTime(date, 2023, 6, 15, 12, 30, 0));
        ASSERT_TRUE(OCI_ObjectSetDate(obj, OTEXT("D"), date));
        const auto date2 = OCI_ObjectGetDate(obj, OTEXT("D"));
        ASSERT_NE(nullptr, date2);
        ASSERT_EQ(0, OCI_DateCompare(date, date2));

        const auto ts = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        ASSERT_NE(nullptr, ts);
        ASSERT_TRUE(OCI_TimestampConstruct(ts, 2023, 6, 15, 12, 30, 0, 0, nullptr));
        ASSERT_TRUE(OCI_ObjectSetTimestamp(obj, OTEXT("TS"), ts));
        const auto ts2 = OCI_ObjectGetTimestamp(obj, OTEXT("TS"));
        ASSERT_NE(nullptr, ts2);

        const auto itv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        ASSERT_NE(nullptr, itv);
        ASSERT_TRUE(OCI_IntervalSetDaySecond(itv, 1, 2, 3, 4, 0));
        ASSERT_TRUE(OCI_ObjectSetInterval(obj, OTEXT("IV"), itv));
        const auto itv2 = OCI_ObjectGetInterval(obj, OTEXT("IV"));
        ASSERT_NE(nullptr, itv2);

        const auto lob = OCI_LobCreate(conn, OCI_CLOB);
        ASSERT_NE(nullptr, lob);
        OCI_UInt32 lob_cc = (OCI_UInt32)ostrlen(OTEXT("object clob"));
        OCI_UInt32 lob_bc = 0;
        ASSERT_TRUE(OCI_LobWrite(lob, (void*)OTEXT("object clob"), &lob_cc, &lob_bc));
        ASSERT_EQ(lob_cc, (OCI_UInt32)ostrlen(OTEXT("object clob")));
        ASSERT_TRUE(OCI_ObjectSetLob(obj, OTEXT("LOB"), lob));
        const auto lob2 = OCI_ObjectGetLob(obj, OTEXT("LOB"));
        ASSERT_NE(nullptr, lob2);
        ASSERT_GT(OCI_LobGetLength(lob2), (OCI_UInt64)0);

        const auto collType = OCI_TypeInfoGet(conn, OTEXT("TestObject_NumVarray"), OCI_TIF_TYPE);
        const auto coll = OCI_CollCreate(collType);
        ASSERT_NE(nullptr, coll);
        const auto elem = OCI_ElemCreate(collType);
        ASSERT_NE(nullptr, elem);
        ASSERT_TRUE(OCI_ElemSetInt(elem, 1));
        ASSERT_TRUE(OCI_CollAppend(coll, elem));
        ASSERT_TRUE(OCI_ElemFree(elem));
        ASSERT_TRUE(OCI_ObjectSetColl(obj, OTEXT("COLL"), coll));
        const auto coll2 = OCI_ObjectGetColl(obj, OTEXT("COLL"));
        ASSERT_NE(nullptr, coll2);
        ASSERT_EQ(1u, OCI_CollGetCount(coll2));

        ASSERT_TRUE(OCI_ObjectSetRef(obj, OTEXT("REF_VAL"), nullptr));
        ASSERT_TRUE(OCI_ObjectIsNull(obj, OTEXT("REF_VAL")));
        ASSERT_EQ(nullptr, OCI_ObjectGetRef(obj, OTEXT("REF_VAL")));

        ASSERT_TRUE(OCI_ObjectSetFile(obj, OTEXT("F"), nullptr));
        ASSERT_TRUE(OCI_ObjectIsNull(obj, OTEXT("F")));
        ASSERT_EQ(nullptr, OCI_ObjectGetFile(obj, OTEXT("F")));

        ASSERT_TRUE(OCI_ObjectSetObject(obj, OTEXT("NESTED"), nested));
        const auto nested2 = OCI_ObjectGetObject(obj, OTEXT("NESTED"));
        ASSERT_NE(nullptr, nested2);
        ASSERT_EQ(7, OCI_ObjectGetInt(nested2, OTEXT("CODE")));
        ASSERT_EQ(String(OTEXT("nested")), String(OCI_ObjectGetString(nested2, OTEXT("NAME"))));

        OCI_UInt32 size = 0;
        ASSERT_TRUE(OCI_ObjectToText(obj, &size, nullptr));
        static std::vector<otext> text(size + 1);
        ASSERT_TRUE(OCI_ObjectToText(obj, &size, text.data()));
        ASSERT_GT(size, 0u);

        const auto tinf = OCI_ObjectGetTypeInfo(obj);
        ASSERT_EQ(fullType, tinf);

        void* p_struct = nullptr;
        void* p_ind = nullptr;
        ASSERT_TRUE(OCI_ObjectGetStruct(obj, &p_struct, &p_ind));

        const auto arr = OCI_ObjectArrayCreate(fullType, 2);
        ASSERT_NE(nullptr, arr);
        ASSERT_TRUE(OCI_ObjectSetInt(arr[0], OTEXT("I"), 1));
        ASSERT_TRUE(OCI_ObjectArrayFree(arr));

        const auto assigned = OCI_ObjectCreate(fullType);
        ASSERT_NE(nullptr, assigned);
        ASSERT_TRUE(OCI_ObjectAssign(assigned, obj));
        ASSERT_EQ(99, OCI_ObjectGetInt(assigned, OTEXT("I")));

        ASSERT_TRUE(OCI_ObjectFree(assigned));
        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_ObjectFree(nested));
        ASSERT_TRUE(OCI_NumberFree(number));
        ASSERT_TRUE(OCI_DateFree(date));
        ASSERT_TRUE(OCI_TimestampFree(ts));
        ASSERT_TRUE(OCI_IntervalFree(itv));
        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_CollFree(coll));
        ASSERT_TRUE(OCI_TypeInfoFree(fullType));
        ASSERT_TRUE(OCI_TypeInfoFree(tgtType));
        ASSERT_TRUE(OCI_TypeInfoFree(collType));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop type TestObject_FullType"));
        ExecDML(OTEXT("drop type TestObject_NumVarray"));
        ExecDML(OTEXT("drop type TestObject_TgtType"));
    }

    TEST(TestObject, NumericVariations)
    {
        ExecDML(OTEXT("create or replace type TestObject_NumericT as object(n number, s varchar2(20))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestObject_NumericT"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto obj = OCI_ObjectCreate(typinf);
        ASSERT_NE(nullptr, obj);

        ASSERT_TRUE(OCI_ObjectSetShort(obj, OTEXT("N"), 11));
        ASSERT_EQ(11, OCI_ObjectGetShort(obj, OTEXT("N")));

        ASSERT_TRUE(OCI_ObjectSetUnsignedShort(obj, OTEXT("N"), 12u));
        ASSERT_EQ(12u, OCI_ObjectGetUnsignedShort(obj, OTEXT("N")));

        ASSERT_TRUE(OCI_ObjectSetInt(obj, OTEXT("N"), 13));
        ASSERT_EQ(13, OCI_ObjectGetInt(obj, OTEXT("N")));

        ASSERT_TRUE(OCI_ObjectSetUnsignedInt(obj, OTEXT("N"), 14u));
        ASSERT_EQ(14u, OCI_ObjectGetUnsignedInt(obj, OTEXT("N")));

        ASSERT_TRUE(OCI_ObjectSetBigInt(obj, OTEXT("N"), static_cast<OCI_Int64>(15)));
        ASSERT_EQ(15, OCI_ObjectGetBigInt(obj, OTEXT("N")));

        ASSERT_TRUE(OCI_ObjectSetUnsignedBigInt(obj, OTEXT("N"), static_cast<OCI_UInt64>(16)));
        ASSERT_EQ(16u, OCI_ObjectGetUnsignedBigInt(obj, OTEXT("N")));

        ASSERT_TRUE(OCI_ObjectSetFloat(obj, OTEXT("N"), 1.5f));
        ASSERT_NEAR(1.5f, OCI_ObjectGetFloat(obj, OTEXT("N")), 0.01f);

        ASSERT_TRUE(OCI_ObjectSetDouble(obj, OTEXT("N"), 2.5));
        ASSERT_NEAR(2.5, OCI_ObjectGetDouble(obj, OTEXT("N")), 0.01);

        ASSERT_TRUE(OCI_ObjectSetString(obj, OTEXT("S"), OTEXT("1234")));
        ASSERT_EQ(1234, OCI_ObjectGetInt(obj, OTEXT("S")));
        ASSERT_NEAR(1234.0, OCI_ObjectGetDouble(obj, OTEXT("S")), 0.01);

        ASSERT_TRUE(OCI_ObjectSetNumber(obj, OTEXT("N"), nullptr));
        ASSERT_TRUE(OCI_ObjectIsNull(obj, OTEXT("N")));
        ASSERT_EQ(nullptr, OCI_ObjectGetNumber(obj, OTEXT("N")));
        ASSERT_EQ(0, OCI_ObjectGetInt(obj, OTEXT("N")));

        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop type TestObject_NumericT"));
    }

    TEST(TestObject, SelfRef)
    {
        ExecDML(OTEXT("create or replace type TestObject_SelfRefT as object(code int)"));
        ExecDML(OTEXT("create table TestObject_SelfRefTable of TestObject_SelfRefT"));
        ExecDML(OTEXT("insert into TestObject_SelfRefTable values (1)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT REF(t) FROM TestObject_SelfRefTable t")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto ref = OCI_GetRef(rs, 1);
        ASSERT_NE(nullptr, ref);

        const auto obj = OCI_RefGetObject(ref);
        ASSERT_NE(nullptr, obj);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestObject_SelfRefT"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto selfRef = OCI_RefCreate(typinf);
        ASSERT_NE(nullptr, selfRef);

        ASSERT_TRUE(OCI_ObjectGetSelfRef(obj, selfRef));
        ASSERT_FALSE(OCI_RefIsNull(selfRef));

        ASSERT_TRUE(OCI_RefFree(selfRef));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestObject_SelfRefTable"));
        ExecDML(OTEXT("drop type TestObject_SelfRefT"));
    }

    TEST(TestObject, InheritedObject)
    {
        ExecDML(OTEXT("create or replace type TestObject_BaseNF as object(code int) not final"));
        ExecDML(OTEXT("create or replace type TestObject_SubNF under TestObject_BaseNF(name varchar2(20))"));
        ExecDML(OTEXT("create table TestObject_SubNFTable of TestObject_BaseNF"));
        ExecDML(OTEXT("insert into TestObject_SubNFTable values (TestObject_SubNF(1, 'subvalue'))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT VALUE(t) FROM TestObject_SubNFTable t")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto obj = OCI_GetObject(rs, 1);
        ASSERT_NE(nullptr, obj);

        const auto typinf = OCI_ObjectGetTypeInfo(obj);
        ASSERT_NE(nullptr, typinf);

        ASSERT_EQ(String(OTEXT("TESTOBJECT_SUBNF")), String(OCI_TypeInfoGetName(typinf)));

        ASSERT_EQ(1, OCI_ObjectGetInt(obj, OTEXT("CODE")));
        ASSERT_EQ(String(OTEXT("subvalue")), String(OCI_ObjectGetString(obj, OTEXT("NAME"))));

        OCI_UInt32 size = 0;
        ASSERT_TRUE(OCI_ObjectToText(obj, &size, nullptr));
        static std::vector<otext> text(size + 1);
        ASSERT_TRUE(OCI_ObjectToText(obj, &size, text.data()));
        ASSERT_NE(String::npos, String(text.data()).find(OTEXT("TESTOBJECT_SUBNF")));

        unsigned char buffer[256] = {};
        OCI_Int32 inds[2] = {};
        ASSERT_TRUE(OCI_GetStruct(rs, buffer, inds));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestObject_SubNFTable"));
        ExecDML(OTEXT("drop type TestObject_SubNF"));
        ExecDML(OTEXT("drop type TestObject_BaseNF"));
    }

    TEST(TestObject, NullAttributeSetters)
    {
        ExecDML(OTEXT("create or replace type TestObject_NullTgtT as object(code int, name varchar2(20))"));
        ExecDML(OTEXT("create or replace type TestObject_NullVarray as varray(1) of number"));
        ExecDML(OTEXT("create or replace type TestObject_NullFullT as object(n number, i int, s varchar2(20), r raw(10), d date, ts timestamp, iv interval day to second, lob clob, coll TestObject_NullVarray, ref_val ref TestObject_NullTgtT, f bfile, nested TestObject_NullTgtT)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto tgtType = OCI_TypeInfoGet(conn, OTEXT("TestObject_NullTgtT"), OCI_TIF_TYPE);
        const auto fullType = OCI_TypeInfoGet(conn, OTEXT("TestObject_NullFullT"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, fullType);

        const auto obj = OCI_ObjectCreate(fullType);
        ASSERT_NE(nullptr, obj);

        ASSERT_TRUE(OCI_ObjectSetNull(obj, OTEXT("N")));
        ASSERT_TRUE(OCI_ObjectIsNull(obj, OTEXT("N")));
        ASSERT_EQ(0, OCI_ObjectGetInt(obj, OTEXT("N")));
        ASSERT_EQ(nullptr, OCI_ObjectGetNumber(obj, OTEXT("N")));

        ASSERT_TRUE(OCI_ObjectSetNull(obj, OTEXT("I")));
        ASSERT_TRUE(OCI_ObjectIsNull(obj, OTEXT("I")));
        ASSERT_EQ(0, OCI_ObjectGetInt(obj, OTEXT("I")));

        ASSERT_TRUE(OCI_ObjectSetString(obj, OTEXT("S"), nullptr));
        ASSERT_TRUE(OCI_ObjectIsNull(obj, OTEXT("S")));
        ASSERT_EQ(nullptr, OCI_ObjectGetString(obj, OTEXT("S")));

        ASSERT_TRUE(OCI_ObjectSetRaw(obj, OTEXT("R"), nullptr, 0));
        ASSERT_TRUE(OCI_ObjectIsNull(obj, OTEXT("R")));
        ASSERT_EQ(0, OCI_ObjectGetRaw(obj, OTEXT("R"), nullptr, 0));
        ASSERT_EQ(0u, OCI_ObjectGetRawSize(obj, OTEXT("R")));

        ASSERT_TRUE(OCI_ObjectSetDate(obj, OTEXT("D"), nullptr));
        ASSERT_TRUE(OCI_ObjectIsNull(obj, OTEXT("D")));
        ASSERT_EQ(nullptr, OCI_ObjectGetDate(obj, OTEXT("D")));

        ASSERT_TRUE(OCI_ObjectSetTimestamp(obj, OTEXT("TS"), nullptr));
        ASSERT_TRUE(OCI_ObjectIsNull(obj, OTEXT("TS")));
        ASSERT_EQ(nullptr, OCI_ObjectGetTimestamp(obj, OTEXT("TS")));

        ASSERT_TRUE(OCI_ObjectSetInterval(obj, OTEXT("IV"), nullptr));
        ASSERT_TRUE(OCI_ObjectIsNull(obj, OTEXT("IV")));
        ASSERT_EQ(nullptr, OCI_ObjectGetInterval(obj, OTEXT("IV")));

        ASSERT_TRUE(OCI_ObjectSetLob(obj, OTEXT("LOB"), nullptr));
        ASSERT_TRUE(OCI_ObjectIsNull(obj, OTEXT("LOB")));
        ASSERT_EQ(nullptr, OCI_ObjectGetLob(obj, OTEXT("LOB")));

        ASSERT_TRUE(OCI_ObjectSetColl(obj, OTEXT("COLL"), nullptr));
        ASSERT_TRUE(OCI_ObjectIsNull(obj, OTEXT("COLL")));
        ASSERT_EQ(nullptr, OCI_ObjectGetColl(obj, OTEXT("COLL")));

        ASSERT_TRUE(OCI_ObjectSetRef(obj, OTEXT("REF_VAL"), nullptr));
        ASSERT_TRUE(OCI_ObjectIsNull(obj, OTEXT("REF_VAL")));
        ASSERT_EQ(nullptr, OCI_ObjectGetRef(obj, OTEXT("REF_VAL")));

        ASSERT_TRUE(OCI_ObjectSetFile(obj, OTEXT("F"), nullptr));
        ASSERT_TRUE(OCI_ObjectIsNull(obj, OTEXT("F")));
        ASSERT_EQ(nullptr, OCI_ObjectGetFile(obj, OTEXT("F")));

        ASSERT_TRUE(OCI_ObjectSetObject(obj, OTEXT("NESTED"), nullptr));
        ASSERT_TRUE(OCI_ObjectIsNull(obj, OTEXT("NESTED")));
        ASSERT_EQ(nullptr, OCI_ObjectGetObject(obj, OTEXT("NESTED")));

        OCI_UInt32 size = 0;
        ASSERT_TRUE(OCI_ObjectToText(obj, &size, nullptr));
        static std::vector<otext> text(size + 1);
        ASSERT_TRUE(OCI_ObjectToText(obj, &size, text.data()));
        ASSERT_GT(size, 0u);

        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_TypeInfoFree(fullType));
        ASSERT_TRUE(OCI_TypeInfoFree(tgtType));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop type TestObject_NullFullT"));
        ExecDML(OTEXT("drop type TestObject_NullVarray"));
        ExecDML(OTEXT("drop type TestObject_NullTgtT"));
    }

    TEST(TestObject, FileAndRefAttributes)
    {
        ExecDML(OTEXT("create or replace type TestObject_RefTgtT as object(code int)"));
        ExecDML(OTEXT("create table TestObject_RefTgtTable of TestObject_RefTgtT"));
        ExecDML(OTEXT("insert into TestObject_RefTgtTable values (1)"));
        ExecDML(OTEXT("create or replace type TestObject_FileRefT as object(code int, f bfile, r ref TestObject_RefTgtT)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT REF(t) FROM TestObject_RefTgtTable t")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto ref = OCI_GetRef(rs, 1);
        ASSERT_NE(nullptr, ref);
        ASSERT_FALSE(OCI_RefIsNull(ref));

        const auto fileType = OCI_TypeInfoGet(conn, OTEXT("TestObject_FileRefT"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, fileType);

        const auto obj = OCI_ObjectCreate(fileType);
        ASSERT_NE(nullptr, obj);

        ASSERT_TRUE(OCI_ObjectSetInt(obj, OTEXT("CODE"), 7));

        const auto file = OCI_FileCreate(conn, OCI_BFILE);
        ASSERT_NE(nullptr, file);
        ASSERT_TRUE(OCI_FileSetName(file, OTEXT("FAKE_DIR"), OTEXT("file.txt")));
        ASSERT_TRUE(OCI_ObjectSetFile(obj, OTEXT("F"), file));

        const auto file2 = OCI_ObjectGetFile(obj, OTEXT("F"));
        ASSERT_NE(nullptr, file2);
        ASSERT_EQ(String(OTEXT("FAKE_DIR")), String(OCI_FileGetDirectory(file2)));
        ASSERT_EQ(String(OTEXT("file.txt")), String(OCI_FileGetName(file2)));

        ASSERT_TRUE(OCI_ObjectSetRef(obj, OTEXT("R"), ref));
        const auto ref2 = OCI_ObjectGetRef(obj, OTEXT("R"));
        ASSERT_NE(nullptr, ref2);
        ASSERT_FALSE(OCI_RefIsNull(ref2));

        OCI_UInt32 size = 0;
        ASSERT_TRUE(OCI_ObjectToText(obj, &size, nullptr));
        static std::vector<otext> text(size + 1);
        ASSERT_TRUE(OCI_ObjectToText(obj, &size, text.data()));
        ASSERT_GT(size, 0u);

        ASSERT_TRUE(OCI_FileFree(file));
        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_TypeInfoFree(fileType));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestObject_RefTgtTable"));
        ExecDML(OTEXT("drop type TestObject_FileRefT"));
        ExecDML(OTEXT("drop type TestObject_RefTgtT"));
    }

    TEST(TestObject, GetStringOnNonTextAttributes)
    {
        ExecDML(OTEXT("create or replace type TestObject_StrT as object(n number, r raw(10), s varchar2(20))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestObject_StrT"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto obj = OCI_ObjectCreate(typinf);
        ASSERT_NE(nullptr, obj);

        ASSERT_TRUE(OCI_ObjectSetDouble(obj, OTEXT("N"), 3.14));
        const otext* numStr = OCI_ObjectGetString(obj, OTEXT("N"));
        ASSERT_NE(nullptr, numStr);
        ASSERT_NE(String::npos, String(numStr).find(OTEXT("3.14")));

        unsigned char raw[] = { 0x01, 0x02, 0x03 };
        ASSERT_TRUE(OCI_ObjectSetRaw(obj, OTEXT("R"), raw, sizeof(raw)));
        const otext* rawStr = OCI_ObjectGetString(obj, OTEXT("R"));
        ASSERT_NE(nullptr, rawStr);
        ASSERT_GT(ostrlen(rawStr), (OCI_Size) 0);

        ASSERT_TRUE(OCI_ObjectSetString(obj, OTEXT("S"), OTEXT("text")));
        ASSERT_EQ(String(OTEXT("text")), String(OCI_ObjectGetString(obj, OTEXT("S"))));

        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop type TestObject_StrT"));
    }

    TEST(TestObject, NumericSubtypeAttributes)
    {
        ExecDML(OTEXT("create or replace type TestObject_SubNumT as object(i int, n number)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestObject_SubNumT"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto obj = OCI_ObjectCreate(typinf);
        ASSERT_NE(nullptr, obj);

        ASSERT_TRUE(OCI_ObjectSetInt(obj, OTEXT("I"), 1));
        ASSERT_TRUE(OCI_ObjectSetDouble(obj, OTEXT("N"), 2.5));

        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop type TestObject_SubNumT"));
    }

    TEST(TestObject, GetStructFromObjectTable)
    {
        ExecDML(OTEXT("create or replace type TestObject_StructT as object(code int, name varchar2(20))"));
        ExecDML(OTEXT("create table TestObject_StructTable of TestObject_StructT"));
        ExecDML(OTEXT("insert into TestObject_StructTable values (1, 'one')"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT VALUE(t) FROM TestObject_StructTable t")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        unsigned char buffer[256] = {};
        OCI_Int32 inds[2] = {};
        ASSERT_TRUE(OCI_GetStruct(rs, buffer, inds));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestObject_StructTable"));
        ExecDML(OTEXT("drop type TestObject_StructT"));
    }

    TEST(TestObject, IndicatorOffsetForNestedObject)
    {
        ExecDML(OTEXT("create or replace type TestObject_IndTgtT as object(code int, label varchar2(20))"));
        ExecDML(OTEXT("create or replace type TestObject_IndT as object(code int, nested TestObject_IndTgtT, flag int)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestObject_IndT"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto obj = OCI_ObjectCreate(typinf);
        ASSERT_NE(nullptr, obj);

        ASSERT_TRUE(OCI_ObjectIsNull(obj, OTEXT("FLAG")));
        ASSERT_TRUE(OCI_ObjectSetNull(obj, OTEXT("FLAG")));
        ASSERT_TRUE(OCI_ObjectIsNull(obj, OTEXT("FLAG")));

        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop type TestObject_IndT"));
        ExecDML(OTEXT("drop type TestObject_IndTgtT"));
    }

    TEST(TestObject, BinaryFloatDoubleObject)
    {
        ExecDML(OTEXT("create or replace type TestObject_BinNumT as object(bf binary_float, bd binary_double)"));
        ExecDML(OTEXT("create table TestObject_BinNumTable of TestObject_BinNumT"));
        ExecDML(OTEXT("insert into TestObject_BinNumTable values (1.5f, 2.5)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestObject_BinNumT"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto obj = OCI_ObjectCreate(typinf);
        ASSERT_NE(nullptr, obj);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT VALUE(t) FROM TestObject_BinNumTable t")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        unsigned char buffer[256] = {};
        OCI_Int32 inds[2] = {};
        ASSERT_TRUE(OCI_GetStruct(rs, buffer, inds));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestObject_BinNumTable"));
        ExecDML(OTEXT("drop type TestObject_BinNumT"));
    }

    TEST(TestObject, BoolAttribute)
    {
        ExecDML(OTEXT("create or replace type TestObject_BoolT as object(b boolean)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestObject_BoolT"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto obj = OCI_ObjectCreate(typinf);
        ASSERT_NE(nullptr, obj);

        ASSERT_TRUE(OCI_ObjectSetBoolean(obj, OTEXT("B"), TRUE));
        ASSERT_EQ(TRUE, OCI_ObjectGetBoolean(obj, OTEXT("B")));

        ASSERT_TRUE(OCI_ObjectSetBoolean(obj, OTEXT("B"), FALSE));
        ASSERT_EQ(FALSE, OCI_ObjectGetBoolean(obj, OTEXT("B")));

        OCI_UInt32 size = 0;
        ASSERT_TRUE(OCI_ObjectToText(obj, &size, nullptr));
        static std::vector<otext> text(size + 1);
        ASSERT_TRUE(OCI_ObjectToText(obj, &size, text.data()));
        ASSERT_GT(size, 0u);

        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop type TestObject_BoolT"));
    }
}