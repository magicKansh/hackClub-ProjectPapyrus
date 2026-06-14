// ============================================================
//  ARDUINO A — Sensor & Joystick Controller
//  Reads ultrasonic + joystick, sends digital signals to Arduino B
// ============================================================
//
//  WIRING — Arduino A:
//    Ultrasonic HC-SR04:  Trig → pin 10,  Echo → pin 5
//                         VCC  → 5V,      GND  → GND
//    Joystick (3.3 V):    VRX  → A0,      VRY  → A1
//                         SW   → pin 8,   VCC  → 3.3V
//                         GND  → GND
//
//  WIRING — Arduino A → Arduino B (4 wires + GND):
//    A pin 2  →  B pin 1    (Motor A signal)
//    A pin 3  →  B pin 10   (Motor B signal)
//    A pin 4  →  B pin 5    (Motor C signal)
//    A pin 9  →  B pin 9    (Ultrasonic trigger)
//    A GND    →  B GND
// ============================================================

const int trigPin = 10;
const int echoPin = 5;

const int vrxPin = A0;
const int vryPin = A1;
const int swPin  = 8;

const int sigA = 2;
const int sigB = 3;
const int sigC = 4;
const int sigU = 9;

const int JOY_LOW  = 250;
const int JOY_HIGH = 450;

long readUltrasonic() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(swPin, INPUT_PULLUP);
  pinMode(sigA, OUTPUT);
  pinMode(sigB, OUTPUT);
  pinMode(sigC, OUTPUT);
  pinMode(sigU, OUTPUT);
  digitalWrite(sigA, LOW);
  digitalWrite(sigB, LOW);
  digitalWrite(sigC, LOW);
  digitalWrite(sigU, LOW);
}

void loop() {
  long dist = readUltrasonic();
  if (dist > 0 && dist < 20) {
    digitalWrite(sigU, HIGH);
    delay(100);
    digitalWrite(sigU, LOW);
  }

  int xVal  = analogRead(vrxPin);
  int yVal  = analogRead(vryPin);
  int swVal = digitalRead(swPin);

  bool moveX = (xVal < JOY_LOW || xVal > JOY_HIGH);
  bool moveY = (yVal < JOY_LOW || yVal > JOY_HIGH);

  digitalWrite(sigA, moveX ? HIGH : LOW);
  digitalWrite(sigB, moveY ? HIGH : LOW);
  digitalWrite(sigC, (swVal == LOW) ? HIGH : LOW);

  delay(20);
}
