/*
* The MIT License (MIT)
*
* Copyright (C) 2018 Gabriel Nikol
*/

#ifndef ARDUINO_MQTTSN_CLIENT_WIFIUDPSOCKET_H
#define ARDUINO_MQTTSN_CLIENT_WIFIUDPSOCKET_H


#include "MqttSnMessageHandler.h"
#include "SocketInterface.h"
#include <Udp.h>

#define RECEIVE_BUFFER_SIZE 255

class UDPSocket : SocketInterface {
private:
    UDP *udp;
    uint16_t localPort;

    device_address own_address;
    device_address broadcast_address;


public:
    UDPSocket(UDP *udp, uint16_t localPort) : udp(udp), localPort(localPort) {
        memset(&own_address, 0x0, sizeof(device_address));
        memset(&broadcast_address, 0x0, sizeof(device_address));
    };

#ifndef ESP8266
    UDPSocket(UDP *udp, uint16_t localPort, IPAddress multicastIp, uint16_t multicastPort) :
    udp(udp), localPort(localPort)
    {
        memset(&own_address, 0x0, sizeof(device_address));
        memset(&broadcast_address, 0x0, sizeof(device_address));
        convertIPAddressAndPortToDeviceAddress(multicastIp, multicastPort, broadcast_address);
    };
#endif

    bool begin() override {
#ifndef ESP8266
        device_address empty_device_address;
        memset(&empty_device_address, 0x0, sizeof(device_address));
        if(!(memcmp(&broadcast_address, &empty_device_address, sizeof(device_address))==0)){
            IPAddress ip;
            uint16_t port;
            convertDeviceAddressToIPAddressAndPort(&broadcast_address, ip, &port);
            if(!udp->beginMulticast(ip, port)){
                return false;
            }
        }
#endif
        return udp->begin(localPort);
    }

    device_address *getBroadcastAddress() override {
        return &broadcast_address;
    }

    void setIpAddress(IPAddress &ipAddress){
        convertIPAddressAndPortToDeviceAddress(ipAddress, localPort, own_address);
    }

    device_address *getAddress() override {
        // return 0, 0, 0, 0, 0, 0 if no address was set
        return &own_address;
    }

    uint8_t getMaximumMessageLength() override {
        return RECEIVE_BUFFER_SIZE;
    }

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) override {
        return send(destination, bytes, bytes_len, UINT8_MAX);
    }

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) override {
        IPAddress destination_IPAddress;
        uint16_t destination_port = 0;
        convertDeviceAddressToIPAddressAndPort(destination, destination_IPAddress, &destination_port);

        if (udp->beginPacket(destination_IPAddress, destination_port) == 1) {
            if (udp->write(bytes, bytes_len) == bytes_len) {
                if (udp->endPacket() == 1) {
                    return true;
                }
            }
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

    bool loop() override {
        if (udp->parsePacket() > 0) {
            device_address receive_address;
            uint8_t receive_buffer[RECEIVE_BUFFER_SIZE];

            memset(&receive_address, 0x0, sizeof(device_address));
            memset(receive_buffer, 0x0, RECEIVE_BUFFER_SIZE);

            int available = udp->available();
            if (available < RECEIVE_BUFFER_SIZE) {

                if (udp->read(receive_buffer, RECEIVE_BUFFER_SIZE) == available) {

                    IPAddress remoteIP = udp->remoteIP();
                    uint16_t remotePort = udp->remotePort();
                    convertIPAddressAndPortToDeviceAddress(remoteIP, remotePort, receive_address);

                    if (mqttSnMessageHandler != nullptr) {
                        mqttSnMessageHandler->receiveData(&receive_address, receive_buffer);
                    }

                    if (transmissionProtocolUartBridge != nullptr) {
                        transmissionProtocolUartBridge->receiveData(&receive_address, receive_buffer, available);
                    }

                }
            }else{
                // too much data => ignored
                udp->flush();
            }

        }
        // TODO check if flush is needed
        udp->flush();
        return true;
    }

    template<class UDPSocket>
    void setMqttSnMessageHandler(
            MqttSnMessageHandler<UDPSocket> *mqttSnMessageHandler) {
        this->mqttSnMessageHandler = mqttSnMessageHandler;
    };

    template<class UDPSocket>
    void setTransmissionProtocolUartBridge(
            TransmissionProtocolUartBridge<UDPSocket> *transmissionProtocolUartBridge) {
        this->transmissionProtocolUartBridge = transmissionProtocolUartBridge;
    };
private:
    MqttSnMessageHandler<UDPSocket> *mqttSnMessageHandler = nullptr;
    TransmissionProtocolUartBridge<UDPSocket> *transmissionProtocolUartBridge = nullptr;

};


#endif //ARDUINO_MQTTSN_CLIENT_WIFIUDPSOCKET_H
