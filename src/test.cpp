#include "Arduino.h"

#ifdef NANODEV
#define MOTOR_PIN 9
#define POT_PIN A4
#endif

#ifdef ALPHAPROTO
#define MOTOR_PIN 10
#define POT_PIN A0
#endif

#define POT_SAMPLES 20
#define POT_SAMPLE_DELAY 5

#define POT_DEADBAND_LOW 30
#define POT_DEADBAND_HIGH (1023 - 13)

#define POT_MIN_DEVIATION 15

#define MOTOR_MIN 40

#define MOVAVG_WEIGHT 95

#define READ_POT_MOVEAVG() (((long)potAvg * MOVAVG_WEIGHT + analogRead(POT_PIN) * (100 - MOVAVG_WEIGHT)) / 100)

int potLast = 0;
int potAvg = 0;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    pinMode(MOTOR_PIN, OUTPUT);
    pinMode(POT_PIN, INPUT_PULLUP);
    for (int k = 0; k < 200; k++)
    {
        READ_POT_MOVEAVG();
        delay(10);
    }
    digitalWrite(LED_BUILTIN, HIGH);
}

int motorFromPot(int potVal)
{
    if (abs(potVal - potLast) < POT_MIN_DEVIATION)
    {
        return -1;
    }
    potLast = potVal;

    if (potVal < POT_DEADBAND_LOW)
    {
        return 0;
    }
    if (potVal > POT_DEADBAND_HIGH)
    {
        return 255;
    }
    return map(potVal, POT_DEADBAND_LOW, POT_DEADBAND_HIGH, MOTOR_MIN, 255);
}

void loop()
{
    potAvg = READ_POT_MOVEAVG();

    int mot = motorFromPot(potAvg);
    if (mot < 0)
        return;

    analogWrite(MOTOR_PIN, mot);
}
