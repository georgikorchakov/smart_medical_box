#ifndef SMART_MEDICL_BOX_H
#define SMART_MEDICL_BOX_H

#include <iostream>
#include <cstring>
#include <wiringPi.h>

#include "./config/settings.h"
#include "./lib/HAL/box_opening/box_opening.h"
#include "./lib/hardware/pcf2129_rtc/pcf2129_rtc.h"
#include "./lib/HAL/database/database.h"


class SmartMedicalBox
{
private:
    BoxOpening __box;
    PCF2129RTC __rtc;
    Database __db;

    TimeInterval __time_interval;

    int __flag_is_box_open = 0;

public:
    SmartMedicalBox(Settings settings);

    int checkTimeAndOpenBox();
};



#endif // SMART_MEDICL_BOX_H