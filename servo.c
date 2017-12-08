#include "system_sam3x.h"
#include "at91sam3x8.h"
#include "core_cm3.h"
#include "stdlib.h"

int LEFT = 2000; //0.7/20*255 -60 deg
int NEUTRAL = 4000; //1.5/20*255 0 deg
int RIGHT = 6000; ///2.3/20*255 60 deg

void initServo(void){
  *AT91C_PMC_PCER1 = 0X10; //PWM ENABLE
  *AT91C_PIOB_PDR = 1<<17; 
  *AT91C_PIOB_ABMR = 1<<17;
  //*AT91C_PIOB_PPUDR = 1<<17;
  *AT91C_PWMC_ENA = AT91C_PWMC_CHID1;
  *AT91C_PWMC_CH1_CMR = (*AT91C_PWMC_CH1_CMR & 0xF^1) | 0x5; //PWMC in CHID1 to Master_Clk/32
  *AT91C_PWMC_CH1_CPRDR = 52500; //Sets the CPRD to 52500 for the formula 32*CPRD/MCK 
  //Neutral position: Channel Duty = 1.5ms
  *AT91C_PWMC_CH1_CDTYR = LEFT;
}

void turnServo(double degree){
  *AT91C_PWMC_CH1_CDTYUPDR = (int)(2000+2000/60*degree);
}