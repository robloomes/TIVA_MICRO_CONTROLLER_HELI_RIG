//*****************************************************************************
//
// display.c - contains functions pertaining to the updating of the display
//             and the serial output of the helicopter.
//
// Authors:  Kate Chamberlin, Josh Lowe, Robert Loomes
// Last modified:  02/06/2018
//
//*****************************************************************************
#include <stdbool.h>
#include "display.h"

#define MAX_VOLTAGE_SWING 1000 //0.8 volts represented on the TIVA.
#define SERIAL_BAUD_RATE 9600
#define SERIAL_CLK_FREQ 16000000

//*****************************************************************************
// Initialise the UART module on the to 9600 Baud, 1 stop bit, no parity
//*****************************************************************************
void initSerial(void) {
    // Enable GPIO port used for UART0 pins.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // Configure the pin muxxing for UART0 functions on port A0 and A1.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    // Enable UART0 so that we can configure the clock.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    // Choose the alt (UART) function for these pins.
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // Initialize the UART for I/O
    UARTStdioConfig(0, SERIAL_BAUD_RATE, SERIAL_CLK_FREQ);
}

//*****************************************************************************
// Intialise the Orbit OLED display
//*****************************************************************************
void initDisplay (void)
{
    // intialise the Orbit OLED display
    OLEDInitialise ();
}

//*****************************************************************************
// Calculates and returns the current height percent. This corresponds 
// to the current 5-95% PWM range.
//*****************************************************************************
int16_t getHeightPercent(uint16_t init_height, uint32_t height_volts) {
    int16_t height_pct = ((float)init_height - (float)height_volts)/MAX_VOLTAGE_SWING * 100; //calculate percentage
    return height_pct;
}

//*****************************************************************************
// Send an array of chars over serial using the Tiva kit's UART module
//*****************************************************************************
void serial_println(const char *pcStr) {
    UARTprintf(pcStr);
}

//*****************************************************************************
// Displays the given paramters on the OLED display as well as send them 
// via serial using the UART module on the Tiva kit. Cycles between OLED and
// serial every time it is called as to not flood outputs too fast.
//*****************************************************************************
bool update_display(uint32_t height_volts,
                    uint16_t init_height,
                    float yawDegrees,
                    bool display_refresh,
                    int16_t yaw_setpoint,
                    uint8_t height_setpoint,
                    uint16_t pwm_main_duty,
                    uint16_t pwm_tail_duty) {
    //TODO implement state printing out
//    char state[12];
//    if (!calibrated) {
//        state = "Calibrating";
//    } else if (!landing && !landed) {
//        state = "Flying";
//    } else if (landing) {
//        state = "Landing";
//    } else {
//        state = "Landed";
//    }
    int16_t height_pct;
    height_pct = getHeightPercent(init_height, height_volts);
    if (display_refresh) {
        char string[300];
        sprintf (string, "Yaw = %3d [%3d] "
                "Alt = %3d [%3d] "
                "Main = %3d  pct "
                "Tail = %3d  pct ",
                 yaw_setpoint,
                 (int16_t)yawDegrees,
                 height_setpoint,
                 height_pct,
                 pwm_main_duty,
                 pwm_tail_duty);
        OLEDStringDraw(string, 0, 0);
    } else {
        char serial_string[300];
        sprintf(serial_string, "------------\nYaw = %3d [%3d] deg\nAlt = %3d [%3d] pct\nMain = %3d  pct\nTail = %3d\n",
                yaw_setpoint,
                (int16_t)yawDegrees,
                height_setpoint,
                height_pct,
                pwm_main_duty,
                pwm_tail_duty
                //state
                ); //TODO
        serial_println(serial_string);
    }

    display_refresh = !display_refresh;
    return display_refresh;
}

