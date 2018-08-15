
#ifndef ARDUINO_MQTTSN_CLIENT_GLOBAL_DEFINES_H
#define ARDUINO_MQTTSN_CLIENT_GLOBAL_DEFINES_H

#include <cstdint>

struct device_address {
    uint8_t bytes[6];  // mac
    device_address(){
        bytes[0] = 0x0;
        bytes[1] = 0x0;
        bytes[2] = 0x0;
        bytes[3] = 0x0;
        bytes[4] = 0x0;
        bytes[5] = 0x0;
    }
};
#endif //ARDUINO_MQTTSN_CLIENT_GLOBAL_DEFINES_H
