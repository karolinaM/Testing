/*
 * Library: wavfile.c
 *
 * Karolina Majstrovic
 * 2016
 *
 */


#include "../inc/wavfile.h"

void WavInfo(int fd){
  char buf[4];

  printf("using lseek...\n");
  lseek(fd, 4, SEEK_CUR);

  printf("lseek: ");
  lseek(fd, 21, 0); 

  printf("read number of channels: ");
  if(read(fd, &buf[0], 2) != 2){
    printf("not successful\n");
  }
  printf("0x%x 0x%x\n", buf[0], buf[1]);

  lseek(fd, 24, 0);
  if(read(fd, &buf[0], 4) != 4)
    printf("not successful\n");
  printf("sample rate: 0x%x 0x%x 0x%x 0x%x\n", 0xFF&buf[0], 0xFF&buf[1], 0xFF&buf[2], 0xFF&buf[3]);
  
  lseek(fd, 34, 0);
  if(read(fd, &buf[0], 2) != 2)
    printf("not successful\n");
  printf("bits/sample: 0x%x 0x%x\n", 0xFF&buf[0], 0xFF&buf[1]);
}

void WavPlay(int fd){
  unsigned long int tmp;
  char TmpString[11];
  char TmepString1[22];
  char character;

 
}

