#include "./smart_medical_box.h"

SmartMedicalBox::SmartMedicalBox(Settings settings)
    : __box {settings.pca9685_address, settings.servo_angle_closed, settings.servo_angle_open},
      __rtc {settings.pcf2129_rtc_address},
      __db {settings.db_host, settings.db_user, settings.db_password, settings.db_name},
      __ui {settings.ui_socket_address}
{
    // Set control pins
    __box.setPowerControlPin(POWER_PIN_MONDAY, settings.power_monday);
    __box.setPowerControlPin(POWER_PIN_TUESDAY, settings.power_tuesday);
    __box.setPowerControlPin(POWER_PIN_WEDNESDAY, settings.power_wednesday);
    __box.setPowerControlPin(POWER_PIN_THURSDAY, settings.power_thursday);
    __box.setPowerControlPin(POWER_PIN_FRIDAY, settings.power_friday);
    __box.setPowerControlPin(POWER_PIN_SATURDAY, settings.power_saturday);
    __box.setPowerControlPin(POWER_PIN_SUNDAY, settings.power_sunday);
    __box.setPowerControlPin(POWER_PIN_INSULIN, settings.power_insulin);

    __box.setServoControlPin(SERVO_PIN_MORNING, settings.morning_pwm);
    __box.setServoControlPin(SERVO_PIN_MIDDAY, settings.midday_pwm);
    __box.setServoControlPin(SERVO_PIN_EVENING, settings.evening_pwm);
    __box.setServoControlPin(SERVO_PIN_NIGHT, settings.night_pwm);
    __box.setServoControlPin(SERVO_PIN_INSULIN_DAY, settings.insulin_day_pwm);
    __box.setServoControlPin(SERVO_PIN_INSULIN_NIGHT, settings.insulin_night_pwm);

    __box.setBuzzerPin(settings.buzzer);

    __box.setButtonPin(settings.button_pin);
    __box.setButtonLedPin(settings.button_led_pin);

    __led.bulgarian_flag_test();
    delay(5000);
    __led.clear();
    __led.render();
}

int SmartMedicalBox::checkTimeAndOpenBox()
{
    __time_interval = __db.getTimeIntervalByTimeAndDay(
        __rtc.getHour24(), 
        __rtc.getMinutes(), 
        __rtc.getDayOfWeek()
    );

    if (!__db.isTimeIntervalEmpty( __time_interval ) && !__flag_is_box_open)
    {
        // Initial Box Opening
        __box.turnButtonLedOn();
        __led.snake_animation_top_to_bottom(
            1 + __rtc.getDayOfWeek(), 
            __box.servoControlLineNameToNumber(__time_interval.interval_name), 
            0xffc738, 0xffffff
        );
        __box.openBox(__rtc.getDayOfWeek(), __time_interval.interval_name);
        __box.buzzerBeep(2000);
        __flag_is_box_open = 1;
    } 
    else if (__db.isTimeIntervalEmpty(__time_interval) && __flag_is_box_open)
    {
        // Final Box close
        __led.snake_animation_bottom_to_top(0xffc738, 0xffffff);
        __box.buzzerBeep(1000);
        __box.closeAll();
        __box.turnButtonLedOff();
        __flag_is_box_open = 0;
    }

    if (__flag_is_box_open)
    {
        // While box is opened
        if (__box.readButton())
        {
            __box.toggleBox(__rtc.getDayOfWeek(), __time_interval.interval_name);
        }
    }

    return 1;
}

int SmartMedicalBox::checkTimeAndOpenInsulin()
{
    __insulin_time_interval = __db.getInsulinTimeIntervalByTime(
        __rtc.getHour24(), 
        __rtc.getMinutes()
    );

    if (!__db.isInsulinTimeIntervalEmpty( __insulin_time_interval ) && !__flag_is_insulin_open)
    {
        // Initial Insulin Opening
        if (__insulin_time_interval.insulin_type == INSULIN_TYPE_DAY)
        {
            // TODO: Replace with animation
            __led.draw_line(3, 7, 4, 0xffffff);
            __led.render();

            __box.openInsulinDay();
        } 
        else if (__insulin_time_interval.insulin_type == INSULIN_TYPE_NIGHT)
        {
            // TODO: Replace with animation
            __led.draw_line(3, 7, 5, 0xffffff);
            __led.render();

            __box.openInsulinNight();
        }

        __box.buzzerBeep(1000);

        __flag_is_insulin_open = 1;
    } 
    else if (__db.isInsulinTimeIntervalEmpty( __insulin_time_interval ) && __flag_is_insulin_open)
    {
        // Final Insulin close

        __led.draw_line(3, 7, 4, 0);
        __led.draw_line(3, 7, 5, 0);
        __led.render();

        __box.buzzerBeep(1000);

        __box.closeAllInsulin();
        __flag_is_insulin_open = 0;
    }

    if (__flag_is_insulin_open)
    {
        // While insulin is opened

        if (__insulin_time_interval.insulin_type == INSULIN_TYPE_DAY)
        {
            __led.draw_line(3, 7, 4, 0xffffff);
            __led.render();
        } 
        else if (__insulin_time_interval.insulin_type == INSULIN_TYPE_NIGHT)
        {
            __led.draw_line(3, 7, 5, 0xffffff);
            __led.render();
        }

        // Implement button
    }

    return 1;
}

void SmartMedicalBox::handleUiRequests()
{
    if (__ui.is_req_waiting())
    {
        std::string command = __ui.get_command();

        if (command.compare("SET_CLOCK_SECONDS") == 0)
        {
            __box.buzzerBeep(150);
            __ui.send_command("OK");
            int seconds = std::stoi(__ui.get_command());
            __rtc.setSeconds(seconds);
            __ui.send_command("OK");
        } 
        else if (command.compare("SET_CLOCK_MINUTES") == 0)
        {
            __box.buzzerBeep(150);
            __ui.send_command("OK");
            int minutes = std::stoi(__ui.get_command());
            __rtc.setMinutes(minutes);
            __ui.send_command("OK");
        }
        else if (command.compare("SET_CLOCK_HOUR") == 0)
        {
            __box.buzzerBeep(150);
            __ui.send_command("OK");
            int hour = std::stoi(__ui.get_command());
            __rtc.setHour(hour);
            __ui.send_command("OK");
        }
        else if (command.compare("SET_CLOCK_DAY_OF_WEEK") == 0)
        {
            __box.buzzerBeep(150);
            __ui.send_command("OK");
            int day_of_week = std::stoi(__ui.get_command());
            __rtc.setDayOfWeek(day_of_week);
            __ui.send_command("OK");
        }
        else if (command.compare("SET_CLOCK_DAY_OF_MONTH") == 0)
        {
            __box.buzzerBeep(150);
            __ui.send_command("OK");
            int day_of_month = std::stoi(__ui.get_command());
            __rtc.setDayOfMonth(day_of_month);
            __ui.send_command("OK");
        }
        else if (command.compare("SET_CLOCK_MONTH") == 0)
        {
            __box.buzzerBeep(150);
            __ui.send_command("OK");
            int month = std::stoi(__ui.get_command());
            __rtc.setMonth(month);
            __ui.send_command("OK");
        }
        else if (command.compare("SET_CLOCK_YEAR") == 0)
        {
            __box.buzzerBeep(150);
            __ui.send_command("OK");
            int year = std::stoi(__ui.get_command());
            __rtc.setYear(year);
            __ui.send_command("OK");
        }
        else if (command.compare("GET_CLOCK_SECONDS") == 0)
        {
            int seconds = __rtc.getSeconds();
            __ui.send_command(std::to_string(seconds));
        }
        else if (command.compare("GET_CLOCK_MINUTES") == 0)
        {
            int minutes = __rtc.getMinutes();
            __ui.send_command(std::to_string(minutes));
        }
        else if (command.compare("GET_CLOCK_HOUR") == 0)
        {
            int hour = __rtc.getHour24();
            __ui.send_command(std::to_string(hour));
        }
        else if (command.compare("GET_CLOCK_DAY_OF_WEEK") == 0)
        {
            int day_of_week = __rtc.getDayOfWeek();
            __ui.send_command(std::to_string(day_of_week));
        }
        else if (command.compare("GET_CLOCK_DAY_OF_MONTH") == 0)
        {
            int day_of_month = __rtc.getDayOfMonth();
            __ui.send_command(std::to_string(day_of_month));
        }
        else if (command.compare("GET_CLOCK_MONTH") == 0)
        {
            int month = __rtc.getMonth();
            __ui.send_command(std::to_string(month));
        }
        else if (command.compare("GET_CLOCK_YEAR") == 0)
        {
            int year = __rtc.getYear();
            __ui.send_command(std::to_string(year));
        }
        else if (command.compare("DISABLE_AUTOMATIC_BOX") == 0)
        {
            __automatic_box_opening = 0;
            __flag_is_box_open = 0;
            __flag_is_insulin_open = 0;
            __ui.send_command("OK");
            __box.closeAll();
            __box.closeAllInsulin();
            __led.clear();
            __led.render();
        }
        else if (command.compare("ENABLE_AUTOMATIC_BOX") == 0)
        {
            __automatic_box_opening = 1;
            __flag_is_box_open = 0;
            __flag_is_insulin_open = 0;
            __ui.send_command("OK");
            __box.closeAll();
            __box.closeAllInsulin();
            __led.clear();
            __led.render();
        }
        else if (command.compare("OPEN_BOX") == 0)
        {
            __ui.send_command("OK");
            int power_segment = std::stoi(__ui.get_command());
            __ui.send_command("OK");
            int servo_control_line = std::stoi(__ui.get_command());
            __ui.send_command("OK");

            __led.set_box_led(power_segment + 1, servo_control_line, 0xffffff);
            __led.render();

            __box.openBox(power_segment, servo_control_line);
        }
        else if (command.compare("CLOSE_BOX") == 0)
        {
            __box.buzzerBeep(150);
            __ui.send_command("OK");
            int power_segment = std::stoi(__ui.get_command());
            __ui.send_command("OK");
            int servo_control_line = std::stoi(__ui.get_command());
            __ui.send_command("OK");

            __led.set_box_led(power_segment + 1, servo_control_line, 0);
            __led.render();

            __box.closeBox(power_segment, servo_control_line);
        }
        else if (command.compare("CLOSE_BOX_ALL") == 0)
        {
            __ui.send_command("OK");
            __led.clear();
            __led.render();
            __box.closeAll();
        }
        else if (command.compare("OPEN_INSULIN_DAY") == 0)
        {
            __box.buzzerBeep(150);
            __ui.send_command("OK");
            __led.draw_line(3, 7, 4, 0xffffff);
            __led.render();
            __box.openInsulinDay();
        }
        else if (command.compare("CLOSE_INSULIN_DAY") == 0)
        {
            __box.buzzerBeep(150);
            __ui.send_command("OK");
            __led.draw_line(3, 7, 4, 0);
            __led.render();
            __box.closeInsulinDay();
        }
        else if (command.compare("OPEN_INSULIN_NIGHT") == 0)
        {
            __box.buzzerBeep(150);
            __ui.send_command("OK");
            __led.draw_line(3, 7, 5, 0xffffff);
            __led.render();
            __box.openInsulinNight();
        }
        else if (command.compare("CLOSE_INSULIN_NIGHT") == 0)
        {
            __box.buzzerBeep(150);
            __ui.send_command("OK");
            __led.draw_line(3, 7, 5, 0);
            __led.render();
            __box.closeInsulinNight();
        }
        else if (command.compare("GET_NORMAL_INTERVAL_BY_NAME") == 0)
        {
            __ui.send_command("OK");

            std::string interval_name = __ui.get_command();
            TimeInterval time_interval = __db.getTimeIntervalByIntervalName(interval_name);
            std::cout << interval_name << std::endl;
            __db.printTimeIntervalStruct(time_interval);

            __ui.send_command(std::to_string(time_interval.is_empty));

            if (!time_interval.is_empty)
            {
                // Active
                __ui.get_command(); // Handle "wathever"
                __ui.send_command(std::to_string(time_interval.active));

                // Time Interval Start Hour
                __ui.get_command(); // Handle "wathever"
                __ui.send_command(std::to_string(time_interval.time_start_h));

                // Time Interval Start Minute
                __ui.get_command(); // Handle "wathever"
                __ui.send_command(std::to_string(time_interval.time_start_m));

                // Time Interval End Hour
                __ui.get_command(); // Handle "wathever"
                __ui.send_command(std::to_string(time_interval.time_end_h));

                // Time Interval End Minute
                __ui.get_command(); // Handle "wathever"
                __ui.send_command(std::to_string(time_interval.time_end_m));
            }
        }
        else if (command.compare("SET_OR_UPDATE_NORMAL_INTERVAL") == 0)
        {
            __box.buzzerBeep(150);
            __ui.send_command("OK");

            std::string interval_name = __ui.get_command();
            __ui.send_command("OK");
            int time_start_h = std::stoi(__ui.get_command());
            __ui.send_command("OK");
            int time_start_m = std::stoi(__ui.get_command());
            __ui.send_command("OK");
            int time_end_h = std::stoi(__ui.get_command());
            __ui.send_command("OK");
            int time_end_m = std::stoi(__ui.get_command());
            __ui.send_command("OK");
            int active = std::stoi(__ui.get_command());
            __ui.send_command("OK");

            __db.createOrUpdateTimeInterval(interval_name, time_start_h, time_start_m, time_end_h, time_end_m, active);
        }
        else if (command.compare("GET_INSULIN_INTERVAL_BY_NUMBER_AND_TYPE") == 0)
        {
            __ui.send_command("OK");
            int interval_number = std::stoi(__ui.get_command());
            __ui.send_command("OK");
            int insulin_type = std::stoi(__ui.get_command());

            InsulinTimeInterval insulin_time_interval = __db.getInsulinTimeIntervalByNumberAndType(interval_number, insulin_type);

            __ui.send_command(std::to_string(insulin_time_interval.is_empty));

            if (!insulin_time_interval.is_empty)
            {
                // Insulin Time Interval Start Hour
                __ui.get_command(); // Handle "wathever"
                __ui.send_command(std::to_string(insulin_time_interval.time_start_h));

                // Insulin Time Interval Start Minute
                __ui.get_command(); // Handle "wathever"
                __ui.send_command(std::to_string(insulin_time_interval.time_start_m));

                // Insulin Time Interval End Hour
                __ui.get_command(); // Handle "wathever"
                __ui.send_command(std::to_string(insulin_time_interval.time_end_h));

                // Insulin Time Interval End Minute
                __ui.get_command(); // Handle "wathever"
                __ui.send_command(std::to_string(insulin_time_interval.time_end_m));
            }
        }
        else if (command.compare("SET_OR_UPDATE_INSULIN_INTERVAL") == 0)
        {
            __box.buzzerBeep(150);
            __ui.send_command("OK");

            int interval_number = std::stoi(__ui.get_command());
            __ui.send_command("OK");
            int insulin_type = std::stoi(__ui.get_command());
            __ui.send_command("OK");
            int time_start_h = std::stoi(__ui.get_command());
            __ui.send_command("OK");
            int time_start_m = std::stoi(__ui.get_command());
            __ui.send_command("OK");
            int time_end_h = std::stoi(__ui.get_command());
            __ui.send_command("OK");
            int time_end_m = std::stoi(__ui.get_command());
            __ui.send_command("OK");

            __db.createOrUpdateInsulinTimeInterval(interval_number, insulin_type, time_start_h, time_start_m, time_end_h, time_end_m);
        }
        else if (command.compare("DELETE_INSULIN_INTERVAL_BY_NUMBER_AND_TYPE") == 0)
        {
            __box.buzzerBeep(150);
            __ui.send_command("OK");
            int interval_number = std::stoi(__ui.get_command());
            __ui.send_command("OK");
            int insulin_type = std::stoi(__ui.get_command());
            __ui.send_command("OK");

            __db.deleteInsulinTimeIntervalByNumberAndType(interval_number, insulin_type);
        }

    }

}

void SmartMedicalBox::start()
{
    while (1)
    {
        handleUiRequests();

        if (__automatic_box_opening)
        {
            checkTimeAndOpenBox();
            checkTimeAndOpenInsulin();
        }
    }
}