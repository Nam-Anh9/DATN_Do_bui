#ifndef APP_DISPLAY_APP_DISPLAY_APP_H
#define APP_DISPLAY_APP_DISPLAY_APP_H

#include <Wire.h>
#include "SH1106Wire.h"
#include "images.h"
#include "app/measure_app/measure_app.h"
#include "soc/sens_reg.h"
#define CHANGE_DURATION 1000
#define POWER_PIN       25
#define BATTERY_PIN     4
typedef struct DISPLAY_APP_t DISPLAY_APP;
struct DISPLAY_APP_t{
    uint8_t wifi_status;
    uint8_t battery_status;
    uint8_t battert_percent;
    uint8_t screen_number;
    uint64_t reg_b;
    long counter;
};

enum BATTERY_STATE {
    BATTERY_CHARGING = 0,
    BATTERY_USING,
    BATTERY_NONE
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
void getBatteryStatus();
#endif