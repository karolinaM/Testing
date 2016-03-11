/*
 * Project: gpio_out
 *
 * Karolina Majstrovic
 * 2016
 *
 */

#include "../../library/inc/gpio.h"

int main(int argc, char *argv[]){
  int gpio = atoi(argv[1]);  
  unsigned int i;
  
  gpioExport(gpio); 
  gpioDirection(gpio, gpioDIRECTION_OUT);
  gpioSet(gpio, 1);
  
  while(1){
    if(gpioRead(gpio) != '1'){
      gpioSet(gpio, 1);
      printf("gpioSet %d\n", 1);
    }
    else{
      gpioSet(gpio, 0);
      printf("gpioSet %d\n", 0);
    }
    for(i=0; i<9000000; ++i);
  }
  
  return 0;
}
