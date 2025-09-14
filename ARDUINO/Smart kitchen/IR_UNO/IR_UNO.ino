const int irSensorPin = 4; // Connect the signal pin of the IR sensor to digital pin 7
const int ledPin = 13;     // Built-in LED on pin 13 for indication

void setup() {
  pinMode(irSensorPin, INPUT); // Set the IR sensor pin as input
  pinMode(ledPin, OUTPUT);     // Set the LED pin as output
  Serial.begin(9600);          // Start serial communication
}

void loop() {
  int irValue = digitalRead(irSensorPin); // Read the IR sensor value

  Serial.print("IR Sensor State: ");
  Serial.println(irValue); // Display the sensor state (0 or 1)

  if (irValue == HIGH) { // Object detected
    digitalWrite(ledPin, HIGH); // Turn ON the LED
  } else {              // No object detected
    digitalWrite(ledPin, LOW);  // Turn OFF the LED
  }

  delay(100); // Small delay for stability
}
