// g++ -Wall -Wextra -Wsign-conversion  -std=c++17 ../../i2c/i2c.cpp ../pcf2129_rtc.cpp set_rtc.cpp -o set_rtc -lwiringPi

#include <iostream>
#include "../pcf2129_rtc.h"

int main()
{
    PCF2129RTC rtc(0x51);

    int day_of_month;
    int month;
    int year;
    int day_of_week;
    int hour;
    int minutes;

    std::cout << "Year = ";
    std::cin >> year;

    std::cout << "Month = ";
    std::cin >> month;

    std::cout << "Day of month = ";
    std::cin >> day_of_month;

    rtc.setDate(day_of_month, month, year);

    std::cout << "Day of the week = ";
    std::cin >> day_of_week;

    rtc.setDayOfWeek(day_of_week);

    std::cout << "Hour = ";
    std::cin >> hour;

    std::cout << "Minutes = ";
    std::cin >> minutes;

    rtc.setTime(hour, minutes, 0);

    return 0;
}