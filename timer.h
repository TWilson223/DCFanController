/*
* Functions/definitions for timer module
* 
*/

#ifndef timer_h
#define timer_h

//PWM frequency set to 25KHz
#define PWM_FREQ            400

#define timerA2_interval    2
#define timerA3_delay       3 

//Interval countdown timer and general delay functions
void timerInitialize(void);
bool timerStart(uint8_t timer, uint16_t delay);                                         
bool timerStop(uint8_t timer);

//PWM functions
void pwmBankInitialize(void);
bool pwmNumStart(uint8_t connNum[]);
bool pwmNumUpdate(uint8_t dc, uint8_t connNum[]);
bool pwmNumStop(uint8_t connNum[]);

//Tachometer functions
void tachInitialize(void);
bool tachRead(uint8_t connNum[]);
bool tachDataUpdate(uint8_t connNum[]);

//Watchdog functions
void watchdogTimerReset(void);

#endif
