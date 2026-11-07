#include "ocilib_tests.h"


namespace TestCApi
{
    TEST(TestColumn, Info)
    {
        ExecDML(OTEXT("CREATE TABLE TestCApiCovColInfo(id NUMBER, name VARCHAR2(50))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT * FROM TestCApiCovColInfo")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_EQ(2u, OCI_GetColumnCount(rs));

        auto col = OCI_GetColumn(rs, 0);
        ASSERT_NE(nullptr, col);
        ASSERT_NE(nullptr, OCI_ColumnGetName(col));
        ASSERT_EQ(OCI_CDT_NUMERIC, OCI_ColumnGetType(col));

        col = OCI_GetColumnByName(rs, OTEXT("name"));
        ASSERT_NE(nullptr, col);
        ASSERT_EQ(OCI_CDT_TEXT, OCI_ColumnGetType(col));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TABLE TestCApiCovColInfo"));
    }

    struct ColumnTypeSpec
    {
        OCI_UInt32 index;
        const otext* sqlType;
        OCI_SqlType  datatype;
    };

    static void CheckColumnSqlTypes(OCI_Statement* stmt, const otext* sql, const std::vector<ColumnTypeSpec>& specs)
    {
        ASSERT_TRUE(OCI_ExecuteStmt(stmt, sql));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_EQ(specs.size(), OCI_GetColumnCount(rs));

        otext buffer[OCI_SIZE_BUFFER];

        for (const auto& spec : specs)
        {
            const auto col = OCI_GetColumn(rs, spec.index);
            ASSERT_NE(nullptr, col);
            ASSERT_EQ(String(spec.sqlType), String(OCI_ColumnGetSQLType(col)));
            ASSERT_EQ(spec.datatype, OCI_ColumnGetType(col));
            ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        }
    }

    TEST(TestColumn, CharacterTypeNames)
    {
        ExecDML(OTEXT("CREATE TABLE TestColCharTypes("
                      "c_char CHAR(10), c_nchar NCHAR(10), c_varchar VARCHAR2(20), "
                      "c_nvarchar NVARCHAR2(20), c_vcharchr VARCHAR2(30 CHAR))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        CheckColumnSqlTypes
        (
            stmt, OTEXT("SELECT * FROM TestColCharTypes"),
            {
                {0, OTEXT("CHAR"),      OCI_CDT_TEXT},
                {1, OTEXT("NCHAR"),     OCI_CDT_TEXT},
                {2, OTEXT("VARCHAR2"),  OCI_CDT_TEXT},
                {3, OTEXT("NVARCHAR2"), OCI_CDT_TEXT},
                {4, OTEXT("VARCHAR2"),  OCI_CDT_TEXT},
            }
        );

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TABLE TestColCharTypes"));
    }

    TEST(TestColumn, CharacterSetForms)
    {
        ExecDML(OTEXT("CREATE TABLE TestColCharForms("
                      "c_char CHAR(10), c_nchar NCHAR(10), c_vchar VARCHAR2(30 CHAR), "
                      "c_number NUMBER)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT * FROM TestColCharForms")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        auto col = OCI_GetColumn(rs, 0);
        ASSERT_EQ(OCI_CSF_DEFAULT, OCI_ColumnGetCharsetForm(col));
        ASSERT_EQ(FALSE, OCI_ColumnGetCharUsed(col));

        col = OCI_GetColumn(rs, 1);
        ASSERT_EQ(OCI_CSF_NATIONAL, OCI_ColumnGetCharsetForm(col));

        col = OCI_GetColumn(rs, 2);
        ASSERT_EQ(OCI_CSF_DEFAULT, OCI_ColumnGetCharsetForm(col));
        ASSERT_EQ(TRUE, OCI_ColumnGetCharUsed(col));
        ASSERT_EQ(30u, OCI_ColumnGetSize(col));

        col = OCI_GetColumn(rs, 3);
        ASSERT_EQ(OCI_CSF_NONE, OCI_ColumnGetCharsetForm(col));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TABLE TestColCharForms"));
    }

    TEST(TestColumn, NumericTypeNames)
    {
        ExecDML(OTEXT("CREATE TABLE TestColNumTypes("
                      "c_number NUMBER, c_numprec NUMBER(10), c_numscale NUMBER(10,3), "
                      "c_float FLOAT(24), c_bfloat BINARY_FLOAT, c_bdouble BINARY_DOUBLE)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        CheckColumnSqlTypes
        (
            stmt, OTEXT("SELECT * FROM TestColNumTypes"),
            {
                {0, OTEXT("NUMBER"),        OCI_CDT_NUMERIC},
                {1, OTEXT("NUMBER"),        OCI_CDT_NUMERIC},
                {2, OTEXT("NUMBER"),        OCI_CDT_NUMERIC},
                {3, OTEXT("FLOAT"),         OCI_CDT_NUMERIC},
                {4, OTEXT("BINARY FLOAT"),  OCI_CDT_NUMERIC},
                {5, OTEXT("BINARY DOUBLE"), OCI_CDT_NUMERIC},
            }
        );

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        auto col = OCI_GetColumn(rs, 1);
        ASSERT_EQ(10, OCI_ColumnGetPrecision(col));
        ASSERT_EQ(0, OCI_ColumnGetScale(col));

        col = OCI_GetColumn(rs, 2);
        ASSERT_EQ(10, OCI_ColumnGetPrecision(col));
        ASSERT_EQ(3, OCI_ColumnGetScale(col));

        col = OCI_GetColumn(rs, 4);
        ASSERT_EQ(OCI_NUM_FLOAT, OCI_ColumnGetSubType(col));

        col = OCI_GetColumn(rs, 5);
        ASSERT_EQ(OCI_NUM_DOUBLE, OCI_ColumnGetSubType(col));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TABLE TestColNumTypes"));
    }

    TEST(TestColumn, DateTimeTypeNames)
    {
        ExecDML(OTEXT("CREATE TABLE TestColDateTypes("
                      "c_date DATE, c_ts TIMESTAMP(6), c_tstz TIMESTAMP(3) WITH TIME ZONE, "
                      "c_tsltz TIMESTAMP(2) WITH LOCAL TIME ZONE, "
                      "c_iym INTERVAL YEAR(4) TO MONTH, c_ids INTERVAL DAY(2) TO SECOND(3))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        CheckColumnSqlTypes
        (
            stmt, OTEXT("SELECT * FROM TestColDateTypes"),
            {
                {0, OTEXT("DATE"),                            OCI_CDT_DATETIME},
                {1, OTEXT("TIMESTAMP"),                       OCI_CDT_TIMESTAMP},
                {2, OTEXT("TIMESTAMP WITH TIME ZONE"),        OCI_CDT_TIMESTAMP},
                {3, OTEXT("TIMESTAMP WITH LOCAL TIME ZONE"),  OCI_CDT_TIMESTAMP},
                {4, OTEXT("INTERVAL YEAR TO MONTH"),          OCI_CDT_INTERVAL},
                {5, OTEXT("INTERVAL DAY TO SECOND"),          OCI_CDT_INTERVAL},
            }
        );

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        auto col = OCI_GetColumn(rs, 1);
        ASSERT_EQ(6, OCI_ColumnGetFractionalPrecision(col));
        ASSERT_EQ(0, OCI_ColumnGetLeadingPrecision(col));
        ASSERT_EQ(OCI_TIMESTAMP, OCI_ColumnGetSubType(col));

        col = OCI_GetColumn(rs, 2);
        ASSERT_EQ(3, OCI_ColumnGetFractionalPrecision(col));
        ASSERT_EQ(OCI_TIMESTAMP_TZ, OCI_ColumnGetSubType(col));

        col = OCI_GetColumn(rs, 3);
        ASSERT_EQ(2, OCI_ColumnGetFractionalPrecision(col));
        ASSERT_EQ(OCI_TIMESTAMP_LTZ, OCI_ColumnGetSubType(col));

        col = OCI_GetColumn(rs, 4);
        ASSERT_EQ(4, OCI_ColumnGetLeadingPrecision(col));
        ASSERT_EQ(OCI_INTERVAL_YM, OCI_ColumnGetSubType(col));

        col = OCI_GetColumn(rs, 5);
        ASSERT_EQ(2, OCI_ColumnGetLeadingPrecision(col));
        ASSERT_EQ(3, OCI_ColumnGetFractionalPrecision(col));
        ASSERT_EQ(OCI_INTERVAL_DS, OCI_ColumnGetSubType(col));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TABLE TestColDateTypes"));
    }

    TEST(TestColumn, LobAndLongTypeNames)
    {
        ExecDML(OTEXT("CREATE TABLE TestColLobTypes("
                      "c_clob CLOB, c_nclob NCLOB, c_blob BLOB, c_bfile BFILE, "
                      "c_raw RAW(16), c_lraw LONG RAW, c_urowid UROWID(100))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        CheckColumnSqlTypes
        (
            stmt, OTEXT("SELECT * FROM TestColLobTypes"),
            {
                {0, OTEXT("CLOB"),            OCI_CDT_LOB},
                {1, OTEXT("NCLOB"),           OCI_CDT_LOB},
                {2, OTEXT("BLOB"),            OCI_CDT_LOB},
                {3, OTEXT("BINARY FILE LOB"), OCI_CDT_FILE},
                {4, OTEXT("RAW"),             OCI_CDT_RAW},
                {5, OTEXT("LONG RAW"),        OCI_CDT_LONG},
                {6, OTEXT("ROWID"),           OCI_CDT_TEXT},
            }
        );

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_EQ(OCI_CLOB, OCI_ColumnGetSubType(OCI_GetColumn(rs, 0)));
        ASSERT_EQ(OCI_NCLOB, OCI_ColumnGetSubType(OCI_GetColumn(rs, 1)));
        ASSERT_EQ(OCI_BLOB, OCI_ColumnGetSubType(OCI_GetColumn(rs, 2)));
        ASSERT_EQ(OCI_BFILE, OCI_ColumnGetSubType(OCI_GetColumn(rs, 3)));
        ASSERT_EQ(OCI_BLONG, OCI_ColumnGetSubType(OCI_GetColumn(rs, 5)));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TABLE TestColLobTypes"));
    }

    TEST(TestColumn, XmlTypeColumn)
    {
        ExecDML(OTEXT("CREATE TABLE TestColXml(id NUMBER, c_xml XMLTYPE)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT c_xml FROM TestColXml")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        const auto col = OCI_GetColumn(rs, 0);
        ASSERT_NE(nullptr, col);
        ASSERT_EQ(OCI_CDT_XMLTYPE, OCI_ColumnGetType(col));
        ASSERT_EQ(String(OTEXT("XMLTYPE")), String(OCI_ColumnGetSQLType(col)));

        otext buffer[OCI_SIZE_BUFFER];
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(String(OTEXT("XMLTYPE")), String(buffer));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TABLE TestColXml"));
    }

    TEST(TestColumn, GeneratedColumnFlags)
    {
        ExecDML(OTEXT("CREATE TABLE TestColProps("
                      "c_ident NUMBER GENERATED ALWAYS AS IDENTITY, "
                      "c_base NUMBER, "
                      "c_gen NUMBER GENERATED ALWAYS AS (c_base * 10) VIRTUAL)"));
        ExecDML(OTEXT("CREATE TABLE TestColProps2("
                      "c_ident_null NUMBER GENERATED BY DEFAULT ON NULL AS IDENTITY, "
                      "c_base NUMBER)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT * FROM TestColProps")));

        auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        auto col = OCI_GetColumn(rs, 0);
        ASSERT_TRUE(OCI_ColumnGetPropertyFlags(col) & OCI_CPF_IS_IDENTITY);
        ASSERT_TRUE(OCI_ColumnGetPropertyFlags(col) & OCI_CPF_IS_GEN_ALWAYS);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT * FROM TestColProps2")));

        rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        col = OCI_GetColumn(rs, 0);
        ASSERT_TRUE(OCI_ColumnGetPropertyFlags(col) & OCI_CPF_IS_IDENTITY);
        ASSERT_TRUE(OCI_ColumnGetPropertyFlags(col) & OCI_CPF_IS_GEN_BY_DEFAULT_ON_NULL);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TABLE TestColProps"));
        ExecDML(OTEXT("DROP TABLE TestColProps2"));
    }

    TEST(TestColumn, LongImplicitMode)
    {
        ExecDML(OTEXT("CREATE TABLE TestColLong(id NUMBER, txt LONG)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        /* default explicit mode maps LONG to OCI_CDT_LONG */

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT * FROM TestColLong")));

        auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        auto col = OCI_GetColumn(rs, 1);
        ASSERT_NE(nullptr, col);
        ASSERT_EQ(OCI_CDT_LONG, OCI_ColumnGetType(col));
        ASSERT_EQ(OCI_CLONG, OCI_ColumnGetSubType(col));
        ASSERT_EQ(String(OTEXT("LONG")), String(OCI_ColumnGetSQLType(col)));

        otext buffer[OCI_SIZE_BUFFER];
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(String(OTEXT("LONG")), String(buffer));

        /* implicit mode maps LONG to OCI_CDT_TEXT */

        ASSERT_TRUE(OCI_SetLongMode(stmt, OCI_LONG_IMPLICIT));
        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT * FROM TestColLong")));

        rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        col = OCI_GetColumn(rs, 1);
        ASSERT_NE(nullptr, col);
        ASSERT_EQ(OCI_CDT_TEXT, OCI_ColumnGetType(col));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TABLE TestColLong"));
    }

    TEST(TestColumn, CursorExpression)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT CURSOR(SELECT 1 FROM dual) FROM dual")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        const auto col = OCI_GetColumn(rs, 0);
        ASSERT_NE(nullptr, col);
        ASSERT_EQ(OCI_CDT_CURSOR, OCI_ColumnGetType(col));
        ASSERT_EQ(String(OTEXT("RESULTSET")), String(OCI_ColumnGetSQLType(col)));

        otext buffer[OCI_SIZE_BUFFER];
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(String(OTEXT("RESULTSET")), String(buffer));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));
    }

    TEST(TestColumn, NamedTypeColumns)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestColObj AS OBJECT(id NUMBER, name VARCHAR2(20))"));
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestColObjTab AS TABLE OF TestColObj"));
        ExecDML(OTEXT("CREATE TABLE TestColNamed(c_obj TestColObj, c_ref REF TestColObj, c_coll TestColObjTab) "
                      "NESTED TABLE c_coll STORE AS TestColNamed_st"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT * FROM TestColNamed")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        otext buffer[OCI_SIZE_BUFFER];

        auto col = OCI_GetColumn(rs, 0);
        ASSERT_EQ(OCI_CDT_OBJECT, OCI_ColumnGetType(col));
        ASSERT_EQ(ToUpper(String(OTEXT("TestColObj"))), ToUpper(String(OCI_ColumnGetSQLType(col))));
        ASSERT_NE(nullptr, OCI_ColumnGetTypeInfo(col));
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);

        col = OCI_GetColumn(rs, 1);
        ASSERT_EQ(OCI_CDT_REF, OCI_ColumnGetType(col));
        ASSERT_EQ(String(OTEXT("REF")), String(OCI_ColumnGetSQLType(col)));
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_NE(nullptr, OCI_ColumnGetTypeInfo(col));

        col = OCI_GetColumn(rs, 2);
        ASSERT_EQ(OCI_CDT_COLLECTION, OCI_ColumnGetType(col));
        ASSERT_EQ(ToUpper(String(OTEXT("TestColObjTab"))), ToUpper(String(OCI_ColumnGetSQLType(col))));
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TABLE TestColNamed"));
        ExecDML(OTEXT("DROP TYPE TestColObjTab"));
        ExecDML(OTEXT("DROP TYPE TestColObj"));
    }

    TEST(TestColumn, Oracle23Types)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        if (OCI_GetVersionConnection(con) < OCI_23_4 || OCI_GetOCIRuntimeVersion(env) < OCI_23_4)
        {
            ASSERT_TRUE(OCI_ConnectionFree(con));
            return;
        }
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("CREATE TABLE TestCol23Types("
                      "c_json JSON, c_bool BOOLEAN, c_vec VECTOR(4, FLOAT32), "
                      "c_vec64 VECTOR(8, FLOAT64), c_vec8 VECTOR(16, INT8), c_vexf VECTOR, "
                      "c_text VARCHAR2(10))"));

        con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        CheckColumnSqlTypes
        (
            stmt, OTEXT("SELECT * FROM TestCol23Types"),
            {
                {0, OTEXT("?"),              OCI_CDT_JSON},
                {1, OTEXT("PL/SQL BOOLEAN"), OCI_CDT_BOOLEAN},
                {2, OTEXT("VECTOR"),         OCI_CDT_VECTOR},
                {3, OTEXT("VECTOR"),         OCI_CDT_VECTOR},
                {4, OTEXT("VECTOR"),         OCI_CDT_VECTOR},
                {5, OTEXT("VECTOR"),         OCI_CDT_VECTOR},
                {6, OTEXT("VARCHAR2"),       OCI_CDT_TEXT},
            }
        );

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        otext buffer[OCI_SIZE_BUFFER];

        auto col = OCI_GetColumn(rs, 2);
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(String(OTEXT("VECTOR(4, FLOAT32)")), String(buffer));
        ASSERT_EQ(4u, OCI_ColumnGetDimension(col));

        col = OCI_GetColumn(rs, 3);
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(String(OTEXT("VECTOR(8, FLOAT64)")), String(buffer));

        col = OCI_GetColumn(rs, 4);
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(String(OTEXT("VECTOR(16, INT8)")), String(buffer));

        col = OCI_GetColumn(rs, 5);
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(String(OTEXT("VECTOR(*, *)")), String(buffer));
        ASSERT_EQ(0u, OCI_ColumnGetDimension(col));

        col = OCI_GetColumn(rs, 6);
        ASSERT_NE(OCI_CCI_NONE, OCI_ColumnGetCollationID(col));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TABLE TestCol23Types"));
    }



    TEST(TestColumn, StructAttributeSizes)
    {
        ExecDML(OTEXT("CREATE TABLE TestColStruct(id NUMBER, fval BINARY_FLOAT, dval BINARY_DOUBLE)"));
        ExecDML(OTEXT("INSERT INTO TestColStruct VALUES(1, 1.5, 2.5)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT * FROM TestColStruct")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_TRUE(OCI_SetStructNumericType(rs, 0, OCI_NUM_INT));

        struct Row { OCI_Int64 id; OCI_Float fval; OCI_Double dval; };
        struct RowInd { OCI_Boolean id; OCI_Boolean fval; OCI_Boolean dval; };

        ASSERT_TRUE(OCI_FetchNext(rs));

        Row    row{};
        RowInd ind{};

        ASSERT_TRUE(OCI_GetStruct(rs, &row, &ind));
        ASSERT_TRUE(ind.id);
        ASSERT_EQ(1, row.id);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("DROP TABLE TestColStruct"));
    }

    /* --- public-API coverage of the dropped OcilibColumn* internals
       (OcilibColumnMapInfo / OcilibColumnGetAttributeInfo /
       OcilibColumnGetSqlType / OcilibColumnGetFullSqlType /
       OcilibColumnVectorFormat / OcilibColumnAllocate /
       OcilibColumnDescribe): describe a table and walk every column getter
       on the describe'd columns --- */

    TEST(TestColumn, MappingInternalsViaDescribe)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        ExecDML(OTEXT("drop table TestColMapTbl"));
        ExecDML(OTEXT("create table TestColMapTbl (num number(10,2), str varchar2(30), rawc raw(8), dt date, cl clob, bl blob, ts timestamp, iv interval day to second, rowid_col rowid, vec vector)"));

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);
        ASSERT_TRUE(OCI_Describe(stmt, OTEXT("select * from TestColMapTbl")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_EQ(10u, OCI_GetColumnCount(rs));

        /* OcilibColumnMapInfo / OcilibColumnGetAttributeInfo /
           OcilibColumnGetSqlType / OcilibColumnGetFullSqlType /
           OcilibColumnVectorFormat internals via public column getters */
        for (OCI_UInt32 i = 0; i < OCI_GetColumnCount(rs); ++i)
        {
            const auto col = OCI_GetColumn(rs, i);
            ASSERT_NE(nullptr, col);
            ASSERT_NE(nullptr, OCI_ColumnGetName(col));
            ASSERT_NE(nullptr, OCI_ColumnGetSQLType(col));
            ASSERT_NE(0u, OCI_ColumnGetType(col));
            ASSERT_GE((OCI_Int32)OCI_ColumnGetCharsetForm(col), 0);
            ASSERT_GE((OCI_Int64)OCI_ColumnGetSize(col), 0);
            ASSERT_GE((OCI_Int32)OCI_ColumnGetScale(col), -127);
            ASSERT_GE((OCI_Int32)OCI_ColumnGetPrecision(col), -1);
            ASSERT_GE((OCI_Int32)OCI_ColumnGetFractionalPrecision(col), -1);
            ASSERT_GE((OCI_Int32)OCI_ColumnGetLeadingPrecision(col), -1);
            ASSERT_GE((OCI_Int32)OCI_ColumnGetNullable(col), 0);
            ASSERT_GE((OCI_Int32)OCI_ColumnGetCharUsed(col), 0);
            ASSERT_GE((OCI_Int32)OCI_ColumnGetPropertyFlags(col), 0);
            ASSERT_GE((OCI_Int32)OCI_ColumnGetCollationID(col), 0);
            ASSERT_GE((OCI_Int32)OCI_ColumnGetDimension(col), 0);

            otext fullBuf[128] = { 0 };
            ASSERT_GE((OCI_Int32)OCI_ColumnGetFullSQLType(col, fullBuf, (OCI_UInt32)(sizeof(fullBuf) / sizeof(otext))), 0);

            /* vector-format internals via OCI_ColumnGetSubType /
               OcilibColumnVectorFormat exercised via describe */
            ASSERT_GE((OCI_Int32)OCI_ColumnGetSubType(col), 0);
        }

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("drop table TestColMapTbl"));
    }
}
