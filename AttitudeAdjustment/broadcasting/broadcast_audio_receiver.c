/*********************************************************
* Broadcast Audio Receiver                               *
* A program that receives audio from a broadcast server. *
*                                                        *
* Usage: Program name <UDP Port>                         *
*                                                        * 
* Name: broadcast_audio_receiver.c                       *
* By: Andreas Fredriksson & Magnus Brenning              *
* KTH-Haninge (2001-05-03)                               *
*********************************************************/ 

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/soundcard.h>

#define error(txt){puts(txt);exit(0);}

#define BUF_SIZE 8129  //Size of buffer to receive

#define RATE 32000     //Sampling rate
#define SIZE 8         //Bits per sample (8/16)
#define CHANNELS 1     //Mono=1 Stereo=2

//Creates a socket for UDP and sets the socket for broadcast.
//Open and sets parameters for the audio device.
//Receives packets and plays them.
int main(int argc, char *argv[])
{
  int sound_q;
  int port_number;
  int client_socket;
  int audio_fd;
  unsigned char audio_buffer[BUF_SIZE];
  struct sockaddr_in cli_addr;
  const int so_bc = 1;

  if(argc !=2)
  {
    puts("Usage: Program name <Portnumber>");
    exit(0);   
  }

  port_number= atoi(argv[1]);
  
  //Create a datagram socket using UDP
  if((client_socket = socket(AF_INET, SOCK_DGRAM, 0))<0)
    error("Socket error");

  memset((char*)&cli_addr, 0, sizeof(cli_addr));     //Zero out structure
  cli_addr.sin_family = AF_INET;                     //For Internet use
  cli_addr.sin_port = htons(port_number);            //Broadcast port 
  cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);      //Any incoming interface

  //Set options to enable broadcast
  if(setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &so_bc, sizeof(so_bc))<0)
    error("Setsockopt error");

  //Bind the address to the socket
  if(bind(client_socket, (struct sockaddr*)&cli_addr, sizeof(cli_addr))<0)
    error("Bind error");

  //Opens /dev/dsp for write only
  if((audio_fd=open("/dev/dsp", O_WRONLY, 0))<0)
    error("Could not open audio device");

  //Setting sample parameters to selected value
  sound_q=SIZE;
  ioctl(audio_fd, SOUND_PCM_WRITE_BITS, &sound_q);
  sound_q=CHANNELS;
  ioctl(audio_fd, SOUND_PCM_WRITE_CHANNELS, &sound_q);
  sound_q=RATE;
  ioctl(audio_fd, SOUND_PCM_WRITE_RATE, &sound_q);
    
  puts("Starting to receive");
  
  while(1)
  {
    //Receiving datagram by datagram from the sender
    if((recv(client_socket, audio_buffer, sizeof(audio_buffer), 0))<0) 
      error("Recvfrom error");

    //Playing audio to speakers
    if(write(audio_fd, audio_buffer, BUF_SIZE)<0)
      error("Write error");
  }
  
  close(client_socket);
}







