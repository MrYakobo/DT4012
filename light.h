#ifndef LIGHT
#define LIGHT

void initLight(void);
void pollLight(void);
int lightLeft(void);
int lightRight(void);
double lightDelta(void);
void lightMeasure(void);
void lightMeasureLeft(void);
void lightMeasureRight(void);

extern int ADC_READY;

#endif