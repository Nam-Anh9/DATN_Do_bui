#ifndef APP_MEASURE_APP_MEASURE_APP_H
#define APP_MEASURE_APP_MEASURE_APP_H

#include <Arduino.h>
#include "PMS.h"
#define PMS_SERIAL Serial2 
typedef struct MEASURE_APP_t MEASURE_APP;

typedef struct PMS_DATA_t{
    uint16_t PMS_10;
    uint16_t PMS_2_5;
    uint16_t PMS_1_0;
    uint8_t AQI;
    void (*init)();
    void (*readData)();
}PMS_DATA;

struct MEASURE_APP_t
{
    PMS_DATA pmsData;
};

extern MEASURE_APP measure_app;

#endif