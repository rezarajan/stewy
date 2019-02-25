#ifndef __STU_PS2_H__
#define __STU_PS2_H__

#ifdef ENABLE_PS2
#include <PS2X_lib.h> // from https://github.com/madsci1016/Arduino-PS2X
#include "config.h"

/**************************************************
 *  PS2 controller pin definitions
 *************************************************/
#define PS2_DAT        13  //14    
#define PS2_CMD        11  //15
#define PS2_SEL        10  //16
#define PS2_CLK        12  //17

#define rumble false
#define pressures false

PS2X ps2; // ps2 controller class instance

#endif
#endif    //__STU_PS2_H__
