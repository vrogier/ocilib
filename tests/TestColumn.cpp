#include "ocilib_tests.h"

#include "test_internals.h"

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

        auto col = OCI_GetColumn(rs, 1);
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
            ASSERT_EQ(ostring(spec.sqlType), ostring(OCI_ColumnGetSQLType(col)));
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
                {1, OTEXT("CHAR"),      OCI_CDT_TEXT},
                {2, OTEXT("NCHAR"),     OCI_CDT_TEXT},
                {3, OTEXT("VARCHAR2"),  OCI_CDT_TEXT},
                {4, OTEXT("NVARCHAR2"), OCI_CDT_TEXT},
                {5, OTEXT("VARCHAR2"),  OCI_CDT_TEXT},
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

        auto col = OCI_GetColumn(rs, 1);
        ASSERT_EQ(OCI_CSF_DEFAULT, OCI_ColumnGetCharsetForm(col));
        ASSERT_EQ(FALSE, OCI_ColumnGetCharUsed(col));

        col = OCI_GetColumn(rs, 2);
        ASSERT_EQ(OCI_CSF_NATIONAL, OCI_ColumnGetCharsetForm(col));

        col = OCI_GetColumn(rs, 3);
        ASSERT_EQ(OCI_CSF_DEFAULT, OCI_ColumnGetCharsetForm(col));
        ASSERT_EQ(TRUE, OCI_ColumnGetCharUsed(col));
        ASSERT_EQ(30u, OCI_ColumnGetSize(col));

        col = OCI_GetColumn(rs, 4);
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
                {1, OTEXT("NUMBER"),        OCI_CDT_NUMERIC},
                {2, OTEXT("NUMBER"),        OCI_CDT_NUMERIC},
                {3, OTEXT("NUMBER"),        OCI_CDT_NUMERIC},
                {4, OTEXT("FLOAT"),         OCI_CDT_NUMERIC},
                {5, OTEXT("BINARY FLOAT"),  OCI_CDT_NUMERIC},
                {6, OTEXT("BINARY DOUBLE"), OCI_CDT_NUMERIC},
            }
        );

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        auto col = OCI_GetColumn(rs, 2);
        ASSERT_EQ(10, OCI_ColumnGetPrecision(col));
        ASSERT_EQ(0, OCI_ColumnGetScale(col));

        col = OCI_GetColumn(rs, 3);
        ASSERT_EQ(10, OCI_ColumnGetPrecision(col));
        ASSERT_EQ(3, OCI_ColumnGetScale(col));

        col = OCI_GetColumn(rs, 5);
        ASSERT_EQ(OCI_NUM_FLOAT, OCI_ColumnGetSubType(col));

        col = OCI_GetColumn(rs, 6);
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
                {1, OTEXT("DATE"),                            OCI_CDT_DATETIME},
                {2, OTEXT("TIMESTAMP"),                       OCI_CDT_TIMESTAMP},
                {3, OTEXT("TIMESTAMP WITH TIME ZONE"),        OCI_CDT_TIMESTAMP},
                {4, OTEXT("TIMESTAMP WITH LOCAL TIME ZONE"),  OCI_CDT_TIMESTAMP},
                {5, OTEXT("INTERVAL YEAR TO MONTH"),          OCI_CDT_INTERVAL},
                {6, OTEXT("INTERVAL DAY TO SECOND"),          OCI_CDT_INTERVAL},
            }
        );

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        auto col = OCI_GetColumn(rs, 2);
        ASSERT_EQ(6, OCI_ColumnGetFractionalPrecision(col));
        ASSERT_EQ(0, OCI_ColumnGetLeadingPrecision(col));
        ASSERT_EQ(OCI_TIMESTAMP, OCI_ColumnGetSubType(col));

        col = OCI_GetColumn(rs, 3);
        ASSERT_EQ(3, OCI_ColumnGetFractionalPrecision(col));
        ASSERT_EQ(OCI_TIMESTAMP_TZ, OCI_ColumnGetSubType(col));

        col = OCI_GetColumn(rs, 4);
        ASSERT_EQ(2, OCI_ColumnGetFractionalPrecision(col));
        ASSERT_EQ(OCI_TIMESTAMP_LTZ, OCI_ColumnGetSubType(col));

        col = OCI_GetColumn(rs, 5);
        ASSERT_EQ(4, OCI_ColumnGetLeadingPrecision(col));
        ASSERT_EQ(OCI_INTERVAL_YM, OCI_ColumnGetSubType(col));

        col = OCI_GetColumn(rs, 6);
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
                {1, OTEXT("CLOB"),            OCI_CDT_LOB},
                {2, OTEXT("NCLOB"),           OCI_CDT_LOB},
                {3, OTEXT("BLOB"),            OCI_CDT_LOB},
                {4, OTEXT("BINARY FILE LOB"), OCI_CDT_FILE},
                {5, OTEXT("RAW"),             OCI_CDT_RAW},
                {6, OTEXT("LONG RAW"),        OCI_CDT_LONG},
                {7, OTEXT("ROWID"),           OCI_CDT_TEXT},
            }
        );

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_EQ(OCI_CLOB, OCI_ColumnGetSubType(OCI_GetColumn(rs, 1)));
        ASSERT_EQ(OCI_NCLOB, OCI_ColumnGetSubType(OCI_GetColumn(rs, 2)));
        ASSERT_EQ(OCI_BLOB, OCI_ColumnGetSubType(OCI_GetColumn(rs, 3)));
        ASSERT_EQ(OCI_BFILE, OCI_ColumnGetSubType(OCI_GetColumn(rs, 4)));
        ASSERT_EQ(OCI_BLONG, OCI_ColumnGetSubType(OCI_GetColumn(rs, 6)));

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

        const auto col = OCI_GetColumn(rs, 1);
        ASSERT_NE(nullptr, col);
        ASSERT_EQ(OCI_CDT_XMLTYPE, OCI_ColumnGetType(col));
        ASSERT_EQ(ostring(OTEXT("XMLTYPE")), ostring(OCI_ColumnGetSQLType(col)));

        otext buffer[OCI_SIZE_BUFFER];
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(ostring(OTEXT("XMLTYPE")), ostring(buffer));

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

        auto col = OCI_GetColumn(rs, 1);
        ASSERT_TRUE(OCI_ColumnGetPropertyFlags(col) & OCI_CPF_IS_IDENTITY);
        ASSERT_TRUE(OCI_ColumnGetPropertyFlags(col) & OCI_CPF_IS_GEN_ALWAYS);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT * FROM TestColProps2")));

        rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        col = OCI_GetColumn(rs, 1);
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

        auto col = OCI_GetColumn(rs, 2);
        ASSERT_NE(nullptr, col);
        ASSERT_EQ(OCI_CDT_LONG, OCI_ColumnGetType(col));
        ASSERT_EQ(OCI_CLONG, OCI_ColumnGetSubType(col));
        ASSERT_EQ(ostring(OTEXT("LONG")), ostring(OCI_ColumnGetSQLType(col)));

        otext buffer[OCI_SIZE_BUFFER];
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(ostring(OTEXT("LONG")), ostring(buffer));

        /* implicit mode maps LONG to OCI_CDT_TEXT */

        ASSERT_TRUE(OCI_SetLongMode(stmt, OCI_LONG_IMPLICIT));
        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT * FROM TestColLong")));

        rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        col = OCI_GetColumn(rs, 2);
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

        const auto col = OCI_GetColumn(rs, 1);
        ASSERT_NE(nullptr, col);
        ASSERT_EQ(OCI_CDT_CURSOR, OCI_ColumnGetType(col));
        ASSERT_EQ(ostring(OTEXT("RESULTSET")), ostring(OCI_ColumnGetSQLType(col)));

        otext buffer[OCI_SIZE_BUFFER];
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(ostring(OTEXT("RESULTSET")), ostring(buffer));

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

        auto col = OCI_GetColumn(rs, 1);
        ASSERT_EQ(OCI_CDT_OBJECT, OCI_ColumnGetType(col));
        ASSERT_EQ(ToUpper(ostring(OTEXT("TestColObj"))), ToUpper(ostring(OCI_ColumnGetSQLType(col))));
        ASSERT_NE(nullptr, OCI_ColumnGetTypeInfo(col));
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);

        col = OCI_GetColumn(rs, 2);
        ASSERT_EQ(OCI_CDT_REF, OCI_ColumnGetType(col));
        ASSERT_EQ(ostring(OTEXT("REF")), ostring(OCI_ColumnGetSQLType(col)));
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_NE(nullptr, OCI_ColumnGetTypeInfo(col));

        col = OCI_GetColumn(rs, 3);
        ASSERT_EQ(OCI_CDT_COLLECTION, OCI_ColumnGetType(col));
        ASSERT_EQ(ToUpper(ostring(OTEXT("TestColObjTab"))), ToUpper(ostring(OCI_ColumnGetSQLType(col))));
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
                {1, OTEXT("?"),              OCI_CDT_JSON},
                {2, OTEXT("PL/SQL BOOLEAN"), OCI_CDT_BOOLEAN},
                {3, OTEXT("VECTOR"),         OCI_CDT_VECTOR},
                {4, OTEXT("VECTOR"),         OCI_CDT_VECTOR},
                {5, OTEXT("VECTOR"),         OCI_CDT_VECTOR},
                {6, OTEXT("VECTOR"),         OCI_CDT_VECTOR},
                {7, OTEXT("VARCHAR2"),       OCI_CDT_TEXT},
            }
        );

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        otext buffer[OCI_SIZE_BUFFER];

        auto col = OCI_GetColumn(rs, 3);
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(ostring(OTEXT("VECTOR(4, FLOAT32)")), ostring(buffer));
        ASSERT_EQ(4u, OCI_ColumnGetDimension(col));

        col = OCI_GetColumn(rs, 4);
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(ostring(OTEXT("VECTOR(8, FLOAT64)")), ostring(buffer));

        col = OCI_GetColumn(rs, 5);
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(ostring(OTEXT("VECTOR(16, INT8)")), ostring(buffer));

        col = OCI_GetColumn(rs, 6);
        ASSERT_GT(OCI_ColumnGetFullSQLType(col, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(ostring(OTEXT("VECTOR(*, *)")), ostring(buffer));
        ASSERT_EQ(0u, OCI_ColumnGetDimension(col));

        col = OCI_GetColumn(rs, 7);
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

        ASSERT_TRUE(OCI_SetStructNumericType(rs, 1, OCI_NUM_INT));

        struct Row { OCI_Int64 id; float fval; double dval; };
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

    TEST(TestColumn, InternalBranchCoverage)
    {
        /* unknown vector format falls back to "?" */
        ASSERT_EQ(ostring(OTEXT("?")), ostring(OCI_TestColumnVectorFormat((OCI_VectorFormat) 0xFF)));
        ASSERT_EQ(ostring(OTEXT("*")), ostring(OCI_TestColumnVectorFormat(OCI_VEC_FLEX)));

        /* out of range attribute info request returns FALSE with size 0 */
        OCI_Size size = 1, align = 1;
        ASSERT_FALSE(OCI_TestColumnGetAttributeInfo(1, 5, OCI_CDT_NUMERIC, OCI_NUM_NUMBER, &size, &align));
        ASSERT_EQ(0u, size);

        /* numeric subtype mapping for user structures */
        ASSERT_TRUE(OCI_TestColumnGetAttributeInfo(1, 0, OCI_CDT_NUMERIC, OCI_NUM_SHORT, &size, &align));
        ASSERT_EQ(sizeof(OCI_Int16), size);
        ASSERT_TRUE(OCI_TestColumnGetAttributeInfo(1, 0, OCI_CDT_NUMERIC, OCI_NUM_INT, &size, &align));
        ASSERT_EQ(sizeof(OCI_Int32), size);
        ASSERT_TRUE(OCI_TestColumnGetAttributeInfo(1, 0, OCI_CDT_NUMERIC, OCI_NUM_BIGUINT, &size, &align));
        ASSERT_EQ(sizeof(OCI_Int64), size);
        ASSERT_TRUE(OCI_TestColumnGetAttributeInfo(1, 0, OCI_CDT_NUMERIC, OCI_NUM_DOUBLE, &size, &align));
        ASSERT_EQ(sizeof(double), size);
        ASSERT_TRUE(OCI_TestColumnGetAttributeInfo(1, 0, OCI_CDT_NUMERIC, OCI_NUM_FLOAT, &size, &align));
        ASSERT_EQ(sizeof(float), size);

        /* boolean attribute info */
        ASSERT_TRUE(OCI_TestColumnGetAttributeInfo(1, 0, OCI_CDT_BOOLEAN, 0, &size, &align));
        ASSERT_EQ(sizeof(OCI_Boolean), size);

        /* SQL codes unreachable through public describe APIs */
        otext buffer[OCI_SIZE_BUFFER];

        ASSERT_EQ(ostring(OTEXT("CFILE")), ostring(OCI_TestColumnGetSqlType(OCI_TEST_CODE_CFILE)));
        ASSERT_GT(OCI_TestColumnGetFullSqlType(OCI_TEST_CODE_CFILE, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(ostring(OTEXT("CFILE")), ostring(buffer));

        ASSERT_EQ(ostring(OTEXT("PL/SQL TABLE INDEX BY")), ostring(OCI_TestColumnGetSqlType(OCI_TEST_CODE_TAB)));
        ASSERT_GT(OCI_TestColumnGetFullSqlType(OCI_TEST_CODE_TAB, buffer, sizeof(buffer) / sizeof(otext)), 0u);

        ASSERT_EQ(ostring(OTEXT("INTEGER")), ostring(OCI_TestColumnGetSqlType(OCI_TEST_CODE_INT)));
        ASSERT_GT(OCI_TestColumnGetFullSqlType(OCI_TEST_CODE_INT, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(ostring(OTEXT("NUMBER")), ostring(buffer));

        ASSERT_EQ(ostring(OTEXT("FLOAT")), ostring(OCI_TestColumnGetSqlType(OCI_TEST_CODE_FLT)));
        ASSERT_GT(OCI_TestColumnGetFullSqlType(OCI_TEST_CODE_FLT, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(ostring(OTEXT("FLOAT(0)")), ostring(buffer));

        /* named type without typinf falls back to a generic label */
        ASSERT_EQ(ostring(OTEXT("NAMED TYPE")), ostring(OCI_TestColumnGetSqlType(OCI_TEST_CODE_NTY)));
        ASSERT_GT(OCI_TestColumnGetFullSqlType(OCI_TEST_CODE_NTY, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(ostring(OTEXT("NAMED TYPE")), ostring(buffer));

        ASSERT_EQ(ostring(OTEXT("PL/SQL RECORD")), ostring(OCI_TestColumnGetSqlType(OCI_TEST_CODE_UNDOC_REC)));
        ASSERT_GT(OCI_TestColumnGetFullSqlType(OCI_TEST_CODE_UNDOC_REC, buffer, sizeof(buffer) / sizeof(otext)), 0u);

        ASSERT_EQ(ostring(OTEXT("PL/SQL INTEGER")), ostring(OCI_TestColumnGetSqlType(OCI_TEST_CODE_PLS_INTEGER)));
        ASSERT_GT(OCI_TestColumnGetFullSqlType(OCI_TEST_CODE_PLS_INTEGER, buffer, sizeof(buffer) / sizeof(otext)), 0u);

        ASSERT_EQ(ostring(OTEXT("PL/SQL BINARY INTEGER")), ostring(OCI_TestColumnGetSqlType(OCI_TEST_CODE_BIN_INTEGER)));
        ASSERT_GT(OCI_TestColumnGetFullSqlType(OCI_TEST_CODE_BIN_INTEGER, buffer, sizeof(buffer) / sizeof(otext)), 0u);

        ASSERT_EQ(ostring(OTEXT("CURSOR")), ostring(OCI_TestColumnGetSqlType(OCI_TEST_CODE_CURSOR)));
        ASSERT_GT(OCI_TestColumnGetFullSqlType(OCI_TEST_CODE_CURSOR, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(ostring(OTEXT("CURSOR")), ostring(buffer));

        ASSERT_EQ(ostring(OTEXT("PL/SQL RECORD")), ostring(OCI_TestColumnGetSqlType(OCI_TEST_CODE_REC)));
        ASSERT_GT(OCI_TestColumnGetFullSqlType(OCI_TEST_CODE_REC, buffer, sizeof(buffer) / sizeof(otext)), 0u);

        /* opaque type without type info and unknown codes resolve to "?" */
        ASSERT_EQ(ostring(OTEXT("?")), ostring(OCI_TestColumnGetSqlType(OCI_TEST_CODE_OPAQUE)));
        ASSERT_GT(OCI_TestColumnGetFullSqlType(OCI_TEST_CODE_OPAQUE, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(ostring(OTEXT("?")), ostring(OCI_TestColumnGetSqlType(OCI_TEST_CODE_UNKNOWN)));
        ASSERT_GT(OCI_TestColumnGetFullSqlType(OCI_TEST_CODE_UNKNOWN, buffer, sizeof(buffer) / sizeof(otext)), 0u);
        ASSERT_EQ(ostring(OTEXT("?")), ostring(buffer));

        /* OcilibColumnMapInfo() arms for SQL codes that only show up when
           describing PL/SQL packaged types or RETURNING INTO placeholders */

        OCI_Int32          datatype = OCI_UNKNOWN;
        OCI_UInt32 subtype  = 0;

        ASSERT_TRUE(OCI_TestColumnMapInfo(OCI_TEST_CODE_INT, &datatype, &subtype));
        ASSERT_EQ(OCI_CDT_NUMERIC, datatype);
        ASSERT_EQ((OCI_UInt32) OCI_NUM_INT, subtype);

        ASSERT_TRUE(OCI_TestColumnMapInfo(OCI_TEST_CODE_UIN, &datatype, &subtype));
        ASSERT_EQ(OCI_CDT_NUMERIC, datatype);
        ASSERT_EQ((OCI_UInt32) OCI_NUM_UINT, subtype);

        ASSERT_TRUE(OCI_TestColumnMapInfo(OCI_TEST_CODE_FLT, &datatype, &subtype));
        ASSERT_EQ(OCI_CDT_NUMERIC, datatype);
        ASSERT_EQ((OCI_UInt32) OCI_NUM_NUMBER, subtype);

        ASSERT_TRUE(OCI_TestColumnMapInfo(OCI_TEST_CODE_REC, &datatype, &subtype));
        ASSERT_EQ(OCI_CDT_OBJECT, datatype);

        ASSERT_TRUE(OCI_TestColumnMapInfo(OCI_TEST_CODE_CFILE, &datatype, &subtype));
        ASSERT_EQ(OCI_CDT_FILE, datatype);
        ASSERT_EQ((OCI_UInt32) OCI_CFILE, subtype);
    }
}
