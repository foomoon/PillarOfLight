#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <LittleFS.h>
#include <WebSocketsServer.h>
#include <FastLED.h>

#include "config.h"
#include "setup.h"

/*__________________________________________________________SETUP__________________________________________________________*/

void setup() {
  
  beginSerial();          // Start Serial interface

  beginLED();             // Activate LEDs (we are going to use them to indicate following startup progress)

  beginWiFi();            // Start Wi-Fi access point, and try to connect to some given access points. Then wait for either an AP or STA connection
  
  beginOTAupdates();      // Start Over-The-Air firware Update service
  
  beginSPIFFS();          // Start SPIFFS and list all contents

  WebSocketServer();      // Start WebSocket server
  
  MDNServer();            // Start mDNS responder

  HTTPServer();           // Start HTTP server with a file read handler and an upload handler
  
}

/*__________________________________________________________LOOP__________________________________________________________*/

void loop() {
  
  webSocket.loop();                           // constantly check for websocket events
  server.handleClient();                      // run the server  
  ArduinoOTA.handle();                        // listen for OTA events
 
  // TODO: Build a PillarOfLight Object that wraps/handles effect rotation and updates
  //  PillarOfLight pillar        // Setup
  //  pillar.ledStrip(FastLED)    // Setup
  //  pillar.setEffect(...)       // Loop
  //  pillar.set("speed", speed)  // Loop
  //  pillar.set(...)             // Loop
  //  pillar.show()               // Loop : Wrap content below (will be non-blocking by nature)

  EVERY_N_MILLISECONDS_I(timerObj, 35) {
    timerObj.setPeriod(speed);
    FastLED.clear();
    FastLED.setBrightness(brightness);

    if (effectName == "fire") {
      Fire2012(); // TODO: Need to elimanate global variables "spark" and "cooling"; make them function inputs
    } else {
      // Nothing
    }
    
    FastLED.show();
  }

}