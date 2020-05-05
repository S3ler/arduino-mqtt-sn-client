/*
* The MIT License (MIT)
*
* Copyright (C) 2018 Gabriel Nikol
*/

#ifndef ARDUINO_MQTT_SN_CLIENT_RHDRELIABLEATAGRAMSOCKET_H
#define ARDUINO_MQTT_SN_CLIENT_RHRELIABLEDATAGRAMSOCKET_H

#include <RHReliableDatagram.h>

#include "MqttSnMessageHandler.h"
#include "SocketInterface.h"

class RHReliableDatagramSocket : SocketInterface {
private:
    RHReliableDatagram &rhDatagram;

    device_address own_address;
    device_address broadcast_address;

    bool reliable = true;
public:
    RHReliableDatagramSocket(RHReliableDatagram &rhDatagram) : rhDatagram(rhDatagram) {};

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

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) {
        bool sendOk;

        if (reliable) {
            sendOk = rhDatagram.sendtoWait(bytes, bytes_len, destination->bytes[0]);
        }
        else {
            sendOk = rhDatagram.sendto(bytes, bytes_len, destination->bytes[0]);
        }
        rhDatagram.waitPacketSent();
        rhDatagram.available(); // put it back to receive mode
        return sendOk;
    }



    bool receive(device_address *source, uint8_t *bytes, uint16_t bytes_len) {
        uint8_t len = bytes_len;
        if (reliable) {
            bool ok = rhDatagram.recvfromAck(bytes, &len, &(source->bytes[0]));
            return ok;
        }
        else {
            return rhDatagram.recvfrom(bytes, &len, &(source->bytes[0]));
        }
    }


    /**
     * Sets whether or not to use RadioHead's "reliable" acknowledged protocol, or its 
     * normal one.  The default is "not reliable"
     */
    void setReliableProtocol(bool isReliable) { this->reliable = isReliable; }

    bool isReliable() { return reliable; }


    bool loop() override {
        
        if (rhDatagram.available()) {

            device_address receive_address;
            memset(&receive_address, 0x0, sizeof(device_address));

            uint8_t receive_buffer[RH_MAX_MESSAGE_LEN];
            memset(&receive_buffer, 0x0, RH_MAX_MESSAGE_LEN);

            uint8_t receive_buffer_len = sizeof(receive_buffer);

            if (receive(&receive_address, receive_buffer, receive_buffer_len)) {

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

    template<class RHReliableDatagramSocket>
    void setMqttSnMessageHandler(
            MqttSnMessageHandler<RHReliableDatagramSocket> *mqttSnMessageHandler) {
        this->mqttSnMessageHandler = mqttSnMessageHandler;
    };

    template<class RHReliableDatagramSocket>
    void setTransmissionProtocolUartBridge(
            TransmissionProtocolUartBridge<RHReliableDatagramSocket> *transmissionProtocolUartBridge) {
        this->transmissionProtocolUartBridge = transmissionProtocolUartBridge;
    };

private:
    MqttSnMessageHandler<RHReliableDatagramSocket> *mqttSnMessageHandler = nullptr;
    TransmissionProtocolUartBridge<RHReliableDatagramSocket> *transmissionProtocolUartBridge = nullptr;
};

#endif //ARDUINO_MQTT_SN_CLIENT_RHRELIABLEDATAGRAMSOCKET_H
