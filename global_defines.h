/*
* The MIT License (MIT)
*
* Copyright (C) 2018 Gabriel Nikol
*/

#ifndef ARDUINO_MQTTSN_CLIENT_GLOBAL_DEFINES_H
#define ARDUINO_MQTTSN_CLIENT_GLOBAL_DEFINES_H

#include <stdint.h>

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
    device_address(uint8_t one, uint8_t two,
                uint8_t three, uint8_t four,
                uint8_t five, uint8_t six) {
        bytes[0] = one;
        bytes[1] = two;
        bytes[2] = three;
        bytes[3] = four;
        bytes[4] = five;
        bytes[5] = six;
    }
};


extern void printDeviceAddress(device_address *address);

#endif //ARDUINO_MQTTSN_CLIENT_GLOBAL_DEFINES_H
