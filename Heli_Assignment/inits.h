//*****************************************************************************
//
// inits.h - Header file for initialising all peripherals and functions
//
// Authors:  Kate Chamberlin, Josh Lowe, Robert Loomes
// Last modified:  02/06/2018
//
//*****************************************************************************

#ifndef INITS_H_
#define INITS_H_

#define SAMPLE_RATE_HZ 320
#define BUF_SIZE 10

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "buttons4.h"
#include "controller_mode.h"
#include "pwm.h"
#include "display.h"
#include "controller_mode.h"
#include "PID.h"
#include "circBufT.h"

volatile int16_t yaw_setpoint;
volatile bool yawRefFound;
volatile int32_t yawTicks;
PIDError yawErrorState;
PIDError altErrorState;

//*****************************************************************************
// Calculate the sum of all items in the circular buffer
//*****************************************************************************
uint32_t calcBufferSum(void);

//*****************************************************************************
// Update the PID
//*****************************************************************************
float* updatePID(uint32_t delta,
                 int16_t height_pct,
                 float yawDegrees,
                 uint8_t height_setpoint,
                 uint16_t yaw_setpoint);

//*****************************************************************************
// Initialise the altitude to 0%
//*****************************************************************************
uint16_t initAlt(uint32_t curr_alt, uint8_t settle);

//*****************************************************************************
// Initialise all peripherals and interrupts
//*****************************************************************************
void initAll(void);


#endif /* INITS_H_ */
