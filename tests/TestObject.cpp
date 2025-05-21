#include "ocilib_tests.h"

using namespace ocilib;

namespace TestCApi
{
    TEST(TestObject, SetGetBasicProps)
    {
        ExecDML(OTEXT("create type TestObjectSetGetBasicPropsVendor as object(code number, name varchar2(30))"));
        ExecDML(OTEXT("create type TestObjectSetGetBasicPropsSale as object(code number, price float, name varchar2(30), ref varchar2(30), date_sale date, vendor TestObjectSetGetBasicPropsVendor)"));

        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto date = OCI_DateCreate(nullptr);
        ASSERT_NE(nullptr, date);

        OCI_DateSysDate(date);

        const auto vendor = OCI_ObjectCreate(conn, OCI_TypeInfoGet(conn, OTEXT("TestObjectSetGetBasicPropsVendor"), OCI_TIF_TYPE));
        ASSERT_NE(nullptr, vendor);
        ASSERT_TRUE(OCI_ObjectSetInt(vendor, OTEXT("CODE"), 134));
        ASSERT_TRUE(OCI_ObjectSetString(vendor, OTEXT("NAME"), OTEXT("JOHN SMITH")));

        const auto sale = OCI_ObjectCreate(conn, OCI_TypeInfoGet(conn, OTEXT("TestObjectSetGetBasicPropsSale"), OCI_TIF_TYPE));
        ASSERT_NE(nullptr, vendor);

        ASSERT_TRUE(OCI_ObjectSetInt(sale, OTEXT("CODE"), 1));
        ASSERT_TRUE(OCI_ObjectSetDouble(sale, OTEXT("PRICE"), 12.99));
        ASSERT_TRUE(OCI_ObjectSetString(sale, OTEXT("NAME"), OTEXT("USB KEY 2go")));
        ASSERT_TRUE(OCI_ObjectSetString(sale, OTEXT("REF"), OTEXT("A56547WSAA")));
        ASSERT_TRUE(OCI_ObjectSetObject(sale, OTEXT("VENDOR"), vendor));
        ASSERT_TRUE(OCI_ObjectSetDate(sale, OTEXT("DATE_SALE"), date));

        ASSERT_EQ(1, OCI_ObjectGetInt(sale, OTEXT("CODE")));
        ASSERT_EQ(12.99, OCI_ObjectGetDouble(sale, OTEXT("PRICE")));
        ASSERT_EQ(ostring(OTEXT("USB KEY 2go")), ostring(OCI_ObjectGetString(sale, OTEXT("NAME"))));
        ASSERT_EQ(ostring(OTEXT("A56547WSAA")), ostring(OCI_ObjectGetString(sale, OTEXT("REF"))));

        const auto vendor2 = OCI_ObjectGetObject(sale, OTEXT("VENDOR"));
        ASSERT_NE(nullptr, vendor2);

        ASSERT_EQ(134, OCI_ObjectGetInt(vendor2, OTEXT("CODE")));
        ASSERT_EQ(ostring(OTEXT("JOHN SMITH")), ostring(OCI_ObjectGetString(vendor2, OTEXT("NAME"))));

        const auto date2 = OCI_ObjectGetDate(sale, OTEXT("DATE_SALE"));
        ASSERT_NE(nullptr, date2);
        ASSERT_EQ(0, OCI_DateCompare(date, date2));

        ASSERT_TRUE(OCI_ObjectFree(sale));
        ASSERT_TRUE(OCI_ObjectFree(vendor));
        ASSERT_TRUE(OCI_DateFree(date));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_Cleanup());

       ExecDML(OTEXT("drop type TestObjectSetGetBasicPropsSale"));
       ExecDML(OTEXT("drop type TestObjectSetGetBasicPropsVendor"));
    }
}

namespace TestCppApi
{
    TEST(TestObject, ToStringCpp)
    {
        ExecDML(OTEXT("create type TestObjectToStringCpp as object(code number, name varchar2(30))"));

        Environment::Initialize();

        Connection conn(DBS, USR, PWD);
        TypeInfo typinf(conn, OTEXT("TestObjectToStringCpp"), TypeInfo::Type);
        Object obj(typinf);

        obj.Set<int>(OTEXT("code"), 1);
        obj.Set<ostring>(OTEXT("name"), OTEXT("computer"));

        ostring expected = OTEXT("TestObjectToStringCpp(1, 'computer')");

        ASSERT_EQ(ToUpper(expected), ToUpper(obj.ToString()));

        Environment::Cleanup();

        ExecDML(OTEXT("drop type TestObjectToStringCpp"));
    }
}