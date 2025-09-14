#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define Serial1 for communication with the fingerprint sensor
#define FINGERPRINT_SERIAL Serial1

// Create a fingerprint object using Serial1
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&FINGERPRINT_SERIAL);

// Initialize the LCD with I2C address 0x27 (change if needed)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Start serial communication for debugging
  Serial.begin(9600);       // Serial Monitor
  FINGERPRINT_SERIAL.begin(57600);    // ZFM-60 Sensor Serial on Serial1

  // Initialize the fingerprint sensor
  finger.begin(57600);  // Set the baud rate to 57600

  // Verify the password and ensure the sensor is working
  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor found!");
  } else {
    Serial.println("Fingerprint sensor not found. Please check connections.");
    while (1);  // Halt the program if sensor is not found
  }

  // Initialize the LCD
  Wire.begin();  
  lcd.begin(16, 2);    // Initialize the LCD
  lcd.backlight();     // Turn on the backlight

  // Display "Welcome to ATM" for 1 second
  lcd.setCursor(0, 0); // Set the cursor to the top-left corner
  lcd.print("WELCOME-TO-ATM");  // Display welcome message
  delay(1000);  // Wait for 1 second
  lcd.clear();   // Clear the display

  // Ask user to place finger
  lcd.setCursor(0, 0);  // Set cursor to first line
  lcd.print("Place U'r Finger");  // Prompt user to place finger
  delay(2000);           // Wait for a moment to let the user place the finger
  lcd.clear();           // Clear the display

  // Start fingerprint verification
  verifyFingerprint();
}

void loop() {
  // Nothing in loop for this test
}

void verifyFingerprint() {
  int p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("404 NOT FOUND");
    delay(2000);
    return;
  }

  // Convert the captured image to a fingerprint template
  p = finger.image2Tz(1);  // Use image2Tz(1) for first scan
  if (p != FINGERPRINT_OK) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error converting");
    lcd.setCursor(0, 1);
    lcd.print("To template");
    delay(2000);
    return;
  }

  // Search for the fingerprint in the sensor’s memory
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    // Fingerprint matched, show message on LCD
    lcd.clear();
    lcd.setCursor(0, 0);  // Set cursor to first line
    lcd.print("Verified Finger");  // Display success message
    lcd.setCursor(0, 1);  // Set cursor to second line
    lcd.print("Enter Password");  // Prompt user for password
    delay(2000); // Keep the success message on the screen
  } else if (p == FINGERPRINT_NOTFOUND) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Finger NOT Valid");
    delay(2000); // Keep the invalid message on the screen
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error searching");
    lcd.setCursor(0, 1);
    lcd.print("for fingerprint");
    delay(2000); // Show error message on LCD for a while
  }
}
