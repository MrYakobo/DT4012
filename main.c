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
    print(FAST_MODE ? "FAST" : "NORMAL");
    print(")");
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

/*
double inputDouble2(){
    print("*: Set decimal place\t#: Done\n");
    char *str;
    int decimalPlace = 0; //flag to make sure that double decimal places are not allowed

    while(1){ //while input != #
        int input = pollInput();
        if(input == 12){break;}
        else if(input == 11){
            sprintf(str, "%s%s", str, "0");
            print("0");
        }
        else if(input == 10 && !decimalPlace){
            sprintf(str, "%s%s", ".");
            print(".");
            decimalPlace = 1;
        }
        else {
            char c = input+0x30;;
            char *pChar = malloc(sizeof(char));
            *pChar = c;
            sprintf(str, "%s%s", c);
            print(pChar);
        }
    }

    double d;
    sscanf(str, "%lf", &d);
    return d;
}*/

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
    print(" input.\n");
    print("*: Set decimal place\t#: Done\n\n");
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
            if(decimalPlace > -1)
                continue;
            decimalPlace = index;
            print(".");
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

void mainMenu(){
    clearScreen();
    printMenu();
    currentMode = pollInput();
}

void viewLog(){
    clearScreen();
    print("\n");
    print("Day | Min  | Max  | Avg\n");
    print("----------------------\n");

    for(int i = 0; i < SUMMARY_MAX; i++){
        if(summary[i].average == 0){
            break;
        }
    
        print("  ");
        printNumber(i+1);
        print(" | ");
        printDouble(summary[i].max);
        print("| ");
        printDouble(summary[i].min);
        print("|");
        printDouble(summary[i].average);
        print("|\n");
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
    int DAY_MAX = FAST_MODE ? 60 : 1440;
    int LOOP = 1;

    int dayIndex = 0;

    while(LOOP && indx < DB_MAX && func() != 11){ //press 0 to exit
        int t = time(0);
       
        double temp = getTemperature();
        db[indx] = encode(temp);

        //note min/max
        if(temp < summary[dayIndex].min){
            summary[dayIndex].min = temp;
        }
        else if(temp > summary[dayIndex].max){
            summary[dayIndex].max = temp;
        }
  
        if((indx+1)%DAY_MAX==0){ //if a "day" has passed, calculate avg
            double sum = 0;
            double total = 0;
            for(int i = dayIndex*DAY_MAX; i < (dayIndex+1)*DAY_MAX; i++){
                if(db[i] != NULL){
                    sum += decode(db[i]);
                    total++;
                }
            }
            summary[dayIndex].average = sum/total;
            dayIndex++;
        }

        while((time(0)-t) < TIME){ //waits for time to pass by
          if(func() == 11){ //if user presses button, exit
            LOOP = 0;
            break;
          }
          delay_ms(400);
          spinner();
        }
        
        indx++;
    }
    
    //indx = 0;
    spinner_end();
    print("Measure finished.\n");
    viewLog();
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
    
    pollButton(0);
}

void alarm(){
    clearScreen();
    print("Set max temperature:\n");
    // double max = inputDouble();
    double max = 22.0;

    print("\nSet min temperature:\n");
    // double min = inputDouble();
    double min = -10.0;

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
            print(" temperature detected!\nPress 0 to restart.");
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
                viewLog();
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
                FAST_MODE = FAST_MODE^1;
                currentMode = 0;
                break;
        }
    }
}