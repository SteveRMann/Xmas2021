#define SKETCH "colorRings.ino"
#define test

/*
   Was showRings, but with dlays, (not dlay.h).
*/



//---------- wifi ----------
#define HOSTPREFIX "colorRings"      //18 chars max
#include "ESP8266WiFi.h"            //Not needed if also using the Arduino OTA Library...
#include <Kaywinnet.h>              //WiFi credentials
char macBuffer[24];                 //Holds the last three digits of the MAC, in hex.
char hostNamePrefix[] = HOSTPREFIX;
char hostName[24];                  //Holds hostNamePrefix + the last three bytes of the MAC address.


//---------- MQTT ----------
#include <ESP8266WiFi.h>        //Connect (and reconnect) an ESP8266 to the a WiFi network.
#include <PubSubClient.h>       //connect to a MQTT broker and publish/subscribe messages in topics.
#include <Kaywinnet.h>          //Net credentials.

//Declare an object of class WiFiClient, which allows to establish a connection to a specific IP and port
//Declare an object of class PubSubClient, which receives as input of the constructor the previously defined WiFiClient.
//The constructor MUST be unique on the network.
WiFiClient colrerings;
PubSubClient client(colrerings);

#define NODENAME "colorRings"                               //Give this node a name
const char *cmndTopic = NODENAME "/cmnd";                   //Incoming commands, payload is a command.
const char *ledTopic = NODENAME "/led";                     //LED number to light (for identifying bad leds).
const char *connectName =  NODENAME "2";                    //Must be unique on the network
const char *mqttServer = MQTT_SERVER;                       //Broker credentials defined in Kaywinnet.h
const int mqttPort = MQTT_PORT;

//Build an array of topics to subscribe to in mqttConnect()
static const char *mqttSubs[] = {
  cmndTopic,
  ledTopic
};



// ---------- ota ----------
#include <ArduinoOTA.h>


// ---------- Timers ----------
//#include <dlay.h>
//dlay dripTimer;
//dlay sequenceTimer;
//dlay sequenceDelay;



// ---------- fastLED ----------
#define FASTLED_INTERNAL        // Pragma fix
#include <FastLED.h>

// ----- Tree LEDs -----
#define NUM_LEDS 785
#define TREE_PIN D2
#define COLOR_ORDER RGB
#define BRIGHTNESS 60
#define LED_TYPE WS2811
CRGB leds[NUM_LEDS];            // Array for the string of tree LEDS
uint8_t data[NUM_LEDS];

// ----- Topper LEDS -----
#define TOP_DATA_PIN    D3
#define TOP_LED_TYPE    WS2811
#define TOP_COLOR_ORDER RGB
#ifdef test
#define TOP_NUM_LEDS    24
#else
#define TOP_NUM_LEDS    21
#endif
CRGB topper[TOP_NUM_LEDS];
CRGB topColor = CRGB::Yellow;
CRGB glitterColor = CRGB::Blue;

//Used in glitter.ino
unsigned long endTime = 0;
int state = 0;
int topBlink;
enum {top_BlinkOn, top_BlinkOff};
unsigned long blinkTime = 50ul;                         //How long the blink is lasts.
unsigned int topInterval = random(1000ul, 6000ul);      //How long between blinks




int colorPtr = 0;
const int COLOR_COUNT = 5;
CRGB colors[COLOR_COUNT] = {CRGB::Red, CRGB::Green, CRGB::Orange, CRGB::Blue, CRGB::Purple};
int colorNumber = 0;


int paletteNumber;
//unsigned long topFlash;;


//define palettes and LED Ring Arrays
#include "palettes.h"
#include "rings.h"            //ringCount is initialized here.

int ringPtr = 0;              //Current ring number
int dripSpeed = 25;           //Smaller is faster
#ifdef test
int dripPeriod = 3000;        //How often a drip starts
#else
int dripPeriod = 15000;
#endif

bool diagFlag = false;        //Set true when an LED number comes over MQTT.
int diagLED = 0;              //LED number


//----------------------- setRing() -------------------
//Set a selected ring to a single color
//Enter with ring number, led count in the ring, and the color.
//Example:
//  setRing(3,sizeof(ring[3]),RED;
//  FastLED.show();

void setRing(int rngNum, int rlc, CRGB rngColor) {
  for (int i = 0; i < rlc; i++) {
    leds[ring[rngNum][i]] = rngColor;
  }
}



// ========================== fadeall ==========================
void fadeAll()  {
  //Every call to fadeall() reduces the intensity of all by about 1%
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(250);
  }
}




void  fadeUp () {
  static int FastLED_fade_counter = 1;
  for ( int i = 0; i < NUM_LEDS; i++ )
  {
    leds[i] = CRGB::Orange; // Can be any colour
    leds[i].maximizeBrightness(FastLED_fade_counter);
  }
  FastLED.show();
  FastLED_fade_counter ++ ;   // Increment
}


// -------------------- dlay() --------------------
void dlay(unsigned long waitTime) {
  //Delay waitTime ms, but keep the critical stuff in the delay loop
  //Put dlay(0); at the top of loop() to make sure the critical stuff is handled properly.
  unsigned long endTime = millis() + waitTime;    //Calculate the end time.
  do {
    ESP.wdtFeed();
    ArduinoOTA.handle();
    top_glitter();
    //Make sure we stay connected to the mqtt broker
    if (!client.connected()) {
      mqttConnect();
    }
    client.loop();                      //Check for MQTT messages
    if (waitTime == 0) return;
  } while (millis() <= endTime);
}



//----------------------- setup() -------------------
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println(F(SKETCH));
  Serial.println(F("---------------"));

  setup_wifi();
  start_OTA();
  client.setServer(mqttServer, mqttPort);
  mqttConnect();


  delay(10);
  FastLED.addLeds<LED_TYPE, TREE_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<TOP_LED_TYPE, TOP_DATA_PIN, COLOR_ORDER>(topper, TOP_NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);


  // Init the topper
  for (int i = 0; i < TOP_NUM_LEDS; i++) {
    topper[i] = topColor;
  }
  FastLED.show();
  endTime = millis();             //Init the top glitter



  // ----- Turn off all LEDs -----
  for (int iLed = 0; iLed < NUM_LEDS; iLed = iLed + 1) leds[iLed] = CRGB::Black;
  FastLED.show();
  delay(100);
  // ----------------------------

}




//---------------------------- loop() ------------------------
void loop() {
  dlay(0);                //Handle the critical stuff

  if (diagFlag) {
    // Turn off all LEDs
    for (int iLed = 0; iLed < NUM_LEDS; iLed = iLed + 1) leds[iLed] = CRGB::Black;
    // Turn on the selected LED
    leds[diagLED] = CRGB::Blue;
    FastLED.show();
    //Serial.print(F("Turn on LED# "));
    //Serial.println(diagLED);
    dlay(10);
    return;
  }

  // Drip
  for (int ringPtr = 0; ringPtr < ringCount; ringPtr++) {     //For each ring
    setRing(ringPtr, ledsPerRing, colors[colorNumber]);       //Paint the color
    FastLED.show();
    dlay(dripSpeed);
  }

  dlay(dripPeriod);                                          //Hold the solid color
  colorNumber++;                                              //Next color.
  if (colorNumber >= COLOR_COUNT) colorNumber = 0;

}
