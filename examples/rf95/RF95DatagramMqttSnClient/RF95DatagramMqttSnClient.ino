/*
  The MIT License (MIT)

  Copyright (C) 2018 Gabriel Nikol
*/

#include <RHDatagram.h>
#include "RHDatagramSocket.h"
#include "MqttSnClient.h"
#include <RH_RF95.h>

#define buffer_length 10
char buffer[buffer_length + 1];
uint16_t buffer_pos = 0;

RH_RF95 rf95;

// Change to 434.0 or other frequency, must match RX's freq!
//#define FREQUENCY 868.0
#define FREQUENCY 434.0
#define TX_POWER  13
#define MODEM_CONFIG_CHOICE RH_RF95::Bw125Cr48Sf4096

#define OWN_ADDRESS 4
RHDatagram rhDatagram(rf95, OWN_ADDRESS);
RHDatagramSocket rhDatagramSocket(rhDatagram);
MqttSnClient<RHDatagramSocket> mqttSnClient(rhDatagramSocket);

#define MqttSnGateway_Address   2

const char* clientId = "RHDSMqttSnClient";
char* subscribeTopicName = "RHDatagram/subscribe";
char* publishTopicName = "RHDatagram/publish";
int8_t qos = 0;


void mqttsn_callback(char *topic, uint8_t *payload, uint16_t length, bool retain) {
  Serial.print(F("Received - Topic: "));
  Serial.print(topic);
  Serial.print(F(" Payload: "));
  for (uint16_t i = 0; i < length; i++) {
    char c =  (char) * (payload + i);
    Serial.print(c);
  }
  Serial.print(F(" Length: "));
  Serial.println(length);
}

void setup() {

  Serial.begin(115200);
  delay(10);
  Serial.println();

  Serial.print(F("Starting MqttSnClient - "));
  mqttSnClient.setCallback(mqttsn_callback);
  if  (!mqttSnClient.begin()) {
    Serial.print(F("Could not initialize MQTT-SN Client "));
    while (true) {
      Serial.println(F("."));
      delay(1000);
    }
  }

  // Configure RH_RF95 driver after init from RHDatagram
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  if (!rf95.setFrequency(FREQUENCY)) {
    Serial.print(F("setFrequency failed\n"));
    while (true) {
      Serial.println(".");
      delay(1000);
    }
  }
  Serial.print("Set Freq to: "); Serial.println(FREQUENCY);

  if (!rf95.setModemConfig(RH_RF95::MODEM_CONFIG_CHOICE)) {
    Serial.print(F("setModemConfig failed\n"));
    while (true) {
      Serial.println(".");
      delay(1000);
    }
  }
  Serial.print("Set Modem Config to: "); Serial.println(RH_RF95::MODEM_CONFIG_CHOICE);

  rf95.setTxPower(TX_POWER);
  Serial.print("Set TX Power to: "); Serial.println(TX_POWER);

  Serial.println(F("ready!"));
}

void loop() {
  if (!mqttSnClient.is_mqttsn_connected()) {
    device_address gateway_device_address;
    memset(&gateway_device_address, 0x0, sizeof(device_address));
    gateway_device_address.bytes[0] = MqttSnGateway_Address;
    Serial.print(F("MQTT-SN Gateway device_address: "));
    printDeviceAddress(&gateway_device_address);
    Serial.println();

    if (!mqttSnClient.connect(&gateway_device_address, clientId, 180) ) {
      Serial.println(F("Could not connect MQTT-SN Client."));
      delay(1000);
      return;
    }
    if (!mqttSnClient.subscribe(subscribeTopicName, qos)) {
      Serial.println("subscription failed.");
      delay(1000);
      return;
    }
    Serial.println("subscribed.");

  }

  if (Serial.available() > 0) {
    buffer[buffer_pos++] = Serial.read();
    if (buffer[buffer_pos - 1] == '\n') {
      // only qos -1, 0, 1 are supported
      if (!mqttSnClient.publish(buffer, publishTopicName , qos)) {
        Serial.println(F("Could not publish"));
      }
      Serial.println(F("Published"));
      memset(buffer, 0x0, buffer_length);
      buffer_pos = 0;
    }
  }

  mqttSnClient.loop();

}
