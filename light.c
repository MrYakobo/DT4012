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

void lightMeasure(){
     ADC_READY = 0;
	*AT91C_ADCC_IER = (1 << 24);
	*AT91C_ADCC_CHER = (0x2);
	*AT91C_ADCC_CR = (2);
	
	while(ADC_READY == 0){}
	ADC_READY = 0;
	*AT91C_ADCC_CHDR = (0x2);
	LIGHT_A = *AT91C_ADCC_LCDR & 0xfff;
	
	*AT91C_ADCC_IER = (1 << 24);
	*AT91C_ADCC_CHER = (0x4);
	*AT91C_ADCC_CR = (2);
	
	while(ADC_READY == 0){}
	ADC_READY = 0;
	*AT91C_ADCC_CHDR = (0x4);
	LIGHT_B = *AT91C_ADCC_LCDR & 0xfff;
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


//lmax: 2047.000000	lmin: 1791.000000
double lightLeft(void){
    //gör lite fancy ADC_Handler stuffs här med globala variabler
    return 1.0-((LIGHT_A-1791.0)/256.0);
}

//rmax: 2483.000000	rmin: 1684.000000
double lightRight(void){
    //samma sak här
    return (LIGHT_B-1684.0)/799.0;
}

double lightDelta(){
    lightMeasure();
    // double l = lightLeft();
    // double r = lightRight();
    // return (l-r)/sqrt(l*l+r*r); //divide by length to get a normalized value between 1 and -1
    // return (LIGHT_B-LIGHT_A)/sqrt(pow(LIGHT_A,2)+pow(LIGHT_B,2));
    return lightRight()-lightLeft();
}