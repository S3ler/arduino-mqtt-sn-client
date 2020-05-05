/*
* The MIT License (MIT)
*
* Copyright (C) 2018 Gabriel Nikol
*/

#include "System.h"

void System::set_heartbeat(uint32_t period) {
    this->heartbeat_period = period;
    this->heartbeat_current = millis();
}

uint32_t System::get_heartbeat() {
    return this->heartbeat_period;
}

bool System::has_beaten() {
    uint32_t current = millis();
    if (current - heartbeat_current > heartbeat_period) {
        this->heartbeat_current = current;
        return true;
    }
    return false;
}

uint32_t System::get_elapsed_time() {
    uint32_t current = millis();
    uint32_t elapsed_time = current - elapsed_current;
    elapsed_current = current;
    return elapsed_time;
}

void System::sleep(uint32_t duration) {
    delay(duration);
}

void System::exit() {
#if defined(ESP8266) || defined(ESP32)
    ESP.restart();
#elif defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_YUN) || defined(ARDUINO_AVR_MEGA2560)
    asm volatile ("  jmp 0");
#else
    #error "System::exit() not properly implemented. This means we cannot reset the hardware. Change #error to #warning if you want to build anyway."
#endif
}
