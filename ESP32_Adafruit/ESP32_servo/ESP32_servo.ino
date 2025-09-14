#include <ESP32Servo.h>

Servo myServo;  
int servoPin = 9;  
int LDR_Top = A1;    // ADC1 pin for LDR Top  
int LDR_Bottom = A2; // ADC1 pin for LDR Bottom  
int initialPosition = 90;  

void setup() {
    Serial.begin(115200);
    
    analogReadResolution(12); // 12-bit ADC (0-4095)
    
    myServo.attach(servoPin);
    myServo.write(initialPosition);
    delay(500);  // Short delay for stabilization
}

void loop() {
    int topValue = analogRead(LDR_Top);
    int bottomValue = analogRead(LDR_Bottom);

    Serial.print("Top LDR: "); Serial.print(topValue);
    Serial.print(" | Bottom LDR: "); Serial.println(bottomValue);

    int difference = topValue - bottomValue;

    // Map the difference to a range of 30° to 160°
    int angle = map(difference, -4095, 4095, -60, 70);
    angle = constrain(angle, -60, 70);  // Ensure angle stays in range

    int targetAngle = angle + 90;  // Adjusted for 30° to 160° range
    targetAngle = constrain(targetAngle, 30, 160);

    myServo.write(targetAngle);  // Move the servo

    delay(300);  // Faster response
}
