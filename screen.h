#ifndef SCREEN
#define SCREEN

void initScreen(void);
void Init_Display_2(void);

void printString(char str[]);
void print_ascii(int h);
void resetPointer(void);
void clearScreen(void);

void spinner();
void spinner_end();
void spinner_begin();

void printDouble(double d);
void printNumber(int n);
void print_double_persistent(double d);
void decrementPointer(int n);

#endif