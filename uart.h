/*
* UART related functions/definitions
* 
*/

#ifndef uart_h
#define uart_h

#define UART_BUADRATE 115200
#define MAX_UART_DATA_LEN 41

void uartInit(void);
bool uartStart(void);
bool uartStop(void);
bool uartSendBufferByte(void);
bool uartSetBufferTx(void);

//Data structure to hold UART information
typedef struct{

    //Data buffers
    uint8_t txMesgBuffer[MAX_UART_DATA_LEN];
    uint8_t rxMesgBuffer[MAX_UART_DATA_LEN];

    //TX buffer data
    uint8_t txMesgEn;       //Transmit in progress
    uint8_t txMesgIndex;    //TX buffer index
    uint8_t txMesgLen;      //TX buffer length
    uint8_t txMesgCntr;     //TX message counter

    //RX buffer data
    uint8_t rxMesgEn;       //Receive in progress
    uint8_t rxMesgIndex;    //RX buffer index
    uint8_t rxMesgLen;      //RX buffer length
    uint8_t rxMesgCntr;     //Tracks # of RX messages

    bool commError;
    bool responsePending;
    
}uartData_t;

#endif
