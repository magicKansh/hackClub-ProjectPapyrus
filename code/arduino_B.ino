// ============================================================
//  ARDUINO B — Motor Controller (3× NEMA 17 via TMC2209)
//  Receives signals from Arduino A and drives motors
// ============================================================
//
//  WIRING — Signals from Arduino A (4 wires + GND):
//    B pin 1   ←  A pin 2    (Motor A signal)   ⚠ pin 1 = TX; do NOT use Serial
//    B pin 10  ←  A pin 3    (Motor B signal)
//    B pin 5   ←  A pin 4    (Motor C signal)
//    B pin 9   ←  A pin 9    (Ultrasonic trigger)
//    B GND     ←  A GND
//
//  WIRING — TMC2209 stepper drivers:
//    Motor A:  STEP → 3,  DIR → 2,  EN → 4
//    Motor B:  STEP → 6,  DIR → 7,  EN → 8
//    Motor C:  STEP → 11, DIR → 12, EN → 13
//    VIO → 5V (logic),  VMOT → 12 V external supply,  GND → GND
//
//  BEHAVIOUR:
//    • Joystick left/right  → Motor A moves (one direction while held)
//    • Joystick up/down     → Motor B moves (one direction while held)
//    • Joystick button      → Motor C moves (while held)
//    • Only ONE motor moves at a time (joystick takes priority)
//    • Ultrasonic trigger   → all 3 motors run sequentially (A then B then C)
//      (ultrasonic sequence only starts if no joystick input is active)
// ============================================================

// --- Signal inputs from Arduino A ---
const int sigA = 1;    // Motor A  ⚠ TX pin — do NOT call Serial.begin()
const int sigB = 10;   // Motor B
const int sigC = 5;    // Motor C
const int sigU = 9;    // Ultrasonic trigger

// --- Motor A (TMC2209) ---
const int stepA = 3,  dirA = 2,  enA = 4;

// --- Motor B (TMC2209) ---
const int stepB = 6,  dirB = 7,  enB = 8;

// --- Motor C (TMC2209) ---
const int stepC = 11, dirC = 12, enC = 13;

// Speed tuning:
//   STEP_DELAY — microseconds per half-pulse. Lower = faster.
//   100 µs → ~5000 steps/sec (max reliable without acceleration for NEMA 17).
//   If your motor stalls or skips, raise this value (try 150, 200…).
//   JOYSTICK_STEPS — steps fired per signal poll. Higher = more movement per tick.
const int STEP_DELAY     = 100;   // µs per half-pulse  (was 400)
const int JOYSTICK_STEPS = 40;    // steps per joystick poll  (was 10)

// --------------------------------------------------------
// Drive 'steps' microsteps on a single motor at 'delayMicros' per half-pulse.
// Lower delayMicros = faster rotation.
void stepMotor(int stepPin, int steps, int delayMicros) {
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delayMicros);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delayMicros);
  }
}

// --------------------------------------------------------
void setup() {
  // Signal pins — Arduino A actively drives these HIGH or LOW,
  // so plain INPUT is correct here (no pull-ups needed).
  pinMode(sigA, INPUT);
  pinMode(sigB, INPUT);
  pinMode(sigC, INPUT);
  pinMode(sigU, INPUT);

  // Motor output pins
  pinMode(stepA, OUTPUT); pinMode(dirA, OUTPUT); pinMode(enA, OUTPUT);
  pinMode(stepB, OUTPUT); pinMode(dirB, OUTPUT); pinMode(enB, OUTPUT);
  pinMode(stepC, OUTPUT); pinMode(dirC, OUTPUT); pinMode(enC, OUTPUT);

  // EN is active LOW on TMC2209 — enable all drivers at startup
  digitalWrite(enA, LOW);
  digitalWrite(enB, LOW);
  digitalWrite(enC, LOW);
}

// --------------------------------------------------------
void loop() {

  bool A = (digitalRead(sigA) == HIGH);   // joystick X axis active
  bool B = (digitalRead(sigB) == HIGH);   // joystick Y axis active
  bool C = (digitalRead(sigC) == HIGH);   // joystick button pressed
  bool U = (digitalRead(sigU) == HIGH);   // ultrasonic object detected

  // === JOYSTICK PRIORITY (one motor at a time) ===
  // Motor A — joystick moved left or right
  if (A) {
    digitalWrite(dirA, HIGH);
    stepMotor(stepA, JOYSTICK_STEPS, STEP_DELAY);
    return;   // skip everything else this loop iteration
  }

  // Motor B — joystick moved up or down
  if (B) {
    digitalWrite(dirB, HIGH);
    stepMotor(stepB, JOYSTICK_STEPS, STEP_DELAY);
    return;
  }

  // Motor C — joystick button held
  if (C) {
    digitalWrite(dirC, HIGH);
    stepMotor(stepC, JOYSTICK_STEPS, STEP_DELAY);
    return;
  }

  // === ULTRASONIC SEQUENTIAL SEQUENCE ===
  // Only runs when no joystick input is active (return statements above).
  // Each motor rotates forward 1000 steps, then reverses 1000 steps.
  if (U) {

    // --- Motor A ---
    digitalWrite(dirA, HIGH);
    stepMotor(stepA, 1000, STEP_DELAY);
    digitalWrite(dirA, LOW);
    stepMotor(stepA, 1000, STEP_DELAY);

    // --- Motor B ---
    digitalWrite(dirB, HIGH);
    stepMotor(stepB, 1000, STEP_DELAY);
    digitalWrite(dirB, LOW);
    stepMotor(stepB, 1000, STEP_DELAY);

    // --- Motor C ---
    digitalWrite(dirC, HIGH);
    stepMotor(stepC, 1000, STEP_DELAY);
    digitalWrite(dirC, LOW);
    stepMotor(stepC, 1000, STEP_DELAY);
  }
}
