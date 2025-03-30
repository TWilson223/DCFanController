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

//Lookup table for duty cycle | Format: {temperature (C), duty cycle %}
int therm_dc_L_table[26][2] = 
{
    {0, 0},     {5, 0},     {10, 0},    {15, 0},    {20, 20},   {25, 20},
    {30, 25},   {35, 25},   {40, 35},   {45, 40},   {50, 45},   {55, 60},
    {60, 65},   {65, 70},   {70, 80},   {75, 85},   {80, 90},   {85, 95},
    {90, 100},  {95, 100},  {100, 100}, {105, 100}, {110, 100}, {115, 100},
    {120, 100}, {125, 100},
};

//Interval countdown timer and general delay functions
void timerInitialize(void);
bool timerStart(uint8_t timer);                                         
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
