#include <TinyGPSPlus.h>

// Define hardware serial ports for GPS and GSM
#define GPS_SERIAL Serial2 // GPS module connected here
#define GSM_SERIAL Serial3 // GSM SIM900A module connected here

// The TinyGPSPlus object
TinyGPSPlus gps;

// Target phone number
const char phoneNumber[] = "+919740917537"; // Replace with the recipient's phone number

// Track initialization state
bool isGSMInitialized = false;

void setup()
{
  Serial.begin(115200);      // Serial Monitor for debugging
  GPS_SERIAL.begin(9600);    // Start communication with GPS module
  GSM_SERIAL.begin(9600);    // Start communication with GSM module

  Serial.println(F("Initializing GPS and GSM..."));

  initializeGSM();
}

void loop()
{
  // Feed GPS data into TinyGPSPlus
  while (GPS_SERIAL.available() > 0)
  {
    gps.encode(GPS_SERIAL.read());
  }

  // If location is updated, send it via SMS
  if (gps.location.isUpdated())
  {
    double latitude = gps.location.lat();
    double longitude = gps.location.lng();

    Serial.print(F("Latitude: "));
    Serial.println(latitude, 10);
    Serial.print(F("Longitude: "));
    Serial.println(longitude, 10);

    // Generate the Google Maps URL
    String locationURL = "https://www.google.com/maps?q=";
    locationURL += String(latitude, 10);
    locationURL += ",";
    locationURL += String(longitude, 10);

    Serial.print(F("Google Maps Link: "));
    Serial.println(locationURL);

    // Send the location via SMS
    sendSMS(phoneNumber, locationURL);
  }
}

// Function to initialize GSM module
void initializeGSM()
{
  if (!isGSMInitialized) // Check if GSM is already initialized
  {
    GSM_SERIAL.println("AT"); // Test the GSM module
    delay(1000);
    GSM_SERIAL.println("AT+CMGF=1"); // Set SMS mode to text
    delay(1000);
    Serial.println(F("GSM Module Initialized."));
    isGSMInitialized = true; // Mark GSM as initialized
  }
}

// Function to send an SMS
void sendSMS(const char *number, String message)
{
  GSM_SERIAL.print("AT+CMGS=\"");
  GSM_SERIAL.print(number);
  GSM_SERIAL.println("\"");
  delay(1000);

  GSM_SERIAL.print(message); // Send the message
  delay(1000);

  GSM_SERIAL.write(26); // End of message with Ctrl+Z
  delay(3000);

  Serial.println(F("SMS Sent."));
}
