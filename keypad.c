#include "system_sam3x.h"
#include "at91sam3x8.h"
#include "core_cm3.h"
#include "stdlib.h"

void initKeypad(void){
    //ROWS
    *AT91C_PIOC_PER = (1<<2); //Pin 34
    *AT91C_PIOC_PER = (1<<3); //Pin 35
    *AT91C_PIOC_PER = (1<<4); //Pin 36
    *AT91C_PIOC_PER = (1<<5); //Pin 37
    //COL
    *AT91C_PIOC_PER = (1<<7); //Pin 39
    *AT91C_PIOC_PER = (1<<8); //Pin 40
    *AT91C_PIOC_PER = (1<<9); //Pin 41
    
    *AT91C_PIOC_OER = (1<<7);
    *AT91C_PIOC_OER = (1<<8);
    *AT91C_PIOC_OER = (1<<9);
    
    *AT91C_PIOD_PER = (1<<2);
    *AT91C_PIOD_OER = (1<<2);
    *AT91C_PIOD_CODR = (1<<2);
}

int func(void){
  initKeypad();
  
  int COLS[] = {9,8,7};
  int ROWS[] = {2,3,4,5};

  
  *AT91C_PIOC_SODR = 0x380; //Set 9,8,7
  
  int val = 0;
  for(int i =0; i<3;i++){
    int Value;
    *AT91C_PIOC_CODR = (1<<COLS[i]); //Clear col
    for(int k = 0; k < 100; k++){
      Value = *AT91C_PIOC_PDSR; //Read
    }

     for(int j =0;j<4;j++){
       if((Value & (1<<ROWS[j]))==0){ //if this iterational button is pressed
         //*AT91C_PIOC_ODR = 0x3F8; //disable 9,8,7,6,5,4
         val = j*3+i+1; //return value
       }
     }
     *AT91C_PIOC_SODR = (1<<COLS[i]); //set Col again
  }
  //*AT91C_PIOC_ODR = 0x3F8; //if no button pressed, return 0
  return val;
}
