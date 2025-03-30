/*
*
* DC Fan Controller
* Controller meant to read temperature data from external sensors through internal ADC
* and resond to that feedback by providing PWM control signals to standard 4-pin DC fans 
* and voltage control to 3-pin DC fans.
* 
*/

#include "system.h"
#include "stateMachine.h"

//Define instance of database
controllerDatabase_t controllerData;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;       //Stop WDT, for now for debugging

    if(!(clockSetup()))
        return EXIT_FAILURE;

    //Initialize first state
    struct state currentState = {powerupInitialize, NULL, 0};    

    while(currentState.next) currentState.next(&currentState);

    return EXIT_SUCCESS;
}
