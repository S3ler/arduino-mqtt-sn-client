# arduino-mqtt-sn-client
Arduino based MQTT-SN Client

### Models
 * Arduino Mega &#x274E;
 * ESP8266  &#x2705;
 * ESP32 &#x274E;

### Transmission Technology to Platform Matrix
|   	| UDP  	| TCP  	| Ethernet  	| WiFi  	| ZigBee  	| LoRa  	| BLE  	|
|---	|---	|---	|---	|---	|---	|---	|---	|
| Arduino ESP8266 	| &#x2705;  	| &#x274E;  	| &#x274C;  	| &#x2705;  	| &#x274E;\*  	| &#x274E;\*  	| &#x274E;\*	|
| Arduino ESP32 	| &#x274E;  	| &#x274E;  	| &#x274C;  	| &#x2705;  	| &#x274E;\*  	| &#x274E;\*  	| &#x274E;\*	|
| Arduino Mega 	| &#x274E;\*  	| &#x274E;\*  	| &#x274E;\*  	| &#x274C;  	| &#x274E;\*  	| &#x274E;\*  	| &#x274C;  	|

\* needs additional transmission hardware

##### Legend: 
* &#x2705; implemented and tested
* &#x274E; not implemented yet
* &#x274C; will not be implemented

## Getting Started

### ESP8266 (UDP)
[Download the the library](https://github.com/S3ler/arduino-mqtt-sn-client/archive/master.zip) and copy it into your Arduino Library directory.
Unzip the archive and rename the folder to `arduino-mqtt-sn-client `.
Restart your Arduino IDE if open.
Select a ESP8266 board in the Arduino IDE.
An example is in `examples -> arduin-mqtt-sn-client -> example -> WiFiUdpMqttSnClient`.
