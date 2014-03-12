#include "ocilib.hpp"

using namespace ocilib;

int main(void)
{
    try
    {
        Environment::Initialize();

        Date date;

        date.FromString("13041978 20:20:12", "DDMMYYYY HH24:MI:SS");
        std::cout << date.ToString("DD/MM/YYYY HH24:MI:SS") << std::endl;

        date.SysDate();
        std::cout << date.ToString("DD/MM/YYYY HH24:MI:SS") << std::endl;

        date.AddDays(5);
        date.AddMonths(2);
        std::cout << "Date + 5 days and 2 months is " << date.ToString("DD/MM/YYYY HH24:MI:SS") << std::endl;

        std::cout << "Last day of the month : " << date.LastDay().ToString("DD/MM/YYYY HH24:MI:SS") << std::endl;

        std::cout << "Number of days until the end of the months : " << date.LastDay().DaysBetween(date) << std::endl;

    }
    catch(Exception &ex)
    {
         std::cout << ex.GetMessage() << std::endl;
    }

    Environment::Cleanup();
 
    return EXIT_SUCCESS;
}

