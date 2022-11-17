#include "display_app.h"

SH1106Wire display(0x3c, SDA, SCL);

void OLED_init(){
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
}

void OLED_drawWifiLogo(){
    DISPLAY_APP *pw = &display_app;
    if(pw->wifi_status)
    {
        display.setFont(ArialMT_Plain_16);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(0,0,"...");
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
    display.setFont(ArialMT_Plain_16);

    // The coordinates define the left starting point of the text
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 20, "AQI:");
    display.drawString(0, 40, "PM2.5:");
    display.drawString(55, 20, "PM10:");
}
void drawSecondScreen(){
    drawTaskbar();
    display.setFont(ArialMT_Plain_16);

    // The coordinates define the left starting point of the text
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 20, "Temp:");
    display.drawString(0, 40, "Humi:");
}