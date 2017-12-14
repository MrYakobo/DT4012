#include "system_sam3x.h"
#include "at91sam3x8.h"
#include "core_cm3.h"
#include "stdlib.h"

#include "delay.h"
#include "handlers.h"

int inited = 0;

void initTemperature(void){
  *AT91C_PIOB_PER = (1<<25);//Enable pin25
  *AT91C_PIOB_OER = (1<<25);
  *AT91C_PIOB_SODR = (1<<25);
  //*AT91C_PIOB_ODR = (1<<25);
  
  *AT91C_TC0_CMR = (*AT91C_TC0_CMR & 0xFFFFFFF8); //Select Timer_Clock1 as TCCLK (set bit 0,1,2 to zero)
  *AT91C_TC0_CCR  = 0x5; //Enable counter and make a sw_reset in TC0_CCR0 (bit 0)
  *AT91C_TC0_CMR  = (*AT91C_TC0_CMR | AT91C_TC_LDRA_FALLING); //Load counter to A when TIOA falling
  *AT91C_TC0_CMR  = (*AT91C_TC0_CMR | AT91C_TC_LDRB_RISING); //Load counter to B when TIOA rising

  //Enable the interrupt (NVIC) with the inline declared function
  NVIC_ClearPendingIRQ(TC0_IRQn);
  NVIC_SetPriority(TC0_IRQn, 1);
  NVIC_EnableIRQ(TC0_IRQn);
  
  //Set burst to none
  //*(AT91C_BASE_TC0+AT91C_TC_BURST) = AT91C_TC_BURST_NONE;
  inited = 1;
}

void measureTemp(void){
    if(!inited){
      printf("Du har glomt att kora initTemperature() din not");
    }
    else{
   *AT91C_TC0_IER = (1<<6); //Enable interrupts on LDRBS

   //PULSE
   *AT91C_PIOB_OER = (1<<25);
   *AT91C_PIOB_CODR = (1<<25); //make a pulse with clear/set
   delay(25);
   *AT91C_PIOB_SODR = (1<<25);
   delay(50);
   *AT91C_PIOB_CODR = (1<<25); //make a pulse with clear/set
   delay(50);
   *AT91C_PIOB_SODR = (1<<25);
  
   *AT91C_PIOB_ODR = (1<<25);
   delay(25);
   //END PULSE

   delay(4000000); //typ 1.5 sekunder
   *AT91C_TC0_IER = (1<<6); //Enable interrupts on LDRBS (again)
   *AT91C_TC0_CCR  = AT91C_TC_SWTRG; //make a sw_reset in TC0_CCR0
    }
}

double getTemperature(void){
    measureTemp();
    return temp;
}