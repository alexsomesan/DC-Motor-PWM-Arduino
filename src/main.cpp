#include "Arduino.h"

#ifdef NANODEV
#define MOTOR_PIN 9
#define POT_PIN A4
#endif

#ifdef ALPHAPROTO
#define MOTOR_PIN 10
#define POT_PIN A0
#endif

#define POT_SAMPLES 100
#define POT_SAMPLE_DELAY 10

#define POT_DEADBAND_LOW 50
#define POT_DEADBAND_HIGH (1023 - 23)

#define POT_MIN_DEVIATION 5

#define MOTOR_MIN 100
#define MOVAVG_WEIGHT 95
#define MOT_PRIME_SECS 5
#define MOT_RAMP_STEP 15

#define READ_POT_MOVEAVG() (potAvg = (((potAvg * MOVAVG_WEIGHT) + ((long)analogRead(POT_PIN) * (100 - MOVAVG_WEIGHT))) / 100))

long potLast = 0;
long potAvg = 0;
int motPWM = 0;

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

void setMotorPWM(int pwmVal) {
    if (pwmVal == motPWM || pwmVal < 0) {
        return;
    }

    if (motPWM > 0) {
        motPWM = pwmVal;
        analogWrite(MOTOR_PIN, motPWM);
        return;
    }

    motPWM = pwmVal;

    // starting from standstill
    // go temporarily full-speed to prime pump
    for(int j = MOTOR_MIN; j <= 255; j++) { // ramp motor up gradually
        analogWrite(MOTOR_PIN, j);
        delay(MOT_RAMP_STEP);
    }
    
    delay(1000 * MOT_PRIME_SECS); // prime for MOT_PRIME_SECS seconds

    for(int k = 254; k >= motPWM; k--) { // ramp motor down gradually
        analogWrite(MOTOR_PIN, k);
        delay(MOT_RAMP_STEP);
    }
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    pinMode(MOTOR_PIN, OUTPUT);
    pinMode(POT_PIN, INPUT_PULLUP);
    for (int k = 0; k < POT_SAMPLES; k++)
    {
        READ_POT_MOVEAVG();
        delay(POT_SAMPLE_DELAY);
    }
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
    long pot = READ_POT_MOVEAVG();
    int mot = motorFromPot(pot);
    setMotorPWM(mot);
}
