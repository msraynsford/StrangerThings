# Stranger Things Lights
Source code and assembly documents for my Stranger Things light displays

# Connecting to the lights
When the micro controller device is connected to a power supply via a usb cable (the kit does not have to be assembled to connect to the chip),
The ESP8266 will start a new WiFi connection called "StrangerThings". You should be able to find this network from your phone/laptop and connect to it using the password:'password'.

![alt text](https://github.com/msraynsford/StrangerThings/blob/master/WifiNetworks.png "Wifi List")

Once the connection has been established, your phone should automatically load a browser which points to the right URL for the device but if it does not you can open a browser window and point towards http://stranger.local/

![alt text](https://github.com/msraynsford/StrangerThings/blob/master/StrangerThingsBrowser.png "Browser Image")

If you are still not able to load the page by typing the URL into the browser, it is possible to reach the right page by typing in the IP address http://192.168.4.1/
