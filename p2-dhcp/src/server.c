#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#include "dhcp.h"
#include "format.h"
#include "port_utils.h"

static bool get_args (int, char **);

static bool debug = false;

int
main (int argc, char **argv)
{
  debug = get_args (argc, argv);
  msg_t msg;
  memset(&msg, 0, sizeof(msg));
  int socketfd = socket (AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = (uint16_t) strtol(get_port(), (char **)NULL, 10);
  address.sin_addr.s_addr = INADDR_ANY;
  int rc = bind(socketfd, (struct sockaddr *) &address, sizeof(address));
  if (rc < 0) {
    printf("error errno : %d\n", errno);
    EXIT_FAILURE;
  }
  socklen_t len = 0;
  recvfrom(socketfd, &msg, sizeof(msg), 0 , (struct sockaddr *)&address, &len);
  if (msg.xid == 0)
  {
    printf("\n++++++++++++++++\n");
    printf("SERVER RECEIVED:\n");
    dump_msg(stdout, &msg, sizeof(msg));
    printf("++++++++++++++++\n");
    close(socketfd);
    return EXIT_SUCCESS;
  }

  if (msg.options[6] == DHCPDISCOVER)
  {
    msg.options[6] = DHCPOFFER;
    msg.op = BOOTREPLY;
    memcpy(&msg.yiaddr, &msg.options[10], sizeof(uint32_t));
    sendto(socketfd, &msg, sizeof(msg), 0, (struct sockaddr *) &address, sizeof(address));
  } else {
    printf("error errno : %d\n", errno);
    EXIT_FAILURE;
  }
  close(socketfd);
  if (debug)
    fprintf (stderr, "Shutting down\n");

  return EXIT_SUCCESS;
}

static bool
get_args (int argc, char **argv)
{
  int ch = 0;
  while ((ch = getopt (argc, argv, "dh")) != -1)
    {
      switch (ch)
        {
        case 'd':
          debug = true;
          break;
        default:
          return false;
        }
    }
  return true;
}
