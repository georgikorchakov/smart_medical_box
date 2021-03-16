#include "i2c.h"

I2C::I2C(int address)
    : m_address{ address }, m_fd{ wiringPiI2CSetup(address) }
{
    if (m_fd == -1)
    {
        std::cout << "Error in i2c file descriptor!\n";

        // This is for test purposes
        while (true);
    }
}

int I2C::writeReg8(int reg, int data)
{
    return wiringPiI2CWriteReg8(m_fd, reg, data);
}

int I2C::writeReg16(int reg, int data)
{
    return wiringPiI2CWriteReg16(m_fd, reg, data);
}

int I2C::readRegWithoutReStart(int reg)
{
    wiringPiI2CWrite (m_fd, reg);
    return wiringPiI2CRead(m_fd);
}

int I2C::readReg8(int reg)
{
    return wiringPiI2CReadReg8(m_fd, reg);
}

int I2C::readReg16(int reg)
{
    return wiringPiI2CReadReg16(m_fd, reg);
}