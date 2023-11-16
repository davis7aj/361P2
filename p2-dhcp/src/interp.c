#include <stdlib.h>

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "dhcp.h"
#include "format.h"

int
main (int argc, char **argv)
{
  // TODO: Open the file specified on the command line
  // Note: The file contains a binary network packet
  int file = open(argv[1], O_RDONLY);

  if (file == -1) {
    perror("couldnt open file");
    return 1;
  }

  // Open the file
    FILE *filePointer = fopen(argv[1], "r"); 

    if (filePointer == NULL) {
        perror("Error opening file");
        return 1;
    }

  // TODO: Determine the size of the packet in bytes
  // Note: You have used the fstat() function before

  struct stat packBuf;
  if (fstat(file, &packBuf) == -1)
  {
    perror("couldnt get file info");
    close(file);
    return 1;
  }

  // TODO: Define a uint8_t * variable
  uint8_t *dhcp;

  // TODO: Allocate enough space to hold the packet (store in your varaible)
  dhcp = (uint8_t *)malloc(packBuf.st_size);

  // TODO: Read the packet data from the file into your variable
  ssize_t nbRead = read(file, dhcp, packBuf.st_size);
  if (nbRead ==-1)
  {
    perror("err reading fle");
    free(dhcp);
    close(file);
    return 1;
  }
  
  // msg_t dhcpMsg;

  //copy contents to struct
  // if (memcpy(&dhcpMsg,  packBuf, sizeof(packBuf)) != 0)
  // {
  //   /* code */
  //   fprintf(stderr, "ERROR cpy data to struct using memcpy_s");
  //   return 1;
  // }
  // memcpy(&dhcpMsg,  packBuf, fileStat.st_size);
  

  // TODO: Call dump_msg() from format.c to print the packet data to stdout
  dump_msg(filePointer, (msg_t *)dhcp, packBuf.st_size);
  // TODO: Don't forget to free the dynamically-allocated space

  close(file);
  fclose(filePointer);
  free(dhcp);
  return EXIT_SUCCESS;
}
