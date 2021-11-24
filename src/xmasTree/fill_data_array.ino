
//----------------------- fill_data_array() -------------------
void fill_data_array()
{
  static uint8_t startValue = 0;
  startValue = startValue + 2;

  uint8_t value = startValue;
  for ( int i = 0; i < NUM_LEDS; i++) {
    data[i] = triwave8( value); // convert value to an up-and-down wave
    value += 7;
  }
}
