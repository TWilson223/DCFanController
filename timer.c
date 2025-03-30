/*
* Timer PWM, CCI, interrupt, and delay init, functions, etc.
*
* The MSP430FR6989 has 5 16 bit timers with up to 7 capture/compare registers each. This controller is 
* limited to 4 DC fans to control due to pin output limitations on the FR6989 eval board used to test. 
* Custom hardware could increase the # of fans this controller can interface with.
*/

#include "system.h"
#include "timer.h"

//Lookup table for duty cycle | Format: {temperature (C), duty cycle %}
int therm_dc_L_table[26][2] = 
{
    {0, 0},     {5, 0},     {10, 0},    {15, 0},    {20, 20},   {25, 20},
    {30, 25},   {35, 25},   {40, 35},   {45, 40},   {50, 45},   {55, 60},
    {60, 65},   {65, 70},   {70, 80},   {75, 85},   {80, 90},   {85, 95},
    {90, 100},  {95, 100},  {100, 100}, {105, 100}, {110, 100}, {115, 100},
    {120, 100}, {125, 100},
};

//Initializes timers for delay and interval
void timerInitialize(void)
{
    /*
    * Set up timerA2_interval, timer A2
    * Controls update interval for PWM & tach
    */    
    TA2CTL = (TASSEL__ACLK);                                    //ACLK CLK 32KHz source, DIV = 1
    TA2EX0 = TAIDEX_0;                                          //CLK divide by 1

    /*
    * Set up timerA3_delay, timer A3
    * Used to generate small delays
    */
    TA3CTL = (TASSEL__SMCLK | ID__8);                           //SMCLK CLK 16/8 = 2MHz source, DIV = 8
    TA3EX0 = TAIDEX_0;                                          //CLK divide by 1
}

bool timerStart(uint8_t timer, uint16_t delay)
{
    if(timer == timerA2_interval)
    {
        TA2CTL &= ~(TAIFG);                                     //CLR int flag
        TA2CTL |= (TAIE);                                       //Enable interrupt
        TA2CTL |= (TACLR);                                      //Timer_2A clear
        TA2R = delay;                                           //delay = 32768 | 1000 milliseconds
        TA2CTL |= (MC__UP);                                     //Start timer in UP mode                                         
    }
    else if(timer == timerA3_delay)
    {
        TA3CTL &= ~(TAIFG);                                     //CLR int flag
        TA3CTL |= (TAIE);                                       //Enable interrupt
        TA3CTL |= (TACLR);                                      //Timer_3A clear
        TA3R = delay;                                           //Set delay
        TA3CTL |= (MC__UP);                                     //Start in UP mode
    }
    else
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

bool timerStop(uint8_t timer)
{
    if(timer == timerA2_interval)
    {
        TA2CTL &= ~(TAIFG);                                     //CLR int flag
        TA2CTL &= ~(TAIE);                                      //Disable interrupt
        TA2CTL &= 0xFFCF;                                       //Stop timer
        TA2CTL |= (TACLR);                                      //Timer_2A clear
                                               
    }
    else if(timer == timerA3_delay)
    {
        TA3CTL &= ~(TAIFG);                                     //CLR int flag
        TA3CTL &= ~(TAIE);                                      //Disable interrupt
        TA3CTL &= 0xFFCF;                                       //Stop timer
        TA3CTL |= (TACLR);                                      //Timer_3A clear
    }
    else
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

//timerA2_interval Interrupt Vector (TAIV) handler
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER2_A1_VECTOR
__interrupt void TIMER2_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER2_A1_VECTOR))) TIMER2_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(TA2IV, TA2IV_TAIFG))
  {
    case TA2IV_NONE:   break;                       //No interrupt
    case TA2IV_TACCR1: break;                       //CCR1
    case TA2IV_3:      break;                       //reserved
    case TA2IV_4:      break;                       //reserved
    case TA2IV_5:      break;                       //reserved
    case TA2IV_6:      break;                       //reserved
    case TA2IV_TAIFG:                               //overflow
        TA2CTL &= ~(TAIFG);                         //CLR int flag
        if(controllerData.secTracker == 65535)      //Check for rollover
            controllerData.secTracker = 736;        //Account for time since last hour + 1 sec
        else
            controllerData.secTracker++;

        if(controllerData.secTracker % 3600 == 0)
            controllerData.hourTracker++;
                
        controllerData.fanDataReadyToUpdate = 1;    //Flag to indicate time to update fan data
        break;
    default: break;
  }
}

//timerA3_delay Interrupt Vector (TAIV) handler
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER3_A1_VECTOR
__interrupt void TIMER3_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER3_A1_VECTOR))) TIMER3_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(TA3IV, TA3IV_TAIFG))
  {
    case TA3IV_NONE:   break;                       //No interrupt
    case TA3IV_TACCR1: break;                       //CCR1
    case TA3IV_TACCR2: break;                       //CCR2
    case TA3IV_TACCR3: break;                       //CCR3
    case TA3IV_TACCR4: break;                       //CCR4
    case TA3IV_TAIFG:                               //overflow
        timerStop(timerA3_delay);                   //Stop timer3            
        __bic_SR_register_on_exit(LPM0_bits);       //Return to execution
        break;
    default: break;
  }
}
