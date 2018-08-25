//*****************************************************************************
//
// pwm.h - Header file for initialising pwm peripherals and starting pwm gen.
//
// Authors:  Kate Chamberlin, Josh Lowe, Robert Loomes
// Last modified:  02/06/2018
//
//*****************************************************************************

#ifndef PWM_H_
#define PWM_H_


#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

/*******************************************
 *      PWM config details.
 *******************************************/
#define PWM_RATE_HZ  200
#define PWM_START_PC  10
#define PWM_DIVIDER_CODE  SYSCTL_PWMDIV_2
#define PWM_DIVIDER  1

/*******************************************
 *      PWM Hardware Details.
 *******************************************/
//---Main Rotor PWM: M0PWM7,PC5, J4-05
#define PWM_MAIN_BASE       PWM0_BASE
#define PWM_MAIN_GEN        PWM_GEN_3   //covers outputs 6 and 7
#define PWM_MAIN_OUTNUM     PWM_OUT_7
#define PWM_MAIN_OUTBIT     PWM_OUT_7_BIT
#define PWM_MAIN_PERIPH_PWM SYSCTL_PERIPH_PWM0 //module 0
#define PWM_MAIN_PERIPH_GPIO    SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_GPIO_BASE      GPIO_PORTC_BASE
#define PWM_MAIN_GPIO_CONFIG    GPIO_PC5_M0PWM7
#define PWM_MAIN_GPIO_PIN       GPIO_PIN_5

//---Tail Rotor PWM: M1PWM5,PF1, J3-10
#define PWM_TAIL_BASE       PWM1_BASE
#define PWM_TAIL_GEN        PWM_GEN_2   //covers outputs 4 and 5
#define PWM_TAIL_OUTNUM     PWM_OUT_5
#define PWM_TAIL_OUTBIT     PWM_OUT_5_BIT
#define PWM_TAIL_PERIPH_PWM SYSCTL_PERIPH_PWM1 //module 0
#define PWM_TAIL_PERIPH_GPIO    SYSCTL_PERIPH_GPIOF
#define PWM_TAIL_GPIO_BASE      GPIO_PORTF_BASE
#define PWM_TAIL_GPIO_CONFIG    GPIO_PF1_M1PWM5
#define PWM_TAIL_GPIO_PIN       GPIO_PIN_1

//*****************************************************************************
// set the PWM duty cycle of the main rotor
//*****************************************************************************
void setPWM_main(uint16_t pwm_main_duty);

//*****************************************************************************
// Set the PWM duty cycle of the tail rotor
//*****************************************************************************
void setPWM_tail(uint16_t pwm_tail_duty);

//*****************************************************************************
// Initialise the PWM generation
//*****************************************************************************
void initPWM(void);

#endif /* PWM_H_ */
