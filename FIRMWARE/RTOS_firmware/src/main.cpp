#include <Arduino.h>
#include <WiFi.h>
#include "app/display_app/display_app.h"
#include "app/measure_app/measure_app.h"
#include "app/flash_app/flash_app.h"
#include "HTTPClient.h"

#define   DEBUGMODE   0

TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;

String UrlThingspeak = "https://api.thingspeak.com/update?api_key=PD6XF5Q7ZZ75D2OA";
String httpGETRequest(const char* Url);

SemaphoreHandle_t xSerialSemaphore;

static TimerHandle_t reload_timer = NULL;
bool update_valid = false;

void TimerCallBack_handle(TimerHandle_t xTimer);
void display_task(void *pvParameters);
void measure_task(void *pvParameters);
void upload_task(void *pvParameters);

const char* ssid = "Song Quynh";
const char* password = "songquynh25042112";
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  PMS_SERIAL.begin(9600);
  EEPROM.begin(FLASH_MAXBYTE_USED);
  pinMode(POWER_PIN, INPUT);
  pinMode(BATTERY_PIN, INPUT);
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
  display_app.reg_b = READ_PERI_REG(SENS_SAR_READ_CTRL2_REG);   //Save ADC2 reg address
  WiFi.begin(ssid, password);
  Serial.println("Waiting for WiFi");
  // int lastime = millis();
  // while (millis() - lastime <= 5000) {
  //   if(WiFi.status() != WL_CONNECTED)
  //   {
  //     WiFi.
  //     delay(500);
  //     Serial.print(".");
  //     display_app.wifi_status = 0;
  //   }
  //   else
  //   {
  //     display_app.wifi_status = 1;
  //     Serial.println("WiFi Connected.");
  //     Serial.print("IP Address: ");
  //     Serial.println(WiFi.localIP());
  //     break;
  //   }
  // }
  // Serial.println("");
  // Serial.println("WiFi connected.");
  // Serial.println("IP address: ");
  // Serial.println(WiFi.localIP());
  AQI_store_init();

  if(xSerialSemaphore == NULL)
  {
    xSerialSemaphore = xSemaphoreCreateCounting(2,1);
    if(xSerialSemaphore != NULL)
    {
      xSemaphoreGive((xSerialSemaphore));
    }
  }
  xTaskCreatePinnedToCore(measure_task,"Task2", 20000, NULL, 5, &Task2, 1);
  xTaskCreatePinnedToCore(display_task,"Task1", 5000, NULL, 5, &Task1, 0);
  xTaskCreatePinnedToCore(upload_task,"Task3", 5000, NULL, 5, &Task3, 0);
  reload_timer = xTimerCreate("Reload Timeer", 60000/ portTICK_PERIOD_MS, pdTRUE, (void*)1, TimerCallBack_handle);

  //xTaskCreatePinnedToCore(power_task,"Task3", 10000, NULL, 3, &Task3, 2);
  // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  // printLocalTime();
  xTimerStart(reload_timer, portMAX_DELAY);
#if DEBUGMODE
  for(int i = 0; i < 12; i++)
  {
    Serial.print("P2_5:");
    Serial.println(measure_app.pmsAQIcal.PM2_5_c[i]);
  }
  for(int i = 0; i < 12; i++)
  {
    Serial.print("P10:");
    Serial.println(measure_app.pmsAQIcal.PM10_c[i]);
  }
#endif
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
        getBatteryStatus();
        OLED_clearScreen();
        drawFirstScreen();
        OLED_display();
        vTaskDelay(1200);
        OLED_clearScreen();
        drawSecondScreen();
        OLED_display();
        vTaskDelay(800);
        xSemaphoreGive(xSerialSemaphore);
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
  measure_app.pmsAQIcal.PM2_5_Nowcast = measure_app.pmsAQIcal.getNowcast(measure_app.pmsAQIcal.PM2_5_c);
#if DEBUGMODE
  Serial.print("Nowcast_PM2.5 = ");
  Serial.println(measure_app.pmsAQIcal.PM2_5_Nowcast);
#endif
  measure_app.pmsAQIcal.PM10_Nowcast = measure_app.pmsAQIcal.getNowcast(measure_app.pmsAQIcal.PM10_c);
#if DEBUGMODE
  Serial.print("Nowcast_PM10 = ");
  Serial.println(measure_app.pmsAQIcal.PM10_Nowcast);
#endif
  measure_app.pmsAQIcal.getAQIh();
#if DEBUGMODE
  Serial.print("AQIh = ");
  Serial.println(measure_app.pmsAQIcal.AQI_h);
#endif
  for(;;)
  { 
    // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
    // if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 10 ) == pdTRUE )
    // { 
      // PMS7003 Task Excute
      //vTaskDelay(300);
      //Serial.println("Measure Task is running");
      TIME_DATA* p_time = &measure_app.timeData;
      PMS_AQI_CAL* p_aqi  = &measure_app.pmsAQIcal;
      measure_app.dht22Data.delay();
      vTaskDelay(300);
      measure_app.dht22Data.getstatus();
      measure_app.pmsData.readData();
      vTaskDelay(300);
      //Save PMS data 1 time per min
      if(p_time->one_min_expried == true)
      {
        if(p_time->min_counter != 59)
        {
          p_aqi->PM10_1h[p_time->min_counter] = measure_app.pmsData.PMS_10;
          p_aqi->PM2_5_1h[p_time->min_counter] = measure_app.pmsData.PMS_2_5;

          p_time->min_counter++;
          p_time->one_min_expried = false;
        }
        else
        {
          p_aqi->PM10_1h[p_time->min_counter] = measure_app.pmsData.PMS_10;
          p_aqi->PM2_5_1h[p_time->min_counter] = measure_app.pmsData.PMS_2_5;

          p_time->min_counter = 0;
          p_time->one_min_expried = false;
          p_time->one_hour_expried = true;
        }
      }

      if(p_time->one_hour_expried == true)
      {
        if(p_time->hour_counter != 11)
        { 
          //Tinh AQI trun binh h
          p_aqi->PM10_c[p_time->hour_counter] = p_aqi->getPMSdata1hour(p_aqi->PM10_1h);
          p_aqi->PM2_5_c[p_time->hour_counter] = p_aqi->getPMSdata1hour(p_aqi->PM2_5_1h);
          //Ghi vao flash
          Write1UshortFlash(p_time->hour_counter*2, p_aqi->PM2_5_c[p_time->hour_counter]);
          Write1UshortFlash(p_time->hour_counter*2 + PM10_C_0_ADDRESS, p_aqi->PM10_c[p_time->hour_counter]);

          p_time->hour_counter++;
          p_time->one_hour_expried = false;
        }
        else
        { 
          //Tinh AQI trung binh h
          p_aqi->PM10_c[p_time->hour_counter] = p_aqi->getPMSdata1hour(p_aqi->PM10_1h);
          p_aqi->PM2_5_c[p_time->hour_counter] = p_aqi->getPMSdata1hour(p_aqi->PM2_5_1h);

          //Ghi vao flash
          Write1UshortFlash(p_time->hour_counter*2, p_aqi->PM2_5_c[p_time->hour_counter]);
          Write1UshortFlash(p_time->hour_counter*2 + PM10_C_0_ADDRESS, p_aqi->PM10_c[p_time->hour_counter]);

          p_time->hour_counter = 0;
          p_time->one_hour_expried = false;
        }

        ReadUshortsFlash(PM2_5_C_0_ADDRESS, measure_app.pmsAQIcal.PM2_5_c, 12);
        ReadUshortsFlash(PM10_C_0_ADDRESS, measure_app.pmsAQIcal.PM10_c, 12);
        p_aqi->PM2_5_Nowcast = p_aqi->getNowcast(p_aqi->PM2_5_c);
        p_aqi->PM10_Nowcast = p_aqi->getNowcast(p_aqi->PM10_c);
        p_aqi->getAQIh();
      }
    //   xSemaphoreGive(xSerialSemaphore);
    // }
    vTaskDelay(10);

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
      if(update_valid)
      { 
        if (display_app.wifi_status == 0)
        {
          bool wifi_temp = WiFi.reconnect();
          if(wifi_temp)
          {
            display_app.wifi_status = 1;
          }
          else
          {
            display_app.wifi_status = 0;
          }
        }
        else
        {
          uint16_t *pm_10 = &pw->pmsData.PMS_10;
          uint16_t *pm_2_5 = &pw->pmsData.PMS_2_5;
          float* humi = &pw->dht22Data.Humidity_update;
          float* temp = &pw->dht22Data.Temperature_update;
          int* aqi_h = &pw->pmsAQIcal.AQI_h;
          char para[100];
          sprintf(para,"&field1=%d&field2=%d&field3=%d&field4=%f&field5=%f",*pm_2_5,*pm_10,*aqi_h,*temp,*humi);
          //Serial.print(para);
          String Url = UrlThingspeak + String(para);
          httpGETRequest(Url.c_str());
          update_valid = false;
        }
      }
      
      xSemaphoreGive(xSerialSemaphore);
    }
    vTaskDelay(100);

  }
}

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

void TimerCallBack_handle(TimerHandle_t xTimer)
{
  if((uint32_t)pvTimerGetTimerID(xTimer) == 1)
  {
    Serial.println("Auto reload timer expried");
    measure_app.timeData.one_min_expried = true;
    update_valid = true;
  }
}
void loop() {
  // put your main code here, to run repeatedly:
}