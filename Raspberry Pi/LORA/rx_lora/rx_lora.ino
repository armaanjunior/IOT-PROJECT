#include <SoftwareSerial.h>
#include "EBYTE.h"

#define PIN_RX 2
#define PIN_TX 3
#define PIN_M0 4
#define PIN_M1 5
#define PIN_AX 6

// Define the same structure as the sender for consistency
struct DATA {
  // unsigned long Count;
  // int Bits;
  // float Volts;
  char Message[300];  // The char array for the string message
};

DATA ReceivedData;

// Define SoftwareSerial and EBYTE transceiver objects
SoftwareSerial ESerial(PIN_RX, PIN_TX);
EBYTE Transceiver(&ESerial, PIN_M0, PIN_M1, PIN_AX);

void setup() {
  Serial.begin(9600);
  ESerial.begin(9600);

  Serial.println("Starting Receiver");

  Transceiver.init();
  Transceiver.PrintParameters();
}

void loop() {
  // Check if data is available
  if (Transceiver.available()) {
    // Read the incoming data into the structure
    Transceiver.GetStruct(&ReceivedData, sizeof(ReceivedData));

    // Display the received data
    // Serial.print("Received Count: ");
    // Serial.println(ReceivedData.Count);
    // Serial.print("Received Bits: ");
    // Serial.println(ReceivedData.Bits);
    // Serial.print("Received Volts: ");
    // Serial.println(ReceivedData.Volts);
    Serial.print("Received Message: ");
    Serial.println(ReceivedData.Message);
  }

  delay(1000);
}