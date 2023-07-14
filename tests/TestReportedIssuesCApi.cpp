#include "ocilib_tests.h"

TEST(ReportedIssuesCApi, Issue216)
{
    ExecDML(OTEXT("CREATE TABLE ISSUE_216(ID NUMBER, DATEVALUE DATE)"));

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

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

    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("DROP TABLE ISSUE_216"));
}

TEST(ReportedIssuesCApi, Issue222)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto pool = OCI_PoolCreate(DBS, SYS_USR, SYS_PWD, OCI_POOL_SESSION, OCI_SESSION_SYSDBA, 0, 1, 1);
    ASSERT_NE(nullptr, pool);

    const auto conn = OCI_PoolGetConnection(pool, nullptr);
    ASSERT_NE(nullptr, conn);

    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_PoolFree(pool));
    ASSERT_TRUE(OCI_Cleanup());
}

TEST(ReportedIssuesCApi, Issue225)
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

    ASSERT_TRUE(OCI_BindArrayOfNumbers(stmt, OTEXT(":v"), numbers.data(), static_cast<unsigned int>(numbers.size())));
    ASSERT_TRUE(OCI_BindSetDirection(OCI_GetBind(stmt, 1), OCI_BDM_IN_OUT));
    ASSERT_TRUE(OCI_Execute(stmt));

    ASSERT_EQ(TO_STRING(numbers.size()), ostring(OCI_ServerGetOutput(conn)));

    for (size_t i = 0; i < numbers.size(); i++)
    {
        otext buffer[100];
        osprintf(buffer, OTEXT("v(%d) = %d"), static_cast<int>(i) + 1, static_cast<int>(i) + 1 + 10);
        ASSERT_EQ(ostring(buffer), ostring(OCI_ServerGetOutput(conn)));
    }

    for (size_t i = 0; i < numbers.size(); i++)
    {
        ostring expected = TO_STRING(i+1+10);
        otext buffer[100];
        ASSERT_NE(0, OCI_NumberToText(numbers[i], NULL, 100, buffer));
        ASSERT_EQ(expected, ostring(buffer));
        ASSERT_TRUE(OCI_NumberFree(numbers[i]));
    }
 
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());
}


#ifdef OCI_CHARSET_ANSI

TEST(ReportedIssuesCApi, Issue238)
{
    ExecDML(OTEXT("create table FetchLobAsString(value clob)"));

    const char* nsl_lang = getenv("NLS_LANG");
    putenv((char*)"NLS_LANG=American_America.UTF8");

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto stmt = OCI_StatementCreate(conn);
    ASSERT_NE(nullptr, stmt);

    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("declare c clob;  begin  c := c||rpad('a', 32767, 'a'); insert into FetchLobAsString values (c); end;")));
    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select value from FetchLobAsString")));

    auto rslt = OCI_GetResultset(stmt);
    ASSERT_NE(nullptr, rslt);

    ASSERT_TRUE(OCI_FetchNext(rslt));

    auto value = OCI_GetString(rslt, 1);
    ASSERT_NE(nullptr, value);
    auto len = strlen(value);
    ASSERT_EQ(32767, len);

    ASSERT_TRUE(OCI_StatementFree(stmt));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    std::string new_nls_lang = "NLS_LANG=";

    if (nsl_lang)
    {
        new_nls_lang += nsl_lang;
    }
    putenv((char*)new_nls_lang.data());

    ExecDML(OTEXT("drop table FetchLobAsString;"));
}

#endif

class ReportedIssues247 : public ::testing::TestWithParam<unsigned int> {};

static std::vector<unsigned int> TimestampTypes{ OCI_TIMESTAMP, OCI_TIMESTAMP_TZ, OCI_TIMESTAMP_LTZ };
static Context contextIssue247NoNullPointerException;

static void AddErrorIssue247NoNullPointerException(OCI_Error* err)
{
    contextIssue247NoNullPointerException.Errs.emplace_back(Error{ OCI_ErrorGetInternalCode(err), OCI_ErrorGetOCICode(err) });
}

TEST_P(ReportedIssues247, NoNullPointerException)
{
    ASSERT_TRUE(OCI_Initialize(AddErrorIssue247NoNullPointerException, HOME, OCI_ENV_DEFAULT));

    const auto tmsp = OCI_TimestampCreate(nullptr, GetParam());
    ASSERT_TRUE(OCI_TimestampSysTimestamp(tmsp));
    ASSERT_TRUE(OCI_TimestampFree(tmsp));

    ASSERT_TRUE(contextIssue247NoNullPointerException.Errs.empty());

    ASSERT_TRUE(OCI_Cleanup());
}

TEST(ReportedIssuesCApi, Issue262)
{
    ExecDML(OTEXT("CREATE TABLE test_urowid(name VARCHAR2(256 CHAR) NOT NULL, CONSTRAINT test_urowid PRIMARY KEY (name) ) ORGANIZATION INDEX"));
    ExecDML(OTEXT("INSERT INTO test_urowid VALUES (LPAD('a', 200, 'a'))"));

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto stmt = OCI_StatementCreate(conn);
    ASSERT_NE(nullptr, stmt);

    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("SELECT NAME, LENGTH(ROWID), ROWID FROM test_urowid")));

    const auto rslt = OCI_GetResultset(stmt);
    ASSERT_NE(nullptr, rslt);
 
    ASSERT_TRUE(OCI_FetchNext(rslt));
    const auto name = OCI_GetString(rslt, 1);
    ASSERT_NE(nullptr, name);
    const auto length = static_cast<size_t>(OCI_GetInt(rslt, 2));
    ASSERT_EQ(true, length > 0);
    const auto rowid = OCI_GetString(rslt, 3);
    ASSERT_NE(nullptr, rowid);

    const auto expected_length = length;
    const auto real_length = ostrlen(rowid);
    ASSERT_EQ(expected_length, real_length);

    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("DROP TABLE test_urowid"));
}

TEST(ReportedIssuesCApi, Issue288)
{
    ExecDML(OTEXT("drop table TestIssue288"));
    ExecDML(OTEXT("create table TestIssue288 (id number, val raw(2000))"));
    ExecDML(OTEXT("insert into TestIssue288 "
       "select lvl - 1, hextoraw(val) from "
       "( "
       "    with l as(select level as lvl from dual connect by level < 102) "
       "    select lvl, listagg('FF', '') within group(order by null) as val "
       "    from l connect by level < lvl and prior lvl = lvl and prior sys_guid() is not null "
       "    group by lvl "
       ") "
       "where lvl > 1"));

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto stmt = OCI_StatementCreate(conn);
    ASSERT_NE(nullptr, stmt);

    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select id, val, length(val) from TestIssue288 order by id")));

    auto rslt = OCI_GetResultset(stmt);
    ASSERT_NE(nullptr, rslt);

    auto counter = 0u;

    while (OCI_FetchNext(rslt))
    {
        counter++;
        
        auto id = OCI_GetInt(rslt, 1);
        auto val = OCI_GetString(rslt, 2);
        auto len = OCI_GetInt(rslt, 3);
  
        ASSERT_NE(nullptr, val);

        ASSERT_EQ(counter, id);
        ASSERT_EQ(counter *2, len);
        ASSERT_EQ(len, ostrlen(val));
    }

    ASSERT_EQ(100u, counter);

    ASSERT_TRUE(OCI_StatementFree(stmt));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("drop table TestIssue288"));
}


TEST(ReportedIssuesCApi, Issue308)
{
    ExecDML(OTEXT("create table TestTableIssue308(num number, id number)"));

    ExecDML
    (
        OTEXT("CREATE OR REPLACE PACKAGE TestPackageIssue308 ")
        OTEXT("IS")
        OTEXT("    TYPE ItemType is record(num number, id number);")
        OTEXT("    FUNCTION Func(num number, item OUT ItemType) return number;")
        OTEXT("END;")
    );

    ExecDML
    (
        OTEXT("CREATE OR REPLACE PACKAGE BODY TestPackageIssue308 ")
        OTEXT("IS")
        OTEXT("    FUNCTION Func(num number, item OUT ItemType) return number")
        OTEXT("    IS")
        OTEXT("         row TestTableIssue308%rowtype;")
        OTEXT("    BEGIN")
        OTEXT("        item.num := row.num;")
        OTEXT("        item.id  := row.id;")
        OTEXT("        return item.num;")
        OTEXT("    END;")
        OTEXT("END;")
    );

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto stmt = OCI_StatementCreate(conn);
    ASSERT_NE(nullptr, stmt);


    big_int num = 1234;
    big_int id  = 0; 
    big_int ret = 0;

    const otext* sql = 
        OTEXT("declare")
        OTEXT("  item TestPackageIssue308.ItemType;")
        OTEXT("begin")
        OTEXT("  :ret := TestPackageIssue308.Func(:num, item);")
        OTEXT("  :id  := item.id;")
        OTEXT("end; ");

    ASSERT_TRUE(OCI_Prepare(stmt, sql));
    ASSERT_TRUE(OCI_BindBigInt(stmt, OTEXT(":num"), &num));
    ASSERT_TRUE(OCI_BindBigInt(stmt, OTEXT(":id"), &id));
    ASSERT_TRUE(OCI_BindBigInt(stmt, OTEXT(":ret"), &ret));
  
    ASSERT_TRUE(OCI_BindSetDirection(OCI_GetBind(stmt, 1), OCI_BDM_IN));
    ASSERT_TRUE(OCI_BindSetDirection(OCI_GetBind(stmt, 2), OCI_BDM_OUT));
    ASSERT_TRUE(OCI_BindSetDirection(OCI_GetBind(stmt, 3), OCI_BDM_OUT));

    ASSERT_TRUE(OCI_Execute(stmt));

    ASSERT_TRUE(OCI_StatementFree(stmt));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("drop package TestPackageIssue308"));
    ExecDML(OTEXT("drop table TestTableIssue308"));
}


TEST(ReportedIssuesCApi, Issue313)
{
    ExecDML(OTEXT("drop table TestIssue313"));
    ExecDML(OTEXT("create table TestIssue313(test int, value clob)"));
    ExecDML(OTEXT("insert into TestIssue313 values(1, null)"));
    ExecDML(OTEXT("insert into TestIssue313 values(2, '')"));
    ExecDML(OTEXT("insert into TestIssue313 values(3, empty_clob())"));

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto stmt = OCI_StatementCreate(conn);
    ASSERT_NE(nullptr, stmt);

    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select test, value from TestIssue313")));

    auto rslt = OCI_GetResultset(stmt);
    ASSERT_NE(nullptr, rslt);

    auto counter = 0u;

    while (OCI_FetchNext(rslt))
    {
        auto test = OCI_GetInt(rslt, 1);
        auto value = OCI_GetString(rslt, 2);

        if (test == 1 || test == 2)
        {
            /* NULL CLOB and '' lead to NULL CLOB */
            ASSERT_EQ(nullptr, value);

        }
        else if (test == 3)
        {
            ASSERT_NE(nullptr, value);
            ASSERT_EQ(0, ostrlen(value));
        }
        else
        {
            ASSERT_TRUE(FALSE);
        }

        counter++;
    }

    ASSERT_EQ(3u, counter);

    ASSERT_TRUE(OCI_StatementFree(stmt));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("drop table TestIssue313"));
}

const auto MessageQueueTableIssue316 = OTEXT("MessageQueueTableIssue316");
const auto MessageQueueIssue316 = OTEXT("MessageQueueIssue316");
const auto JmsBytesMessageType = OTEXT("SYS.AQ$_JMS_BYTES_MESSAGE");
const auto JmsHeaderType = OTEXT("SYS.AQ$_JMS_HEADER");
const auto JmsPropArrayType = OTEXT("SYS.AQ$_JMS_USERPROPARRAY");
const auto JmsPropertyType = OTEXT("SYS.AQ$_JMS_USERPROPERTY");

bool Issue316_ReadMessages(OCI_Connection* conn, OCI_Dequeue *dequeue)
{
    enum class ExpectedPayloadType
    {
        Raw,
        Blob
    };

    struct ExpectedInfo
    {
        ExpectedPayloadType Type;
        unsigned int Size;
    };

    std::array<ExpectedInfo, 2> expectedInfos
    {
        ExpectedInfo { ExpectedPayloadType::Raw, 6 },
        ExpectedInfo { ExpectedPayloadType::Blob, 9 }
    };

    const int bufferSize = 100;

    for (auto& info : expectedInfos)
    {
        const auto msg = OCI_DequeueGet(dequeue);
        if (msg == nullptr) return false;

        OCI_Commit(conn);

        const auto jmsBytesMessage = OCI_MsgGetObject(msg);
        if (jmsBytesMessage == nullptr) return false;

        const auto bytesLen = OCI_ObjectGetUnsignedInt(jmsBytesMessage, OTEXT("BYTES_LEN"));
        if (bytesLen != info.Size) return false;

        if (info.Type == ExpectedPayloadType::Raw)
        {
            char buffer[bufferSize];

            const int rawDataSize = OCI_ObjectGetRaw(jmsBytesMessage, OTEXT("BYTES_RAW"), buffer, bufferSize);
            if ((unsigned int) rawDataSize != info.Size) return false;
        }

        if (info.Type == ExpectedPayloadType::Blob)
        {
            const auto lob = OCI_ObjectGetLob(jmsBytesMessage, OTEXT("BYTES_LOB"));
            if (lob == nullptr) return false;
            
            const auto lobDataSize = static_cast<int>(OCI_LobGetLength(lob));
            if ((unsigned int)lobDataSize != info.Size *2) return false;
        }
    }

    return true;
}

OCI_Object* Issue316_CreateJmsBytesMessage(OCI_Connection *conn)
{
    const auto typeJmsBytesMessage = OCI_TypeInfoGet(conn, JmsBytesMessageType, OCI_TIF_TYPE);
    if (typeJmsBytesMessage == nullptr) return nullptr;

    const auto typeJmsHeader = OCI_TypeInfoGet(conn, JmsHeaderType, OCI_TIF_TYPE);
    if (typeJmsHeader == nullptr) return nullptr;

    const auto typeJmsPropArray = OCI_TypeInfoGet(conn, JmsPropArrayType, OCI_TIF_TYPE);
    if (typeJmsPropArray == nullptr) return nullptr;

    const auto typeJmsProperty = OCI_TypeInfoGet(conn, JmsPropertyType, OCI_TIF_TYPE);
    if (typeJmsProperty == nullptr) return nullptr;

    const auto jmsBytesMessage = OCI_ObjectCreate(conn, typeJmsBytesMessage);
    if (jmsBytesMessage == nullptr) return nullptr;

    const auto jmsHeader = OCI_ObjectCreate(conn, typeJmsHeader);
    if (jmsHeader == nullptr) return nullptr;

    const auto jmsPropArray = OCI_CollCreate(typeJmsPropArray);
    if (jmsPropArray == nullptr) return nullptr;

    const auto jmsProp = OCI_ObjectCreate(conn, typeJmsProperty);
    if (jmsProp == nullptr) return nullptr;

    const auto elem = OCI_ElemCreate(typeJmsPropArray);
    if (elem == nullptr) return nullptr;

    auto res = OCI_ObjectSetInt(jmsProp, OTEXT("TYPE"), 100);
    if (res == false) return nullptr;

    res = OCI_ObjectSetInt(jmsProp, OTEXT("JAVA_TYPE"), 27);
    if (res == false) return nullptr;

    res = OCI_ObjectSetString(jmsProp, OTEXT("STR_VALUE"), OTEXT("prop"));
    if (res == false) return nullptr;

    res = OCI_ObjectSetString(jmsProp, OTEXT("NAME"), OTEXT("value"));
    if (res == false) return nullptr;

    res = OCI_ElemSetObject(elem, jmsProp);
    if (res == false) return nullptr;

    res = OCI_CollAppend(jmsPropArray, elem);
    if (res == false) return nullptr;

    res = OCI_ObjectSetColl(jmsHeader, OTEXT("PROPERTIES"), jmsPropArray);
    if (res == false) return nullptr;

    res = OCI_ObjectSetObject(jmsBytesMessage, OTEXT("HEADER"), jmsHeader);
    if (res == false) return nullptr;

    res = OCI_ObjectFree(jmsHeader);
    if (res == false) return nullptr;

    res = OCI_CollFree(jmsPropArray);
    if (res == false) return nullptr;

    res = OCI_ObjectFree(jmsProp);
    if (res == false) return nullptr;

    res = OCI_ElemFree(elem);
    if (res == false) return nullptr;

    return jmsBytesMessage;
}

OCI_Msg* Issue316_CreateRawMessage(OCI_Connection* conn)
{
    auto res{ true };
 
    const auto typeJmsBytesMessage = OCI_TypeInfoGet(conn, JmsBytesMessageType, OCI_TIF_TYPE);
    if (typeJmsBytesMessage == nullptr) return nullptr;

    const auto msgRaw = OCI_MsgCreate(typeJmsBytesMessage);
    if (msgRaw == nullptr) return nullptr;

    const auto objectRaw = Issue316_CreateJmsBytesMessage(conn);
    if (objectRaw == nullptr) return nullptr;

    const auto rawBuffer = OTEXT("123456");

    res = OCI_ObjectSetRaw(objectRaw, OTEXT("BYTES_RAW"), (void*)rawBuffer, 6);
    if (res == false) return nullptr;

    res = OCI_ObjectSetDouble(objectRaw, OTEXT("BYTES_LEN"), 6);
    if (res == false) return nullptr;

    res = OCI_MsgSetObject(msgRaw, objectRaw);
    if (res == false) return nullptr;

    res = OCI_ObjectFree(objectRaw);
    if (res == false) return nullptr;

    return msgRaw;
}

OCI_Msg* Issue316_CreateLobMessage(OCI_Connection* conn)
{
    auto res{ true };

    const auto typeJmsBytesMessage = OCI_TypeInfoGet(conn, JmsBytesMessageType, OCI_TIF_TYPE);
    if (typeJmsBytesMessage == nullptr) return nullptr;

    const auto msgBlob = OCI_MsgCreate(typeJmsBytesMessage);
    if (msgBlob == nullptr) return nullptr;

    const auto objectBlob = Issue316_CreateJmsBytesMessage(conn);
    if (objectBlob == nullptr) return nullptr;

    const auto lob = OCI_LobCreate(conn, OCI_CLOB);
    if (lob == nullptr) return nullptr;

    const auto lobBuffer = OTEXT("123456789");
    res = OCI_LobWrite(lob, (void*)lobBuffer, 9);
    if (res == false) return nullptr;

    res = OCI_ObjectSetLob(objectBlob, OTEXT("BYTES_LOB"), lob);
    if (res == false) return nullptr;

    res = OCI_ObjectSetDouble(objectBlob, OTEXT("BYTES_LEN"), 9);
    if (res == false) return nullptr;

    res = OCI_MsgSetObject(msgBlob, objectBlob);
    if (res == false) return nullptr;

    res = OCI_LobFree(lob);
    if (res == false) return nullptr;

    res = OCI_ObjectFree(objectBlob);
    if (res == false) return nullptr;

    return msgBlob;
}

TEST(ReportedIssuesCApi, Issue316)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT | OCI_ENV_THREADED));
    
    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    ASSERT_TRUE(OCI_QueueTableCreate(conn, MessageQueueTableIssue316, JmsBytesMessageType, NULL, NULL, FALSE, OCI_AGM_NONE, NULL, 0, 0, NULL));
    ASSERT_TRUE(OCI_QueueCreate(conn, MessageQueueIssue316, MessageQueueTableIssue316, OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL));
    ASSERT_TRUE(OCI_QueueStart(conn, MessageQueueIssue316, TRUE, TRUE));

    const auto typeJmsBytesMessage = OCI_TypeInfoGet(conn, JmsBytesMessageType, OCI_TIF_TYPE);
    ASSERT_NE(nullptr, typeJmsBytesMessage);

    const auto enqueue = OCI_EnqueueCreate(typeJmsBytesMessage, MessageQueueIssue316);
    ASSERT_NE(nullptr, enqueue);

    const auto dequeue = OCI_DequeueCreate(typeJmsBytesMessage, MessageQueueIssue316);
    ASSERT_NE(nullptr, dequeue);
 
    const auto rawMsg = Issue316_CreateRawMessage(conn);
    ASSERT_NE(nullptr, rawMsg);
 
    const auto lobMsg = Issue316_CreateLobMessage(conn);
    ASSERT_NE(nullptr, lobMsg);

    ASSERT_TRUE(OCI_EnqueuePut(enqueue, rawMsg));
    ASSERT_TRUE(OCI_EnqueuePut(enqueue, lobMsg));

    ASSERT_TRUE(OCI_Commit(conn));

    ASSERT_TRUE(OCI_MsgFree(rawMsg));
    ASSERT_TRUE(OCI_MsgFree(lobMsg));

    ASSERT_TRUE(Issue316_ReadMessages(conn, dequeue));

    ASSERT_TRUE(OCI_DequeueFree(dequeue));
    ASSERT_TRUE(OCI_EnqueueFree(enqueue));

    ASSERT_TRUE(OCI_QueueStop(conn, MessageQueueIssue316, TRUE, TRUE, FALSE));
    ASSERT_TRUE(OCI_QueueDrop(conn, MessageQueueIssue316));
    ASSERT_TRUE(OCI_QueueTableDrop(conn, MessageQueueTableIssue316, TRUE));

    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());
}

TEST(ReportedIssuesCApi, Issue326)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_THREADED));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);

    ASSERT_NE(nullptr, conn);

    ASSERT_TRUE(OCI_SetTimeout(conn, OCI_NTO_SEND, 1000));
    ASSERT_TRUE(OCI_SetTimeout(conn, OCI_NTO_RECEIVE, 2000));
    ASSERT_TRUE(OCI_SetTimeout(conn, OCI_NTO_CALL, 3000));

    ASSERT_EQ(1000, OCI_GetTimeout(conn, OCI_NTO_SEND));
    ASSERT_EQ(2000, OCI_GetTimeout(conn, OCI_NTO_RECEIVE));
    ASSERT_EQ(3000, OCI_GetTimeout(conn, OCI_NTO_CALL));


    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());
}


TEST(ReportedIssuesCApi, Issue347)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_THREADED));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto typeJmsBytesMessage = OCI_TypeInfoGet(conn, JmsBytesMessageType, OCI_TIF_TYPE);
    ASSERT_NE(nullptr, typeJmsBytesMessage);

    const auto msgRaw = OCI_MsgCreate(typeJmsBytesMessage);
    ASSERT_NE(nullptr, msgRaw);

    const auto enqueueDate = OCI_MsgGetEnqueueTime(msgRaw);
    ASSERT_NE(nullptr, enqueueDate);

    ASSERT_TRUE(OCI_MsgFree(msgRaw));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());
}

INSTANTIATE_TEST_CASE_P(ReportedIssuesCApi, ReportedIssues247, ::testing::ValuesIn(TimestampTypes));
