#include "SX1276Socket.h"
#include "MqttSnClient.h"

SX1276Socket loRaSocket(sx1276);

MqttSnClient<SX1276Socket> mqttSnClient(loRaSocket);
char* subscribeTopicName = "LoRa/SX1276/subscribe";
int8_t qos = 0;

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
  loRaSocket.begin();
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

void loop() {
   if (mqttSnClient.is_mqttsn_connected()) {
    Serial.println("Connected to gateway");
   }
   Serial.println("MQTT-SN Client connected.");
   mqttSnClient.subscribe(subscribeTopicName, qos);
   mqttSnClient.loop();
  
}
