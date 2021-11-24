//--------------- showCounter ---------------
void showCounter() {
  //Serial.print("Direction: ");
  Serial.print(encoderDirection);
  Serial.print(", ");
  //Serial.println(counter);

  char buffer[10];
  sprintf(buffer, "%d", counter);
  //Serial.print(F("char buffer= "));
  Serial.println(buffer);


  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(buffer);
  display.display();
  delay(100);
}
