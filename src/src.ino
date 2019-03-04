/*
   stewy
   Copyright (C) 2018  Philippe Desrosiers

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//=== Includes
// #include <Servo.h>
#include <Adafruit_PWMServoDriver.h>
#include "config.h"
#include "Platform.h"

#ifdef ENABLE_PS2
#include "ps2.h"
#endif

// this is the magic trick for printf to support float
// asm (".global _printf_float");
// this is the magic trick for scanf to support float
// asm (".global _scanf_float");
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

//=== Actual code

xy_coordf setpoint = DEFAULT_SETPOINT;

Platform stu;            // Stewart platform object.

// #ifdef ENABLE_SERVOS
//Servo servos[6];        // servo objects.
// #endif

float sp_servo[6];      // servo setpoints in degrees, between SERVO_MIN_ANGLE and SERVO_MAX_ANGLE.

float _toUs(int value)
{
    return map(value, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE, SERVO_MIN_US, SERVO_MAX_US);
}

float _toAda(int value)
{
    return map(value, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE, 195, 465);
}

float _toAngle(float value)
{
    return map(value, SERVO_MIN_US, SERVO_MAX_US, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
}

//Set servo values to the angles represented by the setpoints in sp_servo[].
//DOES: Apply trim values.
//DOES: Automatically reverse signal for reversed servos.
//DOES: Write signals to the physical servos.
void updateServos()
{
    static float sValues[6];

    for (int i = 0; i < 6; i++)
    {
        //sp_servo holds a value between SERVO_MIN_ANGLE and SERVO_MAX_ANGLE.
        //apply reverse.
        float val = sp_servo[i];
        if (SERVO_REVERSE[i])
        {
            val = SERVO_MIN_ANGLE + (SERVO_MAX_ANGLE - val);
        }

        //translate angle to pulse width
       // val = _toUs(val);
        val = _toAda(val);
        if (val != sValues[i])
        {
            //don't write to the servo if you don't have to.
            sValues[i] = val;
            // Serial.print("Servos Updated");
            // Serial.print("SRV: s%d = %.2f + %d (value + trim)", i, val, SERVO_TRIM[i]);

#ifdef ENABLE_SERVOS
            // set servo of servo at pin i+1, i.e. pins 1 to 6
            pwm.setPWM(i+1, 0, val);
           //servos[i].writeMicroseconds((int)constrain(val + SERVO_TRIM[i], SERVO_MIN_US, SERVO_MAX_US));
#endif

        }
    }
}

// Calculates and assigns values to sp_servo.
// DOES: Ignore out-of-range values. These will generate a warning on the serial monitor.
// DOES NOT: Apply servo trim values.
// DOES NOT: Automatically reverse signal for reversed servos.
// DOES NOT: digitally write a signal to any servo. Writing is done in updateServos();
void setServo(int i, int angle)
{
    int val = angle;
    if (val >= SERVO_MIN_ANGLE && val <= SERVO_MAX_ANGLE)
    {
        sp_servo[i] = val;
//        Serial.println("setServo called for valid");
    }
    else
    {
        Serial.println("setServo: Invalid value");
    }
}

void setServoMicros(int i, int micros)
{
    int val = micros;
    if (val >= SERVO_MIN_US && val <= SERVO_MAX_US)
    {
        sp_servo[i] = _toAngle(val);
        // Serial.print("setServoMicros %d - %.2f us", i, val);
    }
    else
    {
        // Serial.print("setServoMicros: Invalid value '%.2f' specified for servo #%d. Valid range is %d to %d us.", val, i, SERVO_MIN_US, SERVO_MAX_US);
    }
}

void setupPlatform()
{
    stu.home(sp_servo);     // set platform to "home" position (flat, centered).
    updateServos();
    delay(300);
}

//Initialize servo interface, sweep all six servos from MIN to MAX, to MID, to ensure they're all physically working.
void setupServos()
{
#ifdef ENABLE_SERVOS
    int d=500;
#else
    int d=0;
#endif

    for (int i = 0; i < 6; i++)
    {
#ifdef ENABLE_SERVOS
        // servos[i].attach(SERVO_PINS[i]);
        pwm.begin();
        pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
#endif
        setServo(i, SERVO_MIN_ANGLE);
    }
    updateServos();
    delay(d);

    for (int pos=SERVO_MIN_ANGLE; pos<SERVO_MID_ANGLE; pos+=4)
    {
        for (int i = 0; i < 6; i++)
        {
            setServo(i,pos);
        }
        updateServos();
        delay(10);
    }
    Serial.println("Setup Servos complete");
}

void setup()
{   //config.h

    Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
    //Serial.setTimeout(10); // change default (1000ms) to have faster response time
    pinMode(LED_BUILTIN, OUTPUT);     //power indicator
    digitalWrite(LED_BUILTIN, HIGH);

    setupPS2();
    

    setupPlatform();

    setupServos();     //Servos come last, because this setup takes the most time...
}

void loop()
{
    processPS2();

//delay(100);
    updateServos();     //Servos come last, because they take the most time.
//    static char buff[30];
//    for(int i = 0; i < 6; i++){
//        sprintf(buff, "Servo Value %d: %d ", i, (int)sp_servo[i]);
//        Serial.print(buff);
//    }
//    Serial.println();
}
