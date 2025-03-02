/*
*
* Pin settings & initialization
* 
*/

#include "pinManager.h"
#include <msp430.h>

void pinInitialization(void)
{

    
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port setting
}
