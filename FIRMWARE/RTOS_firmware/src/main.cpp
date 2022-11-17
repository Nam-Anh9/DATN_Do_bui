#include <Arduino.h>
#include "app/display_app/display_app.h"
TaskHandle_t Task1;
void display_task(void *pvParameters);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  xTaskCreatePinnedToCore(display_task,"Task1", 10000, NULL, 1, &Task1, 0);
  delay(500);
}



void display_task(void *pvParameters)
{
  (void) pvParameters;
  Serial.print("display_task is running on core");
  Serial.println(xPortGetCoreID());
  OLED_init();

  display_app.battert_percent = 100;
  display_app.battery_status = 1;
  display_app.wifi_status = 1;
  for(;;)
  {
    OLED_clearScreen();
    drawFirstScreen();
    OLED_display();

    vTaskDelay(10);
  }
}
void loop() {
  // put your main code here, to run repeatedly:
}