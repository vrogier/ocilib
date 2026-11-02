#include "ocilib_tests.h"
#include "test_internals.h"

namespace TestCApi
{
    TEST(TestStringutils, GetStringConversions)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_SetFormat(env, OCI_FMT_NUMERIC, OTEXT("FM999999")));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select 'hello' as str, 123 as num from dual")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        ASSERT_EQ(ostring(OTEXT("hello")), ostring(OCI_GetString(rs, 1)));
        ASSERT_EQ(ostring(OTEXT("hello")), ostring(OCI_GetStringByName(rs, OTEXT("STR"))));

        ASSERT_EQ(ostring(OTEXT("123")), ostring(OCI_GetString(rs, 2)));
        ASSERT_EQ(ostring(OTEXT("123")), ostring(OCI_GetStringByName(rs, OTEXT("NUM"))));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestStringutils, NumberAndDateToString)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select 123456 as n, to_date('2020-05-12', 'YYYY-MM-DD') as d, 3.14 as f from dual")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        ASSERT_EQ(ostring(OTEXT("123456")), ostring(OCI_GetString(rs, 1)));
        ASSERT_EQ(ostring(OTEXT("2020-05-12")), ostring(OCI_GetString(rs, 2)));
        ASSERT_NE(nullptr, OCI_GetString(rs, 3));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestStringutils, UnicodeAndRawConversions)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select 'hello' as s, UTL_RAW.CAST_TO_RAW('world') as r, cast('test' as nvarchar2(50)) as n from dual")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        ASSERT_EQ(ostring(OTEXT("hello")), ostring(OCI_GetString(rs, 1)));
        ASSERT_TRUE(nullptr != OCI_GetString(rs, 2));
        ASSERT_EQ(ostring(OTEXT("test")), ostring(OCI_GetString(rs, 3)));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestStringutils, GetStringForNumericSubtypes)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select cast(12 as number(4)) as s, cast(12345 as number(10)) as i, cast(3.14 as binary_float) as f, cast(3.14 as binary_double) as d from dual")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        ASSERT_EQ(ostring(OTEXT("12")), ostring(OCI_GetString(rs, 1)));
        ASSERT_EQ(ostring(OTEXT("12345")), ostring(OCI_GetString(rs, 2)));
        ASSERT_NE(nullptr, OCI_GetString(rs, 3));
        ASSERT_NE(nullptr, OCI_GetString(rs, 4));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestStringutils, BindAndGetString)
    {
        ExecDML(OTEXT("drop table TestStringutilsData"));
        ExecDML(OTEXT("create table TestStringutilsData (value varchar2(50))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto insertStmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, insertStmt);

        otext value[50] = OTEXT("bound value");
        ASSERT_TRUE(OCI_Prepare(insertStmt, OTEXT("insert into TestStringutilsData values (:1)")));
        ASSERT_NE(nullptr, OCI_BindString(insertStmt, OTEXT(":1"), value, 49));
        ASSERT_TRUE(OCI_Execute(insertStmt));

        ASSERT_TRUE(OCI_StatementFree(insertStmt));

        const auto selectStmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, selectStmt);

        ASSERT_TRUE(OCI_ExecuteStmt(selectStmt, OTEXT("select value from TestStringutilsData")));

        const auto rs = OCI_GetResultset(selectStmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        ASSERT_EQ(ostring(OTEXT("bound value")), ostring(OCI_GetString(rs, 1)));

        ASSERT_TRUE(OCI_StatementFree(selectStmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestStringutilsData"));
    }

    /* --- OcilibStringLength --- */

    struct StrLenCase
    {
        const void*  src;
        OCI_UInt32 elemSize;
        OCI_UInt32 expected;
    };

    class TestStringLength : public ::testing::TestWithParam<StrLenCase> {};

    static const char  strsu_c8[]  = "hello";
    static const OCI_Int16 strsu_c16[] = { 'h', 'e', 'l', 'l', 'o', 0 };
    static const OCI_Int32   strsu_c32[] = { 'h', 'e', 'l', 'l', 'o', 0 };

    TEST_P(TestStringLength, Computes)
    {
        ASSERT_EQ(GetParam().expected, OCI_TestStringLength(GetParam().src, GetParam().elemSize));
    }

    INSTANTIATE_TEST_CASE_P(ElementSizes, TestStringLength,
        ::testing::Values(
            StrLenCase{ nullptr,    sizeof(char),  0 },
            StrLenCase{ strsu_c8,   sizeof(char),  5 },
            StrLenCase{ strsu_c16,  sizeof(OCI_Int16), 5 },
            StrLenCase{ strsu_c32,  sizeof(OCI_Int32),   5 },
            StrLenCase{ strsu_c8,   3,             0 }));

    /* --- OcilibStringTranslate: every expand/pack arm + same-size copies --- */

    struct StrTranslateCase
    {
        OCI_UInt32 srcWidth;
        OCI_UInt32 dstWidth;
    };

    class TestStringTranslate : public ::testing::TestWithParam<StrTranslateCase> {};

    static void StoreElem(unsigned char* buf, OCI_UInt32 width, OCI_UInt32 idx, OCI_Int32 value)
    {
        memset(buf + idx * width, 0, width);
        memcpy(buf + idx * width, &value, width < sizeof(OCI_Int32) ? width : sizeof(OCI_Int32));
    }

    static OCI_Int32 LoadElem(const unsigned char* buf, OCI_UInt32 width, OCI_UInt32 idx)
    {
        OCI_Int32 value = 0;
        memcpy(&value, buf + idx * width, width < sizeof(OCI_Int32) ? width : sizeof(OCI_Int32));
        return value;
    }

    TEST_P(TestStringTranslate, Translates)
    {
        /* widen/narrow "hello" between char/OCI_Int16/OCI_Int32 element sizes */

        const OCI_UInt32 srcW = GetParam().srcWidth;
        const OCI_UInt32 dstW = GetParam().dstWidth;

        unsigned char src[64] = {};
        unsigned char dst[64] = {};

        const OCI_Int32 values[] = { 'h', 'e', 'l', 'l', 'o', 0 };
        for (OCI_UInt32 i = 0; i < 6; i++)
        {
            StoreElem(src, srcW, i, values[i]);
        }

        OCI_TestStringTranslate(src, dst, 5, srcW, dstW);

        for (OCI_UInt32 i = 0; i < 6; i++)
        {
            ASSERT_EQ(values[i], LoadElem(dst, dstW, i));
        }
    }

    INSTANTIATE_TEST_CASE_P(WidthPairs, TestStringTranslate,
        ::testing::Values(
            StrTranslateCase{ sizeof(char),  sizeof(OCI_Int16) },
            StrTranslateCase{ sizeof(char),  sizeof(OCI_Int32) },
            StrTranslateCase{ sizeof(OCI_Int16), sizeof(OCI_Int32) },
            StrTranslateCase{ sizeof(OCI_Int32),   sizeof(char) },
            StrTranslateCase{ sizeof(OCI_Int32),   sizeof(OCI_Int16) },
            StrTranslateCase{ sizeof(OCI_Int16), sizeof(char) },
            StrTranslateCase{ sizeof(char),  sizeof(char) },
            StrTranslateCase{ sizeof(OCI_Int16), sizeof(OCI_Int16) },
            StrTranslateCase{ sizeof(OCI_Int32),   sizeof(OCI_Int32) }));

    TEST(TestStringutils, TranslateNullSource)
    {
        OCI_Int16 dst[8] = {};
        OCI_TestStringTranslate(nullptr, dst, 5, sizeof(char), sizeof(OCI_Int16));
        ASSERT_EQ(0, (OCI_Int32) dst[0]);
    }

    TEST(TestStringutils, TranslateNullDest)
    {
        const char src[] = "hello";
        OCI_TestStringTranslate((void*) src, nullptr, 5, sizeof(char), sizeof(OCI_Int16));
    }

    TEST(TestStringutils, TranslateEmptySource)
    {
        /* empty source returns early without touching the destination */

        const char empty[] = "";
        OCI_Int16      dst[2]  = { 7, 7 };

        OCI_TestStringTranslate((void*) empty, dst, 0, sizeof(char), sizeof(OCI_Int16));
        ASSERT_EQ(7, (OCI_Int32) dst[0]);
    }

    /* --- OcilibStringBinaryToString --- */

    TEST(TestStringutils, BinaryToStringSizeQuery)
    {
        /* sizing call with NULL buffer returns the hex length */

        const unsigned char raw[] = { 0x01, 0xAB, 0xFF };
        ASSERT_EQ(6u, OCI_TestStringBinaryToString(raw, 3, nullptr));
    }

    TEST(TestStringutils, BinaryToStringConverts)
    {
        const unsigned char raw[] = { 0x01, 0xAB, 0xFF };

        otext hex[8] = {};
        ASSERT_EQ(6u, OCI_TestStringBinaryToString(raw, 3, hex));
        ASSERT_EQ(ostring(OTEXT("01ABFF")), ostring(hex));
    }

    /* --- OcilibStringRequestBuffer --- */

    class TestStringBuffer : public ::testing::Test
    {
    protected:

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
        }

        void TearDown() override
        {
            if (buf)
            {
                OCI_TestStringAnsiFree(conn, reinterpret_cast<char*>(buf));
            }
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
        otext*          buf{ nullptr };
        OCI_UInt32    size{ 0 };
    };

    TEST_F(TestStringBuffer, RequestBufferNullBuffer)
    {
        ASSERT_FALSE(OCI_TestStringRequestBuffer(conn, nullptr, &size, 4));
    }

    TEST_F(TestStringBuffer, RequestBufferNullSize)
    {
        ASSERT_FALSE(OCI_TestStringRequestBuffer(conn, &buf, nullptr, 4));
    }

    TEST_F(TestStringBuffer, RequestBufferAllocates)
    {
        ASSERT_TRUE(OCI_TestStringRequestBuffer(conn, &buf, &size, 4));
        ASSERT_NE(nullptr, buf);
    }

    TEST_F(TestStringBuffer, RequestBufferGrows)
    {
        ASSERT_TRUE(OCI_TestStringRequestBuffer(conn, &buf, &size, 4));
        ASSERT_NE(nullptr, buf);

        const auto first_size = size;
        ASSERT_TRUE(OCI_TestStringRequestBuffer(conn, &buf, &size, 64));
        ASSERT_GT(size, first_size);
    }

    TEST_F(TestStringBuffer, RequestBufferShrinks)
    {
        /* smaller request keeps the allocation and shrinks the tracked size */

        ASSERT_TRUE(OCI_TestStringRequestBuffer(conn, &buf, &size, 64));
        ASSERT_TRUE(OCI_TestStringRequestBuffer(conn, &buf, &size, 8));
        ASSERT_EQ(9u * sizeof(otext), size);
    }

    TEST_F(TestStringBuffer, RequestBufferOverflow)
    {
        /* request larger than UINT_MAX fails cleanly */

        ASSERT_FALSE(OCI_TestStringRequestBuffer(conn, &buf, &size, 0xFFFFFFFFu));
    }

    TEST_F(TestStringBuffer, RequestBufferUtf8Charset)
    {
        /* UTF-8 charset multiplier */

        const auto old_charset = OCI_TestEnvSetCharset(conn, 1);
        ASSERT_TRUE(OCI_TestStringRequestBuffer(conn, &buf, &size, 100));
        ASSERT_EQ(404u, size);
        OCI_TestEnvSetCharset(conn, old_charset);
    }

    /* --- wide-char conversion --- */

    TEST(TestStringutils, GetDBStringWide)
    {
        /* UTF32 payload; size expressed in otext bytes as callers do */

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const OCI_UInt32 utf32[] = { 'A', 'B', 'C' };
        OCI_Int32 dbsize = (OCI_Int32)(3 * sizeof(otext));

        const auto wide = OCI_TestStringGetDBStringWide(conn, utf32, &dbsize);
        ASSERT_NE(nullptr, wide);
        OCI_TestStringReleaseDBStringWide(conn, const_cast<void*>(wide));

        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    /* --- ANSI duplicate --- */

    TEST(TestStringutils, AnsiDuplicate)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto dup = OCI_TestStringAnsiDuplicate(conn, "dupstr");
        ASSERT_NE(nullptr, dup);
        ASSERT_EQ(0, strcmp(dup, "dupstr"));
        OCI_TestStringAnsiFree(conn, const_cast<char*>(dup));

        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestStringutils, AnsiDuplicateNull)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        ASSERT_EQ(nullptr, OCI_TestStringAnsiDuplicate(conn, nullptr));

        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    /* --- case-insensitive compares --- */

    struct StrCaseCompareCase
    {
        const otext* left;
        const otext* right;
        OCI_Int32          expectedSign;
    };

    class TestStringCaseCompare : public ::testing::TestWithParam<StrCaseCompareCase> {};

    TEST_P(TestStringCaseCompare, Compares)
    {
        const OCI_Int32 res = OCI_TestStringCaseCompare(GetParam().left, GetParam().right);

        if (GetParam().expectedSign > 0) { ASSERT_GT(res, 0); }
        else if (GetParam().expectedSign < 0) { ASSERT_LT(res, 0); }
        else { ASSERT_EQ(0, res); }
    }

    INSTANTIATE_TEST_CASE_P(Pairs, TestStringCaseCompare,
        ::testing::Values(
            StrCaseCompareCase{ nullptr,      nullptr,      0 },
            StrCaseCompareCase{ nullptr,      OTEXT("a"),   1 },
            StrCaseCompareCase{ OTEXT("a"),   nullptr,      -1 },
            StrCaseCompareCase{ OTEXT("AbC"), OTEXT("aBc"), 0 },
            StrCaseCompareCase{ OTEXT("b"),   OTEXT("a"),   1 },
            StrCaseCompareCase{ OTEXT("a"),   OTEXT("b"),   -1 }));

    struct StrAnsiCaseCompareCase
    {
        const char* left;
        const char* right;
        OCI_Int32         expectedSign;
    };

    class TestStringAnsiCaseCompare : public ::testing::TestWithParam<StrAnsiCaseCompareCase> {};

    TEST_P(TestStringAnsiCaseCompare, Compares)
    {
        const OCI_Int32 res = OCI_TestStringAnsiCaseCompare(GetParam().left, GetParam().right);

        if (GetParam().expectedSign > 0) { ASSERT_GT(res, 0); }
        else if (GetParam().expectedSign < 0) { ASSERT_LT(res, 0); }
        else { ASSERT_EQ(0, res); }
    }

    INSTANTIATE_TEST_CASE_P(Pairs, TestStringAnsiCaseCompare,
        ::testing::Values(
            StrAnsiCaseCompareCase{ nullptr, nullptr,  0 },
            StrAnsiCaseCompareCase{ nullptr, "a",      1 },
            StrAnsiCaseCompareCase{ "a",     nullptr,  -1 },
            StrAnsiCaseCompareCase{ "AbC",   "aBc",    0 },
            StrAnsiCaseCompareCase{ "b",     "a",      1 },
            StrAnsiCaseCompareCase{ "a",     "b",      -1 }));

    /* --- OcilibStringGetTypeName --- */

    TEST(TestStringutils, GetTypeNameNullSource)
    {
        otext name[64] = {};
        ASSERT_EQ(0u, OCI_TestStringGetTypeName(nullptr, name, sizeof(name)));
    }

    TEST(TestStringutils, GetTypeNameNullBuffer)
    {
        ASSERT_EQ(0u, OCI_TestStringGetTypeName(OTEXT("X"), nullptr, 64));
    }

    TEST(TestStringutils, GetTypeNameEmptySource)
    {
        otext name[64] = {};
        ASSERT_EQ(0u, OCI_TestStringGetTypeName(OTEXT(""), name, sizeof(name)));
    }

    TEST(TestStringutils, GetTypeNamePlain)
    {
        otext name[64] = {};
        ASSERT_GT(OCI_TestStringGetTypeName(OTEXT("MYTABLE"), name, 63), 0u);
        ASSERT_EQ(ostring(OTEXT("MYTABLE")), ostring(name));
    }

    TEST(TestStringutils, GetTypeNameMixedCaseQuoted)
    {
        /* mixed-case name gets quoted */

        otext name[64] = {};
        ASSERT_GT(OCI_TestStringGetTypeName(OTEXT("MixedCase"), name, 63), 0u);
        ASSERT_EQ(ostring(OTEXT("\"MixedCase\"")), ostring(name));
    }

    TEST(TestStringutils, GetTypeNameAlreadyQuoted)
    {
        otext name[64] = {};
        ASSERT_GT(OCI_TestStringGetTypeName(OTEXT("\"AlreadyQuoted\""), name, 63), 0u);
    }

    TEST(TestStringutils, GetTypeNameTruncated)
    {
        /* truncation clamps at the limit */

        otext name[64] = {};
        ASSERT_EQ(4u, OCI_TestStringGetTypeName(OTEXT("ABCDEFGH"), name, 4));
        ASSERT_EQ(ostring(OTEXT("ABCD")), ostring(name));
    }

    /* --- OcilibStringGetFullTypeName --- */

    TEST(TestStringutils, GetFullTypeNameNullBuffer)
    {
        ASSERT_EQ(0u, OCI_TestStringGetFullTypeName(OTEXT("S"), OTEXT("P"), OTEXT("T"), OTEXT("L"), nullptr, 63));
    }

    TEST(TestStringutils, GetFullTypeNameComposes)
    {
        /* the full schema.package.type@link composition */

        otext name[64] = {};
        const auto off = OCI_TestStringGetFullTypeName(OTEXT("SCH"), OTEXT("PKG"), OTEXT("TYP"), OTEXT("LNK"), name, 63);
        ASSERT_GT(off, 0u);
        ASSERT_EQ(ostring(OTEXT("SCH.PKG.TYP@LNK")), ostring(name));
    }

    /* --- OcilibStringAddToBuffer --- */

    struct StrAddToBufferCase
    {
        const otext* str;
        OCI_UInt32 len;
        boolean      quote;
        boolean      withBuffer;
        OCI_UInt32 expectedLen;
        const otext* expected;   /* nullptr -> length only */
    };

    class TestStringAddToBuffer : public ::testing::TestWithParam<StrAddToBufferCase> {};

    TEST_P(TestStringAddToBuffer, Appends)
    {
        otext out[32] = {};
        otext* target = GetParam().withBuffer ? out : nullptr;

        const auto len = OCI_TestStringAddToBuffer(target, 0, GetParam().str, GetParam().len, GetParam().quote);
        ASSERT_EQ(GetParam().expectedLen, len);

        if (GetParam().expected)
        {
            ASSERT_EQ(ostring(GetParam().expected), ostring(out));
        }
    }

    INSTANTIATE_TEST_CASE_P(Inputs, TestStringAddToBuffer,
        ::testing::Values(
            StrAddToBufferCase{ nullptr,      3, FALSE, TRUE,  0, nullptr },
            StrAddToBufferCase{ OTEXT("abc"), 3, FALSE, TRUE,  3, OTEXT("abc") },
            /* quote doubling: "a'b" -> "a''b" (+1 char) */
            StrAddToBufferCase{ OTEXT("a'b"), 3, TRUE,  TRUE,  4, OTEXT("a''b") },
            /* sizing pass without a buffer still counts the extra quote */
            StrAddToBufferCase{ OTEXT("a'b"), 3, TRUE,  FALSE, 4, nullptr },
            StrAddToBufferCase{ OTEXT("abc"), 3, TRUE,  FALSE, 3, nullptr },
            StrAddToBufferCase{ OTEXT("abc"), 3, FALSE, FALSE, 3, nullptr }));

    /* --- OcilibStringGetFromType edge cases --- */

    class TestStringGetFromType : public ::testing::Test
    {
    protected:

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
        }

        void TearDown() override
        {
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
    };

    struct StrFromTypeNullCase
    {
        OCI_UInt32 datatype;
        OCI_UInt32 subtype;
    };

    class TestStringFromTypeNull : public TestStringGetFromType,
                                   public ::testing::WithParamInterface<StrFromTypeNullCase> {};

    TEST_P(TestStringFromTypeNull, Fails)
    {
        /* NULL payloads -> conversion fails, len = 0 */

        otext buf[256] = {};
        ASSERT_EQ(0u, OCI_TestStringGetFromType(conn, GetParam().datatype, GetParam().subtype,
                                                nullptr, 0, buf, sizeof(buf), FALSE));
    }

    INSTANTIATE_TEST_CASE_P(Types, TestStringFromTypeNull,
        ::testing::Values(
            StrFromTypeNullCase{ OCI_CDT_BOOLEAN, 0 },
            StrFromTypeNullCase{ OCI_CDT_LONG,    OCI_CLONG },
            StrFromTypeNullCase{ OCI_CDT_LOB,     OCI_BLOB },
            StrFromTypeNullCase{ OCI_CDT_FILE,    OCI_BFILE },
            StrFromTypeNullCase{ OCI_CDT_CURSOR,  0 },
            StrFromTypeNullCase{ OCI_CDT_REF,     0 }));

    TEST_F(TestStringGetFromType, UnsupportedDatatype)
    {
        /* unsupported datatype -> default arm */

        otext buf[256] = {};
        ASSERT_EQ(0u, OCI_TestStringGetFromType(conn, 9999, 0, buf, 4, buf, sizeof(buf), FALSE));
    }

    TEST_F(TestStringGetFromType, BooleanTrue)
    {
        OCI_Boolean flag = TRUE;
        otext buf[256] = {};

        ASSERT_GT(OCI_TestStringGetFromType(conn, OCI_CDT_BOOLEAN, 0, &flag, sizeof(flag), buf, sizeof(buf), FALSE), 0u);
        ASSERT_EQ(ostring(OTEXT("TRUE")), ostring(buf));
    }

    TEST_F(TestStringGetFromType, BooleanFalse)
    {
        OCI_Boolean flag = FALSE;
        otext buf[256] = {};

        ASSERT_GT(OCI_TestStringGetFromType(conn, OCI_CDT_BOOLEAN, 0, &flag, sizeof(flag), buf, sizeof(buf), FALSE), 0u);
        ASSERT_EQ(ostring(OTEXT("FALSE")), ostring(buf));
    }

    TEST_F(TestStringGetFromType, BooleanSizeQuery)
    {
        /* sizing call for a BOOLEAN without a buffer */

        OCI_Boolean flag = FALSE;
        ASSERT_GT(OCI_TestStringGetFromType(conn, OCI_CDT_BOOLEAN, 0, &flag, sizeof(flag), nullptr, 0, FALSE), 0u);
    }

    TEST_F(TestStringGetFromType, TextQuoting)
    {
        /* text with quoting (data_size is in dbtext units) */

        otext buf[256] = {};
        otext txt[] = OTEXT("it's");
        const auto dbsz = OCI_TestBindDbTextSize();

        ASSERT_GT(OCI_TestStringGetFromType(conn, OCI_CDT_TEXT, 0, txt, 4 * dbsz, buf, sizeof(buf), TRUE), 0u);
        ASSERT_EQ(OTEXT('\''), buf[0]);
    }

    TEST_F(TestStringGetFromType, RawBeyondBuffer)
    {
        /* computed length beyond buffer_size -> failure (payload is small
           enough to stay inside the real buffer; only the reported size
           is too small) */

        unsigned char raw[] = { 0x01, 0xAB };
        otext buf[256] = {};

        ASSERT_EQ(0u, OCI_TestStringGetFromType(conn, OCI_CDT_RAW, 0, raw, sizeof(raw), buf, 2, FALSE));
    }

    TEST_F(TestStringGetFromType, CursorFromStatement)
    {
        /* CURSOR arm needs a statement's sql text */

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);
        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select 1 from dual")));

        otext buf[256] = {};
        ASSERT_GT(OCI_TestStringGetFromType(conn, OCI_CDT_CURSOR, 0, stmt, 0, buf, sizeof(buf), FALSE), 0u);

        ASSERT_TRUE(OCI_StatementFree(stmt));
    }

    /* --- OcilibStringGetFromType on real fetched data --- */

    class TestStringFromTypeData : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("drop table TestStrData"));
            ExecDML(OTEXT("create table TestStrData (l long, b blob, r raw(16))"));
            ExecDML(OTEXT("insert into TestStrData values ('long value', hextoraw('0A0B'), utl_raw.cast_to_raw('AB'))"));
            ExecDML(OTEXT("commit"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestStrData"));
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

        OCI_Resultset* FetchRow()
        {
            EXPECT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select l, b, r from TestStrData")));

            const auto rs = OCI_GetResultset(stmt);
            EXPECT_NE(nullptr, rs);
            EXPECT_TRUE(OCI_FetchNext(rs));

            return rs;
        }

        OCI_Connection* conn{ nullptr };
        OCI_Statement*  stmt{ nullptr };
    };

    TEST_F(TestStringFromTypeData, LobSizeQuery)
    {
        /* BLOB sizing arm of OcilibStringGetFromType (NULL buffer call) */

        const auto rs = FetchRow();
        ASSERT_NE(nullptr, rs);

        const auto blob = OCI_GetLob(rs, 2);
        ASSERT_NE(nullptr, blob);

        ASSERT_GT(OCI_TestStringGetFromType(conn, OCI_CDT_LOB, OCI_BLOB, blob, 0, nullptr, 0, FALSE), 0u);
    }

    TEST_F(TestStringFromTypeData, LobConverts)
    {
        const auto rs = FetchRow();
        ASSERT_NE(nullptr, rs);

        const auto blob = OCI_GetLob(rs, 2);
        ASSERT_NE(nullptr, blob);

        otext out[512] = {};
        ASSERT_GT(OCI_TestStringGetFromType(conn, OCI_CDT_LOB, OCI_BLOB, blob, 0, out, sizeof(out), FALSE), 0u);
    }

    TEST_F(TestStringFromTypeData, LongConverts)
    {
        /* CLONG arm through the internal helper with a fetched LONG */

        const auto rs = FetchRow();
        ASSERT_NE(nullptr, rs);

        const auto lg = OCI_GetLong(rs, 1);
        ASSERT_NE(nullptr, lg);

        otext out[512] = {};
        ASSERT_GT(OCI_TestStringGetFromType(conn, OCI_CDT_LONG, OCI_CLONG, lg, 0, out, sizeof(out), FALSE), 0u);
    }

    TEST_F(TestStringFromTypeData, RawConverts)
    {
        unsigned char raw[] = { 0x01, 0xAB };
        otext out[512] = {};

        ASSERT_EQ(4u, OCI_TestStringGetFromType(conn, OCI_CDT_RAW, 0, raw, sizeof(raw), out, sizeof(out), FALSE));
        ASSERT_EQ(ostring(OTEXT("01AB")), ostring(out));
    }

    TEST_F(TestStringFromTypeData, FileConverts)
    {
        const auto file = OCI_FileCreate(conn, OCI_BFILE);
        ASSERT_NE(nullptr, file);
        ASSERT_TRUE(OCI_FileSetName(file, OTEXT("SOME_DIR"), OTEXT("file.bin")));

        otext out[512] = {};
        ASSERT_GT(OCI_TestStringGetFromType(conn, OCI_CDT_FILE, OCI_BFILE, file, 0, out, sizeof(out), FALSE), 0u);

        ASSERT_TRUE(OCI_FileFree(file));
    }
}
