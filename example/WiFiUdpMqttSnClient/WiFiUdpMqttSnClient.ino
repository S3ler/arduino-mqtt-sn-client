#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "WiFiUdpSocket.h"
#include "MqttSnClient.h"

const char* ssid     = "your-ssid";
const char* password = "your-password";

#define buffer_length 10
char buffer[buffer_length + 1];
uint16_t buffer_pos = 0;

const char* gatewayHostAddress = "armada.dedyn.io";
IPAddress gatewayIPAddress;
device_address gateway_device_address;

WiFiUDP udp;
uint16_t localUdpPort = 8884;
WiFiUdpSocket wiFiUdpSocket(udp, localUdpPort);
MqttSnClient<WiFiUdpSocket> mqttSnClient(wiFiUdpSocket);

const char* clientId = "WiFiUdpMqttSnClient";
const char* topicName = "ESP8266/WiFi/Udp/subscribe";
int8_t qos = 1;

void mqttsn_callback(char *topic, uint8_t *payload, uint16_t length, bool retain) {
  Serial.print("Received - Topic: ");
  Serial.print(topic);
  Serial.print(" Payload: ");
  for (uint16_t i = 0; i < length; i++) {
    char c =  (char) * (payload + i);
    Serial.print(c);
  }
  Serial.print(" Lenght: ");
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

  Serial.print("Starting MqttSnClient ");
  mqttSnClient.setCallback(mqttsn_callback);
  if  (!mqttSnClient.begin()) {
    Serial.print("Could not initialize MQTT-SN Client.");
    while (true) {
      Serial.println(".");
      delay(1000);
    }
  }

}

void convertIPAddressAndPortToDeviceAddress(IPAddress& source, uint16_t port, device_address& target) {
  // IPAdress 0 - 3 bytes
  target.bytes[0] = source[0];
  target.bytes[1] = source[2];
  target.bytes[2] = source[3];
  target.bytes[3] = source[4];
  // Port 4 - 5 bytes
  target.bytes[4] = (uint8_t) port >> 8;
  target.bytes[5] = (uint8_t) port ;
}


void loop() {
  if (mqttSnClient.is_mqttsn_connected()) {
    if (!WiFi.hostByName(gatewayHostAddress, gatewayIPAddress, 20000)) {
      Serial.print("Could not lookup MQTT-SN Gateway.");
      return;
    }
    Serial.print("MQTT-SN Gateway IPAddress: ");
    Serial.println(gatewayIPAddress);

    convertIPAddressAndPortToDeviceAddress(gatewayIPAddress, localUdpPort, gateway_device_address);


    if (!mqttSnClient.connect(&gateway_device_address, clientId, 180) ) {
      Serial.print("Could not connect MQTT-SN Client.");
      return;
    }
    Serial.print("MQTT-SN Client connected.");
    mqttSnClient.subscribe(topicName, qos);

  }
  if (Serial.available() > 0) {
    buffer[buffer_pos++] = Serial.read();
    if (buffer[buffer_pos] == '\n') {
      // TODO only qos = 0 and qos = 1 support
      // TODO only topicName = subscribed topic supported for publish
      mqttSnClient.publish(buffer, topicName, qos);
    }
  }

  mqttSnClient.loop();

}
