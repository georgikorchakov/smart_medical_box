#include "./peripherals.h"


Peripherals::Peripherals(int button_pin, int button_led_pin, int buzzer_pin)
    : m_button_pin{ button_pin }, m_button_led_pin{ button_led_pin }, m_buzzer_pin{ buzzer_pin }
{
    wiringPiSetup();

    pinMode(m_button_pin, INPUT);
    pinMode(m_button_led_pin, OUTPUT);
    pinMode(m_buzzer_pin, OUTPUT);
}

int Peripherals::readButton()
{
    return digitalRead(m_button_pin);
}

void Peripherals::setButtonLedOn()
{
    digitalWrite(m_button_led_pin, HIGH);
}

void Peripherals::setButtonLedOff()
{
    digitalWrite(m_button_led_pin, LOW);
}

void Peripherals::setBuzzerOn()
{
    digitalWrite(m_buzzer_pin, HIGH);
}

void Peripherals::setBuzzerOff()
{
    digitalWrite(m_buzzer_pin, LOW);
}
