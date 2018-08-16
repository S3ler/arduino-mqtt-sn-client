
#ifndef ARDUINO_MQTTSN_CLIENT_GLOBAL_DEFINES_H
#define ARDUINO_MQTTSN_CLIENT_GLOBAL_DEFINES_H

#include <cstdint>

struct device_address {
    uint8_t bytes[6];  // mac
    device_address() {
        bytes[0] = 0x0;
        bytes[1] = 0x0;
        bytes[2] = 0x0;
        bytes[3] = 0x0;
        bytes[4] = 0x0;
        bytes[5] = 0x0;
    }
};

void printDeviceAddress(device_address *address) {
    for (uint8_t i = 0; i < sizeof(device_address); i++) {
        if (i == sizeof(device_address) - 1) {
            Serial.print(address->bytes[i]);
        } else {
            Serial.print(address->bytes[i]);
            Serial.print(", ");
        }
    }
}

#endif //ARDUINO_MQTTSN_CLIENT_GLOBAL_DEFINES_H
