/*
* The MIT License (MIT)
*
* Copyright (C) 2018 Gabriel Nikol
*/

#ifndef ARDUINO_MQTT_SN_CLIENT_RHDATAGRAMSOCKET_H
#define ARDUINO_MQTT_SN_CLIENT_RHDATAGRAMSOCKET_H

#include "MqttSnMessageHandler.h"
#include "SocketInterface.h"
#include <RHGenericDriver.h>

#define RECEIVE_BUFFER_SIZE 255

class RHGenericDriverSocket : SocketInterface {
private:
    RHGenericDriver *rhDatagram;

    uint8_t ownAddress;
    device_address own_address;
    device_address broadcast_address;


public:
    RHGenericDriverSocket(RHGenericDriver *rhDatagram, uint8_t ownAddress) : rhDatagram(rhDatagram),
                                                                         ownAddress(ownAddress) {};

    bool begin() override {
        rhDatagram->setThisAddress(ownAddress);
        rhDatagram->setHeaderFrom(ownAddress);
        own_address = device_address(ownAddress, 0, 0, 0, 0, 0);
        broadcast_address = device_address(RH_BROADCAST_ADDRESS, 0, 0, 0, 0, 0);
        return rhDatagram->init();
    }

    device_address *getBroadcastAddress() override {
        return &broadcast_address;
    }

    device_address *getAddress() override {
        return &own_address;
    }

    uint8_t getMaximumMessageLength() override {
        return RECEIVE_BUFFER_SIZE;
    }

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) override {
        return send(destination, bytes, bytes_len, UINT8_MAX);
    }

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) override {
        rhDatagram->setHeaderTo(destination->bytes[0]);
        rhDatagram->setHeaderFrom(own_address.bytes[0]);

        if(!rhDatagram->send(bytes, bytes_len)){
            rhDatagram->waitPacketSent();
            rhDatagram->available(); // put it back to receive mode
            return false;
        }
        rhDatagram->waitPacketSent();
        rhDatagram->available(); // put it back to receive mode
        return true;
    }

    bool loop() override {
        if (rhDatagram->available()) {

            device_address receive_address;
            memset(&receive_address, 0x0, sizeof(device_address));

            uint8_t receive_buffer[RECEIVE_BUFFER_SIZE];
            memset(&receive_buffer, 0x0, RECEIVE_BUFFER_SIZE);

            uint8_t receive_buffer_len = sizeof(receive_buffer);

            if (rhDatagram->recv(receive_buffer, &receive_buffer_len)) {
                if (rhDatagram->headerTo() == own_address.bytes[0] ||
                        rhDatagram->headerTo() == RH_BROADCAST_ADDRESS ) {

                    receive_address.bytes[0] = rhDatagram->headerFrom();

                    if (mqttSnMessageHandler != nullptr) {
                        mqttSnMessageHandler->receiveData(&receive_address, receive_buffer);
                    }

                    if (transmissionProtocolUartBridge != nullptr) {
                        transmissionProtocolUartBridge->receiveData(&receive_address, receive_buffer,
                                                                    receive_buffer_len);
                    }

                }

            }
        }

        return true;
    }

    template<class RHGenericDriverSocket>
    void setMqttSnMessageHandler(
            MqttSnMessageHandler<RHGenericDriverSocket> *mqttSnMessageHandler) {
        this->mqttSnMessageHandler = mqttSnMessageHandler;
    };

    template<class RHGenericDriverSocket>
    void setTransmissionProtocolUartBridge(
            TransmissionProtocolUartBridge<RHGenericDriverSocket> *transmissionProtocolUartBridge) {
        this->transmissionProtocolUartBridge = transmissionProtocolUartBridge;
    };
private:
    MqttSnMessageHandler<RHGenericDriverSocket> *mqttSnMessageHandler = nullptr;
    TransmissionProtocolUartBridge<RHGenericDriverSocket> *transmissionProtocolUartBridge = nullptr;
};

#endif //ARDUINO_MQTT_SN_CLIENT_RHDATAGRAMSOCKET_H
