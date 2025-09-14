#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 5                   // Reset pin for RC522 module
#define SS_PIN 53                   // Slave Select pin for RC522 module

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

// Predefined saved tags (you can update these as needed)
String savedTag1 = "53 BD 11 2F";   // First tag UID
String savedTag2 = "23 60 15 F7";   // Second tag UID

void setup() {
    Serial.begin(115200);           // Initialize serial communications with the PC
    SPI.begin();                    // Initialize SPI bus
    mfrc522.PCD_Init();             // Initialize MFRC522 module
    Serial.println("System ready. Please scan your card to check access.");
}

void loop() {
    // Check if a new card is present
    if (mfrc522.PICC_IsNewCardPresent()) {
        // Check if the card can be read
        if (mfrc522.PICC_ReadCardSerial()) {
            // Construct the UID string for the current tag
            String currentUID = "";
            for (byte i = 0; i < mfrc522.uid.size; i++) {
                currentUID += String(mfrc522.uid.uidByte[i], HEX);
                if (i < mfrc522.uid.size - 1) {
                    currentUID += " ";  // Add space between bytes for readability
                }
            }
            
            // Check if the current UID matches one of the saved tags
            if (currentUID.equalsIgnoreCase(savedTag1) || currentUID.equalsIgnoreCase(savedTag2)) {
                Serial.println("Access Granted!!");
            } else {
                Serial.println("Access Not Granted");
            }

            // Halt the current card and stop encryption on the current card
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
        }
    }
}
