#ifndef LIGHT
#define LIGHT

void initLight(void);
void pollLight(void);
double lightLeft(void);
double lightRight(void);
double lightDelta(void);
void lightMeasure(void);

extern int ADC_READY;

#endif