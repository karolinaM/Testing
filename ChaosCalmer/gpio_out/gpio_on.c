#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>


void gpioExport(int gpio){
  int fd;
  char buf[255];

  fd = open("/sys/class/gpio/export", O_WRONLY);
  
  sprintf(buf, "%d", gpio);
  write(fd, buf, strlen(buf));

  close(fd);
}


void gpioDirection(int gpio, int direction){
  int fd;
  char buf[255];

  sprintf(buf, "/sys/class/gpio%d/direction", gpio);
  fd = open(buf, O_WRONLY);
  
  if(direction){
    write(fd, "out", 3);
  }
  else{
    write(fd, "in", 2);
  }
 
  close(fd);
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
  
  printf("GPIO pin state: %c\n", buf[0]);

  close(fd);
  return buf[0];
}

int main(){
  int gpio = 14;
  int direction = 1; 
  int value = -1; 
  unsigned int i;

  printf("gpioExport\n");
  gpioExport(gpio);
  printf("gpioDirection\n");
  gpioDirection(gpio, direction);
  while(1){
    if(value != 0){
      gpioSet(gpio, 1);
      printf("gpioSet %d\n", 1);
    }
    else{
      gpioSet(gpio, 0);
      printf("gpioSet %d\n", 0);
    }
    for(i=0; i<9000000; ++i);
    value = ~value;
  }
}
