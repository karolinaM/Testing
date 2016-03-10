#include "../../library/inc/gpio.h"

int main(int argc, char *argv[]){
  int gpio = atoi(argv[1]);  
  int direction = atoi(argv[2]);  
  unsigned int i;
  
  gpioExport(gpio); 
  gpioDirection(gpio, direction);
    
  while(1){
    printf("Read: %c\n", gpioRead(gpio));

    for(i=0; i<9000000; ++i);
  }  
  return 0;
}
