#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h> 
#include <unistd.h> 

#define BUFF_SIZE         8192
#define CHANNEL_NUMBER    1
#define SAMPLING_RATE     320000
#define BITS_PER_SAMPLE   8

#define ENABLE_BROADCAST  1
#define DISABLE_BROADCAST 0

struct Soundcard{
  int audio_fd; // file handler
  int channels; // number of channels
  int rate; // sampling rate
  int bps; // bits per sample
  unsigned char buffer[BUFF_SIZE]; // audio buffer
};

struct Broadcast{
  char *broadcast_address;
  int port_number;
  int server_socket;
  struct sockaddr_in server_addr;
  int broadcast_enable; // to enable Broadcast
};

int SoundcardOpen(void){
  int file;
  if((file = open("/dev/dsp", O_RDONLY, 0)) < 0){
    puts("/dev/dsp open error\n");
    exit(0);
  }
  return file;
}

void SoundcardInitialize(struct Soundcard *this,
                         uint8_t channels, uint32_t rate, uint8_t bps){
  
  this->audio_fd = SoundcardOpen();
  this->channels = channels;
  this->rate = rate;
  this->bps = bps;

}

struct Soundcard *SoundcardCreate(uint8_t channels, uint32_t rate, uint8_t bps){
  struct Soundcard *newSoundcard = (struct Soundcard *)malloc(sizeof(struct Soundcard));
  
  SoundcardInitialize(newSoundcard, channels, rate, bps);
  
  return newSoundcard;
}

void SoundcardSet(struct Soundcard *this){
  if(ioctl(this->audio_fd, SOUND_PCM_WRITE_BITS, &(this->bps)) == -1){
    puts("Could not set size\n");
    exit(0);
  }
  if(ioctl(this->audio_fd, SOUND_PCM_WRITE_CHANNELS, &(this->channels)) == -1){
    puts("Could not set channels\n");
    exit(0);
  }
  if(ioctl(this->audio_fd, SOUND_PCM_WRITE_RATE, &(this->rate))==-1)
    error("Could not set rate");
}

void BroadcastCheckArg(uint32_t n, char *broadcast_address){
  struct sockaddr_in sa;
  if(n != 3){
    puts("Arguments are not correct. <Broadcast address> <Port number>\n");
    exit(0);
  }
  if(inet_pton(AF_INET, broadcast_address, &(sa.sin_addr)) != 1){
    puts("IP format is not correct\n");
    exit(0);
  }
}

int BroadcastAssignSocket(){
  int server_socket;
  if(server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) == -1 ){
    puts("Server socket error\n");
    exit(0);
  }
  printf("server socket: %d\n", server_socket);
  return server_socket;
}

void BroadcastSetServerAddr(struct sockaddr_in * serv_addr,
                            char *broadcast_address, 
                            uint32_t broadcast_port_number){

  memset(serv_addr, 0, sizeof(struct sockaddr_in));
  serv_addr->sin_family = AF_INET;
  serv_addr->sin_addr.s_addr = inet_addr(broadcast_address);
  serv_addr->sin_port = htons(broadcast_port_number);

}

/*struct sockaddr_in *BroadcastCreateServerAddr(){
  struct sockaddr_in *newSocketAddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
  return newSocketAddr;
}*/

void BroadcastInitialize(struct Broadcast *this, 
                         char *broadcast_address,
                         char *broadcast_port_number){

  this->broadcast_address = broadcast_address;
  this->port_number = atoi(broadcast_port_number);
  //this->server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //BroadcastAssignSocket(); //
  if((this->server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1){// = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //BroadcastAssignSocket(); //
    puts("socket error");
    exit(0);
  }
  //this->server_addr = BroadcastCreateServerAddr();
  //BroadcastSetServerAddr(&(this->server_addr), broadcast_address, this->port_number);
  
  
}

void BroadcastEnable(struct Broadcast *this){
  const int so_bc = 1;
  if((setsockopt(this->server_socket, SOL_SOCKET, SO_BROADCAST, &so_bc, sizeof(so_bc)))==-1){
    //printf("%d %d %d %p %d\n", this->server_socket, SOL_SOCKET, SO_BROADCAST, &so_bc, sizeof(so_bc));
    puts("Could not set socket options");
    exit(0);
  }
}

struct Broadcast *BroadcastCreate(char *broadcast_address, 
                                  char *broadcast_port_number){

  struct Broadcast *newBroadcast = (struct Broadcast*)malloc(sizeof(struct Broadcast));
  BroadcastInitialize(newBroadcast, broadcast_address, broadcast_port_number);
  return newBroadcast;
}





int main(int argc, char * argv[]){
  struct Soundcard *soundcard;
  struct Broadcast *broadcast;
  const int so_bc = 1;
  struct sockaddr_in *newSocketAddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));


  BroadcastCheckArg(argc, argv[1]);
  broadcast = BroadcastCreate(argv[1], argv[2]);

  //.............
  /*
  broadcast->broadcast_address = argv[1];
  broadcast->port_number = atoi(argv[2]);
  if((broadcast->server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1){// = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //BroadcastAssignSocket(); //
    puts("socket error");
    exit(0);
  }
  */
  memset((char*)&(broadcast->server_addr), 0, sizeof(broadcast->server_addr));             //Zero out structure
  broadcast->server_addr.sin_family = AF_INET;                              //For Internet use
  broadcast->server_addr.sin_port = htons(broadcast->port_number);                     //Broadcast port
  broadcast->server_addr.sin_addr.s_addr = inet_addr(broadcast->broadcast_address);    //Broadcast IP-address

  if((setsockopt(broadcast->server_socket, SOL_SOCKET, SO_BROADCAST, &so_bc, sizeof(so_bc)))==-1){
    puts("Could not set sock options");
    exit(0);
  }
  
  soundcard = SoundcardCreate(CHANNEL_NUMBER, SAMPLING_RATE, BITS_PER_SAMPLE);
  SoundcardSet(soundcard);
  puts("before BroadcastEnable");
  BroadcastEnable(broadcast);
  
  puts("while");
  while(1){
    if((read(soundcard->audio_fd, soundcard->buffer, BUFF_SIZE)) == -1){
      puts("Read /dev/dsp error\n");
      exit(0);
    }
    puts("hello");
    if((sendto(broadcast->server_socket, soundcard->buffer, strlen(soundcard->buffer), 0, (struct sockaddr*)&(broadcast->server_addr), sizeof(struct sockaddr)))==-1){
      puts("sendto error\n");
      exit(0);
    }
  }





  return 0;
}