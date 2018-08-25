//*****************************************************************************
//
// heli_main.c - Main gadfly loop for helicopter project
//
// Authors:  Kate Chamberlin, Josh Lowe, Robert Loomes
// Last modified:	25-05-18
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "driverlib/sysctl.h"
#include "inits.h"
#include "PID.h"

//*****************************************************************************
// Global Variables
//*****************************************************************************

uint8_t height_setpoint;
uint16_t pwm_main_duty = 0;
uint16_t pwm_tail_duty = 0;
bool display_refresh = true;
bool main_on = false;
bool calibrated;

enum State {CALIBRATING, LANDED, FLYING, LANDING}; //states of heli
uint8_t state = LANDED; //initial state

//*****************************************************************************
// Additional Functions
//*****************************************************************************

// Calculates the yaw in degrees
float calcYaw(void) {
    float yawDegrees = yawTicks * (360.0/448.0);
    return yawDegrees;
}

// Polls the buttons to adjust setpoints accordingly.
void pollButtons(void) {
    uint8_t butState;
    // Button polling
    butState = checkButton (LEFT);
    if (butState == PUSHED) {
        yaw_setpoint -= 15;
    }
    butState = checkButton (RIGHT);
    if (butState == PUSHED) {
        yaw_setpoint += 15;
    }
    butState = checkButton (UP);
    if (butState == PUSHED && height_setpoint <= 90) {
        height_setpoint += 10;
    }
    butState = checkButton (DOWN);
    if (butState == PUSHED && height_setpoint >= 10) {
        height_setpoint -= 10;
    }
    updateButtons (); // Poll the buttons
}

// Returns the current state based on switch state, previous state, and whether heli
// has been calibrated or not.
uint8_t determineState(bool main_on, uint8_t state, bool calibrated) {
    uint8_t curr_state;
    //-- statements determining current state--//
    // switched off while flying
    if (!main_on && state == FLYING) {
        curr_state = LANDING;
    // not calibrated
    } else if (!yawRefFound && !calibrated) { //if not calibrated
        curr_state = CALIBRATING;
    // finished calibrating
    } else if (yawRefFound && !calibrated ){
        main_on = false;
        calibrated = true;
        curr_state = LANDED;
    } else if (main_on && state == LANDED){ //turning on
        curr_state = FLYING;
    } else { // switched off
        curr_state = LANDED;
    }
    return curr_state;
}

//*****************************************************************************
// Main Gadfly Loop
//*****************************************************************************
int
main(void)
{
    IntMasterDisable(); // Disable interrupts to the processor for setup
    // Initialises local variables for use within the main loop
    uint32_t buffer_sum;
	uint8_t settle = 0;
	uint16_t init_alt;
	uint32_t mean_val;
    float yawDegrees;
    uint32_t delta = 5;
    int16_t height_pct;

	initAll(); // Initializes the clock, ADC, OLED display, buffer and peripheral buttons etc
    IntMasterEnable(); // Enable interrupts to the processor.

	while (1) //Gadfly loop
	{
	    if (mainSwitchStateChanged(main_on) == 1) {
	        main_on = !main_on; //toggle state
	    }

	    //find current state
	    state = determineState(main_on, state, calibrated);

	    // switch case determining set points dependent on the current state
	    switch (state) {
            case LANDED: height_setpoint = 0; yaw_setpoint = 0; pwm_main_duty = 0; pwm_tail_duty = 0;
                         calibrated = true; height_setpoint = 0; main_on = false; //resetting main_on so it doesnt begin flying immediately
                         break;
            case CALIBRATING: height_setpoint = 10; yaw_setpoint += 1; break;
            case LANDING: if (yaw_setpoint == 0 && height_setpoint != 0) {height_setpoint -= 2;}
                          if (yaw_setpoint != 0) {yaw_setpoint += 2;} break;//gradually return to origin
            case FLYING: pollButtons(); break;
	    }

		// Background task: calculate the (approximate) mean of the values in the
		// circular buffer and display it, together with the sample number.
		buffer_sum = calcBufferSum();
		mean_val = ((float)buffer_sum)/BUF_SIZE + 0.5; //calculate mean with rounding

		// Initialise the starting altitude to 0%
		init_alt = initAlt(mean_val, settle);
		if (settle > 1) {
		    settle = 2; // So the settle int doesnt overflow
		} else {
		    settle += 1;
		}

		// Calculate and display the rounded mean of the buffer contents
	    yawDegrees = calcYaw();
	    display_refresh = update_display(mean_val, init_alt, yawDegrees, display_refresh, yaw_setpoint,
	                   height_setpoint, pwm_main_duty, pwm_tail_duty);
		height_pct = getHeightPercent(init_alt, mean_val); //calculate percentage
		float *PID = updatePID(delta, height_pct, yawDegrees, height_setpoint, yaw_setpoint);

		// Implementing the PID control
        pwm_tail_duty = PID[0];
        pwm_main_duty = PID[1];
		setPWM_main(pwm_main_duty);
		setPWM_tail(pwm_tail_duty);

        SysCtlDelay (SysCtlClockGet() / 8);  // Set gadfly loop timing
	}
}

