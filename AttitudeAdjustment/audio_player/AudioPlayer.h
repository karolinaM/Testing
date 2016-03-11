#include <algorithm>
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <inttypes.h>
#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <unistd.h>

class AudioPlayer
{
 public:
  AudioPlayer();
  ~AudioPlayer();

  bool SetupSocket(uint16_t port);
  bool SetupAudioOutput();
  bool FeedData();
  
 private:
  int Socket;
  int AudioDevice;

  enum {
    BUFSIZE = 1428
  };
  char Buffer[BUFSIZE];
};
