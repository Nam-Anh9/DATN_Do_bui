#ifndef MAIN_APP_H
#define MAIN_APP_H
#include "component/dht_22/dht_22.h"
class MAIN_APP_t {
    public:
    DHT22 dht22;
};

extern MAIN_APP_t main_app;
#endif