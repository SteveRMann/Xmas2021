#define FASTLED_INTERNAL        // Pragma fix
#include "FastLED.h"


// ColorWavesWithPalettes
// Animated shifting color waves, with several cross-fading color palettes.
// by Mark Kriegsman, August 2015
//
// Color palettes courtesy of cpt-city and its contributors:
//   http://soliton.vm.bytemark.co.uk/pub/cpt-city/
//
// Color palettes converted for FastLED using "PaletteKnife" v1:
//   http://fastled.io/tools/paletteknife/
//


//---------- wifi ----------
#define HOSTPREFIX "waves-"   //18 chars max
#include "ESP8266WiFi.h"    //Not needed if also using the Arduino OTA Library...
#include <Kaywinnet.h>
char macBuffer[24];         //Holds the last three digits of the MAC, in hex.
char hostNamePrefix[] = HOSTPREFIX;
char hostName[24];          //Holds hostNamePrefix + the last three bytes of the MAC address.

// ---------- ota ----------
#include <ArduinoOTA.h>

// ---------- Tree ----------
#include "myLeds.h"
CRGB leds[NUM_LEDS];

// Topper
#define TOP_TOPPER_PIN    D3
#define TOP_LED_TYPE    WS2811
#define TOP_COLOR_ORDER RGB
#define TOP_NUM_LEDS    21
CRGB topper[TOP_NUM_LEDS];
//CRGB topColor = CRGB::LightSkyBlue;
CRGB topColor = CRGB::BlanchedAlmond;
CRGB glitterColor = CRGB::Blue;

//Used in glitter.ino
unsigned long endTime = 0;
int state = 0;


// ten seconds per color palette makes a good demo
// 20-120 is better for deployment
#define SECONDS_PER_PALETTE 10


void setup() {
  setup_wifi();

  // tell FastLED about the LED configurations
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
  .setDither(BRIGHTNESS < 255);
  //FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS, MAX_MA);

  FastLED.addLeds<TOP_LED_TYPE, TOP_TOPPER_PIN, TOP_COLOR_ORDER>(topper, TOP_NUM_LEDS)
  .setDither(BRIGHTNESS < 255);
  //FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS, MAX_MA);

  FastLED.setBrightness(BRIGHTNESS);


  // Init the topper
  for (int i = 0; i < TOP_NUM_LEDS; i++) {
    topper[i] = topColor;
  }

  // ----- Turn off all LEDs -----
  for (int iLed = 0; iLed < NUM_LEDS; iLed = iLed + 1) leds[iLed] = CRGB::Black;
  FastLED.show();
  for (int iLed = 0; iLed < TOP_NUM_LEDS; iLed = iLed + 1) leds[iLed] = CRGB::Black;
  FastLED.show();
}


// Forward declarations of an array of cpt-city gradient palettes, and
// a count of how many there are.  The actual color palette definitions
// are at the bottom of this file.
extern const TProgmemRGBGradientPalettePtr gGradientPalettes[];
extern const uint8_t gGradientPaletteCount;

// Current palette number from the 'playlist' of color palettes
uint8_t gCurrentPaletteNumber = 0;

CRGBPalette16 gCurrentPalette( CRGB::Black);
CRGBPalette16 gTargetPalette( gGradientPalettes[0] );



void loop() {
  ArduinoOTA.handle();

  EVERY_N_SECONDS( SECONDS_PER_PALETTE ) {
    gCurrentPaletteNumber = addmod8( gCurrentPaletteNumber, 1, gGradientPaletteCount);
    gTargetPalette = gGradientPalettes[ gCurrentPaletteNumber ];
  }

  EVERY_N_MILLISECONDS(40) {
    nblendPaletteTowardPalette( gCurrentPalette, gTargetPalette, 16);
  }

  colorwaves( leds, NUM_LEDS, gCurrentPalette);
  //top_glitter();

  FastLED.show();
  FastLED.delay(20);
}



// Alternate rendering function just scrolls the current palette
// across the defined LED strip.
void palettetest( CRGB* ledarray, uint16_t numleds, const CRGBPalette16& gCurrentPalette)
{
  static uint8_t startindex = 0;
  startindex--;
  fill_palette( ledarray, numleds, startindex, (256 / NUM_LEDS) + 1, gCurrentPalette, 255, LINEARBLEND);
}

#include "palettes.h"
