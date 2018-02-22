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

In this repository you find a basic Arduino sketch that reads the pushbutton and connects to an MQTT server.

There is also a sample script for my MQTT Broker/Bridge that turns the switch into an MQTT client and broker.

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

This is the script for my MQTT Broker/Bridge (https://github.com/martin-ger/esp_mqtt):
```
% Config params, overwrite any previous settings from the commandline
config ap_ssid 		MQTTbroker
config ap_password	stupidPassword
config broker_user	Martin
config broker_password	secret
config mqtt_host	test.mosquitto.org
config speed		160

% Now the initialization, this is done once after booting
on init
do
	% Device number given in flash var @1 ("* 1" to make even "" a number)
	setvar $device_number = @1 * 1

	% MQTT prefix given in flash var @2
	setvar $mqtt_prefix = @2

	% Status of the relay
	setvar $relay_status=0

	% Command topic
	setvar $command_topic =$mqtt_prefix | "/obiswitch/" | $device_number | "/command"

	% Status topic
	setvar $status_topic = $mqtt_prefix | "/obiswitch/" | $device_number | "/status"

	publish local $status_topic $relay_status retained

	% local subscriptions once in 'init'
	subscribe local $command_topic

	% led
	setvar $wifiled = 0
	gpio_out 4 $wifiled

% If we get the wificonnect let the LED blink
on wificonnect
do
	settimer 2 1000

% If we get the wifidisconnect stop blinking
on wifidisconnect
do
	settimer 2 0
	setvar $wifiled = 0
	gpio_out 4 $wifiled

% Now the MQTT client init, this is done each time the client connects
on mqttconnect
do
	% remote subscriptions for each connection in 'mqttconnect'
	subscribe remote $command_topic

	publish remote $status_topic $relay_status retained

% Is there a remote command?
on topic remote $command_topic
do
	println "Received remote command: " | $this_data

	% republish this locally - this does the action
	publish local $command_topic $this_data


% Is there a local command?
on topic local $command_topic
do
	println "Received local command: " | $this_data

	if $this_data = "1" then
		setvar $relay_status = 1
		gpio_out 12 0
		settimer 1 150
	else
	    if $this_data = "0" then
		setvar $relay_status = 0
		gpio_out 5 0
		settimer 1 150
	    endif
	endif

	publish local $status_topic $relay_status retained
	publish remote $status_topic $relay_status retained


% The local pushbutton
on gpio_interrupt 14 pullup
do
	%println "New state GPIO 14: " | $this_gpio
	if $this_gpio = 0 then
		publish local $command_topic not($relay_status)
	endif


% End pulse
on timer 1
do
	gpio_out 5 1
	gpio_out 12 1

% WifiLED blink
on timer 2
do
	gpio_out 4 $wifiled
	setvar $wifiled = not($wifiled)
	settimer 2 1000

```

