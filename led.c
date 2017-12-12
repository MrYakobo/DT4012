#include "system_sam3x.h"
#include "at91sam3x8.h"
#include "core_cm3.h"
#include "stdlib.h"

void ledOn(void){
    *AT91C_PIOD_SODR = (1<<3);
}

void ledOff(void){
    *AT91C_PIOD_CODR = (1<<3);
}

int isOn = 0;

void ledToggle(void){
    if(isOn) ledOff();
    else ledOn();
    isOn = isOn^1;
}

void ledInit(void){
    *AT91C_PIOD_PER = (1<<3);  //LED
    *AT91C_PIOD_OER = (1<<3);
    *AT91C_PIOD_PPUDR = (1<<3);
}