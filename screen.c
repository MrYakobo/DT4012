#include "system_sam3x.h"
#include "at91sam3x8.h"
#include "core_cm3.h"
#include "stdlib.h"

#include <stdio.h>
#include <string.h>

#include "delay.h"

unsigned char Read_Status_Display(void){
    unsigned char Temp;

    //*AT91C_PIOC_CODR = 0x39000; //clear 12,17,16,15
    //*AT91C_PIOC_SODR = 0x2000; //set dir as input (13)

   *AT91C_PIOC_SODR = (1<<13); //set dir as input (1=input)
   *AT91C_PIOC_CODR = (1<<12); //clear/enable output (0=enable)
   *AT91C_PIOC_SODR = (1<<17);  //set C/D (ödet SODR vi CODR)
 
   *AT91C_PIOC_CODR = (1<<16);   //clear chip select (CE)
   *AT91C_PIOC_CODR = (1<<1);
   *AT91C_PIOC_CODR = (1<<15);    //clear read display (RD)
       
   delay(10);
   
   *AT91C_PIOC_ODR =0x3FC;
   
   for(int i = 0; i < 10; i++){
      Temp = *AT91C_PIOC_PDSR;
   }
   
    *AT91C_PIOC_SODR = (1<<16);    //set chip select (CE)
    *AT91C_PIOC_SODR = (1<<15);   //set read display (RD)
    *AT91C_PIOC_SODR = (1<<12); //disable output
    *AT91C_PIOC_CODR = (1<<13); //set dir as output
    return Temp>>2;
}
void Write_Command_2_Display(unsigned char Command){
   while((Read_Status_Display() & 3) != 3){
    int status = Read_Status_Display();
    printf("%x",status);
      //do nothing
  }

  *AT91C_PIOC_CODR = 0x3FC; //Clear Databus: set bit 2 to 9 = 0
  *AT91C_PIOC_SODR = Command<<2;  //Set Data to DataBus
  
  *AT91C_PIOC_CODR = (1<<13); //Set dir as output (output=0)
  *AT91C_PIOC_CODR = (1<<12); //Enable output (enable=0)

  //*AT91C_PIOC_OER = 0x37000; //enable output for 12,13,14,16,17
  *AT91C_PIOC_OER = 0x3FC;  //Set databus as output
  
  *AT91C_PIOC_CODR = (1<<13);
  //Enable output for bit 2 to 9
  *AT91C_PIOC_CODR = (1<<12);

  *AT91C_PIOC_SODR = (1<<17);  //clear C/D (ÖDET SODR vi CODR) (skillnaden mellan Command/Data)
  *AT91C_PIOC_CODR = (1<<16);  //clear chip select (CE)
  *AT91C_PIOC_CODR = (1<<14);  //clear write display WR
  //make a delay
  delay(20);

  *AT91C_PIOC_SODR = (1<<16);  //set chip enable display
  *AT91C_PIOC_SODR = (1<<14);   //set write display WR
  *AT91C_PIOC_SODR = (1<<12);   //disable output
  //make databus as input
  *AT91C_PIOC_ODR = 0x3FC;
}
void Write_Data_2_Display(unsigned char Data){
  while((Read_Status_Display() & 3) != 3){
    int status = Read_Status_Display();
    printf("%x",status);
      //do nothing
  }

  *AT91C_PIOC_CODR = 0x3FC; //Clear Databus: set bit 2 to 9 = 0
  *AT91C_PIOC_SODR = Data<<2;  //Set Data to DataBus
  
  *AT91C_PIOC_CODR = (1<<13); //Set dir as output (output=0)
  *AT91C_PIOC_CODR = (1<<12); //Enable output (enable=0)

  //*AT91C_PIOC_OER = 0x37000; //enable output for 12,13,14,16,17
  *AT91C_PIOC_OER = 0x3FC;  //Set databus as output

  *AT91C_PIOC_CODR = (1<<17);  //clear C/D
  *AT91C_PIOC_CODR = (1<<16);  //clear chip select (CE)
  *AT91C_PIOC_CODR = (1<<14);  //clear write display WR
  delay(20); //make a delay

  *AT91C_PIOC_SODR = (1<<16);  //set chip enable display
  *AT91C_PIOC_SODR = (1<<14);   //set write display WR
  *AT91C_PIOC_SODR = (1<<12);   //disable output

  *AT91C_PIOC_ODR = 0x3FC;  //make databus as input
}

void setPointer(int h){ //61 => x: 20, y: 2
  int x = h%40;
  int y = (h-x)/40;
  
  //h: x+y*40 => y = (h-x)/40, x: h%40

  Write_Data_2_Display(x);
  Write_Data_2_Display(y);
  Write_Command_2_Display(0x24);
}

void resetPointer(void){
    setPointer(0);
}
void initScreen(){
  *AT91C_PIOD_PER = 1;
  *AT91C_PIOD_CODR = 1;  //Clear Reset Display
  *AT91C_PIOD_PPUDR = 1;
  *AT91C_PIOD_OER = 1;
  
  //Enable all relevant pins on PIOC
  *AT91C_PIOC_PER = 0xFF3FC;  //bit2-9 (input to screen DB0, DB1,...)

  *AT91C_PIOC_PPUDR = 0x3F3FC;
  //Enable output for control-pins
  *AT91C_PIOC_OER = 0x3F000;
  
  delay(1000);  //make a delay
  *AT91C_PIOD_SODR = (1<<0);  //set reset display

  Write_Data_2_Display(0x00);
  Write_Data_2_Display(0x00);
  Write_Command_2_Display(0x40);//Set text home address
  Write_Data_2_Display(0x00);
  Write_Data_2_Display(0x40);
  Write_Command_2_Display(0x42); //Set graphic home address
  //Write_Data_2_Display(0x1e);
  Write_Data_2_Display(0x28); //SOV-VIET-UNG-IGEN
  Write_Data_2_Display(0x00);
  Write_Command_2_Display(0x41); // Set text area
  //Write_Data_2_Display(0x1e);
  Write_Data_2_Display(0x28); //SOV-VIET-UNG-IGEN
  Write_Data_2_Display(0x00);
  Write_Command_2_Display(0x43); // Set graphic area
  Write_Command_2_Display(0x80); // text mode (128 pixels)
  Write_Command_2_Display(0x94); // Text on graphic off

    //Visar inversa färger
  *AT91C_PIOC_OER = 0x80000; 
  *AT91C_PIOC_CODR = 0x80000;
}
void Init_Display_2(){
    *AT91C_PIOD_PER = 1;
    *AT91C_PIOD_PPUDR =1;
    *AT91C_PIOD_OER = 1;
    //Enable all relevant pins on PIOC
    *AT91C_PIOC_PER = 0xFF3FC; //bit2-9 (input to screen DB0, DB1,...)
    *AT91C_PIOC_PPUDR = 0x3F3FC; //Enable output for control-pins
    *AT91C_PIOC_OER = 0x3F000; 
    Write_Data_2_Display(0x00); 
    Write_Data_2_Display(0x00); 
    Write_Command_2_Display(0x40); //Set text home address
    Write_Data_2_Display(0x00); 
    Write_Data_2_Display(0x40); 
    Write_Command_2_Display(0x42); 
    //Set graphic home address
    //Write_Data_2_Display(0x1e); 
    Write_Data_2_Display(0x28); 
    //SOV-VIET-UNG-IGEN
    Write_Data_2_Display(0x00); 
    Write_Command_2_Display(0x41); 
    // Set text area
    //Write_Data_2_Display(0x1e); 
    Write_Data_2_Display(0x28); 
    Write_Data_2_Display(0x00);     //SOV-VIET-UNG-IGEN
    Write_Command_2_Display(0x43); // Set graphic area
    Write_Command_2_Display(0x80); // text mode (128 pixels)
    Write_Command_2_Display(0x94); 
    // Text on graphic off //Visar inversa färger
    *AT91C_PIOC_OER = 0x80000; 
    *AT91C_PIOC_CODR = 0x80000;
}
void print_ascii(int h){
    Init_Display_2();
    Write_Data_2_Display(h);
    Write_Command_2_Display(0xC0);
}
void print_persistent(int h){
    Init_Display_2();
    Write_Data_2_Display(h-0x20);
    Write_Command_2_Display(0xC4);
}

int printedCharacters = 0;

void clearScreen(){
    resetPointer();
    for(int x = 0; x < 40; x++){
        for(int y = 0; y < 20; y++){
            print_ascii(0x0); //skriver ut mellanslag på hela skärmen
        }
    }
    printedCharacters = 0;
    resetPointer();
}

void printString(char str[]){
    //print to LCD screen
    int l = strlen(str);
    for(int i = 0; i < l; i++){
        if(str[i] == '\n'){
            int max = 40-(printedCharacters%40);
            for(int k = 0; k < max; k++){
                print_ascii(0);
                printedCharacters++;
            }
        }
        else if(str[i] == '\t'){
            for(int k = 0; k < 4; k++){
                print_ascii(0);
                printedCharacters++;
            }
        }
        else{
            char c = str[i]-0x20;
            print_ascii((int)c);
            printedCharacters++;
        }
    }
}

int SPINNER_STATE = 0;
char SPINNER_STR[] = "-\\|/";

void spinner_begin(){
    print(" "); //space
}

void spinner(){
    print_persistent(SPINNER_STR[SPINNER_STATE]);
    SPINNER_STATE = (SPINNER_STATE+1)%4;
}

void spinner_end(){ //prints space and decrements ADP
    Write_Data_2_Display(0);
    Write_Data_2_Display(0);
    Write_Command_2_Display(0xC2);
    printedCharacters--;
}

void printNumber(int n){ //OLD CODE: printNumber prints persistant
    char str[12];
    sprintf(str, "%d", n);
    printString(str);
}

void printDouble(double d){
    char str[5];
    sprintf(str, "%.2f", d);
    printString(str);
}

void decrementPointer(int n){
    for(int i = 0; i < n; i++) {
        Write_Command_2_Display(0xC3);
    }
    printedCharacters -= n;
}

void print_double_persistent(double d){
    printDouble(d);
    decrementPointer(5);
}

void printAxes(){
  for(int x = 0; x < 40; x++){
        printNumber(x%10);
  }
  for(int y = 0; y < 20; y++){
      printNumber((y+1)%10);
      print("\n");
  }
}