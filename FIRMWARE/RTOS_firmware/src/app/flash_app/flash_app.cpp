#include "flash_app.h"
#include "app/Measure_app/measure_app.h"


void WriteBytetoFlash(int address, uint8_t byte)
{
    EEPROM.write(address, byte);
    delay(5);
    EEPROM.commit();
}

uint8_t ReadBytefromFlash(int address)
{
    uint8_t buff = EEPROM.read(address);
    delay(5);
    return buff;
}

void WriteUshortsFlash(int address, uint16_t* data, uint16_t length)
{
    for(int i = address; i < length + address; i++)
    {
        EEPROM.writeUShort(i*(sizeof(uint16_t)), *data);
        delay(5);
        data++;
    }
    EEPROM.commit();
}

void ReadUshortsFlash(int address, uint16_t* outData, uint16_t length)
{
    for(int i = address; i < length + address; i++)
    {
        *outData = EEPROM.readUShort(i*(sizeof(uint16_t)));
        delay(5);
        outData++;
    }
}

void Write1UshortFlash(int address, uint16_t data)
{
    EEPROM.writeUShort(address, data);
    delay(5);
    EEPROM.commit();
}

uint16_t Read1UshortFlash(int address)
{
    uint16_t buff = EEPROM.readUShort(address);
    delay(5);
    return buff;
}

void AQI_store_init()
{   
    uint16_t begin_AQI[12] = {30,38,32,43,34,37,41,45,31,35,39,47};
    WriteUshortsFlash(0, begin_AQI, 12);
    WriteUshortsFlash(24, begin_AQI, 12);
    delay(10);
    ReadUshortsFlash(PM2_5_C_0_ADDRESS, measure_app.pmsAQIcal.PM2_5_c, 12);
    ReadUshortsFlash(PM10_C_0_ADDRESS, measure_app.pmsAQIcal.PM10_c, 12);
}
