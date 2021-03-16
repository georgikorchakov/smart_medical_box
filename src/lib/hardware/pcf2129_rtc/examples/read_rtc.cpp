// g++ -Wall -Wextra -Wsign-conversion  -std=c++17 ../../i2c/i2c.cpp ../pcf2129_rtc.cpp read_rtc.cpp -o read_rtc -lwiringPi

#include <iostream>
#include "../pcf2129_rtc.h"

int main()
{
    PCF2129RTC rtc(0x51);

    std::cout << "Seconds: " << rtc.getSeconds() 
              << "  Minutes: " << rtc.getMinutes() 
              << "  Hour: " << rtc.getHour24() 
              << "  Day: " << rtc.getDayOfWeek() 
              << "  Date: " << rtc.getDayOfMonth() 
              << "  Month: " << rtc.getMonth() 
              << "  Year: " << rtc.getYear() << '\n';

    return 0;
}