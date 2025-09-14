#include <TinyGPSPlus.h>
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
// GPS and GSM setup
#define GPS_SERIAL Serial2
#define GSM_SERIAL Serial3
TinyGPSPlus gps;
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
// GSM target number
const char phoneNumber[] = "+919740917537";
// Variables
unsigned int depositBalance = 20000;
unsigned long timeoutMillis = 10000;
unsigned long startMillis;
// GSM initialization flag
bool isGSMInitialized = false;
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

    GPS_SERIAL.begin(9600);
    GSM_SERIAL.begin(9600);
    initializeGSM();

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
            denyAccess("Unauthorized RFID!");
        }
    }
    // Feed GPS data into TinyGPSPlus
    while (GPS_SERIAL.available() > 0) {
        gps.encode(GPS_SERIAL.read());
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
void denyAccess(String reason) {
    lcd.clear();
    lcd.print("Access Denied!!");
    delay(2000); // Delay before sending location
    sendLocationSMS(reason);
    activateRedAlert();
}

void verifyFingerprint(String correctPin) {
    lcd.clear();
    lcd.print("Place Finger!");

    if (finger.getImage() != FINGERPRINT_OK) {
        lcd.clear();
        lcd.print("404 NOT FOUND");
        delay(2000); // Delay before sending location
        sendLocationSMS("Unauthorized Fingerprint Attempt");
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
        delay(2000); // Delay before sending location
        sendLocationSMS("Unauthorized Fingerprint Attempt");
        activateRedAlert();
    }
}
void enterAmount(String correctPin) {
    String inputAmountStr = "";
    lcd.clear();
    lcd.print("Enter Amount:");
    lcd.setCursor(0, 1);
    startMillis = millis();
    while (true) {
        char key = keypad.getKey();
        if (key) {
            if (isDigit(key)) {
                inputAmountStr += key;
                lcd.print(key);
            } else if (key == '#') {
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
        sendLocationSMS("Low Fund Detected");
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
    startMillis = millis();
    while (true) {
        char key = keypad.getKey();
        if (key) {
            if (isDigit(key)) {
                inputPassword += key;
                lcd.print("*");
                if (inputPassword.length() == 4) break;
            } else if (key == '#') {
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
        lcd.print(depositBalance);
        sendSMS(phoneNumber, "Withdrawn: " + String(withdrawnAmount) + ", Balance: " + String(depositBalance));
        delay(3000);
    } else {
        lcd.clear();
        lcd.print("Incorrect PIN");
        delay(2000); // Delay before sending location
        sendLocationSMS("Incorrect PIN Entered");
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
void initializeGSM() {
    if (!isGSMInitialized) {
        GSM_SERIAL.println("AT");
        delay(1000);
        GSM_SERIAL.println("AT+CMGF=1");
        delay(1000);
        Serial.println(F("GSM Module Initialized."));
        isGSMInitialized = true;
    }
}
void sendSMS(const char *number, String message) {
    GSM_SERIAL.print("AT+CMGS=\"");
    GSM_SERIAL.print(number);
    GSM_SERIAL.println("\"");
    delay(1000);
    GSM_SERIAL.print(message);
    delay(1000);
    GSM_SERIAL.write(26);
    delay(3000);
    Serial.println(F("SMS Sent."));
}
void sendLocationSMS(String reason) {
    String locationURL = "https://www.google.com/maps?q=15.8223328,74.4887179 ";
    if (gps.location.isUpdated()) {
        double latitude = gps.location.lat();
        double longitude = gps.location.lng();
        locationURL = "https://www.google.com/maps?q=" + String(latitude, 10) + "," + String(longitude, 10);
    }
    sendSMS(phoneNumber, reason + "\nLocation: " + locationURL);
}
