/*
 * Project: i2c_simple
 *
 * Karolina Majstrovic
 * 2016
 *
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//#include "../../library/inc/gpio.h"
#include "../../library/inc/wavfile.h"
#include <inttypes.h>
#include <linux/i2c-dev.h>

int main(int argc, char *argv[]){
  int fd;
  if((fd = open("audio.wav", O_RDONLY)) == -1){
    printf("not able to open the file\n");
    return -1;
  };
  
  WavInfo(fd);
  WavInitI2C();
 
  WavPlay(fd);
  
  close(fd);
  return 0;

}
