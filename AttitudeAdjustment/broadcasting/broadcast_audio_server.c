/********************************************************
* Broadcast Audio Server                                *
* Broadcasting audio from microphone using /dev/dsp.    *
*                                                       *
* Usage: Program name <Broadcast address> <Portnumber>  *
*                                                       *
* Name: broadcast_audio_server.c                        *
* By: Magnus Brenning & Andreas Fredriksson             *
* KTH-Haninge (2000-05-03)                              *
********************************************************/

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/soundcard.h>

#define error(txt) {puts(txt); exit(0);}

#define BUF_SIZE 8192   //Size of buffer to send

#define RATE 32000      //Sampling rate
#define SIZE 8          //Bits per sample (8/16)
#define CHANNELS 1      //Mono=1 Stereo=2

//Creates a socket for UDP and sets the socket for broadcast.
//Open and sets parameters for the audio device.
//Record from the microphone and sends out packets.
int main(int argc, char* argv[])
{
  int sound_q;
  int port_number;
  int server_socket;
  int audio_fd;
  unsigned char audio_buffer[BUF_SIZE];
  struct sockaddr_in serv_addr;
  const int so_bc = 1;                     //To enable broadcast
  char* broadcast_address;

  if(argc != 3)
  {
    puts("Usage: Program name <Broadcast address> <Portnumber>");
    exit(0);
  }
  
  broadcast_address = argv[1];
  port_number = atoi(argv[2]);

  //Create a datagram socket using UDP
  if((server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
    error("Socket error");

  memset((char*)&serv_addr, 0, sizeof(serv_addr));             //Zero out structure
  serv_addr.sin_family = AF_INET;                              //For Internet use
  serv_addr.sin_port = htons(port_number);                     //Broadcast port
  serv_addr.sin_addr.s_addr = inet_addr(broadcast_address);    //Broadcast IP-address

  //Set options to enable broadcast
  if((setsockopt(server_socket, SOL_SOCKET, SO_BROADCAST, &so_bc,sizeof(so_bc)))==-1)
    error("Could not set sock options");
  
  //Opens /dev/dsp for read only
  if((audio_fd=open("/dev/dsp", O_RDONLY, 0))==-1)
    error("open /dev/dsp/ error");
 
  //Setting sample parameters to selected value
  sound_q=SIZE;
  if(ioctl(audio_fd, SOUND_PCM_WRITE_BITS, &sound_q)==-1)
    error("Could not set size");
  sound_q=CHANNELS;
  if(ioctl(audio_fd, SOUND_PCM_WRITE_CHANNELS, &sound_q)==-1)
    error("Could not set channels");
  sound_q=RATE;
  if(ioctl(audio_fd, SOUND_PCM_WRITE_RATE, &sound_q)==-1)
    error("Could not set rate");

  puts("Starting to record and send");

  while(1)
  {
    //Recording audio from microphone
    if((read(audio_fd, audio_buffer,BUF_SIZE))==-1) 
      error("read /dev/dsp/ error");

    //Sending datagram by datagram to the broadcast-address
    if((sendto(server_socket, audio_buffer, strlen(audio_buffer), 0, (struct sockaddr*)&serv_addr, (sizeof serv_addr)))==-1)
      error("sendto error");
  }
  
  close(server_socket);
}

















