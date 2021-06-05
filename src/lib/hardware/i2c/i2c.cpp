#include "i2c.h"

I2C::I2C(int address)
    : __fd{ wiringPiI2CSetup(address) }
{
    __address = address;
    if (__fd == -1)
    {
        std::cout << "Error in i2c file descriptor!\n";
    }
}

int I2C::writeReg8(int reg, int data)
{
    return wiringPiI2CWriteReg8(__fd, reg, data);
}

int I2C::writeReg16(int reg, int data)
{
    return wiringPiI2CWriteReg16(__fd, reg, data);
}

int I2C::readReg8(int reg)
{
    return wiringPiI2CReadReg8(__fd, reg);
}

int I2C::readReg16(int reg)
{
    return wiringPiI2CReadReg16(__fd, reg);
}

int I2C::readRegWithoutReStart(int reg)
{
    wiringPiI2CWrite (__fd, reg);
    return wiringPiI2CRead(__fd);
}