#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

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

int main(int argc, char *argv[]){
  int gpio = atoi(argv[1]);  
  int direction = atoi(argv[2]);  
  unsigned int i;
  
  gpioExport(gpio); 
  gpioDirection(gpio, direction);
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
