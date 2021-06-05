#ifndef SMART_MEDICL_BOX_H
#define SMART_MEDICL_BOX_H

#include <iostream>
#include <cstring>
#include <wiringPi.h>

#include "./config/settings.h"
#include "./lib/HAL/box/box.h"
#include "./lib/hardware/pcf2129_rtc/pcf2129_rtc.h"
#include "./lib/database/database.h"
#include "./lib/ui_communication/ui_communication.h"
#include "./lib/hardware/ws2812b/ws2812b.h"


class SmartMedicalBox
{
private:
    Box __box;
    PCF2129RTC __rtc;
    Database __db;
    UICommunication __ui;
    WS2812B __led;
    
    TimeInterval __time_interval;
    InsulinTimeInterval __insulin_time_interval;

    int __flag_is_box_open = 0;
    int __flag_is_insulin_open = 0;
    int __automatic_box_opening = 1;

public:
    SmartMedicalBox(Settings settings);

    int checkTimeAndOpenBox();
    int checkTimeAndOpenInsulin();
    void handleUiRequests();
    void start();
};



#endif // SMART_MEDICL_BOX_H