/*
* DC Fan Controller
* Controller meant to read temperature data from external sensors through internal ADC
* and resond to that feedback by providing PWM control signals to standard 4-pin DC fans 
* and voltage control to 3-pin DC fans.
* 
* TWilson223
*
*/

#include <system.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT

    // Configure GPIO
    P1DIR |= BIT0;                          // Clear P1.0 output latch for a defined power-on state
    P1OUT |= BIT0;                          // Set P1.0 to output direction

    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings

    while(1)
    {
        P1OUT ^= BIT0;                      // Toggle LED
        __delay_cycles(100000);
    }
}
