#include <Servo.h>

// Gas Sensor and Buzzer
const int gasSensorPin = A0; // Connect A0 to the analog output pin of MQ2
const int buzzerPin = 2;     // Connect the buzzer to digital pin 2
const int threshold = 45;    // Threshold value for gas level

// Servo
Servo myServo;               // Create a servo object
const int servoPin = 5;      // Servo connected to pin 5
bool servoAt90 = false;      // Track if the servo is already at 90 degrees

void setup() {
  // Setup for gas sensor and buzzer
  Serial.begin(9600);         // Start the serial monitor
  pinMode(gasSensorPin, INPUT); // Set gas sensor pin as input
  pinMode(buzzerPin, OUTPUT);   // Set buzzer pin as output

  // Setup for servo
  myServo.attach(servoPin);    // Attach servo to pin 5
  myServo.write(0);            // Set servo to its original position (0 degrees)
}

void loop() {
  int gasLevel = analogRead(gasSensorPin); // Read the analog value from the sensor

  Serial.print("Gas Level: ");
  Serial.println(gasLevel); // Display raw analog value

  // Buzzer logic based on gas level
  if (gasLevel >= threshold) {
    digitalWrite(buzzerPin, HIGH); // Turn the buzzer ON

    // Move servo to 90 degrees if not already there
    if (!servoAt90) {
      myServo.write(90);           // Rotate servo to 90 degrees
      servoAt90 = true;            // Update flag
    }
  } else {
    digitalWrite(buzzerPin, LOW);  // Turn the buzzer OFF
    // Servo remains at 90 degrees even when gas level drops
  }

  delay(100); // Short delay for stability
}
