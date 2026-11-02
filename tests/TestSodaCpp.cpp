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

        ASSERT_TRUE(coll.Truncate(0));
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
}
