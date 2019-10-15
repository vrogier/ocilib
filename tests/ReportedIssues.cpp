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