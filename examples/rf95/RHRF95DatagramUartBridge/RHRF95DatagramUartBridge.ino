/*
* The MIT License (MIT)
*
* Copyright (C) 2018 Gabriel Nikol
*/

#include <RHDatagram.h>
#include "RHDatagramSocket.h"
#include "MqttSnClient.h"
#include <RH_RF95.h>

RH_RF95 rf95;
#define FREQUENCY 868.0
#define TX_POWER  13
#define MODEM_CONFIG_CHOICE RH_RF95::Bw125Cr48Sf4096

#define OWN_ADDRESS 1
RHDatagram rhDatagram(rf95, OWN_ADDRESS);
RHDatagramSocket rhDatagramSocket(rhDatagram);

TransmissionProtocolUartBridge <RHDatagramSocket> transmissionProtocolUartBridge(rhDatagramSocket);

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  if (!transmissionProtocolUartBridge.begin()) {
    Serial.print("Could not initialize TransmissionProtocolUartBridge ");
    while (true) {
      Serial.println(".");
      delay(1000);
    }
  }

  // Configure RH_RF95 driver after init from RHDatagram
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  rf95.setFrequency(FREQUENCY);
  rf95.setTxPower(TX_POWER);
  rf95.setModemConfig(MODEM_CONFIG_CHOICE);

  Serial.println(" ready!");
}

void loop() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    transmissionProtocolUartBridge.putChar(c);
  }
  transmissionProtocolUartBridge.loop();
}
