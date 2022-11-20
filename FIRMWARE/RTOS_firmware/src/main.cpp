#include <Arduino.h>
#include <WiFi.h>
#include "app/display_app/display_app.h"
#include "app/measure_app/measure_app.h"

TaskHandle_t Task1;
TaskHandle_t Task2;
SemaphoreHandle_t xSerialSemaphore;
void display_task(void *pvParameters);
void measure_task(void *pvParameters);

const char* ssid = "Song Quynh";
const char* password = "songquynh25042112";
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  PMS_SERIAL.begin(9600);
  //Serial.print("Connecting to wifi");
  //Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    display_app.wifi_status = 0;
  }
  // Serial.println("");
  // Serial.println("WiFi connected.");
  // Serial.println("IP address: ");
  // Serial.println(WiFi.localIP());
  display_app.wifi_status = 1;
  if(xSerialSemaphore == NULL)
  {
    xSerialSemaphore = xSemaphoreCreateMutex();
    if((xSerialSemaphore) != NULL)
    {
      xSemaphoreGive((xSerialSemaphore));
    }
  }
  xTaskCreatePinnedToCore(measure_task,"Task2", 10000, NULL, 1, &Task2, 1);
  xTaskCreatePinnedToCore(display_task,"Task1", 10000, NULL, 2, &Task1, 0);
 
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
  for(;;)
  {
    if(xSemaphoreTake(xSerialSemaphore, (TickType_t) 5) == pdTRUE)
    {
      OLED_clearScreen();
      drawFirstScreen();
      OLED_display();
      xSemaphoreGive (xSerialSemaphore);
    }
    vTaskDelay(10);
  }
}

void measure_task (void *pvParameters)
{
  (void) pvParameters;
  Serial.print("measure_task is running on core");
  Serial.println(xPortGetCoreID());
  measure_app.pmsData.init();
  for(;;)
  { 
    // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      vTaskDelay(300);
      measure_app.pmsData.readData();
    
      Serial.print("PM 1.0 (ug/m3): ");
      Serial.println(measure_app.pmsData.PMS_10);

      Serial.print("PM 2.5 (ug/m3): ");
      Serial.println(measure_app.pmsData.PMS_2_5);

      Serial.print("PM 10.0 (ug/m3): ");
      Serial.println(measure_app.pmsData.PMS_1_0);
      // Serial.println(measure_app.PM_10);
      // Serial.println(measure_app.PM_2_5);
      xSemaphoreGive(xSerialSemaphore);
    }
    vTaskDelay(10);
  }
}
void loop() {
  // put your main code here, to run repeatedly:
}