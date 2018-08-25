//*****************************************************************************
//
// pwm.c - Initialises and generates pwm output for helicopter.
//
// Authors:  Kate Chamberlin, Josh Lowe, Robert Loomes
// Last modified:  02/06/2018
//
//*****************************************************************************

#include "pwm.h"

/*******************************************
 *      PWM config details.
 *******************************************/
#define PWM_RATE_HZ  200
#define PWM_START_PC  10
#define PWM_DIVIDER_CODE  SYSCTL_PWMDIV_2
#define PWM_DIVIDER  1
uint32_t ui32Period;

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
void setPWM_main(uint16_t pwm_main_duty) {
    PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM,
        ui32Period * pwm_main_duty / 100);
}

//*****************************************************************************
// Set the PWM duty cycle of the tail rotor
//*****************************************************************************
void setPWM_tail(uint16_t pwm_tail_duty) {
    PWMPulseWidthSet(PWM_TAIL_BASE, PWM_TAIL_OUTNUM,
        ui32Period * pwm_tail_duty / 100);
}

//*****************************************************************************
// Initialise the PWM generation on the relevant pins
//*****************************************************************************
void initPWM(void)
{
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIO);
    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_MAIN_GPIO_BASE, PWM_MAIN_GPIO_PIN);
    // Calculate the PWM period corresponding to PWM_RATE_HZ.
    ui32Period = SysCtlClockGet() / PWM_DIVIDER / PWM_RATE_HZ;
    PWMGenConfigure(PWM_MAIN_BASE, PWM_MAIN_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM_MAIN_BASE, PWM_MAIN_GEN, ui32Period);
    // Set the pulse width for PWM_START_PC % duty cycle.
    //   PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM, ui32Period * PWM_START_PC / 100);
    setPWM_main(0);
    PWMGenEnable(PWM_MAIN_BASE, PWM_MAIN_GEN);
    // Disable the output.  Repeat this call with 'true' to turn O/P on.
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, false);
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_GPIO);
    GPIOPinConfigure(PWM_TAIL_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_TAIL_GPIO_BASE, PWM_TAIL_GPIO_PIN);
    PWMGenConfigure(PWM_TAIL_BASE, PWM_TAIL_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM_TAIL_BASE, PWM_TAIL_GEN, ui32Period);
    // Set the pulse width for PWM_START_PC % duty cycle.
//    PWMPulseWidthSet(PWM_TAIL_BASE, PWM_TAIL_OUTNUM, ui32Period * PWM_START_PC / 100);
    setPWM_tail(0);
    PWMGenEnable(PWM_TAIL_BASE, PWM_TAIL_GEN);
    // Disable the output.  Repeat this call with 'true' to turn O/P on.
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, false);
}
