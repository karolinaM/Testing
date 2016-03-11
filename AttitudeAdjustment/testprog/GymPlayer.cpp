#include <GymPlayer.h>

GymPlayer::GymPlayer()
  : MulticastSocket(-1),
    AudioDevice(-1)
{

}

GymPlayer::~GymPlayer()
{
  if (AudioDevice != -1) {
    close(AudioDevice);
  }
  if (MulticastSocket != -1) {
    close(MulticastSocket);
  }
}

bool GymPlayer::SetupSocket(const std::string& multicastGroup, uint16_t port)
{
  MulticastSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (MulticastSocket < 0) {
    printf("Failed to open socket: %s", strerror(errno));
    return false;
  }

  const int so_bc = 1;
  if (setsockopt(MulticastSocket, SOL_SOCKET, SO_BROADCAST, &so_bc,
		 sizeof(so_bc)) < 0) {
    printf("Failed to set broadcast");
  }

  memset(&MulticastAddr, 0, sizeof(MulticastAddr));
  MulticastAddr.sin_family = AF_INET;
  MulticastAddr.sin_addr.s_addr = inet_addr(multicastGroup.c_str());
  MulticastAddr.sin_port = htons(port);
  return true;
}

bool GymPlayer::SetupAudioInput()
{
  AudioDevice = open("/dev/dsp", O_RDWR);

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

bool GymPlayer::FeedData()
{
  int bytesRead = 0;
  if (AudioDevice != -1) {
    bytesRead = read(AudioDevice, Buffer, BUFSIZE);
    if (bytesRead < 0) {
      printf("Failed to read from AudioDevice: %s", strerror(errno));
      return false;
    }
  }
  printf("read %d bytes from audio device\n", bytesRead);
  int bytesWritten = sendto(MulticastSocket,
  			    Buffer,
  			    bytesRead,
  			    0,
  			    (struct sockaddr *)&MulticastAddr,
  			    sizeof(MulticastAddr));

  if (bytesWritten < 0) {
    printf("Failed to sendto: %s", strerror(errno));
    return false;
  }

  // if (AudioDevice != -1) {
  //   int bytesWritten = write(AudioDevice, Buffer, bytesRead);
  //   if (bytesWritten < 0) {
  //     printf("Failed to write to AudioDevice: %s", strerror(errno));
  //     return false;
  //   }
  // }

  return true;
}
