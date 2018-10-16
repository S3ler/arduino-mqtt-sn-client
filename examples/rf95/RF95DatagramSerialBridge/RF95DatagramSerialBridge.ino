/*
* The MIT License (MIT)
*
* Copyright (C) 2018 Gabriel Nikol
*/


#include <RHDatagram.h>
#include "RHDatagramSocket.h"
#include "MqttSnClient.h"
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
//#define RFM95_INT 7
#define RFM95_INT 2

#define FREQUENCY 434.0
#define TX_POWER  13
#define MODEM_CONFIG_CHOICE RH_RF95::Bw125Cr48Sf4096

#define OWN_ADDRESS 3

RH_RF95 rf95(RFM95_CS, RFM95_INT);
RHDatagram rhDatagram(rf95, OWN_ADDRESS);
RHDatagramSocket rhDatagramSocket(rhDatagram);

TransmissionProtocolUartBridge <RHDatagramSocket> transmissionProtocolUartBridge(&Serial, rhDatagramSocket);

#define BAUDRATE 115200

void setup() {
  // Initialize Console and wait for port to open:
  Serial.begin(BAUDRATE);

  // Wait for Console port to connect
  while (!Serial) {}

  if (!transmissionProtocolUartBridge.begin()) {
    Serial.print("Could not initialize RHRF95DatagramUartBridge ");
    while (true) {
      Serial.println(".");
      delay(1000);
    }
  }

  // Configure RH_RF95 driver after init
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

  Serial.println("RHRF95DatagramSerialBridge v0.1 ready!");
}

void loop() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    transmissionProtocolUartBridge.putChar(c);
  }
  transmissionProtocolUartBridge.loop();
}
