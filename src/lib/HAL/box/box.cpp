#include "./box.h"

Box::Box(int __pca9685_address, int servo_angle_closed, int servo_angle_open)
    : __pca9685{ __pca9685_address },
      __box_state{ 0 },
      __power_control_pin{ 0 },
      __servo_pins{ 0 }
{
    wiringPiSetup();

    __pca9685.setServoBorders(servo_angle_closed, servo_angle_open );
}

////////////////////////////////////////////////////////////
// Utils
////////////////////////////////////////////////////////////
int Box::servoControlLineNameToNumber(std::string servo_control_line_name)
{
    if (servo_control_line_name == "morning")       { return 0; }
    if (servo_control_line_name == "midday")        { return 1; }
    if (servo_control_line_name == "evening")       { return 2; }
    if (servo_control_line_name == "night")         { return 3; }
    if (servo_control_line_name == "insulin_day")   { return 4; }
    if (servo_control_line_name == "insulin_night") { return 5; }
}

void Box::setPowerControlPin(int power_segment, int pca9685_pin)
{
    __power_control_pin[power_segment] = pca9685_pin;
}

int Box::getPowerControlPin(int power_segment)
{
    return __power_control_pin[power_segment];
}

void Box::setServoControlPin(int servo_control_line, int pca9685_pin)
{
    __servo_pins[servo_control_line] = pca9685_pin;
}

int Box::getServoControlPin(int servo_control_line)
{
    return __servo_pins[servo_control_line];
}

void Box::__changePowerAndSynchronizeServoPwm(int power_segment)
{
    // Turn off all power pins
    for (int stop_power_segment = 0; stop_power_segment < 7; ++stop_power_segment)
    {
        __pca9685.digitalWrite( getPowerControlPin( stop_power_segment ) , false);
    }

    for (int servo_control_line = 0; servo_control_line < 4; ++servo_control_line)
    {
        if (__box_state[power_segment][servo_control_line] == 1)
        {
            __pca9685.openServo( getServoControlPin( servo_control_line ) );
        } 
        else 
        {
            __pca9685.closeServo( getServoControlPin( servo_control_line ) );
        }
    }

    delay(100);

    __pca9685.digitalWrite( getPowerControlPin( power_segment ) , true);
    delay(300);
}

////////////////////////////////////////////////////////////
// Debugging and Monitoring Functions
////////////////////////////////////////////////////////////
int Box::checkForOpenBoxesByGivenPowerSegment(int power_segment)
{
    for (int servo_control_line = 0; servo_control_line < 4; ++servo_control_line)
    {
        if (__box_state[power_segment][servo_control_line] == 1)
        {
            return 1;
        }
    }

    return 0;
}

void Box::printBox()
{
    std::cout << "-----------------------------\n";
    std::cout << "|   * Smart Medical Box *   |\n";
    std::cout << "-----------------------------\n";
    std::cout << "|Mo |Tu |We |Th |Fr |Sa |Su |\n";

    for (int servo_control_line = 0; servo_control_line < 4; ++servo_control_line)
    {
        std::cout << "-----------------------------\n";
        for (int power_segment = 0; power_segment < 7; ++power_segment)
        {
            std::cout << "|" << __box_state[power_segment][servo_control_line] << "  ";
        }
        std::cout << "|\n";
    }

    std::cout << "-----------------------------\n";
}

////////////////////////////////////////////////////////////
// Box Interaction Functions
////////////////////////////////////////////////////////////
int Box::openBox(int power_segment, int servo_control_line)
{
    if (__box_state[power_segment][servo_control_line] == 1)
    {
        return 0;
    }

    __changePowerAndSynchronizeServoPwm(power_segment);

    __pca9685.openServo( getServoControlPin( servo_control_line ) );
    delay(1000); // TODO: Delay to become measurement of current.
    // We don't stop servo power supply, because we want 
    // servo to resist if someone tries to close it by hands.

    __box_state[power_segment][servo_control_line] = 1;

    return 0;
}

int Box::openBox(int power_segment, std::string servo_control_line_name)
{
    return openBox(power_segment, servoControlLineNameToNumber(servo_control_line_name));
}

int Box::closeBox(int power_segment, int servo_control_line)
{
    if (__box_state[power_segment][servo_control_line] == 0)
    {
        return 0;
    }

    __changePowerAndSynchronizeServoPwm(power_segment);

    __pca9685.closeServo( getServoControlPin( servo_control_line ) );
    delay(1000); // TODO: Delay to become measurement of current.
    __box_state[power_segment][servo_control_line] = 0;

    if (checkForOpenBoxesByGivenPowerSegment(power_segment) != 1)
    {
        for (int stop_power_segment = 0; stop_power_segment < 7; ++stop_power_segment)
        {
            __pca9685.digitalWrite( getPowerControlPin( stop_power_segment ) , false);
        }
        
    }

    return 0;
}

int Box::closeBox(int power_segment, std::string servo_control_line_name)
{
    return closeBox(power_segment, servoControlLineNameToNumber(servo_control_line_name));
}

int Box::toggleBox(int power_segment, int time_interval)
{
    if (__box_state[power_segment][time_interval] == 0)
    {
        return openBox(power_segment, time_interval);
    }
    else
    {
        return closeBox(power_segment, time_interval);
    }
}

int Box::toggleBox(int power_segment, std::string servo_control_line_name)
{
    return toggleBox(power_segment, servoControlLineNameToNumber(servo_control_line_name));
}

void Box::closeAll()
{
    for (int time_interval = 0; time_interval < 4; ++time_interval)
    {
        for (int power_segment = 0; power_segment < 7; ++power_segment)
        {
            if (__box_state[power_segment][time_interval] == 1)
            {
                closeBox(power_segment, time_interval);
            }
        }
    }
}

////////////////////////////////////////////////////////////
// Insulin Control Functions
////////////////////////////////////////////////////////////
int Box::openInsulinDay()
{
    if (__insulin_day_state == 1)
    {
        return 0;
    }

    __pca9685.digitalWrite( getPowerControlPin( 7 ) , true); // Insulin Power

    for (int i = 0; i < 120; i = i + 10)
    {
        __pca9685.setServo( getServoControlPin( 4 ), i );
        delay(35);
    }

    //__pca9685.setServo( getServoControlPin( 4 ), 120 ); // Insulin Day
    delay(1000); // TODO: Delay to become measurement of current.
    // We don't stop servo power supply, because we want 
    // servo to resist if someone tries to close it by hands.

    __insulin_day_state = 1;
    return 0;
}

int Box::openInsulinNight()
{
    if (__insulin_night_state == 1)
    {
        return 0;
    }

    __pca9685.digitalWrite( getPowerControlPin( 7 ) , true); // Insulin Power

    for (int i = 0; i < 120; i = i + 10)
    {
        __pca9685.setServo( getServoControlPin( 5 ), i );
        delay(35);
    }

    //__pca9685.setServo( getServoControlPin( 5 ), 120 ); // Insulin Night
    delay(1000); // TODO: Delay to become measurement of current.
    // We don't stop servo power supply, because we want 
    // servo to resist if someone tries to close it by hands.

    __insulin_night_state = 1;
    return 0;
}

int Box::closeInsulinDay()
{
    if (__insulin_day_state == 0)
    {
        return 0;
    }

    __pca9685.digitalWrite( getPowerControlPin( 7 ) , true); // Insulin Power

    for (int i = 120; i > 0; i = i - 10)
    {
        __pca9685.setServo( getServoControlPin( 4 ), i );
        delay(35);
    }

    //__pca9685.setServo( getServoControlPin( 4 ), 0 ); // Insulin Day
    delay(1000); // TODO: Delay to become measurement of current.
    // We don't stop servo power supply, because we want 
    // servo to resist if someone tries to close it by hands.

    __insulin_day_state = 0;

    if ((__insulin_day_state || __insulin_night_state) == 0)
    {
        __pca9685.digitalWrite( getPowerControlPin( 7 ) , false); // Insulin Power
    }

    return 0;
}

int Box::closeInsulinNight()
{
    if (__insulin_night_state == 0)
    {
        return 0;
    }

    __pca9685.digitalWrite( getPowerControlPin( 7 ) , true); // Insulin Power


    for (int i = 120; i > 0; i = i - 10)
    {
        __pca9685.setServo( getServoControlPin( 5 ), i );
        delay(35);
    }

    //__pca9685.setServo( getServoControlPin( 5 ), 0 ); // Insulin Night
    delay(1000); // TODO: Delay to become measurement of current.
    // We don't stop servo power supply, because we want 
    // servo to resist if someone tries to close it by hands.

    __insulin_night_state = 0;

    if ((__insulin_day_state || __insulin_night_state) == 0)
    {
        __pca9685.digitalWrite( getPowerControlPin( 7 ) , false); // Insulin Power
    }

    return 0;
}

int Box::toggleInsulinDay()
{
    if (__insulin_day_state == 1)
    {
        return closeInsulinDay();
    }
    return openInsulinDay();
}

int Box::toggleInsulinNight()
{
    if (__insulin_night_state == 1)
    {
        return closeInsulinNight();
    }
    return openInsulinNight();
}

int Box::closeAllInsulin()
{
    std::cout << "closeAllInsulin()\n";
    std::cout << "day: " << __insulin_day_state << "\n";
    std::cout << "night: " << __insulin_night_state << "\n";

    closeInsulinDay();
    closeInsulinNight();

    return 0;
}


////////////////////////////////////////////////////////////
// Buzzer Control Functions
//////////////////////////////////////////////////////////// 

void Box::buzzerBeep(unsigned int ms)
{
    __pca9685.digitalWrite(__buzzer_pin, true);
    delay(ms);
    __pca9685.digitalWrite(__buzzer_pin, false);
}

void Box::setBuzzerPin(int buzzer_pin)
{
    __buzzer_pin = buzzer_pin;
}


////////////////////////////////////////////////////////////
// Button and Button Led Functions
//////////////////////////////////////////////////////////// 

void Box::setButtonPin(int button_pin)
{
    __button_pin = button_pin;
    pinMode(__button_pin, INPUT);
}

int Box::readButton()
{
    return digitalRead(__button_pin);
}

void Box::setButtonLedPin(int button_led_pin)
{
    __button_led_pin = button_led_pin;
    pinMode(__button_led_pin, OUTPUT);
    digitalWrite(__button_led_pin, LOW);
}

void Box::turnButtonLedOn()
{
    digitalWrite(__button_led_pin, HIGH);
}

void Box::turnButtonLedOff()
{
    digitalWrite(__button_led_pin, LOW);
}
