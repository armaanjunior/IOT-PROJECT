#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10   // RFID Slave Select (SDA)
#define RST_PIN 9   // RFID Reset
#define BUZZER 8    // Buzzer pin
#define GREEN_LED 7 // Green LED for authorized access
#define RED_LED 6   // Red LED for unauthorized access

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C LCD at address 0x27 (Change if needed)
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

// Authorized UID (Replace with your card UID)
byte authorizedUID[] = {0x2E, 0xAF, 0x28, 0x03};

void setup() {
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();
    
    pinMode(BUZZER, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    
    lcd.init();
    lcd.backlight();
    showDefaultMessage();
    
    Serial.println("Scan an RFID card...");
}

void loop() {
    // Look for a new card
    if (!mfrc522.PICC_IsNewCardPresent()) return;
    if (!mfrc522.PICC_ReadCardSerial()) return;

    Serial.print("Card UID: ");
    bool isAuthorized = true;

    // Check UID
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        if (mfrc522.uid.uidByte[i] != authorizedUID[i]) {
            isAuthorized = false;
        }
    }
    Serial.println();

    if (isAuthorized) {
        Serial.println("Access Granted!");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Prajwal Present");

        digitalWrite(GREEN_LED, HIGH);
        digitalWrite(BUZZER, HIGH);
        delay(1000);
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(BUZZER, LOW);

    } else {
        Serial.println("Access Denied! Unauthorized Card.");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Not Valid !!");

        for (int i = 0; i < 5; i++) {
            digitalWrite(RED_LED, HIGH);
            digitalWrite(BUZZER, HIGH);
            delay(300);
            digitalWrite(RED_LED, LOW);
            digitalWrite(BUZZER, LOW);
            delay(300);
        }
    }

    delay(1000);
    showDefaultMessage(); // Reset LCD after processing

    // Halt PICC
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}

// Function to display default LCD message
void showDefaultMessage() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter I_D Card");
}
