#include <Arduino.h>

#include "global_defines.h"

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
