// g++ -Wall -Weffc++ -Wextra -Wsign-conversion  -std=c++17 ./lib/pca9685_servo/pca9685_servo.cpp ./lib/shift_register/shift_register.cpp ./lib/i2c/i2c.cpp servo_test.cpp -o servo_test -lwiringPi

#include <iostream>
#include <wiringPi.h>

#include "./lib/pca9685_servo/pca9685_servo.h"
#include "./lib/shift_register/shift_register.h"

#define SERVO_ADDRESS 0x40
#define SDA 21
#define SCK 22
#define LATCH 23

int main()
{
    ShiftRegister power(SDA, SCK, LATCH);
    PCA9685Servo  servo(SERVO_ADDRESS);
    servo.setBorders(10, 170);

    for (int power_segment = 0; power_segment < 7; ++power_segment)
    {
        for (int interval = 0; interval < 4; ++interval)
        {
            power.setHighOnly(power_segment);

            servo.open(interval);
            delay(1000);
            servo.close(interval);
            delay(1000);

            power.setHighOnly(7);   // Insulin servo power

            if (interval == 3)
            {
                servo.open(5);      // Insulin Night
                delay(1000);
                servo.close(5);
            }
            else
            {
                servo.open(4);      // Insulin Day
                delay(1000);
                servo.close(4);
            }
            
            delay(1000);
        }
    }

    power.setAllLow();

    return 0;
}