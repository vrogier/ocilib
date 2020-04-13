#include "ocilib_tests.h"

TEST(ReportedIssues, Issue216)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    ASSERT_TRUE(OCI_Immediate(conn, OTEXT("CREATE TABLE ISSUE_216(ID NUMBER, DATEVALUE DATE)")));

    const auto stmt = OCI_StatementCreate(conn);
    ASSERT_NE(nullptr, stmt);

    const int totalRowCount = 54;
    const int bufferSize = 50;

    int offset = 0;
    int actualRowCount = bufferSize;
    
    auto ints = static_cast<int*>(calloc(actualRowCount, sizeof(int)));
    auto dates = static_cast<OCI_Date**>(calloc(actualRowCount, sizeof(OCI_Date*)));

    ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("INSERT INTO ISSUE_216(ID, DATEVALUE) VALUES(:i, :d)")));
    ASSERT_TRUE(OCI_BindArraySetSize(stmt, bufferSize));
    ASSERT_TRUE(OCI_BindArrayOfInts(stmt, OTEXT(":i"), ints, 0));
    ASSERT_TRUE(OCI_BindArrayOfDates(stmt, OTEXT(":d"), dates, 0));

    while (offset < totalRowCount)
    {
        for (int i = 0; i < actualRowCount; i++)
        {
            if (dates[i])
            {
                ASSERT_TRUE(OCI_DateFree(dates[i]));
            }
        }

        actualRowCount = totalRowCount - offset < bufferSize ? totalRowCount - offset : bufferSize;

        for (int i = 0; i < actualRowCount; i++)
        {
            ints[i] = i;
            dates[i] = OCI_DateCreate(NULL);
            ASSERT_NE(nullptr, dates[i]);
            ASSERT_TRUE(OCI_DateSysDate(dates[i]));
        }

        if (actualRowCount < bufferSize)
        {
            ASSERT_TRUE(OCI_BindArraySetSize(stmt, actualRowCount));
        }

        ASSERT_TRUE(OCI_Execute(stmt));
        offset += actualRowCount;
    }

    ASSERT_EQ(totalRowCount, offset);

    for (int i = 0; i < actualRowCount; i++)
    {
        ASSERT_TRUE(OCI_DateFree(dates[i]));
    }

    ASSERT_TRUE(OCI_Immediate(conn, OTEXT("DROP TABLE ISSUE_216")));

    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());
}


TEST(ReportedIssues, Issue225)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    ASSERT_TRUE(OCI_ServerEnableOutput(conn, 32000, 5, 255));

    const auto stmt = OCI_StatementCreate(conn);
    ASSERT_NE(nullptr, stmt);

    ASSERT_TRUE(OCI_Prepare(stmt,
        OTEXT(
            "declare "
            "type val_tab is table of number index by binary_integer;"
            "procedure Test( v IN OUT val_tab ) is "
            "begin "
            "DBMS_OUTPUT.PUT_LINE(v.count); "
            "for i in 1..v.count loop "
            "  v(i) := i+10; "
            "  DBMS_OUTPUT.PUT_LINE('v('||i||') = '||v(i));"
            "end loop; "
            "end Test; "
            "begin "
            " Test(:v);"
            " end;")));

    std::array<OCI_Number*, 3> numbers;

    for (size_t i = 0; i < numbers.size(); i++)
    {
        numbers[i] = OCI_NumberCreate(conn);
        ASSERT_NE(nullptr, numbers[i]);
        ASSERT_TRUE(OCI_NumberFromText(numbers[i], TO_STRING(i+1).data(), nullptr));
    }

    ASSERT_TRUE(OCI_BindArrayOfNumbers(stmt, ":v", numbers.data(), static_cast<unsigned int>(numbers.size())));
    ASSERT_TRUE(OCI_BindSetDirection(OCI_GetBind(stmt, 1), OCI_BDM_IN_OUT));
    ASSERT_TRUE(OCI_Execute(stmt));

    ASSERT_EQ(TO_STRING(numbers.size()), ostring(OCI_ServerGetOutput(conn)));

    for (size_t i = 0; i < numbers.size(); i++)
    {
        otext buffer[100];
        osprintf(buffer, 100, OTEXT("v(%d) = %d"), i + 1, i + 1 + 10);
        ASSERT_EQ(ostring(buffer), ostring(OCI_ServerGetOutput(conn)));
    }

    for (size_t i = 0; i < numbers.size(); i++)
    {
        ostring expected = TO_STRING(i+1+10);
        otext buffer[100];
        ASSERT_NE(0, OCI_NumberToText(numbers[i], NULL, 100, buffer));
        ASSERT_EQ(expected, ostring(buffer));
    }
 
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());
}