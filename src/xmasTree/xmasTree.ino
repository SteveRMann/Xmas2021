#define SKETCH "xmasTree_OTA.ino"

// Multicolor stripes with "glitter" flashes
// Was "fastLED_twinkle_OTA"
// Change between pallettes is abrupt.

#define FASTLED_INTERNAL        // Pragma fix
#include<FastLED.h>
#include "ESP8266WiFi.h"
#include <Kaywinnet.h>
#include <ArduinoOTA.h>


#include "myLeds.h"
#define LED_PIN_TREE LED_PIN
#undef BRIGHTNESS                 //Redefine brightness.
#define BRIGHTNESS 25

// Tree Topper
#define LED_PIN_TOP D3
#define TOP_TYPE WS2812

CRGB topColor=CRGB::Blue;

CRGB top[1];                // Tree topper, one bulb  //****
CRGB leds[NUM_LEDS];        // Array for the string of LEDS
uint8_t data[NUM_LEDS];

int paletteNumber;
unsigned long loopMillis; 
unsigned long topFlash;; 
static int interval;
unsigned long endTime = 0;
int state = 0;



//------------------ define palettes ---------------
CRGBPalette16 gPalette (
  CRGB::Violet, CRGB::Violet,
  CRGB::Red,   CRGB::Red,  CRGB::Red,  CRGB::Red,
  CRGB::Blue,  CRGB::Blue, CRGB::Blue, CRGB::Blue,
  CRGB::Green,  CRGB::Green, CRGB::Green, CRGB::Green,
  CRGB::Violet, CRGB::Violet
);

CRGBPalette16 xmasPalette (
  CRGB::Blue, CRGB::Blue, CRGB::Red, CRGB::Red,
  CRGB::Red,  CRGB::Blue, CRGB::Red, CRGB::Green,
  CRGB::Gold, CRGB::Gold, CRGB::Violet, CRGB::Green,
  CRGB::Green, CRGB::Green, CRGB::Gold, CRGB::Yellow
);

CRGBPalette16 xPalette (
  CRGB::DimGray, CRGB::DimGray, CRGB::DimGray, CRGB::DimGray,
  CRGB::DimGray, CRGB::DimGray, CRGB::DimGray, CRGB::DimGray,
  CRGB::DimGray, CRGB::DimGray, CRGB::DimGray, CRGB::DimGray,
  CRGB::DimGray, CRGB::DimGray, CRGB::DimGray, CRGB::Coral
);

CRGBPalette16 PaletteBlue (
  CRGB::Turquoise, CRGB::SteelBlue, CRGB::SkyBlue, CRGB::RoyalBlue,
  CRGB::Purple,  CRGB::PowderBlue, CRGB::PaleTurquoise, CRGB::Orchid,
  CRGB::MediumVioletRed, CRGB::MediumTurquoise, CRGB::MediumSlateBlue, CRGB::MediumPurple,
  CRGB::MediumOrchid, CRGB::MediumBlue, CRGB::LightSkyBlue, CRGB::DodgerBlue
);

CRGBPalette16 rgbPalette (
  CRGB::Blue, CRGB::Red, CRGB::Green, CRGB::Blue,
  CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Red,
  CRGB::Green, CRGB::Blue, CRGB::Red, CRGB::Green,
  CRGB::Blue, CRGB::Red, CRGB::Green, CRGB::Blue
);


//----------------------- setup() -------------------
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println(F(SKETCH));
  Serial.println(F("---------------"));

  setup_wifi();
  start_OTA();

  delay(1000);
  FastLED.addLeds<LED_TYPE, LED_PIN_TREE, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<TOP_TYPE, LED_PIN_TOP, COLOR_ORDER>(top, 1).setCorrection(TypicalLEDStrip); 
  FastLED.setBrightness(BRIGHTNESS);

  
  // Init the topper //****
  top[0] = CRGB::Blue;
  FastLED.show();

  paletteNumber = 5;        // Show the default palette first.
}



//----------------------- loop() -------------------
void loop() {
  ArduinoOTA.handle();
  loopMillis=millis();
  
  EVERY_N_SECONDS(5) {
    next_palette();
  }

  fill_data_array();
  render_data_with_palette(paletteNumber);
  add_glitter();
  top_glitter();

  FastLED.show();
  FastLED.delay(20); // Higher is slower changes
  
}
