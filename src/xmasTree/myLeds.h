#define LED_PIN     D2
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define NUM_LEDS    799
#define BRIGHTNESS   40

// if you define max mA, the driver will dim the LEDs to avoid going over.
// Put this line after .addleds
// FastLED.setMaxPowerInVoltsAndMilliamps( VOLTS, MAX_MA);
// Causes flicker??
#define VOLTS 12
#define MAX_MA 15000
