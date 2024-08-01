// Steuerung für eine Arbeitsplattenbeleuchtung mit einer IR-LED, einem IR-Phototransistor und einem Relais, um eine 230V Lampe zu steuern.
// Funktion:
// - IR-LED wird per Transistor immer wieder an- und ausgeschaltet, der Phototransistor aufgelesen und dadurch ein Gegenstand (z.B. Hand) für eine bestimmte Dauer erkannt und das Relais/die Lampe folgend angesteuert:
//     - unterhalb 0,2 Sekunden --> nichts passiert
//     - zwischen 0,2 und 1,0 Sekunden --> Relais/Lampe ON (für 1h) oder OFF
//     - zwischen 1,0 und 4,0 Sekunden --> Relais/Lampe geht oder bleibt ON für 2h
//     - über 4,0 Sekunden --> nichts passiert
// - Sobald ein Gegenstand erkannt wird, leuchtet die grüne LED. Der Abstand, ab dem ein Gegenstand erkannt wird, kann über den Schaltwert "IRPhotoSchalt" geändert werden.
// - Wenn erkannt wird, dass die IR-LED nicht mehr funktioniert, wird über die grüne LED der Morsecode "SOS" ausgegeben. (kurz/kurz/kurz - Pause - lang/lang/lang - Pause - kurz/kurz/kurz)

// Pin Definitionen
int Relais = 6;               // Relais an Pin 6
int IRPhoto = 3;              // IR-Phototransistor an Pin A3
int IRLED = 2;                // IR-LED an Pin 2
int StatusLED = 4;            // Grüne LED an Pin 4

// Relais-Status und Timer
int R1 = 0;                   // Relais-Status (1=ON, 0=OFF)
unsigned long RelaisTime = 0;
unsigned long RelaisTimeOn = 3600000;        // 1 Stunde ON --> OFF
unsigned long RelaisTimeOnExt = 7200000;     // 2 Stunden ON --> OFF

// Variablen für IR-Phototransistor
int IRPhotoreed;
int IRPhotoWert1, IRPhotoWert2, IRPhotoWert3;
int IRPhotoWert11, IRPhotoWert12, IRPhotoWert13;
int IRPhotoWert21, IRPhotoWert22, IRPhotoWert23;
int IRPhotoWert31, IRPhotoWert32, IRPhotoWert33;
int IRPhotoSchalt = 50;             // Schaltwert
int IRPhotoOffset1, IRPhotoOffset2;

// Timer und Sammel-Variablen
unsigned long previousMillis = 0;
unsigned long currentMillis;
int warten = 25;
int SetSammel = 0;
int SetCount = 0;
int Set[20];
int sos = 0;
int fail = 0;

void setup() {
  // Initialisierung der Pins
  pinMode(Relais, OUTPUT);
  digitalWrite(Relais, LOW);
  R1 = 0;
  pinMode(IRPhoto, INPUT);
  pinMode(IRLED, OUTPUT);
  pinMode(StatusLED, OUTPUT);
  
  // Initialisierung der seriellen Kommunikation
  Serial.begin(9600);

  // Kalibrierung der IR-LED und des IR-Phototransistors
  digitalWrite(IRLED, HIGH);
  delay(1000);
  digitalWrite(IRLED, LOW);
  delay(warten);
  IRPhotoWert1 = analogRead(IRPhoto);
  digitalWrite(IRLED, HIGH);
  delay(warten);
  IRPhotoWert2 = analogRead(IRPhoto);
  digitalWrite(IRLED, LOW);
  delay(warten);
  IRPhotoWert3 = analogRead(IRPhoto);
  IRPhotoOffset1 = IRPhotoWert2 - ((IRPhotoWert1 + IRPhotoWert3) / 2);
  previousMillis = millis();
}

void loop() {
  currentMillis = millis();

  // Mehrfache Messung zur Erhöhung der Genauigkeit
  IRPhotoWert11 = analogRead(IRPhoto);
  delayMicroseconds(100);
  IRPhotoWert12 = analogRead(IRPhoto);
  delayMicroseconds(100);
  IRPhotoWert13 = analogRead(IRPhoto);
  digitalWrite(IRLED, HIGH);
  delay(warten);
  IRPhotoWert21 = analogRead(IRPhoto);
  delayMicroseconds(100);
  IRPhotoWert22 = analogRead(IRPhoto);
  delayMicroseconds(100);
  IRPhotoWert23 = analogRead(IRPhoto);
  digitalWrite(IRLED, LOW);
  delay(warten);
  IRPhotoWert31 = analogRead(IRPhoto);
  delayMicroseconds(100);
  IRPhotoWert32 = analogRead(IRPhoto);
  delayMicroseconds(100);
  IRPhotoWert33 = analogRead(IRPhoto);

  // Mittelwertbildung der Messwerte
  IRPhotoWert1 = (IRPhotoWert11 + IRPhotoWert12 + IRPhotoWert13) / 3;
  IRPhotoWert2 = (IRPhotoWert21 + IRPhotoWert22 + IRPhotoWert23) / 3;
  IRPhotoWert3 = (IRPhotoWert31 + IRPhotoWert32 + IRPhotoWert33) / 3;

  // Ausgabe zur Fehlersuche
  // Serial.println("OFF1: " + String(IRPhotoWert1) + "  ON: " + String(IRPhotoWert2) + "  OFF2: " + String(IRPhotoWert3) + " Offset1: " + String(IRPhotoOffset1) + " Offset2: " + String(IRPhotoOffset2) + " fail: " + String(fail));
  // Serial.println(" OFF1: " + String(IRPhotoWert1) + "  ON: " + String(IRPhotoWert2) + "    Count " + String(SetCount) + " Sammel " + String(SetSammel));

  // Überprüfung auf stabile Werte
  if (IRPhotoWert1 + 15 > IRPhotoWert3 && IRPhotoWert1 - 15 < IRPhotoWert3) {
    IRPhotoOffset2 = IRPhotoWert2 - ((IRPhotoWert1 + IRPhotoWert3) / 2);
    
    // Erkennung eines möglichen Defekts der IR-LED
    if (IRPhotoWert2 < ((IRPhotoWert1 + IRPhotoWert3) / 2) + 10) {
      SOS();
    }

    // Objekt erkannt
    if (IRPhotoOffset2 > (IRPhotoOffset1 + IRPhotoSchalt)) {
      digitalWrite(StatusLED, HIGH);  // Grüne LED an
    } else {
      digitalWrite(StatusLED, LOW);   // Grüne LED aus
    }

    if (IRPhotoOffset2 <= (IRPhotoOffset1 + IRPhotoSchalt)) {
      if ((previousMillis + 4000) < currentMillis) {
        digitalWrite(StatusLED, LOW);  // Grüne LED aus
      }
      if (((previousMillis + 200) > currentMillis) || ((previousMillis + 4000) < currentMillis)) {
        previousMillis = currentMillis;
      }
      if (((previousMillis + 210) < currentMillis) && ((previousMillis + 990) > currentMillis)) {
        digitalWrite(StatusLED, LOW);  // Grüne LED aus
        LichtAn(1);  // Licht für 1 Stunde an oder aus
      }
      if (((previousMillis + 1000) < currentMillis) && ((previousMillis + 3990) > currentMillis)) {
        digitalWrite(StatusLED, LOW);  // Grüne LED aus
        LichtAn(2);  // Licht für 2 Stunden an
      }
      if (R1 == 1 && RelaisTime < currentMillis) {
        LichtAn(1);  // Licht aus nach der festgelegten Zeit
      }
    }

    // Neue Kalibrierung bei Werten nahe 0
    if (IRPhotoWert1 == 0 && IRPhotoWert3 == 0) {
      SetCount++;
      if (SetCount > 100) {
        Set[(SetCount - 101)] = IRPhotoWert2;
        if (SetCount == 120) {
          SetSammel = 0;
          for (int i = 0; i < 20; i++) {
            SetSammel += Set[i];
          }
          IRPhotoOffset1 = SetSammel / 20;
          SetCount = 0;
          SetSammel = 0;
        }
      }
    } else {
      SetCount = 0;
      SetSammel = 0;
    }
    delay(warten);
  } else {
    // Anpassung des Timers
    previousMillis = previousMillis - currentMillis + millis();
  }
}

void LichtAn(int x) {
  if (R1 == 0) {  // Licht an
    digitalWrite(Relais, HIGH);
    delay(500);
    R1 = 1;
    if (x == 1) {
      RelaisTime = (currentMillis + RelaisTimeOn);  // Licht an für 1 Stunde
    }
    if (x == 2) {
      RelaisTime = (currentMillis + RelaisTimeOnExt);  // Licht an für 2 Stunden
    }
  } else if (x == 1) {  // Licht aus
    digitalWrite(Relais, LOW);
    R1 = 0;
    delay(500);
  }
  currentMillis = millis();
  previousMillis = currentMillis;
}

void SOS() {
  digitalWrite(Relais, LOW);  // Relais aus
  while (sos < 100) {
    sos++;
    // Ausgabe des Morse-Codes "SOS" über die grüne LED
    for (int i = 0; i < 3; i++) {
      digitalWrite(StatusLED, HIGH);
      delay(400);
      digitalWrite(StatusLED, LOW);
      delay(200);
    }
    delay(1000);
    for (int i = 0; i < 3; i++) {
      digitalWrite(StatusLED, HIGH);
      delay(800);
      digitalWrite(StatusLED, LOW);
      delay(400);
    }
    delay(1000);
    for (int i = 0; i < 3; i++) {
      digitalWrite(StatusLED, HIGH);
      delay(400);
      digitalWrite(StatusLED, LOW);
      delay(200);
    }
    delay(4000);
  }
  sos = 0;
}
