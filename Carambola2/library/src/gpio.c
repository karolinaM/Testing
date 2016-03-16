/*
 * Library: gpio.c
 *
 * Karolina Majstrovic
 * 2016
 *
 */

#include "../inc/gpio.h"

void gpioExport(int gpio){
  int fd;
  char buf[255];

  fd = open("/sys/class/gpio/export", O_WRONLY);
  
  sprintf(buf, "%d", gpio);
  write(fd, buf, strlen(buf));

  close(fd);
}

int gpioDirection(int gpio, int direction){
  int fd;
  char buf[255];

  sprintf(buf, "/sys/class/gpio/gpio%d/direction", gpio);
  fd = open(buf, O_WRONLY);
  /* out != 0, in = 0 */ 
  if(direction){
    if(write(fd, "out", 3) != 3){
      printf("Direction is not set\n");
      exit(0);
    }
  }
  else{
    if(write(fd, "in", 2) != 2){
      printf("Direction is not set\n");
      exit(0);
    }
  }
 
  close(fd);
  return 1;
}

void gpioSet(int gpio, int value){
  int fd;
  char buf[255];
  
  sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
  fd = open(buf, O_WRONLY);
  
  sprintf(buf, "%d", value);
  write(fd, buf, 1);
  
  close(fd);
}

char gpioRead(int gpio){
  int fd;
  char buf[255];
  sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
  fd = open(buf, O_RDONLY);
   
  read(fd, buf, 1);
  /*printf("Read: %d\n", buf[0]);*/ 
  close(fd);
  return buf[0];
}
/*
void gpioInterruptConfig(int gpio){
  int fd;
  char buf[255];
  sprintf(buf, "/sys/class/gpio/gpio%d/uevent", gpio);
  fd = open(buf, O_WRONLY);
  write(fd, buf, "rising");
  fclose(fd);
}
*/
