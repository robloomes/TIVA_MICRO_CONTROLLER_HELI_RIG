//*****************************************************************************
//
// display.h - Header file for updating display
//
// Authors:  Kate Chamberlin, Josh Lowe, Robert Loomes
// Last modified:  02/06/2018
//
//*****************************************************************************

#ifndef DISPLAY_H_
#define DISPLAY_H_


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "OrbitOLED/lib_OrbitOled/OrbitOled.h"

#define MAX_VOLTAGE_SWING 1000 //0.8 volts represented on the TIVA.
#define SERIAL_BAUD_RATE 9600
#define SERIAL_CLK_FREQ 16000000

//*****************************************************************************
// Initialise the UART for serial communication
//*****************************************************************************
void initSerial(void);

//*****************************************************************************
// Initialise the OLED display for debugging
//*****************************************************************************
void initDisplay(void);

//*****************************************************************************
// Calculate the height percentage for display
//*****************************************************************************
int16_t getHeightPercent(uint16_t init_height, uint32_t height_volts);

//*****************************************************************************
// Print the line for serial use
//*****************************************************************************
void serial_println(const char *pcStr);

//*****************************************************************************
// Update the either the display or the serial output
//*****************************************************************************
bool update_display(uint32_t height_volts,
                    uint16_t init_height,
                    float yawDegrees,
                    bool display_refresh,
                    int16_t yaw_setpoint,
                    uint8_t height_setpoint,
                    uint16_t pwm_main_duty,
                    uint16_t pwm_tail_duty);

#endif /* DISPLAY_H_ */
