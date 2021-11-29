#define SKETCH "Pacifica.ino"
#define VERSION "2.00"           // Four characters

//
//  "Pacifica"
//  Gentle, blue-green ocean waves.
//  December 2019, Mark Kriegsman and Mary Corey March.
//  For Dan.
//


//---------- wifi ----------
#define HOSTPREFIX "XMAS-"        //18 chars max
#include "ESP8266WiFi.h"         //Not needed if also using the Arduino OTA Library...
#include <Kaywinnet.h>
char macBuffer[24];              //Holds the last three digits of the MAC, in hex.
char hostNamePrefix[] = HOSTPREFIX;
char hostName[24];               //Holds hostNamePrefix + the last three bytes of the MAC address.


// ---------- ota ----------
#include <ArduinoOTA.h>


// ------------ FastLED ------------
#define FASTLED_INTERNAL        // Pragma fix
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
FASTLED_USING_NAMESPACE
#include <myXmasOutsideLeds.h>
CRGB leds[NUM_LEDS];

#undef MAX_MA
#define MAX_MA 500
#undef BRIGHTNESS
#define BRIGHTNESS 20


// ------------ setup() ------------
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println(F(SKETCH));
  Serial.println(F("---------------"));

  setup_wifi();
  start_OTA();

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
  .setCorrection( TypicalLEDStrip );
  //FastLED.setMaxPowerInVoltsAndMilliamps( VOLTS, MAX_MA);
}




// ------------ loop() ------------
void loop() {
  ArduinoOTA.handle();

  EVERY_N_MILLISECONDS( 20) {
    pacifica_loop();
    topper(NUM_LEDS - NUM_TOP, NUM_LEDS);   //Handle the topper
    FastLED.show();
  }
}
