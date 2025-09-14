#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

// Define the software serial pins (2, 3 in this case for RX and TX)
SoftwareSerial mySerial(2, 3);

// Create a fingerprint object
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

int id = 1; // Template ID for the new fingerprint

void setup() {
  // Start the serial communication for debugging
  Serial.begin(9600);       // Serial Monitor
  mySerial.begin(57600);    // ZFM-60 Sensor Serial

  // Initialize the fingerprint sensor
  finger.begin(57600);  // Set the baud rate to 57600

  // Verify the password and ensure the sensor is working
  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor found!");
  } else {
    Serial.println("Fingerprint sensor not found. Please check connections.");
    while (1);  // Halt the program if sensor is not found
  }

  // Enroll a fingerprint
  enrollFingerprint(id);
}

void loop() {
  // Do nothing in the loop
}

void enrollFingerprint(int id) {
  Serial.print("Place your finger on the sensor to enroll (ID: ");
  Serial.print(id);
  Serial.println(")...");

  // Wait for the finger to be placed on the sensor and capture the image
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

  // Ask for the second scan (to verify the fingerprint)
  Serial.println("Please place the same finger again for verification...");

  // Add a delay before asking the user to place their finger again for verification
  delay(3000);  // Delay for 3 seconds before verification

  // Capture the second fingerprint image for verification
  p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    Serial.print("Error getting second image: ");
    Serial.println(p);
    return;
  }

  // Convert the second image to a template for verification
  p = finger.image2Tz(2);  // Use image2Tz(2) for second scan
  if (p != FINGERPRINT_OK) {
    Serial.print("Error converting second image to template: ");
    Serial.println(p);
    return;
  }

  // Store the fingerprint in the sensor's memory
  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    Serial.print("Error creating fingerprint model: ");
    Serial.println(p);
    return;
  }

  // Store the fingerprint model into the sensor's memory
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.print("Fingerprint ID #");
    Serial.print(id);
    Serial.println(" enrolled successfully!");
  } else {
    Serial.print("Error storing fingerprint ID #");
    Serial.print(id);
    Serial.print(". Error code: ");
    Serial.println(p);
  }
}
