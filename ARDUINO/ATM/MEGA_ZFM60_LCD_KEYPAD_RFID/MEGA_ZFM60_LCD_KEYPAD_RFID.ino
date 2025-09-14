#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// RFID setup
#define RST_PIN 5
#define SS_PIN 53
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Fingerprint sensor setup
#define FINGERPRINT_SERIAL Serial1
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&FINGERPRINT_SERIAL);

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keypad setup
const byte ROW_NUM = 4;
const byte COLUMN_NUM = 4;
char keys[ROW_NUM][COLUMN_NUM] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
byte pin_rows[ROW_NUM] = {24, 26, 28, 30};
byte pin_column[COLUMN_NUM] = {33, 35, 37, 39};
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

// Predefined RFID tags and PINs
String savedTag1 = "53 BD 11 2F";
String savedTag2 = "23 60 15 F7";
String pin1 = "2013";
String pin2 = "2012";

// LED and Buzzer pins
#define GREEN_LED_PIN 2
#define RED_LED_PIN 4
#define BUZZER_PIN 3

// Variables
unsigned int depositBalance = 20000; // Initial deposit balance
unsigned long timeoutMillis = 10000; // Timeout for input (10 seconds)
unsigned long startMillis;

void setup() {
    Serial.begin(115200);
    SPI.begin();
    mfrc522.PCD_Init();

    FINGERPRINT_SERIAL.begin(57600);
    finger.begin(57600);
    if (!finger.verifyPassword()) {
        Serial.println("Fingerprint sensor not found.");
        while (1);
    }

    Wire.begin();
    lcd.begin(16, 2);
    lcd.backlight();

    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    lcd.print("WELCOME-TO-ATM");
    delay(2000);
    lcd.clear();
    lcd.print("Place Your Card");
}

void loop() {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        String currentUID = getUID();
        if (currentUID.equalsIgnoreCase(savedTag1)) {
            grantAccess(pin1);
        } else if (currentUID.equalsIgnoreCase(savedTag2)) {
            grantAccess(pin2);
        } else {
            denyAccess();
        }
    }
}

String getUID() {
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        uid += String(mfrc522.uid.uidByte[i], HEX);
        if (i < mfrc522.uid.size - 1) uid += " ";
    }
    return uid;
}

void grantAccess(String correctPin) {
    lcd.clear();
    lcd.print("Access Granted");
    delay(2000);

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    verifyFingerprint(correctPin);
}

void denyAccess() {
    lcd.clear();
    lcd.print("Access Denied!!");
    activateRedAlert();
}

void verifyFingerprint(String correctPin) {
    lcd.clear();
    lcd.print("Place Finger!");

    if (finger.getImage() != FINGERPRINT_OK) {
        lcd.clear();
        lcd.print("404 NOT FOUND");
        activateRedAlert();
        return;
    }

    finger.image2Tz();
    if (finger.fingerSearch() == FINGERPRINT_OK) {
        lcd.clear();
        lcd.print("Fingerprint OK");
        digitalWrite(GREEN_LED_PIN, HIGH);
        delay(2000);
        digitalWrite(GREEN_LED_PIN, LOW);
        enterAmount(correctPin);
    } else {
        lcd.clear();
        lcd.print("Finger NOT Valid");
        activateRedAlert();
    }
}

void enterAmount(String correctPin) {
    String inputAmountStr = "";
    lcd.clear();
    lcd.print("Enter Amount:");
    lcd.setCursor(0, 1);
    startMillis = millis(); // Reset timeout

    while (true) {
        char key = keypad.getKey();
        if (key) {
            if (isDigit(key)) {
                inputAmountStr += key;
                lcd.print(key);
            } else if (key == '#') { // Enter key
                break;
            } else {
                lcd.clear();
                lcd.print("Invalid Input");
                delay(2000);
                lcd.clear();
                lcd.print("Enter Amount:");
                lcd.setCursor(0, 1);
                inputAmountStr = "";
            }
        }

        if (millis() - startMillis > timeoutMillis) {
            lcd.clear();
            lcd.print("Timeout");
            delay(2000);
            lcd.clear();
            return;
        }
    }

    int inputAmount = inputAmountStr.toInt();
    lcd.clear();

    if (inputAmount <= depositBalance) {
        depositBalance -= inputAmount;
        enterPassword(correctPin, inputAmount);
    } else {
        lcd.print("Low-Fund !!");
        delay(2000);
        lcd.clear();
        lcd.print("Take Your Card!");
        activateRedAlert();
        delay(2000);
        lcd.clear();
    }
}

void enterPassword(String correctPin, int withdrawnAmount) {
    String inputPassword = "";
    lcd.clear();
    lcd.print("Enter PIN:");
    lcd.setCursor(0, 1);
    startMillis = millis(); // Reset timeout

    while (true) {
        char key = keypad.getKey();
        if (key) {
            if (isDigit(key)) {
                inputPassword += key;
                lcd.print("*");
                if (inputPassword.length() == 4) break;
            } else if (key == '#') { // Enter key
                break;
            } else {
                lcd.clear();
                lcd.print("Invalid Input!!");
                delay(2000);
                lcd.clear();
                lcd.print("Enter PIN:");
                lcd.setCursor(0, 1);
                inputPassword = "";
            }
        }

        if (millis() - startMillis > timeoutMillis) {
            lcd.clear();
            lcd.print("Timeout");
            delay(2000);
            lcd.clear();
            return;
        }
    }

    if (inputPassword == correctPin) {
        lcd.clear();
        lcd.print("Transaction Done");
        delay(2000);
        lcd.clear();
        lcd.print("Withdrew:");
        lcd.print(withdrawnAmount);
        delay(2000);
        lcd.clear();
        lcd.print("Balance:");
        lcd.print(depositBalance); // Display balance only after completion
        delay(3000);
    } else {
        lcd.clear();
        lcd.print("Incorrect PIN");
        activateRedAlert();
    }
    lcd.clear();
    lcd.print("VISIT-AGAIN !!");
}

void activateRedAlert() {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(2000);
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
}
