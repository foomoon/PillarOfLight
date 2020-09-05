# PillarOfLight

## Install
Pillar of Light app should be installed to a web-enabled arduino-compatible device such as an ESP8266.  This code is configured to install on a NodeMCU development board with embedded ESP8226.

First install should be via a USB-to-Serial connection similar to any arduino board.  Consequent software updates can be executed Over-the-Air (OTA) via a local wifi network connection.  

## Usage

### Software Dev Environment
This project can probably be configured within the Arduino IDE, however it was created in VS Code with the PlatformIO plugin.  Thus, this is the recommended build environment.

### Software Setup
1) Connect board to your computer via USB 
2) Upload project for first time
3) On separate device, connect to newly initiated network called: AutoConnectAP
4) In a browser, navigate to 192.168.4.1
5) Enter your local wifi credentials (username/password) and save
6) Reconnect to your local wifi (the one you just entered the credentials)
7) In a browser, navigate to pillaroflight.local
8) Done!

### OTA
If the above configuration was done correctly, then future software updates via OTA should work fine when the board is unplugged from USB-to-Serial connection.

### Hardware configuration
This software has only been tested with an ESP8266 NodeMCU dev module and WS2812 adressable LED strips.  Other hardware could be used, but you will have to know what you're doing, as it will require configuration and potentially library changes in the code.  For best results, I recommend sticking to the aformentioned hardware.  

There are 3 pins on a WS2812 led strip
1) Ground
2) Power (5v version is recommended)
3) Signal

Ground -> ESP8266 ground and power source ground
Power -> power source positive
Signal -> Pin D1 on ESP8266 (This can be changed in sofware if another pin is needed)


