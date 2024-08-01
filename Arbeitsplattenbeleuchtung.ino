// Steuerung für eine Arbeitsplattenbeleuchtung mit einer IR-LED, einem IR-Phototransistor und einem Relais um eine 230V Lampe zu steuern.
// Funktion: IR-LED wird per Transistor immer wieder an- und ausgeschaltet, der Photottransistor aufgelesen und dadurch ein Gegenstand (z.B. Hand) für eine bestimmte Dauer erkannt und das Relay/die Lampe folgend angesteuert:
// unterhalb 0,2 Sekunden --> nichts passiert
// zwischen 0,2 und 1,0 Sekunden --> Relay/Lampe ON (für 1h) oder OFF
// zwischen 1,0 und 4,0 Sekunden --> Relay/Lampe geht oder bleibt ON für 2h
// über 4,0 Sekunden --> nichts passiert
// Sobald ein Gegenstand "erkannt" wird, leuchtet die grüne LED. Der Abstand, ab dem ein Gegenstand erkannt wird, kann über den Schaltwert "IRPhotoSchalt" geändert werden.
// Je nachdem wie stark der Gegenstand reflektiert, desto eher wird er erkannt.
// Wenn erkannt wird, dass die IR-LED nicht mehr funktioniert, wird über die grüne LED der Morsecode "SOS" ausgegeben. (kurz/kurz/kurz - Pause - lang/lang/lang - Pause - kurz/kurz/kurz)


// Relay
int Relais = 6;             //Relais an Pin6
int R1 = 0;                 //Relais-Status (1=ON; 0=OFF)
unsigned long RelaisTime = 0;
unsigned long RelaisTimeOn = 3600000;        // 1 Stunde ON --> OFF
unsigned long RelaisTimeOnExt = 7200000;     // 2 Stunde ON --> OFF

// IR-Phototransistor
int IRPhoto = 3;           //IR-Transistor an Pin A3
int IRPhotoreed;

// IR-LED (via Transistor)
int IRLED = 2;             // IR-LED an Pin 2

// LED (green)
int StatusLED = 4;         // grüne LED an Pin 4

// Variablen
int warten = 25;
unsigned long previousMillis = 0;
unsigned long currentMillis;
int SetSammel = 0;
int SetCount = 0;
int Set[20];
int sos = 0;
int fail = 0;
int IRPhotoWert1;
int IRPhotoWert11;
int IRPhotoWert12;
int IRPhotoWert13;
int IRPhotoWert2;
int IRPhotoWert21;
int IRPhotoWert22;
int IRPhotoWert23;
int IRPhotoWert3;
int IRPhotoWert31;
int IRPhotoWert32;
int IRPhotoWert33;
int IRPhotoSchalt = 50;             // Schaltwert
int IRPhotoOffset1;
int IRPhotoOffset2;

void setup() {

  pinMode(Relais, OUTPUT);
  digitalWrite(Relais, LOW);
  R1 = 0;
  pinMode(IRPhoto, INPUT);
  pinMode(IRLED, OUTPUT);
  pinMode(StatusLED, OUTPUT);

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

  IRPhotoWert1 = (IRPhotoWert11 + IRPhotoWert12 + IRPhotoWert13) / 3;
  IRPhotoWert2 = (IRPhotoWert21 + IRPhotoWert22 + IRPhotoWert23) / 3;
  IRPhotoWert3 = (IRPhotoWert31 + IRPhotoWert32 + IRPhotoWert33) / 3;

  if (IRPhotoWert1 + 15 > IRPhotoWert3 && IRPhotoWert1 - 15 < IRPhotoWert3) {
    IRPhotoOffset2 = IRPhotoWert2 - ((IRPhotoWert1 + IRPhotoWert3) / 2);
    if ( IRPhotoWert2 < ((IRPhotoWert1 + IRPhotoWert3) / 2 ) + 10 )
      SOS();

    if ( IRPhotoOffset2 > (IRPhotoOffset1 + IRPhotoSchalt ))
      digitalWrite(StatusLED, HIGH);

    else
      digitalWrite(StatusLED, LOW);

    if ( IRPhotoOffset2 <= (IRPhotoOffset1 + IRPhotoSchalt )) {
      if ( (previousMillis + 4000) < currentMillis) {
        digitalWrite(StatusLED, LOW);
      }
      if ( ((previousMillis + 200) > currentMillis) || ((previousMillis + 4000) < currentMillis) ) {
        previousMillis = currentMillis;
      }
      if ( ((previousMillis + 210) < currentMillis) && ((previousMillis + 990) > currentMillis) ) {
        digitalWrite(StatusLED, LOW);
        LichtAn(1);
      }
      if ( ((previousMillis + 1000) < currentMillis) && ((previousMillis + 3990) > currentMillis) ) {
        digitalWrite(StatusLED, LOW);
        LichtAn(2);
      }
      if ( R1 == 1 && RelaisTime < currentMillis ) {
        LichtAn(1);
      }
    }

    if (IRPhotoWert1 == 0 && IRPhotoWert3 == 0) {
      SetCount++;
      if (SetCount > 100) {
        Set[(SetCount - 101)] = IRPhotoWert2;
        if (SetCount == 120) {
          SetSammel = Set[0] + Set[1] + Set[2] + Set[3] + Set[4] + Set[5] + Set[6] + Set[7] + Set[8] + Set[9] + Set[10] + Set[11] + Set[12] + Set[13] + Set[14] + Set[15] + Set[16] + Set[17] + Set[18] + Set[19];
          IRPhotoOffset1 = (SetSammel / 20);
          SetCount = 0;
          SetSammel = 0;
        }
      }
    }
    if (IRPhotoWert1 != 0 || IRPhotoWert3 != 0) {
      SetCount = 0;
      SetSammel = 0;
    }
    delay(warten);
  }
  else {
    previousMillis = previousMillis - currentMillis + millis();
  }
}


void LichtAn(int x)  {

  if ( R1 == 0 ) {                // Licht an
    digitalWrite(Relais, HIGH);
    delay(500);
    R1 = 1;
    if ( x == 1 )                 // Licht an für 1h
      RelaisTime = (currentMillis + RelaisTimeOn);
    if ( x == 2 )                 // Licht an für 2h
      RelaisTime = (currentMillis + RelaisTimeOnExt);
  }

  else if ( x == 1) {             // Licht aus
    digitalWrite(Relais, LOW);
    R1 = 0;
    delay(500);
  }

  currentMillis = millis();
  previousMillis = currentMillis;
}


void SOS()  {
  digitalWrite(Relais, LOW);
  while (sos < 100) {
    sos++;
    digitalWrite(StatusLED, HIGH);
    delay(400);
    digitalWrite(StatusLED, LOW);
    delay(200);
    digitalWrite(StatusLED, HIGH);
    delay(400);
    digitalWrite(StatusLED, LOW);
    delay(200);
    digitalWrite(StatusLED, HIGH);
    delay(400);
    digitalWrite(StatusLED, LOW);
    delay(1000);

    digitalWrite(StatusLED, HIGH);
    delay(800);
    digitalWrite(StatusLED, LOW);
    delay(400);
    digitalWrite(StatusLED, HIGH);
    delay(800);
    digitalWrite(StatusLED, LOW);
    delay(400);
    digitalWrite(StatusLED, HIGH);
    delay(800);
    digitalWrite(StatusLED, LOW);
    delay(1000);

    digitalWrite(StatusLED, HIGH);
    delay(400);
    digitalWrite(StatusLED, LOW);
    delay(200);
    digitalWrite(StatusLED, HIGH);
    delay(400);
    digitalWrite(StatusLED, LOW);
    delay(200);
    digitalWrite(StatusLED, HIGH);
    delay(400);
    digitalWrite(StatusLED, LOW);
    delay(4000);
  }
  sos = 0;
}
