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

#include "test.h"

// #include <stdio.h>

struct Entry {
    double min;
    double max;
    double average;
} entry;

/*GLOBALS:*/
int FAST_MODE = 0;
int currentMode = 0;
#define DB_MAX 10080
#define SUMMARY_MAX 7 //day entries

int db[DB_MAX]; //storage of temperature each minute
struct Entry summary[SUMMARY_MAX]; //derived from db: contains summary of each day
unsigned indx = 0;

void print(char str[]){
    printString(str);
}

void printMenu(){
    clearScreen();
    print("Welcome to the Weather Station 0.1\nChoose one of the following:\n1) Record data\n2) View logged data\n3) Find the sun\n4) Alarm at high/low temperatures\n5) Toggle fast mode");
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
    print("*: Set decimal place    #: Done\n");
    char *str;
    int decimalPlace = 0; //flag to make sure that double decimal places are not allowed

    while(1){ //while input != #
        int input = pollInput();
        if(input == 12){break;}
        else if(input == 11){
            sprintf(str, "%s%s", "0");
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
}


void mainMenu(){
    printMenu();
    currentMode = pollInput();
}

void recordData(){
    clearScreen();
    char *str;
    sprintf(str, "Measuring temperature each %s%s", FAST_MODE ? "second" : "minute", ". Press 0 to exit.");
    print(str);

    while(indx < DB_MAX){
        double temp = getTemperature();
        db[indx] = temp;
        indx++;
        delay(1000*60); //one minute
    }
}

void viewLog(){
    for(int i = 0; i < SUMMARY_MAX; i++){
        char *str;
        sprintf(str, "Day %d: Minimum: %l - Max: %l - Average: %l\n", i+1, summary[i].min, summary[i].max, summary[i].average);
        print(str);
    }
    print("Press 0 to exit");
    pollButton(0);
    currentMode = 0;
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
    currentMode = 0;
}

void alarm(){
    clearScreen();
    print("Set max temperature: ");
    double max = inputDouble();
    printNumber(max);

    print("Set min temperature: ");
    double min = inputDouble();
    printNumber(min);

    double temp = 0.0;
    print("\nOk! The LED will flash violently when the temperature reaches any of max or min. Press 0 to exit.");

    while(func() != 11) { //user pressed 0
        temp = getTemperature();
        if(temp > max || temp < min){
            ledToggle();
            delay(100);
        }
    }
}

void testDB(void){
    for(int i = 0; i < DB_MAX; i++){
        db[i] = i;
    }
}

void statistik(void){

    double rmax = -100000.0;
    double rmin = 1000000.0;

    double lmax = -10000.0;
    double lmin = 10000.0;
//rmax: 2483.000000	rmin: 1684.000000lmax: 2047.000000	lmin: 1791.000000
while(1){
      lightMeasure();
      double a = lightLeft();
      double b = lightRight();

      if(a < lmin){
          lmin = a;
      }
      if(a > lmax){
          lmax = a;
      }
      if(b > rmax){
          rmax = b;
      }
      if(b < rmin){
          rmin = b;
      }

      printf("lmax: %f\tlmin: %f\nrmax: %f\trmin: %f", lmax, lmin, rmax, rmin);
}
}

void main(void){
    SystemInit();

    *AT91C_PMC_PCER = 0x8007800;      //Clock pin TC0,PIOA,PIOB,PIOC,PIOD
    
    /*
    ledInit();
    initScreen();
    initKeypad();
    initServo();
    initLight();
    initTemperature();

    clearScreen();
    */
    /*
    initKeypad();
    testKeypad();
    */

    // initScreen();
    // clearScreen();
    // testScreen();

    // initTemperature();
    //   testTemperature();

    //testDB();
    
    //initLight();

    while(1){
        lightMeasure();
        printf("%f\t%f\n", lightLeft(), lightRight());
    // printf("%f\n", lightDelta());
    //printf("-------\n");
      //delay_ms(10000);
    }
    
    while(1){
        switch(currentMode){
            case 0:
                mainMenu();
            break;
            case 1:
                recordData();
                break;
            case 2:
                viewLog();
                break;
            case 3:
                findSun();
                break;
            case 4:
                alarm();
                break;
            case 5:
                FAST_MODE = FAST_MODE^1;
                break;
        }
    }
}