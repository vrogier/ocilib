#include "ocilib_tests.h"
#include "NullArgs.h"

using namespace ocilib;

namespace TestCApi
{
    /* -------------------------------------------------------------------------
       Shared fixture: Sale object with Vendor child and Date attr
       ------------------------------------------------------------------------- */

    class TestObjectSetGet : public ::testing::Test
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_Date* date = nullptr;
        OCI_Object* vendor = nullptr;
        OCI_Object* sale = nullptr;

        void SetUp() override
        {
            ExecDML(OTEXT("create or replace type TestObjectSetGetBasicPropsVendor as object(code number, name varchar2(30))"));
            ExecDML(OTEXT("create or replace type TestObjectSetGetBasicPropsSale as object(code number, price float, name varchar2(30), ref varchar2(30), date_sale date, vendor TestObjectSetGetBasicPropsVendor)"));

            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);
            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            date = OCI_DateCreate(env);
            ASSERT_NE(nullptr, date);
            ASSERT_TRUE(OCI_DateSysDate(date));
            vendor = OCI_ObjectCreate(OCI_TypeInfoGet(conn, OTEXT("TestObjectSetGetBasicPropsVendor"), OCI_TIF_TYPE));
            ASSERT_NE(nullptr, vendor);
            ASSERT_TRUE(OCI_ObjectSetInt32(vendor, OTEXT("CODE"), 134));
            ASSERT_TRUE(OCI_ObjectSetString(vendor, OTEXT("NAME"), OTEXT("JOHN SMITH")));
            sale = OCI_ObjectCreate(OCI_TypeInfoGet(conn, OTEXT("TestObjectSetGetBasicPropsSale"), OCI_TIF_TYPE));
            ASSERT_NE(nullptr, sale);
        }

        void TearDown() override
        {
            if (sale) OCI_ObjectFree(sale);
            if (vendor) OCI_ObjectFree(vendor);
            if (date) OCI_DateFree(date);
            if (conn) OCI_ConnectionFree(conn);

            ExecDML(OTEXT("drop type TestObjectSetGetBasicPropsSale"));
            ExecDML(OTEXT("drop type TestObjectSetGetBasicPropsVendor"));
        }
    };

    TEST_F(TestObjectSetGet, SetAndGetScalarAttrs_RoundTripsValues)
    {
        ASSERT_TRUE(OCI_ObjectSetInt32(sale, OTEXT("CODE"), 1));
        ASSERT_TRUE(OCI_ObjectSetDouble(sale, OTEXT("PRICE"), 12.99));
        ASSERT_TRUE(OCI_ObjectSetString(sale, OTEXT("NAME"), OTEXT("USB KEY 2go")));
        ASSERT_TRUE(OCI_ObjectSetString(sale, OTEXT("REF"), OTEXT("A56547WSAA")));

        EXPECT_EQ(1, OCI_ObjectGetInt32(sale, OTEXT("CODE")));
        EXPECT_EQ(12.99, OCI_ObjectGetDouble(sale, OTEXT("PRICE")));
        EXPECT_EQ(String(OTEXT("USB KEY 2go")), String(OCI_ObjectGetString(sale, OTEXT("NAME"))));
        EXPECT_EQ(String(OTEXT("A56547WSAA")), String(OCI_ObjectGetString(sale, OTEXT("REF"))));
    }

    TEST_F(TestObjectSetGet, SetAndGetNestedObjectAttr_ReturnsNestedValues)
    {
        ASSERT_TRUE(OCI_ObjectSetObject(sale, OTEXT("VENDOR"), vendor));

        const auto vendor2 = OCI_ObjectGetObject(sale, OTEXT("VENDOR"));

        ASSERT_NE(nullptr, vendor2);
        EXPECT_EQ(134, OCI_ObjectGetInt32(vendor2, OTEXT("CODE")));
        EXPECT_EQ(String(OTEXT("JOHN SMITH")), String(OCI_ObjectGetString(vendor2, OTEXT("NAME"))));
    }

    TEST_F(TestObjectSetGet, SetAndGetDateAttr_ReturnsSameDate)
    {
        ASSERT_TRUE(OCI_ObjectSetDate(sale, OTEXT("DATE_SALE"), date));

        const auto date2 = OCI_ObjectGetDate(sale, OTEXT("DATE_SALE"));

        ASSERT_NE(nullptr, date2);
        EXPECT_EQ(0, OCI_DateCompare(date, date2));
    }

    /* -------------------------------------------------------------------------
       Shared fixture: TestObject_FullType object + nested TestObject_TgtType
       ------------------------------------------------------------------------- */

    class TestObjectAttrs : public ::testing::Test
    {
    protected:
        OCI_Environment* env = nullptr;
        OCI_Connection* conn = nullptr;
        OCI_TypeInfo* fullType = nullptr;
        OCI_TypeInfo* tgtType = nullptr;
        OCI_Object* obj = nullptr;
        OCI_Object* nested = nullptr;

        void SetUp() override
        {
            ExecDML(OTEXT("create or replace type TestObject_TgtType as object(code int, name varchar2(30))"));
            ExecDML(OTEXT("create or replace type TestObject_NumVarray as varray(3) of number"));
            ExecDML(OTEXT("create or replace type TestObject_FullType as object(n number, i int, s varchar2(30), r raw(20), d date, ts timestamp, iv interval day to second, lob clob, coll TestObject_NumVarray, ref_val ref TestObject_TgtType, f bfile, nested TestObject_TgtType)"));

            env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);
            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            fullType = OCI_TypeInfoGet(conn, OTEXT("TestObject_FullType"), OCI_TIF_TYPE);
            ASSERT_NE(nullptr, fullType);
            tgtType = OCI_TypeInfoGet(conn, OTEXT("TestObject_TgtType"), OCI_TIF_TYPE);
            ASSERT_NE(nullptr, tgtType);
            obj = OCI_ObjectCreate(fullType);
            ASSERT_NE(nullptr, obj);
            nested = OCI_ObjectCreate(tgtType);
            ASSERT_NE(nullptr, nested);
        }

        void TearDown() override
        {
            if (nested) OCI_ObjectFree(nested);
            if (obj) OCI_ObjectFree(obj);
            if (tgtType) OCI_TypeInfoFree(tgtType);
            if (fullType) OCI_TypeInfoFree(fullType);
            if (conn) OCI_ConnectionFree(conn);

            ExecDML(OTEXT("drop type TestObject_FullType"));
            ExecDML(OTEXT("drop type TestObject_NumVarray"));
            ExecDML(OTEXT("drop type TestObject_TgtType"));
        }
    };

    TEST_F(TestObjectAttrs, GetTypeOnCreatedObject_ReturnsKnownType)
    {
        EXPECT_NE((OCI_UInt32) OCI_UNKNOWN, OCI_ObjectGetType(obj));
    }

    TEST_F(TestObjectAttrs, SetAndGetNumberAttr_RoundTripsValue)
    {
        const auto number = OCI_NumberCreate(env);
        OCI_Int32 num = 42;
        ASSERT_TRUE(OCI_NumberSetValue(number, OCI_NUM_INT32, &num));

        ASSERT_TRUE(OCI_ObjectSetNumber(obj, OTEXT("N"), number));

        const auto num2 = OCI_ObjectGetNumber(obj, OTEXT("N"));
        ASSERT_NE(nullptr, num2);
        OCI_Int32 numOut = 0;
        ASSERT_TRUE(OCI_NumberGetValue(num2, OCI_NUM_INT32, &numOut));
        EXPECT_EQ(42, numOut);

        ASSERT_TRUE(OCI_NumberFree(number));
    }

    TEST_F(TestObjectAttrs, SetAndGetIntAttr_RoundTripsValue)
    {
        ASSERT_TRUE(OCI_ObjectSetInt32(obj, OTEXT("I"), 99));

        EXPECT_EQ(99, OCI_ObjectGetInt32(obj, OTEXT("I")));
    }

    TEST_F(TestObjectAttrs, SetAndGetStringAttr_RoundTripsValue)
    {
        ASSERT_TRUE(OCI_ObjectSetString(obj, OTEXT("S"), OTEXT("full")));

        EXPECT_EQ(String(OTEXT("full")), String(OCI_ObjectGetString(obj, OTEXT("S"))));
    }

    TEST_F(TestObjectAttrs, ToString_AllAttributeTypes_Formatted)
    {
        /* covers the per-datatype arms of OcilibObjectToString() :
           OCI_CDT_RAW / DATETIME / TIMESTAMP / INTERVAL / LOB / FILE / REF /
           OBJECT / COLLECTION — every arm needs a non-NULL attribute, so the
           object is populated server-side and fetched back */

        ExecDML(OTEXT("begin execute immediate 'drop table TestObject_RefTarget'; exception when others then null; end;"));
        ExecDML(OTEXT("begin execute immediate 'drop table TestObject_FullRows'; exception when others then null; end;"));
        ExecDML(OTEXT("create table TestObject_RefTarget of TestObject_TgtType"));
        ExecDML(OTEXT("insert into TestObject_RefTarget values (1, 'x')"));
        ExecDML(OTEXT("create table TestObject_FullRows (o TestObject_FullType)"));
        ExecDML(OTEXT("insert into TestObject_FullRows values (TestObject_FullType("
                      "1, 2, 's', 'AABB', date'2024-01-02', timestamp'2024-01-02 03:04:05',"
                      "interval '1' day, 'clobdata', TestObject_NumVarray(1,2,3),"
                      "(select ref(t) from TestObject_RefTarget t where rownum=1),"
                      "bfilename('DATA_PUMP_DIR','x.dat'), TestObject_TgtType(3,'y')))"));

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);
        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select o from TestObject_FullRows")));
        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        const auto fetched = OCI_ResultsetGetObject(rs, 0);
        ASSERT_NE(nullptr, fetched);

        OCI_UInt32 size = 0;
        ASSERT_TRUE(OCI_ObjectToText(fetched, &size, nullptr));
        ASSERT_GT(size, 0u);

        std::vector<otext> buf(size + 1, 0);
        ASSERT_TRUE(OCI_ObjectToText(fetched, &size, buf.data()));
        const String out(buf.data());
        EXPECT_NE(String::npos, out.find(OTEXT("TESTOBJECT_FULLTYPE(")));

        ASSERT_TRUE(OCI_ObjectFree(fetched));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ExecDML(OTEXT("drop table TestObject_FullRows"));
        ExecDML(OTEXT("drop table TestObject_RefTarget"));
    }

    TEST_F(TestObjectAttrs, SetAndGetRawAttr_RoundTripsBytes)
    {
        unsigned char raw[] = { 0x00, 0x01, 0x02, 0x03, 0x04 };

        ASSERT_TRUE(OCI_ObjectSetRaw(obj, OTEXT("R"), raw, sizeof(raw)));

        EXPECT_EQ(sizeof(raw), OCI_ObjectGetRawSize(obj, OTEXT("R")));
        unsigned char rawOut[20] = {};
        EXPECT_EQ((OCI_Int32)sizeof(raw), OCI_ObjectGetRaw(obj, OTEXT("R"), rawOut, sizeof(rawOut)));
        EXPECT_EQ(0, memcmp(raw, rawOut, sizeof(raw)));
    }

    TEST_F(TestObjectAttrs, SetAndGetDateAttr_RoundTripsValue)
    {
        const auto date = OCI_DateCreate(env);
        ASSERT_NE(nullptr, date);
        ASSERT_TRUE(OCI_DateSetDateTime(date, 2023, 6, 15, 12, 30, 0));

        ASSERT_TRUE(OCI_ObjectSetDate(obj, OTEXT("D"), date));

        const auto date2 = OCI_ObjectGetDate(obj, OTEXT("D"));
        ASSERT_NE(nullptr, date2);
        EXPECT_EQ(0, OCI_DateCompare(date, date2));

        ASSERT_TRUE(OCI_DateFree(date));
    }

    TEST_F(TestObjectAttrs, SetAndGetTimestampAttr_ReturnsValue)
    {
        const auto ts = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        ASSERT_NE(nullptr, ts);
        ASSERT_TRUE(OCI_TimestampConstruct(ts, 2023, 6, 15, 12, 30, 0, 0, nullptr));

        ASSERT_TRUE(OCI_ObjectSetTimestamp(obj, OTEXT("TS"), ts));

        EXPECT_NE(nullptr, OCI_ObjectGetTimestamp(obj, OTEXT("TS")));

        ASSERT_TRUE(OCI_TimestampFree(ts));
    }

    TEST_F(TestObjectAttrs, SetAndGetIntervalAttr_ReturnsValue)
    {
        const auto itv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        ASSERT_NE(nullptr, itv);
        ASSERT_TRUE(OCI_IntervalSetDaySecond(itv, 1, 2, 3, 4, 0));

        ASSERT_TRUE(OCI_ObjectSetInterval(obj, OTEXT("IV"), itv));

        EXPECT_NE(nullptr, OCI_ObjectGetInterval(obj, OTEXT("IV")));

        ASSERT_TRUE(OCI_IntervalFree(itv));
    }

    TEST_F(TestObjectAttrs, SetAndGetLobAttr_ReturnsLob)
    {
        const auto lob = OCI_LobCreate(conn, OCI_CLOB);
        ASSERT_NE(nullptr, lob);
        OCI_UInt32 lob_cc = (OCI_UInt32)ostrlen(OTEXT("object clob"));
        OCI_UInt32 lob_bc = 0;
        ASSERT_TRUE(OCI_LobWrite(lob, (void*)OTEXT("object clob"), &lob_cc, &lob_bc));

        ASSERT_TRUE(OCI_ObjectSetLob(obj, OTEXT("LOB"), lob));

        const auto lob2 = OCI_ObjectGetLob(obj, OTEXT("LOB"));
        ASSERT_NE(nullptr, lob2);
        EXPECT_GT(OCI_LobGetLength(lob2), (OCI_UInt64)0);

        ASSERT_TRUE(OCI_LobFree(lob));
    }

    TEST_F(TestObjectAttrs, SetAndGetCollAttr_ReturnsCollection)
    {
        const auto collType = OCI_TypeInfoGet(conn, OTEXT("TestObject_NumVarray"), OCI_TIF_TYPE);
        const auto coll = OCI_CollectionCreate(collType);
        ASSERT_NE(nullptr, coll);
        const auto elem = OCI_ElementCreate(collType);
        ASSERT_TRUE(OCI_ElementSetInt32(elem, 1));
        ASSERT_TRUE(OCI_CollectionAppend(coll, elem));
        ASSERT_TRUE(OCI_ElementFree(elem));

        ASSERT_TRUE(OCI_ObjectSetColl(obj, OTEXT("COLL"), coll));

        const auto coll2 = OCI_ObjectGetColl(obj, OTEXT("COLL"));
        ASSERT_NE(nullptr, coll2);
        EXPECT_EQ(1u, OCI_CollectionGetCount(coll2));

        ASSERT_TRUE(OCI_CollectionFree(coll));
        ASSERT_TRUE(OCI_TypeInfoFree(collType));
    }

    TEST_F(TestObjectAttrs, SetNullOnRefAttr_MarksAttributeNull)
    {
        ASSERT_TRUE(OCI_ObjectSetReference(obj, OTEXT("REF_VAL"), nullptr));

        EXPECT_TRUE(OCI_ObjectIsNull(obj, OTEXT("REF_VAL")));
        EXPECT_EQ(nullptr, OCI_ObjectGetReference(obj, OTEXT("REF_VAL")));
    }

    TEST_F(TestObjectAttrs, SetNullOnFileAttr_MarksAttributeNull)
    {
        ASSERT_TRUE(OCI_ObjectSetFile(obj, OTEXT("F"), nullptr));

        EXPECT_TRUE(OCI_ObjectIsNull(obj, OTEXT("F")));
        EXPECT_EQ(nullptr, OCI_ObjectGetFile(obj, OTEXT("F")));
    }

    TEST_F(TestObjectAttrs, SetAndGetNestedObjectAttr_ReturnsNested)
    {
        ASSERT_TRUE(OCI_ObjectSetInt32(nested, OTEXT("CODE"), 7));
        ASSERT_TRUE(OCI_ObjectSetString(nested, OTEXT("NAME"), OTEXT("nested")));

        ASSERT_TRUE(OCI_ObjectSetObject(obj, OTEXT("NESTED"), nested));

        const auto nested2 = OCI_ObjectGetObject(obj, OTEXT("NESTED"));
        ASSERT_NE(nullptr, nested2);
        EXPECT_EQ(7, OCI_ObjectGetInt32(nested2, OTEXT("CODE")));
        EXPECT_EQ(String(OTEXT("nested")), String(OCI_ObjectGetString(nested2, OTEXT("NAME"))));
    }

    TEST_F(TestObjectAttrs, ObjectToText_RendersNonEmptyText)
    {
        ASSERT_TRUE(OCI_ObjectSetInt32(obj, OTEXT("I"), 99));
        OCI_UInt32 size = 0;
        ASSERT_TRUE(OCI_ObjectToText(obj, &size, nullptr));
        std::vector<otext> text(size + 1);

        ASSERT_TRUE(OCI_ObjectToText(obj, &size, text.data()));

        EXPECT_GT(size, 0u);
    }

    TEST_F(TestObjectAttrs, GetTypeInfo_ReturnsCreationTypeInfo)
    {
        EXPECT_EQ(fullType, OCI_ObjectGetTypeInfo(obj));
    }

    TEST_F(TestObjectAttrs, GetStruct_ReturnsRawStructPointer)
    {
        void* p_struct = nullptr;
        void* p_ind = nullptr;

        ASSERT_TRUE(OCI_ObjectGetStruct(obj, &p_struct, &p_ind));

        EXPECT_NE(nullptr, p_struct);
    }

    TEST_F(TestObjectAttrs, ObjectArrayCreate_AllocatesUsableObjects)
    {
        const auto arr = OCI_ObjectArrayCreate(fullType, 2);
        ASSERT_NE(nullptr, arr);

        EXPECT_TRUE(OCI_ObjectSetInt32(arr[0], OTEXT("I"), 1));

        ASSERT_TRUE(OCI_ObjectArrayFree(arr));
    }

    TEST_F(TestObjectAttrs, ObjectAssign_CopiesAttributeValues)
    {
        ASSERT_TRUE(OCI_ObjectSetInt32(obj, OTEXT("I"), 99));
        const auto assigned = OCI_ObjectCreate(fullType);
        ASSERT_NE(nullptr, assigned);

        ASSERT_TRUE(OCI_ObjectAssign(assigned, obj));

        EXPECT_EQ(99, OCI_ObjectGetInt32(assigned, OTEXT("I")));

        ASSERT_TRUE(OCI_ObjectFree(assigned));
    }

    /* -------------------------------------------------------------------------
       Parameterized: numeric setter/getter round-trip per subtype on attr N
       ------------------------------------------------------------------------- */

    struct ObjNumCase
    {
        const char* name;
        void (*run)(OCI_Object*);
    };

    void PrintTo(const ObjNumCase& c, std::ostream* os) { *os << c.name; }

    static void ObjNumShort(OCI_Object* o)
    {
        ASSERT_TRUE(OCI_ObjectSetInt16(o, OTEXT("N"), 11));
        EXPECT_EQ(11, OCI_ObjectGetInt16(o, OTEXT("N")));
    }
    static void ObjNumUShort(OCI_Object* o)
    {
        ASSERT_TRUE(OCI_ObjectSetUInt16(o, OTEXT("N"), 12u));
        EXPECT_EQ(12u, OCI_ObjectGetUInt16(o, OTEXT("N")));
    }
    static void ObjNumInt(OCI_Object* o)
    {
        ASSERT_TRUE(OCI_ObjectSetInt32(o, OTEXT("N"), 13));
        EXPECT_EQ(13, OCI_ObjectGetInt32(o, OTEXT("N")));
    }
    static void ObjNumUInt(OCI_Object* o)
    {
        ASSERT_TRUE(OCI_ObjectSetUInt32(o, OTEXT("N"), 14u));
        EXPECT_EQ(14u, OCI_ObjectGetUInt32(o, OTEXT("N")));
    }
    static void ObjNumBigInt(OCI_Object* o)
    {
        ASSERT_TRUE(OCI_ObjectSetInt64(o, OTEXT("N"), static_cast<OCI_Int64>(15)));
        EXPECT_EQ(15, OCI_ObjectGetInt64(o, OTEXT("N")));
    }
    static void ObjNumUBigInt(OCI_Object* o)
    {
        ASSERT_TRUE(OCI_ObjectSetUInt64(o, OTEXT("N"), static_cast<OCI_UInt64>(16)));
        EXPECT_EQ(16u, OCI_ObjectGetUInt64(o, OTEXT("N")));
    }
    static void ObjNumFloat(OCI_Object* o)
    {
        ASSERT_TRUE(OCI_ObjectSetFloat(o, OTEXT("N"), 1.5f));
        EXPECT_NEAR(1.5f, OCI_ObjectGetFloat(o, OTEXT("N")), 0.01f);
    }
    static void ObjNumDouble(OCI_Object* o)
    {
        ASSERT_TRUE(OCI_ObjectSetDouble(o, OTEXT("N"), 2.5));
        EXPECT_NEAR(2.5, OCI_ObjectGetDouble(o, OTEXT("N")), 0.01);
    }

    class TestObjectNumeric : public ::testing::TestWithParam<ObjNumCase>
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_TypeInfo* typinf = nullptr;
        OCI_Object* obj = nullptr;

        void SetUp() override
        {
            ExecDML(OTEXT("create or replace type TestObject_NumericT as object(n number, s varchar2(20))"));

            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            typinf = OCI_TypeInfoGet(conn, OTEXT("TestObject_NumericT"), OCI_TIF_TYPE);
            ASSERT_NE(nullptr, typinf);
            obj = OCI_ObjectCreate(typinf);
            ASSERT_NE(nullptr, obj);
        }

        void TearDown() override
        {
            if (obj) OCI_ObjectFree(obj);
            if (typinf) OCI_TypeInfoFree(typinf);
            if (conn) OCI_ConnectionFree(conn);

            ExecDML(OTEXT("drop type TestObject_NumericT"));
        }
    };

    TEST_P(TestObjectNumeric, SetThenGet_RoundTripsValue)
    {
        GetParam().run(obj);
    }

    INSTANTIATE_TEST_CASE_P(NumSubtype, TestObjectNumeric,
        ::testing::Values(
            ObjNumCase{ "Short",   ObjNumShort },
            ObjNumCase{ "UShort",  ObjNumUShort },
            ObjNumCase{ "Int",     ObjNumInt },
            ObjNumCase{ "UInt",    ObjNumUInt },
            ObjNumCase{ "BigInt",  ObjNumBigInt },
            ObjNumCase{ "UBigInt", ObjNumUBigInt },
            ObjNumCase{ "Float",   ObjNumFloat },
            ObjNumCase{ "Double",  ObjNumDouble }),
        ::testing::PrintToStringParamName());

    TEST_F(TestObjectNumeric, GetNumericOnStringAttr_CoercesValue)
    {
        ASSERT_TRUE(OCI_ObjectSetString(obj, OTEXT("S"), OTEXT("1234")));

        EXPECT_EQ(1234, OCI_ObjectGetInt32(obj, OTEXT("S")));
        EXPECT_NEAR(1234.0, OCI_ObjectGetDouble(obj, OTEXT("S")), 0.01);
    }

    TEST_F(TestObjectNumeric, SetNullNumberAttr_MarksNullAndGetterReturnsDefault)
    {
        ASSERT_TRUE(OCI_ObjectSetNumber(obj, OTEXT("N"), nullptr));

        EXPECT_TRUE(OCI_ObjectIsNull(obj, OTEXT("N")));
        EXPECT_EQ(nullptr, OCI_ObjectGetNumber(obj, OTEXT("N")));
        EXPECT_EQ(0, OCI_ObjectGetInt32(obj, OTEXT("N")));
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

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("SELECT REF(t) FROM TestObject_SelfRefTable t")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        const auto ref = OCI_ResultsetGetReference(rs, 0);
        ASSERT_NE(nullptr, ref);

        const auto obj = OCI_ReferenceGetObject(ref);
        ASSERT_NE(nullptr, obj);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestObject_SelfRefT"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        const auto selfRef = OCI_ReferenceCreate(typinf);
        ASSERT_NE(nullptr, selfRef);

        ASSERT_TRUE(OCI_ObjectGetSelfReference(obj, selfRef));
        ASSERT_FALSE(OCI_ReferenceIsNull(selfRef));

        ASSERT_TRUE(OCI_ReferenceFree(selfRef));
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

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("SELECT VALUE(t) FROM TestObject_SubNFTable t")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        const auto obj = OCI_ResultsetGetObject(rs, 0);
        ASSERT_NE(nullptr, obj);

        const auto typinf = OCI_ObjectGetTypeInfo(obj);
        ASSERT_NE(nullptr, typinf);

        ASSERT_EQ(String(OTEXT("TESTOBJECT_SUBNF")), String(OCI_TypeInfoGetName(typinf)));

        ASSERT_EQ(1, OCI_ObjectGetInt32(obj, OTEXT("CODE")));
        ASSERT_EQ(String(OTEXT("subvalue")), String(OCI_ObjectGetString(obj, OTEXT("NAME"))));

        OCI_UInt32 size = 0;
        ASSERT_TRUE(OCI_ObjectToText(obj, &size, nullptr));
        static std::vector<otext> text(size + 1);
        ASSERT_TRUE(OCI_ObjectToText(obj, &size, text.data()));
        ASSERT_NE(String::npos, String(text.data()).find(OTEXT("TESTOBJECT_SUBNF")));

        unsigned char buffer[256] = {};
        OCI_Int32 inds[2] = {};
        ASSERT_TRUE(OCI_ResultsetGetStruct(rs, buffer, inds));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestObject_SubNFTable"));
        ExecDML(OTEXT("drop type TestObject_SubNF"));
        ExecDML(OTEXT("drop type TestObject_BaseNF"));
    }

    /* -------------------------------------------------------------------------
       Parameterized: setting NULL on each attribute type marks it null and
       makes its getter return the documented default
       ------------------------------------------------------------------------- */

    struct ObjNullCase
    {
        const char* name;
        void (*run)(OCI_Object*);
    };

    void PrintTo(const ObjNullCase& c, std::ostream* os) { *os << c.name; }

    static void ObjNullNumber(OCI_Object* obj)
    {
        ASSERT_TRUE(OCI_ObjectSetNull(obj, OTEXT("N")));
        EXPECT_TRUE(OCI_ObjectIsNull(obj, OTEXT("N")));
        EXPECT_EQ(0, OCI_ObjectGetInt32(obj, OTEXT("N")));
        EXPECT_EQ(nullptr, OCI_ObjectGetNumber(obj, OTEXT("N")));
    }
    static void ObjNullInt(OCI_Object* obj)
    {
        ASSERT_TRUE(OCI_ObjectSetNull(obj, OTEXT("I")));
        EXPECT_TRUE(OCI_ObjectIsNull(obj, OTEXT("I")));
        EXPECT_EQ(0, OCI_ObjectGetInt32(obj, OTEXT("I")));
    }
    static void ObjNullString(OCI_Object* obj)
    {
        ASSERT_TRUE(OCI_ObjectSetString(obj, OTEXT("S"), nullptr));
        EXPECT_TRUE(OCI_ObjectIsNull(obj, OTEXT("S")));
        EXPECT_EQ(nullptr, OCI_ObjectGetString(obj, OTEXT("S")));
    }
    static void ObjNullRaw(OCI_Object* obj)
    {
        ASSERT_TRUE(OCI_ObjectSetRaw(obj, OTEXT("R"), nullptr, 0));
        EXPECT_TRUE(OCI_ObjectIsNull(obj, OTEXT("R")));
        EXPECT_EQ(0, OCI_ObjectGetRaw(obj, OTEXT("R"), nullptr, 0));
        EXPECT_EQ(0u, OCI_ObjectGetRawSize(obj, OTEXT("R")));
    }
    static void ObjNullDate(OCI_Object* obj)
    {
        ASSERT_TRUE(OCI_ObjectSetDate(obj, OTEXT("D"), nullptr));
        EXPECT_TRUE(OCI_ObjectIsNull(obj, OTEXT("D")));
        EXPECT_EQ(nullptr, OCI_ObjectGetDate(obj, OTEXT("D")));
    }
    static void ObjNullTimestamp(OCI_Object* obj)
    {
        ASSERT_TRUE(OCI_ObjectSetTimestamp(obj, OTEXT("TS"), nullptr));
        EXPECT_TRUE(OCI_ObjectIsNull(obj, OTEXT("TS")));
        EXPECT_EQ(nullptr, OCI_ObjectGetTimestamp(obj, OTEXT("TS")));
    }
    static void ObjNullInterval(OCI_Object* obj)
    {
        ASSERT_TRUE(OCI_ObjectSetInterval(obj, OTEXT("IV"), nullptr));
        EXPECT_TRUE(OCI_ObjectIsNull(obj, OTEXT("IV")));
        EXPECT_EQ(nullptr, OCI_ObjectGetInterval(obj, OTEXT("IV")));
    }
    static void ObjNullLob(OCI_Object* obj)
    {
        ASSERT_TRUE(OCI_ObjectSetLob(obj, OTEXT("LOB"), nullptr));
        EXPECT_TRUE(OCI_ObjectIsNull(obj, OTEXT("LOB")));
        EXPECT_EQ(nullptr, OCI_ObjectGetLob(obj, OTEXT("LOB")));
    }
    static void ObjNullColl(OCI_Object* obj)
    {
        ASSERT_TRUE(OCI_ObjectSetColl(obj, OTEXT("COLL"), nullptr));
        EXPECT_TRUE(OCI_ObjectIsNull(obj, OTEXT("COLL")));
        EXPECT_EQ(nullptr, OCI_ObjectGetColl(obj, OTEXT("COLL")));
    }
    static void ObjNullRef(OCI_Object* obj)
    {
        ASSERT_TRUE(OCI_ObjectSetReference(obj, OTEXT("REF_VAL"), nullptr));
        EXPECT_TRUE(OCI_ObjectIsNull(obj, OTEXT("REF_VAL")));
        EXPECT_EQ(nullptr, OCI_ObjectGetReference(obj, OTEXT("REF_VAL")));
    }
    static void ObjNullFile(OCI_Object* obj)
    {
        ASSERT_TRUE(OCI_ObjectSetFile(obj, OTEXT("F"), nullptr));
        EXPECT_TRUE(OCI_ObjectIsNull(obj, OTEXT("F")));
        EXPECT_EQ(nullptr, OCI_ObjectGetFile(obj, OTEXT("F")));
    }
    static void ObjNullNested(OCI_Object* obj)
    {
        ASSERT_TRUE(OCI_ObjectSetObject(obj, OTEXT("NESTED"), nullptr));
        EXPECT_TRUE(OCI_ObjectIsNull(obj, OTEXT("NESTED")));
        EXPECT_EQ(nullptr, OCI_ObjectGetObject(obj, OTEXT("NESTED")));
    }

    class TestObjectNullAttrs : public ::testing::TestWithParam<ObjNullCase>
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_TypeInfo* fullType = nullptr;
        OCI_TypeInfo* tgtType = nullptr;
        OCI_Object* obj = nullptr;

        void SetUp() override
        {
            ExecDML(OTEXT("create or replace type TestObject_NullTgtT as object(code int, name varchar2(20))"));
            ExecDML(OTEXT("create or replace type TestObject_NullVarray as varray(1) of number"));
            ExecDML(OTEXT("create or replace type TestObject_NullFullT as object(n number, i int, s varchar2(20), r raw(10), d date, ts timestamp, iv interval day to second, lob clob, coll TestObject_NullVarray, ref_val ref TestObject_NullTgtT, f bfile, nested TestObject_NullTgtT)"));

            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            tgtType = OCI_TypeInfoGet(conn, OTEXT("TestObject_NullTgtT"), OCI_TIF_TYPE);
            fullType = OCI_TypeInfoGet(conn, OTEXT("TestObject_NullFullT"), OCI_TIF_TYPE);
            ASSERT_NE(nullptr, fullType);
            obj = OCI_ObjectCreate(fullType);
            ASSERT_NE(nullptr, obj);
        }

        void TearDown() override
        {
            if (obj) OCI_ObjectFree(obj);
            if (fullType) OCI_TypeInfoFree(fullType);
            if (tgtType) OCI_TypeInfoFree(tgtType);
            if (conn) OCI_ConnectionFree(conn);

            ExecDML(OTEXT("drop type TestObject_NullFullT"));
            ExecDML(OTEXT("drop type TestObject_NullVarray"));
            ExecDML(OTEXT("drop type TestObject_NullTgtT"));
        }
    };

    TEST_P(TestObjectNullAttrs, SetNullOnAttr_MarksNullAndGetterReturnsDefault)
    {
        GetParam().run(obj);
    }

    INSTANTIATE_TEST_CASE_P(AttrType, TestObjectNullAttrs,
        ::testing::Values(
            ObjNullCase{ "Number",    ObjNullNumber },
            ObjNullCase{ "Int",       ObjNullInt },
            ObjNullCase{ "String",    ObjNullString },
            ObjNullCase{ "Raw",       ObjNullRaw },
            ObjNullCase{ "Date",      ObjNullDate },
            ObjNullCase{ "Timestamp", ObjNullTimestamp },
            ObjNullCase{ "Interval",  ObjNullInterval },
            ObjNullCase{ "Lob",       ObjNullLob },
            ObjNullCase{ "Coll",      ObjNullColl },
            ObjNullCase{ "Ref",       ObjNullRef },
            ObjNullCase{ "File",      ObjNullFile },
            ObjNullCase{ "Nested",    ObjNullNested }),
        ::testing::PrintToStringParamName());

    TEST_F(TestObjectNullAttrs, ObjectToTextOnNullObject_RendersNonEmptyText)
    {
        OCI_UInt32 size = 0;
        ASSERT_TRUE(OCI_ObjectToText(obj, &size, nullptr));
        std::vector<otext> text(size + 1);

        ASSERT_TRUE(OCI_ObjectToText(obj, &size, text.data()));

        EXPECT_GT(size, 0u);
    }

    /* -------------------------------------------------------------------------
       Shared fixture: FileRefT object + fetched REF + BFILE
       ------------------------------------------------------------------------- */

    class TestObjectFileRef : public ::testing::Test
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_Statement* stmt = nullptr;
        OCI_TypeInfo* fileType = nullptr;
        OCI_Object* obj = nullptr;
        OCI_Reference* ref = nullptr;

        void SetUp() override
        {
            ExecDML(OTEXT("create or replace type TestObject_RefTgtT as object(code int)"));
            ExecDML(OTEXT("create table TestObject_RefTgtTable of TestObject_RefTgtT"));
            ExecDML(OTEXT("insert into TestObject_RefTgtTable values (1)"));
            ExecDML(OTEXT("create or replace type TestObject_FileRefT as object(code int, f bfile, r ref TestObject_RefTgtT)"));

            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);
            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);
            ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("SELECT REF(t) FROM TestObject_RefTgtTable t")));
            const auto rs = OCI_StatementGetResultset(stmt);
            ASSERT_NE(nullptr, rs);
            ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
            ref = OCI_ResultsetGetReference(rs, 0);
            ASSERT_NE(nullptr, ref);
            fileType = OCI_TypeInfoGet(conn, OTEXT("TestObject_FileRefT"), OCI_TIF_TYPE);
            ASSERT_NE(nullptr, fileType);
            obj = OCI_ObjectCreate(fileType);
            ASSERT_NE(nullptr, obj);
        }

        void TearDown() override
        {
            if (obj) OCI_ObjectFree(obj);
            if (fileType) OCI_TypeInfoFree(fileType);
            if (stmt) OCI_StatementFree(stmt);
            if (conn) OCI_ConnectionFree(conn);

            ExecDML(OTEXT("drop table TestObject_RefTgtTable"));
            ExecDML(OTEXT("drop type TestObject_FileRefT"));
            ExecDML(OTEXT("drop type TestObject_RefTgtT"));
        }
    };

    TEST_F(TestObjectFileRef, SetAndGetFileAttr_ReturnsFileNameParts)
    {
        const auto file = OCI_FileCreate(conn, OCI_BFILE);
        ASSERT_NE(nullptr, file);
        ASSERT_TRUE(OCI_FileSetName(file, OTEXT("FAKE_DIR"), OTEXT("file.txt")));

        ASSERT_TRUE(OCI_ObjectSetFile(obj, OTEXT("F"), file));

        const auto file2 = OCI_ObjectGetFile(obj, OTEXT("F"));
        ASSERT_NE(nullptr, file2);
        EXPECT_EQ(String(OTEXT("FAKE_DIR")), String(OCI_FileGetDirectory(file2)));
        EXPECT_EQ(String(OTEXT("file.txt")), String(OCI_FileGetName(file2)));

        ASSERT_TRUE(OCI_FileFree(file));
    }

    TEST_F(TestObjectFileRef, SetAndGetRefAttr_ReturnsNonNullRef)
    {
        ASSERT_FALSE(OCI_ReferenceIsNull(ref));

        ASSERT_TRUE(OCI_ObjectSetReference(obj, OTEXT("R"), ref));

        const auto ref2 = OCI_ObjectGetReference(obj, OTEXT("R"));
        ASSERT_NE(nullptr, ref2);
        EXPECT_FALSE(OCI_ReferenceIsNull(ref2));
    }

    TEST_F(TestObjectFileRef, ObjectToTextOnFileRefObject_RendersNonEmptyText)
    {
        ASSERT_TRUE(OCI_ObjectSetInt32(obj, OTEXT("CODE"), 7));
        OCI_UInt32 size = 0;
        ASSERT_TRUE(OCI_ObjectToText(obj, &size, nullptr));
        std::vector<otext> text(size + 1);

        ASSERT_TRUE(OCI_ObjectToText(obj, &size, text.data()));

        EXPECT_GT(size, 0u);
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

        ASSERT_TRUE(OCI_ObjectSetInt32(obj, OTEXT("I"), 1));
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

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("SELECT VALUE(t) FROM TestObject_StructTable t")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        unsigned char buffer[256] = {};
        OCI_Int32 inds[2] = {};
        ASSERT_TRUE(OCI_ResultsetGetStruct(rs, buffer, inds));

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

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("SELECT VALUE(t) FROM TestObject_BinNumTable t")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        unsigned char buffer[256] = {};
        OCI_Int32 inds[2] = {};
        ASSERT_TRUE(OCI_ResultsetGetStruct(rs, buffer, inds));

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

    TEST(TestObject, SetInt_OnStringAttr_ReturnsFalse)
    {
        ExecDML(OTEXT("create or replace type TestObjErr_Type as object(code number, name varchar2(30))"));

        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto ti = OCI_TypeInfoGet(conn, OTEXT("TestObjErr_Type"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, ti);
        const auto obj = OCI_ObjectCreate(ti);
        ASSERT_NE(nullptr, obj);

        ASSERT_FALSE(OCI_ObjectSetInt32(obj, OTEXT("name"), 42));

        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ExecDML(OTEXT("drop type TestObjErr_Type"));
    }
    TEST(TestObject, GetOnNonexistentAttr_ReturnsFalseAndRaisesError)
    {
        ExecDML(OTEXT("create or replace type TestObjErr_Type2 as object(code number, name varchar2(30))"));

        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto ti = OCI_TypeInfoGet(conn, OTEXT("TestObjErr_Type2"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, ti);
        const auto obj = OCI_ObjectCreate(ti);
        ASSERT_NE(nullptr, obj);
        unsigned char raw[32];

        ASSERT_EQ(FALSE, OCI_ObjectGetBoolean(obj, OTEXT("no_attr")));
        ASSERT_EQ(0,     OCI_ObjectGetRaw(obj, OTEXT("no_attr"), raw, sizeof(raw)));
        ASSERT_EQ(0u,   OCI_ObjectGetRawSize(obj, OTEXT("no_attr")));
        ASSERT_EQ(FALSE, OCI_ObjectIsNull(obj, OTEXT("no_attr")));
        ASSERT_NE(nullptr, OCI_GetLastError());

        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ExecDML(OTEXT("drop type TestObjErr_Type2"));
    }

    TEST(TestObject, SetOnNonexistentAttr_ReturnsFalse)
    {
        ExecDML(OTEXT("create or replace type TestObjErr_Type3 as object(code number, name varchar2(30))"));

        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto ti = OCI_TypeInfoGet(conn, OTEXT("TestObjErr_Type3"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, ti);
        const auto obj = OCI_ObjectCreate(ti);
        ASSERT_NE(nullptr, obj);

        ASSERT_FALSE(OCI_ObjectSetBoolean(obj, OTEXT("no_attr"), TRUE));
        ASSERT_FALSE(OCI_ObjectSetString(obj, OTEXT("no_attr"), OTEXT("v")));
        ASSERT_FALSE(OCI_ObjectSetNull(obj, OTEXT("no_attr")));
        ASSERT_NE(nullptr, OCI_GetLastError());

        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ExecDML(OTEXT("drop type TestObjErr_Type3"));
    }


    class TestObjectNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestObjectNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestObjectNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "ObjectArrayCreate", [] { return nullptr == OCI_ObjectArrayCreate(nullptr, 1); } },
            { "ObjectArrayFree", [] { return !OCI_ObjectArrayFree(nullptr); } },
            { "ObjectAssign", [] { return !OCI_ObjectAssign(nullptr, nullptr); } },
            { "ObjectCreate", [] { return nullptr == OCI_ObjectCreate(nullptr); } },
            { "ObjectFree", [] { return !OCI_ObjectFree(nullptr); } },
            { "ObjectGetBigInt", [] { return 0 == OCI_ObjectGetInt64(nullptr, OTEXT("x")); } },
            { "ObjectGetBoolean", [] { return !OCI_ObjectGetBoolean(nullptr, OTEXT("x")); } },
            { "ObjectGetColl", [] { return nullptr == OCI_ObjectGetColl(nullptr, OTEXT("x")); } },
            { "ObjectGetDate", [] { return nullptr == OCI_ObjectGetDate(nullptr, OTEXT("x")); } },
            { "ObjectGetDouble", [] { return 0.0 == OCI_ObjectGetDouble(nullptr, OTEXT("x")); } },
            { "ObjectGetFile", [] { return nullptr == OCI_ObjectGetFile(nullptr, OTEXT("x")); } },
            { "ObjectGetFloat", [] { return 0.0f == OCI_ObjectGetFloat(nullptr, OTEXT("x")); } },
            { "ObjectGetInt", [] { return 0 == OCI_ObjectGetInt32(nullptr, OTEXT("x")); } },
            { "ObjectGetInterval", [] { return nullptr == OCI_ObjectGetInterval(nullptr, OTEXT("x")); } },
            { "ObjectGetLob", [] { return nullptr == OCI_ObjectGetLob(nullptr, OTEXT("x")); } },
            { "ObjectGetNumber", [] { return nullptr == OCI_ObjectGetNumber(nullptr, OTEXT("x")); } },
            { "ObjectGetObject", [] { return nullptr == OCI_ObjectGetObject(nullptr, OTEXT("x")); } },
            { "ObjectGetRaw", [] { return 0 == OCI_ObjectGetRaw(nullptr, OTEXT("x"), NullArgBuffer(), 1); } },
            { "ObjectGetRawSize", [] { return 0 == OCI_ObjectGetRawSize(nullptr, OTEXT("x")); } },
            { "ObjectGetRef", [] { return nullptr == OCI_ObjectGetReference(nullptr, OTEXT("x")); } },
            { "ObjectGetSelfRef", [] { return !OCI_ObjectGetSelfReference(nullptr, nullptr); } },
            { "ObjectGetShort", [] { return 0 == OCI_ObjectGetInt16(nullptr, OTEXT("x")); } },
            { "ObjectGetString", [] { return nullptr == OCI_ObjectGetString(nullptr, OTEXT("x")); } },
            { "ObjectGetStruct", [] { return !OCI_ObjectGetStruct(nullptr, nullptr /* void ** */, nullptr /* void ** */); } },
            { "ObjectGetTimestamp", [] { return nullptr == OCI_ObjectGetTimestamp(nullptr, OTEXT("x")); } },
            { "ObjectGetType", [] { return OCI_UNKNOWN == OCI_ObjectGetType(nullptr); } },
            { "ObjectGetTypeInfo", [] { return nullptr == OCI_ObjectGetTypeInfo(nullptr); } },
            { "ObjectGetUnsignedBigInt", [] { return 0 == OCI_ObjectGetUInt64(nullptr, OTEXT("x")); } },
            { "ObjectGetUnsignedInt", [] { return 0 == OCI_ObjectGetUInt32(nullptr, OTEXT("x")); } },
            { "ObjectGetUnsignedShort", [] { return 0 == OCI_ObjectGetUInt16(nullptr, OTEXT("x")); } },
            { "ObjectIsNull", [] { return !OCI_ObjectIsNull(nullptr, OTEXT("x")); } },
            { "ObjectSetBigInt", [] { return !OCI_ObjectSetInt64(nullptr, OTEXT("x"), 1); } },
            { "ObjectSetBoolean", [] { return !OCI_ObjectSetBoolean(nullptr, OTEXT("x"), FALSE); } },
            { "ObjectSetColl", [] { return !OCI_ObjectSetColl(nullptr, OTEXT("x"), nullptr); } },
            { "ObjectSetDate", [] { return !OCI_ObjectSetDate(nullptr, OTEXT("x"), nullptr); } },
            { "ObjectSetDouble", [] { return !OCI_ObjectSetDouble(nullptr, OTEXT("x"), 1); } },
            { "ObjectSetFile", [] { return !OCI_ObjectSetFile(nullptr, OTEXT("x"), nullptr); } },
            { "ObjectSetFloat", [] { return !OCI_ObjectSetFloat(nullptr, OTEXT("x"), 1); } },
            { "ObjectSetInt", [] { return !OCI_ObjectSetInt32(nullptr, OTEXT("x"), 1); } },
            { "ObjectSetInterval", [] { return !OCI_ObjectSetInterval(nullptr, OTEXT("x"), nullptr); } },
            { "ObjectSetLob", [] { return !OCI_ObjectSetLob(nullptr, OTEXT("x"), nullptr); } },
            { "ObjectSetNull", [] { return !OCI_ObjectSetNull(nullptr, OTEXT("x")); } },
            { "ObjectSetNumber", [] { return !OCI_ObjectSetNumber(nullptr, OTEXT("x"), nullptr); } },
            { "ObjectSetObject", [] { return !OCI_ObjectSetObject(nullptr, OTEXT("x"), nullptr); } },
            { "ObjectSetRaw", [] { return !OCI_ObjectSetRaw(nullptr, OTEXT("x"), NullArgBuffer(), 1); } },
            { "ObjectSetRef", [] { return !OCI_ObjectSetReference(nullptr, OTEXT("x"), nullptr); } },
            { "ObjectSetShort", [] { return !OCI_ObjectSetInt16(nullptr, OTEXT("x"), 1); } },
            { "ObjectSetString", [] { return !OCI_ObjectSetString(nullptr, OTEXT("x"), OTEXT("x")); } },
            { "ObjectSetTimestamp", [] { return !OCI_ObjectSetTimestamp(nullptr, OTEXT("x"), nullptr); } },
            { "ObjectSetUnsignedBigInt", [] { return !OCI_ObjectSetUInt64(nullptr, OTEXT("x"), 1); } },
            { "ObjectSetUnsignedInt", [] { return !OCI_ObjectSetUInt32(nullptr, OTEXT("x"), 1); } },
            { "ObjectSetUnsignedShort", [] { return !OCI_ObjectSetUInt16(nullptr, OTEXT("x"), 1); } },
            { "ObjectToText", [] { return !OCI_ObjectToText(nullptr, NullArgOut<OCI_UInt32>(), NullArgText()); } },
        }),
        &NullArgCaseName);

}