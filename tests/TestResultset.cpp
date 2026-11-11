#include "ocilib_tests.h"
#include "NullArgs.h"

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

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select id, name from TestResultsetData order by id")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_EQ(2u, OCI_ResultsetGetColumnCount(rs));

        const auto col1 = OCI_ResultsetGetColumn(rs, 0);
        ASSERT_NE(nullptr, col1);
        ASSERT_EQ(String(OTEXT("ID")), ToUpper(OCI_ColumnGetName(col1)));
        ASSERT_EQ(OCI_CDT_NUMERIC, OCI_ColumnGetType(col1));

        const auto col2 = OCI_ResultsetGetColumnByName(rs, OTEXT("NAME"));
        ASSERT_NE(nullptr, col2);
        ASSERT_EQ(String(OTEXT("NAME")), ToUpper(OCI_ColumnGetName(col2)));
        ASSERT_EQ(OCI_CDT_TEXT, OCI_ColumnGetType(col2));

        ASSERT_EQ(stmt, OCI_ResultsetGetStatement(rs));

        OCI_Int32 row = 0;
        while (OCI_ResultsetFetchNext(rs))
        {
            ++row;
            ASSERT_EQ(row, OCI_ResultsetGetInt32(rs, 0));
            ASSERT_EQ(String(OCI_ResultsetGetString(rs, 1)), String(OCI_ResultsetGetStringByName(rs, OTEXT("NAME"))));

            if (row == 1)
            {
                ASSERT_EQ(String(OTEXT("one")), String(OCI_ResultsetGetString(rs, 1)));
            }
            else if (row == 2)
            {
                ASSERT_EQ(String(OTEXT("two")), String(OCI_ResultsetGetString(rs, 1)));
            }
            else if (row == 3)
            {
                ASSERT_EQ(String(OTEXT("three")), String(OCI_ResultsetGetString(rs, 1)));
            }
        }

        ASSERT_EQ(3, row);
        ASSERT_EQ(3u, OCI_ResultsetGetRowCount(rs));
        ASSERT_EQ(2, OCI_ResultsetGetCurrentRow(rs));

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

        ASSERT_TRUE(OCI_StatementSetFetchMode(stmt, OCI_SFM_SCROLLABLE));
        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select id from TestResultsetScroll order by id")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_TRUE(OCI_ResultsetFetchFirst(rs));
        ASSERT_EQ(0, OCI_ResultsetGetCurrentRow(rs));
        ASSERT_EQ(10, OCI_ResultsetGetInt32(rs, 0));

        ASSERT_TRUE(OCI_ResultsetFetchLast(rs));
        ASSERT_EQ(2, OCI_ResultsetGetCurrentRow(rs));
        ASSERT_EQ(30, OCI_ResultsetGetInt32(rs, 0));

        ASSERT_TRUE(OCI_ResultsetFetchSeek(rs, OCI_SFD_ABSOLUTE, 1));
        ASSERT_EQ(1, OCI_ResultsetGetCurrentRow(rs));
        ASSERT_EQ(20, OCI_ResultsetGetInt32(rs, 0));

        ASSERT_TRUE(OCI_ResultsetFetchPrev(rs));
        ASSERT_EQ(0, OCI_ResultsetGetCurrentRow(rs));

        ASSERT_TRUE(OCI_StatementReleaseResultsets(stmt));
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

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select 1 as n, 'x' as s, cast(null as number) as nn from dual")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_EQ(3u, OCI_ResultsetGetColumnCount(rs));

        const auto col = OCI_ResultsetGetColumnByName(rs, OTEXT("N"));
        ASSERT_NE(nullptr, col);
        ASSERT_EQ(OCI_CDT_NUMERIC, OCI_ColumnGetType(col));
        ASSERT_NE(nullptr, OCI_ColumnGetName(col));

        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        ASSERT_EQ(1, OCI_ResultsetGetInt32(rs, 0));
        ASSERT_EQ(1, OCI_ResultsetGetInt32ByName(rs, OTEXT("N")));
        ASSERT_EQ(String(OTEXT("x")), String(OCI_ResultsetGetString(rs, 1)));
        ASSERT_TRUE(OCI_ResultsetIsColumnNull(rs, 2));
        ASSERT_EQ(nullptr, OCI_ResultsetGetString(rs, 2));

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

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select 1 as n, to_date('2020-01-01', 'YYYY-MM-DD') as d, cast(3.14 as number(3,2)) as f from dual")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        ASSERT_EQ(1, OCI_ResultsetGetInt32(rs, 0));
        ASSERT_NE(nullptr, OCI_ResultsetGetDate(rs, 1));
        ASSERT_GT(OCI_ResultsetGetDouble(rs, 2), 0.0);
        ASSERT_EQ(String(OTEXT("1")), String(OCI_ResultsetGetString(rs, 0)));

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

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select cast(1 as number(4)) as s, cast(123456789012 as number(19)) as b, cast(1.5 as binary_float) as f, UTL_RAW.CAST_TO_RAW('AB') as r, to_clob('x') as c, 42 as n from dual")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        ASSERT_EQ(1, OCI_ResultsetGetInt16(rs, 0));
        ASSERT_NE(0, OCI_ResultsetGetInt64(rs, 1));
        ASSERT_GT(OCI_ResultsetGetFloat(rs, 2), 0.0f);
        ASSERT_NE(nullptr, OCI_ResultsetGetString(rs, 3));
        ASSERT_NE(nullptr, OCI_ResultsetGetLob(rs, 4));
        ASSERT_NE(nullptr, OCI_ResultsetGetNumber(rs, 5));

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

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select interval '1 12' day to hour as iv, to_timestamp('2020-01-01', 'YYYY-MM-DD') as ts, rowid as rid from dual")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        ASSERT_NE(nullptr, OCI_ResultsetGetInterval(rs, 0));
        ASSERT_NE(nullptr, OCI_ResultsetGetTimestamp(rs, 1));
        ASSERT_NE(nullptr, OCI_ResultsetGetString(rs, 2));

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

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select cursor(select 1 as n from dual) as cur from dual")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        const auto sub = OCI_ResultsetGetCursor(rs, 0);
        ASSERT_NE(nullptr, sub);
        const auto subrs = OCI_StatementGetResultset(sub);
        ASSERT_NE(nullptr, subrs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(subrs));
        ASSERT_EQ(1, OCI_ResultsetGetInt32(subrs, 0));

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

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select id, name from TestResultsetStruct order by id")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_TRUE(OCI_ResultsetSetStructNumericType(rs, 0, OCI_NUM_INT32));
        ASSERT_TRUE(OCI_ResultsetSetStructNumericTypeByName(rs, OTEXT("ID"), OCI_NUM_INT32));

        struct Row { OCI_Int32 id; otext* name; };
        struct RowInd { OCI_Boolean id; OCI_Boolean name; };

        while (OCI_ResultsetFetchNext(rs))
        {
            Row row{};
            RowInd ind{};

            ASSERT_TRUE(OCI_ResultsetGetStruct(rs, &row, &ind));

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

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select 1 as id, 'x' as name, to_date('2020-01-01', 'YYYY-MM-DD') as d, 1.5 as f, to_clob('x') as c, to_timestamp('2020-01-01', 'YYYY-MM-DD') as ts, interval '1' day as iv from dual")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        ASSERT_EQ(1, OCI_ResultsetGetInt32ByName(rs, OTEXT("ID")));
        ASSERT_EQ(1u, OCI_ResultsetGetUInt32ByName(rs, OTEXT("ID")));
        ASSERT_EQ(1, OCI_ResultsetGetInt64ByName(rs, OTEXT("ID")));
        ASSERT_EQ(1, OCI_ResultsetGetInt16ByName(rs, OTEXT("ID")));
        ASSERT_EQ(1u, OCI_ResultsetGetUInt16ByName(rs, OTEXT("ID")));
        ASSERT_EQ(String(OTEXT("x")), String(OCI_ResultsetGetStringByName(rs, OTEXT("NAME"))));
        ASSERT_NE(nullptr, OCI_ResultsetGetDateByName(rs, OTEXT("D")));
        ASSERT_GT(OCI_ResultsetGetDoubleByName(rs, OTEXT("F")), 0.0);
        ASSERT_GT(OCI_ResultsetGetFloatByName(rs, OTEXT("F")), 0.0f);
        ASSERT_NE(nullptr, OCI_ResultsetGetNumberByName(rs, OTEXT("ID")));
        ASSERT_NE(nullptr, OCI_ResultsetGetLobByName(rs, OTEXT("C")));
        ASSERT_NE(nullptr, OCI_ResultsetGetTimestampByName(rs, OTEXT("TS")));
        ASSERT_NE(nullptr, OCI_ResultsetGetIntervalByName(rs, OTEXT("IV")));

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

        ASSERT_TRUE(OCI_StatementExecute(stmt,
            OTEXT("select 1 as num_col, 'txt' as txt_col, hextoraw('AABB') as raw_col, "
                  "sysdate as date_col, cursor(select 1 from dual) as cur_col, "
                  "to_clob('x') as lob_col, bfilename('FAKE_DIR', 'f.txt') as file_col, "
                  "systimestamp as ts_col, interval '1 02:03:04' day to second as iv_col, "
                  "value(t) as obj_col, TestRsStructColl(1, 2) as coll_col, ref(t) as ref_col, "
                  "xmltype('<a>1</a>') as xml_col, json('{\"k\":1}') as json_col "
                  "from dual, TestRsStruct t")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        // the row struct is filled column by column; a plain buffer is enough
        unsigned char rowbuf[8192];
        OCI_Boolean inds[32];
        memset(rowbuf, 0, sizeof(rowbuf));
        memset(inds, 0, sizeof(inds));

        ASSERT_TRUE(OCI_ResultsetGetStruct(rs, rowbuf, inds));
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

    /* --- OCI_ResultsetGetString conversion of each fetched column type --- */

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

            ASSERT_TRUE(OCI_StatementExecute(stmt,
                OTEXT("select rowid as rid_col, 42 as num_col, sysdate as date_col, "
                      "systimestamp as ts_col, interval '1 02:03:04' day to second as iv_col, "
                      "hextoraw('AABB') as raw_col, ref(t) as ref_col, "
                      "to_blob(hextoraw('0102')) as blob_col, to_clob('clobval') as clob_col, "
                      "bfilename('FAKE_DIR', 'f.txt') as file_col, "
                      "value(t) as obj_col, TestRsCvtColl(1, 2) as coll_col, "
                      "vector('[1.0,2.0,3.0]') as vec_col, json('{\"k\":1}') as json_col "
                      "from TestRsCvt t")));

            rs = OCI_StatementGetResultset(stmt);
            ASSERT_NE(nullptr, rs);
            ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
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
        ASSERT_NE(nullptr, OCI_ResultsetGetString(rs, GetParam()));
    }

    INSTANTIATE_TEST_CASE_P(Columns, TestResultsetStringCvt,
        ::testing::Range(0u, 14u));

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

    TEST_F(TestResultsetLong, ImplicitLongMode_FetchesAsText)
    {
        // implicit mode: LONG mapped to text with lens-based termination
        ASSERT_TRUE(OCI_StatementSetLongMode(stmt, OCI_LONG_IMPLICIT));
        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select l from TestRsLong")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        ASSERT_EQ(String(OTEXT("long value")), String(OCI_ResultsetGetString(rs, 0)));
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs) == FALSE);
    }

    TEST_F(TestResultsetLong, ExplicitLongMode_FetchesLongHandle)
    {
        // explicit mode: LONG fetched as OCI_Long
        ASSERT_TRUE(OCI_StatementSetLongMode(stmt, OCI_LONG_EXPLICIT));
        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select l from TestRsLong")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        ASSERT_NE(nullptr, OCI_ResultsetGetString(rs, 0));
        ASSERT_NE(nullptr, OCI_ResultsetGetLongByName(rs, OTEXT("L")));
    }

    TEST_F(TestResultsetLong, LongRawInImplicitMode_FetchesHexString)
    {
        // LONG RAW -> hex conversion through GetString
        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select l from TestRsLongRaw")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        ASSERT_NE(nullptr, OCI_ResultsetGetString(rs, 0));
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

            ASSERT_TRUE(OCI_StatementExecute(stmt,
                OTEXT("select 42 as num_col, 'sized' as txt_col, "
                      "value(t) as obj_col, TestRsByNameColl(1, 2) as coll_col, ref(t) as ref_col, "
                      "xmltype('<a>1</a>') as xml_col, vector('[1.0,2.0]') as vec_col, "
                      "cursor(select 1 from dual) as cur_col, "
                      "bfilename('FAKE_DIR', 'f.txt') as file_col "
                      "from dual, TestRsByName t")));

            rs = OCI_StatementGetResultset(stmt);
            ASSERT_NE(nullptr, rs);
            ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
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

    TEST_F(TestResultsetByName, GetUnsignedBigIntByIndex_ReturnsValue)
    {
        ASSERT_EQ(42u, OCI_ResultsetGetUInt64(rs, 0));
    }

    TEST_F(TestResultsetByName, GetUnsignedBigIntByName_ReturnsValue)
    {
        ASSERT_EQ(42u, OCI_ResultsetGetUInt64ByName(rs, OTEXT("NUM_COL")));
    }

    TEST_F(TestResultsetByName, GetObjectByName_ReturnsObject)
    {
        ASSERT_NE(nullptr, OCI_ResultsetGetObjectByName(rs, OTEXT("OBJ_COL")));
    }

    TEST_F(TestResultsetByName, GetCollByName_ReturnsColl)
    {
        ASSERT_NE(nullptr, OCI_ResultsetGetCollByName(rs, OTEXT("COLL_COL")));
    }

    TEST_F(TestResultsetByName, GetRefByName_ReturnsRef)
    {
        ASSERT_NE(nullptr, OCI_ResultsetGetReferenceByName(rs, OTEXT("REF_COL")));
    }

    TEST_F(TestResultsetByName, GetXmlTypeByName_ReturnsXml)
    {
        ASSERT_NE(nullptr, OCI_ResultsetGetXmlTypeByName(rs, OTEXT("XML_COL")));
    }

    TEST_F(TestResultsetByName, GetVectorByName_ReturnsVector)
    {
        ASSERT_NE(nullptr, OCI_ResultsetGetVectorByName(rs, OTEXT("VEC_COL")));
    }

    TEST_F(TestResultsetByName, GetStatementByName_ReturnsStmt)
    {
        ASSERT_NE(nullptr, OCI_ResultsetGetCursorByName(rs, OTEXT("CUR_COL")));
    }

    TEST_F(TestResultsetByName, GetFileByIndex_ReturnsFile)
    {
        ASSERT_NE(nullptr, OCI_ResultsetGetFile(rs, 8));
    }

    TEST_F(TestResultsetByName, GetFileByName_ReturnsFile)
    {
        ASSERT_NE(nullptr, OCI_ResultsetGetFileByName(rs, OTEXT("FILE_COL")));
    }

    TEST_F(TestResultsetByName, GetDataSizeByIndex_ReturnsSize)
    {
        ASSERT_EQ(5u, OCI_ResultsetGetDataSize(rs, 1));
    }

    TEST_F(TestResultsetByName, GetDataSizeByName_ReturnsSize)
    {
        ASSERT_EQ(5u, OCI_ResultsetGetDataSizeByName(rs, OTEXT("TXT_COL")));
    }

    TEST_F(TestResultsetByName, GetColumnIndexByName_ReturnsIndex)
    {
        ASSERT_EQ(1, OCI_ResultsetGetColumnIndex(rs, OTEXT("TXT_COL")));
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
            ASSERT_TRUE(OCI_StatementSetFetchMode(stmt, OCI_SFM_SCROLLABLE));
            ASSERT_TRUE(OCI_StatementExecute(stmt,
                OTEXT("select cursor(select 1 from dual) as c, id from (select 1 as id from dual union all select 2 from dual) order by id")));

            rs = OCI_StatementGetResultset(stmt);
            ASSERT_NE(nullptr, rs);
        }

        void TearDown() override
        {
            OCI_StatementReleaseResultsets(stmt);
            OCI_StatementFree(stmt);
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
        OCI_Statement*  stmt{ nullptr };
        OCI_Resultset*  rs{ nullptr };
    };

    TEST_F(TestResultsetScroll, FetchNextOnCursorColumn_FetchesRows)
    {
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        ASSERT_EQ(1, OCI_ResultsetGetCurrentRow(rs));
    }

    TEST_F(TestResultsetScroll, RelativeSeekBeforeFirst_ClampsAtStart)
    {
        // relative seek past the beginning -> fails and sets bof
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        ASSERT_FALSE(OCI_ResultsetFetchSeek(rs, OCI_SFD_RELATIVE, -10));
    }

    /* --- public-API coverage of the dropped OcilibResultset* /
       OcilibEnvSetScrollableCursors internals: OCI_StatementSetFetchMode /
       OCI_StatementGetFetchMode / OCI_ResultsetFetchSeek / OCI_ResultsetFetchPrev / OCI_ResultsetFetchFirst /
       OCI_ResultsetFetchLast / OCI_ResultsetGetRowCount --- */

    TEST(TestResultset, FetchModeGetSet)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("select 1 as n from dual")));

        /* OcilibStatement* fetch-mode internal path via OCI_StatementSetFetchMode /
           OCI_StatementGetFetchMode */
        ASSERT_TRUE(OCI_StatementSetFetchMode(stmt, OCI_SFM_SCROLLABLE));
        ASSERT_EQ((OCI_FetchMode)OCI_SFM_SCROLLABLE, OCI_StatementGetFetchMode(stmt));
        ASSERT_TRUE(OCI_StatementSetFetchMode(stmt, OCI_SFM_DEFAULT));
        ASSERT_EQ((OCI_FetchMode)OCI_SFM_DEFAULT, OCI_StatementGetFetchMode(stmt));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestResultset, FetchSeekRoundtrip)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);
        ASSERT_TRUE(OCI_StatementSetFetchMode(stmt, OCI_SFM_SCROLLABLE));
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("select 1 as n from dual union all select 2 from dual order by 1")));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        /* OcilibResultset* fetch/seek internals via public fetch API */
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        ASSERT_EQ(1, OCI_ResultsetGetInt32(rs, 0));
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        ASSERT_EQ(2, OCI_ResultsetGetInt32(rs, 0));
        ASSERT_TRUE(OCI_ResultsetGetRowCount(rs) >= 2u);
        ASSERT_TRUE(OCI_ResultsetFetchPrev(rs));
        ASSERT_EQ(1, OCI_ResultsetGetInt32(rs, 0));
        ASSERT_TRUE(OCI_ResultsetFetchLast(rs));
        ASSERT_EQ(2, OCI_ResultsetGetInt32(rs, 0));
        ASSERT_TRUE(OCI_ResultsetFetchFirst(rs));
        ASSERT_EQ(1, OCI_ResultsetGetInt32(rs, 0));
        ASSERT_TRUE(OCI_ResultsetFetchSeek(rs, OCI_SFD_RELATIVE, 1));
        ASSERT_EQ(2, OCI_ResultsetGetInt32(rs, 0));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }






    TEST(TestResultset, GetColumnIndex_UnknownName_ReturnsMinusOne)
    {
        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select 1 as val from dual")));
        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        ASSERT_EQ(-1, OCI_ResultsetGetColumnIndex(rs, OTEXT("no_such_column")));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestResultset, GetColumn_OutOfRangeIndex_ReturnsNull)
    {
        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select 1 as val from dual")));
        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_EQ(nullptr, OCI_ResultsetGetColumn(rs, 999));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    class TestResultsetNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestResultsetNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestResultsetNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "FetchFirst", [] { return !OCI_ResultsetFetchFirst(nullptr); } },
            { "FetchLast", [] { return !OCI_ResultsetFetchLast(nullptr); } },
            { "FetchNext", [] { return !OCI_ResultsetFetchNext(nullptr); } },
            { "FetchPrev", [] { return !OCI_ResultsetFetchPrev(nullptr); } },
            { "FetchSeek", [] { return !OCI_ResultsetFetchSeek(nullptr, 1, 1); } },
            { "GetBigInt", [] { return 0 == OCI_ResultsetGetInt64(nullptr, 1); } },
            { "GetBigIntByName", [] { return 0 == OCI_ResultsetGetInt64ByName(nullptr, OTEXT("x")); } },
            { "GetColl", [] { return nullptr == OCI_ResultsetGetColl(nullptr, 1); } },
            { "GetCollByName", [] { return nullptr == OCI_ResultsetGetCollByName(nullptr, OTEXT("x")); } },
            { "GetColumn", [] { return nullptr == OCI_ResultsetGetColumn(nullptr, 1); } },
            { "GetColumnByName", [] { return nullptr == OCI_ResultsetGetColumnByName(nullptr, OTEXT("x")); } },
            { "GetColumnCount", [] { return 0 == OCI_ResultsetGetColumnCount(nullptr); } },
            { "GetColumnIndex", [] { return -1 == OCI_ResultsetGetColumnIndex(nullptr, OTEXT("x")); } },
            { "GetCurrentRow", [] { return -1 == OCI_ResultsetGetCurrentRow(nullptr); } },
            { "GetDataLength", [] { return 0 == OCI_ResultsetGetDataLength(nullptr, 1); } },
            { "GetDataSize", [] { return 0 == OCI_ResultsetGetDataSize(nullptr, 1); } },
            { "GetDataSizeByName", [] { return 0 == OCI_ResultsetGetDataSizeByName(nullptr, OTEXT("x")); } },
            { "GetDate", [] { return nullptr == OCI_ResultsetGetDate(nullptr, 1); } },
            { "GetDateByName", [] { return nullptr == OCI_ResultsetGetDateByName(nullptr, OTEXT("x")); } },
            { "GetDouble", [] { return 0.0 == OCI_ResultsetGetDouble(nullptr, 1); } },
            { "GetDoubleByName", [] { return 0.0 == OCI_ResultsetGetDoubleByName(nullptr, OTEXT("x")); } },
            { "GetFile", [] { return nullptr == OCI_ResultsetGetFile(nullptr, 1); } },
            { "GetFileByName", [] { return nullptr == OCI_ResultsetGetFileByName(nullptr, OTEXT("x")); } },
            { "GetFloat", [] { return 0.0f == OCI_ResultsetGetFloat(nullptr, 1); } },
            { "GetFloatByName", [] { return 0.0f == OCI_ResultsetGetFloatByName(nullptr, OTEXT("x")); } },
            { "GetInt", [] { return 0 == OCI_ResultsetGetInt32(nullptr, 1); } },
            { "GetIntByName", [] { return 0 == OCI_ResultsetGetInt32ByName(nullptr, OTEXT("x")); } },
            { "GetInterval", [] { return nullptr == OCI_ResultsetGetInterval(nullptr, 1); } },
            { "GetIntervalByName", [] { return nullptr == OCI_ResultsetGetIntervalByName(nullptr, OTEXT("x")); } },
            { "GetJson", [] { return nullptr == OCI_ResultsetGetJson(nullptr, 1); } },
            { "GetJsonByName", [] { return nullptr == OCI_ResultsetGetJsonByName(nullptr, OTEXT("x")); } },
            { "GetLob", [] { return nullptr == OCI_ResultsetGetLob(nullptr, 1); } },
            { "GetLobByName", [] { return nullptr == OCI_ResultsetGetLobByName(nullptr, OTEXT("x")); } },
            { "GetLong", [] { return nullptr == OCI_ResultsetGetLong(nullptr, 1); } },
            { "GetLongByName", [] { return nullptr == OCI_ResultsetGetLongByName(nullptr, OTEXT("x")); } },
            { "GetNumber", [] { return nullptr == OCI_ResultsetGetNumber(nullptr, 1); } },
            { "GetNumberByName", [] { return nullptr == OCI_ResultsetGetNumberByName(nullptr, OTEXT("x")); } },
            { "GetObject", [] { return nullptr == OCI_ResultsetGetObject(nullptr, 1); } },
            { "GetObjectByName", [] { return nullptr == OCI_ResultsetGetObjectByName(nullptr, OTEXT("x")); } },
            { "GetRaw", [] { return 0 == OCI_ResultsetGetRaw(nullptr, 1, NullArgBuffer(), 1); } },
            { "GetRawByName", [] { return 0 == OCI_ResultsetGetRawByName(nullptr, OTEXT("x"), NullArgBuffer(), 1); } },
            { "GetRef", [] { return nullptr == OCI_ResultsetGetReference(nullptr, 1); } },
            { "GetRefByName", [] { return nullptr == OCI_ResultsetGetReferenceByName(nullptr, OTEXT("x")); } },
            { "GetRowCount", [] { return 0 == OCI_ResultsetGetRowCount(nullptr); } },
            { "GetShort", [] { return 0 == OCI_ResultsetGetInt16(nullptr, 1); } },
            { "GetShortByName", [] { return 0 == OCI_ResultsetGetInt16ByName(nullptr, OTEXT("x")); } },
            { "GetStatement", [] { return nullptr == OCI_ResultsetGetCursor(nullptr, 1); } },
            { "GetStatementByName", [] { return nullptr == OCI_ResultsetGetCursorByName(nullptr, OTEXT("x")); } },
            { "GetString", [] { return nullptr == OCI_ResultsetGetString(nullptr, 1); } },
            { "GetStringByName", [] { return nullptr == OCI_ResultsetGetStringByName(nullptr, OTEXT("x")); } },
            { "GetStruct", [] { return !OCI_ResultsetGetStruct(nullptr, NullArgBuffer(), NullArgBuffer()); } },
            { "GetTimestamp", [] { return nullptr == OCI_ResultsetGetTimestamp(nullptr, 1); } },
            { "GetTimestampByName", [] { return nullptr == OCI_ResultsetGetTimestampByName(nullptr, OTEXT("x")); } },
            { "GetUnsignedBigInt", [] { return 0 == OCI_ResultsetGetUInt64(nullptr, 1); } },
            { "GetUnsignedBigIntByName", [] { return 0 == OCI_ResultsetGetUInt64ByName(nullptr, OTEXT("x")); } },
            { "GetUnsignedInt", [] { return 0 == OCI_ResultsetGetUInt32(nullptr, 1); } },
            { "GetUnsignedIntByName", [] { return 0 == OCI_ResultsetGetUInt32ByName(nullptr, OTEXT("x")); } },
            { "GetUnsignedShort", [] { return 0 == OCI_ResultsetGetUInt16(nullptr, 1); } },
            { "GetUnsignedShortByName", [] { return 0 == OCI_ResultsetGetUInt16ByName(nullptr, OTEXT("x")); } },
            { "GetVector", [] { return nullptr == OCI_ResultsetGetVector(nullptr, 1); } },
            { "GetVectorByName", [] { return nullptr == OCI_ResultsetGetVectorByName(nullptr, OTEXT("x")); } },
            { "GetXmlType", [] { return nullptr == OCI_ResultsetGetXmlType(nullptr, 1); } },
            { "GetXmlTypeByName", [] { return nullptr == OCI_ResultsetGetXmlTypeByName(nullptr, OTEXT("x")); } },
            { "IsNull", [] { return !OCI_ResultsetIsColumnNull(nullptr, 1); } },
            { "IsNullByName", [] { return OCI_ResultsetIsColumnNullByName(nullptr, OTEXT("x")); } }, // NULL handle -> null status indeterminate -> TRUE
            { "ResultsetGetStatement", [] { return nullptr == OCI_ResultsetGetStatement(nullptr); } },
            { "SetStructNumericType", [] { return !OCI_ResultsetSetStructNumericType(nullptr, 1, 1); } },
            { "SetStructNumericTypeByName", [] { return !OCI_ResultsetSetStructNumericTypeByName(nullptr, OTEXT("x"), 1); } },
        }),
        &NullArgCaseName);

}
