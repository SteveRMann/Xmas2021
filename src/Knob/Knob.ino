#define SKETCH "Knob.ino"

/*
  Project that reads a Rotary Encoder then
  publishes the raw value to MQTT topic knob/value, and
  the direction to knob/direction.
  The results are also displayed on the Serial Monitor

  This sketch is a companion to my Christmas displays.
  With over700 LEDs, there's bound to be one that is missing a primary color.
  (See colorRings.ino)
  The dissplay program is running MQTT pubSub and looking for two MQTT messages:
  Topic: colorRings/led, msg: LED Number (example: led 35)
  Topic: colorRings/cmnd, msg: reset Returns display to normal use.
  Once you have the LED number of the bad LED, you can mute it individually.

  Inspiration:
  DroneBot Workshop 2019, https://dronebotworkshop.com
*/

//---------- wifi ----------
#define HOSTPREFIX "knob"           //18 chars max
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
WiFiClient knobb;
PubSubClient client(knobb);

#define NODENAME "knob"                                     //Give this node a name
const char *cmndTopic = NODENAME "/cmnd";                   //Incoming commands, payload is a command.
const char *rawValueTopic = NODENAME "/value";              //Publish topics
const char *directionTopic = NODENAME "/direction";
const char *connectName =  NODENAME "1";                    //Must be unique on the network
const char *ledValueTopic = "colorRings/led";
const char *ledCmndTopic = "colorRings/cmnd";

const char *mqttServer = MQTT_SERVER;                       //Broker credentials defined in Kaywinnet.h
const int mqttPort = MQTT_PORT;

//Build an array of topics to subscribe to in mqttConnect()
static const char *mqttSubs[] = {
  cmndTopic
};


// ---------- ota ----------
#include <ArduinoOTA.h>


// ---------- Button Handler ----------
#include <ezButton.h>
ezButton resetButton(D4);        //create Button object that attach to pin D4;

// ----- Rotary Encoder -----
#define INPUTA D1           //A (CLK)
#define INPUTB D3           //B (DT)
#define BUTTON D4           //Button, if the encoder has one.

// LED Outputs
#define ledCW D6
#define ledCCW D7

int counter = 0;
int currentStateCLK;
int previousStateCLK;

char encoderDirection[] = "CCW";

//--------------- OLED --------------
#include <Wire.h>
//#include "SSD1306.h"
#include <Adafruit_SSD1306.h>
SSD1306  display(0x3C, D2, D5); //Address 0x3C, D2 (SDA/Serial Data), and D5 (SCK/Serial Clock).



//--------------- showCounter ---------------
void showCounter() {
  Serial.print("Direction: ");
  Serial.print(encoderDirection);
  Serial.print(",  Value: ");
  Serial.println(counter);

  char buffer[10];
  sprintf(buffer, "%d", counter);
  client.publish(rawValueTopic, buffer);
  client.publish(ledValueTopic, buffer);
  Serial.print(F("char buffer= "));
  Serial.println(buffer);

  display.clear();               //clear the internal RAM
  display.drawString(0, 0, buffer);
  display.display();
}

// ---------------- setup ----------------
void setup() {
  Serial.begin (115200);
  Serial.println();
  Serial.println();
  Serial.println(SKETCH);

  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  mqttConnect();

  Serial.println(F("Publish topics:"));
  Serial.print(F("  "));
  Serial.println(rawValueTopic);
  Serial.print(F("  "));
  Serial.println(directionTopic);


  pinMode (INPUTA, INPUT);
  pinMode (INPUTB, INPUT);
  pinMode (BUTTON, INPUT_PULLUP);
  pinMode (ledCW, OUTPUT);
  pinMode (ledCCW, OUTPUT);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  resetButton.setDebounceTime(50);                 //debounce time in milliseconds

  // Read the initial state of INPUTA
  // Assign to previousStateCLK variable
  previousStateCLK = digitalRead(INPUTA);

}


// ---------------- loop ----------------
void loop() {
  ArduinoOTA.handle();

  //Make sure we stay connected to the mqtt broker
  if (!client.connected()) mqttConnect();
  client.loop();                             //Check for MQTT messages

  resetButton.loop();
  if (resetButton.isPressed()) {
    counter = 0;                            //Reset the count if pressed.
    digitalWrite(ledCW, LOW);               //Turn off the LEDS
    digitalWrite(ledCCW, LOW);
    strcpy(encoderDirection, "---");
    showCounter();
    client.publish(ledCmndTopic, "reset");  //Tell the display to resume normal operations
  }

  //Has a pulse occurred?
  //If the previous and the current state of the INPUTA are different then a pulse has occured
  currentStateCLK = digitalRead(INPUTA);
  if (currentStateCLK != previousStateCLK) {

    // If the INPUTB state is different than the INPUTA state then
    // the encoder is rotating counterclockwise
    if (digitalRead(INPUTB) != currentStateCLK) {
      counter --;
      strcpy(encoderDirection, "CCW");
      digitalWrite(ledCW, LOW);
      digitalWrite(ledCCW, HIGH);

    } else {
      // Encoder is rotating clockwise
      counter ++;
      strcpy(encoderDirection, " CW");
      digitalWrite(ledCW, HIGH);
      digitalWrite(ledCCW, LOW);

    }
    showCounter();
  }

  // Update previousStateCLK with the current state
  previousStateCLK = currentStateCLK;
}
