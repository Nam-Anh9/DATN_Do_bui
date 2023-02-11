#include <Arduino.h>
#include <WiFi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "app/display_app/display_app.h"
#include "app/measure_app/measure_app.h"
#include "string.h"
#include "ThingSpeak.h"

#define CHANNEL_ID 2025978
#define CHANNEL_API_KEY "PD6XF5Q7ZZ75D2OA"

//Global 
TaskHandle_t Task1;
TaskHandle_t Task2;
//TaskHandle_t Task3;
TaskHandle_t Task4;

const char* ssid = "Song Quynh";
const char* password = "songquynh25042112";
WiFiClient client;

SemaphoreHandle_t semaphore;

void display_task(void *pvParameters);
void measure_task(void *pvParameters);
//void power_task(void *pvParameters);
void update_data_Task(void *pvParameter);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  PMS_SERIAL.begin(115200);
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

  // WiFi.begin(ssid,password);
  // /* Wait for WiFi to connect to AP */
  // Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display_app.wifi_status = 0;
  }
  Serial.println("WiFi Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  display_app.wifi_status = 1;

  semaphore = xSemaphoreCreateCounting(3, 3);
  
  xTaskCreatePinnedToCore(measure_task,"Task2", 10000, NULL, 1, &Task2, 0);
  xTaskCreatePinnedToCore(display_task,"Task1", 10000, NULL, 1, &Task1, 0);
  //xTaskCreatePinnedToCore(power_task,"Task3", 10000, NULL, 3, &Task3, 2);
  xTaskCreatePinnedToCore(update_data_Task,"Task4", 10000, NULL, 1, &Task4, 0);
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
  display_app.screen_number = 0;
  
  for(;;)
  { 
    if(xSemaphoreTake(semaphore, (TickType_t)10) == pdTRUE)
    {   
      OLED_clearScreen();
      drawFirstScreen();
      OLED_display();
      vTaskDelay(800);
      OLED_clearScreen();
      drawSecondScreen();
      OLED_display();
      vTaskDelay(400);
      xSemaphoreGive(semaphore);
    }

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
  if(xSemaphoreTake(semaphore, (TickType_t)10) == pdTRUE)
  {
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
    xSemaphoreGive(semaphore);
  }

    // PMS7003 Task Excute
    //vTaskDelay(300);

  }
}

void update_data_Task (void *pvParameters)
{ 
  (void) pvParameters;
  ThingSpeak.begin(client);
  for(;;)
  { 
    if(xSemaphoreTake(semaphore, (TickType_t)10) == pdTRUE)
    { 
      int PM_10 = measure_app.pmsData.PMS_10;
      int PM2_5 = measure_app.pmsData.PMS_2_5;
      float humi = measure_app.dht22Data.Humidity;
      float temp = measure_app.dht22Data.Temperature;
      ThingSpeak.setField(1, PM2_5);
      ThingSpeak.setField(2, PM_10);
      ThingSpeak.setField(4, humi);
      ThingSpeak.setField(5, temp);
      int x;
      do
      {
        x = ThingSpeak.writeFields(CHANNEL_ID, CHANNEL_API_KEY);

        if(x == 200)
        {
          Serial.println("Channel update successful.");
        }
        else
        {
          Serial.println("Problem updating channel. HTTP error code " + String(x));
        }
      } while (x != 200);
      


      xSemaphoreGive(semaphore);
    }
  }

}


void loop() {
  // put your main code here, to run repeatedly:
}

