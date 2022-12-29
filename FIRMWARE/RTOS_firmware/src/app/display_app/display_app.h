#ifndef APP_DISPLAY_APP_DISPLAY_APP_H
#define APP_DISPLAY_APP_DISPLAY_APP_H

#include <Wire.h>
#include "SH1106Wire.h"
#include "images.h"
#include "app/measure_app/measure_app.h"
#define CHANGE_DURATION 1000

typedef struct DISPLAY_APP_t DISPLAY_APP;
struct DISPLAY_APP_t{
    uint8_t wifi_status;
    uint8_t battery_status;
    uint8_t battert_percent;
    uint8_t screen_number;
    long counter;
};

extern DISPLAY_APP display_app;

void OLED_init();
void OLED_drawWifiLogo();
void OLED_drawBatteryPercent();
void OLED_drawBatteryLogo();
void drawTaskbar();
void drawFirstScreen();
void drawSecondScreen();
void OLED_display();
void OLED_clearScreen();
void swtichScreen();

#endif