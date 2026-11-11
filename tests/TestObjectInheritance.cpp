#include "ocilib_tests.h"

namespace TestCApi
{
    TEST(TestObjectInheritance, OCI_TypeInfoGetSuperType)
    {
        ExecDML(OTEXT("create type TestObjectInheritanceVehicule as object (code  number, name  varchar2(30)) not final"));
        ExecDML(OTEXT("create type TestObjectInheritanceCar under TestObjectInheritanceVehicule (price  number) not final"));
        ExecDML(OTEXT("create type TestObjectInheritanceRacingCar under TestObjectInheritanceCar (speed  number)"));

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        otext buffer[512];
        OCI_UInt32 size = sizeof(buffer) - 1;

        const auto racingCar = OCI_ObjectCreate(OCI_TypeInfoGet(conn, OTEXT("TestObjectInheritanceRacingCar"), OCI_TIF_TYPE));
        ASSERT_NE(nullptr, racingCar);

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("begin :obj := TestObjectInheritanceRacingCar(1,'Formula1', 123456789, 300); end;")));
        ASSERT_NE(nullptr, OCI_StatementBindObject(stmt, OTEXT("obj"), racingCar));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));
        ASSERT_TRUE(OCI_ObjectToText(racingCar, &size, buffer));

        ASSERT_EQ(String(OTEXT("TESTOBJECTINHERITANCERACINGCAR(1, 'Formula1', 123456789, 300)")), String(buffer));

        const auto typinfRacingCar = OCI_ObjectGetTypeInfo(racingCar);
        ASSERT_NE(nullptr, typinfRacingCar);
        const auto typinfCar = OCI_TypeInfoGetSuperType(typinfRacingCar);
        ASSERT_NE(nullptr, typinfCar);
        const auto typinfVehicule = OCI_TypeInfoGetSuperType(typinfCar);
        ASSERT_NE(nullptr, typinfVehicule);

        ASSERT_EQ(String(OTEXT("TESTOBJECTINHERITANCERACINGCAR")), String(OCI_TypeInfoGetName(typinfRacingCar)));
        ASSERT_EQ(String(OTEXT("TESTOBJECTINHERITANCECAR")), String(OCI_TypeInfoGetName(typinfCar)));
        ASSERT_EQ(String(OTEXT("TESTOBJECTINHERITANCEVEHICULE")), String(OCI_TypeInfoGetName(typinfVehicule)));

        ASSERT_EQ(TRUE, OCI_TypeInfoIsFinalType(typinfRacingCar));
        ASSERT_EQ(FALSE, OCI_TypeInfoIsFinalType(typinfCar));
        ASSERT_EQ(FALSE, OCI_TypeInfoIsFinalType(typinfVehicule));

        ASSERT_TRUE(OCI_ObjectFree(racingCar));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop type TestObjectInheritanceRacingCar"));
        ExecDML(OTEXT("drop type TestObjectInheritanceCar"));
        ExecDML(OTEXT("drop type TestObjectInheritanceVehicule"));
    }
}