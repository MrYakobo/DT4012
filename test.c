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

void testEncoding(void){
   double test[] = {29.22, 117.0, 29.25, 11.0, 44.0, 11.0, 29.20, 117.0, 29.25};

  for(int i = 0; i < sizeof(test)/sizeof(test[0]); i+=3){
      char enc = encode(test[i]);
      double dec = decode(enc);

      if((double)enc != test[i+1]){
        printf("Encoding of %f failed: Expected %d, got %d\n", test[i], (char)test[i+1], enc);
      }
      else if(dec != test[i+2]){
        printf("Decoding failed: Expected %f, got %f\n", test[i+2], dec);
      }
  }
}

void statistik(void){
    double rmax = -100000.0;
    double rmin = 1000000.0;

    double lmax = -10000.0;
    double lmin = 10000.0;
    //rmax: 2483.000000	rmin: 1684.000000lmax: 2047.000000	lmin: 1791.000000
    //lmax: 2047.000000	lmin: 1791.000000	rmax: 2165.000000	rmin: 1929.000000
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

        printf("lmax: %f\tlmin: %f\trmax: %f\trmin: %f\n", lmax, lmin, rmax, rmin);
    }
}

void testLinebreak(){
  printString("Rad 1\nRad 2\nRad 3");
  printString("\n");
  printString("\n");
  printString("Rad 5\n");
  printString("looooooooooooooooooooooooooooooooooooooooooong word\nand another under");
  print("\n");
  print("Day | Min  | Max | Avg\n");
  print("-------------------\n");
  print(" ");
  printNumber(1);
  print(" | ");
  printDouble(22.52);
  print("| ");
  printDouble(22.60);
  print("| ");
  printDouble(10.19);
  print("| ");

}