
#ifndef ARDUINO_MQTTSN_CLIENT_WIFIUDPSOCKET_H
#define ARDUINO_MQTTSN_CLIENT_WIFIUDPSOCKET_H


#include "MqttSnMessageHandler.h"
#include "SocketInterface.h"
//#include <WiFiUdp.h>

class WiFiUdpSocket : SocketInterface {
  private:
    WiFiUDP& wiFiUdp;
    uint16_t port;

  public:
    WiFiUdpSocket(WiFiUDP& wiFiUdp, uint16_t port) : wiFiUdp(wiFiUdp), port(port) { };

    bool begin() override {
      return false;
    }

    device_address *getBroadcastAddress() override {
      return nullptr;
    }

    device_address *getAddress() override {
      return nullptr;
    }

    uint8_t getMaximumMessageLength() override {
      return 0;
    }

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) override {
      return false;
    }

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) override {
      return false;
    }

    bool loop() override {
      //TODO: mqttSnMessageHandler->receiveData(...);
      return false;
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
