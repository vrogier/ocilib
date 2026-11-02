#include <iostream>

#include "ocilib.hpp"

using namespace ocilib;

int main()
{
    try
    {
        Environment env;

        Timestamp tm(env, Timestamp::NoTimeZone);
        Interval itv(env, Interval::DaySecond);

        tm = Timestamp::SysTimestamp(env);
        std::cout << tm.ToString("DD/MM/YYYY HH24:MI:SS:FF3") << std::endl;

        itv.SetDaySecond(1, 1, 1, 1, 0);
        std::cout << itv.ToString() << std::endl;

        tm += itv;
        std::cout << tm.ToString("DD/MM/YYYY HH24:MI:SS:FF3") << std::endl;
    }
    catch (const Exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return EXIT_SUCCESS;
}

