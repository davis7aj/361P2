#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "dhcp.h"
#include "format.h"
#include "port_utils.h"

static bool
get_args (int argc, char **argv)
{
  bool p = false;
  int socketfd;
  struct sockaddr_in address;
  uint32_t x = strtol("42", (char **)NULL, 10);
  int t = ETH;
  uint8_t c[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  int m = DHCPDISCOVER;
  struct in_addr s;
  inet_aton("127.0.0.1", &s);
  struct in_addr r;
  inet_aton("127.0.0.2", &r);
  int index = 1;
  msg_t msg;
  memset(&msg, 0, sizeof(msg));
  msg.op = m;
  msg.htype = t;
  msg.hlen = ETH_LEN;
  msg.xid = htonl(x);
  msg.ciaddr.s_addr = r.s_addr;
  msg.siaddr.s_addr = s.s_addr;
  memcpy(&msg.chaddr, &c, sizeof(c));
  while (index < argc)
  {
    if (strcmp(argv[index], "-x") == 0)
    {
      x = (uint32_t) strtol(argv[index + 1], (char **)NULL, 10);
      x = htonl(x);
      msg.xid = x;
      index += 2;
    }
    else if (strcmp(argv[index], "-t") == 0)
    {
      t = (int) strtol(argv[index + 1], (char **)NULL, 10);
      memcpy(&msg.htype, &t, sizeof(t));
      msg.hlen = (int) strtol(argv[index + 1], (char **)NULL, 10); ;
      index += 2;
    }
    else if (strcmp(argv[index], "-c") == 0)
    {
      for (int i = 0; i < strlen(argv[index]); i++)
      {
        c[i] = (uint8_t) strtol(argv[index + 1], (char **)NULL, 10);
      }
      memcpy(&msg.chaddr, &c, sizeof(c));
      index += 2;
    }
    else if (strcmp(argv[index], "-m") == 0)
    {
      m = (int) strtol(argv[index + 1], (char **)NULL, 10);
      msg.op = m;
      index += 2;
    }
    else if (strcmp(argv[index], "-s") == 0)
    {
      inet_aton(argv[index + 1], &s);
      msg.siaddr.s_addr = s.s_addr;
      index += 2;
    }
    else if (strcmp(argv[index], "-r") == 0)
    {
      inet_aton(argv[index + 1], &r);
      msg.ciaddr.s_addr = r.s_addr;
      index += 2;
    }
    else if (strcmp(argv[index], "-p") == 0)
    {
      x = htonl(0); //remove after
      memcpy(&msg.xid, &x, sizeof(x)); //remove after
      socketfd = socket (AF_INET, SOCK_DGRAM, 0);
      memset(&address, 0, sizeof(address));
      address.sin_family = AF_INET;
      address.sin_port = (uint16_t) strtol(get_port(), (char **)NULL, 10);
      address.sin_addr.s_addr = s.s_addr;
      // dump_msg(stdout, &msg, sizeof(msg));
      p = true;
      index += 1;
    }
    else
    {
      printf("Error encountered at : %s\n", argv[index]);
      return false;
    }
  }
  dump_msg(stdout, &msg, sizeof(msg));
  if (p)
  {
    sendto(socketfd, &msg, sizeof(msg), 0, (struct sockaddr *) &address, sizeof(address));
    close(socketfd);
  }
  return true;
}

int
main (int argc, char **argv)
{
  get_args(argc, argv);
  return EXIT_SUCCESS;
}
