#include <Servo.h>
Servo myServo; // Create a servo object
void setup() {
  myServo.attach(5); // Attach servo to pin 3
  myServo.write(0);  // Set servo to its original position (0 degrees)
  delay(1000);       // Wait for 1 second
}
void loop() {
  myServo.write(90); // Rotate servo to 90 degrees
  delay(1000);       // Wait for 1 second
  myServo.write(0);
  delay(1000);
}
