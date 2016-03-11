#include <AudioPlayer.h>

AudioPlayer::AudioPlayer()
  : Socket(-1),
    AudioDevice(-1)
{

}

AudioPlayer::~AudioPlayer()
{
  if (AudioDevice != -1) {
    close(AudioDevice);
  }
  if (Socket != -1) {
    close(Socket);
  }
}

bool AudioPlayer::SetupSocket(uint16_t port)
{
  Socket = socket(AF_INET, SOCK_DGRAM, 0);
  if (Socket < 0) {
    printf("Failed to open socket: %s", strerror(errno));
    return false;
  }

  const int enable = 1;
  if ((setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, &enable,
		  sizeof(enable))) < 0) {
    printf("Failed to set SO_REUSEADDR: %s", strerror(errno));
    return false;
  }

  // const int so_bc = 1;
  // if (setsockopt(Socket, SOL_SOCKET, SO_BROADCAST, &so_bc,
  // 		 sizeof(so_bc)) < 0) {
  //   printf("Failed to set broadcast");
  // }

  struct sockaddr_in socketAddr;
  memset(&socketAddr, 0, sizeof(socketAddr));
  socketAddr.sin_family = AF_INET;
  socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  socketAddr.sin_port = htons(port);

  if (bind(Socket, (struct sockaddr*)&socketAddr, sizeof(socketAddr)) < 0) {
    printf("Failed to bind: %s\n", strerror(errno));
    return false;
  }

  return true;
}

bool AudioPlayer::SetupAudioOutput()
{
  AudioDevice = open("/dev/dsp", O_WRONLY);

  // const int sampleRate = 16000;
  // const int bitsPerSample = 8;
  // const int numChannels = 1;
  // if ((ioctl(AudioDevice, SOUND_PCM_WRITE_BITS, &bitsPerSample)) < 0) {
  //   error("error SIZE");
  //   return false;
  // }
  // if ((ioctl(AudioDevice, SOUND_PCM_WRITE_CHANNELS, &numChannels)) < 0) {
  //   error("error CHANNELS");
  //   return false;
  // }
  // if ((ioctl(AudioDevice, SOUND_PCM_WRITE_RATE, &sampleRate)) < 0) {
  //   error("error RATE");
  //   return false;
  // }
  return true;
}

bool AudioPlayer::FeedData()
{
  printf("Entering FeedData\n");
  int bytesRead = 0;
  bytesRead = recv(Socket, Buffer, BUFSIZE, 0);
  if (bytesRead < 0) {
    printf("Failed to recvfrom: %s", strerror(errno));
    return false;
  }
  printf("read %d bytes from broadcast\n", bytesRead);
  if (AudioDevice != -1) {
    bytesRead = write(AudioDevice, Buffer, bytesRead);
    if (bytesRead < 0) {
      printf("Failed to write to AudioDevice: %s", strerror(errno));
      return false;
    }
  }
  printf("Exiting FeedData\n");
  return true;
}
