// #include "config.h"
#include "helperfun.h"
#include "effects.h"
#include "httpHandlers.h"
#include "websocketHandler.h"

uint8_t progress = 0;


/*__________________________________________________________SETUP_FUNCTIONS__________________________________________________________*/

void splashScreen() {
  Serial.println(F("\r\n"));
  Serial.println(F(" ___ _ _ _                 __   _    _      _   _"));
  Serial.println(F("| _ (_) | |__ _ _ _   ___ / _| | |  (_)__ _| |_| |"));
  Serial.println(F("|  _/ | | / _` | '_| / _ \\  _| | |__| / _` | ' \\  _|"));
  Serial.println(F("|_| |_|_|_\\__,_|_|   \\___/_|   |____|_\\__, |_||_\\__|"));
  Serial.println(F("                v1.0 - Daniel Claxton |___/")); 
  Serial.println(F("\r\n"));
}


void startUpAnimate() {
  const uint8_t progressColorWidth = 3;
  const uint16_t progressColor = CRGB::Blue; 
  
  delay(50); // Slows down animation a bit
  if (progress+progressColorWidth > NUM_LEDS) {
    progress = 0;
    FastLED.clear();
  }
  for (uint8_t i=1; i<progressColorWidth; i++) {
    leds[progress++] = progressColor;
  }

  FastLED.show();
}




void beginSerial() {
  Serial.begin(BAUD);        // Start the Serial communication to send messages to the computer
  delay(101);                // Ensure serial has begun before proceeding
  Serial.println("\r\n");
  splashScreen();              // Serial Splash screen
}




void beginLED() {

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setTemperature(Candle);
  FastLED.clear();


  startUpAnimate();
  Serial.println("LED System Enabled");
}




void beginWiFi() {

    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

    //reset saved settings
    // wifiManager.resetSettings();
    
    //set custom ip for portal
    //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    wifiManager.setConnectTimeout(10);
    wifiManager.setWaitConnectCallback(startUpAnimate);
    wifiManager.autoConnect();

    
    //if you get here you have connected to the WiFi
    Serial.println('\n');
    Serial.println("Connection established!");  
    Serial.print("Hostname:\t");
    Serial.println(WiFi.SSID());
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());    
     
    startUpAnimate();    
}




void beginOTAupdates() { // Start the OTA service
  ArduinoOTA.setHostname(OTA_NAME);
  ArduinoOTA.setPassword(OTA_PASS);

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
    FastLED.clear();
    FastLED.show();
    
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\r\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA ready\r\n");
  startUpAnimate();
}





void beginFS() { // Start the SPIFFS and list all contents
  LittleFS.begin();                             // Start the SPI Flash File System (SPIFFS)
  Serial.println("LittleFS started. Contents:");
  {
    Dir dir = LittleFS.openDir("/");
    while (dir.next()) {                      // List the file system contents
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("\tFS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }
  startUpAnimate();
}




void WebSocketServer() { // Start a WebSocket server
  webSocket.begin();                          // start the websocket server
  webSocket.onEvent(webSocketEvent);          // if there's an incomming websocket message, go to function 'webSocketEvent'
  Serial.println("WebSocket server started.");
  startUpAnimate();
}




void MDNServer() { // Start the mDNS responder
  MDNS.begin(MDNS_NAME);                        // start the multicast domain name server
  Serial.print("mDNS responder started: http://");
  Serial.print(MDNS_NAME);
  Serial.println(".local");
  // Serial.println(MDNS.isRunning());
  startUpAnimate();
}




void HTTPServer() { // Start a HTTP server with a file read handler and an upload handler
  server.on("/edit.html",  HTTP_POST, []() {  // If a POST request is sent to the /edit.html address,
    server.send(200, "text/plain", ""); 
  }, handleFileUpload);                       // go to 'handleFileUpload'

  // server.on("/",  HTTP_POST, []() {  // If a POST request is sent to the / address,
  //   // server.send(200, "text/plain", ""); 
  // }, handleUpdate);                       // go to 'handleFileUpload'
  server.on("/", HTTP_POST, handleUpdate);

  //server.on("/", HTTP_GET, handleRoot);

  server.onNotFound(handleNotFound);          // if someone requests any other file or page, go to function 'handleNotFound'
                                              // and check if the file exists

  server.begin();                             // start the HTTP server
  Serial.println("HTTP server started.");
  startUpAnimate();
}





// void startWiFi() {
//   WiFi.begin(WIFI_AP_NAME, WIFI_AP_PASS);             // Connect to the network
//   Serial.print("Connecting to ");
//   Serial.println(WIFI_AP_NAME); //Serial.println(" ...");

//   // int i = 0;
//   while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
//     Serial.print('.');
//     startUpAnimate();
//   }

//   Serial.println('\n');
//   Serial.println("Connection established!");  
//   Serial.print("IP address:\t");
//   Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
// }
