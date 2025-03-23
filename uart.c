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
    UCA0CTLW0 = UCSWRST;                                                                                //Hold in reset state
    UCA0CTLW0 |= (UCSSEL1 | UCSSEL0);                                                                   //Select SMCLK CLK source
    
    /*
    * Set Baud rate, reference table 30-5 in MSP430FR6989 user guide
    * 16 MHz CLK in | 115200 Baud
    */
    UCA0BRW = 0x0008;                                                                                   //Baud rate CLK divider
    UCA0MCTLW |= (0xF700 | 0x00A0 | UCOS16);                                                            //Oversampling mode active, UCBRFx = 10, UCBRSx = 0xF7

    return;
}

//Start UART driver
bool uartStart(void)
{
    //Clear TX & RX interrupt flags, disable TX until needed
    UCA0IFG &= ~(UCTXIFG | UCRXIFG);                                                                    //Clear TX & RX int flags
    UCA0IE &= ~UCTXIE;                                                                                  //Disable TX interrupt

    UCA0CTLW0 &= ~UCSWRST;                                                                              //Activate eUSCI A0

    //Enable interrupts for char in RX buffer
    UCA0IE |= UCRXIE;

    return 1;
}

bool uartStop(void)
{
    //Clear TX & RX interrupt flags, disable TX until needed
    UCA0IFG &= ~(UCTXIFG | UCRXIFG);                                                                    //Clear TX & RX int flags
    UCA0IE &= ~(UCTXIE | UCRXIE);                                                                       //Disable TX & RX interrupt

    UCA0CTLW0 |= UCSWRST;                                                                               //Disable eUSCI A0

    return 1;
}

//Poll while waiting for full TX message to send
bool uartSendBuffer(void)
{
    //Send first TX message
    UCA0TXBUF = controllerData.debugInt.txMesgBuffer[controllerData.debugInt.txMesgIndex++];    

    //Wait for rest of TX message to send
    while(controllerData.debugInt.txMesgEn);

    if(controllerData.debugInt.txMesgIndex == controllerData.debugInt.txMesgLen)                        //Check for successful send
    {
        UCA0IFG &= ~UCTXIFG;                                                                            //Clear TX int flag
        UCA0IE &= ~UCTXIE;                                                                              //Disable TX interrupt

        controllerData.debugInt.txMesgIndex = 0;

        return 1;
    }
    else                                                                                                //Full message not sent
        controllerData.debugInt.debugError = 1;                                                         //Indicate error w/ debug comms
    
    return 0;                                                                                           //Return failed
}

//Setup UART TX buffer, interrupts, and set TX enabled flag
bool uartSetBufferTx(void)
{
    //Error handling: if tx already enabled or mesg len is 0, return false
    if((controllerData.debugInt.txMesgEn == true) || (controllerData.debugInt.txMesgLen == 0))
        return 0;

    controllerData.debugInt.txMesgEn = 1;                                                               //TX enabled flag
    controllerData.debugInt.txMesgIndex = 0;                                                            //Reset mesg index

    UCA0IFG &= ~UCTXIFG;                                                                                //Clear TX int flag
    UCA0IE |= UCTXIE;                                                                                   //Enable TX interrupt

    return uartSendBuffer();                                                                            //Send data in TX buffer
}

//eUSCI UART ISR
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
        controllerData.debugInt.rxByte = (uint8_t)(UCA0RXBUF & 0x00FF);                                 //Grab RX data        
        UCA0IFG &= ~UCRXIFG;                                                                            //Clear RX int flag
        receiveMesg(controllerData.debugInt.rxByte);                                                    //TODO: Might need to remove function call here | Receive RX message, will only be 1 byte for UART
        break;
    case USCI_UART_UCTXIFG: 
        UCA0IFG &= ~UCTXIFG;                                                                            //Clear TX int flag
        if(controllerData.debugInt.txMesgIndex < controllerData.debugInt.txMesgLen)                     //Check for next TX message         
            UCA0TXBUF = controllerData.debugInt.txMesgBuffer[controllerData.debugInt.txMesgIndex++];    //Send next message                  
        else
            controllerData.debugInt.txMesgEn = 0;                                                       //TX done
        break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
  }
}
