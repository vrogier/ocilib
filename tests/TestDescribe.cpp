#include "ocilib_tests.h"

void CheckColumnType
(
    int index, 
    OCI_TypeInfo *typinf,
    const ostring &name,
    const ostring &typeName, 
    unsigned int type
)
{
    const auto col = OCI_TypeInfoGetColumn(typinf, index);
    ASSERT_NE(nullptr, col);

    ASSERT_EQ(name, ostring(OCI_GetColumnName(col)));
    ASSERT_EQ(typeName, ostring(OCI_GetColumnSQLType(col)));
    ASSERT_EQ(type, OCI_GetColumnType(col));
}

TEST(TestDescribe, Type)
{
    ExecDML
    (
        OTEXT("create type TestDescribeSubType as OBJECT (id int, name varchar2(50))")
    );

    ExecDML
    (
        OTEXT("create type TestDescribeType as object ")
        OTEXT("( ")
        OTEXT("    val_int  number, ")
        OTEXT("    val_flt  float, ")
        OTEXT("    val_str  varchar2(30), ")
        OTEXT("    val_date date, ")
        OTEXT("    val_tsmp timestamp, ")
        OTEXT("    val_itv interval year to month, ")
        OTEXT("    val_lob  clob, ")
        OTEXT("    val_file bfile, ")
        OTEXT("    val_raw  raw(10), ")
        OTEXT("    val_obj  TestDescribeSubType")
        OTEXT(")")
    );

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestDescribeType"), OCI_TIF_TYPE);
    ASSERT_NE(nullptr, typinf);

    const int typepPropCount = 10;

    ASSERT_EQ(typepPropCount, OCI_TypeInfoGetColumnCount(typinf));

    CheckColumnType(1, typinf, OTEXT("VAL_INT"), OTEXT("NUMBER"), OCI_CDT_NUMERIC);
    CheckColumnType(2, typinf, OTEXT("VAL_FLT"), OTEXT("FLOAT"), OCI_CDT_NUMERIC);
    CheckColumnType(3, typinf, OTEXT("VAL_STR"), OTEXT("VARCHAR2"), OCI_CDT_TEXT);
    CheckColumnType(4, typinf, OTEXT("VAL_DATE"), OTEXT("DATE"), OCI_CDT_DATETIME);
    CheckColumnType(5, typinf, OTEXT("VAL_TSMP"), OTEXT("TIMESTAMP"), OCI_CDT_TIMESTAMP);
    CheckColumnType(6, typinf, OTEXT("VAL_ITV"), OTEXT("INTERVAL YEAR TO MONTH"), OCI_CDT_INTERVAL);
    CheckColumnType(7, typinf, OTEXT("VAL_LOB"), OTEXT("CLOB"), OCI_CDT_LOB);
    CheckColumnType(8, typinf, OTEXT("VAL_FILE"), OTEXT("BINARY FILE LOB"), OCI_CDT_FILE);
    CheckColumnType(9, typinf, OTEXT("VAL_RAW"), OTEXT("RAW"), OCI_CDT_RAW);
    CheckColumnType(10, typinf, OTEXT("VAL_OBJ"), OTEXT("TESTDESCRIBESUBTYPE"), OCI_CDT_OBJECT);

    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("drop type TestDescribeType"));
    ExecDML(OTEXT("drop type TestDescribeSubType"));
}


TEST(TestDescribe, Table)
{
    ExecDML
    (
        OTEXT("create type TestDescribeTableSubType as OBJECT (id int, name varchar2(50))")
    );

    ExecDML
    (
        OTEXT("create table TestDescribeTable ")
        OTEXT("( ")
        OTEXT("    val_int  number NOT NULL, ")
        OTEXT("    val_flt  float, ")
        OTEXT("    val_str  varchar2(30), ")
        OTEXT("    val_date date, ")
        OTEXT("    val_tsmp timestamp, ")
        OTEXT("    val_itv interval year to month, ")
        OTEXT("    val_lob  clob, ")
        OTEXT("    val_file bfile, ")
        OTEXT("    val_raw  raw(10), ")
        OTEXT("    val_obj  TestDescribeTableSubType")
        OTEXT(")")
    );

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestDescribeTable"), OCI_TIF_TABLE);
    ASSERT_NE(nullptr, typinf);

    const int typepPropCount = 10;

    ASSERT_EQ(typepPropCount, OCI_TypeInfoGetColumnCount(typinf));

    CheckColumnType(1, typinf, OTEXT("VAL_INT"), OTEXT("NUMBER"), OCI_CDT_NUMERIC);
    CheckColumnType(2, typinf, OTEXT("VAL_FLT"), OTEXT("FLOAT"), OCI_CDT_NUMERIC);
    CheckColumnType(3, typinf, OTEXT("VAL_STR"), OTEXT("VARCHAR2"), OCI_CDT_TEXT);
    CheckColumnType(4, typinf, OTEXT("VAL_DATE"), OTEXT("DATE"), OCI_CDT_DATETIME);
    CheckColumnType(5, typinf, OTEXT("VAL_TSMP"), OTEXT("TIMESTAMP"), OCI_CDT_TIMESTAMP);
    CheckColumnType(6, typinf, OTEXT("VAL_ITV"), OTEXT("INTERVAL YEAR TO MONTH"), OCI_CDT_INTERVAL);
    CheckColumnType(7, typinf, OTEXT("VAL_LOB"), OTEXT("CLOB"), OCI_CDT_LOB);
    CheckColumnType(8, typinf, OTEXT("VAL_FILE"), OTEXT("BINARY FILE LOB"), OCI_CDT_FILE);
    CheckColumnType(9, typinf, OTEXT("VAL_RAW"), OTEXT("RAW"), OCI_CDT_RAW);
    CheckColumnType(10, typinf, OTEXT("VAL_OBJ"), OTEXT("TESTDESCRIBETABLESUBTYPE"), OCI_CDT_OBJECT);

    const auto col1 = OCI_TypeInfoGetColumn(typinf, 1);
    ASSERT_NE(nullptr, col1);
    ASSERT_EQ(FALSE, OCI_ColumnGetNullable(col1));

    const auto col2 = OCI_TypeInfoGetColumn(typinf, 2);
    ASSERT_NE(nullptr, col2);
    ASSERT_EQ(TRUE, OCI_ColumnGetNullable(col2));

    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("drop type TestDescribeTableSubType"));
    ExecDML(OTEXT("drop type TestDescribeTable"));
}