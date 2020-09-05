


ESP8266WebServer server(80);       // create a web server on port 80
WebSocketsServer webSocket(81);    // create a websocket server on port 81

File fsUploadFile;                                  // a File variable to temporarily store the received file

const char* mdnsName = "pillaroflight";             // Domain name for the mDNS responder
const char *OTAName = mdnsName;                     // has to be same as mdnsName or OTA will not work
const char *OTAPassword = "jesusisthelight";


uint16_t speed = 20;
uint16_t spark = 50;
uint16_t cooling = 50;
uint16_t textColor = CRGB::Blue;
String message;
bool textUpdate = false;
String effectName = "fire";

#define BAUD 115200


// FAST LED CONFIG
#define NUM_LEDS 85
#define LED_PIN 1
#define BRIGHTNESS 32

CRGB leds[NUM_LEDS];
CRGB rgbA = CRGB::Blue;
CRGB rgbB = CRGB::Aqua;
uint8_t brightness = BRIGHTNESS;

// Allow temporaly dithering, does not work with ESP32 right now
// #ifndef ESP32
// #define delay FastLED.delay
// #endif