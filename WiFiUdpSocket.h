
#ifndef ARDUINO_MQTTSN_CLIENT_WIFIUDPSOCKET_H
#define ARDUINO_MQTTSN_CLIENT_WIFIUDPSOCKET_H


#include "MqttSnMessageHandler.h"
#include "SocketInterface.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define RECEIVE_BUFFER_SIZE 255

class WiFiUdpSocket : SocketInterface {
private:
    WiFiUDP &wiFiUdp;
    uint16_t port;
    device_address own_address;
    device_address receive_address;
    uint8_t receive_buffer[RECEIVE_BUFFER_SIZE];
public:
    WiFiUdpSocket(WiFiUDP &wiFiUdp, uint16_t port) : wiFiUdp(wiFiUdp), port(port) {};

    bool begin() override {
        // TODO multicast socket receive of advertisements
        //  uint8_t beginMulticast(IPAddress interfaceAddr, IPAddress multicast, uint16_t port);
        return wiFiUdp.begin(port);
    }

    device_address *getBroadcastAddress() override {
        // TODO return multicast address
        return nullptr;
    }

    device_address *getAddress() override {
        IPAddress localIP = WiFi.localIP();
        uint16_t localPort = wiFiUdp.localPort();
        convertIPAddressAndPortToDeviceAddress(localIP, localPort, own_address);
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

        if (wiFiUdp.beginPacket(destination_IPAddress, destination_port) == 1) {
            if (wiFiUdp.write(bytes, bytes_len) == bytes_len) {
                if (wiFiUdp.endPacket() == 1) {
                    return true;
                }
            }
        }

        return false;
    }

    void convertIPAddressAndPortToDeviceAddress(IPAddress &source, uint16_t port, device_address &target) {
        // IPAdress 0 - 3 bytes
        target.bytes[0] = source[0];
        target.bytes[1] = source[1];
        target.bytes[2] = source[2];
        target.bytes[3] = source[3];
        // Port 4 - 5 bytes
        target.bytes[4] = port >> 8;
        target.bytes[5] = (uint8_t) port ;
    }


    void convertDeviceAddressToIPAddressAndPort(device_address *source, IPAddress &destination_IPAddress,
                                                uint16_t *destination_port) {
        // convert device_address to IPAddress and port
        // IPAdress 0 - 3 bytes
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
        if (wiFiUdp.parsePacket() > 0) {

            memset(receive_buffer, 0x0, RECEIVE_BUFFER_SIZE);
            int available = wiFiUdp.available();
            if (available < RECEIVE_BUFFER_SIZE) {

                if (wiFiUdp.read(receive_buffer, RECEIVE_BUFFER_SIZE) == available) {

                    IPAddress remoteIP = wiFiUdp.remoteIP();
                    uint16_t remotePort = wiFiUdp.remotePort();
                    convertIPAddressAndPortToDeviceAddress(remoteIP, remotePort, receive_address);
                    mqttSnMessageHandler->receiveData(&receive_address, receive_buffer);

                }
            }
            // too much data => ignored
            wiFiUdp.flush();
        }
        return true;
    }

    template<class WiFiUdpSocket>
    void setMqttSnMessageHandler(
            MqttSnMessageHandler<WiFiUdpSocket> *mqttSnMessageHandler) {
        this->mqttSnMessageHandler = mqttSnMessageHandler;
    };
private:
    MqttSnMessageHandler<WiFiUdpSocket> *mqttSnMessageHandler;
};


#endif //ARDUINO_MQTTSN_CLIENT_WIFIUDPSOCKET_H
