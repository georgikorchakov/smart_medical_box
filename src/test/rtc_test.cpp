// g++ -Wall -Weffc++ -Wextra -Wsign-conversion  -std=c++17 ./lib/ds3231_rtc/ds3231_rtc.cpp ./lib/i2c/i2c.cpp rtc_test.cpp -o rtc_test -lwiringPi

#include <iostream>
#include <wiringPi.h>
#include "./lib/ds3231_rtc/ds3231_rtc.h"

#define CLOCK_ADDRESS 0x68

int main()
{
    DS32331RTC clock(CLOCK_ADDRESS);
    
    clock.setTime(13, 30, 33);
    clock.setDate(24, 8, 2020);
    clock.setDayOfWeek(2);

    std::cout << "Seconds: " << clock.getSeconds() << "  Minutes: " << clock.getMinutes() << "  Hour: " << clock.getHour24() << "  Day: " << clock.getDayOfWeek() << "  Date: " << clock.getDayOfMonth() << "  Month: " << clock.getMonth() << "  Year: " << clock.getYear() << '\n';

    return 0;
}