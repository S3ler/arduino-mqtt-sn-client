/*
  The MIT License (MIT)

  Copyright (C) 2018 Gabriel Nikol
*/

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "UdpSocket.h"
#include "MqttSnClient.h"

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};
// Initialize the Ethernet client library
EthernetClient client;

#define buffer_length 10
char buffer[buffer_length + 1];
uint16_t buffer_pos = 0;

IPAddress gatewayIPAddress(192, 168, 178, 88);
uint16_t localUdpPort = 8888;

// #define gatewayHostAddress "arsmb.de"

EthernetUDP udp;
UDPSocket udpSocket(&udp, localUdpPort);
MqttSnClient<UDPSocket> mqttSnClient(udpSocket);

const char* clientId = "UdpMqttSnClient";
char* subscribeTopicName = "Uno/Ethernet/Udp/subscribe";
char* publishTopicName = "Uno/Ethernet/Udp/publish";

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
  Serial.println("Connecting to network");
  Serial.print("MAC:");
  for (uint8_t i = 0; i < sizeof(mac); i++) {
    Serial.print(" ");
    Serial.print(mac[i], HEX);
  }
  Serial.println();

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    while (true) {
      Serial.println(".");
      delay(1000);
    }
  }
  Serial.print("IP: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }

  Serial.println();


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
  // IPAddress 0 - 3 bytes
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
    device_address gateway_device_address;
    convertIPAddressAndPortToDeviceAddress(gatewayIPAddress, localUdpPort, gateway_device_address);
    Serial.print("MQTT-SN Gateway device_address: ");
    printDeviceAddress(&gateway_device_address);

    if (!mqttSnClient.connect(&gateway_device_address, clientId, 180) ) {
      Serial.println("Could not connect MQTT-SN Client.");
      delay(1000);
      return;
    }
    Serial.println();

    Serial.println("MQTT-SN Client connected.");
    mqttSnClient.subscribe(subscribeTopicName, qos);

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
