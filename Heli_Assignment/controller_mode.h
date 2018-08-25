#ifndef CONTORLLER_MODE_H_
#define CONTORLLER_MODE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"

//*****************************************************************************
// Reads and returns a bool that corresponds to current switch state: 
// Switched up- true, switched down- false
//*****************************************************************************
bool mainSwitchStateChanged (bool main_on);

//*****************************************************************************
// Reads a bool of the slider switch state and stores result in global variable
//*****************************************************************************
void changeMainSwitchState (void);

//*****************************************************************************
// Initialise the specific pin that reads the state on the slider switch
//*****************************************************************************
void initMainSwitchState(void);

#endif /* CONTROLLER_MODE_H_ */
