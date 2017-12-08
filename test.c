#include "keypad.h"
#include "screen.h"
#include "temperature.h"

void testKeypadOnScreen(void){
  initKeypad();
  int res;
  for(int i = 0; i < 3000; i++){
    res = func();
  }
 
  if(res != 0){
    int p = 0x10+res;
    
    if(res == 10){
      p = 0x0A;
    }
    else if(res == 11){
      p = 0x10;
    }
    else if(res == 12){
      p = 0x03;
    }
    print(p);
  }
}

void testServoWithKeypad(void){
  initKeypad();
  int res;
  for(int i = 0; i < 1000; i++){
    res = func();
  }
  if(res != 0){
    turnServo(res*10);
  }
}

void testKeypad(){
  int old = 0;
  while(1){
    int val = func();
    if(val != old){
        printf("%d", val);
    }
    old = val;
  }
}

void testScreen(void){
  printString("tjenis penis");
}

void testTemperature(void){
  printf("%f\n", getTemperature());
}