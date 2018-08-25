//*****************************************************************************
//
// inits.h - functions for initialising all peripherals.
//
// Authors:  Kate Chamberlin, Josh Lowe, Robert Loomes
// Last modified:  02/06/2018
//
//*****************************************************************************

#include "inits.h"

#define SAMPLE_RATE_HZ 320
#define BUF_SIZE 10

PIDError yawErrorState;
PIDError altErrorState;
float *PIDvalues;
volatile bool yawPrevious;
static uint32_t g_ulSampCnt;    // Counter for the interrupts
static circBuf_t g_inBuffer;    // Buffer of size BUF_SIZE integers (sample values)

//*****************************************************************************
// Determines the result of an interrupt via a system tick. Processes ADC and
// sample counter
//*****************************************************************************
void SysTickIntHandler(void)
{
    // Initiate a conversion
    ADCProcessorTrigger(ADC0_BASE, 3);
    g_ulSampCnt++;
}


//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
void
initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}

//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
void ADCIntHandler(void)
{
    uint32_t ulValue;
    // Get the single sample from ADC0.  ADC_BASE is defined in
    // inc/hw_memmap.h
    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
    // Place it in the circular buffer (advancing write index)
    writeCircBuf (&g_inBuffer, ulValue);
    // Clean up, clearing the interrupt
    ADCIntClear(ADC0_BASE, 3);
}

//*****************************************************************************
// Initialisation and configuration of the ADC peripheral and interrupt
//*****************************************************************************
void
initADC (void)
{
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE |
                             ADC_CTL_END);
    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);
    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);
    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}

//*****************************************************************************
// Initialisation and configuration of quadrature encoder pins. Requires the use
// 2 pins that when read, can be used to calculate the exact current 
// rotaional yaw position of the heli-rig
//*****************************************************************************
void initEncoder(void)
{
    // Pin B0 setup
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);        // Enable port B
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);  // Init PB0 as input
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1,
       GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);  // Enable weak pullup resistor for PB0
}

//*****************************************************************************
// Initialisation and configuration of the pin that reads LOW when the heli-rig
// is facing the camera (yaw reference point)
//*****************************************************************************
void initYawRef(void)
{
    // Pin C4 setup
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);        // Enable port C
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4);  // Init PC4 as input
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_4,
       GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);  // Enable weak pullup resistor for PC4
    GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_DIR_MODE_IN);

}

//*****************************************************************************
// Using quadrature encoding, calulates the exact current rotional yaw position
// of the heli rig using the values of 2 yaw encoder pins.
//*****************************************************************************
void readYaw(void) {
    IntMasterDisable();
    bool AChan = (bool)GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_0);
    bool BChan = (bool)GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_1);
    bool direction = AChan ^ yawPrevious;
    if (direction) {yawTicks -= 1;}
    else {yawTicks += 1;}
    yawPrevious = BChan;
    GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);// Clear interrupt flag
    IntMasterEnable();
}

//*****************************************************************************
// Initialisation and configuration of an interrupt that detects a change of
// yaw. Is used to trigger a calulation of the new yaw position. Active on both
// edges (CW and CCW rotations).
//*****************************************************************************
void initIntYaw(void)
{
    GPIOIntDisable(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOIntRegister(GPIO_PORTB_BASE, readYaw);     // Register our handler function for port F
    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1,
            GPIO_BOTH_EDGES);// Configure PF4 for falling edge trigger
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
}



//*****************************************************************************
// For calibration purposes. Let's the module know that the yaw reference 
// point is now known, and that any future yaw changes will be calculated
// to that point.
//*****************************************************************************
void readYawRef(void)
{
    IntMasterDisable();
    yawTicks = 0;
    yaw_setpoint = 0;
    yawRefFound = 1;
    IntMasterEnable();
}

//*****************************************************************************
// Initialisation and configuration of the interrupt triggered by the heli-rig
// facing the camera (yaw reference point).
//*****************************************************************************
void initIntYawRef(void)
{
    GPIOIntDisable(GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOIntRegister(GPIO_PORTC_BASE, readYawRef);
    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTC_BASE, GPIO_PIN_4);
}

//*****************************************************************************
// Read the contents of a circular buffer to return a value indicating 
// the heli-rig's current altitude.
//*****************************************************************************
uint32_t calcBufferSum(void){
    uint16_t i; // counter variable for use in for loops
    uint32_t buffer_sum = 0;
    for (i = 0; i < BUF_SIZE; i++) {
        buffer_sum = buffer_sum + readCircBuf (&g_inBuffer);
    }
    return buffer_sum;
}

//*****************************************************************************
// Calls the PID module to return error new, error controlled PWM values
//*****************************************************************************
float* updatePID(uint32_t delta, int16_t height_pct, float yawDegrees, uint8_t height_setpoint, uint16_t yaw_setpoint) {
    PIDvalues = PIDUpdate(delta,
                           height_pct,
                           yawDegrees,
                           height_setpoint,
                           yaw_setpoint,
                           &yawErrorState,
                           &altErrorState);
    return PIDvalues;
}

//*****************************************************************************
// Initialises the altitude on program startup so that changes can be made in
// reference to the grounded position.
//*****************************************************************************
uint16_t initAlt(uint32_t curr_alt, uint8_t settle) {
    //settling the initial altitude
    uint16_t init_alt = 0;
    if (settle == 1) {
        init_alt = curr_alt;
    }
    return init_alt;
}

//*****************************************************************************
// Calls all initialisation and interrupt functions for use in the main gadfly 
// loop.
//*****************************************************************************
void initAll(void) {
    yawPrevious = (GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1));
    initCircBuf (&g_inBuffer, BUF_SIZE);
    initSerial();
    initPWM();
    initMainSwitchState();
    initYawPID(&yawErrorState);
    initAltPID(&altErrorState);
    initButtons ();
    initClock();
    initADC();
    initEncoder();
    initYawRef();
    initIntYawRef();
    initIntYaw();
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, true);
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, true);
}




