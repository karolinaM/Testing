/*
 * Library: wavfile.c
 *
 * Karolina Majstrovic
 * 2016
 *
 */


#include "../inc/wavfile.h"

struct RoundBuffer{
  char buffer[1024];
  unsigned int buf_in;
  unsigned int buf_out;
}RBuf;

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
  union Data{
    char csize[4];
    unsigned int uisize;
  }usize;
  unsigned long int size;
  char character;
  
  // seeks for subchunk size --> little endian
  lseek(fd, 40, 0);
  if(read(fd, &usize, 4) != 4)
    printf("not successful\n");
  printf("datasize: %d %d\n", usize.csize, __bswap_32(usize.uisize));
  size = __bswap_32(usize.uisize);
  
  RBuf.buf_in = RBuf.buf_out = 0;
  
  //reads subchunk data --> little endian
  while(RBuf.buf_in < 511){
    read(fd, &character, 1);
    RBuf.buffer[RBuf.buf_in++] = character;
    size--;
  }

  // TODO: enable SPI interupt
  // TODO: enable interrupts
  // TODO: activate WM8731

  while(size > 0){
    if(RBuf.buf_in != RBuf.buf_out){
      read(fd, &character, 1);
      RBuf.buffer[RBuf.buf_in++] = character;
      if(RBuf.buf_in == 1023)
        RBuf.buf_in = 0;
      size--;
    }
  }
  // TODO: deactivate WM8731
}

