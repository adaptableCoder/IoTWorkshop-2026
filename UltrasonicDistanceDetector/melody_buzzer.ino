#include <WiFi.h>
#include <WebServer.h>

// --- Wi-Fi Network Credentials ---
const char* ssid = "NSUT_WIFI";
const char* password = "";

// --- Hardware Pin Configurations ---
const int TRIG_PIN = 5;
const int ECHO_PIN = 18;
const int BUZZER_PIN = 27;

const int RED_PIN = 13;
const int GREEN_PIN = 12;
const int BLUE_PIN = 14;

// --- Distance Threshold Configurations (in cm) ---
const int SAFE_DISTANCE = 30;    // Greater than 30cm = Green, No Alarm
const int WARNING_DISTANCE = 15; // Between 15cm and 30cm = Yellow/Orange, No Alarm

// --- Global Variable State ---
long currentDistance = 0;
String safetyStatus = "Safe";
String dynamicColor = "#10b981"; // Default Green Hex

WebServer server(80);

// --- Distance Calculation Engine ---
long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW); 

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout prevents code hang if echo misses
  if (duration == 0) return 999;                  // Return out-of-bounds constant if no echo
  
  long distance = duration * (0.034 / 2); 
  return distance;
}

// --- Asynchronous API Endpoint for Web Data ---
void handleReadings() {
  String jsonPayload = "{";
  jsonPayload += "\"distance\":" + String(currentDistance) + ",";
  jsonPayload += "\"status\":\"" + safetyStatus + "\",";
  jsonPayload += "\"color\":\"" + dynamicColor + "\"";
  jsonPayload += "}";
  server.send(200, "application/json", jsonPayload);
}

// --- HTML / CSS / JS UI Webpage Compilation ---
void handleRoot() {
  String html = R"rawhtml(
<!DOCTYPE html>
<html lang='en'>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>IoT Proximity Dashboard</title>
    <style>
        :root {
            --bg-dark: #0f172a;
            --card-bg: rgba(30, 41, 59, 0.7);
            --text-main: #f8fafc;
            --text-muted: #94a3b8;
        }
        * { box-sizing: border-box; margin: 0; padding: 0; font-family: '-apple-system', BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; }
        body {
            background: radial-gradient(circle at top right, #1e1b4b, var(--bg-dark));
            color: var(--text-main);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }
        .container {
            width: 100%;
            max-width: 480px;
            background: var(--card-bg);
            backdrop-filter: blur(16px);
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: 24px;
            padding: 32px;
            box-shadow: 0 20px 40px rgba(0,0,0,0.3);
            text-align: center;
        }
        h1 { font-size: 1.8rem; font-weight: 700; margin-bottom: 8px; letter-spacing: -0.5px; }
        .subtitle { color: var(--text-muted); font-size: 0.95rem; margin-bottom: 32px; }
        .metric-card {
            background: rgba(15, 23, 42, 0.5);
            border-radius: 16px;
            padding: 24px;
            margin-bottom: 24px;
            border: 1px solid rgba(255, 255, 255, 0.05);
        }
        .label { font-size: 0.85rem; text-transform: uppercase; letter-spacing: 1.5px; color: var(--text-muted); margin-bottom: 8px; }
        .value { font-size: 3.5rem; font-weight: 800; line-height: 1; margin-bottom: 4px; transition: color 0.3s ease; }
        .unit { font-size: 1.2rem; font-weight: 500; color: var(--text-muted); }
        .status-badge {
            display: inline-block;
            padding: 8px 20px;
            border-radius: 100px;
            font-size: 0.9rem;
            font-weight: 600;
            letter-spacing: 0.5px;
            text-transform: uppercase;
            transition: all 0.3s ease;
        }
        .footer { margin-top: 32px; font-size: 0.8rem; color: var(--text-muted); }
    </style>
</head>
<body>
    <div class='container'>
        <h1>Proximity Monitor</h1>
        <p class='subtitle'>ESP32 Distance Detector System</p>
        
        <div class='metric-card'>
            <p class='label'>Live Distance</p>
            <div class='value' id='distance-val'>-- <span class='unit'>cm</span></div>
        </div>

        <div class='status-badge' id='status-lbl' style='background: rgba(255,255,255,0.1); color: #fff;'>
            Initializing...
        </div>

        <p class='footer'>Node MCU System Running Asynchronous Polling</p>
    </div>

    <script>
        function updateDashboard() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    const valEl = document.getElementById('distance-val');
                    const lblEl = document.getElementById('status-lbl');
                    
                    valEl.innerHTML = data.distance + " <span class='unit'>cm</span>";
                    valEl.style.color = data.color;
                    
                    lblEl.innerText = data.status;
                    lblEl.style.backgroundColor = data.color + "20"; // Subtle 12% alpha background
                    lblEl.style.color = data.color;
                    lblEl.style.border = "1px solid " + data.color;
                })
                .catch(err => console.error("Error fetching IoT metrics:", err));
        }
        // Poll every 250ms for near instantaneous responsive feedback
        setInterval(updateDashboard, 250);
    </script>
</body>
</html>
  )rawhtml";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  // Pin Type Assignments
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Set system to clear starting state
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);

  // Establish Wi-Fi Connection Link
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Local Wi-Fi Network");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connection Established successfully!");
  Serial.print("Local Network IP Target: http://");
  Serial.println(WiFi.localIP());

  // Mount API Paths
  server.on("/", handleRoot);
  server.on("/data", handleReadings);
  server.begin();
}

void loop() {
  server.handleClient(); // Essential background web processing routine

  currentDistance = getDistance();

  // --- Multi-Tier Environmental Safety Logic ---
  if (currentDistance > SAFE_DISTANCE) {
    // Condition 1: Green Zone (Safe) -> Light Green, No Audio Alarm
    safetyStatus = "Clear / Safe";
    dynamicColor = "#10b981";
    
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  } 
  else if (currentDistance <= SAFE_DISTANCE && currentDistance > WARNING_DISTANCE) {
    // Condition 2: Yellow/Orange Zone (Warning) -> Yellow/Orange Mix, No Audio Alarm
    safetyStatus = "Warning Approaching";
    dynamicColor = "#f59e0b";
    
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, HIGH); // R + G turns standard RGB packages Yellow/Orange
    digitalWrite(BLUE_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  } 
  else {
    // Condition 3: Red Zone (Danger/Alarm) -> Strong Red, Active Alarm Blasts
    safetyStatus = "CRITICAL BREACH!";
    dynamicColor = "#ef4444";
    
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
    
    // Asynchronous pulsing routine creates an alarming chirp pattern without using dynamic delay() blocks
    if ((millis() / 150) % 2 == 0) {
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
    } else {
      digitalWrite(RED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
    }
  }
}