# WiFi-Socket
Custom SW for the "Wifi Stecker Schuko"

WARNING: This devices uses mains power! For good reasons it is not build to be opened and if you do so, you are responsible for your life! Never ever touch it or connect it to your computer when connected to mains.

German DIY market OBI is selling a WiFi socket (look for item "OBI 2291706"). It contains an ESP chip, two LEDs, a push button, power supply and a relay in a socket case. Opening the case is somewhat tricky as they used deep lying "never-seen-before-screws", but with a little bit of drilling I managed it:

<img src="https://raw.githubusercontent.com/martin-ger/WiFi-Socket/master/IMG_20180221_130652_s.jpg">

You can nicly ready the names of the ESP pins on the daughter board:
<img src="https://raw.githubusercontent.com/martin-ger/WiFi-Socket/master/IMG_20180221_131028_s.jpg">

Next step will be to find out which GPIO pins control the LEDs, the button and the relay and flash new SW on it. 
