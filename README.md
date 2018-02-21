# WiFi-Socket
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
- Flash it using your favourite flash tool (I used the esptool and flashed my MQTT software on it: https://github.com/martin-ger/esp_mqtt)

## Programming
It seems it has the following pin assignment:
- GPIO5  - Relay off (LOW pulse)
- GPIO12 - Relay on (LOW pulse)
- GPIO13 - Power LED
- GPIO14 - Push Button

