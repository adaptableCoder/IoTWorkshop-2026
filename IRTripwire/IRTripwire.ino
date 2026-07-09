#include <WiFi.h>
#include <WebServer.h>

// === Wi-Fi Credentials ===
const char* ssid = "NSUT_WIFI";
const char* password = "";

// === IR Receiver Pin ===
const int irPin = 14;  // Connect OUT from IR receiver to GPIO14

// === Web Server ===
WebServer server(80);

unsigned long lastIRTime = 0;

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta http-equiv='refresh' content='1'><title>IR Receiver</title></head><body>";
  html += "<h2>ESP32 Basic IR Detection</h2>";

  if (millis() - lastIRTime < 500) {
    html += "<p style='color:green;'>IR Signal Detected!</p>";
  } else {
    html += "<p style='color:red;'>No IR Signal Recently</p>";
  }

  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  // IR pin setup
  pinMode(irPin, INPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());

  // Start Web Server
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web server started.");

  Serial.println("IR detection ready on pin " + String(irPin));
}

void loop() {
  server.handleClient();

  // Check IR pin for LOW signal
  if (digitalRead(irPin) == LOW) {
    lastIRTime = millis();
    Serial.println("IR light detected!");
    delay(100);  // Simple debounce
  }
}
