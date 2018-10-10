/*
* The MIT License (MIT)
*
* Copyright (C) 2018 Gabriel Nikol
*/

#ifndef ARDUINO_MQTT_SN_CLIENT_RHDATAGRAMSOCKET_H
#define ARDUINO_MQTT_SN_CLIENT_RHDATAGRAMSOCKET_H

#include "MqttSnMessageHandler.h"
#include "SocketInterface.h"

class RHDatagramSocket : SocketInterface {
private:
    RHDatagram &rhDatagram;

    device_address own_address;
    device_address broadcast_address;

    device_address receive_address;
    uint8_t receive_buffer[RH_MAX_MESSAGE_LEN];
public:
    RHDatagramSocket(RHDatagram &rhDatagram) : rhDatagram(rhDatagram) {};

    bool begin() override {
        own_address = device_address(rhDatagram.thisAddress(), 0, 0, 0, 0, 0);
        broadcast_address = device_address(RH_BROADCAST_ADDRESS, 0, 0, 0, 0, 0);
        return rhDatagram.init();
    }

    device_address *getBroadcastAddress() override {
        return &broadcast_address;
    }

    device_address *getAddress() override {
        return &own_address;
    }

    uint8_t getMaximumMessageLength() override {
        return RH_MAX_MESSAGE_LEN;
    }

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) override {
        return send(destination, bytes, bytes_len, UINT8_MAX);
    }

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) override {
        rhDatagram.sendto(bytes, bytes_len, destination->bytes[0]);
        rhDatagram.waitPacketSent();
        rhDatagram.available(); // put it back to receive mode
        return true;
    }

    bool loop() override {
        if (rhDatagram.available()) {

            uint8_t receive_buffer_len = 0;
            memset(&receive_buffer, 0x0, RH_MAX_MESSAGE_LEN);
            memset(&receive_address, 0x0, sizeof(device_address));

            if (rhDatagram.recvfrom(receive_buffer, &receive_buffer_len, &receive_address.bytes[0])) {

                if (mqttSnMessageHandler != nullptr) {
                    mqttSnMessageHandler->receiveData(&receive_address, receive_buffer);
                }

                if (transmissionProtocolUartBridge != nullptr) {
                    transmissionProtocolUartBridge->receiveData(&receive_address, receive_buffer, receive_buffer_len);
                }

            }
        }

        return true;
    }

    template<class RHDatagramSocket>
    void setMqttSnMessageHandler(
            MqttSnMessageHandler<RHDatagramSocket> *mqttSnMessageHandler) {
        this->mqttSnMessageHandler = mqttSnMessageHandler;
    };

    template<class RHDatagramSocket>
    void setTransmissionProtocolUartBridge(
            TransmissionProtocolUartBridge<RHDatagramSocket> *transmissionProtocolUartBridge) {
        this->transmissionProtocolUartBridge = transmissionProtocolUartBridge;
    };
private:
    MqttSnMessageHandler<RHDatagramSocket> *mqttSnMessageHandler = nullptr;
    TransmissionProtocolUartBridge<RHDatagramSocket> *transmissionProtocolUartBridge = nullptr;
};

#endif //ARDUINO_MQTT_SN_CLIENT_RHDATAGRAMSOCKET_H
