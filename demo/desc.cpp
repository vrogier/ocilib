#include "ocilib.hpp"
#include <iomanip>

using namespace ocilib;

int main(void)
{
    try
    {
        Environment::Initialize();

        Connection con("db", "usr", "pwd");
        TypeInfo table(con, "products", TypeInfo::ObjectType);

        std::cout << "Column Name         Type      Length  Prec.   Scale   Null ?" << std::endl;
        std::cout << "----------------------------  ------------------------------" << std::endl;
   
        std::cout <<  std::setiosflags(std::ios::left);

        for(int i = 1, n = table.GetColumnCount(); i <= n; i++)
        {
            Column col = table.GetColumn(i);

            std::cout <<  std::setw(20) << col.GetName().c_str() 
                      <<  std::setw(10) << col.GetSQLType().c_str() 
                      <<  std::setw(8)  << col.GetSize()
                      <<  std::setw(8)  << col.GetPrecision()
                      <<  std::setw(8)  << col.GetScale()
                      <<  std::setw(2)  << col.IsNullable()
                      <<  std::endl;
        
        }

    }
    catch(Exception &ex)
    {
         std::cout << ex.GetMessage() << std::endl;
    }

    Environment::Cleanup();
 
    return EXIT_SUCCESS;
}

