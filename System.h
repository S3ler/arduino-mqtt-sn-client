
#ifndef ARDUINO_MQTTSN_CLIENT_SYSTEM_H
#define ARDUINO_MQTTSN_CLIENT_SYSTEM_H

#include <stdint.h>

#if ARDUINO >= 100
#include "Arduino.h"
#endif

/**
 * Defines basic functionality provided by an underlying OS or must be implemented otherwise.
 */
class System {
private:
    uint32_t heartbeat_period = 10000;
    uint32_t heartbeat_current = 0;
    uint32_t elapsed_current = 0;

public:
    /**
     * Sets the heartbeat value where the System perform regular checks.
     * Default value is a period of 30 000 ms
     * @param period
     */
    virtual void set_heartbeat(uint32_t period);

    /**
     * Gets the heartbeat value where the System perform regular checks.
     * Default value is a period of 30 000 ms
     */
    virtual uint32_t get_heartbeat();

    /**
     * Checks if the heartbeat is timed out.
     * @return true if the heartbeat value was reached, false otherwise.
     */
    virtual bool has_beaten();

    /**
     * Get the elapsed time between two calls of this function.
     * @return the elapsed time between two calls
     */
    virtual uint32_t get_elapsed_time();

    /**
     * Lets the execution sleep for some seconds.
     */
    virtual void sleep(uint32_t duration);

    /**
     * Stop/exit the whole program or restart it if you want to.
     */
    virtual void exit();
};

#endif //ARDUINO_MQTTSN_CLIENT_SYSTEM_H
