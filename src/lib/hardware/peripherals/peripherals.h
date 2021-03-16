#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include <iostream>
#include <wiringPi.h>

class Peripherals
{
private:
    int m_button_pin;
    int m_button_led_pin;
    int m_buzzer_pin;

public:
    Peripherals(int button_pin, int button_led_pin, int buzzer_pin);
    
    int readButton();
    void setButtonLedOn();
    void setButtonLedOff();
    void setBuzzerOn();
    void setBuzzerOff();
};

#endif // PERIPHERALS_H