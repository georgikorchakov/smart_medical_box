#ifndef I2C_H
#define I2C_H

#include <iostream>
#include <wiringPiI2C.h>

class I2C
{
private:
    const int m_address;
    int m_fd;

public:
    I2C(int address);

    int writeReg8(int reg, int data);
    int writeReg16(int reg, int data);

    int readReg8(int reg);
    int readReg16(int reg);
    int readRegWithoutReStart(int reg);
};

#endif // I2C_H