int speed1 = 9;
int motor1a = 6;
int motor1b = 7;

int speed2 = 10;
int motor2a = 3;
int motor2b = 4;

void setup()
{
  pinMode(speed1, OUTPUT);
  pinMode(motor1a, OUTPUT);
  pinMode(motor1b, OUTPUT);
  pinMode(speed2, OUTPUT);
  pinMode(motor2a, OUTPUT);
  pinMode(motor2b, OUTPUT);
}

void loop()
{
  analogWrite(speed1, 200);
  digitalWrite(motor1a, LOW);
  digitalWrite(motor1b, HIGH);
  delay(5000);
  analogWrite(speed1, 200);
  digitalWrite(motor1a, LOW);
  digitalWrite(motor1b, HIGH);
  
  
  analogWrite(speed2, 200);
  digitalWrite(motor2a, LOW);
  digitalWrite(motor2b, HIGH);
  delay(5000);
  analogWrite(speed2, 200);
  digitalWrite(motor2a, LOW);
  digitalWrite(motor2b, HIGH);
}
