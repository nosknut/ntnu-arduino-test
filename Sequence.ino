class Timer
{
private:
    long startTime;
    long duration;
    long endTime;
    bool complete = true;

public:
    void start(long durationMs)
    {
        startTime = millis();
        endTime = startTime + duration;
        complete = false;
        this->duration = durationMs;
    }
    bool update()
    {
        if (millis() >= endTime)
        {
            complete = true;
        }
        return complete;
    }
    bool isComplete()
    {
        return complete;
    }
    bool countDownFrom(long delayMs)
    {
        if (isComplete())
        {
            start(delayMs);
        }
        return update();
    }
};

class Sequence
{
private:
    int sequenceStep = 0;
    int checkedSteps = 0;
    int numSteps = 0;
    Timer timer;

public:
    template <typename F>
    Sequence &thenCheck(F cb)
    {
        if (checkedSteps == sequenceStep)
        {
            if (cb())
            {
                sequenceStep += 1;
            }
        }
        checkedSteps += 1;
        return *this;
    }

    template <typename F>
    Sequence &then(F cb)
    {
        if (checkedSteps == sequenceStep)
        {
            cb();
            sequenceStep += 1;
        }
        checkedSteps += 1;
        return *this;
    }
    Sequence &delay(long delayMs)
    {
        if (checkedSteps == sequenceStep)
        {
            if (timer.countDownFrom(delayMs))
            {
                sequenceStep += 1;
            }
        }
        checkedSteps += 1;
        return *this;
    }
    void restart()
    {
        checkedSteps = 0;
        sequenceStep = 0;
    }
    Sequence &loop()
    {
        if (checkedSteps == sequenceStep)
        {
            restart();
        }
        return *this;
    }
    // If endSequence returns true the sequence has completed the current execution
    bool endSequence()
    {
        if (numSteps < checkedSteps)
        {
            numSteps = checkedSteps;
        }
        checkedSteps = 0;
        return (numSteps != 0) && (sequenceStep == numSteps);
    }
};

// The actual code starts here

Sequence rampSequence;
Sequence lampSequence;
Sequence fastLampSequence;
int wheels = 8;
int rampMotor = 9;
int rampDeployedSensor = 10;
int lamp = 11;
int rampLamp = 12;

void setup()
{
    Serial.begin(9600);
    pinMode(lamp, OUTPUT);
    pinMode(rampLamp, OUTPUT);
    pinMode(wheels, OUTPUT);
    pinMode(rampMotor, OUTPUT);
    pinMode(rampDeployedSensor, INPUT);
}

void loop()
{
    lampSequence
        .then([&]
              { digitalWrite(lamp, HIGH); })
        .delay(1000)
        .then([&]
              { digitalWrite(lamp, LOW); })
        .delay(1000)
        .loop()
        .endSequence();

    bool rampSequenceComplete = rampSequence
                                    .then([&]
                                          {
                                              Serial.println("Driving ...");
                                              digitalWrite(wheels, HIGH);
                                          })
                                    .delay(2000)
                                    .then([&]
                                          {
                                              Serial.println("Stopping ...");
                                              digitalWrite(wheels, LOW);
                                          })
                                    .delay(2000)
                                    .then([&]
                                          { Serial.println("Deploying ramp (press button to activate ramp sensor)"); })
                                    .thenCheck([&]
                                               {
                                                   // Now a separate sequence will flast a different lamp quickly while the ramp is deploying.
                                                   // The .delay()s in this sequence do not influence the ramp deployment or sensor checks, because they do not halt the program
                                                   fastLampSequence
                                                       .then([&]
                                                             { digitalWrite(rampLamp, HIGH); })
                                                       .delay(200)
                                                       .then([&]
                                                             { digitalWrite(rampLamp, LOW); })
                                                       .delay(200)
                                                       .loop()
                                                       .endSequence();

                                                   digitalWrite(rampMotor, HIGH);
                                                   if (digitalRead(rampDeployedSensor))
                                                   {
                                                       digitalWrite(rampMotor, LOW);
                                                       //Ensure rampLamp is deactivated before continuing to next sequence
                                                       digitalWrite(rampLamp, LOW);
                                                       //Proceeds to next step in the sequence (see .then() below)
                                                       return true;
                                                   }
                                                   //Continues to loop this step until ramp sensor is true/HIGH
                                                   return false;
                                               })
                                    .then([&]
                                          { Serial.println("Ramp deployed!"); })
                                    .delay(4000)
                                    .endSequence();
    if (rampSequenceComplete)
    {
        // When using normal delay, the lapSequence will stop working, as the program is entirely halted
        Serial.println("Ramp Sequence complete! Restarting in 3");
        delay(1000);
        Serial.println("2");
        delay(1000);
        Serial.println("1");
        delay(1000);
        rampSequence.restart();
    }
}

/*
alternative:

int lampSequenceStep = 1;
Timer timer;

void loop()
{
    switch (lampSequenceStep)
    {
    case 1:
        digitalWrite(lamp, HIGH);
        lampSequenceStep++;
        break;
    case 2:
        if (timer.countDownFrom(1000))
        {
            lampSequenceStep += 1;
        }
        break;
    case 3:
        digitalWrite(lamp, LOW);
        lampSequenceStep++;
        break;
    case 4:
        if (timer.countDownFrom(1000))
        {
            lampSequenceStep += 1;
        }
        break;
    case 5:
        digitalWrite(lamp, HIGH);
        lampSequenceStep++;
        break;
    default:
        lampSequenceStep = 1;
        break;
    }
}
*/
