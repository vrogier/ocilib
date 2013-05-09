#include "ocilib.hpp"

using namespace ocilib;

int main(void)
{
    try
    {
        Environment::Initialize();

        Date d1, d2;

        d1.FromString("13041978 20:20:12", "DDMMYYYY HH24:MI:SS");
        std::cout << d1.ToString("DD/MM/YYYY HH24:MI:SS") << std::endl;

        d1.SysDate();
        std::cout << d1.ToString("DD/MM/YYYY HH24:MI:SS") << std::endl;

        d1.AddDays(5);
        d1.AddMonths(2);
        std::cout << "Date + 5 days and 2 months is " << d1.ToString("DD/MM/YYYY HH24:MI:SS") << std::endl;

        d2.Assign(d1);
        
        d1.LastDay();
        std::cout << "Last day of the month : " << d1.ToString("DD/MM/YYYY HH24:MI:SS") << std::endl;

        std::cout << "Number of days until the end of the months : " << d1.DaysBetween(d2) << std::endl;
    }
    catch(Exception &ex)
    {
         std::cout << ex.GetMessage() << std::endl;
    }

    Environment::Cleanup();
 
    return EXIT_SUCCESS;
}

