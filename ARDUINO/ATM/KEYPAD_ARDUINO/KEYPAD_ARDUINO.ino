#include <Keypad.h>

// Define the keypad layout (adjust as per your actual keypad layout)
const byte ROW_NUM    = 4;  // four rows
const byte COLUMN_NUM = 4;  // four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Define the pin numbers for rows and columns using the new pin mapping
byte pin_rows[ROW_NUM] = {24, 26, 28, 30};  // Rows connected to D24, D26, D28, D30
byte pin_column[COLUMN_NUM] = {33, 35, 37, 39};  // Columns connected to D31, D33, D35, D37

// Initialize the keypad library
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

void setup() {
  Serial.begin(9600);  // Start serial communication at 115200 baud rate

  // No need for explicit pinMode or pull-up for the Mega; Keypad library handles it

  Serial.println("Keypad Test - Press any key on the keypad.");
}

void loop() {
  char key = keypad.getKey();  // Get the key pressed
  if (key) {  // If a key is pressed
    Serial.print("Key Pressed: ");
    Serial.println(key);  // Print the key value to Serial Monitor
  }
}
