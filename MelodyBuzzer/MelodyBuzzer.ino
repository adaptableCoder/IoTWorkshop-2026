const int touchSensorPin = 34;   // KY-036 analog out
const int buzzerPin = 25;        // Active buzzer pin
int threshold = 2000;            // Adjust as needed


// === Luis Fonsi - Despacito (Chorus Hook) ===
int melody[] = {
  // Des-pa-ci-to
  587, 554, 494, 370,                      // D5, C#5, B4, F#4
  
  // Pasito a pasito, suave suavecito
  370, 370, 370, 370, 494, 494, 494, 494,  // F#4s then B4s
  494, 440, 494, 392,                      // B4, A4, B4, G4
  
  // Poquito a poquito, suave suavecito
  392, 392, 392, 392, 494, 494, 494, 494,  // G4s then B4s
  494, 554, 587, 440                       // B4, C#5, D5, A4
};

int noteDurations[] = {
  // Des-pa-ci-to timing
  4, 8, 4, 2,                              
  
  // Pasito a pasito timing (Fast 16th notes)
  16, 16, 16, 16, 16, 16, 16, 16,          
  8, 8, 8, 2,                              
  
  // Poquito a poquito timing
  16, 16, 16, 16, 16, 16, 16, 16,          
  8, 8, 8, 2                               
};
int noteIndex = 0;

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  int touchValue = analogRead(touchSensorPin);
  Serial.println(touchValue);

  if (touchValue < threshold) {
    playCurrentNote();
  } else {
    noTone(buzzerPin);  // Silence if not touching
  }

  delay(20);
}

void playCurrentNote() {
  if (noteIndex >= sizeof(melody) / sizeof(int)) {
    noteIndex = 0;  // Restart riff
    return;
  }

  int duration = 1290 / noteDurations[noteIndex];
  tone(buzzerPin, melody[noteIndex]);
  delay(duration);
  noTone(buzzerPin);
  delay(30);  // spacing

  noteIndex++;
}
