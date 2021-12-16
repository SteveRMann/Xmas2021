#define SKETCH "ledTest.ino"

//Simple sketch to light up all LEDs in each primary color.
//Red, Green, Blue, White, Black


//---------- wifi ----------
#define HOSTPREFIX "ledTest-"   //18 chars max
//#include "ESP8266WiFi.h"      //Not needed if also using the Arduino OTA Library...
char macBuffer[24];             //Holds the last three digits of the MAC, in hex.
char hostNamePrefix[] = HOSTPREFIX;
char hostName[24];              //Holds hostNamePrefix + the last three bytes of the MAC address.


// ---------- ota ----------
#include <ArduinoOTA.h>


// ---------- fastLED ----------
#define FASTLED_INTERNAL        // Pragma fix
#include <FastLED.h>
//#include <myXmasOutsideLeds.h>
#include "myTreeLeds.h"

#undef BRIGHTNESS
#define BRIGHTNESS 20

int colorPtr = 0;
const int colorCount = 5;
CRGB colors[colorCount] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White, CRGB::Black};
CRGB leds[NUM_LEDS];        // Array for the string of LEDS

//Top LEDs
CRGB topLeds[NUM_TopLeds];


//----------------------- setup() -------------------
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println(F(SKETCH));
  Serial.println(F("---------------"));

//  setup_wifi();
//  start_OTA();

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<TOP_TYPE, LED_TopPIN, TOP_COLOR_ORDER>(topLeds, NUM_TopLeds).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);


  // ----- Turn off all LEDs -----
  for (int iLed = 0; iLed < NUM_LEDS; iLed = iLed + 1) leds[iLed] = CRGB::Black;
  for (int iLed = 0; iLed < NUM_TopLeds; iLed = iLed + 1) topLeds[iLed] = CRGB::Black;
  FastLED.show();
  delay(1000);
}



//----------------------- loop() -------------------
void loop() {
  ArduinoOTA.handle();
  for (int i = 0; i < colorCount; i++) {
    for (int iLed = 0; iLed < NUM_LEDS; iLed = iLed + 1) leds[iLed] = colors[i];
    for (int tLed = 0; tLed < NUM_TopLeds; tLed = tLed + 1) topLeds[tLed] = colors[i];
    FastLED.show();
    FastLED.delay(1000);
  }
}
