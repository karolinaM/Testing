#include "AudioPlayer.h"

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

static bool keepGoing = true;

void sig_handler(int signo)
{
  if (signo == SIGINT) {
    printf("received SIGINT\n");
    keepGoing = false;
  }
}

int main()
{
  printf("Hello world!\n");

  // if (signal(SIGINT, sig_handler) == SIG_ERR) {
  //   printf("can't catch SIGINT\n");
  // }

  AudioPlayer player;

  player.SetupSocket(11111);
  player.SetupAudioOutput();

  while (keepGoing) {
    player.FeedData();
  }
  printf("Goodbye world!\n");
}
