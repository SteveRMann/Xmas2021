#define LED_PIN     D4
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define NUM_LEDS    176
#define BRIGHTNESS   30

// Tree Topper
#define LED_PIN_TOP D2
#define TOP_TYPE WS2811


// if you define max mA, the driver will dim the LEDs to avoid going over.
// Put this line after .addleds
// FastLED.setMaxPowerInVoltsAndMilliamps( VOLTS, MAX_MA);
// Causes flicker??
#define VOLTS 12
#define MAX_MA 15000
