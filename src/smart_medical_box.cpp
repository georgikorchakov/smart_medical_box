#include "./smart_medical_box.h"

SmartMedicalBox::SmartMedicalBox(Settings settings)
    : __box {settings.pca9685_address, settings.servo_angle_closed, settings.servo_angle_open},
      __rtc {settings.pcf2129_rtc_address},
      __db {settings.db_host, settings.db_user, settings.db_password, settings.db_name}
{
    // Set control pins
    __box.setPowerControlPin(0, settings.power_monday);
    __box.setPowerControlPin(1, settings.power_tuesday);
    __box.setPowerControlPin(2, settings.power_wednesday);
    __box.setPowerControlPin(3, settings.power_thursday);
    __box.setPowerControlPin(4, settings.power_friday);
    __box.setPowerControlPin(5, settings.power_saturday);
    __box.setPowerControlPin(6, settings.power_sunday);
    __box.setPowerControlPin(7, settings.power_insulin);

    __box.setServoControlPin(0, settings.morning_pwm);
    __box.setServoControlPin(1, settings.midday_pwm);
    __box.setServoControlPin(2, settings.evening_pwm);
    __box.setServoControlPin(3, settings.night_pwm);
    __box.setServoControlPin(4, settings.insulin_day_pwm);
    __box.setServoControlPin(5, settings.insulin_night_pwm);

    __db.createOrUpdateTimeInterval("midday", 16, 30, 17, 30);

    std::cout << "Rtc Addr: " << settings.pcf2129_rtc_address << " hour: " << __rtc.getHour24() << "\n";
}

int SmartMedicalBox::checkTimeAndOpenBox()
{
    std::cout << "Hour: " << __rtc.getHour24() << " Minutes: " << __rtc.getMinutes() << " Day: " << __rtc.getDayOfWeek() << "\n";
    __time_interval = __db.getTimeIntervalByTimeAndDay(
        __rtc.getHour24(), 
        __rtc.getMinutes(), 
        __rtc.getDayOfWeek()
    );

    if (__db.isTimeIntervalEmpty( __time_interval ) == 0)
    {
        // DEBUG
        std::cout << "Time start: " << __time_interval.time_start_h << "\n";
    }

    if (__db.isTimeIntervalEmpty( __time_interval ) == 0 && __flag_is_box_open == 0)
    {
        // Initial Box Opening
        __box.openBox(__rtc.getDayOfWeek(), __time_interval.interval_name);

        // Implement Buzzer

        __flag_is_box_open = 1;
    } 
    else if (__db.isTimeIntervalEmpty(__time_interval) == 1 && __flag_is_box_open == 1)
    {
        // Final Box close
        __box.closeAll();
        __flag_is_box_open = 0;
    }

    __box.openBox(0, 0);

    if (__flag_is_box_open)
    {
        // While box is opened

    }


    return 1;
}