/*
* I2C related functions/definitions
* 
*/

#ifndef i2c_h
#define i2c_h

//I2C defines
#define MEM_I2C_WRITE_ADDR 0xA0;
#define MEM_I2C_READ_ADDR 0xA1;
#define MAX_I2C_DATA_LEN 41;

//Data structure to hold I2C information
typedef struct{

    //Data buffers
    uint8_t txMesgBuffer[MAX_I2C_DATA_LEN];
    uint8_t rxMesgBuffer[MAX_I2C_DATA_LEN];

    //TX buffer data
    uint8_t txMesgEn;       //Transmit in progress
    uint8_t txMesgIndex;    //TX buffer index
    uint8_t txMesgLen;      //TX buffer length

    //RX buffer data
    uint8_t rxMesgEn;       //Receive in progress
    uint8_t rxMesgIndex;    //RX buffer index
    uint8_t rxMesgLen;      //RX buffer length
    uint8_t rxMesgCntr;     //Tracks # of RX messages

    //Stores current mem address
    uint16_t currentLogAddr;

}i2cData_t;

void i2cInit(void);
void i2cStart(void);
void i2cStop(void);
bool i2cCharTx(uint8_t dataByte);
uint8_t i2cCharRx(void);
bool i2cSendBufferByte(void);
bool i2cReceiveData(uint16_t startAddr, uint8_t n);
bool logDataToMem(void);

#endif
