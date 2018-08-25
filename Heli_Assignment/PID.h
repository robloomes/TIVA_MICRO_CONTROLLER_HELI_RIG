//*****************************************************************************
//
// PID.h - Header file for initialising and setting PID control
//
// Authors:  Kate Chamberlin, Josh Lowe, Robert Loomes
// Last modified:  02/06/2018
//
//*****************************************************************************
#ifndef PID_H_
#define PID_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

//*****************************************************************************
// PID struct to maintain error values and store the previous error for use of
// the main PID function
//*****************************************************************************
typedef struct {
	double derivative;
	double integrated;
	double previous;
} PIDError;

//*****************************************************************************
// Initialize yaw error control gains
//*****************************************************************************
void initYawPID(PIDError *yawState);

//*****************************************************************************
// Initialize altitude error control gains
//*****************************************************************************
void initAltPID(PIDError *altState);

//*****************************************************************************
// Update the PID values
//*****************************************************************************
float * PIDUpdate(uint32_t deltaT, int16_t actualAlt, float actualYaw,
                  uint8_t desiredAlt, int16_t desiredYaw, 
                  PIDError *yawErrorState,
                  PIDError *altErrorState);

#endif /* PID_H_ */

