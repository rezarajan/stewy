#ifdef ENABLE_NUNCHUCK
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

void processNunchuck()
{
  // Read the current state
  nc.read();

  Logger::trace("Buttons (C/Z): %s/%s\tJoystick (X/Y): %.2f/%.2f\tTilt (X/Y/X): %.2f/%.2f/%.2f\tAccel (X/Y/X): %.2f/%.2f/%.2f",
              (nc.getButtonC() ? "true" : "false"),
              (nc.getButtonZ() ? "true" : "false"),
              nc.getJoyX(),
              nc.getJoyY(),
              nc.getTiltX(),
              nc.getTiltY(),
              nc.getTiltZ(),
              nc.getAccelX(),
              nc.getAccelY(),
              nc.getAccelZ());

  if (nc.isOk()) {
    // de-bounce C
    if (nc.getButtonC()) {
      if (!cDown) {
        onCButtonDown();
      }
      cDown = true;
    } else {
      if (cDown) {
        onCButtonUp();
      }
      cDown = false;
    }

    // de-bounce Z
    if (nc.getButtonZ()) {
      if (!zDown) {
        onZButtonDown();
      }
      zDown = true;
    } else {
      if (zDown) {
        onZButtonUp();
      }
      zDown = false;
    }

    switch (mode) {
      case ANGLE:
        break;
      case CIRCLE:
        break;
      case EIGHT:
        break;
      case SQUARE:
        break;
      case SPIRAL:
        break;
      case MIDDLE:
      default:
        break;
    }
  } else {
    mode = MIDDLE; //Nunchuck is on the fritz / disconnected. Default back to the center setpoint.
    dir = CW;
    Logger::trace("WHOOPS, there was a problem reading the nunchuck!");
  }

  // Wait a short while
  //  delay(50);
}

//C Button changes direction.
void onCButtonDown() {
  Logger::trace("CButtonDown");
  switch (dir) {
    case CW:
      dir = CCW;
      break;
    case CCW:
      dir = CW;
      break;
  }
}

void onCButtonUp() {
  Logger::trace("CButtonUp");

}

//Z Button changes modes.
void onZButtonDown() {
  Logger::trace("ZButtonDown");
  switch (mode) {
    case ANGLE:
      mode = CIRCLE;
      break;
    case CIRCLE:
      mode = EIGHT;
      break;
    case EIGHT:
      mode = SQUARE;
      break;
    case SQUARE:
      mode = SPIRAL;
      break;
    case SPIRAL:
      mode = MIDDLE;
      break;
    case MIDDLE:
      mode = ANGLE;
    default:

      break;
  }
}

void onZButtonUp() {
  Logger::trace("ZButtonUp");

}

#endif