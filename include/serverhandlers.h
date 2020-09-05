#include <ArduinoJson.h>

/*__________________________________________________________SERVER_HANDLERS__________________________________________________________*/

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";          // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) { // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                         // Use the compressed verion
    File file = SPIFFS.open(path, "r");                    // Open the file
    size_t sent = server.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);   // If the file doesn't exist, return false
  return false;
}

void handleFileUpload(){ // upload a new file to the SPIFFS
  HTTPUpload& upload = server.upload();
  String path;
  if(upload.status == UPLOAD_FILE_START){
    path = upload.filename;
    if(!path.startsWith("/")) path = "/"+path;
    if(!path.endsWith(".gz")) {                          // The file server always prefers a compressed version of a file 
      String pathWithGz = path+".gz";                    // So if an uploaded file is not compressed, the existing compressed
      if(SPIFFS.exists(pathWithGz))                      // version of that file must be deleted (if it exists)
         SPIFFS.remove(pathWithGz);
    }
    Serial.print("handleFileUpload Name: "); Serial.println(path);
    fsUploadFile = SPIFFS.open(path, "w");            // Open the file for writing in SPIFFS (create if it doesn't exist)
    path = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile) {                                    // If the file was successfully created
      fsUploadFile.close();                               // Close the file again
      Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
      server.sendHeader("Location","/success.html");      // Redirect the client to the success page
      server.send(303);
    } else {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }
}

int updateFile(String text) {

  text.replace("description=","");

  int bytesWritten = 0;
  File file = SPIFFS.open("/gradient.json", "w");
  if (!file) {
    Serial.println("Error opening file for writing");
    return bytesWritten;
  }

  bytesWritten = file.print(text);

  if (bytesWritten > 0) {
    Serial.println("File was written");
    Serial.println(bytesWritten);

  } else {
    Serial.println("File write failed");
  }

  file.close();
  return bytesWritten;
}

void handleUpdate() { //Handler for the body path
 
      if (server.hasArg("plain")== false){ //Check if body received
 
            // server.send(200, "text/plain", "Body not received");
            server.send(500, "text/plain", "500: could not save settings");
            return;
 
      }

      if (updateFile(server.arg("plain"))) {
        server.sendHeader("Location","/success.html");      // Redirect the client to the success page
        server.send(303);
      } else {
        server.send(500, "text/plain", "500: could not save settings");
      }
 
      
      // String message = "Body received:\n";
      //        message += server.arg("plain");
      //        message += "\n";
 
      // server.send(200, "text/plain", message);
      // Serial.println(message);

}




void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) { // When a WebSocket message is received
  switch (type) {
    case WStype_DISCONNECTED:             // if the websocket is disconnected
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {              // if a new websocket connection is established
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        // rainbow = false;                  // Turn rainbow off when a new connection is established
      }
      break;
    case WStype_TEXT:                     // if new text data is received
      Serial.printf("[%u] Recieved: %s\n", num, payload);
      if (payload[0] == '#') {            // we get RGB data
        uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);   // decode rgb data
        int r = ((rgb >> 16) & 0x3FF);                     // 8 bits per color, so R: bits 16-23
        int g = ((rgb >> 8) & 0x3FF);                     // G: bits 8-15
        int b =          rgb & 0x3FF;                      // B: bits  0-7

    
        textColor = (uint16_t) CRGB(r,g,b);

        

      // } else if (payload[0] == 'R') {                      // the browser sends an R when the rainbow effect is enabled
      //   rainbow = true;
      // } else if (payload[0] == 'N') {                      // the browser sends an N when the rainbow effect is disabled
      //   rainbow = false;
      } else {
        
        

        if (length>0) {
          

          DynamicJsonDocument doc(1024);
          deserializeJson(doc, (const char *) &payload[0]);
          JsonObject obj = doc.as<JsonObject>();


          // Parse Text String
          if (obj.containsKey("text")) {
            String oldMessage = message;
            message = obj["text"].as<String>();
            // Only update if message changed
            if (oldMessage != message) {
              textUpdate = true;
            }
          } 

          JsonObject effect;
          if (obj.containsKey("selected") && obj.containsKey("effects")) {
            effectName = obj["selected"].as<String>(); // defaults to "fire" for backward compatability
            effect = obj["effects"][effectName];
          } 
          
          if (effect.containsKey("data")) {
            effect = effect["data"];
          } else {
            effect = obj;
          }

          // Parse Color
          if (effect.containsKey("color")) {
            String colstr = effect["color"].as<String>();
            colstr = colstr.substring(1);
            // const char* colptr = colstr.c_str();
            // message = String(colptr);
            // uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);   // decode rgb data
            uint32_t rgb = (uint32_t) strtol(colstr.c_str(), NULL, 16);   // decode rgb datas
            int r = ((rgb >> 16) & 0xFF);                     // 8 bits per color, so R: bits 16-23
            int g = ((rgb >> 8) & 0xFF);                     // G: bits 8-15
            int b =          rgb & 0xFF;                      // B: bits  0-7

            textColor = (uint16_t) CRGB(r,g,b);

            
          }

          // Parse Brightness
          if (effect.containsKey("brightness")) {
            brightness = effect["brightness"]["value"].as<uint8_t>();
          }

          // Parse Speed
          if (effect.containsKey("speed")) {
            speed = effect["speed"]["value"].as<uint16_t>();
          }

          // Parse Intensity
          if (effect.containsKey("intensity")) {
            spark = effect["intensity"]["value"].as<uint16_t>();
          }

          if (effect.containsKey("spark")) { // backward compatibility
            spark = effect["spark"].as<uint16_t>();
          }

          // Parse Cooling
          if (effect.containsKey("size")) {
            cooling = effect["size"]["value"].as<uint16_t>();
          }
          if (effect.containsKey("cooling")) { // backward compatibility
            cooling = effect["cooling"].as<uint16_t>();
          }

          // Parse Gradient
          if (effect.containsKey("gradient")) {
            typedef unsigned char byte;
            byte pal[16];
            int n = 0;
            JsonArray array = effect["gradient"]["colors"].as<JsonArray>();
            if (array.size() == 4) {
              for(JsonVariant v : array) {
                // Serial.println(v["r"].as<int>());
                // String result = v["r"].as<String>();
                // webSocket.sendTXT(num, result);
                pal[n++] = v["pos"].as<uint8_t>();
                pal[n++] = v["r"].as<uint8_t>();
                pal[n++] = v["g"].as<uint8_t>();
                pal[n++] = v["b"].as<uint8_t>();
                // String result = String(pal[n-4]);
                // webSocket.sendTXT(num, result);
              }
              webSocket.sendTXT(num, "Loading Dynamic Palette");
              gPal.loadDynamicGradientPalette(pal);
            };
            

          }

          // Parse HSV
          // Here we have to scale Hue from 0-360 to 0-255
          // Also, we create a second hue that is +/-60 deg out of phase 
          if (effect.containsKey("hsv")) {
            // webSocket.sendTXT(num, payload, length);
            
            
            uint16_t h = effect["hsv"]["h"].as<uint16_t>();
            uint8_t s = effect["hsv"]["s"].as<uint8_t>();
            uint8_t v = effect["hsv"]["v"].as<uint8_t>();

            

            uint16_t h2 = (h<180) ? (h + 60) : (h - 60); // create a second hue at 60deg out of phase
            h = map(h,0,359,0,255); // Scale from 0-360 to 0-255 for FastLED maths
            s = map(s,0,100,0,255); // Scale from 0-360 to 0-255 for FastLED maths
            v = map(v,0,100,0,255); // Scale from 0-360 to 0-255 for FastLED maths
            h2 = map(h2,0,359,0,255);
            // CRGB rgbA;
            // CRGB rgbB;
            CHSV hsvA = CHSV(h,s,v);
            CHSV hsvB = CHSV(h2,s,v);
            hsv2rgb_spectrum(hsvA,rgbA);
            hsv2rgb_spectrum(hsvB,rgbB);

            // uint16_t tmp = (uint16_t) hsvA.h;
            // String txt = String(rgbA.r);
            // webSocket.sendTXT(num, txt);
            
          }
          
          
          // Serial.print("(");
          // Serial.print(length);
          // Serial.print(") Text being entered: ");
          // Serial.println(message);
          // sprintf(message,"%s",payload);
        }
        
      }
      break;
  }
}

void handleNotFound(){ // if the requested file or page doesn't exist, return a 404 not found error
  if(!handleFileRead(server.uri())){          // check if the file exists in the flash memory (SPIFFS), if so, send it
    server.send(404, "text/plain", "404: File Not Found");
  }
}