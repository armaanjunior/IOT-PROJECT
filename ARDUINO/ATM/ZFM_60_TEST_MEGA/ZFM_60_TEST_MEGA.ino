#include <Adafruit_Fingerprint.h>

// Define Serial1 for communication with the fingerprint sensor
#define FINGERPRINT_SERIAL Serial1

// Create a fingerprint object using Serial1
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&FINGERPRINT_SERIAL);

void setup() {
  // Start the serial communication for debugging
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

  // Start fingerprint verification
  verifyFingerprint();
}

void loop() {
  // Do nothing in the loop
}

void verifyFingerprint() {
  Serial.println("Place your finger on the sensor for verification...");

  int p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    Serial.print("Error getting image: ");
    Serial.println(p);
    return;
  }

  // Convert the captured image to a fingerprint template
  p = finger.image2Tz(1);  // Use image2Tz(1) for first scan
  if (p != FINGERPRINT_OK) {
    Serial.print("Error converting image to template: ");
    Serial.println(p);
    return;
  }

  // Search for the fingerprint in the sensor’s memory
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.print("Fingerprint matched! ID: ");
    Serial.println(finger.fingerID);
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Fingerprint not found.");
  } else {
    Serial.print("Error searching for fingerprint: ");
    Serial.println(p);
  }
}
