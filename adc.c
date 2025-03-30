/*
*
* ADC initialization, read, operation functions, etc
* Pins are configured in pinManger.c
*
*/

#include "system.h"
#include "adc.h"

// Lookup table for TMP6131QLP temp sensor | Format: {Temperature (C), Resistance (Ohms)}
uint16_t therm_res_L_table[126][2] = 
{
    {0, 8495},    {1, 8550},    {2, 8605},    {3, 8660},    {4, 8716},
    {5, 8772},    {6, 8829},    {7, 8885},    {8, 8942},    {9, 9000},
    {10, 9057},   {11, 9115},   {12, 9174},   {13, 9232},   {14, 9291},
    {15, 9350},   {16, 9410},   {17, 9469},   {18, 9529},   {19, 9590},
    {20, 9651},   {21, 9712},   {22, 9773},   {23, 9835},   {24, 9897},
    {25, 9959},   {26, 10021},  {27, 10084},  {28, 10147},  {29, 10211},
    {30, 10275},  {31, 10339},  {32, 10403},  {33, 10468},  {34, 10533},
    {35, 10599},  {36, 10664},  {37, 10731},  {38, 10797},  {39, 10864},
    {40, 10931},  {41, 10998},  {42, 11066},  {43, 11134},  {44, 11202},
    {45, 11270},  {46, 11339},  {47, 11409},  {48, 11478},  {49, 11548},
    {50, 11618},  {51, 11689},  {52, 11760},  {53, 11831},  {54, 11903},
    {55, 11975},  {56, 12047},  {57, 12119},  {58, 12192},  {59, 12265},
    {60, 12339},  {61, 12413},  {62, 12487},  {63, 12562},  {64, 12637},
    {65, 12712},  {66, 12787},  {67, 12863},  {68, 12940},  {69, 13016},
    {70, 13093},  {71, 13170},  {72, 13248},  {73, 13326},  {74, 13404},
    {75, 13483},  {76, 13562},  {77, 13641},  {78, 13721},  {79, 13801},
    {80, 13881},  {81, 13962},  {82, 14043},  {83, 14124},  {84, 14206},
    {85, 14288},  {86, 14371},  {87, 14454},  {88, 14537},  {89, 14620},
    {90, 14704},  {91, 14789},  {92, 14873},  {93, 14958},  {94, 15043},
    {95, 15129},  {96, 15215},  {97, 15302},  {98, 15389},  {99, 15476},
    {100, 15563}, {101, 15651}, {102, 15739}, {103, 15828}, {104, 15917},
    {105, 16006}, {106, 16096}, {107, 16186}, {108, 16277}, {109, 16368},
    {110, 16459}, {111, 16551}, {112, 16643}, {113, 16735}, {114, 16828},
    {115, 16921}, {116, 17014}, {117, 17108}, {118, 17203}, {119, 17297},
    {120, 17392}, {121, 17488}, {122, 17584}, {123, 17680}, {124, 17777},
    {125, 17874},
};

void adcInitialize(void)
{
    /* 
    * Analog pins 8.4 - 8.7 and 9.0
    * Analog inputs A4 - A8 with temperature sensors
    * Temp sensors are resistive based and are the grounding resistor on a 
    * voltage divider circuit with VCC = 3.3V. 
    *
    *
    * Note: The ADC is setup here to run one channel conversion at a time 
    * as multichannel reads on MSP430s would require a read from A8 -> A0 everytime,
    * which is time consuming. The plan is to manually read A4 - A8 in adcRead() 
    * by manually changing the channel after each successful read
    */    

    ADC12CTL0 = 0x0000;                     //Disable ADC conversion, reset register to 0
    ADC12CTL0 |= (ADC12SHT0_2 | ADC12ON);   //Sampling time 16 cycles, ADC12 on bit set
    ADC12CTL1 = ADC12SHP;                   //Sample timer is source for SAMPCON, no predivider
                                            //No invert, no clock divider, ADC CLK
    ADC12CTL2 = ADC12RES_2;                 //12-bit res, binary unsigned format, LPM off
    ADC12MCTL0 = ADC12INCH_4;               //CTL0 = A4, Vref=AVCC, single ended input
                                            //Comp window dis
    
    ADC12IER0 = ADC12IE0;                   //Interrupt enabled for MEM register 0

    return;
}

void adcStart(void)
{
    ADC12CTL0 |= (ADC12ENC | ADC12SC);

    return;
}

void adcStop(void)
{
    ADC12CTL1 &= ~ADC12CONSEQ_3;            //Stop active conversion, if any
    ADC12CTL0 &= ~(ADC12ENC | ADC12SC);     //Disable ADC conversion

    return;
}

//Iterates through defined ADC channels and places them in controllerData data structure
void adcRead(void)
{
    controllerData.adcReadInProgress = 1;

    uint16_t i;
    
    //Read 5 ADC channels, from A4 -> A8
    for(i = MIN_ADC_CHANNEL; i <= MAX_ADC_CHANNEL; i++)
    {
        ADC12MCTL0 = i;                                                                             //Set channel read, Vref = AVCC
        adcStart();                                                                                 //Start ADC conversion

        __bis_SR_register(LPM0_bits | GIE);                                                         //Wait for read to finish
                                                                                                    //ADC12IFG0 set when ADC12MEM0 has data

        controllerData.tempSensorValues[(i-(MAX_ADC_CHANNEL-MIN_ADC_CHANNEL))] = ADC12MEM0;         //Return from ISR, grab data in ADC12MEM0

        adcStop();                                                                                  //Reset ADC to change ADC12MCTL0        

        //Small delay between reads
        timerStart(timerA3_delay, 625);                                                             //delays for 312 microseconds
    }

    controllerData.adcReadInProgress = 0;

    return;
}

/*
* Takes in desired resistance value x, thermal lookup table is fixed and ordered
*
* Log(n) search through therm_res_L_table. Intention is to always return a temp equal to
* the measured resistance or the closest (n-1) temp. Returns max table temp for any 
* resistance over the max. Returns 0 for values less than min table resistance.
*/
uint16_t thermSearch(uint16_t x)
{
    uint16_t l = 0;                                                                                 //lower value
    uint16_t r = (sizeof(therm_res_L_table)/sizeof(therm_res_L_table[0]));                          //upper value
    uint16_t m = 0;                                                                                 //middle value

    while(l + 1 < r)
    {
        m = (uint16_t)((l+r)/2);                                                                    //Find mid value

        if(x < (therm_res_L_table[m][1]))
            r = m;
        else
            l = m;
    }

    if((therm_res_L_table[l][1]) <= x)
        return l;

    return 0;
}

//ADC ISR
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(ADC12IV, ADC12IV_ADC12RDYIFG))
    {
        case ADC12IV_NONE:        break;    // Vector  0:  No interrupt
        case ADC12IV_ADC12OVIFG:  break;    // Vector  2:  ADC12MEMx Overflow
        case ADC12IV_ADC12TOVIFG: break;    // Vector  4:  Conversion time overflow
        case ADC12IV_ADC12HIIFG:  break;    // Vector  6:  ADC12BHI
        case ADC12IV_ADC12LOIFG:  break;    // Vector  8:  ADC12BLO
        case ADC12IV_ADC12INIFG:  break;    // Vector 10:  ADC12BIN
        case ADC12IV_ADC12IFG0:             // Vector 12:  ADC12MEM0 Interrupt
            // Exit from LPM0 and continue executing previous func
            __bic_SR_register_on_exit(LPM0_bits);
            break;
        case ADC12IV_ADC12IFG1:   break;    // Vector 14:  ADC12MEM1
        case ADC12IV_ADC12IFG2:   break;    // Vector 16:  ADC12MEM2
        case ADC12IV_ADC12IFG3:   break;    // Vector 18:  ADC12MEM3
        case ADC12IV_ADC12IFG4:   break;    // Vector 20:  ADC12MEM4
        case ADC12IV_ADC12IFG5:   break;    // Vector 22:  ADC12MEM5
        case ADC12IV_ADC12IFG6:   break;    // Vector 24:  ADC12MEM6
        case ADC12IV_ADC12IFG7:   break;    // Vector 26:  ADC12MEM7
        case ADC12IV_ADC12IFG8:   break;    // Vector 28:  ADC12MEM8
        case ADC12IV_ADC12IFG9:   break;    // Vector 30:  ADC12MEM9
        case ADC12IV_ADC12IFG10:  break;    // Vector 32:  ADC12MEM10
        case ADC12IV_ADC12IFG11:  break;    // Vector 34:  ADC12MEM11
        case ADC12IV_ADC12IFG12:  break;    // Vector 36:  ADC12MEM12
        case ADC12IV_ADC12IFG13:  break;    // Vector 38:  ADC12MEM13
        case ADC12IV_ADC12IFG14:  break;    // Vector 40:  ADC12MEM14
        case ADC12IV_ADC12IFG15:  break;    // Vector 42:  ADC12MEM15
        case ADC12IV_ADC12IFG16:  break;    // Vector 44:  ADC12MEM16
        case ADC12IV_ADC12IFG17:  break;    // Vector 46:  ADC12MEM17
        case ADC12IV_ADC12IFG18:  break;    // Vector 48:  ADC12MEM18
        case ADC12IV_ADC12IFG19:  break;    // Vector 50:  ADC12MEM19
        case ADC12IV_ADC12IFG20:  break;    // Vector 52:  ADC12MEM20
        case ADC12IV_ADC12IFG21:  break;    // Vector 54:  ADC12MEM21
        case ADC12IV_ADC12IFG22:  break;    // Vector 56:  ADC12MEM22
        case ADC12IV_ADC12IFG23:  break;    // Vector 58:  ADC12MEM23
        case ADC12IV_ADC12IFG24:  break;    // Vector 60:  ADC12MEM24
        case ADC12IV_ADC12IFG25:  break;    // Vector 62:  ADC12MEM25
        case ADC12IV_ADC12IFG26:  break;    // Vector 64:  ADC12MEM26
        case ADC12IV_ADC12IFG27:  break;    // Vector 66:  ADC12MEM27
        case ADC12IV_ADC12IFG28:  break;    // Vector 68:  ADC12MEM28
        case ADC12IV_ADC12IFG29:  break;    // Vector 70:  ADC12MEM29
        case ADC12IV_ADC12IFG30:  break;    // Vector 72:  ADC12MEM30
        case ADC12IV_ADC12IFG31:  break;    // Vector 74:  ADC12MEM31
        case ADC12IV_ADC12RDYIFG: break;    // Vector 76:  ADC12RDY
        default: break;
    }
}
