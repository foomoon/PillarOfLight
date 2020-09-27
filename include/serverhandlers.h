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
 
      if (server.hasArg("plain") == false) { //Check if body received
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


void handleNotFound() { // if the requested file or page doesn't exist, return a 404 not found error
  if(!handleFileRead(server.uri())){          // check if the file exists in the flash memory (SPIFFS), if so, send it
    //TODO: Redirect to a 404.html
    server.send(404, "text/plain", "404: File Not Found");
  }
}



void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) { // When a WebSocket message is received
  switch (type) {
    case WStype_FRAGMENT_BIN_START:
      webSocket.sendTXT(num, "WStype_FRAGMENT_BIN_START");
      break;
    case WStype_FRAGMENT_TEXT_START:
      webSocket.sendTXT(num, "WStype_FRAGMENT_TEXT_START");
      break;
    case WStype_DISCONNECTED:             // if the websocket is disconnected
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {              // if a new websocket connection is established
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:                     // if new text data is received
      
      Serial.printf("[%u] Recieved: %s\n", num, payload);
      //webSocket.sendTXT(num,  payload, length);
      
  
      if (length>0) {

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, (const char *) &payload[0]);
        JsonObject effect = doc.as<JsonObject>();

        if (effect.containsKey("data")) {
          effectName = effect["name"].as<String>();
          effect = effect["data"];
        } 
        
        // Parse Brightness
        if (effect.containsKey("brightness")) {
          brightness = effect["brightness"]["value"].as<uint8_t>();
        }
        // Parse Speed
        if (effect.containsKey("speed")) {
          speed = 1000 / effect["speed"]["value"].as<uint16_t>();
        }
        // Parse Intensity
        if (effect.containsKey("intensity")) {
          spark = effect["intensity"]["value"].as<uint16_t>();
        }
        // Parse Cooling
        if (effect.containsKey("size")) {
          cooling = effect["size"]["value"].as<uint16_t>();
        }


        // Parse Gradient
        if (effect.containsKey("gradient")) {
          typedef unsigned char byte;
          byte pal[16];
          int n = 0;
          JsonArray colorStopArr = effect["gradient"]["value"].as<JsonArray>();
          // TODO: Need to check if "pal" should be resized based on size of colorStopArr
          //       byte pal[colorStopArr.size()];
          //       ASKING FOR A BUFFER OVERFLOW HERE!!! SHAME ON ME
          if (colorStopArr.size() > 0) {
            for(JsonVariant colorStop : colorStopArr) {
              pal[n++] = colorStop["pos"].as<uint8_t>();
              pal[n++] = colorStop["r"].as<uint8_t>();
              pal[n++] = colorStop["g"].as<uint8_t>();
              pal[n++] = colorStop["b"].as<uint8_t>();
            }
            gPal.loadDynamicGradientPalette(pal);
          };
        }


         // // Parse Color
        // if (effect.containsKey("color")) {
        //   String colstr = effect["color"]["value"].as<String>();
        //   colstr = colstr.substring(1);
        //   // const char* colptr = colstr.c_str();
        //   // message = String(colptr);
        //   // uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);   // decode rgb data
        //   uint32_t rgb = (uint32_t) strtol(colstr.c_str(), NULL, 16);   // decode rgb datas
        //   int r = ((rgb >> 16) & 0xFF);                     // 8 bits per color, so R: bits 16-23
        //   int g = ((rgb >> 8) & 0xFF);                     // G: bits 8-15
        //   int b =          rgb & 0xFF;                      // B: bits  0-7
        //
        //   textColor = (uint16_t) CRGB(r,g,b);
        // }


      }
        
      
      break;
  }
}

