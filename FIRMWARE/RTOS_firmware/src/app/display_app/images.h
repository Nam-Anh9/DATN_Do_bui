
#include <Arduino.h>
#define WiFi_Logo_width 16
#define WiFi_Logo_height 16
#define Batery_Logo_width 16
#define Batery_Logo_height 16
const uint8_t Batery_Logo_bits[] = PROGMEM {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x3F, 0x45, 0x60, 
  0xFD, 0xEF, 0xFD, 0xAF, 0xFD, 0xAF, 0xFD, 0xEF, 0x23, 0x64, 0xFF, 0x3F, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const uint8_t WiFi_Logo_bits[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x07, 0xF8, 0x1F, 0x1C, 0x38, 
  0xE4, 0x67, 0x78, 0x0E, 0x90, 0x09, 0xE0, 0x07, 0x20, 0x06, 0x80, 0x01, 
  0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

