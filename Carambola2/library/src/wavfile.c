/*
 * Library: wavfile.c
 *
 * Karolina Majstrovic
 * 2016
 *
 */


#include "../inc/wavfile.h"
#include <linux/i2c-dev.h>

struct RoundBuffer{
  char buffer[1024];
  unsigned int buf_in;
  unsigned int buf_out;
}RBuf;

void WM8731Cmd(int addr, unsigned int cmd){
  char buf[20];
  int fI2cOut;
  union Command{
    unsigned int ui_cmd_full;
    char c_cmd_full[3];
  }command;
  unsigned int cmd_full;
  
  sprintf(buf, "/dev/i2c-0");
  fI2cOut = open(buf, O_RDWR);
  
  command.ui_cmd_full = addr << 1;
  command.ui_cmd_full = command.ui_cmd_full | (((0xFF00 & cmd) >> 8) & 0x01);
  command.ui_cmd_full = (command.ui_cmd_full << 8) |  (0xFF & cmd);
  command.ui_cmd_full = WM8731_ADDRESS << 16 | command.ui_cmd_full;
 
  if(ioctl(fI2cOut, I2C_SLAVE, command.c_cmd_full[1]) < 0){
    printf("something went wrong \n");
  }

  write(fI2cOut, &command.c_cmd_full[2], 2);
  //buf[0] = 0xff;
  //buf[1] = 0xff;
  //write(fI2cOut, &buf[0], 2);

  close(fI2cOut);

  printf("cmd: %x %x %x\n", 0xFF&command.c_cmd_full[1], 0xFF&command.c_cmd_full[2], 0xFF&command.c_cmd_full[3]);
}

void WavInitI2C(){ 
  WM8731Cmd(WM8731_REG_RESET, _WM8731_Reset);               // Reset module
  WM8731Cmd(WM8731_REG_LLINE_IN, _WM8731_left_lineIn);      // Left line in settings
  WM8731Cmd(WM8731_REG_RLINE_IN, _WM8731_Right_lineIn);     // Rigth line in settings
  WM8731Cmd(WM8731_REG_LHPHONE_OUT, _WM8731_Left_hp);       // Left headphone out settings
  WM8731Cmd(WM8731_REG_RHPHONE_OUT, _WM8731_Right_hp);      // Right headphone out settings
  WM8731Cmd(WM8731_REG_ANALOG_PATH, _WM8731_AnalogAudio);   // Analog paths
  WM8731Cmd(WM8731_REG_DIGITAL_PATH, _WM8731_DigitalAudio); // Digital paths
  WM8731Cmd(WM8731_REG_PDOWN_CTRL, _WM8731_power);          // Power down control
  WM8731Cmd(WM8731_REG_DIGITAL_IF, _WM8731_DAIF);           // Digital interface
  WM8731Cmd(WM8731_REG_SAMPLING_CTRL, _WM8731_Sampling);    // Sampling control
}

void WavPlay(int fd){
  char data[4];
  unsigned long int size;
  char character;
  int fSpiOut;
  char buf[20];
  
  sprintf(buf, "/dev/spidev1.0");
  fSpiOut = open(buf, O_WRONLY);
  WM8731Cmd(WM8731_REG_ACTIVE_CTRL, _WM8731_Activate);

  // seeks for subchunk size --> little endian
  lseek(fd, WAV_SUB_CHUNK_SIZE, 0);
  if(read(fd, &data[0], 4) != 4)
    printf("not successful\n");
  size = __bswap_32(ConvertToInt(&data[0],4));
  printf("datasize: %d\n", size);
  
  lseek(fd, WAV_DATA, 0);
  // writing 16bit data
  while(size > 0){
    read(fd, &buf[0], 2);
    //read(fd, &buf[0], 4);
    write(fSpiOut, &buf[0], 2);
    //write(fSpiOut, &buf[2], 2);
    //size -= 4;
    size -= 2;
  }
  /*
   * Only for testing if activate and deactivate are working without outputting noise
  for(size = 0; size < 99999999; ++size)
    printf("%d", size);
  printf("\n");
  */
  WM8731Cmd(WM8731_REG_ACTIVE_CTRL, _WM8731_Deactivate);
  close(fSpiOut);

}

unsigned int ConvertToInt(char *buf, int n){
  unsigned int tmp = 0;
  unsigned int i;

  if(n > 4 || n < 0){
    printf("number n is smaller than 0 or bigger than 4\nERROR!\n");
    return -1;
  }

  for(i = 0; i < n; ++i){
    tmp = (tmp << 8) | (0xFF & *(buf+i));  
  }


  return tmp;
}

void WavInfo(int fd){
  char buf[4];

  printf("File size: ");
  lseek(fd, WAV_SIZE, SEEK_CUR);
  read(fd, &buf[0], 4);
  printf("%d bytes\n", __bswap_32(ConvertToInt(&buf[0], 4)));

  printf("Format: ");
  lseek(fd, WAV_FORMAT, 0); 
  read(fd, &buf[0], 4);
  printf("%c%c%c%c\n", buf[0], buf[1], buf[2], buf[3]);

  printf("Number of channels: ");
  lseek(fd, WAV_NUMBER_CHANNELS, 0);
  read(fd, &buf[0], 2);
  printf("%d\n", __bswap_16(ConvertToInt(&buf[0], 2)));
  
  printf("Sample rate: ");
  lseek(fd, WAV_SAMPLE_RATE, 0);
  read(fd, &buf[0], 4);
  printf("%d\n", __bswap_32(ConvertToInt(&buf[0], 4)));
 
  printf("Block align: ");
  lseek(fd, WAV_BLOCK_ALIGN, 0);
  read(fd, &buf[0], 2);
  printf("%d\n", __bswap_16(ConvertToInt(&buf[0], 2)));
 
  printf("Bits per sample: ");
  lseek(fd, WAV_BITS_PER_SAMPLE, 0);
  read(fd, &buf[0], 2);
  printf("%d\n", __bswap_16(ConvertToInt(&buf[0], 2)));
 
  printf("Sub chunk size: ");
  lseek(fd, WAV_SUB_CHUNK_SIZE, 0);
  read(fd, &buf[0], 4);
  printf("%d\n", __bswap_32(ConvertToInt(&buf[0], 4)));

}




