// ============================================================
//  ARDUINO B — Motor Controller (3x NEMA 17 via TMC2209)
// ============================================================
//
//  WIRING — Signals from Arduino A (6 signal wires + GND):
//    B pin A0  <- A pin 2   (Motor A move flag)
//    B pin A1  <- A pin 6   (Motor A direction)
//    B pin 10  <- A pin 3   (Motor B move flag)
//    B pin A2  <- A pin 7   (Motor B direction)
//    B pin 5   <- A pin 4   (Motor C / button)
//    B pin 9   <- A pin 9   (Ultrasonic trigger)
//    B GND     <- A GND
//
//  Motor A driver:  STEP -> 11, DIR -> 12, EN -> 13
//  Motor B driver:  STEP -> 6,  DIR -> 7,  EN -> 8
//  Motor C driver:  STEP -> 3,  DIR -> 2,  EN -> 4
// ============================================================

const int sigA     = A0;
const int sigA_dir = A1;
const int sigB     = 10;
const int sigB_dir = A2;
const int sigC     = 5;
const int sigU     = 9;

const int stepA = 11, dirA = 12, enA = 13;
const int stepB = 6,  dirB = 7,  enB = 8;
const int stepC = 3,  dirC = 2,  enC = 4;

// --- Joystick tap speed per motor (lower = faster) ---
const int STEP_DELAY_A = 10;  // Motor A unchanged
const int STEP_DELAY_B = 10;   // Motor B faster
const int STEP_DELAY_C = 10;   // Motor C faster
const int JOYSTICK_STEPS = 3;

// --- Ramp settings for ultrasonic-triggered moves ---
const int START_DELAY = 3000;    // shared ramp start (slow, high torque)
const int MIN_DELAY_A =  10;     // Motor A cruise speed unchanged
const int MIN_DELAY_B = 10;     // Motor B faster cruise speed
const int MIN_DELAY_C = 10;     // Motor C faster cruise speed
const int RAMP_STEPS  = 100;

void stepMotor(int stepPin, int steps, int delayMicros) {
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delayMicros);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delayMicros);
  }
}

void stepMotorRamped(int stepPin, int totalSteps, int minDelay) {
  for (int i = 0; i < totalSteps; i++) {
    int delayMicros;
    if (i < RAMP_STEPS) {
      delayMicros = START_DELAY - ((START_DELAY - minDelay) * i / RAMP_STEPS);
    } else if (i >= totalSteps - RAMP_STEPS) {
      int stepsFromEnd = totalSteps - i;
      delayMicros = START_DELAY - ((START_DELAY - minDelay) * stepsFromEnd / RAMP_STEPS);
    } else {
      delayMicros = minDelay;
    }
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delayMicros);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delayMicros);
  }
}

const int SAMPLES = 10;
const int SAMPLE_GAP_US = 50;
const int THRESHOLD = 7;

bool readStable(int pin) {
  int count = 0;
  for (int i = 0; i < SAMPLES; i++) {
    if (digitalRead(pin) == HIGH) count++;
    delayMicroseconds(SAMPLE_GAP_US);
  }
  return count >= THRESHOLD;
}

void setup() {
  pinMode(sigA, INPUT);
  pinMode(sigA_dir, INPUT);
  pinMode(sigB, INPUT);
  pinMode(sigB_dir, INPUT);
  pinMode(sigC, INPUT);
  pinMode(sigU, INPUT);

  pinMode(stepA, OUTPUT); pinMode(dirA, OUTPUT); pinMode(enA, OUTPUT);
  pinMode(stepB, OUTPUT); pinMode(dirB, OUTPUT); pinMode(enB, OUTPUT);
  pinMode(stepC, OUTPUT); pinMode(dirC, OUTPUT); pinMode(enC, OUTPUT);

  digitalWrite(enA, LOW);
  digitalWrite(enB, LOW);
  digitalWrite(enC, LOW);
}

void loop() {
  bool A = readStable(sigA);
  bool B = readStable(sigB);
  bool C = readStable(sigC);
  bool U = readStable(sigU);

  if (A) {
  bool dirHigh = readStable(sigA_dir);
  digitalWrite(dirA, dirHigh ? HIGH : LOW);
  stepMotor(stepA, JOYSTICK_STEPS, STEP_DELAY_A);
  return;
  }

  if (B) {
    bool dirHigh = readStable(sigB_dir);
    digitalWrite(dirB, dirHigh ? HIGH : LOW);
    stepMotor(stepB, JOYSTICK_STEPS, STEP_DELAY_B);
    return;
  }

  if (C) {
    digitalWrite(dirC, HIGH);
    stepMotor(stepC, JOYSTICK_STEPS, STEP_DELAY_C);
    return;
  }

  if (U) {
    digitalWrite(dirA, HIGH);
    stepMotorRamped(stepA, 1000, MIN_DELAY_A);
    digitalWrite(dirA, LOW);
    stepMotorRamped(stepA, 1000, MIN_DELAY_A);

    digitalWrite(dirB, HIGH);
    stepMotorRamped(stepB, 1000, MIN_DELAY_B);
    digitalWrite(dirB, LOW);
    stepMotorRamped(stepB, 1000, MIN_DELAY_B);

    digitalWrite(dirC, HIGH);
    stepMotorRamped(stepC, 1000, MIN_DELAY_C);
    digitalWrite(dirC, LOW);
    stepMotorRamped(stepC, 1000, MIN_DELAY_C);
  }
}
