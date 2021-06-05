#include "./pcf2129_rtc.h"

PCF2129RTC::PCF2129RTC(int address)
    : __i2c{ address }
{
}

int PCF2129RTC::bcd2dec(int bcd)
{
    return (((bcd & 0xF0) >> 4) * 10) + (bcd & 0x0F);
}

int PCF2129RTC::dec2bcd(int dec) 
{
    return ((dec / 10) << 4) + (dec % 10);
}

int PCF2129RTC::getSeconds() 
{
    return bcd2dec(__i2c.readRegWithoutReStart(SECONDS) & 0b01111111);
}

int PCF2129RTC::getMinutes()
{
    return bcd2dec(__i2c.readRegWithoutReStart(MINUTES) & 0b01111111);
}

int PCF2129RTC::getHour24()
{
    return bcd2dec(__i2c.readRegWithoutReStart(HOURS) & 0b00111111);
}

int PCF2129RTC::getDayOfWeek()
{
    return __i2c.readRegWithoutReStart(WEEKDAYS) & 0b00000111;
}

int PCF2129RTC::getDayOfMonth()
{
    return bcd2dec(__i2c.readRegWithoutReStart(DAYS) & 0b00111111);
}

int PCF2129RTC::getMonth()
{
    return bcd2dec(__i2c.readRegWithoutReStart(MONTHS) & 0b00011111);
}

int PCF2129RTC::getYear()
{
    return 2000 + bcd2dec(__i2c.readRegWithoutReStart(YEARS));
}

void PCF2129RTC::setSeconds(int seconds)
{
    __i2c.writeReg8(SECONDS, dec2bcd(seconds));
}

void PCF2129RTC::setMinutes(int minutes)
{
    __i2c.writeReg8(MINUTES, dec2bcd(minutes));
}

void PCF2129RTC::setHour(int hour)
{
    __i2c.writeReg8(HOURS, dec2bcd(hour));
}

void PCF2129RTC::setDayOfWeek(int day_of_week)
{
    __i2c.writeReg8(WEEKDAYS, dec2bcd(day_of_week));
}

void PCF2129RTC::setDayOfMonth(int day_of_month)
{
    __i2c.writeReg8(DAYS, dec2bcd(day_of_month));
}

void PCF2129RTC::setMonth(int month)
{
    __i2c.writeReg8(MONTHS, dec2bcd(month));
}

void PCF2129RTC::setYear(int year)
{
    
    __i2c.writeReg8(YEARS, dec2bcd(year - 2000));
}

void PCF2129RTC::setTime(int hour, int minutes, int seconds)
{
    setHour(hour);
    setMinutes(minutes);
    setSeconds(seconds);
}

void PCF2129RTC::setDate(int day_of_month, int month, int year)
{
    setDayOfMonth(day_of_month);
    setMonth(month);
    setYear(year); 
}
