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

    //Clock setup
    CSCTL0_H = CSKEY_H;                             //Unlock clock select registers
    CSCTL1 = DCORSEL | DCOFSEL_4;                   //HF range select en, DCOCLK = 16MHz
    CSCTL2 = SELA0 | SELM__DCOCLK | SELS__DCOCLK;   //ACLK = VLOCLK, SMCLK & MCLK = DCOCLK;
    CSCTL3 = 0x0000;                                //All CLK dividers set to 1
    CSCTL4 = LFXTOFF | HFXTOFF;                     //External CLK sources off
    CSCTL0_H = 0x00;                                //Re-lock CS registers

    //Initialize first state
    struct state currentState = {powerupInitialize, NULL, 0};    

    while(currentState.next) currentState.next(&currentState);

    return EXIT_SUCCESS;
}
