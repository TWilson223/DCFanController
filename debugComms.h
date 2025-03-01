/*
* Debug terminal functions/defines
* 
*/

#ifndef debugComms_h
#define debugComms_h

//List of available messages for debug/testing
enum debugMessages
{
    DEBUG_TEMP_READ_REQ,
    DEBUG_MEMORY_READ_REQ,
    DEBUG_MEMORY_WRITE_REQ,
    DEBUG_MEMORY_CLEAR_REQ,
    DEBUG_RPM_READ_REQ,
    DEBUG_DC_READ_REQ
};

void clearTxBuffer(void);
bool receiveMesg(uint8_t data);
void processMesg(void);
void sendInvalidCmdMesg(void);
void sendTempReadMesg(void);
void sendMemoryReadMesg(void);
void sendMemoryWriteMesg(void);
void sendRpmReadMesg(void);
void sendDcReadMesg(void);

#endif
