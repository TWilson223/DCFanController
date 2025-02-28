/*
* State machine definitions and function declarations
* 
*/

#ifndef stateMachine_h
#define stateMachine_h

//State definitions
#define POWERUP_INITIALIZE  0x10
#define PWM_SET_UPDATE      0x20
#define FAN_DATA_UPDATE     0x30
#define FAN_LOG_DATA        0x40

//State machine function setup
typedef void *(*FuncArg)(void*);
typedef FuncArg(*StateFunc)(FuncArg);

//State machine functions
StateFunc powerupInitialize(StateFunc prevState);
StateFunc pwmSetUpdate(StateFunc prevState);
StateFunc fanDataUpdate(StateFunc prevState);
StateFunc fanLogData(StateFunc prevState);

uint8_t setControllerState(uint8_t state);

#endif
