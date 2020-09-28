#include <ArduinoJson.h>

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

