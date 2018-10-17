/*
* The MIT License (MIT)
*
* Copyright (C) 2018 2018 Gabriel Nikol
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "WiFiUdpSocket.h"
#include "TransmissionProtocolUartBridge.h"

const char* ssid     = "your-ssid";
const char* password = "your-password";

WiFiUDP udp;
uint16_t localUdpPort = 8888;
WiFiUdpSocket wiFiUdpSocket(udp, localUdpPort);

TransmissionProtocolUartBridge<WiFiUdpSocket> transmissionProtocolUartBridge(&Serial, wiFiUdpSocket);

void setup(){
  Serial.begin(115200);
  while (!Serial) { }

  Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
       would try to act as both a client and an access-point and could cause
       network-issues with your other WiFi-devices on your WiFi-network. */
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.print("Starting TransmissionProtocolUartBridge - ");
    //mqttSnClient.setCallback(mqttsn_callback);
    if  (!transmissionProtocolUartBridge.begin()) {
      Serial.print("Could not initialize TransmissionProtocolUartBridge ");
      while (true) {
        Serial.println(".");
        delay(1000);
      }
    }
    Serial.println(" ready!");
}

void loop(){
  if (Serial.available() > 0) {
    char c = Serial.read();
    transmissionProtocolUartBridge.putChar(c);
  }
  transmissionProtocolUartBridge.loop();
}
