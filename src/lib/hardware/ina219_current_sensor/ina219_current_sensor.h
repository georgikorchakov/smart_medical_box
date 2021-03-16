#ifndef INA219_CURRENT_SENSOR_H
#define INA219_CURRENT_SENSOR_H

#include <iostream>
#include "../i2c/i2c.h"
#include <wiringPi.h>

// All-register reset, settings for bus voltage range, 
// PGA Gain, ADC resolution/averaging. (R/W)
#define CONFIGURATON_REGISTER  0x00 

// Shunt voltage measurement data. (R)
#define SHUNT_VOLTAGE_REGISTER 0x01

// Bus voltage measurement data. (R)
#define BUS_VOLTAGE_REGISTER   0x02 

// Power measurement data. (R)
#define POWER_REGISTER         0x03

// Contains the value of the current 
// flowing through the shunt resistor. (R)
#define CURRENT_REGISTER       0x04

// Sets full-scale range and LSB of current and 
// power measurements. Overall system calibration. (R/W)
#define CALIBRATION_REGISTER   0x05


class INA219CurrentSensor
{
private:
    I2C m_i2c;

public:

};


#endif // INA219_CURRENT_SENSOR_H