#ifndef APP_FLASH_APP_FLASH_APP_H
#define APP_FLASH_APP_FLASH_APP_H

#include "EEPROM.h"
#include "Arduino.h"

#define FLASH_MAXBYTE_USED           100

#define PM2_5_C_0_ADDRESS            0
#define PM2_5_C_1_ADDRESS            2
#define PM2_5_C_2_ADDRESS            4
#define PM2_5_C_3_ADDRESS            6
#define PM2_5_C_4_ADDRESS            8
#define PM2_5_C_5_ADDRESS            10
#define PM2_5_C_6_ADDRESS            12
#define PM2_5_C_7_ADDRESS            14
#define PM2_5_C_8_ADDRESS            16
#define PM2_5_C_9_ADDRESS            18
#define PM2_5_C_10_ADDRESS           20
#define PM2_5_C_11_ADDRESS           22

#define PM10_C_0_ADDRESS             24
#define PM10_C_1_ADDRESS             26
#define PM10_C_2_ADDRESS             28
#define PM10_C_3_ADDRESS             30
#define PM10_C_4_ADDRESS             32
#define PM10_C_5_ADDRESS             34
#define PM10_C_6_ADDRESS             36
#define PM10_C_7_ADDRESS             38
#define PM10_C_8_ADDRESS             40
#define PM10_C_9_ADDRESS             42
#define PM10_C_10_ADDRESS            44
#define PM10_C_11_ADDRESS            46

void AQI_store_init();
void WriteBytetoFlash(int address, uint8_t byte);
uint8_t ReadBytefromFlash(int address);
void WriteUshortsFlash(int address, uint16_t* data, uint16_t length);
void ReadUshortsFlash(int address, uint16_t* outData, uint16_t length);
void Write1UshortFlash(int address, uint16_t data);
uint16_t Read1UshortFlash(int address);


#endif 