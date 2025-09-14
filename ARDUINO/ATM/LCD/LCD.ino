#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize the LCD with I2C address 0x27 (change if needed)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);  // Start serial communication for debugging

  // Initialize the I2C communication with the default SDA/SCL pins
  Wire.begin();  

  lcd.begin(16, 2);    // Initialize the LCD
  lcd.backlight();     // Turn on the backlight
  
  lcd.setCursor(0, 0);  // Set the cursor to the top-left corner
  lcd.print("WEL-COME TO ATM");  // Display message
  delay(1000);
  lcd.clear();
}

void loop() {
  // Nothing in loop for this test
}
