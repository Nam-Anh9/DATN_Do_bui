#ifndef APP_MEASURE_APP_MEASURE_APP_H
#define APP_MEASURE_APP_MEASURE_APP_H

#include <Arduino.h>
#include "PMS.h"
#include "DHT22.h"
#include <math.h>

#define PMS_SERIAL Serial2 
#define DHT_PIN 33

typedef struct MEASURE_APP_t MEASURE_APP;

typedef struct PMS_AQI_CAL_t {
    uint16_t PM2_5_c[12];
    uint16_t PM10_c[12];
    uint16_t PM2_5_1day[24];
    uint16_t PM10_1day[24];
    uint16_t PM2_5_1h[60];
    uint16_t PM10_1h[60];
    int AQI_h;
    int AQI_d;
    float PM2_5_Nowcast;
    float PM10_Nowcast;
    void(*getAQIh)();
    void(*getAQId)();
    uint16_t(*getPMSdata1hour)(uint16_t*);
    float(*getNowcast)(uint16_t*);
}PMS_AQI_CAL;

typedef struct PMS_DATA_t{
    uint16_t PMS_10;
    uint16_t PMS_2_5;
    uint16_t PMS_1_0;
    uint16_t AQI_PM2;
    void (*init)();
    void (*readData)();
}PMS_DATA;

typedef struct DHT22_DATA_t{
    float Humidity;
    float Temperature;
    float Humidity_update;
    float Temperature_update;
    void (*init)();
    void (*delay)();
    void (*getstatus)();
}DHT22_DATA;

typedef struct TIME_DATA_t{
    bool one_min_expried;
    bool one_hour_expried;
    uint16_t min_counter;
    uint16_t hour_counter;
}TIME_DATA;

struct MEASURE_APP_t
{
    PMS_DATA pmsData;
    DHT22_DATA dht22Data;
    TIME_DATA timeData;
    PMS_AQI_CAL pmsAQIcal;
};

extern MEASURE_APP measure_app;

#endif