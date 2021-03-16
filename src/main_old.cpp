#include <iostream>
#include <cstring>
#include <wiringPi.h>
#include "./config/settings.h"
#include "./lib/database/database.h"
#include "./lib/ds3231_rtc/ds3231_rtc.h"
#include "./lib/box_opening/box_opening.h"
#include "./lib/peripherals/peripherals.h"

void opened_box_procedure(Database &database, Peripherals &peripherals, BoxOpening &box, DS32331RTC &clock, TimeInterval time_interval)
{
    int minutes_left = 1;
    static int beep_once = 0;
    static int is_box_open = 0;
    
    if (database.isTimeIntervalEmpty(time_interval) == 0 && is_box_open == 0)
    {
        peripherals.setBuzzerOn();
        box.openBox(clock.getDayOfWeek(), time_interval.interval_name);
        peripherals.setBuzzerOff();
        is_box_open = 1;
    }
    else if (database.isTimeIntervalEmpty(time_interval) == 1 && is_box_open == 1)
    {
        box.closeAll();
        is_box_open = 0;
    }

    if (is_box_open)
    {
        // If button is pressed, box is toggled.
        if (peripherals.readButton())
        {
            // There is natural delay
            box.toggleBox(clock.getDayOfWeek(), time_interval.interval_name);
        }

        if (time_interval.time_end_h == clock.getHour24() && time_interval.time_end_m - clock.getMinutes() <= minutes_left && beep_once == 0)
        {
            peripherals.setBuzzerOn();
            delay(2000);
            peripherals.setBuzzerOff();
            beep_once = 1;
        }
    }
    else
    {
        beep_once = 0;
    }
}

int main()
{
    Database database(settings::db_host, settings::db_user, settings::db_password, settings::db_name);
    DS32331RTC clock(settings::clock_address);
    BoxOpening box(settings::sda, settings::sck, 
                   settings::latch, settings::servo_address, 
                   settings::servo_angle_closed, settings::servo_angle_open);
    Peripherals peripherals(settings::button_pin, settings::button_led_pin, settings::buzzer_pin);

    database.createOrUpdateTimeInterval("evening", 18, 0, 18, 30);

    TimeInterval time_interval;
    
    while (true)
    {
        time_interval = database.getTimeIntervalByTimeAndDay(
            clock.getHour24(), 
            clock.getMinutes(), 
            clock.getDayOfWeek()
        );

        opened_box_procedure(database, peripherals, box, clock, time_interval);
        
    }

    return 0;
}
