#define SKETCH "xmas2020.ino"

// Multicolor stripes with "glitter" flashes
// Was "xmasTree_OTA"
// Was "fastLED_twinkle_OTA"


//---------- wifi ----------
#define HOSTPREFIX "tree3"
#include "ESP8266WiFi.h"   //Not needed if also using the Arduino OTA Library...
//#include "D:\River Documents\Arduino\libraries\Kaywinnet.h"  \\ WiFi credentials
#include <Kaywinnet.h>     //WiFi credentials
char macBuffer[24];        //Holds the last three digits of the MAC, in hex.
char hostNamePrefix[] = HOSTPREFIX;
char hostName[24];         //Holds hostNamePrefix + the last three bytes of the MAC address.

// ---------- ota ----------
#include <ArduinoOTA.h>

// ---------- fastLED ----------
#define FASTLED_INTERNAL        //Pragma fix
#include <FastLED.h>


#define NUM_LEDS 786
#define NUM_TOP_LEDS 21
#define LED_PIN_1 D3    //Tree topper.
#define LED_PIN_2 D2    //Main tree part. 
#define COLOR_ORDER RGB
#define BRIGHTNESS 60
#define LED_TYPE WS2811
#define TOP_TYPE WS2811

CRGB topColor = CRGB::LightBlue;

CRGB topper[NUM_TOP_LEDS];            //Tree topper
CRGB leds[NUM_LEDS];                  //Array for the string of LEDS
///uint8_t data[NUM_LEDS];

int paletteNumber;
///unsigned long loopMillis;
///unsigned long topFlash;
//static int interval;
unsigned long endTime = 0;            //Used in glitter.ino
int state = 0;



//------------------ define palettes ---------------
#include "palettes.h"


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
  FastLED.addLeds<TOP_TYPE, LED_PIN_1, COLOR_ORDER>(topper, NUM_TOP_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);


  // Init the topper
  for (int i = 0; i < NUM_TOP_LEDS; i++) {
    topper[i] = topColor;
  }
  FastLED.show();

  paletteNumber = 5;        // Show the default palette first.
}



//----------------------- loop() -------------------
void loop() {
  ArduinoOTA.handle();
  ///loopMillis = millis();

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
