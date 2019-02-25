#ifdef ENABLE_PS2
/*
   6dof-stewduino
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
#include "ps2.h"

void processPS2()
{

  // initializing PS2 controller
  error = ps2.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);

  type = ps2.readType();
  switch(type) {
    case 0:
      Serial.print("Unknown Controller type found");
      break;
    case 1:
      Serial.print("DualShock Controller found");
      break;
    case 2:
      Serial.print("GuitarHero Controller found");
      break;
  case 3:
      Serial.print("Wireless Sony DualShock Controller found");
      break;
  }

  if (!error) {

  if(!error)
    Serial.print("Found Controller, configuration successful");
  else if(error == 1)
    Serial.print("No controller found, check wiring");
  else if(error == 2)
    Serial.print("Controller found but not accepting commands.");
  else if(error == 3)
    Serial.print("Controller refusing to enter Pressures mode, may not support it.");

    switch (mode) {

      case CONTROL:
        if(int(ps2.Analog(PSS_LX)) >= 126 || int(ps2.Analog(PSS_LX)) <= 128 &&
           int(ps2.Analog(PSS_RY)) >= 126 || int(ps2.Analog(PSS_RY)) <= 128) {
          switch (controlSubMode) {
            case PITCH_ROLL: {
              double pitch = map(ps2.Analog(PSS_LX), 0, 255, MIN_PITCH, MAX_PITCH)
              double roll = map(ps2.Analog(PSS_RY), 0, 255, MIN_ROLL, MAX_ROLL)

              Serial.print("PITCH_ROLL moving to %.2f , %.2f",pitch,roll);
              stu.moveTo(sp_servo, pitch, roll);

              break;
            }
            // TODO: add functionality for forward/back and side/side motion
            // case SWAY_SURGE: {
            //   double surge = map(ps2.getJoyY(), -100, 100, MIN_SURGE, MAX_SURGE);
            //   double sway = map(ps2.getJoyX(), -100, 100, MIN_SWAY, MAX_SWAY);

            //   Serial.print("SWAY_SURGE moving to %.2f , %.2f",sway,surge);
            //   stu.moveTo(sp_servo, sway, surge, 0, 0, 0, 0);

            //   break;
            // }
            // case HEAVE_YAW: {
            //   double heave = map(ps2.getJoyY(), -100, 100, MIN_HEAVE, MAX_HEAVE);
            //   double yaw = map(ps2.getJoyX(), -100, 100, MIN_YAW, MAX_YAW);

            //   Serial.print("HEAVE_YAW moving to %.2f , %.2f",heave,yaw);
            //   stu.moveTo(sp_servo, 0, 0, heave, 0, 0, yaw);

            // }
            // break;
          }
        } else {
          //move to center if the joysticks are in the neutral position
          // Serial.print("Controller in deadband. (%d, %d) vs. (%d, %d)",ps2.getJoyX(), ps2.getJoyY(),deadBand.x,deadBand.y);
          stu.home(sp_servo);
        }
        break;
    }

  } else {
    mode = SETPOINT; //Controller is on the fritz / disconnected. Default back to the center setpoint.
    dir = CW;
    Serial.print("No PS2 Controller.");
  }

  // Wait a short while
  //  delay(50);
}


void setMode(Mode newMode){

  if(mode !=newMode){

#ifdef ENABLE_TOUCHSCREEN
    Mode oldMode = mode;
    if((oldMode == CONTROL) != (newMode == CONTROL)) {
      // Turn off the PIDs if we're moving to CONTROL mode.
      // Turn them on if we're moving out of CONTROL mode.
      int onOff = newMode == CONTROL ? MANUAL : AUTOMATIC;
      Serial.print("setting PID mode to %s",onOff ? "AUTOMATIC" : "MANUAL");
      rollPID.SetMode(onOff);
      pitchPID.SetMode(onOff);
    }
#endif

    mode = newMode;

    // Serial.print("Mode = %s",modeStrings[mode]);
    // blinker.blink(int(mode)+1);

    //initialize the mode
    sp_speed = DEFAULT_SPEED;           //reset to default speed.
    controlSubMode = DEFAULT_SUB_MODE;  //reset to default subMode.
    stu.home(sp_servo);                 //reset the platform.

    //TODO: Make sure that the radius never goes outside the actual plate.
    sp_radius = sqrt(pow(setpoint.x - DEFAULT_SETPOINT.x,2) + pow(setpoint.y - DEFAULT_SETPOINT.y,2));  //radius is based on wherever the setpoint is right now.
  }
}

// TODO: include an action for resetting the platform
// void onCButtonDblClick(){
//   Serial.print("CButtonDblClick");
//   setMode(DEFAULT_MODE);
// }

#endif
