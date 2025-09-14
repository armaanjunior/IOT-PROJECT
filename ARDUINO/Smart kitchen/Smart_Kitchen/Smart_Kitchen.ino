#include <Servo.h>

// IR Sensor and LED Pin Definitions
const int irSensorPin = 4;  // Connect the signal pin of the IR sensor to digital pin 4
const int ledPin = 7;      // Built-in LED on pin 13 for indication

// Gas Sensor and Buzzer  .
const int gasSensorPin = A0; // Connect A0 to the analog output pin of MQ2
const int buzzerPin = 2;     // Connect the buzzer to digital pin 2
const int threshold = 85;    // Threshold value for gas level

// Servo 1 (IR sensor control)
Servo servo1;               // Create a servo object for IR sensor
const int servoPin1 = 3;    // Servo connected to pin 3

// Servo 2 (Gas sensor control)
Servo servo2;               // Create a servo object for gas sensor
const int servoPin2 = 5;    // Servo connected to pin 5
bool servoAt90 = false;     // Track if servo 2 is already at 90 degrees

void setup() {
  // Setup for gas sensor and buzzer
  Serial.begin(9600);         // Start the serial monitor
  pinMode(gasSensorPin, INPUT); // Set gas sensor pin as input
  pinMode(buzzerPin, OUTPUT);   // Set buzzer pin as output

  // Setup for servo 1 (IR sensor)
  servo1.attach(servoPin1);    // Attach servo to pin 3
  servo1.write(0);             // Set servo 1 to its initial position (0 degrees)

  // Setup for servo 2 (Gas sensor)
  servo2.attach(servoPin2);    // Attach servo to pin 5
  servo2.write(0);             // Set servo 2 to its initial position (0 degrees)

  // IR Sensor and LED Setup
  pinMode(irSensorPin, INPUT); // Set the IR sensor pin as input
  pinMode(ledPin, OUTPUT);     // Set the LED pin as output
}

void loop() {
  // Gas sensor and buzzer logic
  int gasLevel = analogRead(gasSensorPin); // Read the analog value from the gas sensor
  Serial.print("Gas Level: ");
  Serial.println(gasLevel); // Display raw analog value

  if (gasLevel >= threshold) {
    digitalWrite(buzzerPin, HIGH); // Turn the buzzer ON

    // Move servo 2 to 90 degrees if not already there
    if (!servoAt90) {
      servo2.write(90);           // Rotate servo 2 to 90 degrees
      servoAt90 = true;           // Update flag
    }
  } else {
    digitalWrite(buzzerPin, LOW);  // Turn the buzzer OFF
    // Servo 2 remains at 90 degrees even when gas level drops
  }

  // IR sensor logic
  int irValue = digitalRead(irSensorPin); // Read the IR sensor value

  if (irValue == LOW) { // If object is detected
    digitalWrite(ledPin, HIGH); // Turn ON the LED
    servo1.write(180);          // Rotate servo 1 to 180 degrees
  } else {              // If no object is detected
    digitalWrite(ledPin, LOW);  // Turn OFF the LED
    servo1.write(0);            // Rotate servo 1 back to 0 degrees
  }

  delay(100); // Small delay for stability
}
