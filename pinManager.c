/*
*
* Pin setting functions
* 
*/

#include "pinManager.h"
#include <msp430.h>

void pinInitialization(void)
{
    //Set all ports to output low to eliminate wasted current consumption on unused pins
    P1DIR = P2DIR = P3DIR = P4DIR = P5DIR = P6DIR = P7DIR = P8DIR = P9DIR = P10DIR = 0xFF;
    P1OUT = P2OUT = P3OUT = P4OUT = P5OUT = P6OUT = P7OUT = P8OUT = P9OUT = P10OUT = 0x00;

    //Input pins
    P8DIR = 0x0F;                           //8.4 - 8.7 configured as analog inputs
    P9DIR = 0x90;                           //9.0 analog input, 9.1,2,3,5,6 as compare/count input

    P8REN = 0x00;                           //Resistor PU/PD disabled on ports 8 & 9     
    P9REN = 0x00;

    P8SEL1 = 0xF0;                          //Select Analog/CompE function A7-A4 for 8.4-8.7
    P8SEL0 = 0xF0;
    P9SEL1 = 0x6F;                          //Select Analog/CompE function 9.0-9.5, & 9.6
    P9SEL0 = 0x6F;

    //PWM pins
    P1SEL0 |= BIT5;                         //P1.5 timer A, TA0.0 function selected
    P1SEL1 |= BIT5;
    P2SEL1 |= BIT1;                         //P2.1 timer B, TB0.5 function selected
    P2SEL0 |= BIT7;                         //P2.7 timer B, TB0.6 function selected
    P3SEL1 |= (BIT3 | BIT6);                //P3.3 timer A TA1.1 and 3.6 timer B, TB0.2  function selected

    //CCIs
    P2DIR &= ~(BIT0 | BIT2 | BIT4 | BIT5);  //P2.0 & P2.2 & P2.4 & P2.5 inputs
    P3DIR &= ~(BIT7);                       //P3.7 input

    P2SEL1 |= (BIT0 | BIT2);                //P2.0 TB0.CCI6B, P2.2 TB0.CCI4B function
    P2SEL0 |= (BIT4 | BIT5);                //P2.4 TB0.CCI3A, P2.5 TB0.CCI4A function
    P3SEL1 |= BIT7;                         //P3.7 TB0.CCI3B function    

    //I2C
    P4SEL1 |= (BIT0 | BIT1);                //P4.0, 4.1 SDA/SCL function on UCB1
    
    //UART, debug
    P4SEL0 |= (BIT2 | BIT3);                //P4.2, 4.3 TXD/RXD function on UCA0

    //SPI
    P1SEL0 |= (BIT4 | BIT6 | BIT7);         //P1.4, 1.6, 1.7 CLK, SIMO, SOMI on UCCB0
                                            //P1.3 configured as CS for LCD
                                            //P2.3 configured as CS for data transmit device

    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port setting
}
