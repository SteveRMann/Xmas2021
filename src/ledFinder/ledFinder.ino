#define SKETCH "ledFinder2.ino"

/*
   Need to try different pins for the encoder because D1 and D2 are needed for i2c (display)
*/

/**************************************************************************
  This sketch lights one LED at a time from the rotary encoder count.
  Using a 128x32 pixel display using I2C on a Wemos D1 Mini
  Rotary encoder on SDA(D2)/SCL(D1) pins
  **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixels

// Declare the SSD1306 display connected to I2C: SDA(D2), SCL(D1) pins.
// On the Wemos D1 Mini:   D2(SDA),  D1(SCL)
#define OLED_RESET     0    // Reset pin # (0 for Wemos or ESP8266)
#define SCREEN_ADDRESS 0x3C // 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



// ----- Rotary Encoder -----
#define INPUTA D5           //A (CLK)
#define INPUTB D6           //B (DT)
#define BUTTON D4           //Button, if the encoder has one.

// LED Outputs
#define ledCW D7
#define ledCCW D3

int counter = 0;
int currentStateCLK;
int previousStateCLK;

char encoderDirection[] = "CCW";


// ---------------- setup ----------------
void setup() {
  pinMode (INPUTA, INPUT);
  pinMode (INPUTB, INPUT);
  pinMode (BUTTON, INPUT_PULLUP);
  pinMode (ledCW, OUTPUT);
  pinMode (ledCCW, OUTPUT);


  Serial.begin (115200);
  Serial.println();
  Serial.println(SKETCH);


  // Read the initial state of INPUTA
  // Assign to previousStateCLK variable
  previousStateCLK = digitalRead(INPUTA);


  // Generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();

}


// ---------------- loop ----------------
void loop() {

  if (digitalRead(BUTTON) == 0) {
    counter = 0;        //Reset the count if pressed.
    digitalWrite(ledCW, LOW);         //Turn off the LEDS
    digitalWrite(ledCCW, LOW);
    strcpy(encoderDirection, "---");
  }

  currentStateCLK = digitalRead(INPUTA);

  // If the previous and the current state of the INPUTA are different then a pulse has occured
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
      strcpy(encoderDirection, "CW ");
      digitalWrite(ledCW, HIGH);
      digitalWrite(ledCCW, LOW);

    }
    showCounter();
  }
  // Update previousStateCLK with the current state
  previousStateCLK = currentStateCLK;
}
