#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "NSUT_WIFI";
const char* password = "";

WebServer server(80);

int LDR = 34;
int buzzer = 4;
int laser = 5;

// PWM settings
const int buzzerChannel = 0;
const int buzzerFreq = 2000;      // 2 kHz
const int buzzerResolution = 8;

String tripwireStatus = "INTACT";

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta http-equiv='refresh' content='1'>";
  html += "<title>Laser Tripwire</title>";
  html += "<style>body{font-family: Arial; text-align: center; margin-top: 50px;";
  if (tripwireStatus == "BROKEN") {
    html += "background-color: red; color: white;";
  } else {
    html += "background-color: #d4edda; color: #155724;";
  }
  html += "}</style></head><body>";
  html += "<h1>Laser Tripwire Security</h1>";
  html += "<h2>Status: " + tripwireStatus + "</h2>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  pinMode(LDR, INPUT);
  pinMode(laser, OUTPUT);
  digitalWrite(laser, HIGH);

  ledcAttach(buzzer, buzzerFreq, buzzerResolution);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected! IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();

  int value = analogRead(LDR);
  
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    Serial.println(value);
    lastPrint = millis();
  }
  
  if (value < 1000) {
    tripwireStatus = "BROKEN";
    ledcWrite(buzzer, 128);   // 50% duty cycle -> buzzer ON
  } else {
    tripwireStatus = "INTACT";
    ledcWrite(buzzer, 0);     // buzzer OFF
  }
}