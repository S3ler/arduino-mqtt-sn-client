/*
  The MIT License (MIT)

  Copyright (C) 2018 Gabriel Nikol
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "UdpSocket.h"
#include "MqttSnClient.h"


const char* ssid     = "...";
const char* password = "...";


#define buffer_length 10
char buffer[buffer_length + 1];
uint16_t buffer_pos = 0;

IPAddress gatewayIPAddress(192, 168, 178, 88);
uint16_t localUdpPort = 8888;

// #define gatewayHostAddress "arsmb.de"

WiFiUDP udp;
UDPSocket udpSocket(&udp, localUdpPort);
MqttSnClient<UDPSocket> mqttSnClient(udpSocket);

const char* clientId = "UdpMqttSnClient";
char* subscribeTopicName = "ESP8266/Udp/subscribe";
char* publishTopicName = "ESP8266/Udp/publish";

int8_t qos = 0;

void mqttsn_callback(char *topic, uint8_t *payload, uint16_t length, bool retain) {
  Serial.print("Received - Topic: ");
  Serial.print(topic);
  Serial.print(" Payload: ");
  for (uint16_t i = 0; i < length; i++) {
    char c =  (char) * (payload + i);
    Serial.print(c);
  }
  Serial.print(" Length: ");
  Serial.println(length);
}

void setup() {

  Serial.begin(115200);
  delay(10);
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

  Serial.print("Starting MqttSnClient - ");
  mqttSnClient.setCallback(mqttsn_callback);
  if  (!mqttSnClient.begin()) {
    Serial.print("Could not initialize MQTT-SN Client ");
    while (true) {
      Serial.println(".");
      delay(1000);
    }
  }
  Serial.println(" ready!");
}

void convertIPAddressAndPortToDeviceAddress(IPAddress& source, uint16_t port, device_address& target) {
  // IPAdress 0 - 3 bytes
  target.bytes[0] = source[0];
  target.bytes[1] = source[1];
  target.bytes[2] = source[2];
  target.bytes[3] = source[3];
  // Port 4 - 5 bytes
  target.bytes[4] = port >> 8;
  target.bytes[5] = (uint8_t) port ;
}


void loop() {
  if (!mqttSnClient.is_mqttsn_connected()) {
#if defined(gatewayHostAddress)
    IPAddress gatewayIPAddress;
    if (!WiFi.hostByName(gatewayHostAddress, gatewayIPAddress, 20000)) {
      Serial.println("Could not lookup MQTT-SN Gateway.");
      return;
    }
#endif
    device_address gateway_device_address;
    convertIPAddressAndPortToDeviceAddress(gatewayIPAddress, localUdpPort, gateway_device_address);
    Serial.print("MQTT-SN Gateway device_address: ");
    printDeviceAddress(&gateway_device_address);
    Serial.println();
    if (!mqttSnClient.connect(&gateway_device_address, clientId, 180) ) {
      Serial.println("Could not connect MQTT-SN Client.");
      delay(1000);
      return;
    }
    Serial.println("MQTT-SN Client connected.");
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
        Serial.println("Could not publish");
      }
      Serial.println("Published");
      memset(buffer, 0x0, buffer_length);
      buffer_pos = 0;
    }
  }

  mqttSnClient.loop();

}