/*
 * PID.h
 *
 *  Created on: Mar 8, 2017
 *      Author: tdawson
 */

#ifndef PID_H_
#define PID_H_

#include "MPU9250.h"

void calculatePIDOutput(float& outPIDRoll, float& outPIDPitch, const float& inPIDRoll, const float& inPIDPitch, const float& sensedRoll, const float& sensedPitch);
void setPIDValues(const float* pValues, const float* iValues, const float* dValues);

#endif /* PID_H_ */

