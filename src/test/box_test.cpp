// g++ -Wall -Weffc++ -Wextra -Wsign-conversion  -std=c++17 ./lib/pca9685_servo/pca9685_servo.cpp ./lib/shift_register/shift_register.cpp ./lib/i2c/i2c.cpp box_test.cpp ./lib/box_opening/box_opening.cpp -o box_test -lwiringPi
#include <iostream>
#include <wiringPi.h>
#include "./config/settings.h"
#include "./lib/box_opening/box_opening.h"

int main()
{
    BoxOpening box(settings::sda, settings::sck, 
                   settings::latch, settings::servo_address, 
                   settings::servo_angle_closed, settings::servo_angle_open);
    
    for (int interval = 0; interval < 4; interval++)
    {
        for (int day = 1; day < 8; ++day)
        {
            box.openBox(day, interval); //Morning
            box.printBox();
        }
    }

    delay(3000);

    box.closeAll();
    box.printBox();

    return 0;
}