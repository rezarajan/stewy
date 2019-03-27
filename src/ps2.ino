#ifdef ENABLE_PS2
/*
   Feb 25, 2019
   Author: Reza Rajan
 */
#include "ps2.h"
#include "PID.h"

static int error = 0;
static int type = 0;
#include <PS2X_lib.h> // from https://github.com/madsci1016/Arduino-PS2X
#include "config.h"

/**************************************************
 *  PS2 controller pin definitions
 *************************************************/
#define PS2_SEL        27  //16
#define PS2_CMD        25  //15
#define PS2_CLK        23  //17
#define PS2_DAT        29  //14    

#define RUMBLE false
#define PRESSURES false

PS2X ps2; // ps2 controller class instance
// sp_servo is defined in src.ino and passed here // float sp_servo[6]
// variables to print pitch and roll values
// Serial monitor messes up without them
char pitchString[20];
char rollString[20];

void setupPS2()
{
    Serial.println("PS2 support is ENABLED.");
    delay(500);
   // initializing PS2 controller
    error = ps2.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, RUMBLE);
  
    type = ps2.readType();
    switch(type) {
      case 0: {
        Serial.println("Unknown Controller type found");
        delay(500);
        break;
        }
      case 1: {
        Serial.println("DualShock Controller found");
        delay(500);
        break;
        }
      case 2: {
        Serial.println("GuitarHero Controller found");
        delay(500);
        break;
        }
    case 3:{
        Serial.println("Wireless Sony DualShock Controller found");
        delay(500);
        break;
      }
    }
}

void processPS2(MPU9250& imu)
{
    if (error == 0 || (error == 3 && !PRESSURES)) {
//    Serial.print("Found Controller, configuration successful");
      static float fPitch = 0.0;
      static float fRoll = 0.0;
    ps2.read_gamepad(false, 0);
    int pitch = ps2.Analog(PSS_RY);
    int roll = ps2.Analog(PSS_RX);

    if(roll <= 125 || roll >= 129 ||
       pitch <= 126 || pitch >= 129)
    {
 #if defined(POSITION_CONTROL)

              fPitch = mapfloat(pitch, 0, 255, MIN_PITCH, MAX_PITCH);
              fRoll = mapfloat(roll, 0, 255, MIN_ROLL, MAX_ROLL);
//              sprintf(pitchString, "Pitch: %d", (int)fPitch);
//              sprintf(rollString, "Roll: %d", (int)fRoll);
//              Serial.println(pitchString);
//              Serial.println(rollString);
#elif defined(VELOCITY_CONTROL)
              float fPitchRate = mapfloat(pitch, 0, 255, MIN_PITCH_RATE, MAX_PITCH_RATE);
              float fRollRate = mapfloat(roll, 0, 255, MIN_ROLL_RATE, MAX_ROLL_RATE);
              fPitch += fPitchRate;
              fRoll += fRollRate;
              fPitch = constrain(fPitch, MIN_PITCH, MAX_PITCH);
              fRoll = constrain(fRoll, MIN_ROLL, MAX_ROLL);
//              Serial.println(fRoll);
#endif
      stu.moveTo(sp_servo, fPitch, fRoll);
    }
    else {

      imu.update();
      float sensedRoll = imu.getRoll();
      float sensedPitch = imu.getPitch();
      Serial.println(sensedRoll);
#if defined(POSITION_CONTROL)
      //move to center if the joysticks are in the neutral position
      stu.home(sp_servo);
#elif defined(VELOCITY_CONTROL)
#ifdef USE_PID_AUTOLEVEL
      imu.update();
      float sensedRoll = imu.getRoll();
      float sensedPitch = imu.getPitch();
      if(abs(sensedRoll) > 0.1 && abs(sensedPitch) > 0.1)
      {
        calculatePIDOutput(fRoll, fPitch, 0.0, 0.0, sensedRoll, sensedPitch);
        stu.moveTo(sp_servo, fPitch, fRoll);
      }
#else
    if(fPitch < 0)
      {
        fPitch += MAX_PITCH_RATE;
        fPitch = min(fPitch, 0);
      }
      else
      {
        fPitch -= MAX_PITCH_RATE;
        fPitch = max(fPitch, 0);
      }
      if(fRoll < 0)
      {
        fRoll += MAX_ROLL_RATE;
        fRoll = min(fRoll, 0);
      }
      else{
        fRoll -= MAX_ROLL_RATE; 
        fRoll = max(fRoll, 0);
      }
      stu.moveTo(sp_servo, fPitch, fRoll);
#endif
#endif
    }

    // switch (mode) {

    //   case CONTROL:
    //     if(int(ps2.Analog(PSS_LX)) >= 126 || int(ps2.Analog(PSS_LX)) <= 128 &&
    //        int(ps2.Analog(PSS_RY)) >= 126 || int(ps2.Analog(PSS_RY)) <= 128) {
    //       switch (controlSubMode) {
    //         case PITCH_ROLL: {
    //           double pitch = map(ps2.Analog(PSS_LX), 0, 255, MIN_PITCH, MAX_PITCH)
    //           double roll = map(ps2.Analog(PSS_RY), 0, 255, MIN_ROLL, MAX_ROLL)

    //           // Serial.print("PITCH_ROLL moving to %.2f , %.2f",pitch,roll);
    //           stu.moveTo(sp_servo, pitch, roll);

    //           break;
    //         }
    //         // TODO: add functionality for forward/back and side/side motion
    //         // case SWAY_SURGE: {
    //         //   double surge = map(ps2.getJoyY(), -100, 100, MIN_SURGE, MAX_SURGE);
    //         //   double sway = map(ps2.getJoyX(), -100, 100, MIN_SWAY, MAX_SWAY);

    //         //   Serial.print("SWAY_SURGE moving to %.2f , %.2f",sway,surge);
    //         //   stu.moveTo(sp_servo, sway, surge, 0, 0, 0, 0);

    //         //   break;
    //         // }
    //         // case HEAVE_YAW: {
    //         //   double heave = map(ps2.getJoyY(), -100, 100, MIN_HEAVE, MAX_HEAVE);
    //         //   double yaw = map(ps2.getJoyX(), -100, 100, MIN_YAW, MAX_YAW);

    //         //   Serial.print("HEAVE_YAW moving to %.2f , %.2f",heave,yaw);
    //         //   stu.moveTo(sp_servo, 0, 0, heave, 0, 0, yaw);

    //         // }
    //         // break;
    //       }
    //     } else {
    //       //move to center if the joysticks are in the neutral position
    //       // Serial.print("Controller in deadband. (%d, %d) vs. (%d, %d)",ps2.getJoyX(), ps2.getJoyY(),deadBand.x,deadBand.y);
    //       stu.home(sp_servo);
    //     }
    //     break;
    // }
  }
  else if(error == 1)
    Serial.println("No controller found, check wiring");
  else if(error == 2)
    Serial.println("Controller found but not accepting commands.");
  else if(error == 3 && PRESSURES)
    Serial.println("Controller refusing to enter Pressures mode, may not support it.");

  // Wait a short while
    delay(50);
}
#else
    Serial.println("PS2 support is DISABLED.");
#endif

float mapfloat(long x, long in_min, long in_max, long out_min, long out_max)
{
 return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}
