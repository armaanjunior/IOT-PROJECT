#include <Servo.h>

// IR Sensor and LED Pin Definitions
const int irSensorPin = 4;  // Connect the signal pin of the IR sensor to digital pin 4
const int ledPin = 13;      // Built-in LED on pin 13 for indication

// Servo Object and Pin
Servo myServo;              // Create a servo object
const int servoPin = 3;     // Servo connected to pin 3

void setup() {
  // IR Sensor and LED Setup
  pinMode(irSensorPin, INPUT); // Set the IR sensor pin as input
  pinMode(ledPin, OUTPUT);     // Set the LED pin as output

  // Servo Setup
  myServo.attach(servoPin);    // Attach servo to pin 3
  myServo.write(0);            // Set servo to its original position (0 degrees)

  Serial.begin(9600);          // Start serial communication
}

void loop() {
  // Read the IR sensor value
  int irValue = digitalRead(irSensorPin);

  // Print the IR sensor state to the Serial Monitor
  Serial.print("IR Sensor State: ");
  Serial.println(irValue);

  if (irValue == LOW) { // If object is detected
    digitalWrite(ledPin, HIGH); // Turn ON the LED
    myServo.write(180);         // Rotate servo to 180 degrees
    delay(1000);                // Wait for 1 second
    myServo.write(0);           // Rotate servo back to 0 degrees
    delay(1000);                // Wait for 1 second
  } else {              // If no object is detected
    digitalWrite(ledPin, LOW);  // Turn OFF the LED
  }

  delay(100); // Small delay for stability
}
