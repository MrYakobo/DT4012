#include "system_sam3x.h"
#include "at91sam3x8.h"
#include "core_cm3.h"
#include "stdlib.h"

#include <math.h>

/*
void initLight(void){
  *AT91C_PMC_PCER1 = (1<<5); //PMC på adc

  *AT91C_PIOA_PER = (1<<3);
  *AT91C_PIOA_PER = (1<<4);

  *AT91C_ADCC_IER = (1<<24);
  *AT91C_ADCC_MR = (*AT91C_ADCC_MR & 0xff00^1) | (2<<8); //clear 15-8, set 2<<8

  
//   NVIC_EnableIRQ(ADC_IRQn);
//   NVIC_SetPriority(ADC_IRQn, 1);
//   NVIC_ClearPendingIRQ(ADC_IRQn);
//  *AT91C_ADCC_IER = (0x3)<<1; //jeppe
}
*/

int ADC_READY = 0;

void initLight(){
	
	// *AT91C_PMC_PCER = (1 << 11); // PMC för PIOA
	*AT91C_PMC_PCER1 = (1 << 5); // PMC för ADC
	
	*AT91C_ADCC_MR = (0x2 << 8);//Sets the prescal value to the value 2.
	*AT91C_ADCC_CWR = (1 << 24); //sets TAG bit to store value in LDCR
	
	NVIC_ClearPendingIRQ(ADC_IRQn);                
	NVIC_SetPriority(ADC_IRQn, 1);
	NVIC_EnableIRQ(ADC_IRQn);
}

int LIGHT_A = 0;
int LIGHT_B = 0;

void lightMeasureRight(){
    ADC_READY = 0;
	*AT91C_ADCC_CHER = (0x2);
	*AT91C_ADCC_CR = (2);
	*AT91C_ADCC_IER = (1 << 24);
	
	while(ADC_READY == 0){}
	ADC_READY = 0;
	*AT91C_ADCC_CHDR = (0x2);
	*AT91C_ADCC_IDR = (1 << 24);
	LIGHT_A = *AT91C_ADCC_LCDR & 0xfff;
}

void lightMeasureLeft(){
    ADC_READY = 0;
	*AT91C_ADCC_CHER = (0x4);
	*AT91C_ADCC_CR = (2);
	*AT91C_ADCC_IER = (1 << 24);
	
	while(ADC_READY == 0){}
	ADC_READY = 0;
	*AT91C_ADCC_CHDR = (0x4);
	*AT91C_ADCC_IDR = (1 << 24);
	LIGHT_B = *AT91C_ADCC_LCDR & 0xfff;
}

void lightMeasure(){
    // delay(4000);
    lightMeasureLeft();
    // delay(84);
    lightMeasureRight();
}

void pollLight(void){
  //Desperat försök för att få igång ljussensorn
  *AT91C_ADCC_CHER = (1<<1); //select channel 1
  *AT91C_ADCC_CHER = (1<<2); //select channel 1
  *AT91C_ADCC_CR = 2;
  
  int value = 0;
  
  while(value == 0){
      value = *AT91C_ADCC_LCDR;
  }
  printf("%x", value);
}


int lightLeft(void){
    return LIGHT_A;
}

int lightRight(void){
    return LIGHT_B;
}