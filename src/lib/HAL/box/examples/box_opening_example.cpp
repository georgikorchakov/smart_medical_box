// g++ -Wall -Weffc++ -Wextra -Wsign-conversion  -std=c++17 ../../../hardware/pca9685/pca9685.cpp  ../../../hardware/i2c/i2c.cpp box_opening_example.cpp ../box_opening.cpp -o box_opening_example -lwiringPi
#include <iostream>
#include <wiringPi.h>
#include "../../../../config/settings.h"
#include "../box.h"

int main()
{
    Box box(settings::pca9685_address, 30, 140);

    box.setPowerControlPin(0, settings::power_monday);
    box.setPowerControlPin(1, settings::power_tuesday);
    box.setPowerControlPin(2, settings::power_wednesday);
    box.setPowerControlPin(3, settings::power_thursday);
    box.setPowerControlPin(4, settings::power_friday);
    box.setPowerControlPin(5, settings::power_saturday);
    box.setPowerControlPin(6, settings::power_sunday);
    box.setPowerControlPin(7, settings::power_insulin);

    box.setServoControlPin(0, settings::morning_pwm);
    box.setServoControlPin(1, settings::midday_pwm);
    box.setServoControlPin(2, settings::evening_pwm);
    box.setServoControlPin(3, settings::night_pwm);
    box.setServoControlPin(4, settings::insulin_day_pwm);
    box.setServoControlPin(5, settings::insulin_night_pwm);
    
    for (int interval = 0; interval < 4; interval++)
    {   
        for (int day = 0; day < 7; ++day)
        {
            box.openBox(day, interval); //Morning
            //box.closeBox(day, interval); //Morning
            box.printBox();
        }
    }

    delay(3000);

    box.closeAll();
    

    return 0;
}