#include <Arduino.h>
#include <WiFi.h>
#include "app/display_app/display_app.h"
#include "app/measure_app/measure_app.h"

TaskHandle_t Task1;
TaskHandle_t Task2;
//TaskHandle_t Task3;
SemaphoreHandle_t xSerialSemaphore;
void display_task(void *pvParameters);
void measure_task(void *pvParameters);
//void power_task(void *pvParameters);

const char* ssid = "Song Quynh";
const char* password = "songquynh25042112";
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  PMS_SERIAL.begin(9600);
  WiFi.mode(WIFI_AP_STA);
  /* start SmartConfig */
  WiFi.beginSmartConfig();
 
  /* Wait for SmartConfig packet from mobile */
  Serial.println("Waiting for SmartConfig.");
  while (!WiFi.smartConfigDone()) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("SmartConfig done.");
 
  /* Wait for WiFi to connect to AP */
  Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display_app.wifi_status = 0;
  }
  Serial.println("WiFi Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
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
  //xTaskCreatePinnedToCore(power_task,"Task3", 10000, NULL, 3, &Task3, 2);
  delay(500);

}

void display_task(void *pvParameters)
{ 
  (void) pvParameters;

  // Serial.print("display_task is running on core");
  // Serial.println(xPortGetCoreID());
  OLED_init();

  display_app.battert_percent = 100;
  display_app.battery_status = 1;
  long timeSinceLastSwitch = 0;
  display_app.screen_number = 1;
  for(;;)
  { 
    if(xSemaphoreTake(xSerialSemaphore, (TickType_t) 5) == pdTRUE)
    { 
        OLED_clearScreen();
        drawFirstScreen();
        OLED_display();
        vTaskDelay(1200);
        OLED_clearScreen();
        drawSecondScreen();
        OLED_display();
        vTaskDelay(800);
      xSemaphoreGive (xSerialSemaphore);
    }

    vTaskDelay(10);

  }
}

void measure_task (void *pvParameters)
{
  (void) pvParameters;

  // Serial.print("measure_task is running on core");
  // Serial.println(xPortGetCoreID());
  measure_app.pmsData.init();
  measure_app.dht22Data.init();
  for(;;)
  { 
    // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    { 
      // PMS7003 Task Excute
      //vTaskDelay(300);
      measure_app.dht22Data.delay();
      measure_app.pmsData.readData();

      measure_app.dht22Data.getstatus();
      // Serial.print("Humidity: ");
      // Serial.println(measure_app.dht22Data.Humidity);
      // Serial.print("Temperature: ");
      // Serial.println(measure_app.dht22Data.Temperature);

      // Serial.print("PM 2.5 (ug/m3): ");
      // Serial.println(measure_app.pmsData.PMS_2_5);

      // Serial.print("PM 10.0 (ug/m3): ");
      // Serial.println(measure_app.pmsData.PMS_1_0);
      xSemaphoreGive(xSerialSemaphore);
    }
    vTaskDelay(10);

  }
}

// void power_task (void *pvParameters)
// {
//   (void) pvParameters;

//   for(;;)
//   {
//     if(xSemaphoreTake(xSerialSemaphore, (TickType_t) 5) == pdTRUE)
//     { 
//       vTaskDelay(10);
//       xSemaphoreGive (xSerialSemaphore);
//     }
//   }
// }
void loop() {
  // put your main code here, to run repeatedly:
}