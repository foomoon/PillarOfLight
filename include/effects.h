/*
 * paste in the Fire2012 code with a small edit at the end which uses the
 * setPixelColor() function to copy the color data to the ws2812fx instance. 
*/

// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//// 
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation, 
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking. 
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
// #define COOLING  50

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
// #define SPARKING 50

// CRGBPalette16 gPal = HeatColors_p;

CRGBPalette16 gPal = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);

// int c = hsv2rgb_spectrum(6);



void Fire2012()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((cooling * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k = NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < spark ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {

      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      leds[j] = ColorFromPalette( gPal, colorindex);
     
    }
}

void loadPalette(CRGB c1, CRGB c2, CRGB c3, CRGB c4) {

    typedef unsigned char byte;
    
    byte pal[16];
    pal[0] = 0; // 1st pos
    pal[1] = c1.r;
    pal[2] = c1.g;
    pal[3] = c1.b;

    pal[4] = 85; // 2nd pos
    pal[5] = c2.r;
    pal[6] = c2.g;
    pal[7] = c2.b;

    pal[8] = 170; // 3rd pos
    pal[9] = c3.r;
    pal[10] = c3.g;
    pal[11] = c3.b;

    pal[12] = 255; // 4th pos
    pal[13] = c4.r;
    pal[14] = c4.g;
    pal[15] = c4.b;

    gPal.loadDynamicGradientPalette(pal);
}


// in the custom effect run the Fire2012 algorithm
// uint16_t myCustomEffect() {
//   Fire2012();

//   // return the animation speed based on the ws2812fx speed setting
//   return (ws2812fx.getSpeed() / NUM_LEDS);
// }
