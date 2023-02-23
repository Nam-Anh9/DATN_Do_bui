#include "measure_app.h"

PMS pms(Serial2);
PMS::DATA data;

DHT dht;

static void PMS_Init(){
    pms.passiveMode();
}
static void PMS_ReadData()
{   
    MEASURE_APP *pw = &measure_app;
    pms.requestRead();
    if (pms.readUntil(data))
    {
      // if(data.PM_AE_UG_10_0 <= 35)
      // {
        pw->pmsData.PMS_10 =  0.849901*(float)data.PM_AE_UG_10_0 - 0.04372*pw->dht22Data.Temperature - 0.12205*pw->dht22Data.Humidity + 12.51084;
        pw->pmsData.PMS_2_5 = 0.9087*(float)data.PM_AE_UG_2_5 - 0.04624*pw->dht22Data.Temperature - 0.01895*pw->dht22Data.Humidity + 4.29551;
        pw->pmsData.PMS_1_0 = data.PM_AE_UG_1_0;
      //}
      // else
      // {
      //   pw->pmsData.PMS_10 = 14;
      //   pw->pmsData.PMS_2_5 = 18;
      //   pw->pmsData.PMS_1_0 = 20;
      // }

    }
    else
    {
      Serial.println("No data.");
    }
}

static uint16_t PMS_Get1Hour_Data(uint16_t* PM_Data)
{
  float c = 0;
  for(int i = 0; i < 60; i++)
  {
    c += (float)*PM_Data;
    PM_Data++;
  }

  c = c/60;

  return round(c);
}

static float PMS_GetNowcast(uint16_t* PM_C)
{
  float Nowcast = 0;
  float w = 0, w_temp = 0;
  if(*(PM_C+4) == 0)
  {
    Serial.println("No Nowcast Value");
    Nowcast = 0;
  }
  else
  {
    float c_min = (float)*PM_C;
    float c_max = (float)*PM_C;

    for(int i = 0; i < 12; i++)
    {
      if(c_max < (float)*PM_C)  c_max = (float)*PM_C;
      if(c_min > (float)*PM_C)  c_min = (float)*PM_C;
      *PM_C++;
    }

    //Return Pointer to start posistion
    PM_C = PM_C - 12;
    //Calculate w
    w_temp = c_max/c_min;
    if(w_temp <= 0.5)
    {
      w = 0.5;
      for(float i = 0; i < 12; i++)
      {
        Nowcast += pow(w,i)*((float)*PM_C); // 0.5c0 + 0.5^2c1 + ....
        *PM_C++;
      }
    }
    else
    {
      w = w_temp;
      float divide_number = 0;
      for(float i = 0; i < 12; i++)
      {
        divide_number += pow(w,i);      //w + w^1 + w^2 + ...
      }
      for(float i = 0; i < 12; i++)
      {
        Nowcast += (pow(w,i)*((float)*PM_C))/divide_number; // (w.c0 + w^1.c1 + ....) / (w + w^1 + w^2 + ...)
        *PM_C++;
      }
    }
  }
  
  return Nowcast;
}

void PMS_GetAQIh()
{ 
  PMS_AQI_CAL* p_aqi = &measure_app.pmsAQIcal;
  int I[8] = {0,50,100,150,200,300,400,500};
  int BP_PM10[8] = {0,50,150,250,350,420,500,600};
  int BP_PM2_5[8] = {0,25,50,80,150,250,350,500};
  uint8_t AQI_PM_10_counter = 0;
  uint8_t AQI_PM_2_5_counter = 0;

  float AQI_PM10 = 0;
  float AQI_PM2_5 = 0;
  if(p_aqi->PM2_5_Nowcast == 0 || p_aqi->PM10_Nowcast == 0)
  { 
    p_aqi->AQI_h = 1;
    return;
  }
  else
  {
    while(p_aqi->PM2_5_c[0] > BP_PM2_5[AQI_PM_2_5_counter])
    {
      AQI_PM_2_5_counter++;
    }
    while(p_aqi->PM10_c[0] < BP_PM10[AQI_PM_10_counter])
    {
      AQI_PM_10_counter++;
    }

    AQI_PM10 = (((float)(I[AQI_PM_10_counter] - I[AQI_PM_10_counter-1]))
                /((float)(BP_PM10[AQI_PM_10_counter] - BP_PM10[AQI_PM_10_counter-1])))
                *((float)(p_aqi->PM10_Nowcast - BP_PM10[AQI_PM_10_counter])) + (float)I[AQI_PM_10_counter];
    AQI_PM2_5 = (((float)(I[AQI_PM_2_5_counter] - I[AQI_PM_2_5_counter-1]))
                /((float)(BP_PM10[AQI_PM_2_5_counter] - BP_PM2_5[AQI_PM_2_5_counter-1])))
                *((float)(p_aqi->PM2_5_Nowcast - BP_PM2_5[AQI_PM_2_5_counter])) + (float)I[AQI_PM_2_5_counter];
    if(AQI_PM10 > AQI_PM2_5)
    {
      p_aqi->AQI_h = (int)AQI_PM10;
    }
    else
    {
      p_aqi->AQI_h = (int)AQI_PM2_5;
    }
  }
}
static void DHT_Init() {
  dht.setup(DHT_PIN);
}
static void DHT_Delay() {
  delay(dht.getMinimumSamplingPeriod());
}

static void DHT_GetStatus() {
  MEASURE_APP* pw = &measure_app;
  pw->dht22Data.Humidity = dht.getHumidity();
  pw->dht22Data.Temperature = dht.getTemperature();
  pw->dht22Data.Humidity_update = -0.7563*pw->dht22Data.Humidity + 105.3394;
  pw->dht22Data.Temperature_update = 0.269821*pw->dht22Data.Temperature + 18.14026;
}

MEASURE_APP measure_app = {
    .pmsData = {
        .init = PMS_Init,
        .readData = PMS_ReadData
    },
    .dht22Data = {
      .init = DHT_Init,
      .delay = DHT_Delay,
      .getstatus = DHT_GetStatus
    },
    .pmsAQIcal = {
      .getAQIh = PMS_GetAQIh,
      .getPMSdata1hour = PMS_Get1Hour_Data,
      .getNowcast = PMS_GetNowcast
    }
};


