//*****************************************************************************
//
// PID.c - PID control: Monitors the difference between the desired
// excitation (or input) signal x(t) and an estimate of the plant 
// output m(t) obtained from the sensor. 
//
// Authors:  Kate Chamberlin, Josh Lowe, Robert Loomes
// Last modified:	03.06.18
//
//*****************************************************************************
// Based on the ENCE361 lecture 17 "Digital PID control"
//*****************************************************************************

#include "PID.h"

//*****************************************************************************
// Constants
//*****************************************************************************
float ALT_KP = 0.6;
float ALT_KI = 0.0093;
float ALT_KD = 0.5;
float YAW_KP = 1.0;
float YAW_KI = 0.0009;
float YAW_KD = 2.0;

//*****************************************************************************
// Initiaslise yaw error control gains within a given yaw struct. For use
// within the main gadfly loop.
//*****************************************************************************
void initYawPID(PIDError *yawState)
{
	yawState->derivative = 0.0;
	yawState->integrated = 0.0;
	yawState->previous = 0.0;
}

//*****************************************************************************
// Initiaslise yaw error control gains within a given alt struct. For use
// within the main gadfly loop.
//*****************************************************************************
void initAltPID(PIDError *altState)
{
	altState->derivative = 0.0;
	altState->integrated = 0.0;
	altState->previous = 0.0;
}

//*****************************************************************************
// Return adjustment PID controls for current yaw and altitude based on the 
// current state and variable values of the heli-rig. Essentially produces
// and error control PWM value for altitude and yaw which pushes/maintains
// the helicopter towards it's current set points.
//*****************************************************************************
float * PIDUpdate(uint32_t deltaT, int16_t actualAlt, float actualYaw,
                  uint8_t desiredAlt, int16_t desiredYaw, 
                  PIDError *yawErrorState,
                  PIDError *altErrorState)
{


	//calculate current error values
	double yawError = (double)desiredYaw - (double)actualYaw;
	double altError = (double)desiredAlt - (double)actualAlt;
	
	//init variables
	double yawErrorIntegrated, yawErrorDerivative;
	double altErrorIntegrated, altErrorDerivative;
	
	//calculate and store integrated error values
	yawErrorState->integrated += yawError * (double)deltaT;
	yawErrorIntegrated = yawErrorState->integrated;
	altErrorState->integrated += altError * (double)deltaT;
	altErrorIntegrated = altErrorState->integrated;
	
	//calculate and store derivative error values
	yawErrorState->derivative = (yawError - yawErrorState->previous) / (double)deltaT ;
	yawErrorDerivative = yawErrorState->derivative;
	altErrorState->derivative = (altError - altErrorState->previous) / (double)deltaT ;
	altErrorDerivative = altErrorState->derivative;
	
	//calculate and store a yaw control value 
	float yawControl = yawError * YAW_KP //yaw proportional gain
		+ YAW_KI * yawErrorIntegrated
		+ YAW_KD * yawErrorDerivative;
	yawControl = (yawControl < 5) ? 5 : (yawControl > 95) ? 95 : yawControl;
	
	//calculate and store a altitude control value 	
	float altControl = altError * ALT_KP //altitide proportional gain
		+ ALT_KI * altErrorIntegrated
		+ ALT_KD * altErrorDerivative;
	altControl = (altControl < 5) ? 5 : (altControl > 95) ? 95 : altControl;
	
	yawErrorState->previous = yawError;
	altErrorState->previous = altError;
	
	static float controls[2];
	controls[0] = yawControl;
	controls[1] = altControl;
	//USE THESE CONTROL VALUES TO ADJUST MAIN/TAIL DUTIES DIRECTLY	
	
	return controls;
}

