#ifdef ENABLE_PS2
/*
   Feb 25, 2019
   Author: Reza Rajan
 */
#include "ps2.h"

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

void processPS2()
{
  if (error == 0 || (error == 3 && !PRESSURES)) {
//    Serial.print("Found Controller, configuration successful");

    ps2.read_gamepad(false, 0);
    int pitch = ps2.Analog(PSS_RY);
    int roll = ps2.Analog(PSS_LX);
    if(roll >= 126 || roll <= 128 &&
           pitch >= 126 || pitch <= 128)
    {
//              sprintf(pitchString, "Pitch: %d", pitch);
//              sprintf(rollString, "Roll: %d", roll);
//              Serial.println(pitchString);
//              Serial.println(rollString);
              float fPitch = map(pitch, 0, 255, MIN_PITCH, MAX_PITCH);
              float fRoll = map(roll, 0, 255, MIN_ROLL, MAX_ROLL);
//              
              stu.moveTo(sp_servo, fPitch, fRoll);

    }
    else {
      //move to center if the joysticks are in the neutral position
      stu.home(sp_servo);
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
