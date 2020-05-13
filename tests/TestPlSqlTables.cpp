#include "ocilib_tests.h"


TEST(TestPlSqlTables, BasicInOutStringTableBinding)
{
    ExecDML
    (
        OTEXT("create package test_plsqltables as")
        OTEXT("  type string_tab is table of varchar2(50) index by binary_integer;")
        OTEXT("  procedure combine_values(value1 in string_tab, value2 in string_tab, result out string_tab);")
        OTEXT("end;")
    );

    ExecDML
    (
        OTEXT("create package body test_plsqltables as")
        OTEXT("  procedure combine_values(value1 in string_tab, value2 in string_tab, result out string_tab) is")
        OTEXT("    begin")
        OTEXT("      for i in 1 ..value1.count")
        OTEXT("      loop")
        OTEXT("        result(i) := value1(i) || ' - ' || value2(i);")
        OTEXT("      end loop;")
        OTEXT("    end;")
        OTEXT("end;")
    );

    const unsigned int ArraySize = 10;
    const unsigned int StringSize = 50;

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto stmt = OCI_StatementCreate(conn);
    ASSERT_NE(nullptr, stmt);

    otext tab_names[ArraySize][StringSize + 1];
    otext tab_values[ArraySize][StringSize + 1];
    otext tab_results[ArraySize][StringSize + 1];


    ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("BEGIN test_plsqltables.combine_values(:tab_names, :tab_values, :tab_results); END;")));

    /* set values */

    for (unsigned int i = 0; i < ArraySize; i++)
    {
        sprintf(tab_names[i], "name  %03d", i + 1);
        sprintf(tab_values[i], "value %03d", i + 1);

        tab_results[i][0] = 0;
    }

    /* prepare call and bind local arrays */

    ASSERT_TRUE(OCI_BindArrayOfStrings(stmt, OTEXT(":tab_names"), reinterpret_cast<char*>(tab_names), StringSize, ArraySize));
    ASSERT_TRUE(OCI_BindArrayOfStrings(stmt, OTEXT(":tab_values"), reinterpret_cast<char*>(tab_values), StringSize, ArraySize));
    ASSERT_TRUE(OCI_BindArrayOfStrings(stmt, OTEXT(":tab_results"), reinterpret_cast<char*>(tab_results), StringSize, ArraySize));

    ASSERT_TRUE(OCI_BindSetDirection(OCI_GetBind(stmt, 1), OCI_BDM_IN));
    ASSERT_TRUE(OCI_BindSetDirection(OCI_GetBind(stmt, 2), OCI_BDM_IN));
    ASSERT_TRUE(OCI_BindSetDirection(OCI_GetBind(stmt, 3), OCI_BDM_OUT));

    /* execute */

    ASSERT_TRUE(OCI_Execute(stmt));

    otext buffer[512];

    for (unsigned int i = 0; i < ArraySize; i++)
    {
        sprintf(buffer, "name  %03d - value %03d", i + 1, i + 1);
        ASSERT_EQ(ostring(buffer), tab_results[i]);
    }

    ASSERT_TRUE(OCI_StatementFree(stmt));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("drop package test_plsqltables"));
}