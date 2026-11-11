#include "ocilib_tests.h"

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
    TEST(TestSodaCpp, CreateDb)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        SodaDatabase sdb(con);

        ASSERT_FALSE(sdb.IsNull());
    }

    TEST(TestSodaCpp, DbGetConnection)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        SodaDatabase sdb(con);

        ASSERT_FALSE(sdb.GetConnection().IsNull());
    }

    TEST(TestSodaCpp, CollectionLifecycle)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

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

    TEST(TestSodaCpp, OpenCollectionAndCursor)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        SodaDatabase sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_OPEN"));

        SodaCollection coll = sdb.CreateCollection(OTEXT("OCILIB_SODA_CPP_OPEN"), String());
        ASSERT_FALSE(coll.IsNull());

        SodaCollection opened = sdb.OpenCollection(OTEXT("OCILIB_SODA_CPP_OPEN"));
        ASSERT_FALSE(opened.IsNull());
        ASSERT_EQ(String(OTEXT("OCILIB_SODA_CPP_OPEN")), opened.GetName());

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

    TEST(TestSodaCpp, InsertFindAndDocMetadata)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

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

    TEST(TestSodaCpp, FindWithOptionsAndCursor)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        SodaDatabase sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_FIND"));

        SodaCollection coll = sdb.CreateCollection(OTEXT("OCILIB_SODA_CPP_FIND"), String());
        ASSERT_FALSE(coll.IsNull());

        for (OCI_Int32 i = 0; i < 4; i++)
        {
            SodaDocument doc = sdb.CreateDocumentFromText
            (
                (i % 2) ? OTEXT("{\"grp\":1}") : OTEXT("{\"grp\":0}"),
                String()
            );
            coll.Insert(doc);
        }

        SodaOptions options(sdb);
        options.SetFilter(OTEXT("{\"grp\":1}"));

        ASSERT_EQ(2u, coll.GetCount(options));
        ASSERT_EQ(2u, coll.GetCount(OTEXT("{\"grp\":1}")));
        ASSERT_EQ(4u, coll.GetCount(SodaOptions()));

        /* skip/limit apply to find operations, not to count */

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

        /* FindOne with options */

        SodaDocument first = coll.FindOne(options);
        ASSERT_FALSE(first.IsNull());

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, ReplaceAndRemove)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        SodaDatabase sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_REPL"));

        SodaCollection coll = sdb.CreateCollection(OTEXT("OCILIB_SODA_CPP_REPL"), String());
        ASSERT_FALSE(coll.IsNull());

        SodaDocument doc = sdb.CreateDocumentFromText(OTEXT("{\"v\":1}"), String());
        coll.Insert(doc);

        const String key = doc.GetKey();
        ASSERT_FALSE(key.empty());

        /* replace by key */

        SodaDocument newDoc = sdb.CreateDocumentFromText(OTEXT("{\"v\":2}"), String());
        coll.ReplaceOneByKey(key, newDoc);

        SodaDocument found = coll.FindOne(key);
        ASSERT_NE(String::npos, found.GetText().find(OTEXT("\"v\":2")));

        /* replace-and-get by key */

        SodaDocument newDoc2 = sdb.CreateDocumentFromText(OTEXT("{\"v\":3}"), String());
        coll.ReplaceOneByKey(key, newDoc2, true);
        ASSERT_FALSE(newDoc2.GetKey().empty());

        /* replace with key options */

        SodaOptions options(sdb);
        options.SetKey(key);

        SodaDocument newDoc3 = sdb.CreateDocumentFromText(OTEXT("{\"v\":4}"), String());
        coll.ReplaceOne(options, newDoc3);

        /* replace-and-get with key + version options — fetch the current
           version (newDoc3's replace already bumped it past newDoc2's) */

        options.SetVersion(coll.FindOne(key).GetVersion());

        SodaDocument newDoc4 = sdb.CreateDocumentFromText(OTEXT("{\"v\":5}"), String());
        coll.ReplaceOne(options, newDoc4, true);
        ASSERT_FALSE(newDoc4.GetKey().empty());

        /* keys option */

        SodaOptions keysOptions(sdb);
        keysOptions.SetKeys(std::vector<String> { key });

        SodaDocument foundByKeys = coll.FindOne(keysOptions);
        ASSERT_FALSE(foundByKeys.IsNull());

        /* remove by key */

        ASSERT_TRUE(coll.RemoveOne(key));
        ASSERT_EQ(0u, coll.GetCount(SodaOptions()));

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, RemoveWithOptionsAndTruncate)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        SodaDatabase sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_REM"));

        SodaCollection coll = sdb.CreateCollection(OTEXT("OCILIB_SODA_CPP_REM"), String());
        ASSERT_FALSE(coll.IsNull());

        for (OCI_Int32 i = 0; i < 4; i++)
        {
            SodaDocument doc = sdb.CreateDocumentFromText
            (
                (i % 2) ? OTEXT("{\"del\":1}") : OTEXT("{\"del\":0}"),
                String()
            );
            coll.Insert(doc);
        }

        SodaOptions options(sdb);
        options.SetFilter(OTEXT("{\"del\":0}"));

        OCI_UInt64 removed = 0;
        ASSERT_TRUE(coll.Remove(options, &removed));
        ASSERT_EQ(2u, removed);

        ASSERT_TRUE(coll.Truncate());
        ASSERT_EQ(0u, coll.GetCount(SodaOptions()));

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, CreateDocumentFromJson)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

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

    TEST(TestSodaCpp, ErrorPaths)
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

    TEST(TestSodaCpp, BulkSaveIndexAndMeta)
        {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        SodaDatabase sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_BULK"));

        SodaCollection coll = sdb.CreateCollection
        (
            OTEXT("OCILIB_SODA_CPP_BULK"),
            OTEXT("{\"keyColumn\":{\"name\":\"ID\",\"sqlType\":\"VARCHAR2\",\"maxLength\":255,\"assignmentMethod\":\"CLIENT\"}}")
        );
        ASSERT_FALSE(coll.IsNull());

        /* bulk insert */
        std::vector<SodaDocument> docs;
        docs.push_back(sdb.CreateDocumentFromText(OTEXT("{\"a\":1}"), OTEXT("k1")));
        docs.push_back(sdb.CreateDocumentFromText(OTEXT("{\"a\":2}"), OTEXT("k2")));
        try { coll.InsertMany(docs); } catch (const Exception&) { FAIL() << "InsertMany"; }

        try { ASSERT_TRUE(docs[0].IsJson()); } catch (const Exception&) { FAIL() << "IsJson"; }

        /* bulk insert and get metadata */
        std::vector<SodaDocument> docs2;
        docs2.push_back(sdb.CreateDocumentFromText(OTEXT("{\"a\":3}"), OTEXT("k3")));
        try { coll.InsertMany(docs2, true); } catch (const Exception&) { FAIL() << "InsertManyAndGet"; }
        ASSERT_EQ(String(OTEXT("k3")), docs2[0].GetKey());

        /* save (upsert) and save-and-get */
        SodaDocument sv = sdb.CreateDocumentFromText(OTEXT("{\"a\":99}"), OTEXT("k1"));
        try { coll.Save(sv); } catch (const Exception&) { FAIL() << "Save"; }
        try { coll.Save(sv, true); } catch (const Exception&) { FAIL() << "SaveAndGet"; }
        ASSERT_EQ(String(OTEXT("k1")), sv.GetKey());

        /* option setters */
        SodaOptions options(sdb);
        try { options.SetFetchArraySize(16); } catch (const Exception&) { FAIL() << "fetchsize"; }
        try { options.SetLock(); }             catch (const Exception&) { FAIL() << "lock"; }
        /* SODA hints reject enclosing comment markers (ORA-40856) */
        try { options.SetHint(OTEXT("index(OCILIB_SODA_CPP_BULK)")); } catch (const Exception&) {}

        /* ListIndexes / GetDataGuide covered in the C BulkSaveIndexAndMeta and
           the MBCS-guarded IndexCreateDropList tests — index ops aren't
           callable here: CreateIndex errors (ORA-40671) and IndexDrop SEGVs
           inside the Oracle client in UNICODE builds */

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

#ifdef OCI_CHARSET_MBCS

    /* The OCISodaIndex* calls work in MBCS builds; in UNICODE builds they fail
       inside the Oracle client (ORA-40671 / OCI-22303, IndexDrop even SEGVs) —
       the real assertions live here */
    TEST(TestSodaCpp, IndexCreateDropList)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

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
}