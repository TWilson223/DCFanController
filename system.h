/*
* System level constants and definitions
* 
*/

#ifndef system_h
#define system_h

#include <msp430.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stateMachine.h>
#include <pinManager.h>
#include <uart.h>
#include <debugComms.h>
#include <i2c.h>

//Constants
#define TEMP_LO_LIMIT 0x0000    //Low & high limit for ADC temp reads, 0 - 125 in degrees C
#define TEMP_HI_LIMIT 0x007D
#define RPM_LO_LIMIT  0x0000    //Upper & lower RPM limits
#define RPM_HI_LIMIT  0x2710
#define DC_LO_LIMIT   0         //Duty cycle upper & lower limits expressed as a percentage
#define DC_HI_LIMIT   100

//Data structure for controller
typedef struct
{
    //State machine, current state
    uint8_t controllerState;

    //Debug comms message structure
    uint8_t controllerStatusByte0;
    uint8_t controllerStatusByte1;
    uint8_t controllerStatusByte2;
    uint8_t controllerStatusByte3;

    //UART debug interface
    uartData_t debugInt;

    //I2C interface
    i2cData_t i2cInt;
    
    //Status flags
    bool postInProgress;        //POST in progress
    bool adcReadInProgress;     //ADC read in progress on temp sensors
    bool isConnected[5];        //Array to indicate if a fan is connected
    bool softwareError;         //Flag for software error
    bool debugIntMsg;           //Indicates debug interface message & routing

    //ADC channel read values
    uint16_t temp0;
    uint16_t temp1;
    uint16_t temp2;
    uint16_t temp3;
    uint16_t temp4;

    //Timers
    uint8_t secTracker;         //Variable to track seconds in timer module
    uint16_t tenthSecTracker;   //Variable to track intervals of 100 ms in timer module

    //DC Fan data values
    uint16_t fan0Tach;          //Tracks connected fan speed in rotations per minute
    uint16_t fan1Tach;
    uint16_t fan2Tach;
    uint16_t fan3Tach;
    uint16_t fan4Tach;
    uint8_t fan0DutyCyc;        //Tracks fan duty cycle as a percentage
    uint8_t fan1DutyCyc;
    uint8_t fan2DutyCyc;
    uint8_t fan3DutyCyc;
    uint8_t fan4DutyCyc;

}controllerDatabase_t;

extern controllerDatabase_t controllerData;

#endif
