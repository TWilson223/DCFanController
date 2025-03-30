/*
* Timer PWM, CCI, interrupt, and delay init, functions, etc.
*
* The MSP430FR6989 has 5 16 bit timers with up to 7 capture/compare registers each. This controller is limited
* to 4 DC fans to control due to pin output limitations on the FR6989 eval board used to test. Custom
* hardware could increase the # of fans this controller can interface with.
*/

#include "system.h"
#include "timer.h"

//Initializes timer for delay and interval between PWM/Tach update periods
void timerInitialize(void)
{
    //Set up interval timer, timer A2
    TA2CTL = 0x0000;



}
