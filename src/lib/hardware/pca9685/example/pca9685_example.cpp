// g++ -Wall -Wextra -Wsign-conversion  -std=c++17 ../../i2c/i2c.cpp ../pca9685.cpp pca9685_example.cpp -o pca9685_example -lwiringPi

#include "../pca9685.h"
#define SERVO_ADDRESS 0x40

#define MORNING_PWM 0
#define MIDDAY_PWM 1
#define EVENING_PWM 2
#define NIGHT_PWM 3
#define INSULIN_DAY_PWM 4
#define MORNING_NIGHT_PWM 5

#define POWER_THURSDAY 8
#define POWER_WEDNESDAY 9
#define POWER_TUESDAY 10
#define POWER_MONDAY 11
#define POWER_SUNDAY 12
#define POWER_SATURDAY 13
#define POWER_FRIDAY 14
#define POWER_INSULIN 15

int main()
{
    PCA9685 pwmDriver(SERVO_ADDRESS);
    pwmDriver.setServoBorders(30, 140);

    //////////////////////////////////////////////////////
    pwmDriver.digitalWrite(POWER_SUNDAY, true);

    pwmDriver.openServo(NIGHT_PWM);
    delay(1000);
    pwmDriver.closeServo(NIGHT_PWM);
    delay(1000);

    // Start and stop power
    pwmDriver.digitalWrite(POWER_SUNDAY, false);
}