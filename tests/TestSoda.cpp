#include "ocilib_tests.h"
#include "test_internals.h"

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

        const ostring content = OTEXT("{\"name\":\"soda test\"}");

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
        ASSERT_EQ(ostring(docKey), ostring(foundKey));

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

        const ostring content = OTEXT("{\"name\":\"soda json test\"}");

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
        ASSERT_EQ(ostring(OTEXT("OCILIB_SODA_PROPS_COLL")), ostring(name));

        const auto metadata = OCI_SodaCollGetMetadata(coll);
        ASSERT_NE(nullptr, metadata);
        ASSERT_FALSE(ostring(metadata).empty());

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

        const ostring metadata = OTEXT("{\"keyColumn\":{\"name\":\"ID\",\"assignmentMethod\":\"CLIENT\",\"sqlType\":\"VARCHAR2\",\"maxLength\":255}}");

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_META_COLL"));
        const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_META_COLL"), metadata.c_str());
        ASSERT_NE(nullptr, coll);

        const auto actual = OCI_SodaCollGetMetadata(coll);
        ASSERT_NE(nullptr, actual);
        ASSERT_NE(ostring::npos, ostring(actual).find(OTEXT("CLIENT")));

        /* client-assigned keys : the key supplied at creation is used as is */

        const ostring content = OTEXT("{\"value\":1}");

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
        ASSERT_EQ(ostring(OTEXT("OCILIB_SODA_OPEN_COLL")), ostring(name));

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

            if (ostring(name).find(OTEXT("OCILIB_SODA_CUR_COLL")) != ostring::npos)
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

        const ostring content = OTEXT("{\"name\":\"meta test\"}");

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
            ASSERT_NE(ostring::npos, ostring(text).find(OTEXT("from json")));

            ASSERT_TRUE(OCI_SodaDocFree(doc));
            ASSERT_TRUE(OCI_JsonFree(json));
        }

        ASSERT_TRUE(OCI_Commit(conn));
        ASSERT_TRUE(OCI_SodaCollDrop(coll));
        ASSERT_TRUE(OCI_SodaCollFree(coll));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, FindWithOptions)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_OPT_COLL"));
        const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_OPT_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        for (OCI_Int32 i = 1; i <= 5; i++)
        {
            std::basic_ostringstream<otext> oss;
            oss << OTEXT("{\"seq\":") << i << OTEXT(",\"parity\":")
                << ((i % 2) ? OTEXT("\"odd\"") : OTEXT("\"even\"")) << OTEXT("}");
            const ostring content = oss.str();

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

        /* QBE filter + skip + limit */

        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        ASSERT_TRUE(OCI_SodaOptionsSetFilter(options, OTEXT("{\"parity\":\"even\"}")));
        ASSERT_TRUE(OCI_SodaOptionsSetSkip(options, 0));
        ASSERT_TRUE(OCI_SodaOptionsSetLimit(options, 10));

        auto* cursor = OCI_SodaCollFind(coll, options);
        ASSERT_NE(nullptr, cursor);

        OCI_UInt32 count = 0;
        OCI_SodaDoc* item = NULL;
        while (NULL != (item = OCI_SodaDocCursorGetNext(cursor)))
        {
            count++;
            ASSERT_TRUE(OCI_SodaDocFree(item));
        }

        ASSERT_EQ(2u, count);

        ASSERT_TRUE(OCI_SodaDocCursorFree(cursor));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));

        ASSERT_TRUE(OCI_Commit(conn));
        ASSERT_TRUE(OCI_SodaCollDrop(coll));
        ASSERT_TRUE(OCI_SodaCollFree(coll));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestSoda, FindOneWithOptions)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_FINDONE_COLL"));
        const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_FINDONE_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        const ostring content = OTEXT("{\"tag\":\"findone\"}");

        const auto doc = OCI_SodaDocCreateFromText
        (
            sdb,
            content.c_str(),
            static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, doc);
        ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, doc));

        const auto key = OCI_SodaDocGetKey(doc);
        ASSERT_NE(nullptr, key);

        /* FindOne with a key option */

        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);
        ASSERT_TRUE(OCI_SodaOptionsSetKey(options, key));

        auto* foundDoc = OCI_SodaCollFindOne(coll, options);
        ASSERT_NE(nullptr, foundDoc);
        const auto foundKey = OCI_SodaDocGetKey(foundDoc);
        ASSERT_EQ(ostring(key), ostring(foundKey));

        ASSERT_TRUE(OCI_SodaDocFree(foundDoc));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));

        /* FindOne with keys option */

        options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        const otext* keys[] = { key };
        ASSERT_TRUE(OCI_SodaOptionsSetKeys(options, keys, 1));

        foundDoc = OCI_SodaCollFindOne(coll, options);
        ASSERT_NE(nullptr, foundDoc);

        ASSERT_TRUE(OCI_SodaDocFree(foundDoc));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));

        ASSERT_TRUE(OCI_SodaDocFree(doc));

        ASSERT_TRUE(OCI_Commit(conn));
        ASSERT_TRUE(OCI_SodaCollDrop(coll));
        ASSERT_TRUE(OCI_SodaCollFree(coll));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
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
            const ostring content = (i % 2) ? OTEXT("{\"grp\":1}") : OTEXT("{\"grp\":0}");

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

        const ostring content = OTEXT("{\"v\":1}");

        const auto doc = OCI_SodaDocCreateFromText
        (
            sdb,
            content.c_str(),
            static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, doc);
        ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, doc));

        const ostring key = OCI_SodaDocGetKey(doc);
        ASSERT_FALSE(key.empty());

        const ostring newContent = OTEXT("{\"v\":2}");

        const auto newDoc = OCI_SodaDocCreateFromText
        (
            sdb,
            newContent.c_str(),
            static_cast<OCI_UInt32>(newContent.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, newDoc);

        ASSERT_TRUE(OCI_SodaCollReplaceOneByKey(coll, key.c_str(), newDoc));

        const auto foundDoc = OCI_SodaCollFindOneByKey(coll, key.c_str());
        ASSERT_NE(nullptr, foundDoc);

        const auto text = OCI_SodaDocGetText(foundDoc);
        ASSERT_NE(nullptr, text);
        ASSERT_NE(ostring::npos, ostring(text).find(OTEXT("\"v\":2")));

        ASSERT_TRUE(OCI_SodaDocFree(foundDoc));

        /* replace-and-get updates the passed document metadata */

        const ostring newContent2 = OTEXT("{\"v\":3}");
        const auto newDoc2 = OCI_SodaDocCreateFromText
        (
            sdb,
            newContent2.c_str(),
            static_cast<OCI_UInt32>(newContent2.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, newDoc2);

        ASSERT_TRUE(OCI_SodaCollReplaceOneAndGetByKey(coll, key.c_str(), newDoc2));
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

    TEST(TestSoda, ReplaceOneWithOptions)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sdb = OCI_SodaDbCreate(conn);
        ASSERT_NE(nullptr, sdb);

        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_REPLOPT_COLL"));
        const auto coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_REPLOPT_COLL"), NULL);
        ASSERT_NE(nullptr, coll);

        const ostring content = OTEXT("{\"v\":10}");

        const auto doc = OCI_SodaDocCreateFromText
        (
            sdb,
            content.c_str(),
            static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, doc);
        ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, doc));

        const ostring key = OCI_SodaDocGetKey(doc);
        ASSERT_FALSE(key.empty());

        const ostring newContent = OTEXT("{\"v\":20}");
        const auto newDoc = OCI_SodaDocCreateFromText
        (
            sdb,
            newContent.c_str(),
            static_cast<OCI_UInt32>(newContent.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, newDoc);

        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);
        ASSERT_TRUE(OCI_SodaOptionsSetKey(options, key.c_str()));

        ASSERT_TRUE(OCI_SodaCollReplaceOne(coll, options, newDoc));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));

        const auto foundDoc = OCI_SodaCollFindOneByKey(coll, key.c_str());
        ASSERT_NE(nullptr, foundDoc);

        const auto text = OCI_SodaDocGetText(foundDoc);
        ASSERT_NE(nullptr, text);
        ASSERT_NE(ostring::npos, ostring(text).find(OTEXT("\"v\":20")));

        const ostring version = OCI_SodaDocGetVersion(foundDoc);
        ASSERT_TRUE(OCI_SodaDocFree(foundDoc));

        /* replace-and-get with key + version options */

        options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);
        ASSERT_TRUE(OCI_SodaOptionsSetKey(options, key.c_str()));
        ASSERT_TRUE(OCI_SodaOptionsSetVersion(options, version.c_str()));

        const ostring newContent2 = OTEXT("{\"v\":30}");
        const auto newDoc2 = OCI_SodaDocCreateFromText
        (
            sdb,
            newContent2.c_str(),
            static_cast<OCI_UInt32>(newContent2.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, newDoc2);

        ASSERT_TRUE(OCI_SodaCollReplaceOneAndGet(coll, options, newDoc2));
        ASSERT_NE(nullptr, OCI_SodaDocGetVersion(newDoc2));

        ASSERT_TRUE(OCI_SodaOptionsFree(options));
        ASSERT_TRUE(OCI_SodaDocFree(newDoc2));
        ASSERT_TRUE(OCI_SodaDocFree(newDoc));
        ASSERT_TRUE(OCI_SodaDocFree(doc));

        ASSERT_TRUE(OCI_Commit(conn));
        ASSERT_TRUE(OCI_SodaCollDrop(coll));
        ASSERT_TRUE(OCI_SodaCollFree(coll));

        ASSERT_TRUE(OCI_SodaDbFree(sdb));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
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

        const ostring content = OTEXT("{\"r\":1}");

        const auto doc = OCI_SodaDocCreateFromText
        (
            sdb,
            content.c_str(),
            static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, doc);
        ASSERT_TRUE(OCI_SodaCollInsertDoc(coll, doc));

        const ostring key = OCI_SodaDocGetKey(doc);
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
            const ostring content = (i % 2) ? OTEXT("{\"del\":1}") : OTEXT("{\"del\":0}");

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

        const ostring content = OTEXT("{\"t\":1}");

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

    TEST_F(TestSodaErrors, SodaDbNullArgs)
    {
        ASSERT_EQ(nullptr, OCI_SodaDbCreateColl(NULL, OTEXT("X"), NULL));
        ASSERT_EQ(nullptr, OCI_SodaDbOpenColl(NULL, OTEXT("X")));
        ASSERT_EQ(nullptr, OCI_SodaDbOpenColl(sdb, NULL));
        ASSERT_EQ(nullptr, OCI_SodaDbGetColls(NULL, NULL));
        ASSERT_EQ(nullptr, OCI_SodaDbGetConnection(NULL));
    }

    TEST_F(TestSodaErrors, DocCreateNullArgs)
    {
        ASSERT_EQ(nullptr, OCI_SodaDocCreateFromText(NULL, OTEXT("{}"), 2, NULL));
        ASSERT_EQ(nullptr, OCI_SodaDocCreateFromJson(NULL, NULL, NULL));
    }

    TEST_F(TestSodaErrors, DocCreateNullContentIsValid)
    {
        /* NULL content is valid - creates an empty document */
        OCI_SodaDoc* emptyDoc = OCI_SodaDocCreateFromText(sdb, NULL, 0, NULL);
        ASSERT_NE(nullptr, emptyDoc);
        ASSERT_TRUE(OCI_SodaDocFree(emptyDoc));
    }

    TEST_F(TestSodaErrors, CollNullHandle)
    {
        ASSERT_EQ(nullptr, OCI_SodaCollGetName(NULL));
        ASSERT_EQ(nullptr, OCI_SodaCollGetMetadata(NULL));
        ASSERT_EQ(nullptr, OCI_SodaCollGetConnection(NULL));
        ASSERT_FALSE(OCI_SodaCollIsMapped(NULL));
        ASSERT_FALSE(OCI_SodaCollDrop(NULL));
        ASSERT_FALSE(OCI_SodaCollTruncate(NULL));
        ASSERT_FALSE(OCI_SodaCollFree(NULL));
    }

    TEST_F(TestSodaErrors, DocNullHandle)
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

    TEST_F(TestSodaErrors, OptionsNullHandle)
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

    TEST_F(TestSodaErrors, CursorNullHandles)
    {
        ASSERT_EQ(nullptr, OCI_SodaCollCursorGetNext(NULL));
        ASSERT_FALSE(OCI_SodaCollCursorFree(NULL));
        ASSERT_EQ(nullptr, OCI_SodaDocCursorGetNext(NULL));
        ASSERT_FALSE(OCI_SodaDocCursorFree(NULL));
    }

    TEST_F(TestSodaErrors, CollOpsNullArgs)
    {
        ASSERT_FALSE(OCI_SodaCollInsertDoc(NULL, NULL));
        ASSERT_FALSE(OCI_SodaCollInsertDoc(coll, NULL));
        ASSERT_EQ(nullptr, OCI_SodaCollFind(NULL, NULL));
        ASSERT_EQ(nullptr, OCI_SodaCollFindOne(NULL, NULL));
        ASSERT_EQ(nullptr, OCI_SodaCollFindOneByKey(NULL, OTEXT("k")));
        ASSERT_EQ(nullptr, OCI_SodaCollFindOneByKey(coll, NULL));
        ASSERT_EQ(0u, OCI_SodaCollDocCount(NULL, NULL));
        ASSERT_EQ(0u, OCI_SodaCollDocCountWithFilter(NULL, OTEXT("{}")));
        ASSERT_FALSE(OCI_SodaCollReplaceOne(NULL, NULL, NULL));
        ASSERT_FALSE(OCI_SodaCollReplaceOneAndGet(NULL, NULL, NULL));
        ASSERT_FALSE(OCI_SodaCollReplaceOneByKey(NULL, NULL, NULL));
        ASSERT_FALSE(OCI_SodaCollReplaceOneAndGetByKey(NULL, NULL, NULL));
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

                const ostring content = OTEXT("{\"idx\":1}");

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

        static ostring      keystr[DocCount];
        static const otext* keys[DocCount];
    };

    ostring      TestSodaMultiKey::keystr[DocCount];
    const otext* TestSodaMultiKey::keys[DocCount];

    TEST_F(TestSodaMultiKey, FindWithKeyList)
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

    TEST_F(TestSodaMultiKey, FindOneWithMixedKeys)
    {
        /* first keys miss, a later one hits */
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        const otext* mixedKeys[] = { OTEXT("nope1"), keys[2], OTEXT("nope2") };
        ASSERT_TRUE(OCI_SodaOptionsSetKeys(options, mixedKeys, 3));

        auto* found = OCI_SodaCollFindOne(coll, options);
        ASSERT_NE(nullptr, found);
        ASSERT_EQ(ostring(keys[2]), ostring(OCI_SodaDocGetKey(found)));
        ASSERT_TRUE(OCI_SodaDocFree(found));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    TEST_F(TestSodaMultiKey, FindOneWithMissingKeys)
    {
        /* full miss loop + doc cleanup */
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        const otext* missKeys[] = { OTEXT("nope1"), OTEXT("nope2") };
        ASSERT_TRUE(OCI_SodaOptionsSetKeys(options, missKeys, 2));

        ASSERT_EQ(nullptr, OCI_SodaCollFindOne(coll, options));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    TEST_F(TestSodaMultiKey, DocCountWithKeyList)
    {
        /* per-key counts are summed */
        auto* options = OCI_SodaOptionsCreate(sdb);
        ASSERT_NE(nullptr, options);

        const otext* countKeys[] = { keys[0], keys[1], OTEXT("nope") };
        ASSERT_TRUE(OCI_SodaOptionsSetKeys(options, countKeys, 3));
        ASSERT_EQ(2u, OCI_SodaCollDocCount(coll, options));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
    }

    TEST_F(TestSodaMultiKey, ReplaceOneWithKeyList)
    {
        const ostring repl = OTEXT("{\"idx\":2}");
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
        ASSERT_TRUE(OCI_SodaCollReplaceOne(coll, options, replDoc));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
        ASSERT_TRUE(OCI_SodaDocFree(replDoc));
    }

    TEST_F(TestSodaMultiKey, ReplaceOneAndGetWithKeyList)
    {
        const ostring repl = OTEXT("{\"idx\":2}");
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
        ASSERT_TRUE(OCI_SodaCollReplaceOneAndGet(coll, options, replDoc));
        ASSERT_TRUE(OCI_SodaOptionsFree(options));
        ASSERT_TRUE(OCI_SodaDocFree(replDoc));
    }

    TEST_F(TestSodaMultiKey, RemoveWithKeyList)
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

            const ostring content = OTEXT("{\"name\":\"nopts\"}");

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

    TEST_F(TestSodaNullOptions, FindWithNullOptions)
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

    TEST_F(TestSodaNullOptions, FindOneWithNullOptions)
    {
        auto* found = OCI_SodaCollFindOne(coll, NULL);
        ASSERT_NE(nullptr, found);
        ASSERT_TRUE(OCI_SodaDocFree(found));
    }

    TEST_F(TestSodaNullOptions, DocCountWithNullOptions)
    {
        ASSERT_EQ(1u, OCI_SodaCollDocCount(coll, NULL));
    }

    TEST_F(TestSodaNullOptions, FindOneByKeyMiss)
    {
        /* NO_DATA cleanup arm */
        ASSERT_EQ(nullptr, OCI_SodaCollFindOneByKey(coll, OTEXT("no_such_key")));
    }

    TEST_F(TestSodaNullOptions, ReplaceOneWithNullOptions)
    {
        /* NULL options exercises the temporary-options arm; an empty
           filter does not replace the doc, so only the call matters */
        const ostring repl = OTEXT("{\"name\":\"nopts2\"}");

        auto* replDoc = OCI_SodaDocCreateFromText
        (
            sdb, repl.c_str(),
            static_cast<OCI_UInt32>(repl.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, replDoc);

        (void) OCI_SodaCollReplaceOne(coll, NULL, replDoc);
        ASSERT_TRUE(OCI_SodaDocFree(replDoc));
    }

    TEST_F(TestSodaNullOptions, ReplaceOneAndGetWithNullOptions)
    {
        const ostring repl = OTEXT("{\"name\":\"nopts2\"}");

        auto* replDoc = OCI_SodaDocCreateFromText
        (
            sdb, repl.c_str(),
            static_cast<OCI_UInt32>(repl.size() * sizeof(otext)),
            NULL
        );
        ASSERT_NE(nullptr, replDoc);

        (void) OCI_SodaCollReplaceOneAndGet(coll, NULL, replDoc);
        ASSERT_TRUE(OCI_SodaDocFree(replDoc));
    }

    TEST_F(TestSodaNullOptions, RemoveWithNullOptions)
    {
        OCI_UInt64 removed = 0;
        (void) OCI_SodaCollRemove(coll, NULL, &removed);
    }

    /* --- armed OCI fault injection --- */

    class TestSodaFaults : public ::testing::Test
    {
    protected:

        void SetUp() override
        {
            env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            sdb = OCI_SodaDbCreate(conn);
            ASSERT_NE(nullptr, sdb);

            DropCollIfExists(sdb, OTEXT("OCILIB_SODA_ERR_COLL"));
            coll = OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_ERR_COLL"), NULL);
            ASSERT_NE(nullptr, coll);

            const ostring content = OTEXT("{\"name\":\"err\"}");
            doc = OCI_SodaDocCreateFromText
            (
                sdb,
                content.c_str(),
                static_cast<OCI_UInt32>(content.size() * sizeof(otext)),
                NULL
            );
            ASSERT_NE(nullptr, doc);
        }

        void TearDown() override
        {
            OCI_TestApiRestore(env);
            OCI_SodaDocFree(doc);
            OCI_Commit(conn);
            OCI_SodaCollDrop(coll);
            OCI_SodaCollFree(coll);
            OCI_SodaDbFree(sdb);
            OCI_ConnectionFree(conn);
        }

        OCI_Environment* env{ nullptr };
        OCI_Connection*  conn{ nullptr };
        OCI_SodaDb*      sdb{ nullptr };
        OCI_SodaColl*    coll{ nullptr };
        OCI_SodaDoc*     doc{ nullptr };
    };

    TEST_F(TestSodaFaults, CreateCollAttrGet1)
    {
        /* collection property reads use OCIAttrGet: armed fault covers
           the error arm of the attribute read at that position */
        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_ERR_COLL2"));
        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_GET, 1);
        ASSERT_EQ(nullptr, OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_ERR_COLL2"), NULL));
        OCI_TestApiRestore(env);
        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_ERR_COLL2"));
    }

    TEST_F(TestSodaFaults, CreateCollAttrGet2)
    {
        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_ERR_COLL2"));
        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_GET, 2);
        ASSERT_EQ(nullptr, OCI_SodaDbCreateColl(sdb, OTEXT("OCILIB_SODA_ERR_COLL2"), NULL));
        OCI_TestApiRestore(env);
        DropCollIfExists(sdb, OTEXT("OCILIB_SODA_ERR_COLL2"));
    }

    TEST_F(TestSodaFaults, GetNameAttrGet)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_GET, 1);
        ASSERT_EQ(nullptr, OCI_SodaCollGetName(coll));
    }

    TEST_F(TestSodaFaults, InsertDocFault)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_SODA_INSERT_AND_GET, 1);
        ASSERT_FALSE(OCI_SodaCollInsertDoc(coll, doc));
    }

    TEST_F(TestSodaFaults, FindFault)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_SODA_FIND, 1);
        ASSERT_EQ(nullptr, OCI_SodaCollFind(coll, NULL));
    }

    TEST_F(TestSodaFaults, FindOneFault)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_SODA_FIND_ONE, 1);
        ASSERT_EQ(nullptr, OCI_SodaCollFindOne(coll, NULL));
    }

    TEST_F(TestSodaFaults, DocCountFault)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_SODA_DOC_COUNT, 1);
        ASSERT_EQ(0u, OCI_SodaCollDocCount(coll, NULL));
    }

    TEST_F(TestSodaFaults, ReplaceOneFault)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_SODA_REPL_ONE, 1);
        ASSERT_FALSE(OCI_SodaCollReplaceOne(coll, NULL, doc));
    }

    TEST_F(TestSodaFaults, RemoveFault)
    {
        OCI_UInt64 removed = 0;
        OCI_TestFailApiCall(env, OCI_TEST_API_SODA_REMOVE, 1);
        ASSERT_FALSE(OCI_SodaCollRemove(coll, NULL, &removed));
    }

    TEST_F(TestSodaFaults, TruncateFault)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_SODA_TRUNCATE, 1);
        ASSERT_FALSE(OCI_SodaCollTruncate(coll));
    }

    TEST_F(TestSodaFaults, DropFault)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_SODA_DROP, 1);
        ASSERT_FALSE(OCI_SodaCollDrop(coll));
    }
}
