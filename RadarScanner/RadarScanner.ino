#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>

// ---------------- WiFi Configuration ----------------
const char* ssid = "NSUT_WIFI";
const char* password = "";
WebServer server(80);

// ---------------- Hardware Pins ----------------
Servo myservo;
#define TRIG 5
#define ECHO 18
#define SERVOPIN 4

// ---------------- Scan Speed Configuration ----------------
// The servo sweeps faster as an object gets closer.
const float DIST_MIN = 5.0;    // cm - closest range (maximum speed)
const float DIST_MAX = 50.0;   // cm - farthest range (minimum speed)
const int MIN_DELAY = 5;       // ms per step at closest range (fastest)
const int MAX_DELAY = 40;      // ms per step at farthest range (slowest)
const int STEP_SIZE = 5;       // servo increment in degrees

// ---------------- Live State ----------------
long currentDistance = 0;
int currentDelay = MAX_DELAY;
String speedStatus = "SLOW";

/**
 * Non-blocking delay. Waits for the given duration while continuing
 * to serve web requests, so the dashboard stays responsive.
 */
void waitAndServe(int ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    server.handleClient();
    delay(1);
  }
}

/**
 * Triggers the HC-SR04 and returns the measured distance in centimetres.
 * Returns DIST_MAX if no echo is received within the timeout.
 */
long getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000);  // 30 ms timeout
  if (duration == 0) return DIST_MAX;          // no echo: treat as out of range

  return duration * (0.034 / 2);               // speed of sound, halved for round trip
}

/**
 * Maps the measured distance to a per-step delay.
 * Closer object -> shorter delay -> faster sweep.
 */
int calculateDelay(long distance) {
  float percentage = (float)(distance - DIST_MIN) / (DIST_MAX - DIST_MIN);
  if (percentage > 1.0) percentage = 1.0;
  if (percentage < 0.0) percentage = 0.0;

  return MIN_DELAY + (percentage * (MAX_DELAY - MIN_DELAY));
}

/**
 * Serves the live dashboard. Auto-refreshes every second and turns
 * red when an object is detected within the alert range.
 */
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta http-equiv='refresh' content='1'>";
  html += "<title>Radar Scanner</title>";
  html += "<style>body{font-family: Arial; text-align: center; margin-top: 50px;";
  if (currentDistance < 15) {
    html += "background-color: #f8d7da; color: #721c24;";
  } else {
    html += "background-color: #d4edda; color: #155724;";
  }
  html += "}</style></head><body>";
  html += "<h1>Ultrasonic Radar Scanner</h1>";
  html += "<h2>Distance: " + String(currentDistance) + " cm</h2>";
  html += "<h2>Servo Speed: " + speedStatus + "</h2>";
  html += "<h3>Step Delay: " + String(currentDelay) + " ms</h3>";
  if (currentDistance < 15) {
    html += "<h2 style='color:red;'>OBJECT CLOSE - SCANNING FAST</h2>";
  }
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // Reserve hardware timers and configure the servo pulse range
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  myservo.setPeriodHertz(50);
  myservo.attach(SERVOPIN, 500, 2400);
  myservo.write(0);

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
  Serial.println("Radar Ready.");
}

/**
 * Executes a single scan step: moves the servo, measures the distance,
 * updates the sweep speed, and waits for the calculated interval.
 */
void doStep(int angle) {
  myservo.write(angle);

  currentDistance = getDistance();
  currentDelay = calculateDelay(currentDistance);

  if (currentDelay <= 12)      speedStatus = "VERY FAST";
  else if (currentDelay <= 25) speedStatus = "MEDIUM";
  else                         speedStatus = "SLOW";

  Serial.print("Angle: ");
  Serial.print(angle);
  Serial.print(" | Distance: ");
  Serial.print(currentDistance);
  Serial.print(" cm | Delay: ");
  Serial.print(currentDelay);
  Serial.print(" ms | Speed: ");
  Serial.println(speedStatus);

  waitAndServe(currentDelay);
}

void loop() {
  server.handleClient();

  // Forward sweep
  for (int angle = 0; angle <= 180; angle += STEP_SIZE) {
    doStep(angle);
  }

  // Return sweep
  for (int angle = 180; angle >= 0; angle -= STEP_SIZE) {
    doStep(angle);
  }
}