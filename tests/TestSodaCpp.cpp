#include "ocilib_tests.h"
#include "TestFeatures.h"

using namespace ocilib;

namespace
{
    void DropCollIfExistsCpp(SodaDatabase& sdb, const otext* name)
    {
        SodaCollection coll = sdb.OpenCollection(name);
        if (!coll.IsNull())
        {
            coll.Drop();
        }
    }
}

namespace TestCppApi
{
    TEST(TestSodaCpp, SodaDatabaseConstruction_ReturnsValidDb)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        ASSERT_FALSE(sdb.IsNull());
    }

    TEST(TestSodaCpp, GetConnectionOnSodaDb_ReturnsOwningConnection)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        ASSERT_FALSE(sdb.GetConnection().IsNull());
    }

    TEST(TestSodaCpp, CollectionCreateGetPropsAndDrop_CompletesLifecycle)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_COLL"));

        SodaCollection coll = sdb.CreateCollection(OTEXT("OCILIB_SODA_CPP_COLL"), String());
        ASSERT_FALSE(coll.IsNull());

        ASSERT_EQ(String(OTEXT("OCILIB_SODA_CPP_COLL")), coll.GetName());
        ASSERT_FALSE(coll.GetMetadata().empty());
        ASSERT_FALSE(coll.GetConnection().IsNull());
        (void) coll.IsMapped();

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, OpenCollectionByName_ReturnsExistingCollection)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_OPEN"));

        SodaCollection coll = sdb.CreateCollection(OTEXT("OCILIB_SODA_CPP_OPEN"), String());
        ASSERT_FALSE(coll.IsNull());

        SodaCollection opened = sdb.OpenCollection(OTEXT("OCILIB_SODA_CPP_OPEN"));
        ASSERT_FALSE(opened.IsNull());
        ASSERT_EQ(String(OTEXT("OCILIB_SODA_CPP_OPEN")), opened.GetName());

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, GetCollectionsCursorWithGetNext_ListsMatchingColls)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_OPEN"));

        SodaCollection coll = sdb.CreateCollection(OTEXT("OCILIB_SODA_CPP_OPEN"), String());
        ASSERT_FALSE(coll.IsNull());

        SodaCollectionCursor cursor = sdb.GetCollections(OTEXT("OCILIB_SODA_CPP"));

        bool found = false;
        for (SodaCollection item = cursor.GetNext(); !item.IsNull(); item = cursor.GetNext())
        {
            if (item.GetName() == String(OTEXT("OCILIB_SODA_CPP_OPEN")))
            {
                found = true;
            }
        }

        ASSERT_TRUE(found);

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, GetCollectionsCursorWithRangeFor_ListsMatchingColls)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_OPEN"));

        SodaCollection coll = sdb.CreateCollection(OTEXT("OCILIB_SODA_CPP_OPEN"), String());
        ASSERT_FALSE(coll.IsNull());

        /* range-for over a fresh cursor */
        SodaCollectionCursor cursor2 = sdb.GetCollections(OTEXT("OCILIB_SODA_CPP"));
        bool found2 = false;
        for (SodaCollection& item : cursor2)
        {
            if (item.GetName() == String(OTEXT("OCILIB_SODA_CPP_OPEN")))
            {
                found2 = true;
            }
        }
        ASSERT_TRUE(found2);

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, GetCollectionNames_IncludesCreatedColl)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_OPEN"));

        SodaCollection coll = sdb.CreateCollection(OTEXT("OCILIB_SODA_CPP_OPEN"), String());
        ASSERT_FALSE(coll.IsNull());

        /* collection name list */
        std::vector<String> names = sdb.GetCollectionNames();
        bool foundName = false;
        for (const String& n : names)
        {
            if (n == String(OTEXT("OCILIB_SODA_CPP_OPEN")))
            {
                foundName = true;
            }
        }
        ASSERT_TRUE(foundName);

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, InsertDocument_PopulatesGeneratedMetadata)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        /* the collection content column uses the JSON SQL type (21.1+)
           — 19c servers reject it with ORA-40636 */

        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_JSON);

        SodaDatabase sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_DOC"));

        SodaCollection coll = sdb.CreateCollection
        (
            OTEXT("OCILIB_SODA_CPP_DOC"),
            OTEXT("{\"keyColumn\":{\"name\":\"ID\"},"
                  "\"contentColumn\":{\"name\":\"JSON_DOCUMENT\",\"sqlType\":\"JSON\"},"
                  "\"versionColumn\":{\"name\":\"VERSION\",\"method\":\"UUID\"},"
                  "\"lastModifiedColumn\":{\"name\":\"LAST_MODIFIED\"},"
                  "\"creationTimeColumn\":{\"name\":\"CREATED_ON\"}}")
        );
        ASSERT_FALSE(coll.IsNull());

        SodaDocument doc = sdb.CreateDocumentFromText(OTEXT("{\"name\":\"cpp test\"}"), String());
        ASSERT_FALSE(doc.IsNull());
        ASSERT_FALSE(doc.GetMediaType().empty());

        coll.Insert(doc);

        ASSERT_FALSE(doc.GetKey().empty());
        ASSERT_FALSE(doc.GetVersion().empty());
        ASSERT_FALSE(doc.GetCreatedOn().empty());
        ASSERT_FALSE(doc.GetLastModified().empty());
        ASSERT_FALSE(doc.GetText().empty());

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, FindOneByKeyOnInsertedDoc_ReturnsDocWithJson)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        /* the collection content column uses the JSON SQL type (21.1+)
           — 19c servers reject it with ORA-40636 */

        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_JSON);

        SodaDatabase sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_DOC"));

        SodaCollection coll = sdb.CreateCollection
        (
            OTEXT("OCILIB_SODA_CPP_DOC"),
            OTEXT("{\"keyColumn\":{\"name\":\"ID\"},"
                  "\"contentColumn\":{\"name\":\"JSON_DOCUMENT\",\"sqlType\":\"JSON\"},"
                  "\"versionColumn\":{\"name\":\"VERSION\",\"method\":\"UUID\"},"
                  "\"lastModifiedColumn\":{\"name\":\"LAST_MODIFIED\"},"
                  "\"creationTimeColumn\":{\"name\":\"CREATED_ON\"}}")
        );
        ASSERT_FALSE(coll.IsNull());

        SodaDocument doc = sdb.CreateDocumentFromText(OTEXT("{\"name\":\"cpp test\"}"), String());
        ASSERT_FALSE(doc.IsNull());

        coll.Insert(doc);

        SodaDocument found = coll.FindOne(doc.GetKey());
        ASSERT_FALSE(found.IsNull());
        ASSERT_EQ(doc.GetKey(), found.GetKey());

        if (OCI_ConnectionGetVersion(con) >= OCI_23_4 && OCI_EnvironmentGetOCIRuntimeVersion(env.GetHandle()) >= OCI_23_4)
        {
            Json json = found.GetJson();
            ASSERT_FALSE(json.IsNull());
        }

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    static SodaCollection CreateCollWithFourDocs(SodaDatabase& sdb, const otext* name)
    {
        DropCollIfExistsCpp(sdb, name);
        SodaCollection coll = sdb.CreateCollection(name, String());
        for (OCI_Int32 i = 0; i < 4; i++)
        {
            SodaDocument doc = sdb.CreateDocumentFromText
            (
                (i % 2) ? OTEXT("{\"grp\":1}") : OTEXT("{\"grp\":0}"),
                String()
            );
            coll.Insert(doc);
        }
        return coll;
    }

    TEST(TestSodaCpp, GetCountWithFilter_ReturnsMatchingCount)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        SodaCollection coll = CreateCollWithFourDocs(sdb, OTEXT("OCILIB_SODA_CPP_FIND"));
        ASSERT_FALSE(coll.IsNull());

        SodaOptions options(sdb);
        options.SetFilter(OTEXT("{\"grp\":1}"));

        ASSERT_EQ(2u, coll.GetCount(options));
        ASSERT_EQ(2u, coll.GetCount(OTEXT("{\"grp\":1}")));
        ASSERT_EQ(4u, coll.GetCount(SodaOptions()));

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, FindWithFilterCursor_ReturnsMatchingDocs)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        SodaCollection coll = CreateCollWithFourDocs(sdb, OTEXT("OCILIB_SODA_CPP_FIND"));
        ASSERT_FALSE(coll.IsNull());

        /* skip/limit apply to find operations, not to count */

        SodaOptions options(sdb);
        options.SetFilter(OTEXT("{\"grp\":1}"));
        options.SetSkip(0);
        options.SetLimit(10);

        SodaDocumentCursor cursor = coll.Find(options);

        OCI_UInt32 count = 0;
        for (SodaDocument doc = cursor.GetNext(); !doc.IsNull(); doc = cursor.GetNext())
        {
            ASSERT_FALSE(doc.GetText().empty());
            count++;
        }

        ASSERT_EQ(2u, count);

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, FindOneWithFilter_ReturnsDoc)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        SodaCollection coll = CreateCollWithFourDocs(sdb, OTEXT("OCILIB_SODA_CPP_FIND"));
        ASSERT_FALSE(coll.IsNull());

        SodaOptions options(sdb);
        options.SetFilter(OTEXT("{\"grp\":1}"));

        /* FindOne with options */

        SodaDocument first = coll.FindOne(options);
        ASSERT_FALSE(first.IsNull());

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    /* seeds a one-doc collection and returns its document key */

    static String CreateCollWithOneDoc(SodaDatabase& sdb, SodaCollection& coll, const otext* name)
    {
        DropCollIfExistsCpp(sdb, name);
        coll = sdb.CreateCollection(name, String());

        SodaDocument doc = sdb.CreateDocumentFromText(OTEXT("{\"v\":1}"), String());
        coll.Insert(doc);

        return doc.GetKey();
    }

    TEST(TestSodaCpp, ReplaceOneByKeyThenFind_ReturnsUpdatedContent)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        SodaCollection coll;
        const String key = CreateCollWithOneDoc(sdb, coll, OTEXT("OCILIB_SODA_CPP_REPL"));
        ASSERT_FALSE(coll.IsNull());
        ASSERT_FALSE(key.empty());

        /* replace by key */

        SodaDocument newDoc = sdb.CreateDocumentFromText(OTEXT("{\"v\":2}"), String());
        coll.ReplaceOneByKey(key, newDoc);

        SodaDocument found = coll.FindOne(key);
        ASSERT_NE(String::npos, found.GetText().find(OTEXT("\"v\":2")));

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, ReplaceOneByKeyAndGet_RefreshesDocKey)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        SodaCollection coll;
        const String key = CreateCollWithOneDoc(sdb, coll, OTEXT("OCILIB_SODA_CPP_REPL"));
        ASSERT_FALSE(coll.IsNull());
        ASSERT_FALSE(key.empty());

        /* replace-and-get by key */

        SodaDocument newDoc2 = sdb.CreateDocumentFromText(OTEXT("{\"v\":3}"), String());
        coll.ReplaceOneByKey(key, newDoc2, true);
        ASSERT_FALSE(newDoc2.GetKey().empty());

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, ReplaceOneWithKeyOption_ReplacesDoc)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        SodaCollection coll;
        const String key = CreateCollWithOneDoc(sdb, coll, OTEXT("OCILIB_SODA_CPP_REPL"));
        ASSERT_FALSE(coll.IsNull());
        ASSERT_FALSE(key.empty());

        /* replace with key options */

        SodaOptions options(sdb);
        options.SetKey(key);

        SodaDocument newDoc3 = sdb.CreateDocumentFromText(OTEXT("{\"v\":4}"), String());
        coll.ReplaceOne(options, newDoc3);

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, ReplaceOneWithKeyAndVersionOption_RefreshesDocKey)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        SodaCollection coll;
        const String key = CreateCollWithOneDoc(sdb, coll, OTEXT("OCILIB_SODA_CPP_REPL"));
        ASSERT_FALSE(coll.IsNull());
        ASSERT_FALSE(key.empty());

        /* replace-and-get with key + version options */

        SodaOptions options(sdb);
        options.SetKey(key);
        options.SetVersion(coll.FindOne(key).GetVersion());

        SodaDocument newDoc4 = sdb.CreateDocumentFromText(OTEXT("{\"v\":5}"), String());
        coll.ReplaceOne(options, newDoc4, true);
        ASSERT_FALSE(newDoc4.GetKey().empty());

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, FindOneWithKeysOption_ReturnsMatchingDoc)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        SodaCollection coll;
        const String key = CreateCollWithOneDoc(sdb, coll, OTEXT("OCILIB_SODA_CPP_REPL"));
        ASSERT_FALSE(coll.IsNull());
        ASSERT_FALSE(key.empty());

        /* keys option */

        SodaOptions keysOptions(sdb);
        keysOptions.SetKeys(std::vector<String> { key });

        SodaDocument foundByKeys = coll.FindOne(keysOptions);
        ASSERT_FALSE(foundByKeys.IsNull());

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, RemoveOneByKey_DeletesDoc)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        SodaCollection coll;
        const String key = CreateCollWithOneDoc(sdb, coll, OTEXT("OCILIB_SODA_CPP_REPL"));
        ASSERT_FALSE(coll.IsNull());
        ASSERT_FALSE(key.empty());

        /* remove by key */

        ASSERT_TRUE(coll.RemoveOne(key));
        ASSERT_EQ(0u, coll.GetCount(SodaOptions()));

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    static SodaCollection CreateCollWithFourDelDocs(SodaDatabase& sdb, const otext* name)
    {
        DropCollIfExistsCpp(sdb, name);
        SodaCollection coll = sdb.CreateCollection(name, String());
        for (OCI_Int32 i = 0; i < 4; i++)
        {
            SodaDocument doc = sdb.CreateDocumentFromText
            (
                (i % 2) ? OTEXT("{\"del\":1}") : OTEXT("{\"del\":0}"),
                String()
            );
            coll.Insert(doc);
        }
        return coll;
    }

    TEST(TestSodaCpp, RemoveWithFilter_DeletesMatchingDocs)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        SodaCollection coll = CreateCollWithFourDelDocs(sdb, OTEXT("OCILIB_SODA_CPP_REM"));
        ASSERT_FALSE(coll.IsNull());

        SodaOptions options(sdb);
        options.SetFilter(OTEXT("{\"del\":0}"));

        OCI_UInt64 removed = 0;
        ASSERT_TRUE(coll.Remove(options, &removed));
        ASSERT_EQ(2u, removed);

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, Truncate_RemovesAllDocs)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        /* OcilibSodaCollectionTruncate gates at min(client,server) >= 20.1
           (CHECK_FEATURE(..., OCI_FEATURE_SODA, OCI_20_1)) — Oracle's SODA
           matrix documents client >= 19.11, but OCILIB's floor applies */

        SKIP_UNLESS(GetTestCaps().client_ver >= OCI_20_1 && GetTestCaps().server_ver >= OCI_20_1,
                    "OCI_SodaCollectionTruncate requires client and server >= 20.1");

        SodaDatabase sdb(con);

        SodaCollection coll = CreateCollWithFourDelDocs(sdb, OTEXT("OCILIB_SODA_CPP_REM"));
        ASSERT_FALSE(coll.IsNull());

        ASSERT_TRUE(coll.Truncate());
        ASSERT_EQ(0u, coll.GetCount(SodaOptions()));

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, CreateDocumentFromJsonThenInsert_StoresJsonContent)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_JSON"));

        SodaCollection coll = sdb.CreateCollection(OTEXT("OCILIB_SODA_CPP_JSON"), String());
        ASSERT_FALSE(coll.IsNull());

        if (OCI_ConnectionGetVersion(con) >= OCI_23_4 && OCI_EnvironmentGetOCIRuntimeVersion(env.GetHandle()) >= OCI_23_4)
        {
            Json json(con);
            json.LoadText(OTEXT("{\"name\":\"cpp json\"}"));

            SodaDocument doc = sdb.CreateDocumentFromJson(json, String());
            ASSERT_FALSE(doc.IsNull());

            coll.Insert(doc);
            ASSERT_NE(String::npos, doc.GetText().find(OTEXT("cpp json")));
        }

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, MethodsOnDefaultConstructedWrappers_Throw)
    {
        ASSERT_THROW((void)(SodaCollection().GetName()), Exception);
        ASSERT_THROW((void)(SodaCollection().Drop()), Exception);
        ASSERT_THROW((void)(SodaDatabase().GetConnection()), Exception);
        ASSERT_THROW((void)(SodaDocument().GetKey()), Exception);
        ASSERT_THROW((void)(SodaDocument().GetText()), Exception);
        ASSERT_THROW((void)(SodaOptions().SetSkip(0)), Exception);
        ASSERT_THROW((void)(SodaCollectionCursor().GetNext()), Exception);
        ASSERT_THROW((void)(SodaDocumentCursor().GetNext()), Exception);
    }

    static SodaCollection CreateClientKeyColl(SodaDatabase& sdb, const otext* name)
    {
        DropCollIfExistsCpp(sdb, name);
        return sdb.CreateCollection
        (
            name,
            OTEXT("{\"keyColumn\":{\"name\":\"ID\",\"sqlType\":\"VARCHAR2\",\"maxLength\":255,\"assignmentMethod\":\"CLIENT\"}}")
        );
    }

    TEST(TestSodaCpp, InsertManyWithDocs_InsertsJsonDocs)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        SodaCollection coll = CreateClientKeyColl(sdb, OTEXT("OCILIB_SODA_CPP_BULK"));
        ASSERT_FALSE(coll.IsNull());

        /* bulk insert */
        std::vector<SodaDocument> docs;
        docs.push_back(sdb.CreateDocumentFromText(OTEXT("{\"a\":1}"), OTEXT("k1")));
        docs.push_back(sdb.CreateDocumentFromText(OTEXT("{\"a\":2}"), OTEXT("k2")));
        try { coll.InsertMany(docs); } catch (const Exception&) { FAIL() << "InsertMany"; }

        try { ASSERT_TRUE(docs[0].IsJson()); } catch (const Exception&) { FAIL() << "IsJson"; }

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, InsertManyAndGet_PopulatesDocKey)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        SodaCollection coll = CreateClientKeyColl(sdb, OTEXT("OCILIB_SODA_CPP_BULK"));
        ASSERT_FALSE(coll.IsNull());

        /* bulk insert and get metadata */
        std::vector<SodaDocument> docs2;
        docs2.push_back(sdb.CreateDocumentFromText(OTEXT("{\"a\":3}"), OTEXT("k3")));
        try { coll.InsertMany(docs2, true); } catch (const Exception&) { FAIL() << "InsertManyAndGet"; }
        ASSERT_EQ(String(OTEXT("k3")), docs2[0].GetKey());

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, SaveOnExistingKey_UpsertsDoc)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        SodaCollection coll = CreateClientKeyColl(sdb, OTEXT("OCILIB_SODA_CPP_BULK"));
        ASSERT_FALSE(coll.IsNull());

        /* save (upsert) */
        SodaDocument sv = sdb.CreateDocumentFromText(OTEXT("{\"a\":99}"), OTEXT("k1"));
        try { coll.Save(sv); } catch (const Exception&) { FAIL() << "Save"; }
        ASSERT_EQ(String(OTEXT("k1")), sv.GetKey());

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, SaveAndGet_RefreshesDocKey)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        SodaCollection coll = CreateClientKeyColl(sdb, OTEXT("OCILIB_SODA_CPP_BULK"));
        ASSERT_FALSE(coll.IsNull());

        /* save-and-get */
        SodaDocument sv = sdb.CreateDocumentFromText(OTEXT("{\"a\":99}"), OTEXT("k1"));
        try { coll.Save(sv, true); } catch (const Exception&) { FAIL() << "SaveAndGet"; }
        ASSERT_EQ(String(OTEXT("k1")), sv.GetKey());

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, OptionSettersOnSodaOptions_Succeed)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);

        SodaDatabase sdb(con);

        /* option setters */
        SodaOptions options(sdb);
        try { options.SetFetchArraySize(16); } catch (const Exception&) { FAIL() << "fetchsize"; }
        try { options.SetLock(); }             catch (const Exception&) { FAIL() << "lock"; }
        /* SODA hints reject enclosing comment markers (ORA-40856) */
        try { options.SetHint(OTEXT("index(OCILIB_SODA_CPP_BULK)")); } catch (const Exception&) {}

        /* ListIndexes / GetDataGuide are covered by the C index tests and
           the MBCS-guarded index tests; index ops aren't callable here:
           CreateIndex errors (ORA-40671) and IndexDrop SEGVs inside the
           Oracle client in wide builds */
    }

#ifdef OCI_CHARSET_MBCS

    /* The OCISodaIndex* calls work in MBCS builds; in wide builds they fail
       inside the Oracle client (ORA-40671 / OCI-22303, IndexDrop even SEGVs) —
       the real assertions live here */
    TEST(TestSodaCpp, CreateListAndDropIndex_ManagesIndexes)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SODA);
        /* JSON search index needs the Oracle Text indextype (missing on
           -slim docker images → ORA-29833) */
        SKIP_UNLESS(GetTestCaps().ctxsys, "CTXSYS.CONTEXT indextype missing");

        SodaDatabase sdb(con);
        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_IDX"));

        SodaCollection coll = sdb.CreateCollection(OTEXT("OCILIB_SODA_CPP_IDX"), String());
        ASSERT_FALSE(coll.IsNull());

        SodaDocument sd = sdb.CreateDocumentFromText(OTEXT("{\"a\":1}"), String());
        coll.Insert(sd);
        con.Commit();

        coll.CreateIndex(OTEXT("{\"name\":\"OCILIB_SODA_CPP_IDX_NAME\"}"));

        ASSERT_GE(coll.ListIndexes().size(), 1u);

        ASSERT_TRUE(coll.DropIndex(OTEXT("OCILIB_SODA_CPP_IDX_NAME")));
        ASSERT_TRUE(coll.Drop());
    }

#endif

    /* SODA requires client and server >= 18.1 — verify the exception
       contract on a real unsupported environment */

    TEST(TestSodaCpp, DbCreate_NotAvailable)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        SKIP_IF_FEATURE_AVAILABLE(env, &con, OCI_FEATURE_SODA);

        try
        {
            SodaDatabase sdb(con);
            FAIL() << "expected ocilib::Exception";
        }
        catch (const Exception& ex)
        {
            EXPECT_EQ(Exception::OcilibError, ex.GetCategory());
            EXPECT_EQ(static_cast<UInt32>(OCI_ERR_NOT_AVAILABLE), ex.GetCode());
        }
    }

}
