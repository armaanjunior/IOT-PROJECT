#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

// Define pins for SPI communication
#define SDA_PIN 11   // Slave Select (SS) pin
#define RST_PIN 5    // Reset pin
#define SCK_PIN 14   // Serial Clock
#define MOSI_PIN 13  // Master Out Slave In
#define MISO_PIN 12  // Master In Slave Out

// Configure MFRC522 SPI driver
MFRC522DriverPinSimple ss_pin(SDA_PIN); // Configurable Slave Select pin
MFRC522DriverSPI driver{ss_pin};       // Create SPI driver
// Create MFRC522 instance
MFRC522 mfrc522{driver};
void setup() {
  Serial.begin(115200);  // Initialize serial communications with the PC for debugging
  while (!Serial);       // Wait for Serial connection (for boards like ATMEGA32U4-based ones)

  // Initialize SPI bus
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);

  // Initialize MFRC522
  mfrc522.PCD_Init();
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial); // Show details of the card reader
  Serial.println(F("Scan a PICC to see UID..."));
}

void loop() {
  // Check if a new card is present
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Attempt to read the card serial number
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Print only the UID of the card
  Serial.print("Card UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "); // Format for single digits
    Serial.print(mfrc522.uid.uidByte[i], HEX); // Print UID byte in hexadecimal
  }
  Serial.println();

  // Halt communication with the card
  mfrc522.PICC_HaltA();
}
