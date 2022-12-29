#ifndef APP_MEASURE_APP_MEASURE_APP_H
#define APP_MEASURE_APP_MEASURE_APP_H

#include <Arduino.h>
#include "PMS.h"
#include "DHT22.h"

#define PMS_SERIAL Serial2 
#define DHT_PIN 23

typedef struct MEASURE_APP_t MEASURE_APP;

typedef struct PMS_AQI_CAL_t {
    float PM2_5_SUM_1H;
    float PM10_SUM_1H;
    long sum_count;
    uint8_t recentHour;
    uint8_t preHour;
    float PM2_5_c[24];
    float PM10_c[24];
    float Nowcast_w;
    float Nowcast;
}PMS_AQI_CAL;

typedef struct PMS_DATA_t{
    uint16_t PMS_10;
    uint16_t PMS_2_5;
    uint16_t PMS_1_0;
    uint16_t AQI_PM2;
    PMS_AQI_CAL pms_aqi_cal;
    void (*init)();
    void (*readData)();
}PMS_DATA;

typedef struct DHT22_DATA_t{
    float Humidity;
    float Temperature;
    void (*init)();
    void (*delay)();
    void (*getstatus)();
}DHT22_DATA;

struct MEASURE_APP_t
{
    PMS_DATA pmsData;
    DHT22_DATA dht22Data;
};

extern MEASURE_APP measure_app;

#endif