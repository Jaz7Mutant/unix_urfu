#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#define fileSize 4 * 1024 * 1024 + 1

int main(int argc, char *argv[])
{
  int fd = open("A", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
  if (fd == -1)
  {
    perror("Error while opening file 'A'");
    exit(-1);
  }
  char *buffer;
  buffer = (char *)calloc(fileSize, 1);
  if (buffer == NULL)
  {
    perror("Error while alocating buffer with 'calloc'");
    exit(-1);
  }
  buffer[0] = 1;
  buffer[10000] = 1;
  buffer[fileSize - 1] = 1;
  int result = write(fd, buffer, fileSize);
  if (result == -1)
  {
    perror("Error while writing buffer to a file 'A'");
    exit(-1);
  }
  result = close(fd);
  if (result == -1)
  {
    perror("Error while closing file 'A'");
  }
  free(buffer);
  return 0;
}