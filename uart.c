/*
* UART init, setup, and functions
* 
*/

#include "system.h"
#include "uart.h"

//Init UART for debug communication
void uartInit(void)
{
    //Configure eUSCI A0 for UART
    //UCA0CTLW0 defaults to 1 stop bit, no parity, 8 bit data
    UCA0CTLW0 = UCSWRST;                            //Hold in reset state
    UCA0CTLW0 |= (UCSSEL1 | UCSSEL0);               //Select SMCLK CLK source
    
    /*
    * Set Baud rate, reference table 30-5 in MSP430FR6989 user guide
    * 16 MHz CLK in | 115200 Baud
    */
    UCA0BRW = 0x0008;                               //Baud rate CLK divider
    UCA0MCTLW |= (0xF700 | 0x00A0 | UCOS16);        //Oversampling mode active, UCBRFx = 10, UCBRSx = 0xF7

    //Enable interrupts for char in RX buffer & empty TX buffer
    UCA0IE = (UCTXIE | UCRXIE);

    return;
}

