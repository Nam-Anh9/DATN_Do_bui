#include <Arduino.h>
#include <WiFi.h>
#include "app/display_app/display_app.h"
#include "app/measure_app/measure_app.h"
#include "HTTPClient.h"

TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;

String UrlThingspeak = "https://api.thingspeak.com/update?api_key=PD6XF5Q7ZZ75D2OA";
String httpGETRequest(const char* Url);
SemaphoreHandle_t xSerialSemaphore;
void display_task(void *pvParameters);
void measure_task(void *pvParameters);
void upload_task(void *pvParameters);

const char* ssid = "Song Quynh";
const char* password = "songquynh25042112";
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  PMS_SERIAL.begin(9600);
  // WiFi.mode(WIFI_AP_STA);
  // /* start SmartConfig */
  // WiFi.beginSmartConfig();
 
  // /* Wait for SmartConfig packet from mobile */
  // Serial.println("Waiting for SmartConfig.");
  // while (!WiFi.smartConfigDone()) {
  //   delay(500);
  //   Serial.print(".");
  // }
  // Serial.println("");
  // Serial.println("SmartConfig done.");
 
  /* Wait for WiFi to connect to AP */
  WiFi.begin(ssid, password);
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
    xSerialSemaphore = xSemaphoreCreateCounting(3,3);
    if((xSerialSemaphore) != NULL)
    {
      xSemaphoreGive((xSerialSemaphore));
    }
  }
  xTaskCreatePinnedToCore(measure_task,"Task2", 10000, NULL, 5, &Task2, 0);
  xTaskCreatePinnedToCore(display_task,"Task1", 10000, NULL, 5, &Task1, 0);
  xTaskCreatePinnedToCore(upload_task,"Task3", 10000, NULL, 5, &Task1, 0);
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
    if(xSemaphoreTake(xSerialSemaphore, (TickType_t) 10) == pdTRUE)
    {   
        //Serial.println("Display Task is running");
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
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 10 ) == pdTRUE )
    { 
      // PMS7003 Task Excute
      //vTaskDelay(300);
      //Serial.println("Measure Task is running");
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
    vTaskDelay(100);

  }
}

void upload_task(void *pvParameters)
{
  (void) pvParameters;
  MEASURE_APP* pw = &measure_app;
  for(;;)
  { 
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 10 ) == pdTRUE )
    {
      //Serial.println("Measure Task is running");
      int pm_10 = pw->pmsData.PMS_10;
      int pm_2_5 = pw->pmsData.PMS_2_5;
      float humi = pw->dht22Data.Humidity;
      float temp = pw->dht22Data.Temperature;
      char para[60];
      sprintf(para,"&field1=%d&field2=%d&field4=%f&field5=%f",pm_2_5,pm_10,temp,humi);
      String Url = UrlThingspeak + String(para);
      httpGETRequest(Url.c_str());
      xSemaphoreGive(xSerialSemaphore);
    }
    vTaskDelay(100);

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

String httpGETRequest(const char* Url)
{
  HTTPClient http;
  //Serial.println(Url);
  http.begin(Url);
  int responseCode = http.GET();
  String responseBody = "{}";
  if(responseCode > 0)
  {
    Serial.print("responseCode:");
    Serial.println(responseCode);
    responseBody = http.getString();
  }
  else
  {
    Serial.print("Error Code: ");
    Serial.println(responseCode);
  }
  http.end();
  return responseBody;
}
void loop() {
  // put your main code here, to run repeatedly:
}