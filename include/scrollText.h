// #include "config.h"



class sText {
    private:
        uint8_t size = max(int(mh/8), 1);
        String displayText;
        uint8_t messageLength = 0;
        int x;
        uint16_t frameTime;
        uint16_t defaultFrameTime=120;
        uint16_t myColor = CRGB::Blue;
        
    public:
        //
        void begin(String txt) {
            // Serial.print(txt);
            
            matrix->clear();
            matrix->setTextWrap(false);  // we don't wrap text so it scrolls nicely
            matrix->setRotation(0);
            matrix->setTextSize(size);
            matrix->setTextColor(myColor);
            matrix->setCursor(0,0);
            displayText = txt;
            messageLength = displayText.length();
            x=8*size;
            defaultFrameTime = 120; // milliseconds
            frameTime = defaultFrameTime;
        }

        void run() {
            //  for (int16_t x=8*size; x>=-messageLength*8*size; x--) {
            if (x>=-messageLength*6*size) {
                // yield();
                EVERY_N_MILLISECONDS_I(timerObj, defaultFrameTime) {
                    timerObj.setPeriod(frameTime);
                    matrix->clear(); 
                    matrix->setTextColor(myColor);
                    matrix->setCursor(x,mh/2-size*4);
                    matrix->print(displayText);
                    matrix->show();
                    x--; // decriment one pixel distance for cursor in x direction
                }
                
            } else {
                begin(displayText);
            }
            
        }

        void setFrameTime(uint16_t t) {
            frameTime = t;
        }

        void setTextColor(uint16_t color) {
            myColor = color;
            Serial.println(myColor);
            // matrix->setTextColor(color);
        }

       
};