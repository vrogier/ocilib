#include <iostream>
#include <iomanip>

#include "ocilib.hpp"

using namespace ocilib;

int main(void)
{
    try
    {
        Environment::Initialize();

        Connection con("db", "usr", "pwd");
        TypeInfo table(con, "all_users", TypeInfo::View);

        std::cout << "Column Name         Type      Length  Prec.   Scale   Null ?" << std::endl;
        std::cout << "----------------------------  ------------------------------" << std::endl;

        std::cout << std::setiosflags(std::ios::left);

        for (int i = 1, n = table.GetColumnCount(); i <= n; i++)
        {
            Column col = table.GetColumn(i);

            std::cout << std::setw(20) << col.GetName()
                << std::setw(10) << col.GetSQLType()
                << std::setw(8) << col.GetSize()
                << std::setw(8) << col.GetPrecision()
                << std::setw(8) << col.GetScale()
                << std::setw(2) << col.IsNullable()
                << std::endl;
        }

    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    Environment::Cleanup();

    return EXIT_SUCCESS;
}

