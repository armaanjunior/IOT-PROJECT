// Pin definitions
const int soilMoisturePin = A0;  // Soil moisture sensor analog pin
const int relayPin = 7;          // Relay pin for the water pump
const int buzzerPin = 8;         // Buzzer pin

// Threshold values for soil moisture
const int dryThreshold = 450;    // Turn on water pump and buzzer if below this value
const int wetThreshold = 650;    // Turn off water pump and buzzer if above this value

void setup() {
  pinMode(relayPin, OUTPUT);     // Set relay pin as output
  pinMode(buzzerPin, OUTPUT);    // Set buzzer pin as output
  digitalWrite(relayPin, LOW);   // Ensure relay is off initially
  digitalWrite(buzzerPin, LOW);  // Ensure buzzer is off initially

  Serial.begin(9600);            // Start serial communication
  Serial.println("Smart Watering System");
}

void loop() {
  int soilMoistureValue = analogRead(soilMoisturePin);  // Read soil moisture value
  int SoilMoisturedata = 1023 - soilMoistureValue;      // Invert the value
  
  Serial.print("Soil Moisture Data: ");
  Serial.println(SoilMoisturedata);  // Print the inverted value to the Serial Monitor

  // Check soil moisture levels
  if (SoilMoisturedata <= dryThreshold) {
    // Soil is dry, turn on water pump and buzzer
    digitalWrite(relayPin, HIGH);    // Turn on water pump
    digitalWrite(buzzerPin, HIGH);   // Turn on buzzer
    Serial.println("Soil is dry! Water pump and buzzer ON!!");
  } else if (SoilMoisturedata >= wetThreshold) {
    // Soil is wet, turn off water pump and buzzer
    digitalWrite(relayPin, LOW);     // Turn off water pump
    digitalWrite(buzzerPin, LOW);    // Turn off buzzer
    Serial.println("Soil is wet! Water pump and buzzer OFF!!");
  }

  delay(1000);  // Wait for 1 second before taking another reading
}
