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

class GymPlayer
{
 public:
  GymPlayer();
  ~GymPlayer();

  bool SetupSocket(const std::string& multicastGroup, uint16_t port);
  bool SetupAudioInput();
  bool FeedData();
  


 private:
  int MulticastSocket;
  struct sockaddr_in MulticastAddr;
  int AudioDevice;

  enum {
    BUFSIZE = 1428
  };
  char Buffer[BUFSIZE];
};
