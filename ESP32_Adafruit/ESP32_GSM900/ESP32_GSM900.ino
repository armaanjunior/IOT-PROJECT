#include <SoftwareSerial.h>

// Define pins for Software Serial
#define gsmSerialTx 10  // ESP32 TX to GSM RX
#define gsmSerialRx 9   // ESP32 RX to GSM TX

// Create SoftwareSerial object
SoftwareSerial gsmSerial(gsmSerialRx, gsmSerialTx);

void setup() {
  // Start Serial Monitor
  Serial.begin(115200);
  
  // Try different baud rates if the default doesn't work
  const int baudRates[] = {9600, 19200, 38400, 57600, 115200};
  bool connected = false;

  for (int i = 0; i < sizeof(baudRates)/sizeof(baudRates[0]); i++) {
    gsmSerial.begin(baudRates[i]);
    Serial.print("Trying baud rate: ");
    Serial.println(baudRates[i]);

    delay(1000);  // Wait for GSM module to initialize
    
    gsmSerial.println("AT");
    delay(1000);
    
    if (gsmSerial.available()) {
      String response = gsmSerial.readString();
      Serial.println(response);  // Print the response from GSM module
      connected = true;
      break;
    } else {
      Serial.println("No response, retrying...");
    }
  }

  if (!connected) {
    Serial.println("Failed to connect to GSM module.");
  } else {
    Serial.println("GSM Module connected");
  }
}

void loop() {
  if (!gsmSerial) return;  // Return if not connected
  
  // Example: Send and receive SMS
  gsmSerial.println("AT+CMGF=1");  // Set SMS mode to text
  delay(1000);
  gsmSerial.println("AT+CMGS=\"+919740917537\"");  // Replace with the recipient's number
  delay(1000);
  gsmSerial.println("Hello from ESP32!");  // SMS content
  gsmSerial.write(26);  // ASCII Ctrl+Z to send the SMS
  delay(5000);  // Repeat every 5 seconds
}
