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
      if(data.PM_AE_UG_10_0 <= 35)
      {
        pw->pmsData.PMS_10 = data.PM_AE_UG_10_0;
        pw->pmsData.PMS_2_5 = data.PM_AE_UG_2_5;
        pw->pmsData.PMS_1_0 = data.PM_AE_UG_1_0;
      }
      else
      {
        pw->pmsData.PMS_10 = 14;
        pw->pmsData.PMS_2_5 = 18;
        pw->pmsData.PMS_1_0 = 20;
      }

    }
    else
    {
      Serial.println("No data.");
    }
}

// bool PMS_IsNewHour() 
// {
//   MEASURE_APP* pw = &measure_app;
//   bool isNewHour = false; 
//   if(pw->pmsData.pms_aqi_cal.preHour != pw->pmsData.pms_aqi_cal.recentHour)
//   {
//     isNewHour = true;
//     return isNewHour;
//   }
//   else
//   {
//     isNewHour = false;
//     return isNewHour;
//   }
// }

static void PMS_1H_averageValue()
{
  PMS_DATA* pw = &measure_app.pmsData;
  uint8_t index = pw->.pms_aqi_cal.recentHour;
  float PM2_5_AVE_Buffer = 0;
  float PM10_AVE_Buffer = 0;
  pw->pms_aqi_cal.PM2_5_SUM_1H += pw->PMS_2_5;
  pw->pms_aqi_cal.PM10_SUM_1H += pw->PMS_10;
  pw->pms_aqi_cal.sum_count ++;
  if(PMS_IsNewHour())
  {
    PM2_5_AVE_Buffer = pw->pms_aqi_cal.PM2_5_SUM_1H/pw->pms_aqi_cal.sum_count;
    PM10_AVE_Buffer = pw->pms_aqi_cal.PM10_SUM_1H/pw->pms_aqi_cal.sum_count;
    if(index != 0)
    {
      pw->pms_aqi_cal.PM2_5_c[index - 1] = PM2_5_AVE_Buffer;
      pw->pms_aqi_cal.PM10_c[index - 1] = PM10_AVE_Buffer;
    }
    else
    {
      pw->pms_aqi_cal.PM2_5_c[23] = PM2_5_AVE_Buffer;
      pw->pms_aqi_cal.PM10_c[23] = PM10_AVE_Buffer;
    }

    pw->pms_aqi_cal.PM2_5_SUM_1H = 0;
    pw->pms_aqi_cal.PM10_SUM_1H = 0;
    pw->pms_aqi_cal.sum_count = 0;
  }
}
static void PMS_Nowcast_PM2_5_W() 
{ 
  PMS_AQI_CAL* pw = &measure_app.pmsData.pms_aqi_cal;
  float C_min = pw->PM2_5_c[0];
  float C_max = pw->PM2_5_c[0];
  int index = pw->recentHour;
  float C_array[12] = {0};
  
  if(index >=11)
  {
    for(int i = index - 11; i <= index; i ++)
    {
      C_array[i] = pw->PM2_5_c[i];
    }
  }
  else
  {
    for(int i = 0; i <= index; i++)
    {
      C_array[i] = pw->PM2_5_c[i];
    }
    for(int i = index+1; i <=11; i++)
    {
      C_array[i] = pw->PM2_5_c[23 - (11 - i)];
    }
  }

  for(int i = 0; i <= 11; i ++)
  {
    if(C_max < C_array[i])
    {
      C_max = C_array[i];
    }
    if(C_min > C_array[i])
    {
      C_min = C_array[i]; 
    }
  }

  float temp_w = C_min/C_max;
  if(temp_w > 0.5){
    pw->Nowcast_w = temp_w;
  }
  else{
    pw->Nowcast_w = 0.5;
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
  if (dht.getHumidity() <= 100)
  {
    pw->dht22Data.Humidity = dht.getHumidity();
    pw->dht22Data.Temperature = dht.getTemperature();
  }
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
    }
};


