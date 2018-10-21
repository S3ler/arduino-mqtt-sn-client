/*
* The MIT License (MIT)
*
* Copyright (C) 2018 Gabriel Nikol
*/

#ifndef ARDUINO_MQTTSN_CLIENT_CLIENTSOCKET_H
#define ARDUINO_MQTTSN_CLIENT_CLIENTSOCKET_H


#include "MqttSnMessageHandler.h"
#include "SocketInterface.h"
#include <IPAddress.h>
#include <Client.h>

#define RECEIVE_BUFFER_SIZE 255

class ClientSocket : SocketInterface {
private:
    Client *client;
    uint16_t localPort = 0;

    device_address own_address;
    device_address receive_address;

public:
    ClientSocket(Client *client) : client(client){};

    bool begin() override {
        memset(&own_address, 0x0, sizeof(device_address));
        memset(&receive_address, 0x0, sizeof(device_address));
        return true;
    }

    device_address *getBroadcastAddress() override {
        return nullptr;
    }

    void setIpAddressAndPort(IPAddress &ipAddress, uint16_t localPort){
        this->localPort = localPort;
        convertIPAddressAndPortToDeviceAddress(ipAddress, localPort, own_address);
    }

    device_address *getAddress() override {
        // returns 0, 0, 0, 0, 0, 0 if no address was set
        return &own_address;
    }

    uint8_t getMaximumMessageLength() override {
        return RECEIVE_BUFFER_SIZE;
    }

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) override {
        return send(destination, bytes, bytes_len, UINT8_MAX);
    }

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) override {
        if(client->connected()){
            if(memcmp(&receive_address, destination, sizeof(device_address))==0){
                if (client->write(bytes, bytes_len) == bytes_len) {
                    client->flush();
                    return true;
                }
            }
        } else {
            IPAddress ip;
            uint16_t port;
            convertDeviceAddressToIPAddressAndPort(destination, ip, &port);
            if(client->connect(ip, port)){
                if (client->write(bytes, bytes_len) == bytes_len) {
                    memcpy(&receive_address, destination, sizeof(device_address));
                    client->flush();
                    return true;
                }
            }
        }
        return false;
    }

    bool loop() override {
        if(client->connected()) {

            if (client->available()) {

                uint8_t receive_buffer[RECEIVE_BUFFER_SIZE];
                memset(receive_buffer, 0x0, RECEIVE_BUFFER_SIZE);

                int available = client->available();
                if (available < RECEIVE_BUFFER_SIZE) {

                    if (client->read(receive_buffer, RECEIVE_BUFFER_SIZE) == available) {

                        if (mqttSnMessageHandler != nullptr) {
                            mqttSnMessageHandler->receiveData(&receive_address, receive_buffer);
                        }

                        if (transmissionProtocolUartBridge != nullptr) {
                            transmissionProtocolUartBridge->receiveData(&receive_address, receive_buffer, available);
                        }
                    }

                } else {
                    client->flush();
                }
            }
            client->flush();
            return true;
        }
        return false;
    }

    void convertIPAddressAndPortToDeviceAddress(IPAddress &source, uint16_t port, device_address &target) {
        // IPAddress 0 - 3 bytes
        target.bytes[0] = source[0];
        target.bytes[1] = source[1];
        target.bytes[2] = source[2];
        target.bytes[3] = source[3];
        // Port 4 - 5 bytes
        target.bytes[4] = port >> 8;
        target.bytes[5] = (uint8_t) port;
    }


    void convertDeviceAddressToIPAddressAndPort(device_address *source, IPAddress &destination_IPAddress,
                                                uint16_t *destination_port) {
        // convert device_address to IPAddress and port
        // IPAddress 0 - 3 bytes
        destination_IPAddress[0] = source->bytes[0];
        destination_IPAddress[1] = source->bytes[1];
        destination_IPAddress[2] = source->bytes[2];
        destination_IPAddress[3] = source->bytes[3];
        // Port 4 - 5 bytes
        uint16_t tmp_port = 0;
        tmp_port = ((uint16_t) source->bytes[4]) << 8;
        tmp_port += (uint16_t) source->bytes[5];
        *destination_port = tmp_port;
    }

    template<class ClientSocket>
    void setMqttSnMessageHandler(
            MqttSnMessageHandler<ClientSocket> *mqttSnMessageHandler) {
        this->mqttSnMessageHandler = mqttSnMessageHandler;
    };

    template<class ClientSocket>
    void setTransmissionProtocolUartBridge(
            TransmissionProtocolUartBridge<ClientSocket> *transmissionProtocolUartBridge) {
        this->transmissionProtocolUartBridge = transmissionProtocolUartBridge;
    };
private:
    MqttSnMessageHandler<ClientSocket> *mqttSnMessageHandler = nullptr;
    TransmissionProtocolUartBridge<ClientSocket> *transmissionProtocolUartBridge = nullptr;

};


#endif //ARDUINO_MQTTSN_CLIENT_CLIENTSOCKET_H
