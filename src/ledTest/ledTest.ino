#define SKETCH "ledTest.ino"



//---------- wifi ----------
#define HOSTPREFIX "ledTest"    //18 chars max
#include "ESP8266WiFi.h"        //Not needed if also using the Arduino OTA Library...
#include "D:\River Documents\Arduino\libraries\Kaywinnet.h"  \\ WiFi credentials
char macBuffer[24];             //Holds the last three digits of the MAC, in hex.
char hostNamePrefix[] = HOSTPREFIX;
char hostName[24];              //Holds hostNamePrefix + the last three bytes of the MAC address.

// ---------- ota ----------
#include <ArduinoOTA.h>

// ---------- fastLED ----------
#define FASTLED_INTERNAL        // Pragma fix
#include <FastLED.h>

#include <ArduinoOTA.h>


// ---------- Timers ----------
unsigned long startMillis1;
unsigned long startMillis2;
unsigned long currentMillis;
const unsigned long oneSecond = 1000;


#define NUM_LEDS 785
#define LED_PIN_1 D2    //Tree topper.
#define LED_PIN_2 D1
#define COLOR_ORDER RGB
#define BRIGHTNESS 75
#define LED_TYPE WS2811
#define TOP_TYPE WS2812

int colorPtr = 0;
CRGB colors[3] = {CRGB::Red, CRGB::Green, CRGB::Blue};

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

//  {34, 35, 99, 100, 164, 165, 230, 231, 296, 297, 362, 363, 428, 429, 493, 494, 557, 558, 622, 623, 688, 689, 753, 754}, // Ring 0
//  {3, 65, 69, 131, 134, 196, 200, 261, 265, 328, 332, 395, 398, 459, 462, 524, 527, 589, 592, 653, 657, 719, 723, 784}, // Ring 31

//Ring[0] is on top, ring[32] is on the bottom. Each ring has 24 LEDs.
int ring[32] [24] {
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}, // Test Ring
  {33, 36, 98, 101, 163, 166, 229, 232, 295, 298, 361, 364, 427, 430, 492, 495, 556, 559, 621, 624, 687, 690, 752, 755}, // Ring 1
  {32, 37, 97, 102, 162, 167, 228, 233, 294, 299, 360, 365, 426, 431, 491, 496, 555, 560, 620, 625, 686, 691, 751, 756}, // Ring 2
  {31, 38, 96, 103, 161, 168, 227, 234, 293, 300, 359, 366, 425, 432, 490, 497, 554, 561, 619, 626, 685, 692, 750, 757}, // Ring 3
  {30, 39, 95, 104, 160, 169, 226, 235, 292, 301, 358, 367, 424, 433, 489, 498, 553, 562, 618, 627, 684, 693, 749, 758}, // Ring 4
  {29, 40, 94, 105, 159, 170, 225, 236, 291, 302, 357, 368, 423, 434, 488, 499, 552, 563, 617, 628, 683, 694, 748, 759}, // Ring 5
  {28, 41, 93, 106, 158, 171, 224, 237, 290, 303, 356, 369, 422, 435, 487, 500, 551, 564, 616, 629, 682, 695, 747, 760}, // Ring 6
  {27, 42, 92, 107, 157, 172, 223, 238, 289, 304, 355, 370, 421, 436, 486, 501, 550, 565, 615, 630, 681, 696, 746, 761}, // Ring 7
  {26, 43, 91, 108, 156, 173, 222, 239, 288, 305, 354, 371, 420, 437, 485, 502, 549, 566, 614, 631, 680, 697, 745, 762}, // Ring 8
  {25, 44, 90, 109, 155, 174, 221, 240, 287, 306, 353, 372, 419, 438, 484, 503, 548, 567, 613, 632, 679, 698, 744, 763}, // Ring 9
  {24, 45, 89, 110, 154, 175, 220, 241, 286, 307, 352, 373, 418, 439, 483, 504, 547, 568, 612, 633, 678, 699, 743, 764}, // Ring 10
  {23, 46, 88, 111, 153, 176, 219, 242, 285, 308, 351, 374, 417, 440, 482, 505, 546, 569, 611, 634, 677, 700, 742, 765}, // Ring 11
  {22, 47, 87, 112, 152, 177, 218, 243, 284, 309, 350, 375, 416, 441, 481, 506, 545, 570, 610, 635, 676, 701, 741, 766}, // Ring 12
  {21, 48, 86, 113, 151, 178, 217, 244, 283, 310, 349, 376, 415, 442, 480, 507, 544, 571, 609, 636, 675, 702, 740, 767}, // Ring 13
  {20, 49, 85, 114, 150, 179, 216, 245, 282, 311, 348, 377, 414, 443, 479, 508, 543, 572, 608, 637, 674, 703, 739, 768}, // Ring 14
  {19, 50, 84, 115, 149, 180, 215, 246, 281, 312, 347, 378, 413, 444, 478, 509, 542, 573, 607, 638, 673, 704, 738, 769}, // Ring 15
  {18, 51, 83, 116, 148, 181, 214, 247, 280, 313, 346, 379, 412, 445, 477, 510, 541, 574, 606, 639, 672, 705, 737, 770}, // Ring 16
  {17, 52, 82, 117, 147, 182, 213, 248, 279, 314, 345, 380, 411, 446, 476, 511, 540, 575, 605, 640, 671, 706, 736, 771}, // Ring 17
  {16, 53, 81, 118, 146, 183, 212, 249, 278, 315, 344, 381, 410, 447, 475, 512, 539, 576, 604, 641, 670, 707, 735, 772}, // Ring 18
  {15, 54, 80, 119, 145, 184, 211, 250, 277, 316, 343, 382, 409, 448, 474, 513, 538, 577, 603, 642, 669, 708, 734, 773}, // Ring 19
  {14, 55, 79, 120, 144, 185, 210, 251, 276, 317, 342, 383, 408, 449, 473, 514, 537, 578, 602, 643, 668, 709, 733, 774}, // Ring 20
  {13, 56, 78, 121, 143, 186, 209, 252, 275, 318, 341, 384, 407, 450, 472, 515, 536, 579, 601, 644, 667, 710, 732, 775}, // Ring 21
  {12, 57, 77, 122, 142, 187, 208, 253, 274, 319, 340, 385, 406, 451, 471, 516, 535, 580, 600, 645, 666, 711, 731, 776}, // Ring 22
  {11, 58, 76, 123, 141, 188, 207, 254, 273, 320, 339, 386, 405, 452, 470, 517, 534, 581, 599, 646, 665, 712, 730, 777}, // Ring 23
  {10, 59, 75, 124, 140, 189, 206, 255, 272, 321, 338, 387, 404, 453, 469, 518, 533, 582, 598, 647, 664, 713, 729, 778}, // Ring 24
  {9, 60, 74, 125, 139, 190, 205, 256, 271, 322, 337, 388, 403, 454, 468, 519, 532, 583, 597, 648, 663, 714, 728, 779}, // Ring 25
  {8, 61, 73, 126, 138, 191, 204, 257, 270, 323, 336, 389, 402, 455, 467, 520, 531, 584, 596, 649, 662, 715, 727, 780}, // Ring 26
  {7, 62, 72, 127, 137, 192, 203, 258, 269, 324, 335, 390, 401, 456, 466, 521, 530, 585, 595, 650, 661, 716, 726, 781}, // Ring 27
  {6, 63, 71, 128, 136, 193, 202, 259, 268, 325, 334, 391, 400, 457, 465, 522, 529, 586, 594, 651, 660, 717, 725, 782}, // Ring 28
  {5, 64, 70, 129, 135, 194, 201, 260, 267, 326, 333, 392, 399, 458, 464, 523, 528, 587, 593, 652, 659, 718, 724, 783}, // Ring 29
  {4, 65, 69, 130, 134, 195, 200, 261, 266, 327, 332, 393, 398, 459, 463, 524, 527, 588, 592, 653, 658, 719, 723, 784}, // Ring 30
  {31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54}, // Test Ring 31
};


//----------------------- setRing() -------------------
//Set a selected ring to a single color
//Enter with ring number, led count in the ring, and the color.
//Example:
//  setRing(3,sizeof(ring[3]),RED;

void setRing(int r, int rlc, CRGB ringColor) {
  //Debugging
  Serial.println();
  Serial.print(F("Ring number: "));
  Serial.println(r);
  Serial.print(F("Ring LED count= "));
  Serial.println(rlc);

  for (int i = 0; i < rlc; i++) {
    leds[ring[r][i]] = ringColor;
    //debug
    Serial.println (ring[r][i]);
  }
}




void nextRingLed(int rn, int rlc, CRGB ringColor) {
  static int i = 0;
  if (i >= rlc) {
    //done.
    i = 0;
    colorPtr++;
    if (colorPtr > 2)
      colorPtr = 0;
    return;
  }

  /*
   * if (rn = 31) i++;
    Serial.print(F("rb= "));
    Serial.print(rn);
    Serial.print(F(",  i= "));
    Serial.println(i);
  */

  leds[ring[rn][i]] = ringColor;
  FastLED.show();
  i++;
}





//----------------------- setup() -------------------
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println(F(SKETCH));
  Serial.println(F("---------------"));

  setup_wifi();
  start_OTA();

  delay(100);
  FastLED.addLeds<LED_TYPE, LED_PIN_2, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<TOP_TYPE, LED_PIN_1, COLOR_ORDER>(top, 1).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  /*
    // Init the topper //****
    top[0] = CRGB::Blue;
    FastLED.show();

    paletteNumber = 5;        // Show the default palette first.
  */


  // ---------- Turn off all LEDs ----------
  for (int iLed = 0; iLed < NUM_LEDS; iLed = iLed + 1) {
    leds[iLed] = CRGB::Red;
  }
  FastLED.show();

  delay(500);
  for (int iLed = 0; iLed < NUM_LEDS; iLed = iLed + 1) {
    leds[iLed] = CRGB::Blue;
  }
  FastLED.show();

  delay(500);
  for (int iLed = 0; iLed < NUM_LEDS; iLed = iLed + 1) {
    leds[iLed] = CRGB::Black;
  }
  FastLED.show();
  // --------------------------------------


  // setRing(0, sizeof(ring[0]), CRGB::Red);   //Set ring 0 to red
  setRing(0, 24, CRGB::Blue);       //Set ring 0 to blue
  setRing(31, 24, CRGB::Green);    //Set ring 0 to Green
  FastLED.show();

  delay(1000);

  startMillis1 = millis();           //initial start time
  startMillis2 = millis();           //initial start time

}



//----------------------- loop() -------------------
void loop() {
  ArduinoOTA.handle();
  currentMillis = millis();

  //Every second, call nextRingLed
  if (currentMillis - startMillis1 >= oneSecond / 2) {
    //nextRingLed(0, 24, colors[colorPtr]);
    nextRingLed(31, 24, colors[colorPtr]);
    startMillis1 = currentMillis;
  }

  //  if (currentMillis - startMillis2 >= oneSecond) {
  //    nextRingLed(31, 24, colors[colorPtr]);
  //    startMillis2 = currentMillis;
  //  }




}
