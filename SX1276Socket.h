//
// Created by Bassirou on 09.09.2018.
// 

#ifndef ARDUINO_MQTTSN_CLIENT_LORASOCKET_H
#define ARDUINO_MQTTSN_CLIENT_LORASOCKET_H


#include "MqttSnMessageHandler.h"
#include "SocketInterface.h"
#include <SX1276.h>

#define LORA_MODE 4
#define LORA_CHANNEL CH_10_868
#define LORA_POWER 'H'
#define LORA_ADDR 3


class SX1276Socket : SocketInterface {
private:
    SX1276 &loraConnection;
    uint16_t port;
    device_address own_address;
    device_address receive_address;
    uint8_t receive_buffer[MAX_PAYLOAD];
public:
    
    SX1276Socket(SX1276 &loraConnection) : loraConnection(loraConnection){};


    bool begin() override {
        loraConnection.ON();
        loraConnection.setMode(LORA_MODE);
        loraConnection.setChannel(CH_10_868);
        loraConnection.setPower(LORA_POWER);
        loraConnection.setNodeAddress(LORA_ADDR);
        return 1;
    }
    device_address *getBroadcastAddress() override {
        // TODO return multicast address
        return nullptr;
    }

    device_address *getAddress() override {
        own_address.bytes[0] = LORA_ADDR;
        return &own_address;
    }

    uint8_t getMaximumMessageLength() override {
        return MAX_PAYLOAD;
    }

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) override {
        return send(destination, bytes, bytes_len, UINT8_MAX);
    }

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) override {
        loraConnection.sendPacketTimeout(destination->bytes[1], bytes, bytes_len);

        return false;
    }

    bool loop() override {
        if (loraConnection.availableData()) {
            device_address from;
            memset(&from.bytes[0], 0x0, sizeof(device_address));
            int available = loraConnection.receive();
            if (available == 0) {
                memcpy(receive_buffer,loraConnection.packet_received.data, loraConnection.packet_received.length);
                from.bytes[0] = loraConnection.packet_received.dst;
                mqttSnMessageHandler->receiveData(&from, receive_buffer);
            }
        }
        return true;
    }

    template<class SX1276Socket>
    void setMqttSnMessageHandler(
            MqttSnMessageHandler<SX1276Socket> *mqttSnMessageHandler) {
        this->mqttSnMessageHandler = mqttSnMessageHandler;
    };
private:
    MqttSnMessageHandler<SX1276Socket> *mqttSnMessageHandler;
};


#endif //ARDUINO_MQTTSN_CLIENT_LORASOCKET_H
