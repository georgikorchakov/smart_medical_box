#ifndef BOX_OPENING_H
#define BOX_OPENING_H

#include <string>
#include <wiringPi.h>
#include "../../hardware/pca9685/pca9685.h"

#define POWER_PIN_MONDAY 0
#define POWER_PIN_TUESDAY 1
#define POWER_PIN_WEDNESDAY 2
#define POWER_PIN_THURSDAY 3
#define POWER_PIN_FRIDAY 4
#define POWER_PIN_SATURDAY 5
#define POWER_PIN_SUNDAY 6
#define POWER_PIN_INSULIN 7

#define SERVO_PIN_MORNING 0
#define SERVO_PIN_MIDDAY 1
#define SERVO_PIN_EVENING 2
#define SERVO_PIN_NIGHT 3
#define SERVO_PIN_INSULIN_DAY 4
#define SERVO_PIN_INSULIN_NIGHT 5

class Box
{
private:
    PCA9685 __pca9685;

    int __box_state[7][4];
    int __power_control_pin[8]; // Monday -> Sunday + Insulin
    int __servo_pins[6]; // Morning, Midday, Evening, Night, Insulin Day and Insulin Night 
    int __buzzer_pin = 0;
    int __button_pin = 0;
    int __button_led_pin = 0;

    int __insulin_day_state = 0;
    int __insulin_night_state = 0;

    // Utils
    void __changePowerAndSynchronizeServoPwm(int power_segment);

public:
    Box(int pca9685_address, 
               int servo_angle_closed=10, int servo_angle_open=170);

    int servoControlLineNameToNumber(std::string servo_control_line_name);
    
    // Set / Get 
    void setPowerControlPin(int power_segment, int pca9685_pin);
    int getPowerControlPin(int power_segment);
    void setServoControlPin(int servo_control_line, int pca9685_pin);
    int getServoControlPin(int servo_control_line);

    // Debugging and Monitoring Functions
    int checkForOpenBoxesByGivenPowerSegment(int power_segment);
    void printBox();

    // Box Interaction Functions
    int openBox(int power_segment, int servo_control_line);
    int openBox(int power_segment, std::string servo_control_line_name);
    int closeBox(int power_segment, int servo_control_line);
    int closeBox(int power_segment, std::string servo_control_line_name);
    int toggleBox(int power_segment, int servo_control_line);
    int toggleBox(int power_segment, std::string servo_control_line_name);
    void closeAll();

    // Insulin Control Functions
    int openInsulinDay();
    int openInsulinNight();
    int closeInsulinDay();
    int closeInsulinNight();
    int toggleInsulinDay();
    int toggleInsulinNight();
    int closeAllInsulin();

    // Buzzer 
    void buzzerBeep(unsigned int ms);
    void setBuzzerPin(int buzzer_pin);

    // Button 
    void setButtonPin(int button_pin);
    int readButton();

    void setButtonLedPin(int button_led_pin);
    void turnButtonLedOn();
    void turnButtonLedOff();

};

#endif // BOX_OPENING_H