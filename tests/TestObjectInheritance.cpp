#include "ocilib_tests.h"

namespace TestCApi
{
    TEST(TestObjectInheritance, OCI_TypeInfoGetSuperType)
    {
        ExecDML(OTEXT("create type TestObjectInheritanceVehicule as object (code  number, name  varchar2(30)) not final"));
        ExecDML(OTEXT("create type TestObjectInheritanceCar under TestObjectInheritanceVehicule (price  number) not final"));
        ExecDML(OTEXT("create type TestObjectInheritanceRacingCar under TestObjectInheritanceCar (speed  number)"));

        ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

        const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        otext buffer[512];
        unsigned int size = sizeof(buffer) - 1;

        const auto racingCar = OCI_ObjectCreate(conn, OCI_TypeInfoGet(conn, OTEXT("TestObjectInheritanceRacingCar"), OCI_TIF_TYPE));
        ASSERT_NE(nullptr, racingCar);

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("begin :obj := TestObjectInheritanceRacingCar(1,'Formula1', 123456789, 300); end;")));
        ASSERT_TRUE(OCI_BindObject(stmt, OTEXT("obj"), racingCar));
        ASSERT_TRUE(OCI_Execute(stmt));
        ASSERT_TRUE(OCI_ObjectToText(racingCar, &size, buffer));

        ASSERT_EQ(ostring(OTEXT("TESTOBJECTINHERITANCERACINGCAR(1, 'Formula1', 123456789, 300)")), ostring(buffer));

        const auto typinfRacingCar = OCI_ObjectGetTypeInfo(racingCar);
        ASSERT_NE(nullptr, typinfRacingCar);
        const auto typinfCar = OCI_TypeInfoGetSuperType(typinfRacingCar);
        ASSERT_NE(nullptr, typinfCar);
        const auto typinfVehicule = OCI_TypeInfoGetSuperType(typinfCar);
        ASSERT_NE(nullptr, typinfVehicule);

        ASSERT_EQ(ostring(OTEXT("TESTOBJECTINHERITANCERACINGCAR")), ostring(OCI_TypeInfoGetName(typinfRacingCar)));
        ASSERT_EQ(ostring(OTEXT("TESTOBJECTINHERITANCECAR")), ostring(OCI_TypeInfoGetName(typinfCar)));
        ASSERT_EQ(ostring(OTEXT("TESTOBJECTINHERITANCEVEHICULE")), ostring(OCI_TypeInfoGetName(typinfVehicule)));

        ASSERT_EQ(TRUE, OCI_TypeInfoIsFinalType(typinfRacingCar));
        ASSERT_EQ(FALSE, OCI_TypeInfoIsFinalType(typinfCar));
        ASSERT_EQ(FALSE, OCI_TypeInfoIsFinalType(typinfVehicule));

        ASSERT_TRUE(OCI_ObjectFree(racingCar));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_Cleanup());

        ExecDML(OTEXT("drop type TestObjectInheritanceRacingCar"));
        ExecDML(OTEXT("drop type TestObjectInheritanceCar"));
        ExecDML(OTEXT("drop type TestObjectInheritanceVehicule"));
    }
}