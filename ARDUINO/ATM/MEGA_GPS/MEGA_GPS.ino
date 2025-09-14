#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

// Define SoftwareSerial pins
#define RXPin 17
#define TXPin 16

SoftwareSerial gpsSerial(RXPin, TXPin); // RX, TX
TinyGPSPlus gps;

void setup()
{
  Serial.begin(115200);     // Serial Monitor
  gpsSerial.begin(9600);    // GPS baud rate

  Serial.println(F("Initializing GPS..."));
}

void loop()
{
  while (gpsSerial.available() > 0)
  {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isUpdated())
  {
    double latitude = gps.location.lat();
    double longitude = gps.location.lng();

    Serial.print(F("Latitude: "));
    Serial.println(latitude, 6);
    Serial.print(F("Longitude: "));
    Serial.println(longitude, 6);

    // Generate and print the Google Maps link
    Serial.print(F("Google Maps Link: "));
    Serial.print("https://www.google.com/maps?q=");
    Serial.print(latitude, 6);
    Serial.print(",");
    Serial.println(longitude, 6);
    Serial.println();
  }
}
