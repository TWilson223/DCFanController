/*
 * ADC related functions/definitions
 *
 */

#ifndef adc_h
#define adc_h

#define MIN_ADC_CHANNEL 4
#define MAX_ADC_CHANNEL 8
#define NUM_ADC_CHANNEL 5
#define MAX_RES_THERMAL 17874
#define MIN_RES_THERMAL 8495

void adcInitialize(void);
void adcStart(void);
void adcStop(void);
void adcRead(void);
uint16_t thermSearch(uint16_t x);

#endif
