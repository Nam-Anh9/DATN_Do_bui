#include <Arduino.h>
#include "app/display_app/display_app.h"
void display_task(void *pvParameters);

void setup() {
  // put your setup code here, to run once:
  xTaskCreate(
    display_task
    ,"Task1"
    , 128
    , NULL
    , 2
    , NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
}

void display_task(void *pvParameters)
{
  (void) pvParameters;
  OLED_init();
  for(;;)
  {
    drawFirstScreen();
  }
}