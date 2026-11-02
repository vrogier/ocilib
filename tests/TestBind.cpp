#include "ocilib_tests.h"

#include "test_internals.h"

namespace TestCApi
{
    TEST(TestBind, BasicBind)
    {
        ExecDML(OTEXT("create table TestBind_Table (id int, name varchar2(50))"));
        ExecDML(OTEXT("truncate table TestBind_Table"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        OCI_Int32 id = 42;
        otext name[64] = OTEXT("hello");

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into TestBind_Table values (:id, :name)")));
        ASSERT_NE(nullptr, OCI_BindInt(stmt, OTEXT(":id"), &id));
        ASSERT_NE(nullptr, OCI_BindString(stmt, OTEXT(":name"), name, 50));
        ASSERT_TRUE(OCI_Execute(stmt));

        const auto bnd_id = OCI_GetBind(stmt, 1);
        const auto bnd_name = OCI_GetBindByName(stmt, OTEXT(":name"));
        ASSERT_NE(nullptr, bnd_id);
        ASSERT_NE(nullptr, bnd_name);

        ASSERT_EQ(ostring(OTEXT(":id")), ostring(OCI_BindGetName(bnd_id)));
        ASSERT_EQ(ostring(OTEXT(":name")), ostring(OCI_BindGetName(bnd_name)));
        ASSERT_EQ(stmt, OCI_BindGetStatement(bnd_id));

        ASSERT_EQ((OCI_UInt32) OCI_BDM_IN_OUT, OCI_BindGetDirection(bnd_id));
        ASSERT_EQ((OCI_UInt32) OCI_CDT_NUMERIC, OCI_BindGetType(bnd_id));
        ASSERT_EQ((OCI_UInt32) OCI_CDT_TEXT, OCI_BindGetType(bnd_name));
        ASSERT_EQ(1u, OCI_BindGetDataCount(bnd_id));

        ASSERT_NE(nullptr, OCI_BindGetData(bnd_id));
        ASSERT_NE(nullptr, OCI_BindGetData(bnd_name));
        ASSERT_EQ(id, *static_cast<OCI_Int32*>(OCI_BindGetData(bnd_id)));
        ASSERT_EQ(ostring(name), ostring(static_cast<otext*>(OCI_BindGetData(bnd_name))));

        ASSERT_TRUE(OCI_StatementFree(stmt));

        stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);
        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select name from TestBind_Table where id = 42")));

        const auto rset = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rset);
        ASSERT_TRUE(OCI_FetchNext(rset));
        ASSERT_EQ(ostring(OTEXT("hello")), ostring(OCI_GetString(rset, 1)));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestBind_Table"));
    }

    TEST(TestBind, OutBindsAndDirection)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        OCI_Int32 iout = 0;
        otext sout[64] = OTEXT("");
        OCI_Date* dout = OCI_DateCreate(env);
        ASSERT_NE(nullptr, dout);

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("BEGIN :i := 123; :s := 'hello'; :d := TO_DATE('2023-06-15', 'YYYY-MM-DD'); END;")));

        ASSERT_NE(nullptr, OCI_BindInt(stmt, OTEXT(":i"), &iout));
        ASSERT_NE(nullptr, OCI_BindString(stmt, OTEXT(":s"), sout, 50));
        ASSERT_NE(nullptr, OCI_BindDate(stmt, OTEXT(":d"), dout));
        const auto bnd_i = OCI_GetBindByName(stmt, OTEXT(":i"));
        const auto bnd_s = OCI_GetBindByName(stmt, OTEXT(":s"));
        const auto bnd_d = OCI_GetBindByName(stmt, OTEXT(":d"));
        ASSERT_NE(nullptr, bnd_i);
        ASSERT_NE(nullptr, bnd_s);
        ASSERT_NE(nullptr, bnd_d);

        ASSERT_TRUE(OCI_BindSetDirection(bnd_i, OCI_BDM_OUT));
        ASSERT_TRUE(OCI_BindSetDirection(bnd_s, OCI_BDM_OUT));
        ASSERT_TRUE(OCI_BindSetDirection(bnd_d, OCI_BDM_OUT));

        ASSERT_EQ((OCI_UInt32) OCI_BDM_OUT, OCI_BindGetDirection(bnd_i));
        ASSERT_EQ((OCI_UInt32) OCI_BDM_OUT, OCI_BindGetDirection(bnd_s));
        ASSERT_EQ((OCI_UInt32) OCI_CDT_NUMERIC, OCI_BindGetType(bnd_i));
        ASSERT_EQ((OCI_UInt32) OCI_CDT_TEXT, OCI_BindGetType(bnd_s));
        ASSERT_EQ((OCI_UInt32) OCI_CDT_DATETIME, OCI_BindGetType(bnd_d));
        ASSERT_EQ(1u, OCI_BindGetDataCount(bnd_i));
        ASSERT_EQ(stmt, OCI_BindGetStatement(bnd_i));
        ASSERT_EQ(static_cast<void*>(&iout), OCI_BindGetData(bnd_i));

        ASSERT_TRUE(OCI_Execute(stmt));

        ASSERT_EQ(123, iout);
        ASSERT_EQ(ostring(OTEXT("hello")), ostring(sout));

        OCI_Int32 year = 0, month = 0, day = 0;
        ASSERT_TRUE(OCI_DateGetDate(dout, &year, &month, &day));
        ASSERT_EQ(2023, year);
        ASSERT_EQ(6, month);
        ASSERT_EQ(15, day);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_DateFree(dout));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

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

    TEST(TestBind, MultipleDataTypes)
    {
        ExecDML(OTEXT("create type TestBind_ObjType as object(code int, name varchar2(30))"));
        ExecDML(OTEXT("create table TestBind_Types (n number, s varchar2(50), d date, r raw(20), l clob, o TestBind_ObjType)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        const auto number = OCI_NumberCreate(env);
        OCI_Int32 num = 42;
        ASSERT_TRUE(OCI_NumberSetValue(number, OCI_NUM_INT, &num));

        otext str[64] = OTEXT("typed bind");

        OCI_Date* date = OCI_DateCreate(env);
        ASSERT_NE(nullptr, date);
        ASSERT_TRUE(OCI_DateSetDateTime(date, 2024, 1, 2, 3, 4, 5));

        unsigned char raw[20] = { 0x00, 0x01, 0x02, 0x03, 0x04 };

        OCI_Lob* lob = OCI_LobCreate(conn, OCI_CLOB);
        ASSERT_NE(nullptr, lob);
        OCI_UInt32 lob_cc = (OCI_UInt32)ostrlen(OTEXT("clob data"));
        OCI_UInt32 lob_bc = 0;
        ASSERT_TRUE(OCI_LobWrite(lob, (void*)OTEXT("clob data"), &lob_cc, &lob_bc));
        ASSERT_EQ((OCI_UInt32) 9, lob_cc);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestBind_ObjType"), OCI_TIF_TYPE);
        OCI_Object* obj = OCI_ObjectCreate(typinf);
        ASSERT_NE(nullptr, obj);
        ASSERT_TRUE(OCI_ObjectSetInt(obj, OTEXT("CODE"), 7));
        ASSERT_TRUE(OCI_ObjectSetString(obj, OTEXT("NAME"), OTEXT("obj")));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into TestBind_Types values (:n, :s, :d, :r, :l, :o)")));

        ASSERT_NE(nullptr, OCI_BindNumber(stmt, OTEXT(":n"), number));
        ASSERT_NE(nullptr, OCI_BindString(stmt, OTEXT(":s"), str, 50));
        ASSERT_NE(nullptr, OCI_BindDate(stmt, OTEXT(":d"), date));
        ASSERT_NE(nullptr, OCI_BindRaw(stmt, OTEXT(":r"), raw, 20));
        ASSERT_NE(nullptr, OCI_BindLob(stmt, OTEXT(":l"), lob));
        ASSERT_NE(nullptr, OCI_BindObject(stmt, OTEXT(":o"), obj));

        const auto bnd_n = OCI_GetBindByName(stmt, OTEXT(":n"));
        const auto bnd_s = OCI_GetBindByName(stmt, OTEXT(":s"));
        const auto bnd_d = OCI_GetBindByName(stmt, OTEXT(":d"));
        const auto bnd_r = OCI_GetBindByName(stmt, OTEXT(":r"));
        const auto bnd_l = OCI_GetBindByName(stmt, OTEXT(":l"));
        const auto bnd_o = OCI_GetBindByName(stmt, OTEXT(":o"));

        ASSERT_NE(nullptr, bnd_n);
        ASSERT_NE(nullptr, bnd_s);
        ASSERT_NE(nullptr, bnd_d);
        ASSERT_NE(nullptr, bnd_r);
        ASSERT_NE(nullptr, bnd_l);
        ASSERT_NE(nullptr, bnd_o);

        ASSERT_EQ((OCI_UInt32) OCI_NUM_NUMBER, OCI_BindGetSubtype(bnd_n));
        ASSERT_EQ((OCI_UInt32) OCI_UNKNOWN, OCI_BindGetSubtype(bnd_s));
        ASSERT_EQ(static_cast<void*>(number), OCI_BindGetData(bnd_n));
        ASSERT_EQ(static_cast<void*>(date), OCI_BindGetData(bnd_d));
        ASSERT_EQ(static_cast<void*>(lob), OCI_BindGetData(bnd_l));
        ASSERT_EQ(static_cast<void*>(obj), OCI_BindGetData(bnd_o));
        ASSERT_EQ((OCI_UInt32) OCI_BAM_EXTERNAL, OCI_BindGetAllocationMode(bnd_n));

        ASSERT_TRUE(OCI_BindSetDataSize(bnd_r, 5));
        ASSERT_EQ(5u, OCI_BindGetDataSize(bnd_r));

        ASSERT_TRUE(OCI_Execute(stmt));

        OCI_Int32 count = 0;
        ASSERT_TRUE(OCI_Immediate(conn, OTEXT("select count(*) from TestBind_Types"), OCI_ARG_INT, &count));
        ASSERT_EQ(1, count);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_NumberFree(number));
        ASSERT_TRUE(OCI_DateFree(date));
        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestBind_Types"));
        ExecDML(OTEXT("drop type TestBind_ObjType"));
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

    TEST(TestBind, InternalAllocRegisterBinds)
    {
        ExecDML(OTEXT("create or replace type TestBind_RegObj as object(code int, name varchar2(30))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestBind_RegObj"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        ASSERT_TRUE(OCI_SetBindAllocation(stmt, OCI_BAM_INTERNAL));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT(
            "BEGIN :i := :i; :bi := :bi; :n := :n; :s := :s; :r := :r; :db := :db; "
            ":f := :f; :d := :d; :ts := :ts; :iv := :iv; :d2 := :d2; END;")));

        /* NULL data input bind in internal mode: covers the non-VNU numeric arm */

        ASSERT_NE(nullptr, OCI_BindDouble(stmt, OTEXT(":d2"), NULL));
        ASSERT_EQ(1u, OCI_GetBindIndex(stmt, OTEXT(":d2")));

        ASSERT_TRUE(OCI_RegisterInt(stmt, OTEXT(":i")));
        ASSERT_TRUE(OCI_RegisterBigInt(stmt, OTEXT(":bi")));
        ASSERT_TRUE(OCI_RegisterNumber(stmt, OTEXT(":n")));
        ASSERT_TRUE(OCI_RegisterString(stmt, OTEXT(":s"), 20));
        ASSERT_TRUE(OCI_RegisterRaw(stmt, OTEXT(":r"), 16));
        ASSERT_TRUE(OCI_RegisterDouble(stmt, OTEXT(":db")));
        ASSERT_TRUE(OCI_RegisterFloat(stmt, OTEXT(":f")));
        ASSERT_TRUE(OCI_RegisterDate(stmt, OTEXT(":d")));
        ASSERT_TRUE(OCI_RegisterTimestamp(stmt, OTEXT(":ts"), OCI_TIMESTAMP));
        ASSERT_TRUE(OCI_RegisterInterval(stmt, OTEXT(":iv"), OCI_INTERVAL_DS));

        /* registered binds are stored as negative indexes in the map */

        ASSERT_EQ(1u, OCI_GetBindIndex(stmt, OTEXT(":i")));
        ASSERT_EQ(4u, OCI_GetBindIndex(stmt, OTEXT(":s")));
        ASSERT_EQ(10u, OCI_GetBindIndex(stmt, OTEXT(":iv")));
        ASSERT_EQ(0u, OCI_GetBindIndex(stmt, OTEXT(":missing")));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop type TestBind_RegObj"));
    }

    TEST(TestBind, InternalAllocRegisterComplexBinds)
    {
        ExecDML(OTEXT("create or replace type TestBind_RegObj2 as object(code int)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestBind_RegObj2"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);

        ASSERT_TRUE(OCI_SetBindAllocation(stmt, OCI_BAM_INTERNAL));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT(
            "BEGIN :cl := :cl; :fl := :fl; :o := :o; :rf := :rf; :v := :v; :j := :j; END;")));

        ASSERT_TRUE(OCI_RegisterLob(stmt, OTEXT(":cl"), OCI_CLOB));
        ASSERT_TRUE(OCI_RegisterFile(stmt, OTEXT(":fl"), OCI_BFILE));
        ASSERT_TRUE(OCI_RegisterObject(stmt, OTEXT(":o"), typinf));
        ASSERT_TRUE(OCI_RegisterRef(stmt, OTEXT(":rf"), typinf));
        ASSERT_TRUE(OCI_RegisterVector(stmt, OTEXT(":v")));
        ASSERT_TRUE(OCI_RegisterJson(stmt, OTEXT(":j")));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop type TestBind_RegObj2"));
    }

    TEST(TestBind, InternalAllocArrayBinds)
    {
        ExecDML(OTEXT("create or replace type TestBind_ArrObj as object(code int)"));
        ExecDML(OTEXT("create or replace type TestBind_ArrNt as table of number"));
        ExecDML(OTEXT("create table TestBind_Arr (id number, nn number, bi number, s varchar2(30), r raw(16), d date, ts timestamp, iv interval day to second, bl blob, o TestBind_ArrObj, c TestBind_ArrNt, rf ref TestBind_ArrObj, fl bfile, v vector, j json) nested table c store as TestBind_ArrNtt"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf_obj = OCI_TypeInfoGet(conn, OTEXT("TestBind_ArrObj"), OCI_TIF_TYPE);
        const auto typinf_nt  = OCI_TypeInfoGet(conn, OTEXT("TestBind_ArrNt"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf_obj);
        ASSERT_NE(nullptr, typinf_nt);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_SetBindAllocation(stmt, OCI_BAM_INTERNAL));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT(
            "insert into TestBind_Arr values(:n, :nn, :bi, :s, :r, :d, :ts, :iv, :bl, :o, :c, :rf, :fl, :v, :j)")));
        ASSERT_TRUE(OCI_BindArraySetSize(stmt, 3));

        ASSERT_NE(nullptr, OCI_BindArrayOfInts(stmt, OTEXT(":n"), NULL, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfNumbers(stmt, OTEXT(":nn"), NULL, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfBigInts(stmt, OTEXT(":bi"), NULL, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfStrings(stmt, OTEXT(":s"), NULL, 20, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfRaws(stmt, OTEXT(":r"), NULL, 16, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfDates(stmt, OTEXT(":d"), NULL, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfTimestamps(stmt, OTEXT(":ts"), NULL, OCI_TIMESTAMP, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfIntervals(stmt, OTEXT(":iv"), NULL, OCI_INTERVAL_DS, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfLobs(stmt, OTEXT(":bl"), NULL, OCI_BLOB, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfObjects(stmt, OTEXT(":o"), NULL, typinf_obj, 0));
        ASSERT_NE(nullptr, OCI_BindArrayOfJsons(stmt, OTEXT(":j"), NULL, 0));

        /* NULL data array binds for types whose public API forbids NULL */

        OCI_TestBindCreateNull(stmt, OTEXT(":c"),  OCI_TEST_BIND_COLL_ARRAY,   typinf_nt,  0);
        OCI_TestBindCreateNull(stmt, OTEXT(":rf"), OCI_TEST_BIND_REF_ARRAY,    typinf_obj, 0);
        OCI_TestBindCreateNull(stmt, OTEXT(":fl"), OCI_TEST_BIND_FILE_ARRAY,   NULL,       0);
        OCI_TestBindCreateNull(stmt, OTEXT(":v"),  OCI_TEST_BIND_VECTOR_ARRAY, NULL,       0);

        ASSERT_TRUE(OCI_StatementFree(stmt));

        /* scalar collection bind: no public API can create it with NULL data */

        const auto stmt2 = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt2);

        ASSERT_TRUE(OCI_SetBindAllocation(stmt2, OCI_BAM_INTERNAL));
        ASSERT_TRUE(OCI_Prepare(stmt2, OTEXT("insert into TestBind_Arr (c) values(:c)")));

        OCI_TestBindCreateNull(stmt2, OTEXT(":c"), OCI_TEST_BIND_COLL_SCALAR, typinf_nt, 0);

        ASSERT_TRUE(OCI_StatementFree(stmt2));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf_obj));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf_nt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestBind_Arr"));
        ExecDML(OTEXT("drop type TestBind_ArrNt"));
        ExecDML(OTEXT("drop type TestBind_ArrObj"));
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
        ASSERT_EQ(1u, OCI_GetBindIndex(stmt, OTEXT(":x")));

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

        const auto fresh_size = OCI_BindGetDataSizeAtPos(bnd, 1);
        ASSERT_EQ(20u, fresh_size);

        /* setting the data size to the full buffer size hits the text padding branch */

        const auto buf_size = OCI_TestBindBufferSize(bnd);
        ASSERT_TRUE(OCI_BindSetDataSizeAtPos(bnd, 1, buf_size));
        ASSERT_EQ(buf_size + OCI_TestBindDbTextSize(), OCI_BindGetDataSizeAtPos(bnd, 1));

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
}