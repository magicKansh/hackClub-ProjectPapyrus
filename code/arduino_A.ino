// ============================================================
//  ARDUINO A — Sensor & Joystick Controller
// ============================================================
//
//  WIRING — Arduino A:
//    Ultrasonic HC-SR04:  Trig -> pin 10,  Echo -> pin 5
//    Joystick (3.3V):     VRX  -> A0,      VRY  -> A1,  SW -> pin 8
//
//  WIRING — Arduino A -> Arduino B (6 signal wires + GND):
//    A pin 2  -> B pin A0   (Motor A move flag)
//    A pin 6  -> B pin A1   (Motor A direction)   [NEW]
//    A pin 3  -> B pin 10   (Motor B move flag)
//    A pin 7  -> B pin A2   (Motor B direction)   [NEW]
//    A pin 4  -> B pin 5    (Motor C / button)
//    A pin 9  -> B pin 9    (Ultrasonic trigger)
//    A GND    -> B GND
//
//  BEHAVIOUR:
//    Joystick UP    -> Motor A clockwise
//    Joystick DOWN  -> Motor A counterclockwise
//    Joystick LEFT  -> Motor B clockwise
//    Joystick RIGHT -> Motor B counterclockwise
// ============================================================

const int trigPin = 10;
const int echoPin = 5;

const int vrxPin = A0;
const int vryPin = A1;
const int swPin  = 8;

const int sigA     = 2;  // Motor A move flag
const int sigA_dir = 6;  // Motor A direction: HIGH = UP (CW), LOW = DOWN (CCW)
const int sigB     = 3;  // Motor B move flag
const int sigB_dir = 7;  // Motor B direction: HIGH = LEFT (CW), LOW = RIGHT (CCW)
const int sigC     = 4;  // Motor C (button)
const int sigU     = 9;  // Ultrasonic trigger

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
  pinMode(sigA_dir, OUTPUT);
  pinMode(sigB, OUTPUT);
  pinMode(sigB_dir, OUTPUT);
  pinMode(sigC, OUTPUT);
  pinMode(sigU, OUTPUT);

  digitalWrite(sigA, LOW);
  digitalWrite(sigA_dir, LOW);
  digitalWrite(sigB, LOW);
  digitalWrite(sigB_dir, LOW);
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

  bool up    = (yVal < JOY_LOW);
  bool down  = (yVal > JOY_HIGH);
  bool left  = (xVal < JOY_LOW);
  bool right = (xVal > JOY_HIGH);

  digitalWrite(sigA, (up || down) ? HIGH : LOW);
  digitalWrite(sigA_dir, up ? HIGH : LOW);

  digitalWrite(sigB, (left || right) ? HIGH : LOW);
  digitalWrite(sigB_dir, left ? HIGH : LOW);

  digitalWrite(sigC, (swVal == LOW) ? HIGH : LOW);

  delay(20);
}
