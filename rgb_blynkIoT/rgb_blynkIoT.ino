#define BLYNK_TEMPLATE_ID "TMPL3LGxyG3Jr"
#define BLYNK_TEMPLATE_NAME "RGB LED Slider"
#define BLYNK_AUTH_TOKEN "-gJzouWCyPnJHeJpmfuOerKbgZiq1BLF"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

//─── Wi‑Fi & Blynk credentials ─────────────────────────────
char ssid[]     = "NSUT_WIFI";
char pass[]     = "";

//─── RGB LED pins & PWM channels ───────────────────────────
#define RED_PIN 25
#define GREEN_PIN 26
#define BLUE_PIN 27

// PWM settings: 1 kHz frequency, 8‑bit resolution (0–255)
const int PWM_FREQ      = 10000;
const int PWM_RES_BITS  = 8;

void setup() {
  Serial.begin(115200);

  // Attach each channel to its pin
  ledcAttach(RED_PIN,   PWM_FREQ, PWM_RES_BITS);
  ledcAttach(GREEN_PIN, PWM_FREQ, PWM_RES_BITS);
  ledcAttach(BLUE_PIN,  PWM_FREQ, PWM_RES_BITS);

  // Connect to Blynk
  Serial.println("Connecting to Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("\nBlynk connected");
}

// Handler for slider on V1 → Red
BLYNK_WRITE(V1) {
  int value = param.asInt();          // 0–255 from slider
  ledcWrite(RED_PIN, value);           // Set PWM duty
  Serial.printf("Red = %d\n", value);
}

// Handler for slider on V2 → Green
BLYNK_WRITE(V2) {
  int value = param.asInt();
  ledcWrite(GREEN_PIN, value);
  Serial.printf("Green = %d\n", value);
}

// Handler for slider on V3 → Blue
BLYNK_WRITE(V3) {
  int value = param.asInt();
  ledcWrite(BLUE_PIN, value);
  Serial.printf("Blue = %d\n", value);
}

void loop() {
  Blynk.run();  // Process Blynk events
}
