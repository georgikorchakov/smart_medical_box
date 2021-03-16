#include "./box_opening.h"

BoxOpening::BoxOpening(int pca9685_address, int servo_angle_closed, int servo_angle_open)
    : pca9685{ pca9685_address },
      m_box_state{ 0 },
      m_power_control_pin{ 0 },
      m_servo_pins{ 0 }
{
    pca9685.setServoBorders(servo_angle_closed, servo_angle_open );
}

////////////////////////////////////////////////////////////
// Utils
////////////////////////////////////////////////////////////
int BoxOpening::servoControlLineNameToNumber(std::string servo_control_line_name)
{
    if (servo_control_line_name == "morning")       { return 0; }
    if (servo_control_line_name == "midday")        { return 1; }
    if (servo_control_line_name == "evening")       { return 2; }
    if (servo_control_line_name == "night")         { return 3; }
    if (servo_control_line_name == "insulin_day")   { return 4; }
    if (servo_control_line_name == "insulin_night") { return 5; }
}

void BoxOpening::setPowerControlPin(int power_segment, int pca9685_pin)
{
    m_power_control_pin[power_segment] = pca9685_pin;
}

int BoxOpening::getPowerControlPin(int power_segment)
{
    return m_power_control_pin[power_segment];
}

void BoxOpening::setServoControlPin(int servo_control_line, int pca9685_pin)
{
    m_servo_pins[servo_control_line] = pca9685_pin;
}

int BoxOpening::getServoControlPin(int servo_control_line)
{
    return m_servo_pins[servo_control_line];
}

void BoxOpening::changePowerAndSynchronizeServoPwm(int power_segment)
{
    // Turn off all power pins
    for (int stop_power_segment = 0; stop_power_segment < 8; ++stop_power_segment)
    {
        pca9685.digitalWrite( getPowerControlPin( stop_power_segment ) , false);
    }

    for (int servo_control_line = 0; servo_control_line < 4; ++servo_control_line)
    {
        if (m_box_state[power_segment][servo_control_line] == 1)
        {
            pca9685.openServo( getServoControlPin( servo_control_line ) );
        } 
        else 
        {
            pca9685.closeServo( getServoControlPin( servo_control_line ) );
        }
    }

    delay(100);

    pca9685.digitalWrite( getPowerControlPin( power_segment ) , true);
    delay(300);
}

////////////////////////////////////////////////////////////
// Debugging and Monitoring Functions
////////////////////////////////////////////////////////////
int BoxOpening::checkForOpenBoxesByGivenPowerSegment(int power_segment)
{
    for (int servo_control_line = 0; servo_control_line < 4; ++servo_control_line)
    {
        if (m_box_state[power_segment][servo_control_line] == 1)
        {
            return 1;
        }
    }

    return 0;
}

void BoxOpening::printBox()
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
            std::cout << "|" << m_box_state[power_segment][servo_control_line] << "  ";
        }
        std::cout << "|\n";
    }

    std::cout << "-----------------------------\n";
}

////////////////////////////////////////////////////////////
// Box Interaction Functions
////////////////////////////////////////////////////////////
int BoxOpening::openBox(int power_segment, int servo_control_line)
{
    if (m_box_state[power_segment][servo_control_line] == 1)
    {
        return 1;
    }

    changePowerAndSynchronizeServoPwm(power_segment);

    pca9685.openServo( getServoControlPin( servo_control_line ) );
    delay(1000); // TODO: Delay to become measurement of current.
    // We don't stop servo power supply, because we want 
    // servo to resist if someone tries to close it by hands.

    m_box_state[power_segment][servo_control_line] = 1;

    return 1;
}

int BoxOpening::openBox(int power_segment, std::string servo_control_line_name)
{
    return openBox(power_segment, servoControlLineNameToNumber(servo_control_line_name));
}

int BoxOpening::closeBox(int power_segment, int servo_control_line)
{
    if (m_box_state[power_segment][servo_control_line] == 0)
    {
        return 1;
    }

    changePowerAndSynchronizeServoPwm(power_segment);

    pca9685.closeServo( getServoControlPin( servo_control_line ) );
    delay(1000); // TODO: Delay to become measurement of current.
    m_box_state[power_segment][servo_control_line] = 0;

    if (checkForOpenBoxesByGivenPowerSegment(power_segment) != 1)
    {
        for (int stop_power_segment = 0; stop_power_segment < 8; ++stop_power_segment)
        {
            pca9685.digitalWrite( getPowerControlPin( stop_power_segment ) , false);
        }
        
    }

    return 1;
}

int BoxOpening::closeBox(int power_segment, std::string servo_control_line_name)
{
    return closeBox(power_segment, servoControlLineNameToNumber(servo_control_line_name));
}

int BoxOpening::toggleBox(int power_segment, int time_interval)
{
    if (m_box_state[power_segment][time_interval] == 0)
    {
        return openBox(power_segment, time_interval);
    }
    else
    {
        return closeBox(power_segment, time_interval);
    }
}

int BoxOpening::toggleBox(int power_segment, std::string servo_control_line_name)
{
    return toggleBox(power_segment, servoControlLineNameToNumber(servo_control_line_name));
}

void BoxOpening::closeAll()
{
    for (int time_interval = 0; time_interval < 4; ++time_interval)
    {
        for (int power_segment = 0; power_segment < 7; ++power_segment)
        {
            if (m_box_state[power_segment][time_interval] == 1)
            {
                closeBox(power_segment, time_interval);
            }
        }
    }
}
