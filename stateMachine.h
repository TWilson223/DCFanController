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
struct state;
typedef void stateFunc(struct state *);

struct state
{
    stateFunc * next;
    stateFunc * previous;
    int i;
};

//State machine functions
stateFunc powerupInitialize;
stateFunc pwmSetUpdate;
stateFunc fanDataUpdate;
stateFunc fanLogData;

#endif
