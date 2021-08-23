// C++ code
//
enum Pin {
  RED = 2,
  YELLOW = 3,
  GREEN = 4,
};

int delayMs = 100;

void togglePin(int pin, int delayMs){
  digitalWrite(pin, HIGH);
  delay(delayMs);
  digitalWrite(pin, LOW);
  delay(delayMs);
}

void setup()
{
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
}

void loop()
{
  togglePin(RED, delayMs);
  togglePin(YELLOW, delayMs);
  togglePin(GREEN, delayMs);
}