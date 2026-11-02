#include "ocilib_tests.h"

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestFileCpp, CreateAndGettersSettersCpp)
    {
        Environment& env = GetEnvironmentCpp();
        Connection conn(env, DBS, USR, PWD);

        File file(conn);
        ASSERT_FALSE(file.IsOpened());
        ASSERT_EQ(0u, file.GetOffset());

        file.SetInfos(OTEXT("TEST_DIR"), OTEXT("test.txt"));
        ASSERT_EQ(ostring(OTEXT("TEST_DIR")), file.GetDirectory());
        ASSERT_EQ(ostring(OTEXT("test.txt")), file.GetName());

        File file2(conn, OTEXT("TEST_DIR"), OTEXT("test.txt"));
        ASSERT_TRUE(file == file2);
        ASSERT_FALSE(file != file2);

        File file3 = file2.Clone();
        ASSERT_TRUE(file3 == file2);
        ASSERT_EQ(ostring(OTEXT("test.txt")), file3.GetName());
    }
}
