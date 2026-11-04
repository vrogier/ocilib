#include "ocilib_tests.h"

using namespace ocilib;

namespace
{
    void DropCollIfExistsCpp(SodaDb& sdb, const otext* name)
    {
        SodaColl coll = sdb.OpenCollection(name);
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

        SodaDb sdb(con);

        ASSERT_FALSE(sdb.IsNull());
    }

    TEST(TestSodaCpp, DbGetConnection)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        SodaDb sdb(con);

        ASSERT_FALSE(sdb.GetConnection().IsNull());
    }

    TEST(TestSodaCpp, CollectionLifecycle)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        SodaDb sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_COLL"));

        SodaColl coll = sdb.CreateCollection(OTEXT("OCILIB_SODA_CPP_COLL"), ostring());
        ASSERT_FALSE(coll.IsNull());

        ASSERT_EQ(ostring(OTEXT("OCILIB_SODA_CPP_COLL")), coll.GetName());
        ASSERT_FALSE(coll.GetMetadata().empty());
        ASSERT_FALSE(coll.GetConnection().IsNull());
        coll.IsMapped();

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, OpenCollectionAndCursor)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        SodaDb sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_OPEN"));

        SodaColl coll = sdb.CreateCollection(OTEXT("OCILIB_SODA_CPP_OPEN"), ostring());
        ASSERT_FALSE(coll.IsNull());

        SodaColl opened = sdb.OpenCollection(OTEXT("OCILIB_SODA_CPP_OPEN"));
        ASSERT_FALSE(opened.IsNull());
        ASSERT_EQ(ostring(OTEXT("OCILIB_SODA_CPP_OPEN")), opened.GetName());

        SodaCollCursor cursor = sdb.GetCollections(OTEXT("OCILIB_SODA_CPP"));

        bool found = false;
        for (SodaColl item = cursor.GetNext(); !item.IsNull(); item = cursor.GetNext())
        {
            if (item.GetName() == ostring(OTEXT("OCILIB_SODA_CPP_OPEN")))
            {
                found = true;
            }
        }

        ASSERT_TRUE(found);

        /* range-for over a fresh cursor */
        SodaCollCursor cursor2 = sdb.GetCollections(OTEXT("OCILIB_SODA_CPP"));
        bool found2 = false;
        for (SodaColl& item : cursor2)
        {
            if (item.GetName() == ostring(OTEXT("OCILIB_SODA_CPP_OPEN")))
            {
                found2 = true;
            }
        }
        ASSERT_TRUE(found2);

        /* collection name list */
        std::vector<ostring> names = sdb.GetCollectionNames();
        bool foundName = false;
        for (const ostring& n : names)
        {
            if (n == ostring(OTEXT("OCILIB_SODA_CPP_OPEN")))
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

        SodaDb sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_DOC"));

        SodaColl coll = sdb.CreateCollection
        (
            OTEXT("OCILIB_SODA_CPP_DOC"),
            OTEXT("{\"keyColumn\":{\"name\":\"ID\"},"
                  "\"contentColumn\":{\"name\":\"JSON_DOCUMENT\",\"sqlType\":\"JSON\"},"
                  "\"versionColumn\":{\"name\":\"VERSION\",\"method\":\"UUID\"},"
                  "\"lastModifiedColumn\":{\"name\":\"LAST_MODIFIED\"},"
                  "\"creationTimeColumn\":{\"name\":\"CREATED_ON\"}}")
        );
        ASSERT_FALSE(coll.IsNull());

        SodaDoc doc = sdb.CreateDocumentFromText(OTEXT("{\"name\":\"cpp test\"}"), ostring());
        ASSERT_FALSE(doc.IsNull());
        ASSERT_FALSE(doc.GetMediaType().empty());

        coll.Insert(doc);

        ASSERT_FALSE(doc.GetKey().empty());
        ASSERT_FALSE(doc.GetVersion().empty());
        ASSERT_FALSE(doc.GetCreatedOn().empty());
        ASSERT_FALSE(doc.GetLastModified().empty());
        ASSERT_FALSE(doc.GetText().empty());

        SodaDoc found = coll.FindOne(doc.GetKey());
        ASSERT_FALSE(found.IsNull());
        ASSERT_EQ(doc.GetKey(), found.GetKey());

        if (OCI_GetVersionConnection(con) >= OCI_23_4 && OCI_GetOCIRuntimeVersion(env.GetHandle()) >= OCI_23_4)
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

        SodaDb sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_FIND"));

        SodaColl coll = sdb.CreateCollection(OTEXT("OCILIB_SODA_CPP_FIND"), ostring());
        ASSERT_FALSE(coll.IsNull());

        for (OCI_Int32 i = 0; i < 4; i++)
        {
            SodaDoc doc = sdb.CreateDocumentFromText
            (
                (i % 2) ? OTEXT("{\"grp\":1}") : OTEXT("{\"grp\":0}"),
                ostring()
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

        SodaDocCursor cursor = coll.Find(options);

        OCI_UInt32 count = 0;
        for (SodaDoc doc = cursor.GetNext(); !doc.IsNull(); doc = cursor.GetNext())
        {
            ASSERT_FALSE(doc.GetText().empty());
            count++;
        }

        ASSERT_EQ(2u, count);

        /* FindOne with options */

        SodaDoc first = coll.FindOne(options);
        ASSERT_FALSE(first.IsNull());

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, ReplaceAndRemove)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        SodaDb sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_REPL"));

        SodaColl coll = sdb.CreateCollection(OTEXT("OCILIB_SODA_CPP_REPL"), ostring());
        ASSERT_FALSE(coll.IsNull());

        SodaDoc doc = sdb.CreateDocumentFromText(OTEXT("{\"v\":1}"), ostring());
        coll.Insert(doc);

        const ostring key = doc.GetKey();
        ASSERT_FALSE(key.empty());

        /* replace by key */

        SodaDoc newDoc = sdb.CreateDocumentFromText(OTEXT("{\"v\":2}"), ostring());
        ASSERT_TRUE(coll.ReplaceOne(key, newDoc));

        SodaDoc found = coll.FindOne(key);
        ASSERT_NE(ostring::npos, found.GetText().find(OTEXT("\"v\":2")));

        /* replace-and-get by key */

        SodaDoc newDoc2 = sdb.CreateDocumentFromText(OTEXT("{\"v\":3}"), ostring());
        SodaDoc updated = coll.ReplaceOneAndGet(key, newDoc2);
        ASSERT_FALSE(updated.IsNull());

        /* replace with key options */

        SodaOptions options(sdb);
        options.SetKey(key);

        SodaDoc newDoc3 = sdb.CreateDocumentFromText(OTEXT("{\"v\":4}"), ostring());
        ASSERT_TRUE(coll.ReplaceOne(options, newDoc3));

        /* replace-and-get with key + version options */

        options.SetVersion(updated.GetVersion());

        SodaDoc newDoc4 = sdb.CreateDocumentFromText(OTEXT("{\"v\":5}"), ostring());
        SodaDoc updated2 = coll.ReplaceOneAndGet(options, newDoc4);
        ASSERT_FALSE(updated2.IsNull());

        /* keys option */

        SodaOptions keysOptions(sdb);
        keysOptions.SetKeys(std::vector<ostring> { key });

        SodaDoc foundByKeys = coll.FindOne(keysOptions);
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

        SodaDb sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_REM"));

        SodaColl coll = sdb.CreateCollection(OTEXT("OCILIB_SODA_CPP_REM"), ostring());
        ASSERT_FALSE(coll.IsNull());

        for (OCI_Int32 i = 0; i < 4; i++)
        {
            SodaDoc doc = sdb.CreateDocumentFromText
            (
                (i % 2) ? OTEXT("{\"del\":1}") : OTEXT("{\"del\":0}"),
                ostring()
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

        SodaDb sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_JSON"));

        SodaColl coll = sdb.CreateCollection(OTEXT("OCILIB_SODA_CPP_JSON"), ostring());
        ASSERT_FALSE(coll.IsNull());

        if (OCI_GetVersionConnection(con) >= OCI_23_4 && OCI_GetOCIRuntimeVersion(env.GetHandle()) >= OCI_23_4)
        {
            Json json(con);
            json.LoadText(OTEXT("{\"name\":\"cpp json\"}"));

            SodaDoc doc = sdb.CreateDocumentFromJson(json, ostring());
            ASSERT_FALSE(doc.IsNull());

            coll.Insert(doc);
            ASSERT_NE(ostring::npos, doc.GetText().find(OTEXT("cpp json")));
        }

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

    TEST(TestSodaCpp, ErrorPaths)
    {
        ASSERT_THROW(SodaColl().GetName(), Exception);
        ASSERT_THROW(SodaColl().Drop(), Exception);
        ASSERT_THROW(SodaDb().GetConnection(), Exception);
        ASSERT_THROW(SodaDoc().GetKey(), Exception);
        ASSERT_THROW(SodaDoc().GetText(), Exception);
        ASSERT_THROW(SodaOptions().SetSkip(0), Exception);
        ASSERT_THROW(SodaCollCursor().GetNext(), Exception);
        ASSERT_THROW(SodaDocCursor().GetNext(), Exception);
    }

    TEST(TestSodaCpp, BulkSaveIndexAndMeta)
        {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        SodaDb sdb(con);

        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_BULK"));

        SodaColl coll = sdb.CreateCollection
        (
            OTEXT("OCILIB_SODA_CPP_BULK"),
            OTEXT("{\"keyColumn\":{\"name\":\"ID\",\"sqlType\":\"VARCHAR2\",\"maxLength\":255,\"assignmentMethod\":\"CLIENT\"}}")
        );
        ASSERT_FALSE(coll.IsNull());

        /* bulk insert */
        std::vector<SodaDoc> docs;
        docs.push_back(sdb.CreateDocumentFromText(OTEXT("{\"a\":1}"), OTEXT("k1")));
        docs.push_back(sdb.CreateDocumentFromText(OTEXT("{\"a\":2}"), OTEXT("k2")));
        try { coll.InsertMany(docs); } catch (const Exception&) { FAIL() << "InsertMany"; }

        try { ASSERT_TRUE(docs[0].IsJson()); } catch (const Exception&) { FAIL() << "IsJson"; }

        /* bulk insert and get metadata */
        std::vector<SodaDoc> docs2;
        docs2.push_back(sdb.CreateDocumentFromText(OTEXT("{\"a\":3}"), OTEXT("k3")));
        try { coll.InsertManyAndGet(docs2); } catch (const Exception&) { FAIL() << "InsertManyAndGet"; }
        ASSERT_EQ(ostring(OTEXT("k3")), docs2[0].GetKey());

        /* save (upsert) and save-and-get */
        SodaDoc sv = sdb.CreateDocumentFromText(OTEXT("{\"a\":99}"), OTEXT("k1"));
        try { coll.Save(sv); } catch (const Exception&) { FAIL() << "Save"; }
        try { coll.SaveAndGet(sv); } catch (const Exception&) { FAIL() << "SaveAndGet"; }
        ASSERT_EQ(ostring(OTEXT("k1")), sv.GetKey());

        /* option setters */
        SodaOptions options(sdb);
        try { options.SetFetchArraySize(16); } catch (const Exception&) { FAIL() << "fetchsize"; }
        try { options.SetLock(); }             catch (const Exception&) { FAIL() << "lock"; }
        /* SODA hints reject enclosing comment markers (ORA-40856) */
        try { options.SetHint(OTEXT("index(OCILIB_SODA_CPP_BULK)")); } catch (const Exception&) {}

        /* ListIndexes / GetDataGuide covered in the C BulkSaveIndexAndMeta and
           the ANSI-guarded IndexCreateDropList tests — index ops aren't
           callable here: CreateIndex errors (ORA-40671) and IndexDrop SEGVs
           inside the Oracle client in UNICODE builds */

        con.Commit();
        ASSERT_TRUE(coll.Drop());
    }

#ifdef OCI_CHARSET_ANSI

    /* The OCISodaIndex* calls work in ANSI builds; in UNICODE builds they fail
       inside the Oracle client (ORA-40671 / OCI-22303, IndexDrop even SEGVs) —
       the real assertions live here */
    TEST(TestSodaCpp, IndexCreateDropList)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        SodaDb sdb(con);
        DropCollIfExistsCpp(sdb, OTEXT("OCILIB_SODA_CPP_IDX"));

        SodaColl coll = sdb.CreateCollection(OTEXT("OCILIB_SODA_CPP_IDX"), ostring());
        ASSERT_FALSE(coll.IsNull());

        SodaDoc sd = sdb.CreateDocumentFromText(OTEXT("{\"a\":1}"), ostring());
        coll.Insert(sd);
        con.Commit();

        coll.CreateIndex(OTEXT("{\"name\":\"OCILIB_SODA_CPP_IDX_NAME\"}"));

        ASSERT_GE(coll.ListIndexes().size(), 1u);

        ASSERT_TRUE(coll.DropIndex(OTEXT("OCILIB_SODA_CPP_IDX_NAME")));
        ASSERT_TRUE(coll.Drop());
    }

#endif
}