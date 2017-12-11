#include <stdio.h>
#include <math.h>

char encode(double t){
    int i = (int)t;
    int d = round((t-(double)i)*4);
    return (i<<2) + d;
}

double decode(char t){
  double i = t>>2;
  double d = (t&3)/4.0;
  return i+d;
}