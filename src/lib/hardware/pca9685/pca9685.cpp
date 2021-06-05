#include "pca9685.h"

PCA9685::PCA9685(int address, int frequency)
    : __i2c{ address }, __frequency{ frequency }, 
      __angle_closed{ 10 }, __angle_open{ 170 },
      __pin_states{ 0 }
{
    __i2c.writeReg8(MODE1, 0);
    __i2c.writeReg8(MODE2, (1 << OCH) | (1 << OUTDRV));

    changeFreq(__frequency);
}

void PCA9685::changeFreq(int frequency)
{
    if (frequency < 24)
    {
        frequency = 24;
    }
    else if (frequency > 1526)
    {
        frequency = 1526;
    }

    __frequency = frequency;
    int prescale_value = static_cast<int>(round( 25000000 / ( 4096 *  __frequency) ) - 1);
    int mode1_register = __i2c.readReg8(MODE1);

    // Enter Sleep mode to enable prescale of frequency
    __i2c.writeReg8(MODE1, mode1_register | (1 << SLEEP) );
    delay(5);
    __i2c.writeReg8(PRE_SCALE, prescale_value);

    mode1_register = __i2c.readReg8(MODE1);

    __i2c.writeReg8(MODE1, mode1_register & ~(1 << SLEEP) );
    delay(5);
    mode1_register = __i2c.readReg8(MODE1);
    
    __i2c.writeReg8(MODE1, mode1_register | (1 << RESTART) );
}

////////////////////////////////////////////////////////////
// Utils
////////////////////////////////////////////////////////////

bool PCA9685::__isValidPinNumber(int pin_number)
{
    return (pin_number >= 0) && (pin_number <= 15);
}

int PCA9685::__getPinOnLAddress(int pin_number)
{
    return LED0_ON_L + (pin_number * 4);
}

int PCA9685::__getPinOnHAddress(int pin_number)
{
    return LED0_ON_H + (pin_number * 4);
}

int PCA9685::__getPinOffLAddress(int pin_number)
{
    return LED0_OFF_L + (pin_number * 4);
}

int PCA9685::__getPinOffHAddress(int pin_number)
{
    return LED0_OFF_H + (pin_number * 4);
}


////////////////////////////////////////////////////////////
// Basic Functionality
////////////////////////////////////////////////////////////

void PCA9685::digitalWrite(int pin_number, int value)
{
    if ( !__isValidPinNumber(pin_number) ) return;
    __pin_states[pin_number] = (value > 0); // 1 or 0

    if (value)
    {
        __i2c.writeReg8(__getPinOnLAddress(pin_number), 0);
        __i2c.writeReg8(__getPinOnHAddress(pin_number), (1 << 4));
        __i2c.writeReg8(__getPinOffLAddress(pin_number), 0);
        __i2c.writeReg8(__getPinOffHAddress(pin_number), 0);
    } 
    else 
    {
        __i2c.writeReg8(__getPinOnLAddress(pin_number), 0);
        __i2c.writeReg8(__getPinOnHAddress(pin_number), 0);
        __i2c.writeReg8(__getPinOffLAddress(pin_number), 0);
        __i2c.writeReg8(__getPinOffHAddress(pin_number), (1 << 4) );
    }
}

void PCA9685::pwmWrite(int pin_number, int value)
{
    if ( !__isValidPinNumber(pin_number) ) return;
    __pin_states[pin_number] = value;

    __i2c.writeReg8(__getPinOnLAddress(pin_number), 0);
    __i2c.writeReg8(__getPinOnHAddress(pin_number), 0);

    __i2c.writeReg8(__getPinOffLAddress(pin_number), value & 255);
    __i2c.writeReg8(__getPinOffHAddress(pin_number), (value & (255 << 8)) >> 8);
}


////////////////////////////////////////////////////////////
// Servo Control Functionality
////////////////////////////////////////////////////////////

void PCA9685::setServo(int pin_number, int angle)
{
    if ( !__isValidPinNumber(pin_number) ) return;

    __pin_states[pin_number] = angle;

    __i2c.writeReg8(__getPinOnLAddress(pin_number), 0);
    __i2c.writeReg8(__getPinOnHAddress(pin_number), 0);

    // Servo Control 

    // Calculate time for one period
    int T = 1000 / __frequency;

    int mili_second = 4096 / T;
    int step = (mili_second*2) / 180;
    int turn_off_value = (mili_second - 0.1*mili_second) + step * angle;

    __i2c.writeReg8(__getPinOffLAddress(pin_number), turn_off_value & 255);
    __i2c.writeReg8(__getPinOffHAddress(pin_number), (turn_off_value & (255 << 8)) >> 8 );
}

void PCA9685::setServoBorders(int angle_closed, int angle_open)
{
    __angle_closed = angle_closed;
    __angle_open = angle_open;
}

void PCA9685::openServo(int pin_number)
{
    setServo(pin_number, __angle_open);
}

void PCA9685::closeServo(int pin_number)
{
    setServo(pin_number, __angle_closed);
}

int PCA9685::isServoOpen(int pin_number)
{
    if (pin_number == -1) return -1;
    return __pin_states[pin_number] >= __angle_open;
}

int PCA9685::isServoClosed(int pin_number)
{
    if (pin_number == -1) return -1;
    return __pin_states[pin_number] <= __angle_closed;
}

int PCA9685::getCurrentAngleForServo(int pin_number)
{
    if (pin_number == -1) return -1;
    return __pin_states[pin_number];
}
