// g++ -Wall -Wextra -Wsign-conversion  -std=c++17 ./lib/peripherals/peripherals.cpp peripheral_test.cpp -o peripheral_test -lwiringPi

#include <iostream>
#include "./lib/peripherals/peripherals.h"

int main()
{
    Peripherals peripherals(2, 3, 27);

    while (true)
    {
        if (peripherals.readButton())
        {
            peripherals.setButtonLedOn();
            peripherals.setBuzzerOn();
        }
        else
        {
            peripherals.setButtonLedOff();
            peripherals.setBuzzerOff();
        }
    }

    return 0;
}