class Light
{
public:
  static enum State { INACTIVE = 0,
                      ACTIVATING = 1,
                      ACTIVE = 2,
                      DEACTIVATING = 3,
  };

private:
  int red, yellow, green;
  State state = State::INACTIVE;

public:
  Light(int red, int yellow, int green)
  {
    this->red = red;
    this->yellow = yellow;
    this->green = green;
  }

  void setup()
  {
    pinMode(red, OUTPUT);
    pinMode(yellow, OUTPUT);
    pinMode(green, OUTPUT);
  }
  void update()
  {
    updateLightState();
  }

private:
  void setLightOutputs(bool r, bool y, bool g)
  {
    digitalWrite(red, r);
    digitalWrite(yellow, y);
    digitalWrite(green, g);
  }
  void updateLightState()
  {
    switch (state)
    {
    case State::INACTIVE:
      setLightOutputs(LOW, LOW, LOW);
      return;
    case State::ACTIVATING:
      setLightOutputs(HIGH, HIGH, LOW);
      return;
    case State::ACTIVE:
      setLightOutputs(LOW, LOW, HIGH);
      return;
    case State::DEACTIVATING:
      setLightOutputs(LOW, HIGH, LOW);
      return;
    default:
      return;
    }
  }
};

Light a(1, 2, 3);
Light b(4, 5, 6);
Light c(7, 8, 9);
Light d(10, 11, 12);

void setup()
{
  a.setup();
  b.setup();
  c.setup();
  d.setup();
}

void loop()
{
  a.update();
  b.update();
  c.update();
  d.update();
}