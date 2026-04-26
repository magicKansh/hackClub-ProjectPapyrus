int echoPin = 7;
int trigPin = 6;
int ledPin = 8;
float duration = 0.0; 
float distance = 0.0;

void setup()
{
 pinMode(echoPin, INPUT);
 pinMode(trigPin, OUTPUT);
 pinMode(ledPin, OUTPUT);

}

void loop()
{
    digitalWrite(trigPin, LOW); 
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
  
    duration = pulseIn(echoPin, HIGH);
    distance = (duration / 2) * 0.01354331;
  if (distance > 25){
    digitalWrite(ledPin, HIGH);
  } else{
    digitalWrite(ledPin, LOW);
  }

}
