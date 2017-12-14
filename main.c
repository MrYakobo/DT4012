/*SAM3X*/
#include "system_sam3x.h"
#include "at91sam3x8.h"
#include "core_cm3.h"
#include "stdlib.h"

#include "screen.h"
#include "delay.h"
#include "temperature.h"
#include "keypad.h"
#include "light.h"
#include "servo.h"
#include "handlers.h"
#include "led.h"

#include "encode.h"

#include "test.h"
#include <time.h>
#include <float.h>
#include <math.h>

// #include <stdio.h>

struct Entry {
    double min;
    double max;
    double average;
} entry;

/*GLOBALS:*/
int FAST_MODE = 1;
int currentMode = 0;

#define DB_MAX 10080
#define SUMMARY_MAX 7 //day entries

char db[DB_MAX]; //storage of temperature each minute
struct Entry summary[SUMMARY_MAX]; //derived from db: contains summary of each day
unsigned indx = 0;

void print(char str[]){
    printString(str);
}

void printMenu(){
    clearScreen();
    print("Welcome to the Weather Station 0.1\nChoose one of the following:\n1) Record data\n2) View logged data\n3) Find the sun\n4) Alarm at high/low temperatures\n5) Toggle modes (current mode: ");
    print(FAST_MODE ? "FAST)  " : "NORMAL)");
}

//waits until user inputs something
int pollInput(void){
    int val = func();
    while(val == 0){
        val = func();
    } //here, user has pressed some button
    return val;
}
//waits until user inputs desired button
void pollButton(int btn){
    int val = func();
    while(val == 0 || val != btn){
        val = func();
    } //here, val is btn
}


double inputDouble(){
    print("*: Negative input\t#: Positive input:\n");
    int signInput = 0;
    while(signInput != 10 && signInput != 12){
        signInput = pollInput();
    }
    delay_ms(3000);
    //0 : 1 => 0 : 2 => -1 : 1
    int sign = ((signInput & (1<<2))>>1)-1;
    print("Ok! You chose ");
    print(sign == 1 ? "positive" : "negative");
    print(" input.\n*: Set decimal place\t#: Done\n\n");
    int arr[4] = {-1, -1, -1, -1};
    
    int decimalPlace = -1; //index, börjar på 0
    int index = 0;

    print(sign == 1 ? "" : "-");

    while(index <= 4){
        int input = pollInput();
        if(input == 12){
            delay_ms(2000);
            break;
        }
        else if(input == 11){
            arr[index] = 0;
            print("0");
        }
        else if(input == 10){
          if(decimalPlace == -1){
              decimalPlace = index;
              print(".");
          }
          continue;
        }
        else{
            arr[index] = input;
            printNumber(input);
        }
        delay_ms(2000);
        index++;
    }
    if(index == 0){ //user didn't input anything, probably want to exit
        return -1.0;
    }

    /*
        [1,2,5,0]
        decimalPlace: 3

        [9,NULL,NULL,NULL]
        decimalPlace: 0
    */
    double total = 0.0;

    decimalPlace = decimalPlace > -1 ? decimalPlace : index;

    for(int i = 0; i < 4; i++){
        if(arr[i] != -1){
            total += arr[i]*pow(10.0, decimalPlace-i-1);
        }
    }
    
    return total*sign;
}

double average_range(int low, int high){
    double sum = 0.0;
    double total = 0.0;

    for(int i = low; i < high; i++){
        if(db[i] != NULL){
            sum += decode(db[i]);
            total++;
        }
    }
    return sum/total;
}

void chooseMode(){
  int input = 6;
  while(input > 5)
    input = pollInput();
  currentMode = input;
}

void mainMenu(){
    clearScreen();
    printMenu();
    chooseMode();
}

void printLogHeader(){
    print("\nDay | Min  | Max  | Avg\n");
    print("------------------------\n");
}

void viewDayLog(int i){
    print("  ");
    printNumber(i+1);
    print(" | ");
    printDouble(summary[i].min);
    print("| ");
    printDouble(summary[i].max);
    print("|");
    printDouble(summary[i].average);
    print("|\n");
}

void viewWeekLog(){
    clearScreen();
    printLogHeader();
    for(int i = 0; i < SUMMARY_MAX; i++){
        if(summary[i].average == 0){
            break;
        }
        viewDayLog(i);
    }

    print("\nPress 0 to exit");
    delay_ms(1000); //debouncing from previous zero-click
    pollButton(11);
}


void recordData(){
    clearScreen();

    print("Measuring temperature each ");
    print(FAST_MODE ? "second" : "minute");
    print(".\nPress 0 to stop measure.\n\n");
    spinner_begin();

    int TIME = FAST_MODE ? 1 : 60;
    
    int DAY_MAX = 1440;
    int CONTINUE = 1;

    int dayIndex = 0;


    int FILL_INSTANTLY = 0;
    char FILL[] = "OPSQRT";

    printLogHeader();

    if(FILL_INSTANTLY){
        for(int i = 0; i < DB_MAX; i++){
            db[i] = FILL[i%6];
            summary[dayIndex].min = 15.15;
            summary[dayIndex].max = 22.0;

            if((i+1)%DAY_MAX==0){ //if a "day" has passed, calculate avg
                summary[dayIndex].average = average_range(dayIndex*DAY_MAX, (dayIndex+1)*DAY_MAX);
                viewDayLog(dayIndex);
                dayIndex++;
            }
        }
    }
    
    else{
    while(CONTINUE && indx < DB_MAX && func() != 11){ //press 0 to exit
        int t = time(0);
       
        double temp = getTemperature();
        db[indx] = encode(temp);

        //note min/max
        if(temp < summary[dayIndex].min){
            if(temp < 17){
                printf("Nu e de kallt. %.2f grader, index %d", temp, indx);
            }
            summary[dayIndex].min = temp;
        }
        else if(temp > summary[dayIndex].max){
            if(temp > 23){
                printf("Nu e de varmt. %.2f grader, index %d", temp, indx);
            }
            summary[dayIndex].max = temp;
        }
  
        if((indx+1)%DAY_MAX==0){ //if a "day" has passed, calculate avg
            summary[dayIndex].average = average_range(dayIndex*DAY_MAX, (dayIndex+1)*DAY_MAX);
            viewDayLog(dayIndex);
            dayIndex++;
        }

        while((time(0)-t) < TIME){ //waits for time to pass by
          if(func() == 11){ //if user presses button, exit
            CONTINUE = 0;
            break;
          }
          delay_ms(400);
          spinner();
        }
        
        indx++;
    }
    spinner_end();
    }
    //indx = 0;

    print("Measure finished.\nPress 0 to exit.");
    delay_ms(1000);
    pollButton(11);
}

void findSun(){
    clearScreen();
    print("Trying to find sun...");

    double delta;
    while(delta > 0.1){
        delta = lightDelta(); //difference between lightLeft and lightRight, given as a number between -1 and 1
        turnServo(delta*60.0);
    }
    char *str;
    sprintf(str, "Found sun! It was at %d degrees. Press 0 to exit.", delta*60);
    print(str);
    
    pollButton(11);
}

void alarm(){
    clearScreen();
    print("Set max temperature:\n");
    double max = inputDouble();
    // double max = 22.0;

    print("\nSet min temperature:\n");
    double min = inputDouble();
    // double min = -10.0;

    // print("\nOk! The LED will flash violently when the temperature reaches any of max or min. Press 0 to exit.");
    clearScreen();

    print("Max: ");
    printDouble(max);
    print("\tMin: ");
    printDouble(min);

    print("\nPress 0 to exit.");

    print("\nCurrent temperature: ");
    while(func() != 11) { //user pressed 0
        double temp = getTemperature();
        print_double_persistent(temp);
        if(temp > max || temp < min){
            decrementPointer(21);
            if(temp > max){
                print("Max");
            }
            else{
                print("Min");
            }
            print(" temperature detected! Press 0 to restart.");
            while(func() != 11){
                ledToggle();
                delay_ms(1000);
            }

            ledOff();
            decrementPointer(59);
            print("\nCurrent temperature: ");
            delay_ms(4000);
        }
        delay_ms(1000);
    }
}

void initSummary(){
  for(int i = 0; i < SUMMARY_MAX; i++){
    summary[i].min = DBL_MAX;
    summary[i].max = -DBL_MAX;
    summary[i].average = 0;
  }
}

void main(void){
    SystemInit();

    *AT91C_PMC_PCER = 0x8007800;      //Clock pin TC0,PIOA,PIOB,PIOC,PIOD
    
    // initServo();

    // initLight();

    initSummary();
    initTemperature();
    //testDB();
    
    initScreen();
    clearScreen();

    initKeypad();
    initLight();

    ledInit();

    // inputDouble();

    while(1){
        switch(currentMode){
            case 0:
                mainMenu();
                break;
            case 1:
                recordData();
                currentMode = 0;
                break;
            case 2:
                viewWeekLog();
                currentMode = 0;
                break;
            case 3:
                findSun();
                currentMode = 0;
                break;
            case 4:
                alarm();
                currentMode = 0;
                break;
            case 5:
                decrementPointer(FAST_MODE ? 7 : 7);
                FAST_MODE = FAST_MODE^1;
                print(FAST_MODE ?  "FAST)  ":"NORMAL)");
                chooseMode();
                break;
        }
    }
}