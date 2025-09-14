#include <Adafruit_Fingerprint.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>

// RFID Setup
#define RST_PIN 5
#define SS_PIN 53
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create an MFRC522 instance

// Fingerprint Sensor Setup
#define FINGERPRINT_SERIAL Serial1
Adafruit_Fingerprint finger(&FINGERPRINT_SERIAL);

// Variables
int id = 1; // Template ID for fingerprints
const int maxFingerprints = 200; // Maximum fingerprint capacity
const int maxAssociations = 50; // Maximum RFID-Fingerprint associations

void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Initialize RFID
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Place your RFID card...");

  // Initialize Fingerprint Sensor
  FINGERPRINT_SERIAL.begin(57600);
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor initialized.");
  } else {
    Serial.println("Fingerprint sensor not found. Check connections.");
    while (1); // Halt if the fingerprint sensor is not found
  }
}

void loop() {
  // Wait for RFID card
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String rfidUID = getRFIDUID();
    Serial.print("RFID UID Detected: ");
    Serial.println(rfidUID);

    // Enroll a fingerprint for this RFID
    enrollFingerprint(id, rfidUID);
    id++; // Increment fingerprint ID
    if (id > maxFingerprints) {
      Serial.println("Fingerprint storage is full.");
    }

    mfrc522.PICC_HaltA(); // Halt the RFID card
  }
}

String getRFIDUID() {
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i], HEX);
    if (i < mfrc522.uid.size - 1) uid += ":";
  }
  return uid;
}

void enrollFingerprint(int id, String rfidUID) {
  Serial.println("Place your finger on the sensor to enroll...");

  // Capture the first fingerprint image
  int p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    Serial.println("Error capturing image. Try again.");
    return;
  }

  // Convert the image to a fingerprint template
  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println("Error converting image. Try again.");
    return;
  }

  // Ask for the second scan
  Serial.println("Remove your finger and place it again...");
  delay(2000);

  // Capture the second fingerprint image
  p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    Serial.println("Error capturing second image. Try again.");
    return;
  }

  // Convert the second image to a fingerprint template
  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println("Error converting second image. Try again.");
    return;
  }

  // Create a model from the two scans
  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    Serial.println("Error creating fingerprint model. Try again.");
    return;
  }

  // Save the fingerprint model to the sensor's memory
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.print("Fingerprint enrolled successfully with ID #");
    Serial.println(id);
    saveAssociationToEEPROM(id, rfidUID);
  } else {
    Serial.println("Error storing fingerprint model.");
  }
}

void saveAssociationToEEPROM(int fingerprintID, String rfidUID) {
  int addr = (fingerprintID - 1) * 20; // Calculate EEPROM address (20 bytes per entry)
  if (addr + 20 > EEPROM.length()) {
    Serial.println("EEPROM is full. Cannot save more associations.");
    return;
  }

  // Save the RFID UID and Fingerprint ID to EEPROM
  EEPROM.put(addr, fingerprintID); // Store Fingerprint ID (4 bytes)
  for (int i = 0; i < rfidUID.length(); i++) {
    EEPROM.write(addr + 4 + i, rfidUID[i]); // Store RFID UID as characters
  }
  EEPROM.write(addr + 4 + rfidUID.length(), '\0'); // Null terminator for the UID

  Serial.print("Association saved: RFID UID -> ");
  Serial.print(rfidUID);
  Serial.print(", Fingerprint ID -> ");
  Serial.println(fingerprintID);
}

void loadAssociationFromEEPROM(int fingerprintID) {
  int addr = (fingerprintID - 1) * 20;
  if (addr + 20 > EEPROM.length()) {
    Serial.println("Invalid address in EEPROM.");
    return;
  }

  int savedID;
  EEPROM.get(addr, savedID);
  char rfidUID[16];
  for (int i = 0; i < 16; i++) {
    rfidUID[i] = EEPROM.read(addr + 4 + i);
    if (rfidUID[i] == '\0') break;
  }

  Serial.print("Loaded Association: RFID UID -> ");
  Serial.print(rfidUID);
  Serial.print(", Fingerprint ID -> ");
  Serial.println(savedID);
}
