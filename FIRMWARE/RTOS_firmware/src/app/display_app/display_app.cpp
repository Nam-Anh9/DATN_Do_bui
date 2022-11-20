#include "display_app.h"
#include <string.h>
SH1106Wire display(0x3c, SDA, SCL);
DISPLAY_APP display_app = {
    .battert_percent = 0
};
void OLED_init(){
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
}

void OLED_drawWifiLogo(){
    DISPLAY_APP *pw = &display_app;
    if(pw->wifi_status)
    {
        display.drawXbm(0,0, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
    }
    else
    {
        display.setFont(ArialMT_Plain_16);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(0,0,"!");
    }
}
void OLED_drawBatteryPercent(){
    DISPLAY_APP *pw = &display_app;
    display.setFont(ArialMT_Plain_10);
    display.drawString(40, 2, String(pw->battert_percent));
}
void OLED_drawBatteryLogo(){
    DISPLAY_APP *pw = &display_app;
    if(pw->battery_status)
    {
        display.drawXbm(20, 0, Batery_Logo_width,Batery_Logo_height, Batery_Logo_bits);
    }
    else
    {
        display.setFont(ArialMT_Plain_10);
        display.drawString(20,0,"charge");
    }
}
void OLED_display(){
    display.display();
}
void OLED_clearScreen(){
    display.clear();
}
void drawTaskbar(){
    OLED_drawWifiLogo();
    OLED_drawBatteryPercent();
    OLED_drawBatteryLogo();
}
void drawFirstScreen(){
    drawTaskbar();
    MEASURE_APP *pw = &measure_app;
    String PM_10 = "PM10:";
    String PM_2_5 = "PM2.5:";
    PM_10 += pw->pmsData.PMS_10;
    PM_2_5 += pw->pmsData.PMS_2_5;
    display.setFont(ArialMT_Plain_16);

    // The coordinates define the left starting point of the text
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 20, "AQI:");
    display.drawString(0, 40, PM_2_5);
    display.drawString(55, 20, PM_10);
}
void drawSecondScreen(){
    drawTaskbar();
    display.setFont(ArialMT_Plain_16);

    // The coordinates define the left starting point of the text
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 20, "Temp:");
    display.drawString(0, 40, "Humi:");
}