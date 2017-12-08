#include "system_sam3x.h"
#include "at91sam3x8.h"
#include "core_cm3.h"
#include "stdlib.h"

#include "delay.h"
#include "light.h"

double temp = 0.0;

void TC0_Handler(void){
    delay(1000); //behövs?

    double number = *AT91C_TC0_RB - *AT91C_TC0_RA;
    temp = ((number/210)-273.15)-3;        // -3 cuz margin of error
    *AT91C_TC0_IDR = (1<<6); //disable interrupts (LDRBS)
    *AT91C_TC0_SR; //read status register to reset interrupt
   //*AT91C_TC0_IER = (1<<6); //enable again
}

int lightValue = 0;

void ADC_Handler(void){
  *AT91C_ADCC_IDR = (1<<24);
  *AT91C_ADCC_SR; //read status register
  ADC_READY = 1;
  int light1 = *AT91C_ADCC_CDR1;
  int light2 = *AT91C_ADCC_CDR2;
  
  lightValue = light2 - light1;
}
