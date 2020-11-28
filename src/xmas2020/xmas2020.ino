#define SKETCH "xmas2020.ino"

// Multicolor stripes with "glitter" flashes
// Was "xmasTree_OTA"
// Was "fastLED_twinkle_OTA"


//---------- wifi ----------
#define HOSTPREFIX "tree3"
#include "ESP8266WiFi.h"   //Not needed if also using the Arduino OTA Library...
#include "D:\River Documents\Arduino\libraries\Kaywinnet.h"  \\ WiFi credentials
char macBuffer[24];       //Holds the last three digits of the MAC, in hex.
char hostNamePrefix[] = HOSTPREFIX;
char hostName[24];        //Holds hostNamePrefix + the last three bytes of the MAC address.

// ---------- ota ----------
#include <ArduinoOTA.h>

// ---------- fastLED ----------
#define FASTLED_INTERNAL        // Pragma fix
#include <FastLED.h>


#include <ArduinoOTA.h>

#define NUM_LEDS 785
#define LED_PIN_1 D2    //Tree topper.
#define LED_PIN_2 D1
#define COLOR_ORDER RGB
#define BRIGHTNESS 15
#define LED_TYPE WS2811
#define TOP_TYPE WS2812

CRGB topColor = CRGB::Blue;

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
  FastLED.addLeds<LED_TYPE, LED_PIN_2, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<TOP_TYPE, LED_PIN_1, COLOR_ORDER>(top, 1).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);


  // Init the topper //****
  top[0] = CRGB::Blue;
  FastLED.show();

  paletteNumber = 5;        // Show the default palette first.
}



//----------------------- loop() -------------------
void loop() {
  ArduinoOTA.handle();
  loopMillis = millis();

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
