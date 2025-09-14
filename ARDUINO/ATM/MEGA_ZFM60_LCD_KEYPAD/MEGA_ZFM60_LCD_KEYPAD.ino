#include <Keypad.h>
#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define Serial1 for communication with the fingerprint sensor
#define FINGERPRINT_SERIAL Serial1

// Create a fingerprint object using Serial1
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&FINGERPRINT_SERIAL);

// Initialize the LCD with I2C address 0x27 (change if needed)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define the keypad layout
const byte ROW_NUM    = 4;  // four rows
const byte COLUMN_NUM = 4;  // four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Define the pin numbers for rows and columns
byte pin_rows[ROW_NUM] = {24, 26, 28, 30};  // Rows connected to D24, D26, D28, D30
byte pin_column[COLUMN_NUM] = {33, 35, 37, 39};  // Columns connected to D31, D33, D35, D37

// Initialize the keypad library
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

void setup() {
  Serial.begin(9600);       
  FINGERPRINT_SERIAL.begin(57600);    // ZFM-60 Sensor Serial on Serial1

  finger.begin(57600);  

  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor found!");
  } else {
    Serial.println("Fingerprint sensor not found. Please check connections.");
    while (1);  // Halt the program if sensor is not found
  }

  Wire.begin();  
  lcd.begin(16, 2);   
  lcd.backlight();     

  lcd.setCursor(0, 0); 
  lcd.print("WELCOME-TO-ATM");  
  delay(1000);  
  lcd.clear();   

  lcd.setCursor(0, 0);  
  lcd.print("Place U'r Finger");  
  delay(2000);           
  lcd.clear();           

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

  p = finger.image2Tz(1);  
  if (p != FINGERPRINT_OK) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error converting");
    lcd.setCursor(0, 1);
    lcd.print("To template");
    delay(2000);
    return;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.setCursor(0, 0);  
    lcd.print("Verified Finger");  
    lcd.setCursor(0, 1);  
    lcd.print("Enter Password");  
    delay(2000);
    lcd.clear();
    enterPassword();  // Call function to handle PIN entry
  } else if (p == FINGERPRINT_NOTFOUND) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Finger NOT Valid");
    delay(2000); 
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error searching");
    lcd.setCursor(0, 1);
    lcd.print("for fingerprint");
    delay(2000); 
  }
}

void enterPassword() {
  String inputPassword = "";  // Variable to store input
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter 4-digit PIN");

  while (inputPassword.length() < 4) {  
    char key = keypad.getKey();  
    if (key) {  
      if (isDigit(key)) {  
        inputPassword += key;
        lcd.setCursor(inputPassword.length() - 1, 1);
        lcd.print("*");  // Mask input with asterisks
      } else {  
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter Valid PIN");  
        delay(2000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter 4-digit PIN");  
        inputPassword = "";  
      }
    }
  }

  lcd.clear();
  if (inputPassword == "2013") {  
    lcd.setCursor(0, 0);
    lcd.print("Transaction Done");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Incorrect PIN");
  }
  
  delay(2000);
  lcd.clear();
}
