
#include "System.h"

void System::set_heartbeat(uint32_t period) {
    this->heartbeat_period = period;
#ifdef ARDUINO
    this->heartbeat_current = millis();
#else
    this->heartbeat_current = 0;
#endif
}

uint32_t System::get_heartbeat() {
    return this->heartbeat_period;
}

bool System::has_beaten() {
#ifdef ARDUINO
    uint32_t current = millis();
#else
    uint32_t current = 0;
#endif
    if (current - heartbeat_current > heartbeat_period) {
        this->heartbeat_current = current;
        return true;
    }
    return false;
}

uint32_t System::get_elapsed_time() {
#ifdef ARDUINO
    uint32_t current = millis();
#else
    uint32_t current = 0;
#endif
    uint32_t elapsed_time = current - elapsed_current;
    elapsed_current = current;
    return elapsed_time;
}

void System::sleep(uint32_t duration) {
#ifdef ARDUINO
    delay(duration);
#endif
}

void System::exit() {
#ifdef ESP8266
    ESP.restart();
#endif
}
