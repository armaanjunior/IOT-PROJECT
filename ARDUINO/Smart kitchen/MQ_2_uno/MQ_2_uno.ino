const int gasSensorPin = A0; // Connect A0 to the analog output pin of MQ2
const int buzzerPin = 2;     // Connect the buzzer to digital pin 2
const int threshold = 45;    // Threshold value for gas level

void setup() {
  Serial.begin(9600);         // Start the serial monitor
  pinMode(gasSensorPin, INPUT); // Set gas sensor pin as input
  pinMode(buzzerPin, OUTPUT);   // Set buzzer pin as output
}

void loop() {
  int gasLevel = analogRead(gasSensorPin); // Read the analog value from the sensor

  Serial.print("Gas Level: ");
  Serial.println(gasLevel); // Display raw analog value

  // Check if the gas level is greater than or equal to the threshold
  if (gasLevel >= threshold) {
    digitalWrite(buzzerPin, HIGH); // Turn the buzzer ON
  } else {
    digitalWrite(buzzerPin, LOW);  // Turn the buzzer OFF
  }

  delay(100); // Short delay for stability
}
