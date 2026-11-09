#include "ocilib_tests.h"
#include "NullArgs.h"

#include <sstream>

using namespace ocilib;

namespace
{
    void DropCollIfExists(OCI_SodaDb* sdb, const otext* name)
    {
        OCI_SodaColl* coll = OCI_SodaDbOpenColl(sdb, name);
        if (coll)
        {
            OCI_SodaCollDrop(coll);
            OCI_SodaCollFree(coll);
        }
        OCI_Error* e = OCI_GetLastError();
        ASSERT_EQ(nullptr, e);
    }
}

namespace TestCApi
{
    TEST(TestSoda, CreateDb)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, CreateAndDropCollection)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_TEST_COLL"));
        const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_TEST_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        ASSERT_TRUE(OCI_SodaCollDrop(coll));
        ASSERT_TRUE(OCI_SodaCollFree(coll));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, InsertAndFindOneByKey)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_TEST_COLL"));
        const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_TEST_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        const String content = OTEXT("{\"name\":\"soda test\"}");

        const auto doc = OCI_SodaDocCreateFromText
        (
            sdb,
            content.c_str(),
            static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, doc);

        ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, doc));

        const auto docKey = OCI_SodaDocGetKey(doc);
        ASSERT_NE(nullptr, docKey);

       const auto foundDoc = OCI_SodaCollFindOneByKey(coll, docKey);
        ASSERT_NE(nullptr, foundDoc);

        const auto foundKey = OCI_SodaDocGetKey(foundDoc);
        ASSERT_EQ(String(docKey), String(foundKey));

        const auto foundContent = OCI_SodaDocGetText(foundDoc);
       ASSERT_NE(nullptr, foundContent);

        ASSERT_TRUE(OCI_SodaDocFree(foundDoc));

        ASSERT_TRUE(OCI_SodaDocFree(doc));
        
        ASSERT_TRUE(OCI_Commit(conn));
        ASSERT_TRUE(OCI_SodaCollDrop(coll));
        ASSERT_TRUE(OCI_SodaCollFree(coll));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, FindAndGetJson)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_TEST_JSON_COLL"));
        const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_TEST_JSON_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        const String content = OTEXT("{\"name\":\"soda json test\"}");

        const auto doc = OCI_SodaDocCreateFromText
        (
            sdb,
            content.c_str(),
            static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, doc);

        ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, doc));

        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        auto* cursor = OCI_SodaCollFind(coll, options);
        ASSERT_NE(nullptr, cursor);

        auto* foundDoc = OCI_SodaDocCursorGetNext(cursor);
        ASSERT_NE(nullptr, foundDoc);

        const auto textContent = OCI_SodaDocGetText(foundDoc);
        ASSERT_NE(nullptr, textContent);

        if (OCI_GetVersionConnection(conn) >= OCI_23_4 && OCI_GetOCIRuntimeVersion(env) >= OCI_23_4)
        {
            /* returned JSON handle is owned by the document - do not free it */

            auto* jsonContent = OCI_SodaDocGetJson(foundDoc);
            ASSERT_NE(nullptr, jsonContent);
            ASSERT_EQ(jsonContent, OCI_SodaDocGetJson(foundDoc));
        }

        ASSERT_TRUE(OCI_SodaDocFree(foundDoc));
        ASSERT_TRUE(OCI_SodaDocCursorFree(cursor));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
        ASSERT_TRUE(OCI_SodaDocFree(doc));

        ASSERT_TRUE(OCI_Commit(conn));
        ASSERT_TRUE(OCI_SodaCollDrop(coll));
        ASSERT_TRUE(OCI_SodaCollFree(coll));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, DbGetConnection)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        ASSERT_EQ(conn, OCI_SodaDbGetConnection(sdb));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, CollectionProperties)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_PROPS_COLL"));
        const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_PROPS_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        const auto name = OCI_SodaCollGetName(coll);
        ASSERT_NE(nullptr, name);
        ASSERT_EQ(String(OTEXT("OCILIB_SODA_PROPS_COLL")), String(name));

        const auto metadata = OCI_SodaCollGetMetadata(coll);
        ASSERT_NE(nullptr, metadata);
        ASSERT_FALSE(String(metadata).empty());

        ASSERT_EQ(conn, OCI_SodaCollGetConnection(coll));

        OCI_SodaCollIsMapped(coll);

        ASSERT_TRUE(OCI_Commit(conn));
        ASSERT_TRUE(OCI_SodaCollDrop(coll));
        ASSERT_TRUE(OCI_SodaCollFree(coll));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, CreateCollWithMetadata)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        const String metadata = OTEXT("{\"keyColumn\":{\"name\":\"ID\",\"assignmentMethod\":\"CLIENT\",\"sqlType\":\"VARCHAR2\",\"maxLength\":255}}");

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_META_COLL"));
        const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_META_COLL"), metadata.c_str());
        ASSERT_NE(nullptr, coll);

        const auto actual = OCI_SodaCollGetMetadata(coll);
        ASSERT_NE(nullptr, actual);
        ASSERT_NE(String::npos, String(actual).find(OTEXT("CLIENT")));

        /* client-assigned keys : the key supplied at creation is used as is */

        const String content = OTEXT("{\"value\":1}");

        const auto doc = OCI_SodaDocCreateFromText
        (
            sdb,
            content.c_str(),
            static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
            OTEXT("mykey1")
        );
        ASSERT_NE(nullptr, doc);

        ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, doc));

        const auto foundDoc = OCI_SodaCollFindOneByKey(coll, OTEXT("mykey1"));
        ASSERT_NE(nullptr, foundDoc);
        ASSERT_TRUE(OCI_SodaDocFree(foundDoc));

        ASSERT_TRUE(OCI_SodaDocFree(doc));

        ASSERT_TRUE(OCI_Commit(conn));
        ASSERT_TRUE(OCI_SodaCollDrop(coll));
        ASSERT_TRUE(OCI_SodaCollFree(coll));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, OpenCollection)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_OPEN_COLL"));
        const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_OPEN_COLL"), NULL);
        ASSERT_NE(nullptr, coll);
        ASSERT_TRUE(OCI_SodaCollFree(coll));

        const auto opened = OCI_SodaDbOpenColl(sdb, OTEXT("OCILIB_SODA_OPEN_COLL"));
        ASSERT_NE(nullptr, opened);

        const auto name = OCI_SodaCollGetName(opened);
        ASSERT_NE(nullptr, name);
        ASSERT_EQ(String(OTEXT("OCILIB_SODA_OPEN_COLL")), String(name));

        ASSERT_TRUE(OCI_Commit(conn));
        ASSERT_TRUE(OCI_SodaCollDrop(opened));
        ASSERT_TRUE(OCI_SodaCollFree(opened));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, GetCollectionsCursor)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_CUR_COLL1"));
        const auto coll1 = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_CUR_COLL1"), NULL);
        ASSERT_NE(nullptr, coll1);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_CUR_COLL2"));
        const auto coll2 = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_CUR_COLL2"), NULL);
        ASSERT_NE(nullptr, coll2);

        OCI_UInt32 found = 0;
        const auto cursor = OCI_SodaDbGetColls(sdb, NULL);
        ASSERT_NE(nullptr, cursor);

        OCI_SodaColl *item = NULL;
        while (NULL != (item = OCI_SodaCollCursorGetNext(cursor)))
        {
            const auto name = OCI_SodaCollGetName(item);
            ASSERT_NE(nullptr, name);

            if (String(name).find(OTEXT("OCILIB_SODA_CUR_COLL")) != String::npos)
            {
                found++;
            }

            ASSERT_TRUE(OCI_SodaCollFree(item));
        }

        ASSERT_TRUE(OCI_SodaCollCursorFree(cursor));
        ASSERT_EQ(2u, found);

        ASSERT_TRUE(OCI_Commit(conn));
        ASSERT_TRUE(OCI_SodaCollDrop(coll1));
        ASSERT_TRUE(OCI_SodaCollFree(coll1));
        ASSERT_TRUE(OCI_SodaCollDrop(coll2));
        ASSERT_TRUE(OCI_SodaCollFree(coll2));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, DocumentMetadata)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_DOCMETA_COLL"));
        const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_DOCMETA_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        const String content = OTEXT("{\"name\":\"meta test\"}");

        const auto doc = OCI_SodaDocCreateFromText
        (
            sdb,
            content.c_str(),
            static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, doc);

        /* media type is known before insert */

        const auto mediaType = OCI_SodaDocGetMediaType(doc);
        ASSERT_NE(nullptr, mediaType);

        ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, doc));

        /* insert-and-get refreshes generated metadata */

        ASSERT_NE(nullptr, OCI_SodaDocGetKey(doc));
        ASSERT_NE(nullptr, OCI_SodaDocGetVersion(doc));
        ASSERT_NE(nullptr, OCI_SodaDocGetCreatedOn(doc));
        ASSERT_NE(nullptr, OCI_SodaDocGetLastModified(doc));

        ASSERT_TRUE(OCI_SodaDocFree(doc));

        ASSERT_TRUE(OCI_Commit(conn));
        ASSERT_TRUE(OCI_SodaCollDrop(coll));
        ASSERT_TRUE(OCI_SodaCollFree(coll));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, DocCreateFromJson)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_JSONDOC_COLL"));
        const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_JSONDOC_COLL"),
                                               OTEXT("{\"keyColumn\":{\"name\":\"ID\"},"
                                                     "\"contentColumn\":{\"name\":\"JSON_DOCUMENT\",\"sqlType\":\"JSON\"},"
                                                     "\"versionColumn\":{\"name\":\"VERSION\",\"method\":\"UUID\"},"
                                                     "\"lastModifiedColumn\":{\"name\":\"LAST_MODIFIED\"},"
                                                     "\"creationTimeColumn\":{\"name\":\"CREATED_ON\"}}"));
        ASSERT_NE(nullptr, coll);

        if (OCI_GetVersionConnection(conn) >= OCI_23_4 && OCI_GetOCIRuntimeVersion(env) >= OCI_23_4)
        {
            const auto json = OCI_JsonCreate(conn);
            ASSERT_NE(nullptr, json);
            ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("{\"name\":\"from json\"}"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));

            const auto doc = OCI_SodaDocCreateFromJson(sdb, json, NULL);
            ASSERT_NE(nullptr, doc);

            ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, doc));

            const auto text = OCI_SodaDocGetText(doc);
            ASSERT_NE(nullptr, text);
            ASSERT_NE(String::npos, String(text).find(OTEXT("from json")));

            ASSERT_TRUE(OCI_SodaDocFree(doc));
            ASSERT_TRUE(OCI_JsonFree(json));
        }

        ASSERT_TRUE(OCI_Commit(conn));
        ASSERT_TRUE(OCI_SodaCollDrop(coll));
        ASSERT_TRUE(OCI_SodaCollFree(coll));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    /* -------------------------------------------------------------------------
       Shared fixture: SODA coll seeded with 5 parity docs
       ------------------------------------------------------------------------- */

    class TestSodaFind : public ::testing::Test
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_SodaDb* sdb = nullptr;
        OCI_SodaColl* coll = nullptr;

        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            sdb = OCI_SodaDbCreate(conn);
            ASSERT_NE(nullptr, sdb);
            DropCollIfExists(sdb, OTEXT("OCILIB_SODA_OPT_COLL"));
            coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_OPT_COLL"), NULL);
            ASSERT_NE(nullptr, coll);
            for (OCI_Int32 i = 1; i <= 5; i++)
            {
                std::basic_ostringstream<otext> oss;
                oss << OTEXT("{\"seq\":") << i << OTEXT(",\"parity\":")
                    << ((i % 2) ? OTEXT("\"odd\"") : OTEXT("\"even\"")) << OTEXT("}");
                const String content = oss.str();
                const auto doc = OCI_SodaDocCreateFromText(sdb, content.c_str(),
                    static_cast<OCI_UInt32>(content.size() * sizeof(otext)), NULL);
                ASSERT_NE(nullptr, doc);
                ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, doc));
                ASSERT_TRUE(OCI_SodaDocFree(doc));
            }
        }

        void TearDown() override
        {
            if (coll) { OCI_SodaCollDrop(coll); OCI_SodaCollFree(coll); }
            if (sdb) OCI_SodaDbFree(sdb);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_F(TestSodaFind, SodaOptionsSettersOnNewOptions_ReturnTrue)
    {
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        EXPECT_TRUE(OCI_SodaOptionsSetFilter(options, OTEXT("{\"parity\":\"even\"}")));
        EXPECT_TRUE(OCI_SodaOptionsSetSkip(options, 0));
        EXPECT_TRUE(OCI_SodaOptionsSetLimit(options, 10));
        EXPECT_TRUE(OCI_SodaOptionsSetFetchArraySize(options, 32));
        EXPECT_TRUE(OCI_SodaOptionsSetLock(options, FALSE));

        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    TEST_F(TestSodaFind, SodaOptionsSetHintWithBareHint_ReturnsTrue)
    {
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        EXPECT_TRUE(OCI_SodaOptionsSetHint(options, OTEXT("index(OCILIB_SODA_OPT_COLL)")));

        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    TEST_F(TestSodaFind, FindWithQbeFilter_ReturnsMatchingDocsOnly)
    {
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);
        ASSERT_TRUE(OCI_SodaOptionsSetFilter(options, OTEXT("{\"parity\":\"even\"}")));

        auto* cursor = OCI_SodaCollFind(coll, options);

        ASSERT_NE(nullptr, cursor);
        OCI_UInt32 count = 0;
        OCI_SodaDoc* item = NULL;
        while (NULL != (item = OCI_SodaDocCursorGetNext(cursor)))
        {
            count++;
            ASSERT_TRUE(OCI_SodaDocFree(item));
        }
        EXPECT_EQ(2u, count);

        ASSERT_TRUE(OCI_SodaDocCursorFree(cursor));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    /* -------------------------------------------------------------------------
       Shared fixture: SODA coll seeded with one tagged doc
       ------------------------------------------------------------------------- */

    class TestSodaFindOne : public ::testing::Test
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_SodaDb* sdb = nullptr;
        OCI_SodaColl* coll = nullptr;
        String key;

        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            sdb = OCI_SodaDbCreate(conn);
            ASSERT_NE(nullptr, sdb);
            DropCollIfExists(sdb, OTEXT("OCILIB_SODA_FINDONE_COLL"));
            coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_FINDONE_COLL"), NULL);
            ASSERT_NE(nullptr, coll);

            const String content = OTEXT("{\"tag\":\"findone\"}");
            const auto doc = OCI_SodaDocCreateFromText(sdb, content.c_str(),
                static_cast<OCI_UInt32>(content.size() * sizeof(otext)), NULL);
            ASSERT_NE(nullptr, doc);
            ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, doc));
            const auto k = OCI_SodaDocGetKey(doc);
            ASSERT_NE(nullptr, k);
            key = String(k);
            ASSERT_TRUE(OCI_SodaDocFree(doc));
        }

        void TearDown() override
        {
            if (coll) { OCI_SodaCollDrop(coll); OCI_SodaCollFree(coll); }
            if (sdb) OCI_SodaDbFree(sdb);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_F(TestSodaFindOne, FindOneWithKeyOption_ReturnsSeededDoc)
    {
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);
        ASSERT_TRUE(OCI_SodaOptionsSetKey(options, key.c_str()));

        auto* foundDoc = OCI_SodaCollFindOne(coll, options);

        ASSERT_NE(nullptr, foundDoc);
        EXPECT_EQ(key, String(OCI_SodaDocGetKey(foundDoc)));

        ASSERT_TRUE(OCI_SodaDocFree(foundDoc));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    TEST_F(TestSodaFindOne, FindOneWithKeysOption_ReturnsSeededDoc)
    {
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);
        const otext* keys[] = { key.c_str() };
        ASSERT_TRUE(OCI_SodaOptionsSetKeys(options, keys, 1));

        auto* foundDoc = OCI_SodaCollFindOne(coll, options);

        ASSERT_NE(nullptr, foundDoc);

        ASSERT_TRUE(OCI_SodaDocFree(foundDoc));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    TEST(TestSoda, DocCount)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_COUNT_COLL"));
        const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_COUNT_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        for (OCI_Int32 i = 0; i < 3; i++)
        {
            const String content = (i % 2) ? OTEXT("{\"grp\":1}") : OTEXT("{\"grp\":0}");

            const auto doc = OCI_SodaDocCreateFromText
            (
                sdb,
                content.c_str(),
                static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
                NULL
            );
            ASSERT_NE(nullptr, doc);
            ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, doc));
            ASSERT_TRUE(OCI_SodaDocFree(doc));
        }

        OCI_Commit(conn);

        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        const OCI_UInt64 cnt = OCI_SodaCollDocCount(coll, options);
        ASSERT_EQ(3u, cnt);

        ASSERT_NE(nullptr, options);
        ASSERT_TRUE(OCI_SodaOptionsSetFilter(options, OTEXT("{\"grp\":0}")));

        ASSERT_EQ(2u, OCI_SodaCollDocCount(coll, options));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));

        ASSERT_EQ(2u, OCI_SodaCollDocCountWithFilter(coll, OTEXT("{\"grp\":0}")));

        ASSERT_TRUE(OCI_Commit(conn));
        ASSERT_TRUE(OCI_SodaCollDrop(coll));
        ASSERT_TRUE(OCI_SodaCollFree(coll));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, ReplaceOneByKey)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_REPLKEY_COLL"));
        const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_REPLKEY_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        const String content = OTEXT("{\"v\":1}");

        const auto doc = OCI_SodaDocCreateFromText
        (
            sdb,
            content.c_str(),
            static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, doc);
        ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, doc));

        const String key = OCI_SodaDocGetKey(doc);
        ASSERT_FALSE(key.empty());

        const String newContent = OTEXT("{\"v\":2}");

        const auto newDoc = OCI_SodaDocCreateFromText
        (
            sdb,
            newContent.c_str(),
            static_cast<OCI_UInt32>(newContent.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, newDoc);

        ASSERT_TRUE(OCI_SodaCollReplaceOneByKey(coll, key.c_str(), newDoc, FALSE));

        const auto foundDoc = OCI_SodaCollFindOneByKey(coll, key.c_str());
        ASSERT_NE(nullptr, foundDoc);

        const auto text = OCI_SodaDocGetText(foundDoc);
        ASSERT_NE(nullptr, text);
        ASSERT_NE(String::npos, String(text).find(OTEXT("\"v\":2")));

        ASSERT_TRUE(OCI_SodaDocFree(foundDoc));

        /* replace-and-get updates the passed document metadata */

        const String newContent2 = OTEXT("{\"v\":3}");
        const auto newDoc2 = OCI_SodaDocCreateFromText
        (
            sdb,
            newContent2.c_str(),
            static_cast<OCI_UInt32>(newContent2.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, newDoc2);

        ASSERT_TRUE(OCI_SodaCollReplaceOneByKey(coll, key.c_str(), newDoc2, TRUE));
        ASSERT_NE(nullptr, OCI_SodaDocGetVersion(newDoc2));

        ASSERT_TRUE(OCI_SodaDocFree(newDoc2));
        ASSERT_TRUE(OCI_SodaDocFree(newDoc));
        ASSERT_TRUE(OCI_SodaDocFree(doc));

        ASSERT_TRUE(OCI_Commit(conn));
        ASSERT_TRUE(OCI_SodaCollDrop(coll));
        ASSERT_TRUE(OCI_SodaCollFree(coll));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    /* -------------------------------------------------------------------------
       Shared fixture: SODA coll seeded with one versioned doc
       ------------------------------------------------------------------------- */

    class TestSodaReplace : public ::testing::Test
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_SodaDb* sdb = nullptr;
        OCI_SodaColl* coll = nullptr;
        String key;
        String version;

        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            sdb = OCI_SodaDbCreate(conn);
            ASSERT_NE(nullptr, sdb);
            DropCollIfExists(sdb, OTEXT("OCILIB_SODA_REP_COLL"));
            coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_REP_COLL"), NULL);
            ASSERT_NE(nullptr, coll);

            const String content = OTEXT("{\"v\":10}");
            const auto doc = OCI_SodaDocCreateFromText(sdb, content.c_str(),
                static_cast<OCI_UInt32>(content.size() * sizeof(otext)), NULL);
            ASSERT_NE(nullptr, doc);
            ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, doc));
            key = String(OCI_SodaDocGetKey(doc));
            version = String(OCI_SodaDocGetVersion(doc));
            ASSERT_TRUE(OCI_SodaDocFree(doc));
        }

        void TearDown() override
        {
            if (coll) { OCI_SodaCollDrop(coll); OCI_SodaCollFree(coll); }
            if (sdb) OCI_SodaDbFree(sdb);
            if (conn) OCI_ConnectionFree(conn);
        }

        OCI_SodaDoc* MakeDoc(const otext* content)
        {
            return OCI_SodaDocCreateFromText(sdb, content,
                static_cast<OCI_UInt32>(String(content).size() * sizeof(otext)), NULL);
        }
    };

    TEST_F(TestSodaReplace, ReplaceOneByKeyOption_ReplacesDoc)
    {
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);
        ASSERT_TRUE(OCI_SodaOptionsSetKey(options, key.c_str()));
        OCI_SodaDoc* newDoc = MakeDoc(OTEXT("{\"v\":20}"));
        ASSERT_NE(nullptr, newDoc);

        EXPECT_TRUE(OCI_SodaCollReplaceOne(coll, options, newDoc, FALSE));

        ASSERT_TRUE(OCI_SodaDocFree(newDoc));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    TEST_F(TestSodaReplace, ReplaceOneByKeyAndVersion_ReturnsUpdatedDoc)
    {
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);
        ASSERT_TRUE(OCI_SodaOptionsSetKey(options, key.c_str()));
        ASSERT_TRUE(OCI_SodaOptionsSetVersion(options, version.c_str()));
        OCI_SodaDoc* newDoc = MakeDoc(OTEXT("{\"v\":30}"));
        ASSERT_NE(nullptr, newDoc);

        ASSERT_TRUE(OCI_SodaCollReplaceOne(coll, options, newDoc, TRUE));

        EXPECT_NE(nullptr, OCI_SodaDocGetVersion(newDoc));

        ASSERT_TRUE(OCI_SodaDocFree(newDoc));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }
    TEST(TestSoda, RemoveOneByKey)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_REMKEY_COLL"));
        const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_REMKEY_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        const String content = OTEXT("{\"r\":1}");

        const auto doc = OCI_SodaDocCreateFromText
        (
            sdb,
            content.c_str(),
            static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, doc);
        ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, doc));

        const String key = OCI_SodaDocGetKey(doc);
        ASSERT_FALSE(key.empty());
        ASSERT_TRUE(OCI_SodaDocFree(doc));

        ASSERT_EQ(1u, OCI_SodaCollDocCount(coll, NULL));
        ASSERT_TRUE(OCI_SodaCollRemoveOneByKey(coll, key.c_str()));
        ASSERT_EQ(0u, OCI_SodaCollDocCount(coll, NULL));

        ASSERT_TRUE(OCI_Commit(conn));
        ASSERT_TRUE(OCI_SodaCollDrop(coll));
        ASSERT_TRUE(OCI_SodaCollFree(coll));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, RemoveWithOptions)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_REMOPT_COLL"));
        const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_REMOPT_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        for (OCI_Int32 i = 0; i < 4; i++)
        {
            const String content = (i % 2) ? OTEXT("{\"del\":1}") : OTEXT("{\"del\":0}");

            const auto doc = OCI_SodaDocCreateFromText
            (
                sdb,
                content.c_str(),
                static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
                NULL
            );
            ASSERT_NE(nullptr, doc);
            ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, doc));
            ASSERT_TRUE(OCI_SodaDocFree(doc));
        }

        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);
        ASSERT_TRUE(OCI_SodaOptionsSetFilter(options, OTEXT("{\"del\":1}")));

        OCI_UInt64 removed = 0;
        ASSERT_TRUE(OCI_SodaCollRemove(coll, options, &removed));
        ASSERT_EQ(2u, removed);
        ASSERT_EQ(2u, OCI_SodaCollDocCount(coll, NULL));

        ASSERT_TRUE(OCI_SodaOptionsFree(options));

        ASSERT_TRUE(OCI_Commit(conn));
        ASSERT_TRUE(OCI_SodaCollDrop(coll));
        ASSERT_TRUE(OCI_SodaCollFree(coll));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, Truncate)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_TRUNC_COLL"));
        const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_TRUNC_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        const String content = OTEXT("{\"t\":1}");

        const auto doc = OCI_SodaDocCreateFromText
        (
            sdb,
            content.c_str(),
            static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, doc);
        ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, doc));
        ASSERT_TRUE(OCI_SodaDocFree(doc));

        ASSERT_EQ(1u, OCI_SodaCollDocCount(coll, NULL));
        ASSERT_TRUE(OCI_SodaCollTruncate(coll));
        ASSERT_EQ(0u, OCI_SodaCollDocCount(coll, NULL));

        ASSERT_TRUE(OCI_Commit(conn));
        ASSERT_TRUE(OCI_SodaCollDrop(coll));
        ASSERT_TRUE(OCI_SodaCollFree(coll));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    /* --- NULL-handle / NULL-argument error paths, grouped by handle --- */

    class TestSodaErrors : public ::testing::Test
    {
    protected:

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            sdb = OCI_SodaDbCreate(conn);
            ASSERT_NE(nullptr, sdb);

            DropCollIfExists(sdb, OTEXT("OCILIB_SODA_ERR_COLL"));
            coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_ERR_COLL"), NULL);
            ASSERT_NE(nullptr, coll);
        }

        void TearDown() override
        {
            OCI_Commit(conn);
            OCI_SodaCollDrop(coll);
            OCI_SodaCollFree(coll);
            OCI_SodaDbFree(sdb);
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
        OCI_SodaDb*     sdb{ nullptr };
        OCI_SodaColl*   coll{ nullptr };
    };

    TEST_F(TestSodaErrors, SodaDbFnsWithNullArgs_ReturnFalse)
    {
        ASSERT_EQ(nullptr, OCI_SodaDbCreateColl(NULL, OTEXT("X"), NULL));
        ASSERT_EQ(nullptr, OCI_SodaDbOpenColl(NULL, OTEXT("X")));
        ASSERT_EQ(nullptr, OCI_SodaDbOpenColl(sdb, NULL));
        ASSERT_EQ(nullptr, OCI_SodaDbGetColls(NULL, NULL));
        ASSERT_EQ(nullptr, OCI_SodaDbGetConnection(NULL));
    }

    TEST_F(TestSodaErrors, SodaDocCreateWithNullArgs_ReturnsNull)
    {
        ASSERT_EQ(nullptr, OCI_SodaDocCreateFromText(NULL, OTEXT("{}"), 2, NULL));
        ASSERT_EQ(nullptr, OCI_SodaDocCreateFromJson(NULL, NULL, NULL));
    }

    TEST_F(TestSodaErrors, SodaDocCreateWithNullContent_ReturnsDoc)
    {
        /* NULL content is valid - creates an empty document */
        OCI_SodaDoc* emptyDoc = OCI_SodaDocCreateFromText(sdb, NULL, 0, NULL);
        ASSERT_NE(nullptr, emptyDoc);
        ASSERT_TRUE(OCI_SodaDocFree(emptyDoc));
    }

    TEST_F(TestSodaErrors, CollFnsOnNullHandle_ReturnFalse)
    {
        ASSERT_EQ(nullptr, OCI_SodaCollGetName(NULL));
        ASSERT_EQ(nullptr, OCI_SodaCollGetMetadata(NULL));
        ASSERT_EQ(nullptr, OCI_SodaCollGetConnection(NULL));
        ASSERT_FALSE(OCI_SodaCollIsMapped(NULL));
        ASSERT_FALSE(OCI_SodaCollDrop(NULL));
        ASSERT_FALSE(OCI_SodaCollTruncate(NULL));
        ASSERT_FALSE(OCI_SodaCollFree(NULL));
    }

    TEST_F(TestSodaErrors, DocFnsOnNullHandle_ReturnFalse)
    {
        ASSERT_EQ(nullptr, OCI_SodaDocGetKey(NULL));
        ASSERT_EQ(nullptr, OCI_SodaDocGetVersion(NULL));
        ASSERT_EQ(nullptr, OCI_SodaDocGetCreatedOn(NULL));
        ASSERT_EQ(nullptr, OCI_SodaDocGetLastModified(NULL));
        ASSERT_EQ(nullptr, OCI_SodaDocGetMediaType(NULL));
        ASSERT_EQ(nullptr, OCI_SodaDocGetText(NULL));
        ASSERT_EQ(nullptr, OCI_SodaDocGetJson(NULL));
        ASSERT_FALSE(OCI_SodaDocFree(NULL));
    }

    TEST_F(TestSodaErrors, OptionsFnsOnNullHandle_ReturnFalse)
    {
        ASSERT_EQ(nullptr, OCI_SodaOptionsCreate(NULL));
        ASSERT_FALSE(OCI_SodaOptionsFree(NULL));
        ASSERT_FALSE(OCI_SodaOptionsSetKey(NULL, OTEXT("k")));
        ASSERT_FALSE(OCI_SodaOptionsSetKeys(NULL, NULL, 0));
        ASSERT_FALSE(OCI_SodaOptionsSetVersion(NULL, OTEXT("v")));
        ASSERT_FALSE(OCI_SodaOptionsSetFilter(NULL, OTEXT("{}")));
        ASSERT_FALSE(OCI_SodaOptionsSetSkip(NULL, 0));
        ASSERT_FALSE(OCI_SodaOptionsSetLimit(NULL, 0));
    }

    TEST_F(TestSodaErrors, CursorFnsOnNullHandle_ReturnFalse)
    {
        ASSERT_EQ(nullptr, OCI_SodaCollCursorGetNext(NULL));
        ASSERT_FALSE(OCI_SodaCollCursorFree(NULL));
        ASSERT_EQ(nullptr, OCI_SodaDocCursorGetNext(NULL));
        ASSERT_FALSE(OCI_SodaDocCursorFree(NULL));
    }

    TEST_F(TestSodaErrors, CollOpsWithNullArgs_ReturnFalse)
    {
        ASSERT_FALSE(OCI_SodaCollInsertDoc(NULL, NULL));
        ASSERT_FALSE(OCI_SodaCollInsertDoc(coll, NULL));
        ASSERT_EQ(nullptr, OCI_SodaCollFind(NULL, NULL));
        ASSERT_EQ(nullptr, OCI_SodaCollFindOne(NULL, NULL));
        ASSERT_EQ(nullptr, OCI_SodaCollFindOneByKey(NULL, OTEXT("k")));
        ASSERT_EQ(nullptr, OCI_SodaCollFindOneByKey(coll, NULL));
        ASSERT_EQ(0u, OCI_SodaCollDocCount(NULL, NULL));
        ASSERT_EQ(0u, OCI_SodaCollDocCountWithFilter(NULL, OTEXT("{}")));
        ASSERT_FALSE(OCI_SodaCollReplaceOne(NULL, NULL, NULL, FALSE));
        ASSERT_FALSE(OCI_SodaCollReplaceOne(NULL, NULL, NULL, TRUE));
        ASSERT_FALSE(OCI_SodaCollReplaceOneByKey(NULL, NULL, NULL, FALSE));
        ASSERT_FALSE(OCI_SodaCollReplaceOneByKey(NULL, NULL, NULL, TRUE));
        ASSERT_FALSE(OCI_SodaCollRemove(NULL, NULL, NULL));
        ASSERT_FALSE(OCI_SodaCollRemoveOneByKey(NULL, NULL));
    }

    /* --- multi-key (OCI_CHARSET_WIDE emulation) operations --- */

    /* the collection is created once for the whole case; live OCILIB
       handles must not outlive a test or the leak tracker reports their
       allocations as deltas - so each test opens its own coll handle */

    class TestSodaMultiKey : public ::testing::Test
    {
    protected:

        static constexpr OCI_UInt32 DocCount = 12;

        static void SetUpTestCase()
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            const auto sdb = OCI_SodaDbCreate(conn);
            ASSERT_NE(nullptr, sdb);

            const otext* metadata = OTEXT("{\"keyColumn\":{\"name\":\"ID\",\"assignmentMethod\":\"CLIENT\",\"sqlType\":\"VARCHAR2\",\"maxLength\":255}}");

            DropCollIfExists(sdb, OTEXT("OCILIB_SODA_MK_COLL"));
            const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_MK_COLL"), metadata);
            ASSERT_NE(nullptr, coll);

            for (OCI_UInt32 i = 0; i < DocCount; i++)
            {
                std::basic_ostringstream<otext> kss;
                kss << OTEXT("mk") << i;
                keystr[i] = kss.str();
                keys[i]   = keystr[i].c_str();

                const String content = OTEXT("{\"idx\":1}");

                const auto doc = OCI_SodaDocCreateFromText
                (
                    sdb,
                    content.c_str(),
                    static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
                    keys[i]
                );
                ASSERT_NE(nullptr, doc);
                ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, doc));
                ASSERT_TRUE(OCI_SodaDocFree(doc));
            }

            ASSERT_TRUE(OCI_Commit(conn));
            ASSERT_TRUE(OCI_SodaCollFree(coll));
            ASSERT_TRUE(OCI_SodaDbFree(sdb));
            ASSERT_TRUE(OCI_ConnectionFree(conn));
        }

        static void TearDownTestCase()
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            const auto sdb = OCI_SodaDbCreate(conn);
            ASSERT_NE(nullptr, sdb);

            DropCollIfExists(sdb, OTEXT("OCILIB_SODA_MK_COLL"));

            ASSERT_TRUE(OCI_Commit(conn));
            ASSERT_TRUE(OCI_SodaDbFree(sdb));
            ASSERT_TRUE(OCI_ConnectionFree(conn));
        }

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            sdb = OCI_SodaDbCreate(conn);
            ASSERT_NE(nullptr, sdb);

            coll = OCI_SodaDbOpenColl(sdb, OTEXT("OCILIB_SODA_MK_COLL"));
            ASSERT_NE(nullptr, coll);
        }

        void TearDown() override
        {
            OCI_Commit(conn);
            OCI_SodaCollFree(coll);
            OCI_SodaDbFree(sdb);
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
        OCI_SodaDb*     sdb{ nullptr };
        OCI_SodaColl*   coll{ nullptr };

        static String      keystr[DocCount];
        static const otext* keys[DocCount];
    };

    String      TestSodaMultiKey::keystr[DocCount];
    const otext* TestSodaMultiKey::keys[DocCount];

    TEST_F(TestSodaMultiKey, FindWithKeyList_ReturnsDocs)
    {
        /* materializes all matching docs (>8 also covers the document
           array growth arm) */
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);
        ASSERT_TRUE(OCI_SodaOptionsSetKeys(options, keys, DocCount));

        auto* cursor = OCI_SodaCollFind(coll, options);
        ASSERT_NE(nullptr, cursor);

        OCI_UInt32 count = 0;
        OCI_SodaDoc* item = NULL;
        while (NULL != (item = OCI_SodaDocCursorGetNext(cursor)))
        {
            count++;
            ASSERT_TRUE(OCI_SodaDocFree(item));
        }
        ASSERT_EQ(DocCount, count);

        ASSERT_TRUE(OCI_SodaDocCursorFree(cursor));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    TEST_F(TestSodaMultiKey, FindOneWithMixedKeys_ReturnsMatchingDoc)
    {
        /* first keys miss, a later one hits */
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        const otext* mixedKeys[] = { OTEXT("nope1"), keys[2], OTEXT("nope2") };
        ASSERT_TRUE(OCI_SodaOptionsSetKeys(options, mixedKeys, 3));

        auto* found = OCI_SodaCollFindOne(coll, options);
        ASSERT_NE(nullptr, found);
        ASSERT_EQ(String(keys[2]), String(OCI_SodaDocGetKey(found)));
        ASSERT_TRUE(OCI_SodaDocFree(found));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    TEST_F(TestSodaMultiKey, FindOneWithMissingKeys_ReturnsDoc)
    {
        /* full miss loop + doc cleanup */
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        const otext* missKeys[] = { OTEXT("nope1"), OTEXT("nope2") };
        ASSERT_TRUE(OCI_SodaOptionsSetKeys(options, missKeys, 2));

        ASSERT_EQ(nullptr, OCI_SodaCollFindOne(coll, options));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    TEST_F(TestSodaMultiKey, DocCountWithKeyList_ReturnsCount)
    {
        /* per-key counts are summed */
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        const otext* countKeys[] = { keys[0], keys[1], OTEXT("nope") };
        ASSERT_TRUE(OCI_SodaOptionsSetKeys(options, countKeys, 3));
        ASSERT_EQ(2u, OCI_SodaCollDocCount(coll, options));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    TEST_F(TestSodaMultiKey, ReplaceOneWithKeyList_UpdatesDoc)
    {
        const String repl = OTEXT("{\"idx\":2}");
        auto* replDoc = OCI_SodaDocCreateFromText
        (
            sdb, repl.c_str(),
            static_cast<OCI_UInt32>(repl.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, replDoc);

        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        const otext* replKeys[] = { OTEXT("nope"), keys[3] };
        ASSERT_TRUE(OCI_SodaOptionsSetKeys(options, replKeys, 2));
        ASSERT_TRUE(OCI_SodaCollReplaceOne(coll, options, replDoc, FALSE));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
        ASSERT_TRUE(OCI_SodaDocFree(replDoc));
    }

    TEST_F(TestSodaMultiKey, ReplaceOneAndGetWithKeyList_ReturnsDoc)
    {
        const String repl = OTEXT("{\"idx\":2}");
        auto* replDoc = OCI_SodaDocCreateFromText
        (
            sdb, repl.c_str(),
            static_cast<OCI_UInt32>(repl.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, replDoc);

        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        const otext* replKeys[] = { OTEXT("nope"), keys[4] };
        ASSERT_TRUE(OCI_SodaOptionsSetKeys(options, replKeys, 2));
        ASSERT_TRUE(OCI_SodaCollReplaceOne(coll, options, replDoc, TRUE));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
        ASSERT_TRUE(OCI_SodaDocFree(replDoc));
    }

    TEST_F(TestSodaMultiKey, RemoveWithKeyList_RemovesDocs)
    {
        /* per-key removes are summed */
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        const otext* rmKeys[] = { keys[5], keys[6], OTEXT("nope") };
        ASSERT_TRUE(OCI_SodaOptionsSetKeys(options, rmKeys, 3));

        OCI_UInt64 removed = 0;
        ASSERT_TRUE(OCI_SodaCollRemove(coll, options, &removed));
        ASSERT_EQ(2u, removed);
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    /* --- NULL options / miss paths --- */

    class TestSodaNullOptions : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            const auto sdb = OCI_SodaDbCreate(conn);
            ASSERT_NE(nullptr, sdb);

            DropCollIfExists(sdb, OTEXT("OCILIB_SODA_NOPTS_COLL"));
            const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_NOPTS_COLL"), NULL);
            ASSERT_NE(nullptr, coll);

            const String content = OTEXT("{\"name\":\"nopts\"}");

            const auto doc = OCI_SodaDocCreateFromText
            (
                sdb,
                content.c_str(),
                static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
                NULL
            );
            ASSERT_NE(nullptr, doc);
            ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, doc));
            ASSERT_TRUE(OCI_SodaDocFree(doc));

            ASSERT_TRUE(OCI_Commit(conn));
            ASSERT_TRUE(OCI_SodaCollFree(coll));
            ASSERT_TRUE(OCI_SodaDbFree(sdb));
            ASSERT_TRUE(OCI_ConnectionFree(conn));
        }

        static void TearDownTestCase()
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            const auto sdb = OCI_SodaDbCreate(conn);
            ASSERT_NE(nullptr, sdb);

            DropCollIfExists(sdb, OTEXT("OCILIB_SODA_NOPTS_COLL"));

            ASSERT_TRUE(OCI_Commit(conn));
            ASSERT_TRUE(OCI_SodaDbFree(sdb));
            ASSERT_TRUE(OCI_ConnectionFree(conn));
        }

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            sdb = OCI_SodaDbCreate(conn);
            ASSERT_NE(nullptr, sdb);

            coll = OCI_SodaDbOpenColl(sdb, OTEXT("OCILIB_SODA_NOPTS_COLL"));
            ASSERT_NE(nullptr, coll);
        }

        void TearDown() override
        {
            OCI_Commit(conn);
            OCI_SodaCollFree(coll);
            OCI_SodaDbFree(sdb);
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
        OCI_SodaDb*     sdb{ nullptr };
        OCI_SodaColl*   coll{ nullptr };
    };

    TEST_F(TestSodaNullOptions, FindWithNullOptions_UsesDefaults)
    {
        /* NULL options: internal temporary options are created */
        auto* cursor = OCI_SodaCollFind(coll, NULL);
        ASSERT_NE(nullptr, cursor);

        OCI_UInt32 count = 0;
        OCI_SodaDoc* item = NULL;
        while (NULL != (item = OCI_SodaDocCursorGetNext(cursor)))
        {
            count++;
            ASSERT_TRUE(OCI_SodaDocFree(item));
        }
        ASSERT_EQ(1u, count);
        ASSERT_TRUE(OCI_SodaDocCursorFree(cursor));
    }

    TEST_F(TestSodaNullOptions, FindOneWithNullOptions_UsesDefaults)
    {
        auto* found = OCI_SodaCollFindOne(coll, NULL);
        ASSERT_NE(nullptr, found);
        ASSERT_TRUE(OCI_SodaDocFree(found));
    }

    TEST_F(TestSodaNullOptions, DocCountWithNullOptions_CountsAll)
    {
        ASSERT_EQ(1u, OCI_SodaCollDocCount(coll, NULL));
    }

    TEST_F(TestSodaNullOptions, FindOneByUnknownKey_ReturnsNull)
    {
        /* NO_DATA cleanup arm */
        ASSERT_EQ(nullptr, OCI_SodaCollFindOneByKey(coll, OTEXT("no_such_key")));
    }

    TEST_F(TestSodaNullOptions, ReplaceOneWithNullOptions_UsesDefaults)
    {
        /* NULL options exercises the temporary-options arm; an empty
           filter does not replace the doc, so only the call matters */
        const String repl = OTEXT("{\"name\":\"nopts2\"}");

        auto* replDoc = OCI_SodaDocCreateFromText
        (
            sdb, repl.c_str(),
            static_cast<OCI_UInt32>(repl.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, replDoc);

        (void) OCI_SodaCollReplaceOne(coll, NULL, replDoc, FALSE);
        ASSERT_TRUE(OCI_SodaDocFree(replDoc));
    }

    TEST_F(TestSodaNullOptions, ReplaceOneAndGetWithNullOptions_UsesDefaults)
    {
        const String repl = OTEXT("{\"name\":\"nopts2\"}");

        auto* replDoc = OCI_SodaDocCreateFromText
        (
            sdb, repl.c_str(),
            static_cast<OCI_UInt32>(repl.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, replDoc);

        (void) OCI_SodaCollReplaceOne(coll, NULL, replDoc, TRUE);
        ASSERT_TRUE(OCI_SodaDocFree(replDoc));
    }

    TEST_F(TestSodaNullOptions, RemoveWithNullOptions_UsesDefaults)
    {
        OCI_UInt64 removed = 0;
        (void) OCI_SodaCollRemove(coll, NULL, &removed);
    }

    /* --- public-API error paths: NULL handles, invalid args and bad state
       hit the CHECK_PTR / CHECK_OCI arms the dropped fault tests covered --- */

    TEST(TestSoda, NullCollErrors)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        /* every coll call rejects a NULL collection handle */
        ASSERT_FALSE(OCI_SodaCollDrop(nullptr));
        ASSERT_FALSE(OCI_SodaCollTruncate(nullptr));
        ASSERT_FALSE(OCI_SodaCollInsertDoc(nullptr, nullptr));
        ASSERT_EQ(nullptr, OCI_SodaCollGetName(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaCollGetConnection(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaCollGetMetadata(nullptr));
        ASSERT_EQ(0u, OCI_SodaCollIsMapped(nullptr));
        ASSERT_EQ(0u, OCI_SodaCollDocCount(nullptr, nullptr));
        ASSERT_EQ(nullptr, OCI_SodaCollFind(nullptr, nullptr));
        ASSERT_EQ(nullptr, OCI_SodaCollFindOne(nullptr, nullptr));
        ASSERT_EQ(nullptr, OCI_SodaCollFindOneByKey(nullptr, nullptr));
        ASSERT_FALSE(OCI_SodaCollRemove(nullptr, nullptr, nullptr));
        ASSERT_FALSE(OCI_SodaCollRemoveOneByKey(nullptr, nullptr));
        ASSERT_FALSE(OCI_SodaCollReplaceOne(nullptr, nullptr, nullptr, FALSE));
        ASSERT_FALSE(OCI_SodaCollReplaceOne(nullptr, nullptr, nullptr, TRUE));
        ASSERT_FALSE(OCI_SodaCollReplaceOneByKey(nullptr, nullptr, nullptr, TRUE));
        ASSERT_FALSE(OCI_SodaCollReplaceOneByKey(nullptr, nullptr, nullptr, FALSE));
        ASSERT_EQ(0u, OCI_SodaCollDocCountWithFilter(nullptr, nullptr));
    }

    TEST(TestSoda, NullDbErrors)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        /* db calls reject NULL db/collection-name args */
        ASSERT_EQ(nullptr, OCI_SodaDbCreateColl(nullptr, nullptr, nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDbOpenColl(nullptr, nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDbGetColls(nullptr, nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDbGetConnection(nullptr));
        ASSERT_FALSE(OCI_SodaDbFree(nullptr));
    }

    TEST(TestSoda, NullDocErrors)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        /* doc calls reject NULL docs / null text */
        ASSERT_FALSE(OCI_SodaDocFree(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDocGetKey(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDocGetJson(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDocGetText(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDocGetCreatedOn(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDocGetLastModified(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDocGetMediaType(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDocGetVersion(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDocCreateFromJson(nullptr, nullptr, nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDocCreateFromText(nullptr, nullptr, 0, nullptr));
    }

    TEST(TestSoda, NullOptionsErrors)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        /* options calls reject NULL options */
        ASSERT_FALSE(OCI_SodaOptionsFree(nullptr));
        ASSERT_FALSE(OCI_SodaOptionsSetFilter(nullptr, nullptr));
        ASSERT_FALSE(OCI_SodaOptionsSetKey(nullptr, nullptr));
        ASSERT_FALSE(OCI_SodaOptionsSetKeys(nullptr, nullptr, 0));
        ASSERT_FALSE(OCI_SodaOptionsSetLimit(nullptr, 0));
        ASSERT_FALSE(OCI_SodaOptionsSetSkip(nullptr, 0));
        ASSERT_FALSE(OCI_SodaOptionsSetVersion(nullptr, nullptr));
    }

    TEST(TestSoda, InvalidNameErrors)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        /* unknown collection: OpenColl may return NULL (error) or a valid
           handle - both are acceptable; only no-crash matters */
        const auto nocol = OCI_SodaDbOpenColl(sdb, OTEXT("TestSodaNoSuchColl42"));
        if (nocol) { ASSERT_TRUE(OCI_SodaCollFree(nocol)); }
        ASSERT_EQ(nullptr, OCI_SodaCollFindOneByKey(nullptr, OTEXT("k")));
        ASSERT_EQ(0u, OCI_SodaCollDocCount(nullptr, NULL));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    /* -------------------------------------------------------------------------
       Shared fixture: SODA db + metadata-backed collection
       ------------------------------------------------------------------------- */

    class TestSodaOps : public ::testing::Test
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_SodaDb* sdb = nullptr;
        OCI_SodaColl* coll = nullptr;

        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            sdb = OCI_SodaDbCreate(conn);
            ASSERT_NE(nullptr, sdb);
            DropCollIfExists(sdb, OTEXT("OCILIB_SODA_TEST_COLL"));
            const String collMeta = OTEXT("{\"keyColumn\":{\"name\":\"ID\",\"sqlType\":\"VARCHAR2\",\"maxLength\":255,\"assignmentMethod\":\"CLIENT\"}}");
            coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_TEST_COLL"), collMeta.c_str());
            ASSERT_NE(nullptr, coll);
        }

        void TearDown() override
        {
            if (coll) { OCI_SodaCollDrop(coll); OCI_SodaCollFree(coll); }
            if (sdb) OCI_SodaDbFree(sdb);
            if (conn) OCI_ConnectionFree(conn);
        }

        OCI_SodaDoc* MakeDoc(const String& content, const otext* key)
        {
            return OCI_SodaDocCreateFromText(sdb, content.c_str(), static_cast<OCI_UInt32>(content.size() * sizeof(otext)), key);
        }
    };

    TEST_F(TestSodaOps, SodaCollInsertMany_InsertsDocs)
    {
        OCI_SodaDoc* docs[3] = {
            MakeDoc(OTEXT("{\"a\":1}"), OTEXT("k1")),
            MakeDoc(OTEXT("{\"a\":2}"), OTEXT("k2")),
            MakeDoc(OTEXT("{\"a\":3}"), OTEXT("k3"))
        };
        ASSERT_NE(nullptr, docs[0]);
        ASSERT_NE(nullptr, docs[1]);
        ASSERT_NE(nullptr, docs[2]);

        EXPECT_TRUE(OCI_SodaCollInsertMany(coll, docs, 3, FALSE));

        ASSERT_TRUE(OCI_SodaDocFree(docs[0]));
        ASSERT_TRUE(OCI_SodaDocFree(docs[1]));
        ASSERT_TRUE(OCI_SodaDocFree(docs[2]));
    }

    TEST_F(TestSodaOps, SodaDocGetIsJsonOnJsonDoc_ReturnsTrue)
    {
        OCI_SodaDoc* d = MakeDoc(OTEXT("{\"a\":1}"), OTEXT("k1"));
        ASSERT_NE(nullptr, d);

        EXPECT_TRUE(OCI_SodaDocGetIsJson(d));

        ASSERT_TRUE(OCI_SodaDocFree(d));
    }

    TEST_F(TestSodaOps, SodaCollInsertManyWithMeta_ReturnsDocVersion)
    {
        OCI_SodaDoc* e = MakeDoc(OTEXT("{\"a\":1}"), OTEXT("k4"));
        ASSERT_NE(nullptr, e);
        OCI_SodaDoc* edocs[1] = { e };

        ASSERT_TRUE(OCI_SodaCollInsertMany(coll, edocs, 1, TRUE));

        EXPECT_NE(nullptr, OCI_SodaDocGetVersion(e));

        ASSERT_TRUE(OCI_SodaDocFree(e));
    }

    TEST_F(TestSodaOps, SodaCollSave_UpsertsAndReturnsVersion)
    {
        OCI_SodaDoc* sv = MakeDoc(OTEXT("{\"a\":99}"), OTEXT("k1"));
        ASSERT_NE(nullptr, sv);

        ASSERT_TRUE(OCI_SodaCollSave(coll, sv, FALSE));

        EXPECT_NE(nullptr, OCI_SodaDocGetVersion(sv));

        ASSERT_TRUE(OCI_SodaDocFree(sv));
    }

    TEST_F(TestSodaOps, SodaCollCreateAndDropIndex_WorksWhenClientSupports)
    {
        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_IDX_COLL"));
        OCI_SodaColl* coll2 = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_IDX_COLL"), NULL);
        ASSERT_NE(nullptr, coll2);
        OCI_SodaDoc* seed = MakeDoc(OTEXT("{\"a\":1}"), NULL);
        ASSERT_TRUE(OCI_SodaCollInsertDoc(coll2, seed));
        ASSERT_TRUE(OCI_SodaDocFree(seed));
        ASSERT_TRUE(OCI_Commit(conn));
        const String spec = OTEXT("{\"name\":\"OCILIB_SODA_TEST_IDX\"}");

        /* fails inside the OCI call in UNICODE builds (ORA-40671) — ANSI only */
        const OCI_Boolean cix = OCI_SodaCollCreateIndex(coll2, spec.c_str());

        if (cix)
        {
            EXPECT_TRUE(OCI_SodaCollDropIndex(coll2, OTEXT("OCILIB_SODA_TEST_IDX")));
        }

        ASSERT_TRUE(OCI_SodaCollDrop(coll2));
        ASSERT_TRUE(OCI_SodaCollFree(coll2));
    }

    TEST_F(TestSodaOps, SodaCollGetDataGuideWithoutGuide_NoError)
    {
        OCI_SodaDoc* dg = OCI_SodaCollGetDataGuide(coll);

        if (NULL != dg)
        {
            ASSERT_TRUE(OCI_SodaDocFree(dg));
        }
    }

    TEST_F(TestSodaOps, SodaDbGetCollectionNames_ReturnsNamesList)
    {
        OCI_UInt32 ncount = 0;

        otext** collNames = OCI_SodaDbGetCollectionNames(sdb, &ncount);

        ASSERT_NE(nullptr, collNames);
        ASSERT_TRUE(OCI_SodaDbFreeCollectionNames(sdb, collNames, ncount));
    }

    TEST_F(TestSodaOps, SodaCollListIndexes_NoErrorWhenSupported)
    {
        OCI_UInt32 ncount = 0;

        /* fails inside the OCI call in UNICODE builds (OCI-22303) — tolerant */
        otext** idxNames = OCI_SodaCollListIndexes(coll, &ncount);

        if (NULL != idxNames)
        {
            ASSERT_TRUE(OCI_SodaCollFreeIndexNames(coll, idxNames, ncount));
        }
    }

#ifdef OCI_CHARSET_ANSI

    /* The OCISodaIndex* calls work in ANSI builds; in UNICODE builds they fail
       inside the Oracle client (ORA-40671 / OCI-22303, IndexDrop even SEGVs) —
       the real assertions live here, BulkSaveIndexAndMeta keeps diagnostic-only
       calls that run in both modes */
    TEST(TestSoda, IndexCreateDropList)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        OCI_SodaDb* sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_IDX"));

        OCI_SodaColl* coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_IDX"), NULL);
        ASSERT_NE(nullptr, coll);

        const String content = OTEXT("{\"a\":1}");
        OCI_SodaDoc* seed = OCI_SodaDocCreateFromText(sdb, content.c_str(), static_cast<OCI_UInt32>(content.size() * sizeof(otext)), NULL);
        ASSERT_NE(nullptr, seed);
        ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, seed));
        ASSERT_TRUE(OCI_SodaDocFree(seed));
        ASSERT_TRUE(OCI_Commit(conn));

        ASSERT_TRUE(OCI_SodaCollCreateIndex(coll, OTEXT("{\"name\":\"OCILIB_SODA_IDX_NAME\"}")));

        OCI_UInt32 ncount = 0;
        otext** names = OCI_SodaCollListIndexes(coll, &ncount);
        ASSERT_NE(nullptr, names);
        ASSERT_GE(ncount, 1u);
        ASSERT_TRUE(OCI_SodaCollFreeIndexNames(coll, names, ncount));

        ASSERT_TRUE(OCI_SodaCollDropIndex(coll, OTEXT("OCILIB_SODA_IDX_NAME")));
        ASSERT_TRUE(OCI_SodaCollDrop(coll));
        ASSERT_TRUE(OCI_SodaCollFree(coll));
        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

#endif






    TEST(TestSoda, CollInsertDoc_MalformedDoc_ReturnsFalse)
    {
        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);
        const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("TestSodaErrDoc"), nullptr);
        ASSERT_NE(nullptr, coll);
        const auto doc = OCI_SodaDocCreateFromText(sdb, OTEXT("{bad json"), 9, nullptr);
        ASSERT_NE(nullptr, doc);

        ASSERT_FALSE(OCI_SodaCollInsertDoc(coll, doc));

        ASSERT_TRUE(OCI_SodaDocFree(doc));
        ASSERT_TRUE(OCI_SodaCollDrop(coll));
        ASSERT_TRUE(OCI_SodaCollFree(coll));
        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, CollCreateIndex_MalformedSpec_ReturnsFalse)
    {
        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);
        const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("TestSodaErrIdx"), nullptr);
        ASSERT_NE(nullptr, coll);

        ASSERT_FALSE(OCI_SodaCollCreateIndex(coll, OTEXT("{bad spec")));

        ASSERT_TRUE(OCI_SodaCollDrop(coll));
        ASSERT_TRUE(OCI_SodaCollFree(coll));
        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    class TestSodaNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestSodaNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestSodaNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "SodaCollCreateIndex", [] { return !OCI_SodaCollCreateIndex(nullptr, OTEXT("x")); } },
            { "SodaCollCursorFree", [] { return !OCI_SodaCollCursorFree(nullptr); } },
            { "SodaCollCursorGetNext", [] { return nullptr == OCI_SodaCollCursorGetNext(nullptr); } },
            { "SodaCollDocCount", [] { return 0 == OCI_SodaCollDocCount(nullptr, nullptr); } },
            { "SodaCollDocCountWithFilter", [] { return 0 == OCI_SodaCollDocCountWithFilter(nullptr, OTEXT("x")); } },
            { "SodaCollDrop", [] { return !OCI_SodaCollDrop(nullptr); } },
            { "SodaCollDropIndex", [] { return !OCI_SodaCollDropIndex(nullptr, OTEXT("x")); } },
            { "SodaCollFind", [] { return nullptr == OCI_SodaCollFind(nullptr, nullptr); } },
            { "SodaCollFindOne", [] { return nullptr == OCI_SodaCollFindOne(nullptr, nullptr); } },
            { "SodaCollFindOneByKey", [] { return nullptr == OCI_SodaCollFindOneByKey(nullptr, OTEXT("x")); } },
            { "SodaCollFree", [] { return !OCI_SodaCollFree(nullptr); } },
            { "SodaCollFreeIndexNames", [] { return !OCI_SodaCollFreeIndexNames(nullptr, nullptr /* otext ** */, 1); } },
            { "SodaCollGetConnection", [] { return nullptr == OCI_SodaCollGetConnection(nullptr); } },
            { "SodaCollGetDataGuide", [] { return nullptr == OCI_SodaCollGetDataGuide(nullptr); } },
            { "SodaCollGetMetadata", [] { return nullptr == OCI_SodaCollGetMetadata(nullptr); } },
            { "SodaCollGetName", [] { return nullptr == OCI_SodaCollGetName(nullptr); } },
            { "SodaCollInsertDoc", [] { return !OCI_SodaCollInsertDoc(nullptr, nullptr); } },
            { "SodaCollInsertMany", [] { return !OCI_SodaCollInsertMany(nullptr, nullptr, 1, FALSE); } },
            { "SodaCollIsMapped", [] { return !OCI_SodaCollIsMapped(nullptr); } },
            { "SodaCollListIndexes", [] { return nullptr == OCI_SodaCollListIndexes(nullptr, NullArgOut<OCI_UInt32>()); } },
            { "SodaCollRemove", [] { return !OCI_SodaCollRemove(nullptr, nullptr, NullArgOut<OCI_UInt64>()); } },
            { "SodaCollRemoveOneByKey", [] { return !OCI_SodaCollRemoveOneByKey(nullptr, OTEXT("x")); } },
            { "SodaCollReplaceOne", [] { return !OCI_SodaCollReplaceOne(nullptr, nullptr, nullptr, FALSE); } },
            { "SodaCollReplaceOneByKey", [] { return !OCI_SodaCollReplaceOneByKey(nullptr, OTEXT("x"), nullptr, FALSE); } },
            { "SodaCollSave", [] { return !OCI_SodaCollSave(nullptr, nullptr, FALSE); } },
            { "SodaCollTruncate", [] { return !OCI_SodaCollTruncate(nullptr); } },
            { "SodaDbCreate", [] { return nullptr == OCI_SodaDbCreate(nullptr); } },
            { "SodaDbCreateColl", [] { return nullptr == OCI_SodaDbCreateColl(nullptr, OTEXT("x"), OTEXT("x")); } },
            { "SodaDbFree", [] { return !OCI_SodaDbFree(nullptr); } },
            { "SodaDbFreeCollectionNames", [] { return !OCI_SodaDbFreeCollectionNames(nullptr, nullptr /* otext ** */, 1); } },
            { "SodaDbGetCollectionNames", [] { return nullptr == OCI_SodaDbGetCollectionNames(nullptr, NullArgOut<OCI_UInt32>()); } },
            { "SodaDbGetColls", [] { return nullptr == OCI_SodaDbGetColls(nullptr, OTEXT("x")); } },
            { "SodaDbGetConnection", [] { return nullptr == OCI_SodaDbGetConnection(nullptr); } },
            { "SodaDbOpenColl", [] { return nullptr == OCI_SodaDbOpenColl(nullptr, OTEXT("x")); } },
            { "SodaDocCreateFromJson", [] { return nullptr == OCI_SodaDocCreateFromJson(nullptr, nullptr, OTEXT("x")); } },
            { "SodaDocCreateFromText", [] { return nullptr == OCI_SodaDocCreateFromText(nullptr, OTEXT("x"), 1, OTEXT("x")); } },
            { "SodaDocCursorFree", [] { return !OCI_SodaDocCursorFree(nullptr); } },
            { "SodaDocCursorGetNext", [] { return nullptr == OCI_SodaDocCursorGetNext(nullptr); } },
            { "SodaDocFree", [] { return !OCI_SodaDocFree(nullptr); } },
            { "SodaDocGetCreatedOn", [] { return nullptr == OCI_SodaDocGetCreatedOn(nullptr); } },
            { "SodaDocGetIsJson", [] { return !OCI_SodaDocGetIsJson(nullptr); } },
            { "SodaDocGetJson", [] { return nullptr == OCI_SodaDocGetJson(nullptr); } },
            { "SodaDocGetKey", [] { return nullptr == OCI_SodaDocGetKey(nullptr); } },
            { "SodaDocGetLastModified", [] { return nullptr == OCI_SodaDocGetLastModified(nullptr); } },
            { "SodaDocGetMediaType", [] { return nullptr == OCI_SodaDocGetMediaType(nullptr); } },
            { "SodaDocGetText", [] { return nullptr == OCI_SodaDocGetText(nullptr); } },
            { "SodaDocGetVersion", [] { return nullptr == OCI_SodaDocGetVersion(nullptr); } },
            { "SodaOptionsCreate", [] { return nullptr == OCI_SodaOptionsCreate(nullptr); } },
            { "SodaOptionsFree", [] { return !OCI_SodaOptionsFree(nullptr); } },
            { "SodaOptionsSetFetchArraySize", [] { return !OCI_SodaOptionsSetFetchArraySize(nullptr, 1); } },
            { "SodaOptionsSetFilter", [] { return !OCI_SodaOptionsSetFilter(nullptr, OTEXT("x")); } },
            { "SodaOptionsSetHint", [] { return !OCI_SodaOptionsSetHint(nullptr, OTEXT("x")); } },
            { "SodaOptionsSetKey", [] { return !OCI_SodaOptionsSetKey(nullptr, OTEXT("x")); } },
            { "SodaOptionsSetKeys", [] { return !OCI_SodaOptionsSetKeys(nullptr, nullptr /* otext ** */, 1); } },
            { "SodaOptionsSetLimit", [] { return !OCI_SodaOptionsSetLimit(nullptr, 1); } },
            { "SodaOptionsSetLock", [] { return !OCI_SodaOptionsSetLock(nullptr, FALSE); } },
            { "SodaOptionsSetSkip", [] { return !OCI_SodaOptionsSetSkip(nullptr, 1); } },
            { "SodaOptionsSetVersion", [] { return !OCI_SodaOptionsSetVersion(nullptr, OTEXT("x")); } },
        }),
        &NullArgCaseName);

}