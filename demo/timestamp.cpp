#include <iostream>

#include "ocilib.hpp"

using namespace ocilib;

int main(void)
{
    try
    {
        Environment::Initialize();

        Timestamp tm(Timestamp::NoTimeZone);
        Interval itv(Interval::DaySecond);

        tm = Timestamp::SysTimestamp();
        std::cout << tm.ToString("DD/MM/YYYY HH24:MI:SS:FF3") << std::endl;

        itv.SetDaySecond(1, 1, 1, 1, 0);
        std::cout << itv.ToString() << std::endl;

        tm += itv;
        std::cout << tm.ToString("DD/MM/YYYY HH24:MI:SS:FF3") << std::endl;
    }
    catch (Exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    Environment::Cleanup();

    return EXIT_SUCCESS;
}

