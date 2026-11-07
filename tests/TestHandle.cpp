#include "ocilib_tests.h"

namespace TestCApi
{
    TEST(TestHandle, HandleGetEnvironment)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_EQ(env, OCI_HandleGetEnvironment(env));
        ASSERT_EQ(env, OCI_HandleGetEnvironment(con));
        ASSERT_EQ(env, OCI_HandleGetEnvironment(stmt));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestHandle, UserData)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        OCI_Int32 value = 0x12345678;
        void* data = &value;

        ASSERT_TRUE(OCI_SetUserData(con, data));
        ASSERT_EQ(data, OCI_GetUserData(con));

        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    static void ExpectNullPointerError()
    {
        const auto err = OCI_GetLastError();

        ASSERT_NE(nullptr, err);
        ASSERT_EQ(OCI_ERR_NULL_POINTER, (OCI_Int32)OCI_ErrorGetCode(err));
    }

    /* Non-regression: every handle-based constructor called with a NULL
       environment/parent handle must fail gracefully (NULL return +
       OCI_ERR_NULL_POINTER recorded), not crash in the cleanup path. */

    TEST(TestHandle, NullEnvironmentConstructors)
    {
        ASSERT_NE(nullptr, GetEnvironment(nullptr));

        ASSERT_EQ(nullptr, OCI_ConnectionCreate(nullptr, DBS, USR, PWD, OCI_SESSION_DEFAULT));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_PoolCreate(nullptr, DBS, USR, PWD, OCI_POOL_CONNECTION, OCI_SESSION_DEFAULT, 1u, 1u, 0u));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_ShardingKeyCreate(nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_ConnectionCreateSharded(nullptr, DBS, USR, PWD, OCI_SESSION_DEFAULT, nullptr, nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_NumberCreate(nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_NumberArrayCreate(nullptr, 1u));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_DateCreate(nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_DateArrayCreate(nullptr, 1u));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_TimestampCreate(nullptr, OCI_TIMESTAMP));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_TimestampArrayCreate(nullptr, OCI_TIMESTAMP, 1u));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_IntervalCreate(nullptr, OCI_INTERVAL_YM));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_IntervalArrayCreate(nullptr, OCI_INTERVAL_YM, 1u));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_HashCreate(nullptr, OCI_HASH_DEFAULT_SIZE, OCI_HASH_STRING));
        ExpectNullPointerError();
    }

    TEST(TestHandle, NullParentConstructors)
    {
        ASSERT_NE(nullptr, GetEnvironment(nullptr));

        ASSERT_EQ(nullptr, OCI_TransactionCreate(nullptr, 0u, OCI_TRS_READWRITE, nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_StatementCreate(nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_CollCreate(nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_CollArrayCreate(nullptr, 1u));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_IterCreate(nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_ElemCreate(nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_LobCreate(nullptr, OCI_CLOB));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_LobArrayCreate(nullptr, OCI_CLOB, 1u));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_FileCreate(nullptr, OCI_BFILE));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_FileArrayCreate(nullptr, OCI_BFILE, 1u));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_LongCreate(nullptr, OCI_CLONG));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_ObjectCreate(nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_ObjectArrayCreate(nullptr, 1u));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_RefCreate(nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_RefArrayCreate(nullptr, 1u));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_VectorCreate(nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_VectorArrayCreate(nullptr, 1u));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_DirPathCreate(nullptr, nullptr, 0u, 0u));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_MsgCreate(nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_EnqueueCreate(nullptr, OTEXT("queue")));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_DequeueCreate(nullptr, OTEXT("queue")));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_AgentCreate(nullptr, OTEXT("agent"), nullptr));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_QueueCreate(nullptr, OTEXT("queue"), OTEXT("queue_table"),
                                     OCI_AQT_NORMAL, 0u, 0u, 0u, FALSE, nullptr));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_QueueTableCreate(nullptr, OTEXT("queue_table"), OTEXT("payload_type"),
                                          nullptr, nullptr, FALSE, OCI_AGM_NONE, nullptr, 0u, 0u, nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_SodaDbCreate(nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_SodaDbCreateColl(nullptr, OTEXT("coll"), nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_SodaDocCreateFromText(nullptr, OTEXT("{}"), 2u, nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_SodaDocCreateFromJson(nullptr, nullptr, nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_SodaOptionsCreate(nullptr));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_SodaCollCreateIndex(nullptr, OTEXT("{}")));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_JsonCreate(nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_JsonArrayCreate(nullptr, 1u));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_JsonNodeCreate(nullptr, OCI_JNT_OBJECT));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_JsonNodeCreateString(nullptr, OTEXT("value"), 5u));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_JsonNodeCreateNumber(nullptr, nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_JsonNodeCreateBoolean(nullptr, TRUE));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_JsonNodeCreateNull(nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_JsonNodeCreateBinary(nullptr, "x", 1u));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_JsonNodeCreateDate(nullptr, nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_JsonNodeCreateTimestamp(nullptr, nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_JsonNodeCreateInterval(nullptr, nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_JsonNodeCreateObject(nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_JsonNodeCreateArray(nullptr));
        ExpectNullPointerError();
    }

    /* Non-regression: public calls whose cleanup block dereferences the
       source handle (A1 — v4.9.1 reported OCI_ERR_NULL_POINTER cleanly,
       v5.0.0 segfaulted before the LABEL_CLEANUP source_ptr guard). */

    TEST(TestHandle, NullSourceCalls)
    {
        ASSERT_NE(nullptr, GetEnvironment(nullptr));

        otext buffer[64] = { 0 };

        ASSERT_EQ(nullptr, OCI_HandleGetEnvironment(nullptr));

        ASSERT_FALSE(OCI_DateToText(nullptr, OTEXT("YYYY-MM-DD"), 64, buffer));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_DateFromText(nullptr, OTEXT("2024-01-01"), OTEXT("YYYY-MM-DD")));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_TimestampToText(nullptr, OTEXT("YYYY-MM-DD"), 64, buffer, 0));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_IntervalFromText(nullptr, OTEXT("+1-2")));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_FileSetName(nullptr, OTEXT("dir"), OTEXT("name")));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_SetPassword(nullptr, OTEXT("pwd")));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_SetTrace(nullptr, OCI_TRC_MODULE, OTEXT("module")));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_DirPathSetDateFormat(nullptr, OTEXT("YYYY")));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_Prepare(nullptr, OTEXT("select 1 from dual")));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_Execute(nullptr));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_BindObject(nullptr, OTEXT("bind"), nullptr));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_EnqueuePut(nullptr, nullptr));
        ExpectNullPointerError();

        ASSERT_EQ(nullptr, OCI_DequeueGet(nullptr));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_ConnectionFree(nullptr));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_StatementFree(nullptr));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_DateFree(nullptr));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_TypeInfoFree(nullptr));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_SubscriptionUnregister(nullptr));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_JsonClone(nullptr, nullptr));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_JsonLoadText(nullptr, OTEXT("{}"), 2u, OCI_JTV_ALLOW_NONE, OCI_JTE_DETECT));
        ExpectNullPointerError();

        ASSERT_EQ(0u, OCI_JsonNodeGetFieldCount(nullptr));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_JsonNodeGetBoolean(nullptr));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_JsonNodeHasField(nullptr, OTEXT("field")));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_JsonNodeSetField(nullptr, OTEXT("field"), nullptr));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_JsonNodeSetString(nullptr, OTEXT("value"), 5u));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_JsonNodeRemoveField(nullptr, OTEXT("field")));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_JsonNodeRenameField(nullptr, OTEXT("old"), OTEXT("new")));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_JsonNodeClear(nullptr));
        ExpectNullPointerError();

        ASSERT_FALSE(OCI_EnvironmentFree(nullptr));
        ExpectNullPointerError();
    }
}
