#ifndef PCF2129_RTC_H
#define PCF2129_RTC_H

#include "../i2c/i2c.h"

#define CONTROL_1       0x00
#define CONTROL_2       0x01
#define CONTROL_3       0x02

#define SECONDS         0x03
#define MINUTES         0x04
#define HOURS           0x05
#define DAYS            0x06
#define WEEKDAYS        0x07
#define MONTHS          0x08
#define YEARS           0x09

#define SECOND_ALARM    0x0A
#define MINUTE_ALARM    0x0B
#define HOUR_ALARM      0x0C
#define DAY_ALARM       0x0D
#define WEEKDAY_ALARM   0x0E

#define CLKOUT_CTL      0x0F

#define WATCHDG_TIM_CTL 0x10
#define WATCHDG_TIM_VAL 0x11

#define TIMESTP_CTL     0x12
#define SEC_TIMESTP     0x13
#define MIN_TIMESTP     0x14
#define HOUR_TIMESTP    0x15
#define DAY_TIMESTP     0x16
#define MON_TIMESTP     0x17
#define YEAR_TIMESTP    0x18

#define AGING_OFFSET    0x19

class PCF2129RTC
{
private:
    I2C m_i2c;
    int bcd2dec(int bcd);
    int dec2bcd(int dec);
public:
    PCF2129RTC(int address=0x51);

    int getSeconds();
    int getMinutes();
    int getHour24();
    int getDayOfWeek();
    int getDayOfMonth();
    int getMonth();
    int getYear();

    void setSeconds(int seconds);
    void setMinutes(int minutes);
    void setHour(int hour);
    void setDayOfMonth(int day_of_month);
    void setMonth(int month);
    void setYear(int year); 

    void setTime(int hour, int minutes, int seconds=0);
    void setDate(int day_of_month, int month, int year);
    void setDayOfWeek(int day_of_week);
};

#endif // PCF2129_RTC_H
