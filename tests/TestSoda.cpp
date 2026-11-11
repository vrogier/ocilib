#include "ocilib_tests.h"
#include "NullArgs.h"

#include <sstream>

using namespace ocilib;

namespace
{
    void DropCollIfExists(OCI_SodaDatabase* sdb, const otext* name)
    {
        OCI_SodaCollection* coll = OCI_SodaDatabaseOpenCollection(sdb, name);
        if (coll)
        {
            OCI_SodaCollectionDrop(coll);
            OCI_SodaCollectionFree(coll);
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

        const auto sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);

        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, CreateAndDropCollection)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_TEST_COLL"));
        const auto coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_TEST_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        ASSERT_TRUE(OCI_SodaCollectionDrop(coll));
        ASSERT_TRUE(OCI_SodaCollectionFree(coll));

        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, InsertAndFindOneByKey)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_TEST_COLL"));
        const auto coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_TEST_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        const String content = OTEXT("{\"name\":\"soda test\"}");

        const auto doc = OCI_SodaDocumentCreateFromText
        (
            sdb,
            content.c_str(),
            static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, doc);

        ASSERT_TRUE(OCI_SodaCollectionInsertDocument(coll, doc));

        const auto docKey = OCI_SodaDocumentGetKey(doc);
        ASSERT_NE(nullptr, docKey);

       const auto foundDoc = OCI_SodaCollectionFindOneByKey(coll, docKey);
        ASSERT_NE(nullptr, foundDoc);

        const auto foundKey = OCI_SodaDocumentGetKey(foundDoc);
        ASSERT_EQ(String(docKey), String(foundKey));

        const auto foundContent = OCI_SodaDocumentGetText(foundDoc);
       ASSERT_NE(nullptr, foundContent);

        ASSERT_TRUE(OCI_SodaDocumentFree(foundDoc));

        ASSERT_TRUE(OCI_SodaDocumentFree(doc));
        
        ASSERT_TRUE(OCI_ConnectionCommit(conn));
        ASSERT_TRUE(OCI_SodaCollectionDrop(coll));
        ASSERT_TRUE(OCI_SodaCollectionFree(coll));

        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, FindAndGetJson)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_TEST_JSON_COLL"));
        const auto coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_TEST_JSON_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        const String content = OTEXT("{\"name\":\"soda json test\"}");

        const auto doc = OCI_SodaDocumentCreateFromText
        (
            sdb,
            content.c_str(),
            static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, doc);

        ASSERT_TRUE(OCI_SodaCollectionInsertDocument(coll, doc));

        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        auto* cursor = OCI_SodaCollectionFind(coll, options);
        ASSERT_NE(nullptr, cursor);

        auto* foundDoc = OCI_SodaDocumentCursorGetNext(cursor);
        ASSERT_NE(nullptr, foundDoc);

        const auto textContent = OCI_SodaDocumentGetText(foundDoc);
        ASSERT_NE(nullptr, textContent);

        if (OCI_ConnectionGetVersion(conn) >= OCI_23_4 && OCI_EnvironmentGetOCIRuntimeVersion(env) >= OCI_23_4)
        {
            /* returned JSON handle is owned by the document - do not free it */

            auto* jsonContent = OCI_SodaDocumentGetJson(foundDoc);
            ASSERT_NE(nullptr, jsonContent);
            ASSERT_EQ(jsonContent, OCI_SodaDocumentGetJson(foundDoc));
        }

        ASSERT_TRUE(OCI_SodaDocumentFree(foundDoc));
        ASSERT_TRUE(OCI_SodaDocumentCursorFree(cursor));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
        ASSERT_TRUE(OCI_SodaDocumentFree(doc));

        ASSERT_TRUE(OCI_ConnectionCommit(conn));
        ASSERT_TRUE(OCI_SodaCollectionDrop(coll));
        ASSERT_TRUE(OCI_SodaCollectionFree(coll));

        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, DbGetConnection)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);

        ASSERT_EQ(conn, OCI_SodaDatabaseGetConnection(sdb));

        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, CollectionProperties)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_PROPS_COLL"));
        const auto coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_PROPS_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        const auto name = OCI_SodaCollectionGetName(coll);
        ASSERT_NE(nullptr, name);
        ASSERT_EQ(String(OTEXT("OCILIB_SODA_PROPS_COLL")), String(name));

        const auto metadata = OCI_SodaCollectionGetMetadata(coll);
        ASSERT_NE(nullptr, metadata);
        ASSERT_FALSE(String(metadata).empty());

        ASSERT_EQ(conn, OCI_SodaCollectionGetConnection(coll));

        OCI_SodaCollectionIsMapped(coll);

        ASSERT_TRUE(OCI_ConnectionCommit(conn));
        ASSERT_TRUE(OCI_SodaCollectionDrop(coll));
        ASSERT_TRUE(OCI_SodaCollectionFree(coll));

        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, CreateCollWithMetadata)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);

        const String metadata = OTEXT("{\"keyColumn\":{\"name\":\"ID\",\"assignmentMethod\":\"CLIENT\",\"sqlType\":\"VARCHAR2\",\"maxLength\":255}}");

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_META_COLL"));
        const auto coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_META_COLL"), metadata.c_str());
        ASSERT_NE(nullptr, coll);

        const auto actual = OCI_SodaCollectionGetMetadata(coll);
        ASSERT_NE(nullptr, actual);
        ASSERT_NE(String::npos, String(actual).find(OTEXT("CLIENT")));

        /* client-assigned keys : the key supplied at creation is used as is */

        const String content = OTEXT("{\"value\":1}");

        const auto doc = OCI_SodaDocumentCreateFromText
        (
            sdb,
            content.c_str(),
            static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
            OTEXT("mykey1")
        );
        ASSERT_NE(nullptr, doc);

        ASSERT_TRUE(OCI_SodaCollectionInsertDocument(coll, doc));

        const auto foundDoc = OCI_SodaCollectionFindOneByKey(coll, OTEXT("mykey1"));
        ASSERT_NE(nullptr, foundDoc);
        ASSERT_TRUE(OCI_SodaDocumentFree(foundDoc));

        ASSERT_TRUE(OCI_SodaDocumentFree(doc));

        ASSERT_TRUE(OCI_ConnectionCommit(conn));
        ASSERT_TRUE(OCI_SodaCollectionDrop(coll));
        ASSERT_TRUE(OCI_SodaCollectionFree(coll));

        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, OpenCollection)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_OPEN_COLL"));
        const auto coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_OPEN_COLL"), NULL);
        ASSERT_NE(nullptr, coll);
        ASSERT_TRUE(OCI_SodaCollectionFree(coll));

        const auto opened = OCI_SodaDatabaseOpenCollection(sdb, OTEXT("OCILIB_SODA_OPEN_COLL"));
        ASSERT_NE(nullptr, opened);

        const auto name = OCI_SodaCollectionGetName(opened);
        ASSERT_NE(nullptr, name);
        ASSERT_EQ(String(OTEXT("OCILIB_SODA_OPEN_COLL")), String(name));

        ASSERT_TRUE(OCI_ConnectionCommit(conn));
        ASSERT_TRUE(OCI_SodaCollectionDrop(opened));
        ASSERT_TRUE(OCI_SodaCollectionFree(opened));

        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, GetCollectionsCursor)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_CUR_COLL1"));
        const auto coll1 = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_CUR_COLL1"), NULL);
        ASSERT_NE(nullptr, coll1);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_CUR_COLL2"));
        const auto coll2 = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_CUR_COLL2"), NULL);
        ASSERT_NE(nullptr, coll2);

        OCI_UInt32 found = 0;
        const auto cursor = OCI_SodaDatabaseGetCollections(sdb, NULL);
        ASSERT_NE(nullptr, cursor);

        OCI_SodaCollection *item = NULL;
        while (NULL != (item = OCI_SodaCollectionCursorGetNext(cursor)))
        {
            const auto name = OCI_SodaCollectionGetName(item);
            ASSERT_NE(nullptr, name);

            if (String(name).find(OTEXT("OCILIB_SODA_CUR_COLL")) != String::npos)
            {
                found++;
            }

            ASSERT_TRUE(OCI_SodaCollectionFree(item));
        }

        ASSERT_TRUE(OCI_SodaCollectionCursorFree(cursor));
        ASSERT_EQ(2u, found);

        ASSERT_TRUE(OCI_ConnectionCommit(conn));
        ASSERT_TRUE(OCI_SodaCollectionDrop(coll1));
        ASSERT_TRUE(OCI_SodaCollectionFree(coll1));
        ASSERT_TRUE(OCI_SodaCollectionDrop(coll2));
        ASSERT_TRUE(OCI_SodaCollectionFree(coll2));

        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, DocumentMetadata)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_DOCMETA_COLL"));
        const auto coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_DOCMETA_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        const String content = OTEXT("{\"name\":\"meta test\"}");

        const auto doc = OCI_SodaDocumentCreateFromText
        (
            sdb,
            content.c_str(),
            static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, doc);

        /* media type is known before insert */

        const auto mediaType = OCI_SodaDocumentGetMediaType(doc);
        ASSERT_NE(nullptr, mediaType);

        ASSERT_TRUE(OCI_SodaCollectionInsertDocument(coll, doc));

        /* insert-and-get refreshes generated metadata */

        ASSERT_NE(nullptr, OCI_SodaDocumentGetKey(doc));
        ASSERT_NE(nullptr, OCI_SodaDocumentGetVersion(doc));
        ASSERT_NE(nullptr, OCI_SodaDocumentGetCreatedOn(doc));
        ASSERT_NE(nullptr, OCI_SodaDocumentGetLastModified(doc));

        ASSERT_TRUE(OCI_SodaDocumentFree(doc));

        ASSERT_TRUE(OCI_ConnectionCommit(conn));
        ASSERT_TRUE(OCI_SodaCollectionDrop(coll));
        ASSERT_TRUE(OCI_SodaCollectionFree(coll));

        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, DocCreateFromJson)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_JSONDOC_COLL"));
        const auto coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_JSONDOC_COLL"),
                                               OTEXT("{\"keyColumn\":{\"name\":\"ID\"},"
                                                     "\"contentColumn\":{\"name\":\"JSON_DOCUMENT\",\"sqlType\":\"JSON\"},"
                                                     "\"versionColumn\":{\"name\":\"VERSION\",\"method\":\"UUID\"},"
                                                     "\"lastModifiedColumn\":{\"name\":\"LAST_MODIFIED\"},"
                                                     "\"creationTimeColumn\":{\"name\":\"CREATED_ON\"}}"));
        ASSERT_NE(nullptr, coll);

        if (OCI_ConnectionGetVersion(conn) >= OCI_23_4 && OCI_EnvironmentGetOCIRuntimeVersion(env) >= OCI_23_4)
        {
            const auto json = OCI_JsonCreate(conn);
            ASSERT_NE(nullptr, json);
            ASSERT_TRUE(OCI_JsonLoadText(json, OTEXT("{\"name\":\"from json\"}"), 0, OCI_JTV_ALLOW_ALL, OCI_JTE_DETECT));

            const auto doc = OCI_SodaDocumentCreateFromJson(sdb, json, NULL);
            ASSERT_NE(nullptr, doc);

            ASSERT_TRUE(OCI_SodaCollectionInsertDocument(coll, doc));

            const auto text = OCI_SodaDocumentGetText(doc);
            ASSERT_NE(nullptr, text);
            ASSERT_NE(String::npos, String(text).find(OTEXT("from json")));

            ASSERT_TRUE(OCI_SodaDocumentFree(doc));
            ASSERT_TRUE(OCI_JsonFree(json));
        }

        ASSERT_TRUE(OCI_ConnectionCommit(conn));
        ASSERT_TRUE(OCI_SodaCollectionDrop(coll));
        ASSERT_TRUE(OCI_SodaCollectionFree(coll));

        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    /* -------------------------------------------------------------------------
       Shared fixture: SODA coll seeded with 5 parity docs
       ------------------------------------------------------------------------- */

    class TestSodaFind : public ::testing::Test
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_SodaDatabase* sdb = nullptr;
        OCI_SodaCollection* coll = nullptr;

        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            sdb = OCI_SodaDatabaseCreate(conn);
            ASSERT_NE(nullptr, sdb);
            DropCollIfExists(sdb, OTEXT("OCILIB_SODA_OPT_COLL"));
            coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_OPT_COLL"), NULL);
            ASSERT_NE(nullptr, coll);
            for (OCI_Int32 i = 1; i <= 5; i++)
            {
                std::basic_ostringstream<otext> oss;
                oss << OTEXT("{\"seq\":") << i << OTEXT(",\"parity\":")
                    << ((i % 2) ? OTEXT("\"odd\"") : OTEXT("\"even\"")) << OTEXT("}");
                const String content = oss.str();
                const auto doc = OCI_SodaDocumentCreateFromText(sdb, content.c_str(),
                    static_cast<OCI_UInt32>(content.size() * sizeof(otext)), NULL);
                ASSERT_NE(nullptr, doc);
                ASSERT_TRUE(OCI_SodaCollectionInsertDocument(coll, doc));
                ASSERT_TRUE(OCI_SodaDocumentFree(doc));
            }
        }

        void TearDown() override
        {
            if (coll) { OCI_SodaCollectionDrop(coll); OCI_SodaCollectionFree(coll); }
            if (sdb) OCI_SodaDatabaseFree(sdb);
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

        auto* cursor = OCI_SodaCollectionFind(coll, options);

        ASSERT_NE(nullptr, cursor);
        OCI_UInt32 count = 0;
        OCI_SodaDocument* item = NULL;
        while (NULL != (item = OCI_SodaDocumentCursorGetNext(cursor)))
        {
            count++;
            ASSERT_TRUE(OCI_SodaDocumentFree(item));
        }
        EXPECT_EQ(2u, count);

        ASSERT_TRUE(OCI_SodaDocumentCursorFree(cursor));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    /* -------------------------------------------------------------------------
       Shared fixture: SODA coll seeded with one tagged doc
       ------------------------------------------------------------------------- */

    class TestSodaFindOne : public ::testing::Test
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_SodaDatabase* sdb = nullptr;
        OCI_SodaCollection* coll = nullptr;
        String key;

        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            sdb = OCI_SodaDatabaseCreate(conn);
            ASSERT_NE(nullptr, sdb);
            DropCollIfExists(sdb, OTEXT("OCILIB_SODA_FINDONE_COLL"));
            coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_FINDONE_COLL"), NULL);
            ASSERT_NE(nullptr, coll);

            const String content = OTEXT("{\"tag\":\"findone\"}");
            const auto doc = OCI_SodaDocumentCreateFromText(sdb, content.c_str(),
                static_cast<OCI_UInt32>(content.size() * sizeof(otext)), NULL);
            ASSERT_NE(nullptr, doc);
            ASSERT_TRUE(OCI_SodaCollectionInsertDocument(coll, doc));
            const auto k = OCI_SodaDocumentGetKey(doc);
            ASSERT_NE(nullptr, k);
            key = String(k);
            ASSERT_TRUE(OCI_SodaDocumentFree(doc));
        }

        void TearDown() override
        {
            if (coll) { OCI_SodaCollectionDrop(coll); OCI_SodaCollectionFree(coll); }
            if (sdb) OCI_SodaDatabaseFree(sdb);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_F(TestSodaFindOne, FindOneWithKeyOption_ReturnsSeededDoc)
    {
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);
        ASSERT_TRUE(OCI_SodaOptionsSetKey(options, key.c_str()));

        auto* foundDoc = OCI_SodaCollectionFindOne(coll, options);

        ASSERT_NE(nullptr, foundDoc);
        EXPECT_EQ(key, String(OCI_SodaDocumentGetKey(foundDoc)));

        ASSERT_TRUE(OCI_SodaDocumentFree(foundDoc));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    TEST_F(TestSodaFindOne, FindOneWithKeysOption_ReturnsSeededDoc)
    {
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);
        const otext* keys[] = { key.c_str() };
        ASSERT_TRUE(OCI_SodaOptionsSetKeys(options, keys, 1));

        auto* foundDoc = OCI_SodaCollectionFindOne(coll, options);

        ASSERT_NE(nullptr, foundDoc);

        ASSERT_TRUE(OCI_SodaDocumentFree(foundDoc));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    TEST(TestSoda, DocCount)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_COUNT_COLL"));
        const auto coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_COUNT_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        for (OCI_Int32 i = 0; i < 3; i++)
        {
            const String content = (i % 2) ? OTEXT("{\"grp\":1}") : OTEXT("{\"grp\":0}");

            const auto doc = OCI_SodaDocumentCreateFromText
            (
                sdb,
                content.c_str(),
                static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
                NULL
            );
            ASSERT_NE(nullptr, doc);
            ASSERT_TRUE(OCI_SodaCollectionInsertDocument(coll, doc));
            ASSERT_TRUE(OCI_SodaDocumentFree(doc));
        }

        OCI_ConnectionCommit(conn);

        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        const OCI_UInt64 cnt = OCI_SodaCollectionDocumentCount(coll, options);
        ASSERT_EQ(3u, cnt);

        ASSERT_NE(nullptr, options);
        ASSERT_TRUE(OCI_SodaOptionsSetFilter(options, OTEXT("{\"grp\":0}")));

        ASSERT_EQ(2u, OCI_SodaCollectionDocumentCount(coll, options));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));

        ASSERT_EQ(2u, OCI_SodaCollectionDocumentCountWithFilter(coll, OTEXT("{\"grp\":0}")));

        ASSERT_TRUE(OCI_ConnectionCommit(conn));
        ASSERT_TRUE(OCI_SodaCollectionDrop(coll));
        ASSERT_TRUE(OCI_SodaCollectionFree(coll));

        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, ReplaceOneByKey)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_REPLKEY_COLL"));
        const auto coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_REPLKEY_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        const String content = OTEXT("{\"v\":1}");

        const auto doc = OCI_SodaDocumentCreateFromText
        (
            sdb,
            content.c_str(),
            static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, doc);
        ASSERT_TRUE(OCI_SodaCollectionInsertDocument(coll, doc));

        const String key = OCI_SodaDocumentGetKey(doc);
        ASSERT_FALSE(key.empty());

        const String newContent = OTEXT("{\"v\":2}");

        const auto newDoc = OCI_SodaDocumentCreateFromText
        (
            sdb,
            newContent.c_str(),
            static_cast<OCI_UInt32>(newContent.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, newDoc);

        ASSERT_TRUE(OCI_SodaCollectionReplaceOneByKey(coll, key.c_str(), newDoc, FALSE));

        const auto foundDoc = OCI_SodaCollectionFindOneByKey(coll, key.c_str());
        ASSERT_NE(nullptr, foundDoc);

        const auto text = OCI_SodaDocumentGetText(foundDoc);
        ASSERT_NE(nullptr, text);
        ASSERT_NE(String::npos, String(text).find(OTEXT("\"v\":2")));

        ASSERT_TRUE(OCI_SodaDocumentFree(foundDoc));

        /* replace-and-get updates the passed document metadata */

        const String newContent2 = OTEXT("{\"v\":3}");
        const auto newDoc2 = OCI_SodaDocumentCreateFromText
        (
            sdb,
            newContent2.c_str(),
            static_cast<OCI_UInt32>(newContent2.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, newDoc2);

        ASSERT_TRUE(OCI_SodaCollectionReplaceOneByKey(coll, key.c_str(), newDoc2, TRUE));
        ASSERT_NE(nullptr, OCI_SodaDocumentGetVersion(newDoc2));

        ASSERT_TRUE(OCI_SodaDocumentFree(newDoc2));
        ASSERT_TRUE(OCI_SodaDocumentFree(newDoc));
        ASSERT_TRUE(OCI_SodaDocumentFree(doc));

        ASSERT_TRUE(OCI_ConnectionCommit(conn));
        ASSERT_TRUE(OCI_SodaCollectionDrop(coll));
        ASSERT_TRUE(OCI_SodaCollectionFree(coll));

        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    /* -------------------------------------------------------------------------
       Shared fixture: SODA coll seeded with one versioned doc
       ------------------------------------------------------------------------- */

    class TestSodaReplace : public ::testing::Test
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_SodaDatabase* sdb = nullptr;
        OCI_SodaCollection* coll = nullptr;
        String key;
        String version;

        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            sdb = OCI_SodaDatabaseCreate(conn);
            ASSERT_NE(nullptr, sdb);
            DropCollIfExists(sdb, OTEXT("OCILIB_SODA_REP_COLL"));
            coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_REP_COLL"), NULL);
            ASSERT_NE(nullptr, coll);

            const String content = OTEXT("{\"v\":10}");
            const auto doc = OCI_SodaDocumentCreateFromText(sdb, content.c_str(),
                static_cast<OCI_UInt32>(content.size() * sizeof(otext)), NULL);
            ASSERT_NE(nullptr, doc);
            ASSERT_TRUE(OCI_SodaCollectionInsertDocument(coll, doc));
            key = String(OCI_SodaDocumentGetKey(doc));
            version = String(OCI_SodaDocumentGetVersion(doc));
            ASSERT_TRUE(OCI_SodaDocumentFree(doc));
        }

        void TearDown() override
        {
            if (coll) { OCI_SodaCollectionDrop(coll); OCI_SodaCollectionFree(coll); }
            if (sdb) OCI_SodaDatabaseFree(sdb);
            if (conn) OCI_ConnectionFree(conn);
        }

        OCI_SodaDocument* MakeDoc(const otext* content)
        {
            return OCI_SodaDocumentCreateFromText(sdb, content,
                static_cast<OCI_UInt32>(String(content).size() * sizeof(otext)), NULL);
        }
    };

    TEST_F(TestSodaReplace, ReplaceOneByKeyOption_ReplacesDoc)
    {
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);
        ASSERT_TRUE(OCI_SodaOptionsSetKey(options, key.c_str()));
        OCI_SodaDocument* newDoc = MakeDoc(OTEXT("{\"v\":20}"));
        ASSERT_NE(nullptr, newDoc);

        EXPECT_TRUE(OCI_SodaCollectionReplaceOne(coll, options, newDoc, FALSE));

        ASSERT_TRUE(OCI_SodaDocumentFree(newDoc));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    TEST_F(TestSodaReplace, ReplaceOneByKeyAndVersion_ReturnsUpdatedDoc)
    {
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);
        ASSERT_TRUE(OCI_SodaOptionsSetKey(options, key.c_str()));
        ASSERT_TRUE(OCI_SodaOptionsSetVersion(options, version.c_str()));
        OCI_SodaDocument* newDoc = MakeDoc(OTEXT("{\"v\":30}"));
        ASSERT_NE(nullptr, newDoc);

        ASSERT_TRUE(OCI_SodaCollectionReplaceOne(coll, options, newDoc, TRUE));

        EXPECT_NE(nullptr, OCI_SodaDocumentGetVersion(newDoc));

        ASSERT_TRUE(OCI_SodaDocumentFree(newDoc));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }
    TEST(TestSoda, RemoveOneByKey)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_REMKEY_COLL"));
        const auto coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_REMKEY_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        const String content = OTEXT("{\"r\":1}");

        const auto doc = OCI_SodaDocumentCreateFromText
        (
            sdb,
            content.c_str(),
            static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, doc);
        ASSERT_TRUE(OCI_SodaCollectionInsertDocument(coll, doc));

        const String key = OCI_SodaDocumentGetKey(doc);
        ASSERT_FALSE(key.empty());
        ASSERT_TRUE(OCI_SodaDocumentFree(doc));

        ASSERT_EQ(1u, OCI_SodaCollectionDocumentCount(coll, NULL));
        ASSERT_TRUE(OCI_SodaCollectionRemoveOneByKey(coll, key.c_str()));
        ASSERT_EQ(0u, OCI_SodaCollectionDocumentCount(coll, NULL));

        ASSERT_TRUE(OCI_ConnectionCommit(conn));
        ASSERT_TRUE(OCI_SodaCollectionDrop(coll));
        ASSERT_TRUE(OCI_SodaCollectionFree(coll));

        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, RemoveWithOptions)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_REMOPT_COLL"));
        const auto coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_REMOPT_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        for (OCI_Int32 i = 0; i < 4; i++)
        {
            const String content = (i % 2) ? OTEXT("{\"del\":1}") : OTEXT("{\"del\":0}");

            const auto doc = OCI_SodaDocumentCreateFromText
            (
                sdb,
                content.c_str(),
                static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
                NULL
            );
            ASSERT_NE(nullptr, doc);
            ASSERT_TRUE(OCI_SodaCollectionInsertDocument(coll, doc));
            ASSERT_TRUE(OCI_SodaDocumentFree(doc));
        }

        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);
        ASSERT_TRUE(OCI_SodaOptionsSetFilter(options, OTEXT("{\"del\":1}")));

        OCI_UInt64 removed = 0;
        ASSERT_TRUE(OCI_SodaCollectionRemove(coll, options, &removed));
        ASSERT_EQ(2u, removed);
        ASSERT_EQ(2u, OCI_SodaCollectionDocumentCount(coll, NULL));

        ASSERT_TRUE(OCI_SodaOptionsFree(options));

        ASSERT_TRUE(OCI_ConnectionCommit(conn));
        ASSERT_TRUE(OCI_SodaCollectionDrop(coll));
        ASSERT_TRUE(OCI_SodaCollectionFree(coll));

        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, Truncate)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_TRUNC_COLL"));
        const auto coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_TRUNC_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        const String content = OTEXT("{\"t\":1}");

        const auto doc = OCI_SodaDocumentCreateFromText
        (
            sdb,
            content.c_str(),
            static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, doc);
        ASSERT_TRUE(OCI_SodaCollectionInsertDocument(coll, doc));
        ASSERT_TRUE(OCI_SodaDocumentFree(doc));

        ASSERT_EQ(1u, OCI_SodaCollectionDocumentCount(coll, NULL));
        ASSERT_TRUE(OCI_SodaCollectionTruncate(coll));
        ASSERT_EQ(0u, OCI_SodaCollectionDocumentCount(coll, NULL));

        ASSERT_TRUE(OCI_ConnectionCommit(conn));
        ASSERT_TRUE(OCI_SodaCollectionDrop(coll));
        ASSERT_TRUE(OCI_SodaCollectionFree(coll));

        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
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

            sdb = OCI_SodaDatabaseCreate(conn);
            ASSERT_NE(nullptr, sdb);

            DropCollIfExists(sdb, OTEXT("OCILIB_SODA_ERR_COLL"));
            coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_ERR_COLL"), NULL);
            ASSERT_NE(nullptr, coll);
        }

        void TearDown() override
        {
            OCI_ConnectionCommit(conn);
            OCI_SodaCollectionDrop(coll);
            OCI_SodaCollectionFree(coll);
            OCI_SodaDatabaseFree(sdb);
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
        OCI_SodaDatabase*     sdb{ nullptr };
        OCI_SodaCollection*   coll{ nullptr };
    };

    TEST_F(TestSodaErrors, SodaDbFnsWithNullArgs_ReturnFalse)
    {
        ASSERT_EQ(nullptr, OCI_SodaDatabaseCreateCollection(NULL, OTEXT("X"), NULL));
        ASSERT_EQ(nullptr, OCI_SodaDatabaseOpenCollection(NULL, OTEXT("X")));
        ASSERT_EQ(nullptr, OCI_SodaDatabaseOpenCollection(sdb, NULL));
        ASSERT_EQ(nullptr, OCI_SodaDatabaseGetCollections(NULL, NULL));
        ASSERT_EQ(nullptr, OCI_SodaDatabaseGetConnection(NULL));
    }

    TEST_F(TestSodaErrors, SodaDocCreateWithNullArgs_ReturnsNull)
    {
        ASSERT_EQ(nullptr, OCI_SodaDocumentCreateFromText(NULL, OTEXT("{}"), 2, NULL));
        ASSERT_EQ(nullptr, OCI_SodaDocumentCreateFromJson(NULL, NULL, NULL));
    }

    TEST_F(TestSodaErrors, SodaDocCreateWithNullContent_ReturnsDoc)
    {
        /* NULL content is valid - creates an empty document */
        OCI_SodaDocument* emptyDoc = OCI_SodaDocumentCreateFromText(sdb, NULL, 0, NULL);
        ASSERT_NE(nullptr, emptyDoc);
        ASSERT_TRUE(OCI_SodaDocumentFree(emptyDoc));
    }

    TEST_F(TestSodaErrors, CollFnsOnNullHandle_ReturnFalse)
    {
        ASSERT_EQ(nullptr, OCI_SodaCollectionGetName(NULL));
        ASSERT_EQ(nullptr, OCI_SodaCollectionGetMetadata(NULL));
        ASSERT_EQ(nullptr, OCI_SodaCollectionGetConnection(NULL));
        ASSERT_FALSE(OCI_SodaCollectionIsMapped(NULL));
        ASSERT_FALSE(OCI_SodaCollectionDrop(NULL));
        ASSERT_FALSE(OCI_SodaCollectionTruncate(NULL));
        ASSERT_FALSE(OCI_SodaCollectionFree(NULL));
    }

    TEST_F(TestSodaErrors, DocFnsOnNullHandle_ReturnFalse)
    {
        ASSERT_EQ(nullptr, OCI_SodaDocumentGetKey(NULL));
        ASSERT_EQ(nullptr, OCI_SodaDocumentGetVersion(NULL));
        ASSERT_EQ(nullptr, OCI_SodaDocumentGetCreatedOn(NULL));
        ASSERT_EQ(nullptr, OCI_SodaDocumentGetLastModified(NULL));
        ASSERT_EQ(nullptr, OCI_SodaDocumentGetMediaType(NULL));
        ASSERT_EQ(nullptr, OCI_SodaDocumentGetText(NULL));
        ASSERT_EQ(nullptr, OCI_SodaDocumentGetJson(NULL));
        ASSERT_FALSE(OCI_SodaDocumentFree(NULL));
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
        ASSERT_EQ(nullptr, OCI_SodaCollectionCursorGetNext(NULL));
        ASSERT_FALSE(OCI_SodaCollectionCursorFree(NULL));
        ASSERT_EQ(nullptr, OCI_SodaDocumentCursorGetNext(NULL));
        ASSERT_FALSE(OCI_SodaDocumentCursorFree(NULL));
    }

    TEST_F(TestSodaErrors, CollOpsWithNullArgs_ReturnFalse)
    {
        ASSERT_FALSE(OCI_SodaCollectionInsertDocument(NULL, NULL));
        ASSERT_FALSE(OCI_SodaCollectionInsertDocument(coll, NULL));
        ASSERT_EQ(nullptr, OCI_SodaCollectionFind(NULL, NULL));
        ASSERT_EQ(nullptr, OCI_SodaCollectionFindOne(NULL, NULL));
        ASSERT_EQ(nullptr, OCI_SodaCollectionFindOneByKey(NULL, OTEXT("k")));
        ASSERT_EQ(nullptr, OCI_SodaCollectionFindOneByKey(coll, NULL));
        ASSERT_EQ(0u, OCI_SodaCollectionDocumentCount(NULL, NULL));
        ASSERT_EQ(0u, OCI_SodaCollectionDocumentCountWithFilter(NULL, OTEXT("{}")));
        ASSERT_FALSE(OCI_SodaCollectionReplaceOne(NULL, NULL, NULL, FALSE));
        ASSERT_FALSE(OCI_SodaCollectionReplaceOne(NULL, NULL, NULL, TRUE));
        ASSERT_FALSE(OCI_SodaCollectionReplaceOneByKey(NULL, NULL, NULL, FALSE));
        ASSERT_FALSE(OCI_SodaCollectionReplaceOneByKey(NULL, NULL, NULL, TRUE));
        ASSERT_FALSE(OCI_SodaCollectionRemove(NULL, NULL, NULL));
        ASSERT_FALSE(OCI_SodaCollectionRemoveOneByKey(NULL, NULL));
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

            const auto sdb = OCI_SodaDatabaseCreate(conn);
            ASSERT_NE(nullptr, sdb);

            const otext* metadata = OTEXT("{\"keyColumn\":{\"name\":\"ID\",\"assignmentMethod\":\"CLIENT\",\"sqlType\":\"VARCHAR2\",\"maxLength\":255}}");

            DropCollIfExists(sdb, OTEXT("OCILIB_SODA_MK_COLL"));
            const auto coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_MK_COLL"), metadata);
            ASSERT_NE(nullptr, coll);

            for (OCI_UInt32 i = 0; i < DocCount; i++)
            {
                std::basic_ostringstream<otext> kss;
                kss << OTEXT("mk") << i;
                keystr[i] = kss.str();
                keys[i]   = keystr[i].c_str();

                const String content = OTEXT("{\"idx\":1}");

                const auto doc = OCI_SodaDocumentCreateFromText
                (
                    sdb,
                    content.c_str(),
                    static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
                    keys[i]
                );
                ASSERT_NE(nullptr, doc);
                ASSERT_TRUE(OCI_SodaCollectionInsertDocument(coll, doc));
                ASSERT_TRUE(OCI_SodaDocumentFree(doc));
            }

            ASSERT_TRUE(OCI_ConnectionCommit(conn));
            ASSERT_TRUE(OCI_SodaCollectionFree(coll));
            ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
            ASSERT_TRUE(OCI_ConnectionFree(conn));
        }

        static void TearDownTestCase()
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            const auto sdb = OCI_SodaDatabaseCreate(conn);
            ASSERT_NE(nullptr, sdb);

            DropCollIfExists(sdb, OTEXT("OCILIB_SODA_MK_COLL"));

            ASSERT_TRUE(OCI_ConnectionCommit(conn));
            ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
            ASSERT_TRUE(OCI_ConnectionFree(conn));
        }

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            sdb = OCI_SodaDatabaseCreate(conn);
            ASSERT_NE(nullptr, sdb);

            coll = OCI_SodaDatabaseOpenCollection(sdb, OTEXT("OCILIB_SODA_MK_COLL"));
            ASSERT_NE(nullptr, coll);
        }

        void TearDown() override
        {
            OCI_ConnectionCommit(conn);
            OCI_SodaCollectionFree(coll);
            OCI_SodaDatabaseFree(sdb);
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
        OCI_SodaDatabase*     sdb{ nullptr };
        OCI_SodaCollection*   coll{ nullptr };

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

        auto* cursor = OCI_SodaCollectionFind(coll, options);
        ASSERT_NE(nullptr, cursor);

        OCI_UInt32 count = 0;
        OCI_SodaDocument* item = NULL;
        while (NULL != (item = OCI_SodaDocumentCursorGetNext(cursor)))
        {
            count++;
            ASSERT_TRUE(OCI_SodaDocumentFree(item));
        }
        ASSERT_EQ(DocCount, count);

        ASSERT_TRUE(OCI_SodaDocumentCursorFree(cursor));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    TEST_F(TestSodaMultiKey, FindOneWithMixedKeys_ReturnsMatchingDoc)
    {
        /* first keys miss, a later one hits */
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        const otext* mixedKeys[] = { OTEXT("nope1"), keys[2], OTEXT("nope2") };
        ASSERT_TRUE(OCI_SodaOptionsSetKeys(options, mixedKeys, 3));

        auto* found = OCI_SodaCollectionFindOne(coll, options);
        ASSERT_NE(nullptr, found);
        ASSERT_EQ(String(keys[2]), String(OCI_SodaDocumentGetKey(found)));
        ASSERT_TRUE(OCI_SodaDocumentFree(found));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    TEST_F(TestSodaMultiKey, FindOneWithMissingKeys_ReturnsDoc)
    {
        /* full miss loop + doc cleanup */
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        const otext* missKeys[] = { OTEXT("nope1"), OTEXT("nope2") };
        ASSERT_TRUE(OCI_SodaOptionsSetKeys(options, missKeys, 2));

        ASSERT_EQ(nullptr, OCI_SodaCollectionFindOne(coll, options));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    TEST_F(TestSodaMultiKey, DocCountWithKeyList_ReturnsCount)
    {
        /* per-key counts are summed */
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        const otext* countKeys[] = { keys[0], keys[1], OTEXT("nope") };
        ASSERT_TRUE(OCI_SodaOptionsSetKeys(options, countKeys, 3));
        ASSERT_EQ(2u, OCI_SodaCollectionDocumentCount(coll, options));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    TEST_F(TestSodaMultiKey, ReplaceOneWithKeyList_UpdatesDoc)
    {
        const String repl = OTEXT("{\"idx\":2}");
        auto* replDoc = OCI_SodaDocumentCreateFromText
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
        ASSERT_TRUE(OCI_SodaCollectionReplaceOne(coll, options, replDoc, FALSE));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
        ASSERT_TRUE(OCI_SodaDocumentFree(replDoc));
    }

    TEST_F(TestSodaMultiKey, ReplaceOneAndGetWithKeyList_ReturnsDoc)
    {
        const String repl = OTEXT("{\"idx\":2}");
        auto* replDoc = OCI_SodaDocumentCreateFromText
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
        ASSERT_TRUE(OCI_SodaCollectionReplaceOne(coll, options, replDoc, TRUE));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
        ASSERT_TRUE(OCI_SodaDocumentFree(replDoc));
    }

    TEST_F(TestSodaMultiKey, RemoveWithKeyList_RemovesDocs)
    {
        /* per-key removes are summed */
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        const otext* rmKeys[] = { keys[5], keys[6], OTEXT("nope") };
        ASSERT_TRUE(OCI_SodaOptionsSetKeys(options, rmKeys, 3));

        OCI_UInt64 removed = 0;
        ASSERT_TRUE(OCI_SodaCollectionRemove(coll, options, &removed));
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

            const auto sdb = OCI_SodaDatabaseCreate(conn);
            ASSERT_NE(nullptr, sdb);

            DropCollIfExists(sdb, OTEXT("OCILIB_SODA_NOPTS_COLL"));
            const auto coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_NOPTS_COLL"), NULL);
            ASSERT_NE(nullptr, coll);

            const String content = OTEXT("{\"name\":\"nopts\"}");

            const auto doc = OCI_SodaDocumentCreateFromText
            (
                sdb,
                content.c_str(),
                static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
                NULL
            );
            ASSERT_NE(nullptr, doc);
            ASSERT_TRUE(OCI_SodaCollectionInsertDocument(coll, doc));
            ASSERT_TRUE(OCI_SodaDocumentFree(doc));

            ASSERT_TRUE(OCI_ConnectionCommit(conn));
            ASSERT_TRUE(OCI_SodaCollectionFree(coll));
            ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
            ASSERT_TRUE(OCI_ConnectionFree(conn));
        }

        static void TearDownTestCase()
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            const auto sdb = OCI_SodaDatabaseCreate(conn);
            ASSERT_NE(nullptr, sdb);

            DropCollIfExists(sdb, OTEXT("OCILIB_SODA_NOPTS_COLL"));

            ASSERT_TRUE(OCI_ConnectionCommit(conn));
            ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
            ASSERT_TRUE(OCI_ConnectionFree(conn));
        }

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            sdb = OCI_SodaDatabaseCreate(conn);
            ASSERT_NE(nullptr, sdb);

            coll = OCI_SodaDatabaseOpenCollection(sdb, OTEXT("OCILIB_SODA_NOPTS_COLL"));
            ASSERT_NE(nullptr, coll);
        }

        void TearDown() override
        {
            OCI_ConnectionCommit(conn);
            OCI_SodaCollectionFree(coll);
            OCI_SodaDatabaseFree(sdb);
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
        OCI_SodaDatabase*     sdb{ nullptr };
        OCI_SodaCollection*   coll{ nullptr };
    };

    TEST_F(TestSodaNullOptions, FindWithNullOptions_UsesDefaults)
    {
        /* NULL options: internal temporary options are created */
        auto* cursor = OCI_SodaCollectionFind(coll, NULL);
        ASSERT_NE(nullptr, cursor);

        OCI_UInt32 count = 0;
        OCI_SodaDocument* item = NULL;
        while (NULL != (item = OCI_SodaDocumentCursorGetNext(cursor)))
        {
            count++;
            ASSERT_TRUE(OCI_SodaDocumentFree(item));
        }
        ASSERT_EQ(1u, count);
        ASSERT_TRUE(OCI_SodaDocumentCursorFree(cursor));
    }

    TEST_F(TestSodaNullOptions, FindOneWithNullOptions_UsesDefaults)
    {
        auto* found = OCI_SodaCollectionFindOne(coll, NULL);
        ASSERT_NE(nullptr, found);
        ASSERT_TRUE(OCI_SodaDocumentFree(found));
    }

    TEST_F(TestSodaNullOptions, DocCountWithNullOptions_CountsAll)
    {
        ASSERT_EQ(1u, OCI_SodaCollectionDocumentCount(coll, NULL));
    }

    TEST_F(TestSodaNullOptions, FindOneByUnknownKey_ReturnsNull)
    {
        /* NO_DATA cleanup arm */
        ASSERT_EQ(nullptr, OCI_SodaCollectionFindOneByKey(coll, OTEXT("no_such_key")));
    }

    TEST_F(TestSodaNullOptions, ReplaceOneWithNullOptions_UsesDefaults)
    {
        /* NULL options exercises the temporary-options arm; an empty
           filter does not replace the doc, so only the call matters */
        const String repl = OTEXT("{\"name\":\"nopts2\"}");

        auto* replDoc = OCI_SodaDocumentCreateFromText
        (
            sdb, repl.c_str(),
            static_cast<OCI_UInt32>(repl.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, replDoc);

        (void) OCI_SodaCollectionReplaceOne(coll, NULL, replDoc, FALSE);
        ASSERT_TRUE(OCI_SodaDocumentFree(replDoc));
    }

    TEST_F(TestSodaNullOptions, ReplaceOneAndGetWithNullOptions_UsesDefaults)
    {
        const String repl = OTEXT("{\"name\":\"nopts2\"}");

        auto* replDoc = OCI_SodaDocumentCreateFromText
        (
            sdb, repl.c_str(),
            static_cast<OCI_UInt32>(repl.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, replDoc);

        (void) OCI_SodaCollectionReplaceOne(coll, NULL, replDoc, TRUE);
        ASSERT_TRUE(OCI_SodaDocumentFree(replDoc));
    }

    TEST_F(TestSodaNullOptions, RemoveWithNullOptions_UsesDefaults)
    {
        OCI_UInt64 removed = 0;
        (void) OCI_SodaCollectionRemove(coll, NULL, &removed);
    }

    /* --- public-API error paths: NULL handles, invalid args and bad state
       hit the CHECK_PTR / CHECK_OCI arms the dropped fault tests covered --- */

    TEST(TestSoda, NullCollErrors)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        /* every coll call rejects a NULL collection handle */
        ASSERT_FALSE(OCI_SodaCollectionDrop(nullptr));
        ASSERT_FALSE(OCI_SodaCollectionTruncate(nullptr));
        ASSERT_FALSE(OCI_SodaCollectionInsertDocument(nullptr, nullptr));
        ASSERT_EQ(nullptr, OCI_SodaCollectionGetName(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaCollectionGetConnection(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaCollectionGetMetadata(nullptr));
        ASSERT_EQ(0u, OCI_SodaCollectionIsMapped(nullptr));
        ASSERT_EQ(0u, OCI_SodaCollectionDocumentCount(nullptr, nullptr));
        ASSERT_EQ(nullptr, OCI_SodaCollectionFind(nullptr, nullptr));
        ASSERT_EQ(nullptr, OCI_SodaCollectionFindOne(nullptr, nullptr));
        ASSERT_EQ(nullptr, OCI_SodaCollectionFindOneByKey(nullptr, nullptr));
        ASSERT_FALSE(OCI_SodaCollectionRemove(nullptr, nullptr, nullptr));
        ASSERT_FALSE(OCI_SodaCollectionRemoveOneByKey(nullptr, nullptr));
        ASSERT_FALSE(OCI_SodaCollectionReplaceOne(nullptr, nullptr, nullptr, FALSE));
        ASSERT_FALSE(OCI_SodaCollectionReplaceOne(nullptr, nullptr, nullptr, TRUE));
        ASSERT_FALSE(OCI_SodaCollectionReplaceOneByKey(nullptr, nullptr, nullptr, TRUE));
        ASSERT_FALSE(OCI_SodaCollectionReplaceOneByKey(nullptr, nullptr, nullptr, FALSE));
        ASSERT_EQ(0u, OCI_SodaCollectionDocumentCountWithFilter(nullptr, nullptr));
    }

    TEST(TestSoda, NullDbErrors)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        /* db calls reject NULL db/collection-name args */
        ASSERT_EQ(nullptr, OCI_SodaDatabaseCreateCollection(nullptr, nullptr, nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDatabaseOpenCollection(nullptr, nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDatabaseGetCollections(nullptr, nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDatabaseGetConnection(nullptr));
        ASSERT_FALSE(OCI_SodaDatabaseFree(nullptr));
    }

    TEST(TestSoda, NullDocErrors)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        /* doc calls reject NULL docs / null text */
        ASSERT_FALSE(OCI_SodaDocumentFree(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDocumentGetKey(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDocumentGetJson(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDocumentGetText(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDocumentGetCreatedOn(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDocumentGetLastModified(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDocumentGetMediaType(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDocumentGetVersion(nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDocumentCreateFromJson(nullptr, nullptr, nullptr));
        ASSERT_EQ(nullptr, OCI_SodaDocumentCreateFromText(nullptr, nullptr, 0, nullptr));
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

        const auto sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);

        /* unknown collection: OpenColl may return NULL (error) or a valid
           handle - both are acceptable; only no-crash matters */
        const auto nocol = OCI_SodaDatabaseOpenCollection(sdb, OTEXT("TestSodaNoSuchColl42"));
        if (nocol) { ASSERT_TRUE(OCI_SodaCollectionFree(nocol)); }
        ASSERT_EQ(nullptr, OCI_SodaCollectionFindOneByKey(nullptr, OTEXT("k")));
        ASSERT_EQ(0u, OCI_SodaCollectionDocumentCount(nullptr, NULL));

        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    /* -------------------------------------------------------------------------
       Shared fixture: SODA db + metadata-backed collection
       ------------------------------------------------------------------------- */

    class TestSodaOps : public ::testing::Test
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_SodaDatabase* sdb = nullptr;
        OCI_SodaCollection* coll = nullptr;

        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            sdb = OCI_SodaDatabaseCreate(conn);
            ASSERT_NE(nullptr, sdb);
            DropCollIfExists(sdb, OTEXT("OCILIB_SODA_TEST_COLL"));
            const String collMeta = OTEXT("{\"keyColumn\":{\"name\":\"ID\",\"sqlType\":\"VARCHAR2\",\"maxLength\":255,\"assignmentMethod\":\"CLIENT\"}}");
            coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_TEST_COLL"), collMeta.c_str());
            ASSERT_NE(nullptr, coll);
        }

        void TearDown() override
        {
            if (coll) { OCI_SodaCollectionDrop(coll); OCI_SodaCollectionFree(coll); }
            if (sdb) OCI_SodaDatabaseFree(sdb);
            if (conn) OCI_ConnectionFree(conn);
        }

        OCI_SodaDocument* MakeDoc(const String& content, const otext* key)
        {
            return OCI_SodaDocumentCreateFromText(sdb, content.c_str(), static_cast<OCI_UInt32>(content.size() * sizeof(otext)), key);
        }
    };

    TEST_F(TestSodaOps, SodaCollInsertMany_InsertsDocs)
    {
        OCI_SodaDocument* docs[3] = {
            MakeDoc(OTEXT("{\"a\":1}"), OTEXT("k1")),
            MakeDoc(OTEXT("{\"a\":2}"), OTEXT("k2")),
            MakeDoc(OTEXT("{\"a\":3}"), OTEXT("k3"))
        };
        ASSERT_NE(nullptr, docs[0]);
        ASSERT_NE(nullptr, docs[1]);
        ASSERT_NE(nullptr, docs[2]);

        EXPECT_TRUE(OCI_SodaCollectionInsertMany(coll, docs, 3, FALSE));

        ASSERT_TRUE(OCI_SodaDocumentFree(docs[0]));
        ASSERT_TRUE(OCI_SodaDocumentFree(docs[1]));
        ASSERT_TRUE(OCI_SodaDocumentFree(docs[2]));
    }

    TEST_F(TestSodaOps, SodaDocGetIsJsonOnJsonDoc_ReturnsTrue)
    {
        OCI_SodaDocument* d = MakeDoc(OTEXT("{\"a\":1}"), OTEXT("k1"));
        ASSERT_NE(nullptr, d);

        EXPECT_TRUE(OCI_SodaDocumentGetIsJson(d));

        ASSERT_TRUE(OCI_SodaDocumentFree(d));
    }

    TEST_F(TestSodaOps, SodaCollInsertManyWithMeta_ReturnsDocVersion)
    {
        OCI_SodaDocument* e = MakeDoc(OTEXT("{\"a\":1}"), OTEXT("k4"));
        ASSERT_NE(nullptr, e);
        OCI_SodaDocument* edocs[1] = { e };

        ASSERT_TRUE(OCI_SodaCollectionInsertMany(coll, edocs, 1, TRUE));

        EXPECT_NE(nullptr, OCI_SodaDocumentGetVersion(e));

        ASSERT_TRUE(OCI_SodaDocumentFree(e));
    }

    TEST_F(TestSodaOps, SodaCollSave_UpsertsAndReturnsVersion)
    {
        OCI_SodaDocument* sv = MakeDoc(OTEXT("{\"a\":99}"), OTEXT("k1"));
        ASSERT_NE(nullptr, sv);

        ASSERT_TRUE(OCI_SodaCollectionSave(coll, sv, FALSE));

        EXPECT_NE(nullptr, OCI_SodaDocumentGetVersion(sv));

        ASSERT_TRUE(OCI_SodaDocumentFree(sv));
    }

    TEST_F(TestSodaOps, SodaCollCreateAndDropIndex_WorksWhenClientSupports)
    {
        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_IDX_COLL"));
        OCI_SodaCollection* coll2 = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_IDX_COLL"), NULL);
        ASSERT_NE(nullptr, coll2);
        OCI_SodaDocument* seed = MakeDoc(OTEXT("{\"a\":1}"), NULL);
        ASSERT_TRUE(OCI_SodaCollectionInsertDocument(coll2, seed));
        ASSERT_TRUE(OCI_SodaDocumentFree(seed));
        ASSERT_TRUE(OCI_ConnectionCommit(conn));
        const String spec = OTEXT("{\"name\":\"OCILIB_SODA_TEST_IDX\"}");

        /* fails inside the OCI call in wide builds (ORA-40671) — MBCS only */
        const OCI_Boolean cix = OCI_SodaCollectionCreateIndex(coll2, spec.c_str());

        if (cix)
        {
            EXPECT_TRUE(OCI_SodaCollectionDropIndex(coll2, OTEXT("OCILIB_SODA_TEST_IDX")));
        }

        ASSERT_TRUE(OCI_SodaCollectionDrop(coll2));
        ASSERT_TRUE(OCI_SodaCollectionFree(coll2));
    }

    TEST_F(TestSodaOps, SodaCollGetDataGuideWithoutGuide_NoError)
    {
        OCI_SodaDocument* dg = OCI_SodaCollectionGetDataGuide(coll);

        if (NULL != dg)
        {
            ASSERT_TRUE(OCI_SodaDocumentFree(dg));
        }
    }

    TEST_F(TestSodaOps, SodaDbGetCollectionNames_ReturnsNamesList)
    {
        OCI_UInt32 ncount = 0;

        otext** collNames = OCI_SodaDatabaseGetCollectionNames(sdb, &ncount);

        ASSERT_NE(nullptr, collNames);
        ASSERT_TRUE(OCI_SodaDatabaseFreeCollectionNames(sdb, collNames, ncount));
    }

    TEST_F(TestSodaOps, SodaCollListIndexes_NoErrorWhenSupported)
    {
        OCI_UInt32 ncount = 0;

        /* fails inside the OCI call in wide builds (OCI-22303) — tolerant */
        otext** idxNames = OCI_SodaCollectionListIndexes(coll, &ncount);

        if (NULL != idxNames)
        {
            ASSERT_TRUE(OCI_SodaCollectionFreeIndexNames(coll, idxNames, ncount));
        }
    }

#ifdef OCI_CHARSET_MBCS

    /* The OCISodaIndex* calls work in MBCS builds; in wide builds they fail
       inside the Oracle client (ORA-40671 / OCI-22303, IndexDrop even SEGVs) —
       the real assertions live here, BulkSaveIndexAndMeta keeps diagnostic-only
       calls that run in both modes */
    TEST(TestSoda, IndexCreateDropList)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        OCI_SodaDatabase* sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_IDX"));

        OCI_SodaCollection* coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("OCILIB_SODA_IDX"), NULL);
        ASSERT_NE(nullptr, coll);

        const String content = OTEXT("{\"a\":1}");
        OCI_SodaDocument* seed = OCI_SodaDocumentCreateFromText(sdb, content.c_str(), static_cast<OCI_UInt32>(content.size() * sizeof(otext)), NULL);
        ASSERT_NE(nullptr, seed);
        ASSERT_TRUE(OCI_SodaCollectionInsertDocument(coll, seed));
        ASSERT_TRUE(OCI_SodaDocumentFree(seed));
        ASSERT_TRUE(OCI_ConnectionCommit(conn));

        ASSERT_TRUE(OCI_SodaCollectionCreateIndex(coll, OTEXT("{\"name\":\"OCILIB_SODA_IDX_NAME\"}")));

        OCI_UInt32 ncount = 0;
        otext** names = OCI_SodaCollectionListIndexes(coll, &ncount);
        ASSERT_NE(nullptr, names);
        ASSERT_GE(ncount, 1u);
        ASSERT_TRUE(OCI_SodaCollectionFreeIndexNames(coll, names, ncount));

        ASSERT_TRUE(OCI_SodaCollectionDropIndex(coll, OTEXT("OCILIB_SODA_IDX_NAME")));
        ASSERT_TRUE(OCI_SodaCollectionDrop(coll));
        ASSERT_TRUE(OCI_SodaCollectionFree(coll));
        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

#endif






    TEST(TestSoda, CollInsertDoc_MalformedDoc_ReturnsFalse)
    {
        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);
        const auto coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("TestSodaErrDoc"), nullptr);
        ASSERT_NE(nullptr, coll);
        const auto doc = OCI_SodaDocumentCreateFromText(sdb, OTEXT("{bad json"), 9, nullptr);
        ASSERT_NE(nullptr, doc);

        ASSERT_FALSE(OCI_SodaCollectionInsertDocument(coll, doc));

        ASSERT_TRUE(OCI_SodaDocumentFree(doc));
        ASSERT_TRUE(OCI_SodaCollectionDrop(coll));
        ASSERT_TRUE(OCI_SodaCollectionFree(coll));
        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, CollCreateIndex_MalformedSpec_ReturnsFalse)
    {
        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto sdb = OCI_SodaDatabaseCreate(conn);
        ASSERT_NE(nullptr, sdb);
        const auto coll = OCI_SodaDatabaseCreateCollection(sdb, OTEXT("TestSodaErrIdx"), nullptr);
        ASSERT_NE(nullptr, coll);

        ASSERT_FALSE(OCI_SodaCollectionCreateIndex(coll, OTEXT("{bad spec")));

        ASSERT_TRUE(OCI_SodaCollectionDrop(coll));
        ASSERT_TRUE(OCI_SodaCollectionFree(coll));
        ASSERT_TRUE(OCI_SodaDatabaseFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, DbCreate_PreClient18_Fails)
    {
        /* SODA is an 18.1+ client feature */

        const auto env = GetSpecificEnvironment(nullptr, OCI_12_2);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        ASSERT_EQ(nullptr, OCI_SodaDatabaseCreate(conn));
        ASSERT_NE(nullptr, OCI_GetLastError());

        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }


    class TestSodaNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestSodaNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestSodaNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "SodaCollCreateIndex", [] { return !OCI_SodaCollectionCreateIndex(nullptr, OTEXT("x")); } },
            { "SodaCollCursorFree", [] { return !OCI_SodaCollectionCursorFree(nullptr); } },
            { "SodaCollCursorGetNext", [] { return nullptr == OCI_SodaCollectionCursorGetNext(nullptr); } },
            { "SodaCollDocCount", [] { return 0 == OCI_SodaCollectionDocumentCount(nullptr, nullptr); } },
            { "SodaCollDocCountWithFilter", [] { return 0 == OCI_SodaCollectionDocumentCountWithFilter(nullptr, OTEXT("x")); } },
            { "SodaCollDrop", [] { return !OCI_SodaCollectionDrop(nullptr); } },
            { "SodaCollDropIndex", [] { return !OCI_SodaCollectionDropIndex(nullptr, OTEXT("x")); } },
            { "SodaCollFind", [] { return nullptr == OCI_SodaCollectionFind(nullptr, nullptr); } },
            { "SodaCollFindOne", [] { return nullptr == OCI_SodaCollectionFindOne(nullptr, nullptr); } },
            { "SodaCollFindOneByKey", [] { return nullptr == OCI_SodaCollectionFindOneByKey(nullptr, OTEXT("x")); } },
            { "SodaCollFree", [] { return !OCI_SodaCollectionFree(nullptr); } },
            { "SodaCollFreeIndexNames", [] { return !OCI_SodaCollectionFreeIndexNames(nullptr, nullptr /* otext ** */, 1); } },
            { "SodaCollGetConnection", [] { return nullptr == OCI_SodaCollectionGetConnection(nullptr); } },
            { "SodaCollGetDataGuide", [] { return nullptr == OCI_SodaCollectionGetDataGuide(nullptr); } },
            { "SodaCollGetMetadata", [] { return nullptr == OCI_SodaCollectionGetMetadata(nullptr); } },
            { "SodaCollGetName", [] { return nullptr == OCI_SodaCollectionGetName(nullptr); } },
            { "SodaCollInsertDoc", [] { return !OCI_SodaCollectionInsertDocument(nullptr, nullptr); } },
            { "SodaCollInsertMany", [] { return !OCI_SodaCollectionInsertMany(nullptr, nullptr, 1, FALSE); } },
            { "SodaCollIsMapped", [] { return !OCI_SodaCollectionIsMapped(nullptr); } },
            { "SodaCollListIndexes", [] { return nullptr == OCI_SodaCollectionListIndexes(nullptr, NullArgOut<OCI_UInt32>()); } },
            { "SodaCollRemove", [] { return !OCI_SodaCollectionRemove(nullptr, nullptr, NullArgOut<OCI_UInt64>()); } },
            { "SodaCollRemoveOneByKey", [] { return !OCI_SodaCollectionRemoveOneByKey(nullptr, OTEXT("x")); } },
            { "SodaCollReplaceOne", [] { return !OCI_SodaCollectionReplaceOne(nullptr, nullptr, nullptr, FALSE); } },
            { "SodaCollReplaceOneByKey", [] { return !OCI_SodaCollectionReplaceOneByKey(nullptr, OTEXT("x"), nullptr, FALSE); } },
            { "SodaCollSave", [] { return !OCI_SodaCollectionSave(nullptr, nullptr, FALSE); } },
            { "SodaCollTruncate", [] { return !OCI_SodaCollectionTruncate(nullptr); } },
            { "SodaDbCreate", [] { return nullptr == OCI_SodaDatabaseCreate(nullptr); } },
            { "SodaDbCreateColl", [] { return nullptr == OCI_SodaDatabaseCreateCollection(nullptr, OTEXT("x"), OTEXT("x")); } },
            { "SodaDbFree", [] { return !OCI_SodaDatabaseFree(nullptr); } },
            { "SodaDbFreeCollectionNames", [] { return !OCI_SodaDatabaseFreeCollectionNames(nullptr, nullptr /* otext ** */, 1); } },
            { "SodaDbGetCollectionNames", [] { return nullptr == OCI_SodaDatabaseGetCollectionNames(nullptr, NullArgOut<OCI_UInt32>()); } },
            { "SodaDbGetColls", [] { return nullptr == OCI_SodaDatabaseGetCollections(nullptr, OTEXT("x")); } },
            { "SodaDbGetConnection", [] { return nullptr == OCI_SodaDatabaseGetConnection(nullptr); } },
            { "SodaDbOpenColl", [] { return nullptr == OCI_SodaDatabaseOpenCollection(nullptr, OTEXT("x")); } },
            { "SodaDocCreateFromJson", [] { return nullptr == OCI_SodaDocumentCreateFromJson(nullptr, nullptr, OTEXT("x")); } },
            { "SodaDocCreateFromText", [] { return nullptr == OCI_SodaDocumentCreateFromText(nullptr, OTEXT("x"), 1, OTEXT("x")); } },
            { "SodaDocCursorFree", [] { return !OCI_SodaDocumentCursorFree(nullptr); } },
            { "SodaDocCursorGetNext", [] { return nullptr == OCI_SodaDocumentCursorGetNext(nullptr); } },
            { "SodaDocFree", [] { return !OCI_SodaDocumentFree(nullptr); } },
            { "SodaDocGetCreatedOn", [] { return nullptr == OCI_SodaDocumentGetCreatedOn(nullptr); } },
            { "SodaDocGetIsJson", [] { return !OCI_SodaDocumentGetIsJson(nullptr); } },
            { "SodaDocGetJson", [] { return nullptr == OCI_SodaDocumentGetJson(nullptr); } },
            { "SodaDocGetKey", [] { return nullptr == OCI_SodaDocumentGetKey(nullptr); } },
            { "SodaDocGetLastModified", [] { return nullptr == OCI_SodaDocumentGetLastModified(nullptr); } },
            { "SodaDocGetMediaType", [] { return nullptr == OCI_SodaDocumentGetMediaType(nullptr); } },
            { "SodaDocGetText", [] { return nullptr == OCI_SodaDocumentGetText(nullptr); } },
            { "SodaDocGetVersion", [] { return nullptr == OCI_SodaDocumentGetVersion(nullptr); } },
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