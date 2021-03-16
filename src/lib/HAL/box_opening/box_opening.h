#ifndef BOX_OPENING_H
#define BOX_OPENING_H

#include <string>
#include <wiringPi.h>
#include "../../hardware/pca9685/pca9685.h"

class BoxOpening
{
private:
    PCA9685 pca9685;

    int m_box_state[7][4];
    int m_power_control_pin[8]; // Monday -> Sunday + Insulin
    int m_servo_pins[4]; // Morning, Midday, Evening and Night

    // Utils
    int servoControlLineNameToNumber(std::string servo_control_line_name);
    void changePowerAndSynchronizeServoPwm(int power_segment);

public:
    BoxOpening(int pca9685_address, 
               int servo_angle_closed=10, int servo_angle_open=170);
    
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
};

#endif // BOX_OPENING_H