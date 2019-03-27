
#include "PID.h"
#include "config.h"


static float pidValues[3][3];
//static float pidSetPoints[3];
//static void calculatePIDSetPoints(IMU& imu);


void calculatePIDOutput(float& outPIDRoll, float& outPIDPitch, const float& inPIDRoll, const float& inPIDPitch, const float& sensedRoll, const float& sensedPitch) {

  // local static variables hold the previous errors and the total I components
  static int pidITotal[2] = {0};
  static int lastRollError = 0, lastPitchError = 0, lastYawError = 0;
 
  //------------------------------------------------ Roll Calculations------------------------------------------------------------------------------------
  float rollError = inPIDRoll - sensedRoll;
  pidITotal[ROLL_VALUE] += (pidValues[ROLL_VALUE][INTEGRAL_VALUE] * rollError);

  outPIDRoll = (pidValues[ROLL_VALUE][PROP_VALUE] * rollError) + pidITotal[ROLL_VALUE] + (pidValues[ROLL_VALUE][DERIVATIVE_VALUE] * (rollError - lastRollError));
  lastRollError = rollError;

  //-------------------------------------------------Pitch Calculations-----------------------------------------------------------------------------------
  float pitchError = inPIDPitch - sensedPitch;
  pidITotal[PITCH_VALUE] += (pidValues[PITCH_VALUE][INTEGRAL_VALUE] * pitchError);

  outPIDPitch = (pidValues[PITCH_VALUE][PROP_VALUE] * rollError) + pidITotal[PITCH_VALUE] + (pidValues[PITCH_VALUE][DERIVATIVE_VALUE] * (pitchError - lastPitchError));
  lastPitchError = pitchError;
}

// set the pid gains
void setPIDValues(const float* pValues, const float* iValues, const float* dValues){

  for(int i = 0; i < 2; i++) {

    pidValues[i][PROP_VALUE] = pValues[i];
    pidValues[i][INTEGRAL_VALUE] = iValues[i];
    pidValues[i][DERIVATIVE_VALUE] = dValues[i];
  }
}
