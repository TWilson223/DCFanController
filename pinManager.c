/*
*
* Pin setting functions
* 
*/

#include "system.h"
#include "pinManager.h"

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
    P3SEL1 |= BIT3;                         //P3.3 timer A TA1.1

    //CCIs for Tach
    P1DIR &= ~(BIT3);                       //P1.3 input
    P2DIR &= ~(BIT4 | BIT5);                //P2.4 & P2.5 inputs
    P3DIR &= ~(BIT6);                       //P3.6 input

    P1SEL0 |= (BIT3);                       //P1.3 TA1.CCI2A function
    P2SEL0 |= (BIT4 | BIT5);                //P2.4 TB0.CCI3A, P2.5 TB0.CCI4A function
    P3SEL1 |= (BIT6);                       //P3.6 TB0.CCI2A

    //I2C
    P4SEL1 |= (BIT0 | BIT1);                //P4.0, 4.1 SDA/SCL function on UCB1
    
    //UART, debug
    P4SEL0 |= (BIT2 | BIT3);                //P4.2, 4.3 TXD/RXD function on UCA0

    //SPI
    P1DIR &= ~(BIT7);                       //P1.7 input
    P1SEL0 |= (BIT4 | BIT6 | BIT7);         //P1.4, 1.6, 1.7 CLK, SIMO, SOMI on UCCB0
                                            //P3.1 configured as CS for LCD
                                            //P2.3 configured as CS for data transmit device                                      

    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port setting
}

bool clockSetup(void)
{
    //Pin setup for external LF crystal
    PJSEL0 |= (BIT4 | BIT5);                         //LFXIN & LFXOUT in crystal mode
    PJSEL1 &= ~(BIT4 | BIT5);

    PM5CTL0 &= ~LOCKLPM5;                           // Disable the GPIO power-on default high-impedance mode
                                                    // to activate previously configured port setting

    //Clock setup
    CSCTL0_H = CSKEY_H;                             //Unlock clock select registers
    CSCTL1 = (DCORSEL | DCOFSEL_4);                 //HF range select en, DCOCLK = 16MHz
    CSCTL2 = (SELM__DCOCLK | SELS__DCOCLK);         //SMCLK & MCLK = DCOCLK;
    CSCTL2 &= ~(SELA_7);                            //ACLK = LFXTCLK
    CSCTL3 = 0x0000;                                //All CLK dividers set to 1
    
    CSCTL4 = (VLOOFF | HFXTOFF);                    //VLO off, HFXT off
    CSCTL4 &= ~(LFXTOFF);                           //LFXT on
    
    //OSC fault interrupt enable
    SFRIE1 |= OFIE;

    //Wait for LFXT configuration and clear fault flag
    do
    {
        CSCTL5 &= ~LFXTOFFG;                        //Clear XT1 fault flag
        SFRIFG1 &= ~OFIFG;                          //Clear OSC fault flag
    }while (SFRIFG1&OFIFG);                         //Test oscillator fault flag 

    CSCTL0_H = 0x00;                                //Re-lock CS registers

    return EXIT_SUCCESS;
}
