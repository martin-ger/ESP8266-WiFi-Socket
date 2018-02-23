# ESP8266 WiFi-Socket
Custom SW for the "Wifi Stecker Schuko"

*WARNING: This devices uses mains power! For good reasons it is not build to be opened and if you do so, you are responsible for your life! Never ever touch it or connect it to your computer when connected to mains.*

German DIY market OBI is selling a WiFi socket (look for item "OBI 2291706"). It contains an ESP chip, two LEDs, a push button, power supply 
and a relay in a socket case. 

<img src="https://raw.githubusercontent.com/martin-ger/WiFi-Socket/master/IMG_20180221_160706_s.jpg">

## Hardware
Opening the case is somewhat tricky as they used deep lying "never-seen-before-screws", but with a little bit of drilling I managed it:

<img src="https://raw.githubusercontent.com/martin-ger/WiFi-Socket/master/IMG_20180221_130652_s.jpg">

You can nicely read the names of the ESP pins on the daughter board:

<img src="https://raw.githubusercontent.com/martin-ger/WiFi-Socket/master/IMG_20180221_131028_s.jpg">

Soldering a pin header onto the board isn't a big deal (I first soldered it on straight and bend it down afterwards):

<img src="https://raw.githubusercontent.com/martin-ger/WiFi-Socket/master/IMG_20180221_150011_s.jpg">

This even fits into the case when flashing is done:

<img src="https://raw.githubusercontent.com/martin-ger/WiFi-Socket/master/IMG_20180221_152842_s.jpg">

## Flashing
Flashing follows the usual procedure for the ESP:
- Make sure you are DISCONNECTED from mains power (3,3V is enough)
- Get your USB-UART adapter
- Connect 3,3V, GND, Rx-Tx, Tx-Rx, GPIO0-GND
- Power it up
- Flash it using your favourite flash tool and an appropriate software

## Programming
The device has the following pin assignment:
- GPIO4 - blue WiFi LED
- GPIO5  - Relay off (LOW pulse)
- GPIO12 - Relay on (LOW pulse)
- GPIO14 - Push Button

It is reported that when holding GPIO12 LOW, the Relay can also be switched by simply turning GPIO5 HIGH and LOW. There is a Tasmota mapping that uses these settings: https://github.com/arendst/Sonoff-Tasmota/issues/1988

### Arduino Sketch
In this repository you find a basic Arduino sketch that reads the pushbutton and connects to an MQTT server (https://github.com/martin-ger/ESP8266-WiFi-Socket/blob/master/OBISocket.ino). Find the basic WiFi and MQTT config in first lines of the sketch.

### MQTT Broker Script
There is also a sample script for my MQTT Broker/Bridge, an alternative firmware that turns the switch into a OTA programmable MQTT client and broker. For further instructions see: https://github.com/martin-ger/esp_mqtt

On the CLI define the two flash variables @1 with the device number and @2 with the systems prefix and configure WiFi and the remote MQTT broker, e.g.:
```
set @1 0
set @2 myhome
set ssid MyWiFiSSID
set ap_ssid MQTTbroker
set ap_password stupidPassword
set password MyWiFiPassword
set mqtt_host test.mosquitto.org
```
Now you can send "0" or "1" MQTT messages to the topic @2/obiswitch/@1/command (in this example "myhome/obiswitch/0/command") and the switch will turn on and off. You can send the messages either to the given remote MQTT broker (in this example "test.mosquitto.org") or to the switch itself (use the IP of the switch). You might connect to the switch eiter via the home WiFi (in this example "MyWiFiSSID") or directly to the AP of the switch (in this example "MQTTbroker" with pw "stupidPassword").

You can also toggle the status locally using the button on the device. The blue LED will start to blink as soon as the switch has WiFi connectivity.

The retained topic @2/obiswitch/@1/status (in this example "myhome/obiswitch/0/status") will always report on the current state of the switch.

Here is this script - you can install it directly OTA via this link: https://raw.githubusercontent.com/martin-ger/ESP8266-WiFi-Socket/master/script.obi
