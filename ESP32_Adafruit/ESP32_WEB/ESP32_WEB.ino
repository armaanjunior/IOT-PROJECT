#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "MShelby";       // Your Wi-Fi SSID
const char* password = "Mustang46"; // Your Wi-Fi password

// Create a WebServer object on port 80
WebServer server(80);

void setup() {
  // Start Serial Monitor
  Serial.begin(115200);
  delay(1000); // Allow time for Serial Monitor to open
  Serial.println("\nStarting ESP32-S2 Server");

  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Define route for root URL
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/plain", "Hello, World!");
  });

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Handle incoming HTTP requests
  server.handleClient();
}
