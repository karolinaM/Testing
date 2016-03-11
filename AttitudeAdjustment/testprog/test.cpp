#include <algorithm>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


int main()
{
  printf("Hello world!\n");

  int dspFile = open("/dev/dsp", O_RDONLY);
  int outFile = open("/tmp/audio_data", O_WRONLY | O_CREAT);
  
  const int BUFSIZE = 10000;
  char buffer[BUFSIZE];
  int bytesRead = read(dspFile, buffer, BUFSIZE);

  printf("Read %d bytes. %s\n", bytesRead, strerror(errno));

  int bytesWritten = write(outFile, buffer, std::max(BUFSIZE, bytesRead));

  printf("Wrote %d bytes. %s\n", bytesWritten, strerror(errno));
}
