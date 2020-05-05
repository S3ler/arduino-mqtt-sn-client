/*
* The MIT License (MIT)
*
* Copyright (C) 2018 Copyright Gabriel Nikol
*/

#ifndef ARDUINO_MQTTSN_CLIENT_TRANSMISSIONPROTOCOLUARTBRDIGE_H
#define ARDUINO_MQTTSN_CLIENT_TRANSMISSIONPROTOCOLUARTBRDIGE_H

#include "MqttSnMessageHandler.h"
#include "System.h"
#include <stdint.h>
#include "global_defines.h"
#include "mqttsn_messages.h"
#include <Stream.h>


#if defined(ESP8266) || defined(ESP32)
#include <cerrno>
#include <climits>
#endif

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_YUN) || defined(ARDUINO_AVR_MEGA2560)
#ifndef LONG_MIN
#define LONG_MIN -2147483647
#endif // LONG_MIN
#ifndef LONG_MAX
#define LONG_MAX 2147483647
#endif // LONG_MAX
#endif

enum TransmissionProtocolUartBridgeStatus {
    STARTING,
    IDLE,
    SEND,
    RECEIVE,
    CONFIGURATION,
    PARSE_FAILURE,
    ERROR
};

enum SEND_STATUS {
    SEND_NONE,
    AWAIT_ADDRESS,
    AWAIT_DATA,
    SENDING
};

enum RECEIVE_STATUS {
    RECEIVE_NONE,
    SEND_ADDRESS,
    SEND_DATA
};

enum CONFIGURATION_STATUS {
    CONFIGURATION_NONE = 0,
    CONFIGURATION_STATUS = 1,
    CONFIGURATION_OWN_ADDRESS = 2,
    CONFIGURATION_BROADCAST_ADDRESS = 3,
    CONFIGURATION_MAXIMUM_MESSAGE_LENGTH = 4,
    CONFIGURATION_SERIAL_BUFFER_SIZE = 5
};

// #define PARSERDATADEBUG
// #define PARSERADDRESSDEBUG
#define SERIAL_BUFFER_SIZE 200
#define RECEIVE_BUFFER_SIZE 64
#define SEND_BUFFER_SIZE 64

template<class TransmissionProtocolUartBridge_SocketInterface>
class TransmissionProtocolUartBridge {
private:
    // SocketInterface
    TransmissionProtocolUartBridge_SocketInterface &socketInterface;
    // friend TransmissionProtocolUartBridge_SocketInterface;

    // Status
    TransmissionProtocolUartBridgeStatus status = STARTING;
    RECEIVE_STATUS receive_status = RECEIVE_NONE;
    SEND_STATUS send_status = SEND_NONE;
    uint8_t configuration_status = CONFIGURATION_NONE;


    // StreamBuffer
    bool lineReady = false;
    char serialBuffer[SERIAL_BUFFER_SIZE];
    uint16_t serialBufferCounter = 0;

    // SendBuffer
    device_address destination_address;
    uint8_t data[SEND_BUFFER_SIZE];
    uint16_t data_length = 0;

    // ReceiveBuffer
    device_address receive_address;
    uint8_t receive_buffer[RECEIVE_BUFFER_SIZE];
    uint16_t receive_buffer_length = 0;
    Stream *stream;

public:
    explicit TransmissionProtocolUartBridge(Stream *stream,
                                            TransmissionProtocolUartBridge_SocketInterface &socketInterface) :
            stream(stream), socketInterface(socketInterface) {}

    bool begin() {
        memset(serialBuffer, 0x0, sizeof(serialBuffer));
        socketInterface.setTransmissionProtocolUartBridge(this);
        if (!socketInterface.begin()) {
            status = ERROR;
            return false;
        }
        status = IDLE;
        return true;
    }

    /*
     * Receive from Stream
     */
    void putChar(char c) {
        serialBuffer[serialBufferCounter++] = c;
        if (c == '\n') {
            lineReady = true;
        } else if (serialBufferCounter == SERIAL_BUFFER_SIZE) {
            status = PARSE_FAILURE;
        }
    }

    void receiveData(device_address *address, uint8_t *bytes, uint16_t bytes_length) {
        reset_received_buffer();

        memcpy(&receive_address, address, sizeof(device_address));
        receive_buffer_length = bytes_length;
        memcpy(receive_buffer, bytes, bytes_length);
        /*
        if (bytes[0] != bytes_length) {
            return;
        }
        if (receive_buffer_length == 0 && sizeof(receive_buffer) > bytes[0]) {
            // buffer empty
            // received bytes less or equal to receive_buffer size
            reset_received_buffer();

            memcpy(&receive_address, address, sizeof(device_address));
            receive_buffer_length = bytes[0];
            memcpy(receive_buffer, bytes, receive_buffer_length);
        }
         */
    }

    bool print_received_address() {
        stream->print(F("ADDRESS"));
        for (uint16_t i = 0; i < sizeof(device_address); i++) {
            stream->print(F(" "));
            stream->print(receive_address.bytes[i], DEC);
        }
        stream->print(F("\n"));
        return true;
    }

    bool print_received_data() {
        stream->print(F("DATA"));
        for (uint16_t i = 0; i < receive_buffer_length; i++) {
            stream->print(F(" "));
            stream->print(receive_buffer[i], DEC);
        }
        stream->print(F("\n"));
        return true;
    }

    void reset_received_buffer() {
        memset(&receive_address, 0x0, sizeof(device_address));
        memset(receive_buffer, 0x0, sizeof(receive_buffer));
        receive_buffer_length = 0;
    }

    void notify_socket_disconnected() {
        status == ERROR;
    }

    void resetChip() {
#if defined(ESP8266) || defined(ESP32)
        stream->print(F("OK RESET\n"));
        delay(500);
        ESP.restart();
#elif defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_YUN)
        stream->print(F("OK RESET\n"));
        delay(500);
        asm volatile ("  jmp 0");
#else
#warning "resetChip() is not properly implemented. This means we cannot reset the TransmissionProtocolUartBridge."
        stream->print(F("ERROR RESET_NOT_SUPPORTED\n"));
#endif
    }

    bool loop() {

        if (!socketInterface.loop()) {
            return false;
        }

        if (lineReady) {
            if (isReset(serialBuffer)) {
                resetSerialBuffer();
                resetChip();
            }
        }

        if (status == STARTING) {
            // begin should be already called
            stream->print(F("ERROR NOT_STARTED\n"));
            return false;
        }

        if (status == IDLE && lineReady) {
            // parse
            if (isSend(serialBuffer)) {
                status = SEND;
                send_status = SEND_NONE;
                resetSerialBuffer();
            } else if (isReceived(serialBuffer)) {
                status = RECEIVE;
                receive_status = RECEIVE_NONE;
                resetSerialBuffer();
            } else if (isConfiguration(serialBuffer)) {
                status = CONFIGURATION;
                configuration_status = CONFIGURATION_NONE;
                resetSerialBuffer();
            } else if (isReset(serialBuffer)) {
                resetSerialBuffer();
                resetChip();
            } else {
                status = PARSE_FAILURE;
            }
        }


        if (status == SEND) {
            if (send_status == SEND_NONE) {
                stream->print(F("OK AWAIT_ADDRESS\n"));
                resetSendBuffer();
                send_status = AWAIT_ADDRESS;
            } else if (send_status == AWAIT_ADDRESS && lineReady) {
                if (parseAddress(serialBuffer)) {
                    stream->print(F("OK AWAIT_DATA\n"));
                    resetSerialBuffer();
                    send_status = AWAIT_DATA;
                } else {
                    status = PARSE_FAILURE;
                }
            } else if (send_status == AWAIT_DATA && lineReady) {
                if (parseData(serialBuffer)) {
                    stream->print(F("OK SENDING\n"));
                    resetSerialBuffer();
                    send_status = SENDING;
                } else {
                    status = PARSE_FAILURE;
                }
            } else if (send_status == SENDING) {
                if (sendDataToAddress()) {
                    stream->print(F("OK IDLE\n"));
                    resetSendBuffer();
                    send_status = SEND_NONE;
                    status = IDLE;
                } else {
                    status = ERROR;
                }
            }
        }

        if (status == RECEIVE) {
            if (receive_status == RECEIVE_NONE) {
                stream->print(F("OK SEND_ADDRESS\n"));
                receive_status = SEND_ADDRESS;
            } else if (receive_status == SEND_ADDRESS) {
                if (send_receive_address()) {
                    stream->print(F("OK SEND_DATA\n"));
                    receive_status = SEND_DATA;
                } else {
                    status = ERROR;
                }
            } else if (receive_status == SEND_DATA) {
                if (send_receive_data()) {
                    stream->print(F("OK IDLE\n"));
                    resetReceiveBuffer();
                    receive_status = RECEIVE_NONE;
                    status = IDLE;
                } else {
                    status = ERROR;
                }
            }
        }

        if (status == CONFIGURATION) {
            if (configuration_status == CONFIGURATION_NONE) {
                stream->print(F("OK SEND_STATUS\n"));
                configuration_status = CONFIGURATION_STATUS;
            } else if (configuration_status == CONFIGURATION_STATUS) {
                if (printStatus()) {
                    stream->print(F("OK SEND_OWN_ADDRESS\n"));
                    configuration_status = CONFIGURATION_OWN_ADDRESS;
                } else {
                    status = ERROR;
                }
            } else if (configuration_status == CONFIGURATION_OWN_ADDRESS) {
                if (printOwnAddress()) {
                    stream->print(F("OK SEND_BROADCAST_ADDRESS\n"));
                    configuration_status = CONFIGURATION_BROADCAST_ADDRESS;
                } else {
                    status = ERROR;
                }
            } else if (configuration_status == CONFIGURATION_BROADCAST_ADDRESS) {
                if (printBroadcastAddress()) {
                    stream->print(F("OK SEND_MAXIMUM_MESSAGE_LENGTH\n"));
                    configuration_status = CONFIGURATION_MAXIMUM_MESSAGE_LENGTH;
                } else {
                    status = ERROR;
                }
            } else if (configuration_status == CONFIGURATION_MAXIMUM_MESSAGE_LENGTH) {
                if (printMaximumMessageLength()) {
                    stream->print(F("OK SEND_SERIAL_BUFFER_SIZE\n"));
                    configuration_status = CONFIGURATION_SERIAL_BUFFER_SIZE;
                } else {
                    status = ERROR;
                }
            } else if (configuration_status == CONFIGURATION_SERIAL_BUFFER_SIZE) {
                if (printSerialBufferSize()) {
                    stream->print(F("OK IDLE\n"));
                    configuration_status = CONFIGURATION_NONE;
                    status = IDLE;
                } else {
                    status = ERROR;
                }
            }
        }

        if (status == PARSE_FAILURE) {
            stream->print(F("FAILURE PARSE_FAILURE\n"));
            resetSerialBuffer();
            receive_status = RECEIVE_NONE;
            send_status = SEND_NONE;
            status = IDLE;
        }
        /*
        if (status == FAILURE) {
            stream->print(F("FAILURE\n"));
            resetSerialBuffer();
            receive_status = RECEIVE_NONE;
            send_status = SEND_NONE;
            status = STARTING;
        }
        */
        if (status == ERROR) {
            stream->print(F("ERROR\n"));
            resetSerialBuffer();
            resetChip();
        }

    }

    bool printStatus() {
        stream->print(F("STATUS"));
        stream->print(F(" "));
        if (status == STARTING) {
            stream->print(F("STARTING"));
        } else if (status == IDLE) {
            stream->print(F("IDLE"));
        } else if (status == SEND) {
            stream->print(F("SEND"));
        } else if (status == CONFIGURATION) {
            stream->print(F("CONFIGURATION"));
        } else if (status == PARSE_FAILURE) {
            stream->print(F("PARSE_FAILURE"));
        } else if (status == ERROR) {
            stream->print(F("ERROR"));
        } else {
            stream->print(F("\n"));
            return false;
        }
        stream->print(F("\n"));
        return true;
    }

    bool printOwnAddress() {
        stream->print(F("OWN_ADDRESS"));
        device_address *address = socketInterface.getAddress();
        if (address != nullptr) {
            for (uint16_t i = 0; i < sizeof(device_address); i++) {
                stream->print(F(" "));
                stream->print(address->bytes[i], DEC);
            }
        } else {
            stream->print(F(" "));
            stream->print(F("N/A"));
        }
        stream->print(F("\n"));
        return true;
    }

    bool printBroadcastAddress() {
        stream->print(F("BROADCAST_ADDRESS"));
        device_address *address = socketInterface.getBroadcastAddress();
        if (address != nullptr) {
            for (uint16_t i = 0; i < sizeof(device_address); i++) {
                stream->print(F(" "));
                stream->print(address->bytes[i], DEC);
            }
        } else {
            stream->print(F(" "));
            stream->print(F("N/A"));
        }
        stream->print(F("\n"));
        return true;
    }

    bool printMaximumMessageLength() {
        stream->print(F("MAXIMUM_MESSAGE_LENGTH"));
        stream->print(F(" "));
        uint8_t maximumMessageLength = socketInterface.getMaximumMessageLength();
        stream->print(maximumMessageLength, DEC);
        stream->print(F("\n"));
        return true;
    }

    bool printSerialBufferSize() {
        stream->print(F("SERIAL_BUFFER_SIZE"));
        stream->print(F(" "));
        stream->print(SERIAL_BUFFER_SIZE, DEC);
        stream->print(F("\n"));
        return true;
    }


    void printSerialBuffer() {
        if (lineReady) {
            for (uint16_t i = 0; i < serialBufferCounter; i++) {
                stream->print(serialBuffer[i]);
            }
        } else {
            stream->print(F("lineNotReady"));
        }
    }

    bool isReset(char *buffer) {
        /*
          char *token = strsep(&buffer, " ");
          if (token == NULL) {
            return false;
          }
        */
        return strncmp(buffer, "RESET", strlen("RESET")) == 0;
        //return memcmp(token, "RESET", strlen("RESET")) == 0;
    }

    void resetReceiveBuffer() {
        reset_received_buffer();
    }

    bool send_error() {
        stream->print(F("ERROR\n"));
        return true;
    }

    bool send_receive_data() {
        return print_received_data();
    }

    bool send_receive_address() {
        return print_received_address();
    }

    void resetSerialBuffer() {
        memset(serialBuffer, 0x0, sizeof(serialBuffer));
        serialBufferCounter = 0;
        lineReady = false;
    }

    void resetSendBuffer() {
        memset(&destination_address, 0x0, sizeof(device_address));
        memset(data, 0x0, sizeof(data));
        data_length = 0;
    }

    bool sendDataToAddress() {
        return socketInterface.send(&destination_address, (uint8_t * ) & data, data_length);
    }

    bool parseData(char *buffer) {
#if defined(PARSERDATADEBUG)
        stream->print(F("parseData\n"));
#endif
        char *token = strsep(&buffer, " ");
        if (token == NULL) {
#if defined(PARSERDATADEBUG)
            stream->print(F("token NULL\n"));
#endif
            return false;
        }
        if (memcmp(token, "DATA", strlen("DATA")) != 0) {
#if defined(PARSERDATADEBUG)
            stream->print(F("does not start with DATA\n"));
#endif
            return false;
        }

        memset(data, 0x0, sizeof(data));
        data_length = 0;

        while ((token = strsep(&buffer, " ")) != NULL) {
            long int number = 0;
            if (!parseLong(token, &number)) {
#if defined(PARSERDATADEBUG)
                stream->print(F("failure parsing parseLong\n"));
#endif
                return false;
            }
            if (number > UINT8_MAX || number < 0) {
#if defined(PARSERDATADEBUG)
                stream->print(F("number out of range\n"));
#endif
                return false;
            }
#if defined(PARSERDATADEBUG)
            stream->print(number, DEC);
            stream->print(F("\n"));
#endif
            data[data_length++] = (uint8_t) number;
        }
        return true;
    }

    bool parseAddress(char *buffer) {
#if defined(PARSERADDRESSDEBUG)
        stream->print(F("parseAddress\n"));
#endif
        char *token = strsep(&buffer, " ");
        if (token == NULL) {
#if defined(PARSERADDRESSDEBUG)
            stream->print(F("token NULL\n"));
#endif
            return false;
        }
        if (memcmp(token, "ADDRESS", strlen("ADDRESS")) != 0) {
#if defined(PARSERADDRESSDEBUG)
            stream->print(F("does not start with ADDRESS\n"));
#endif
            return false;
        }

        memset(&destination_address, 0x0, sizeof(device_address));
        uint16_t destination_address_length = 0;

        while ((token = strsep(&buffer, " ")) != NULL) {
            long int number = 0;
            if (!parseLong(token, &number)) {
#if defined(PARSERADDRESSDEBUG)
                stream->print(F("failure parsing parseLong\n"));
#endif
                return false;
            }
#if defined(PARSERADDRESSDEBUG)
            stream->print(number, DEC);
            stream->print(F("\n"));
#endif
            if (number > UINT8_MAX || number < 0) {
#if defined(PARSERADDRESSDEBUG)
                stream->print(F("number out of bounds: "));
                stream->print(number, DEC);
                stream->print(F("\n"));
#endif
                return false;
            }
            if (destination_address_length + 1 > sizeof(device_address)) {
#if defined(PARSERADDRESSDEBUG)
                stream->print(F("address size too long"));
#endif
                return false;
            }
            destination_address.bytes[destination_address_length++] = (uint8_t) number;
        }
#if defined(PARSERADDRESSDEBUG)
        if (destination_address_length != sizeof(device_address)) {
            stream->print(F("destination_address_length unequal device_address length"));
        }
#endif
        return destination_address_length == sizeof(device_address);
    }

    bool isReceived(char *buffer) {
        char *token = strsep(&buffer, " ");
        if (token == NULL) {
            return false;
        }
        return memcmp(token, "RECEIVE", strlen("RECEIVE")) == 0;
    }

    bool isSend(char *buffer) {
        char *token = strsep(&buffer, " ");
        if (token == NULL) {
            return false;
        }
        return memcmp(token, "SEND", strlen("SEND")) == 0;
    }

    bool isConfiguration(char *buffer) {
        char *token = strsep(&buffer, " ");
        if (token == NULL) {
            return false;
        }
        return memcmp(token, "CONFIGURATION", strlen("CONFIGURATION")) == 0;
    }


    bool parseLong(const char *str, long *val) {
        char *temp;
        bool rc = true;
#if defined(ESP8266)
        errno = 0;
#endif
        *val = strtol(str, &temp, 0);

        if (temp == str || (*temp != '\0' && *temp != '\n' && *temp != '\r') ||
            #if defined(ESP8266)
            ((*val == LONG_MIN || *val == LONG_MAX) && errno == ERANGE))
            #else
            // see: strtol() function return values
            ((*val == LONG_MIN || *val == LONG_MAX) && true))
#endif
        {
            rc = false;
        }
        return rc;
    }


};


#endif //ARDUINO_MQTTSN_CLIENT_TRANSMISSIONPROTOCOLUARTBRDIGE_H

