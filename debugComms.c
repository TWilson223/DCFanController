/*
* Debug communication functions for UART ports
*/

#include "system.h"
#include "debugComms.h"

void clearTxBuffer(void)
{
    return;
}

bool receiveMesg(uint8_t data);
void processMesg(void);
void sendInvalidCmdMesg(void);
void sendTempReadMesg(void);
void sendMemoryReadMesg(void);
void sendMemoryWriteMesg(void);
void sendRpmReadMesg(void);
void sendDcReadMesg(void);
