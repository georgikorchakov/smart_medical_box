#include "pca9685.h"

PCA9685::PCA9685(int address, int frequency)
    : m_i2c{ address }, m_frequency{ frequency }, 
      m_angle_closed{ 10 }, m_angle_open{ 170 },
      m_pin_states{ 0 }
{
    m_i2c.writeReg8(MODE1, 0);
    m_i2c.writeReg8(MODE2, (1 << OCH) | (1 << OUTDRV));

    changeFreq(m_frequency);
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

    m_frequency = frequency;
    int prescale_value = static_cast<int>(round( 25000000 / ( 4096 *  m_frequency) ) - 1);
    int mode1_register = m_i2c.readReg8(MODE1);

    // Enter Sleep mode to enable prescale of frequency
    m_i2c.writeReg8(MODE1, mode1_register | (1 << SLEEP) );
    delay(5);
    m_i2c.writeReg8(PRE_SCALE, prescale_value);

    mode1_register = m_i2c.readReg8(MODE1);

    m_i2c.writeReg8(MODE1, mode1_register & ~(1 << SLEEP) );
    delay(5);
    mode1_register = m_i2c.readReg8(MODE1);
    
    m_i2c.writeReg8(MODE1, mode1_register | (1 << RESTART) );
}

////////////////////////////////////////////////////////////
// Utils
////////////////////////////////////////////////////////////

bool PCA9685::isValidPinNumber(int pin_number)
{
    return (pin_number >= 0) && (pin_number <= 15);
}

int PCA9685::getPinOnLAddress(int pin_number)
{
    return LED0_ON_L + (pin_number * 4);
}

int PCA9685::getPinOnHAddress(int pin_number)
{
    return LED0_ON_H + (pin_number * 4);
}

int PCA9685::getPinOffLAddress(int pin_number)
{
    return LED0_OFF_L + (pin_number * 4);
}

int PCA9685::getPinOffHAddress(int pin_number)
{
    return LED0_OFF_H + (pin_number * 4);
}


////////////////////////////////////////////////////////////
// Basic Functionality
////////////////////////////////////////////////////////////

void PCA9685::digitalWrite(int pin_number, int value)
{
    if ( !isValidPinNumber(pin_number) ) return;
    m_pin_states[pin_number] = (value > 0); // 1 or 0

    if (value)
    {
        m_i2c.writeReg8(getPinOnLAddress(pin_number), 0);
        m_i2c.writeReg8(getPinOnHAddress(pin_number), (1 << 4));
        m_i2c.writeReg8(getPinOffLAddress(pin_number), 0);
        m_i2c.writeReg8(getPinOffHAddress(pin_number), 0);
    } 
    else 
    {
        m_i2c.writeReg8(getPinOnLAddress(pin_number), 0);
        m_i2c.writeReg8(getPinOnHAddress(pin_number), 0);
        m_i2c.writeReg8(getPinOffLAddress(pin_number), 0);
        m_i2c.writeReg8(getPinOffHAddress(pin_number), (1 << 4) );
    }
}

void PCA9685::pwmWrite(int pin_number, int value)
{
    if ( !isValidPinNumber(pin_number) ) return;
    m_pin_states[pin_number] = value;

    m_i2c.writeReg8(getPinOnLAddress(pin_number), 0);
    m_i2c.writeReg8(getPinOnHAddress(pin_number), 0);

    m_i2c.writeReg8(getPinOffLAddress(pin_number), value & 255);
    m_i2c.writeReg8(getPinOffHAddress(pin_number), (value & (255 << 8)) >> 8);
}

void PCA9685::setServo(int pin_number, int angle)
{
    if ( !isValidPinNumber(pin_number) ) return;

    m_pin_states[pin_number] = angle;

    m_i2c.writeReg8(getPinOnLAddress(pin_number), 0);
    m_i2c.writeReg8(getPinOnHAddress(pin_number), 0);

    // Servo Control 

    // Calculate time for one period
    int T = 1000 / m_frequency;

    int mili_second = 4096 / T;
    int step = (mili_second*2) / 180;
    int turn_off_value = (mili_second - 0.1*mili_second) + step * angle;

    m_i2c.writeReg8(getPinOffLAddress(pin_number), turn_off_value & 255);
    m_i2c.writeReg8(getPinOffHAddress(pin_number), (turn_off_value & (255 << 8)) >> 8 );
}

void PCA9685::setServoBorders(int angle_closed, int angle_open)
{
    m_angle_closed = angle_closed;
    m_angle_open = angle_open;
}

void PCA9685::openServo(int pin_number)
{
    setServo(pin_number, m_angle_open);
}

void PCA9685::closeServo(int pin_number)
{
    setServo(pin_number, m_angle_closed);
}

int PCA9685::isServoOpen(int pin_number)
{
    if (pin_number == -1) return -1;
    return m_pin_states[pin_number] >= m_angle_open;
}

int PCA9685::isServoClosed(int pin_number)
{
    if (pin_number == -1) return -1;
    return m_pin_states[pin_number] <= m_angle_closed;
}

int PCA9685::getCurrentAngleForServo(int pin_number)
{
    if (pin_number == -1) return -1;
    return m_pin_states[pin_number];
}
