//*****************************************************************************
//
// controller_mode.c - Handles the SW1 switch actions,
//                       Used for flying/landing the helicopter
//
// Authors:  Kate Chamberlin, Josh Lowe, Robert Loomes
// Last modified:	3-06-18
//
//*****************************************************************************

#include "controller_mode.h"

//*****************************************************************************
// Global variables
//*****************************************************************************
unsigned long mainSwitchState; //1 = flying, 0 = landed

//*****************************************************************************
// Reads a bool of the slider switch state and stores result in global variable
//*****************************************************************************
void changeMainSwitchState (void) {
	mainSwitchState = (bool)GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_7);
}

//*****************************************************************************
// Reads and returns a bool that corresponds to current switch state: 
// Switched up- true, switched down- false
//*****************************************************************************
bool mainSwitchStateChanged (bool main_on) {
	bool state = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_7);
	if (main_on != state) {
	    return true;
	} else {
	    return false;
	}
}

//*****************************************************************************
// Initialise the specific pin that reads the state on the slider switch
//*****************************************************************************
void initMainSwitchState(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_7);
    GPIOPadConfigSet(GPIO_PORTA_BASE,  GPIO_PIN_7, GPIO_STRENGTH_2MA,
             GPIO_PIN_TYPE_STD_WPD);
}
