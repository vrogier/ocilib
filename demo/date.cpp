#include <iostream>

#include "ocilib.hpp"

using namespace ocilib;

const ostring format = "YYYY-MM-DD HH24:MI:SS";

int main(void)
{
    try
    {
        Environment::Initialize();

        Date date("1978-04-13 20:20:12", format);
        std::cout << date.ToString(format) << std::endl;

        date = Date::SysDate();
        std::cout << date.ToString(format) << std::endl;

        date.AddDays(5);
        date.AddMonths(2);
        std::cout << "Date + 5 days and 2 months is " << date.ToString(format) << std::endl;

        std::cout << "Last day of the month : " << date.LastDay().ToString(format) << std::endl;

        std::cout << "Number of days until the end of the months : " << date.LastDay().DaysBetween(date) << std::endl;

    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    Environment::Cleanup();

    return EXIT_SUCCESS;
}
