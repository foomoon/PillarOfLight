#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <WebSocketsServer.h>
#include <FastLED.h>

#include "config.h"
#include "setup.h"

/*__________________________________________________________SETUP__________________________________________________________*/

void setup() {
  
  startSerial();               // Start Serial interface

  startLED();                  // Start LED

  startWiFiManager();          // Start a Wi-Fi access point, and try to connect to some given access points. Then wait for either an AP or STA connection
  
  startOTA();                  // Start the OTA service
  
  startSPIFFS();               // Start the SPIFFS and list all contents

  startWebSocket();            // Start a WebSocket server
  
  startMDNS();                 // Start the mDNS responder

  startServer();               // Start a HTTP server with a file read handler and an upload handler
  
}

/*__________________________________________________________LOOP__________________________________________________________*/

void loop() {
  
  // if (!rainbow) { //pause all comms while we upload new firware
  webSocket.loop();                           // constantly check for websocket events
  server.handleClient();                      // run the server
  // }
  
  ArduinoOTA.handle();                        // listen for OTA events
 


  EVERY_N_MILLISECONDS_I(timerObj,35) {
    timerObj.setPeriod(speed);
    FastLED.clear();
    FastLED.setBrightness(brightness);
    // gPal = CRGBPalette16( CRGB::Black, rgbA, rgbB,  CRGB::White);
    // gPal = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);
    // loadPalette(CRGB::Black, rgbA, rgbB,  CRGB::White);
    if (effectName == "fire") {
      Fire2012();
    } else {
      // Nothing
    }
    
    FastLED.show();
  }

  

  // ws2812fx.setBrightness(brightness);
  // ws2812fx.setBrightness(brightness);
  // ws2812fx.setColor(BLUE);
  // ws2812fx.setSpeed(1000);
  // ws2812fx.setMode(FX_MODE_CUSTOM);
  // ws2812fx.setCustomMode(myCustomEffect);

  // ws2812fx.service();


  // if (textUpdate) {
  //   LEDmessage.begin(message);
  //   textUpdate = false;
  // }

  // LEDmessage.setTextColor(textColor);
  // LEDmessage.setFrameTime(speed);
  // matrix->setBrightness(brightness);
  // LEDmessage.run();
}