#include "measure_app.h"


PMS pms(Serial2);
PMS::DATA data;


static void PMS_Init(){
    pms.passiveMode();
}
static void PMS_ReadData()
{   
    MEASURE_APP *pw = &measure_app;
    Serial.println("Waking up,...");
    vTaskDelay(300);

    Serial.println("Send read request...");
    pms.requestRead();

    Serial.println("Wait max. 1 second for read...");
    if (pms.readUntil(data))
    {
      pw->pmsData.PMS_10 = data.PM_AE_UG_10_0;
      pw->pmsData.PMS_2_5 = data.PM_AE_UG_2_5;
      pw->pmsData.PMS_1_0 = data.PM_AE_UG_1_0;
    }
    else
    {
      Serial.println("No data.");
    }
}

MEASURE_APP measure_app = {
    .pmsData = {
        .init = PMS_Init,
        .readData = PMS_ReadData
    }
};


