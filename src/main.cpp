#include <iostream>
#include "./smart_medical_box.h"
#include "./config/settings.h"

int main()
{
    Settings settings;
    SmartMedicalBox smb(settings);

    smb.start();

    return 0;
}