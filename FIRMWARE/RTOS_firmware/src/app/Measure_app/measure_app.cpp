#include "measure_app.h"

PMS pms(Serial2);
PMS::DATA data;

DHT dht;

void PMS_Init(){
    pms.passiveMode();
}
void PMS_ReadData()
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

uint16_t PMS_Get1Hour_Data(uint16_t* PM_Data)
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

float PMS_GetNowcast(uint16_t* PM_C)
{
  float Nowcast = 0;
  float w = 0, w_temp = 0;
  float c_min = (float)*PM_C;
  float c_max = (float)*PM_C;

  for(int i = 0; i < 12; i++)
  {
    if(c_max < (float)*PM_C)  c_max = (float)*PM_C;
    if(c_min > (float)*PM_C)  c_min = (float)*PM_C;
    *PM_C++;
  }

  // Serial.print("c_min = ");
  // Serial.println(c_min);
  // Serial.print("c_max = ");
  // Serial.println(c_max);
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
    // PM2_5_AQI_h
    if(p_aqi->PM2_5_Nowcast <= 25 && p_aqi->PM2_5_Nowcast > 0)
    {
      AQI_PM2_5 = (50.0/25.0)*(p_aqi->PM2_5_Nowcast - 0) + 0;               //I1
    }
    else if(p_aqi->PM2_5_Nowcast <= 50 && p_aqi->PM2_5_Nowcast > 25)
    {
      AQI_PM2_5 = ((100.0-50.0)/(50.0-25.0))*(p_aqi->PM2_5_Nowcast - 25.0) + 50.0;    //I2
    }
    else if(p_aqi->PM2_5_Nowcast <= 80 && p_aqi->PM2_5_Nowcast > 50)
    {
      AQI_PM2_5 = ((150.0-100.0)/(80.0-50.0))*(p_aqi->PM2_5_Nowcast - 50.0) + 100.0;  //I3
    }
    else if(p_aqi->PM2_5_Nowcast <= 150 && p_aqi->PM2_5_Nowcast > 80)
    {
      AQI_PM2_5 = ((200.0-150.0)/(150.0-80.0))*(p_aqi->PM2_5_Nowcast - 80.0) + 150.0;  //I4
    }
    else if(p_aqi->PM2_5_Nowcast <= 250 && p_aqi->PM2_5_Nowcast > 150)
    {
      AQI_PM2_5 = ((300.0-200.0)/(250.0-150.0))*(p_aqi->PM2_5_Nowcast - 150.0) + 200.0;  //I5
    }
    else if(p_aqi->PM2_5_Nowcast <= 350 && p_aqi->PM2_5_Nowcast > 250)
    {
      AQI_PM2_5 = ((400.0-300.0)/(350.0-250.0))*(p_aqi->PM2_5_Nowcast - 250.0) + 300.0;  //I6
    }
    else if(p_aqi->PM2_5_Nowcast <= 500 && p_aqi->PM2_5_Nowcast > 350)
    {
      AQI_PM2_5 = ((500.0-400.0)/(500.0-350.0))*(p_aqi->PM2_5_Nowcast - 350.0) + 400.0;  //I7
    }
    else
    {
      AQI_PM2_5 = ((600.0-500.0)/(600-500))*(p_aqi->PM2_5_Nowcast - 500.0) + 400.0;  //I8
    }

    // Tinh PM_10_AQI_h

    if(p_aqi->PM10_Nowcast <= 50 && p_aqi->PM10_Nowcast > 0)
    {
      AQI_PM10 = (50.0/50.0)*(p_aqi->PM10_Nowcast - 0) + 0;               //I1
    }
    else if(p_aqi->PM10_Nowcast <= 150 && p_aqi->PM10_Nowcast > 50)
    {
      AQI_PM10 = ((100.0-50.0)/(150.0-50.0))*(p_aqi->PM10_Nowcast - 25.0) + 50.0;    //I2
    }
    else if(p_aqi->PM10_Nowcast <= 250 && p_aqi->PM10_Nowcast > 150)
    {
      AQI_PM10 = ((150.0-100.0)/(250.0-150.0))*(p_aqi->PM10_Nowcast - 150.0) + 100.0;  //I3
    }
    else if(p_aqi->PM10_Nowcast <= 350 && p_aqi->PM10_Nowcast > 250)
    {
      AQI_PM10 = ((200.0-150.0)/(350.0-250.0))*(p_aqi->PM10_Nowcast - 250.0) + 150.0;  //I4
    }
    else if(p_aqi->PM10_Nowcast <= 420 && p_aqi->PM10_Nowcast > 350)
    {
      AQI_PM10 = ((300.0-200.0)/(420.0-350.0))*(p_aqi->PM10_Nowcast - 350.0) + 200.0;  //I5
    }
    else if(p_aqi->PM10_Nowcast <= 500 && p_aqi->PM10_Nowcast > 420)
    {
      AQI_PM10 = ((400.0-300.0)/(500.0-420.0))*(p_aqi->PM10_Nowcast - 420.0) + 300.0;  //I6
    }
    else if(p_aqi->PM10_Nowcast <= 600 && p_aqi->PM10_Nowcast > 500)
    {
      AQI_PM10 = ((500.0-400.0)/(600.0-500.0))*(p_aqi->PM10_Nowcast - 500.0) + 400.0;  //I7
    }
    else
    {
      AQI_PM10 = ((700.0-600.0)/(700-600))*(p_aqi->PM10_Nowcast - 600.0) + 400.0;  //I8
    }
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
void DHT_Init() {
  dht.setup(DHT_PIN);
}
void DHT_Delay() {
  delay(dht.getMinimumSamplingPeriod());
}

void DHT_GetStatus() {
  MEASURE_APP* pw = &measure_app;
  if(dht.getTemperature() < 100)
  {
    pw->dht22Data.Humidity = dht.getHumidity();
    pw->dht22Data.Temperature = dht.getTemperature();
  }
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


