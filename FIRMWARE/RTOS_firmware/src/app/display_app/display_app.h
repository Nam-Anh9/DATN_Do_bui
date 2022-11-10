#ifndef DISPLAY_APP_h
#define DISPLAY_APP_h

//#include <Arduino.h>
#include <Wire.h>
#include "SH1106Wire.h"
#include "images.h"
#define CHANGE_DURATION 3000


class DISPLAY_APP{
    public:
    uint8_t wifi_status;
    uint8_t battery_status;
    uint8_t battert_percent;
};

void OLED_init();
void OLED_drawWifiLogo();
void OLED_drawBatteryPercent();
void OLED_drawBatteryLogo();
void drawTaskbar();
void drawFirstScreen();
void drawSecondScreen();
void swtichScreen();

#endif