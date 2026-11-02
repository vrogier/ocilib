#include "ocilib_tests.h"
#include "test_internals.h"

namespace TestCApi
{
    TEST(TestResultset, FetchValuesAndColumns)
    {
        ExecDML(OTEXT("drop table TestResultsetData"));
        ExecDML(OTEXT("create table TestResultsetData (id number, name varchar2(50))"));
        ExecDML(OTEXT("insert into TestResultsetData values (1, 'one')"));
        ExecDML(OTEXT("insert into TestResultsetData values (2, 'two')"));
        ExecDML(OTEXT("insert into TestResultsetData values (3, 'three')"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select id, name from TestResultsetData order by id")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_EQ(2u, OCI_GetColumnCount(rs));

        const auto col1 = OCI_GetColumn(rs, 1);
        ASSERT_NE(nullptr, col1);
        ASSERT_EQ(ostring(OTEXT("ID")), ToUpper(OCI_ColumnGetName(col1)));
        ASSERT_EQ(OCI_CDT_NUMERIC, OCI_ColumnGetType(col1));

        const auto col2 = OCI_GetColumnByName(rs, OTEXT("NAME"));
        ASSERT_NE(nullptr, col2);
        ASSERT_EQ(ostring(OTEXT("NAME")), ToUpper(OCI_ColumnGetName(col2)));
        ASSERT_EQ(OCI_CDT_TEXT, OCI_ColumnGetType(col2));

        ASSERT_EQ(stmt, OCI_ResultsetGetStatement(rs));

        OCI_Int32 row = 0;
        while (OCI_FetchNext(rs))
        {
            ++row;
            ASSERT_EQ(row, OCI_GetInt(rs, 1));
            ASSERT_EQ(ostring(OCI_GetString(rs, 2)), ostring(OCI_GetStringByName(rs, OTEXT("NAME"))));

            if (row == 1)
            {
                ASSERT_EQ(ostring(OTEXT("one")), ostring(OCI_GetString(rs, 2)));
            }
            else if (row == 2)
            {
                ASSERT_EQ(ostring(OTEXT("two")), ostring(OCI_GetString(rs, 2)));
            }
            else if (row == 3)
            {
                ASSERT_EQ(ostring(OTEXT("three")), ostring(OCI_GetString(rs, 2)));
            }
        }

        ASSERT_EQ(3, row);
        ASSERT_EQ(3u, OCI_GetRowCount(rs));
        ASSERT_EQ(3u, OCI_GetCurrentRow(rs));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestResultsetData"));
    }

    TEST(TestResultset, ScrollableFetch)
    {
        ExecDML(OTEXT("drop table TestResultsetScroll"));
        ExecDML(OTEXT("create table TestResultsetScroll (id number)"));
        ExecDML(OTEXT("insert into TestResultsetScroll values (10)"));
        ExecDML(OTEXT("insert into TestResultsetScroll values (20)"));
        ExecDML(OTEXT("insert into TestResultsetScroll values (30)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_SetFetchMode(stmt, OCI_SFM_SCROLLABLE));
        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select id from TestResultsetScroll order by id")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_TRUE(OCI_FetchFirst(rs));
        ASSERT_EQ(1u, OCI_GetCurrentRow(rs));
        ASSERT_EQ(10, OCI_GetInt(rs, 1));

        ASSERT_TRUE(OCI_FetchLast(rs));
        ASSERT_EQ(3u, OCI_GetCurrentRow(rs));
        ASSERT_EQ(30, OCI_GetInt(rs, 1));

        ASSERT_TRUE(OCI_FetchSeek(rs, OCI_SFD_ABSOLUTE, 2));
        ASSERT_EQ(2u, OCI_GetCurrentRow(rs));
        ASSERT_EQ(20, OCI_GetInt(rs, 1));

        ASSERT_TRUE(OCI_FetchPrev(rs));
        ASSERT_EQ(1u, OCI_GetCurrentRow(rs));

        ASSERT_TRUE(OCI_ReleaseResultsets(stmt));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestResultsetScroll"));
    }

    TEST(TestResultset, GetDataAndNull)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select 1 as n, 'x' as s, cast(null as number) as nn from dual")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_EQ(3u, OCI_GetColumnCount(rs));

        const auto col = OCI_GetColumnByName(rs, OTEXT("N"));
        ASSERT_NE(nullptr, col);
        ASSERT_EQ(OCI_CDT_NUMERIC, OCI_ColumnGetType(col));
        ASSERT_NE(nullptr, OCI_ColumnGetName(col));

        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(1, OCI_GetInt(rs, 1));
        ASSERT_EQ(1, OCI_GetIntByName(rs, OTEXT("N")));
        ASSERT_EQ(ostring(OTEXT("x")), ostring(OCI_GetString(rs, 2)));
        ASSERT_TRUE(OCI_IsNull(rs, 3));
        ASSERT_EQ(nullptr, OCI_GetString(rs, 3));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestResultset, FetchTypes)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select 1 as n, to_date('2020-01-01', 'YYYY-MM-DD') as d, cast(3.14 as number(3,2)) as f from dual")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        ASSERT_EQ(1, OCI_GetInt(rs, 1));
        ASSERT_NE(nullptr, OCI_GetDate(rs, 2));
        ASSERT_GT(OCI_GetDouble(rs, 3), 0.0);
        ASSERT_EQ(ostring(OTEXT("1")), ostring(OCI_GetString(rs, 1)));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestResultset, FetchNativeCdt)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select cast(1 as number(4)) as s, cast(123456789012 as number(19)) as b, cast(1.5 as binary_float) as f, UTL_RAW.CAST_TO_RAW('AB') as r, to_clob('x') as c, 42 as n from dual")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        ASSERT_EQ(1, OCI_GetShort(rs, 1));
        ASSERT_NE(0, OCI_GetBigInt(rs, 2));
        ASSERT_GT(OCI_GetFloat(rs, 3), 0.0f);
        ASSERT_NE(nullptr, OCI_GetString(rs, 4));
        ASSERT_NE(nullptr, OCI_GetLob(rs, 5));
        ASSERT_NE(nullptr, OCI_GetNumber(rs, 6));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestResultset, FetchIntervalTimestampAndRowid)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select interval '1 12' day to hour as iv, to_timestamp('2020-01-01', 'YYYY-MM-DD') as ts, rowid as rid from dual")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        ASSERT_NE(nullptr, OCI_GetInterval(rs, 1));
        ASSERT_NE(nullptr, OCI_GetTimestamp(rs, 2));
        ASSERT_NE(nullptr, OCI_GetString(rs, 3));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestResultset, FetchNestedCursor)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select cursor(select 1 as n from dual) as cur from dual")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto sub = OCI_GetStatement(rs, 1);
        ASSERT_NE(nullptr, sub);
        const auto subrs = OCI_GetResultset(sub);
        ASSERT_NE(nullptr, subrs);
        ASSERT_TRUE(OCI_FetchNext(subrs));
        ASSERT_EQ(1, OCI_GetInt(subrs, 1));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestResultset, FetchStruct)
    {
        ExecDML(OTEXT("drop table TestResultsetStruct"));
        ExecDML(OTEXT("create table TestResultsetStruct (id number, name varchar2(50))"));
        ExecDML(OTEXT("insert into TestResultsetStruct values (1, 'one')"));
        ExecDML(OTEXT("insert into TestResultsetStruct values (2, 'two')"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select id, name from TestResultsetStruct order by id")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_TRUE(OCI_SetStructNumericType(rs, 1, OCI_NUM_INT));
        ASSERT_TRUE(OCI_SetStructNumericTypeByName(rs, OTEXT("ID"), OCI_NUM_INT));

        struct Row { OCI_Int32 id; otext* name; };
        struct RowInd { OCI_Boolean id; OCI_Boolean name; };

        while (OCI_FetchNext(rs))
        {
            Row row{};
            RowInd ind{};

            ASSERT_TRUE(OCI_GetStruct(rs, &row, &ind));

            ASSERT_TRUE(ind.id);
            ASSERT_TRUE(ind.name);
            ASSERT_NE(nullptr, row.name);
        }

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestResultsetStruct"));
    }

    TEST(TestResultset, GetByName)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select 1 as id, 'x' as name, to_date('2020-01-01', 'YYYY-MM-DD') as d, 1.5 as f, to_clob('x') as c, to_timestamp('2020-01-01', 'YYYY-MM-DD') as ts, interval '1' day as iv from dual")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        ASSERT_EQ(1, OCI_GetIntByName(rs, OTEXT("ID")));
        ASSERT_EQ(1u, OCI_GetUnsignedIntByName(rs, OTEXT("ID")));
        ASSERT_EQ(1, OCI_GetBigIntByName(rs, OTEXT("ID")));
        ASSERT_EQ(1, OCI_GetShortByName(rs, OTEXT("ID")));
        ASSERT_EQ(1u, OCI_GetUnsignedShortByName(rs, OTEXT("ID")));
        ASSERT_EQ(ostring(OTEXT("x")), ostring(OCI_GetStringByName(rs, OTEXT("NAME"))));
        ASSERT_NE(nullptr, OCI_GetDateByName(rs, OTEXT("D")));
        ASSERT_GT(OCI_GetDoubleByName(rs, OTEXT("F")), 0.0);
        ASSERT_GT(OCI_GetFloatByName(rs, OTEXT("F")), 0.0f);
        ASSERT_NE(nullptr, OCI_GetNumberByName(rs, OTEXT("ID")));
        ASSERT_NE(nullptr, OCI_GetLobByName(rs, OTEXT("C")));
        ASSERT_NE(nullptr, OCI_GetTimestampByName(rs, OTEXT("TS")));
        ASSERT_NE(nullptr, OCI_GetIntervalByName(rs, OTEXT("IV")));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestResultset, GetStructAllTypes)
    {
        ExecDML(OTEXT("drop table TestRsStruct"));
        ExecDML(OTEXT("drop type TestRsStructColl"));
        ExecDML(OTEXT("drop type TestRsStructObj"));
        ExecDML(OTEXT("create type TestRsStructObj as object(code int, name varchar2(30))"));
        ExecDML(OTEXT("create type TestRsStructColl as varray(4) of number"));
        ExecDML(OTEXT("create table TestRsStruct of TestRsStructObj"));
        ExecDML(OTEXT("insert into TestRsStruct values (TestRsStructObj(7, 'seven'))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt,
            OTEXT("select 1 as num_col, 'txt' as txt_col, hextoraw('AABB') as raw_col, "
                  "sysdate as date_col, cursor(select 1 from dual) as cur_col, "
                  "to_clob('x') as lob_col, bfilename('FAKE_DIR', 'f.txt') as file_col, "
                  "systimestamp as ts_col, interval '1 02:03:04' day to second as iv_col, "
                  "value(t) as obj_col, TestRsStructColl(1, 2) as coll_col, ref(t) as ref_col, "
                  "xmltype('<a>1</a>') as xml_col, json('{\"k\":1}') as json_col "
                  "from dual, TestRsStruct t")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        // the row struct is filled column by column; a plain buffer is enough
        unsigned char rowbuf[8192];
        OCI_Boolean inds[32];
        memset(rowbuf, 0, sizeof(rowbuf));
        memset(inds, 0, sizeof(inds));

        ASSERT_TRUE(OCI_GetStruct(rs, rowbuf, inds));
        for (OCI_UInt32 i = 0; i < 14; i++)
        {
            ASSERT_TRUE(inds[i]);
        }

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestRsStruct"));
        ExecDML(OTEXT("drop type TestRsStructColl"));
        ExecDML(OTEXT("drop type TestRsStructObj"));
    }

    /* --- OCI_GetString conversion of each fetched column type --- */

    class TestResultsetStringCvt : public ::testing::TestWithParam<OCI_UInt32>
    {
    public:

        /* public: TEST_P fixtures require access in gtest 1.8 */

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("drop table TestRsCvt"));
            ExecDML(OTEXT("drop type TestRsCvtColl"));
            ExecDML(OTEXT("drop type TestRsCvtObj"));
            ExecDML(OTEXT("create type TestRsCvtObj as object(code int, name varchar2(30))"));
            ExecDML(OTEXT("create type TestRsCvtColl as varray(4) of number"));
            ExecDML(OTEXT("create table TestRsCvt of TestRsCvtObj"));
            ExecDML(OTEXT("insert into TestRsCvt values (TestRsCvtObj(7, 'seven'))"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestRsCvt"));
            ExecDML(OTEXT("drop type TestRsCvtColl"));
            ExecDML(OTEXT("drop type TestRsCvtObj"));
        }

    protected:

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);

            ASSERT_TRUE(OCI_ExecuteStmt(stmt,
                OTEXT("select rowid as rid_col, 42 as num_col, sysdate as date_col, "
                      "systimestamp as ts_col, interval '1 02:03:04' day to second as iv_col, "
                      "hextoraw('AABB') as raw_col, ref(t) as ref_col, "
                      "to_blob(hextoraw('0102')) as blob_col, to_clob('clobval') as clob_col, "
                      "bfilename('FAKE_DIR', 'f.txt') as file_col, "
                      "value(t) as obj_col, TestRsCvtColl(1, 2) as coll_col, "
                      "vector('[1.0,2.0,3.0]') as vec_col, json('{\"k\":1}') as json_col "
                      "from TestRsCvt t")));

            rs = OCI_GetResultset(stmt);
            ASSERT_NE(nullptr, rs);
            ASSERT_TRUE(OCI_FetchNext(rs));
        }

        void TearDown() override
        {
            OCI_StatementFree(stmt);
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
        OCI_Statement*  stmt{ nullptr };
        OCI_Resultset*  rs{ nullptr };
    };

    TEST_P(TestResultsetStringCvt, ConvertsToString)
    {
        ASSERT_NE(nullptr, OCI_GetString(rs, GetParam()));
    }

    INSTANTIATE_TEST_CASE_P(Columns, TestResultsetStringCvt,
        ::testing::Range(1u, 15u));

    /* --- LONG columns --- */

    class TestResultsetLong : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("drop table TestRsLong"));
            ExecDML(OTEXT("create table TestRsLong (l long)"));
            ExecDML(OTEXT("insert into TestRsLong values ('long value')"));

            ExecDML(OTEXT("drop table TestRsLongRaw"));
            ExecDML(OTEXT("create table TestRsLongRaw (l long raw)"));
            ExecDML(OTEXT("insert into TestRsLongRaw values (hextoraw('AABBCC'))"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestRsLong"));
            ExecDML(OTEXT("drop table TestRsLongRaw"));
        }

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);
        }

        void TearDown() override
        {
            OCI_StatementFree(stmt);
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
        OCI_Statement*  stmt{ nullptr };
    };

    TEST_F(TestResultsetLong, ImplicitModeMapsToText)
    {
        // implicit mode: LONG mapped to text with lens-based termination
        ASSERT_TRUE(OCI_SetLongMode(stmt, OCI_LONG_IMPLICIT));
        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select l from TestRsLong")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(ostring(OTEXT("long value")), ostring(OCI_GetString(rs, 1)));
        ASSERT_TRUE(OCI_FetchNext(rs) == FALSE);
    }

    TEST_F(TestResultsetLong, ExplicitModeFetchesLongHandle)
    {
        // explicit mode: LONG fetched as OCI_Long
        ASSERT_TRUE(OCI_SetLongMode(stmt, OCI_LONG_EXPLICIT));
        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select l from TestRsLong")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_NE(nullptr, OCI_GetString(rs, 1));
        ASSERT_NE(nullptr, OCI_GetLongByName(rs, OTEXT("L")));
    }

    TEST_F(TestResultsetLong, LongRawConvertsToHexString)
    {
        // LONG RAW -> hex conversion through GetString
        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select l from TestRsLongRaw")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_NE(nullptr, OCI_GetString(rs, 1));
    }

    /* --- by-name handle getters --- */

    class TestResultsetByName : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("drop table TestRsByName"));
            ExecDML(OTEXT("drop type TestRsByNameColl"));
            ExecDML(OTEXT("drop type TestRsByNameObj"));
            ExecDML(OTEXT("create type TestRsByNameObj as object(code int, name varchar2(30))"));
            ExecDML(OTEXT("create type TestRsByNameColl as varray(4) of number"));
            ExecDML(OTEXT("create table TestRsByName of TestRsByNameObj"));
            ExecDML(OTEXT("insert into TestRsByName values (TestRsByNameObj(7, 'seven'))"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestRsByName"));
            ExecDML(OTEXT("drop type TestRsByNameColl"));
            ExecDML(OTEXT("drop type TestRsByNameObj"));
        }

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);

            ASSERT_TRUE(OCI_ExecuteStmt(stmt,
                OTEXT("select 42 as num_col, 'sized' as txt_col, "
                      "value(t) as obj_col, TestRsByNameColl(1, 2) as coll_col, ref(t) as ref_col, "
                      "xmltype('<a>1</a>') as xml_col, vector('[1.0,2.0]') as vec_col, "
                      "cursor(select 1 from dual) as cur_col, "
                      "bfilename('FAKE_DIR', 'f.txt') as file_col "
                      "from dual, TestRsByName t")));

            rs = OCI_GetResultset(stmt);
            ASSERT_NE(nullptr, rs);
            ASSERT_TRUE(OCI_FetchNext(rs));
        }

        void TearDown() override
        {
            OCI_StatementFree(stmt);
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
        OCI_Statement*  stmt{ nullptr };
        OCI_Resultset*  rs{ nullptr };
    };

    TEST_F(TestResultsetByName, GetUnsignedBigInt)
    {
        ASSERT_EQ(42u, OCI_GetUnsignedBigInt(rs, 1));
    }

    TEST_F(TestResultsetByName, GetUnsignedBigIntByName)
    {
        ASSERT_EQ(42u, OCI_GetUnsignedBigIntByName(rs, OTEXT("NUM_COL")));
    }

    TEST_F(TestResultsetByName, GetObjectByName)
    {
        ASSERT_NE(nullptr, OCI_GetObjectByName(rs, OTEXT("OBJ_COL")));
    }

    TEST_F(TestResultsetByName, GetCollByName)
    {
        ASSERT_NE(nullptr, OCI_GetCollByName(rs, OTEXT("COLL_COL")));
    }

    TEST_F(TestResultsetByName, GetRefByName)
    {
        ASSERT_NE(nullptr, OCI_GetRefByName(rs, OTEXT("REF_COL")));
    }

    TEST_F(TestResultsetByName, GetXmlTypeByName)
    {
        ASSERT_NE(nullptr, OCI_GetXmlTypeByName(rs, OTEXT("XML_COL")));
    }

    TEST_F(TestResultsetByName, GetVectorByName)
    {
        ASSERT_NE(nullptr, OCI_GetVectorByName(rs, OTEXT("VEC_COL")));
    }

    TEST_F(TestResultsetByName, GetStatementByName)
    {
        ASSERT_NE(nullptr, OCI_GetStatementByName(rs, OTEXT("CUR_COL")));
    }

    TEST_F(TestResultsetByName, GetFile)
    {
        ASSERT_NE(nullptr, OCI_GetFile(rs, 9));
    }

    TEST_F(TestResultsetByName, GetFileByName)
    {
        ASSERT_NE(nullptr, OCI_GetFileByName(rs, OTEXT("FILE_COL")));
    }

    TEST_F(TestResultsetByName, GetDataSize)
    {
        ASSERT_EQ(5u, OCI_GetDataSize(rs, 2));
    }

    TEST_F(TestResultsetByName, GetDataSizeByName)
    {
        ASSERT_EQ(5u, OCI_GetDataSizeByName(rs, OTEXT("TXT_COL")));
    }

    TEST_F(TestResultsetByName, GetColumnIndex)
    {
        ASSERT_EQ(2u, OCI_GetColumnIndex(rs, OTEXT("TXT_COL")));
    }

    /* --- scrollable edge cases --- */

    class TestResultsetScroll : public ::testing::Test
    {
    protected:

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);

            // scrollable + cursor column: fetch size forced to 1
            ASSERT_TRUE(OCI_SetFetchMode(stmt, OCI_SFM_SCROLLABLE));
            ASSERT_TRUE(OCI_ExecuteStmt(stmt,
                OTEXT("select cursor(select 1 from dual) as c, id from (select 1 as id from dual union all select 2 from dual) order by id")));

            rs = OCI_GetResultset(stmt);
            ASSERT_NE(nullptr, rs);
        }

        void TearDown() override
        {
            OCI_ReleaseResultsets(stmt);
            OCI_StatementFree(stmt);
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
        OCI_Statement*  stmt{ nullptr };
        OCI_Resultset*  rs{ nullptr };
    };

    TEST_F(TestResultsetScroll, FetchNextThroughCursorColumn)
    {
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(2u, OCI_GetCurrentRow(rs));
    }

    TEST_F(TestResultsetScroll, RelativeSeekPastBeginning)
    {
        // relative seek past the beginning -> fails and sets bof
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_FALSE(OCI_FetchSeek(rs, OCI_SFD_RELATIVE, -10));
    }

    /* --- legacy fetch path and internal initialize --- */

    class TestResultsetLegacy : public ::testing::Test
    {
    protected:

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);

            ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select 1 as n from dual union all select 2 from dual order by n")));

            rs = OCI_GetResultset(stmt);
            ASSERT_NE(nullptr, rs);
        }

        void TearDown() override
        {
            OCI_StatementFree(stmt);
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
        OCI_Statement*  stmt{ nullptr };
        OCI_Resultset*  rs{ nullptr };
    };

    TEST_F(TestResultsetLegacy, FetchViaOciStmtFetch)
    {
        // force the legacy OCIStmtFetch() call instead of OCIStmtFetch2()
        const auto old_scroll = OCI_TestEnvSetScrollableCursors(conn, FALSE);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(1, OCI_GetInt(rs, 1));
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(2, OCI_GetInt(rs, 1));
        OCI_TestEnvSetScrollableCursors(conn, old_scroll);
    }

    TEST_F(TestResultsetLegacy, ResultsetInitialize)
    {
        // internal entry point (dead public path)
        ASSERT_TRUE(OCI_TestResultsetInitialize(rs));
        ASSERT_EQ(0u, OCI_GetCurrentRow(rs));
    }
}
