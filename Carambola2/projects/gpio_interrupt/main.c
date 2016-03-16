/*
 * Project: gpio_interrupt
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
  gpioDirection(gpio, gpioDIRECTION_IN);
    
  while(1){
    printf("Read: %c\n", gpioRead(gpio));
    for(i=0; i<9000000; ++i);
  }  
  return 0;
}
