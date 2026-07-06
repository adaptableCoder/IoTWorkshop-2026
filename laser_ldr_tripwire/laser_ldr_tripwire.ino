int LDR = 34;
int buzzer = 4;
int laser = 5;

// PWM settings
const int buzzerChannel = 0;
const int buzzerFreq = 2000;      // 2 kHz
const int buzzerResolution = 8;

void setup() {
  Serial.begin(115200);

  pinMode(LDR, INPUT);
  pinMode(laser, OUTPUT);

  digitalWrite(laser, HIGH);

  // Configure PWM for buzzer
  ledcAttach(buzzer, buzzerFreq, buzzerResolution);
}

void loop() {
  int value = analogRead(LDR);
  Serial.println(value);

  if (value < 1000) {
    ledcWrite(buzzer, 128);   // 50% duty cycle -> buzzer ON
  } else {
    ledcWrite(buzzer, 0);     // buzzer OFF
  }

  delay(50);
}