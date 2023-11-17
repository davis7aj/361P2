#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>

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
  uint8_t c[16] = {1, 2, 3, 4, 5, 6};
  int m = 1;
  char *s = "127.0.0.1";
  char *r = "127.0.0.2";
  int index = 1;
  msg_t msg;
  memset(&msg, 0, sizeof(msg));
  msg.op = 1;
  msg.htype = t;
  msg.hlen = ETH_LEN;
  msg.xid = htonl(x);
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
      msg.htype = t;
      if (t == ETH)
      {
        msg.hlen = ETH_LEN;
      } else if (t == IEEE802) {
        msg.hlen = IEEE802_LEN;
      } else if (t == ARCNET) {
        msg.hlen = ARCNET_LEN;
      } else if (t == FRAME_RELAY) {
        msg.hlen = FRAME_LEN;
      } else if (t == FIBRE) {
        msg.hlen = FIBRE_LEN;
      } else if (t == ATM) {
        msg.hlen = ATM_LEN;
      } else {
        printf("t out of bounds : %d", t);
      }
      index += 2;
    }
    else if (strcmp(argv[index], "-c") == 0)
    {
      memset(&c, 0, sizeof(c));
      char *str = argv[index + 1];
      for (int i = 0; i < msg.hlen; i++)
      {
        unsigned char byte;
        sscanf(&str[i * 2], "%02hhx",  &byte);
        msg.chaddr[i] = byte;
      }
      //memcpy(&msg.chaddr, &c, sizeof(c));
      index += 2;
    }
    else if (strcmp(argv[index], "-m") == 0)
    {
      m = strtol(argv[index + 1], (char **) NULL, 10);
      index += 2;
    }
    else if (strcmp(argv[index], "-s") == 0)
    {
      s = argv[index + 1];
      index += 2;
    }
    else if (strcmp(argv[index], "-r") == 0)
    {
      r = argv[index + 1];
      index += 2;
    }
    else if (strcmp(argv[index], "-p") == 0)
    {
      // x = htonl(0); //remove after
      // memcpy(&msg.xid, &x, sizeof(x)); //remove after
      struct timeval timeout = { 5, 0 };
      socketfd = socket (AF_INET, SOCK_DGRAM, 0);
      setsockopt (socketfd, SOL_SOCKET, SO_RCVTIMEO, (const void *) &timeout,
                  sizeof (timeout));
      memset(&address, 0, sizeof(address));
      address.sin_family = AF_INET;
      address.sin_port = (uint16_t) strtol(get_port(), (char **)NULL, 10);
      struct in_addr tmp;
      inet_aton("127.0.0.1", &tmp);
      address.sin_addr.s_addr = tmp.s_addr;
      p = true;
      index += 1;
    }
    else
    {
      printf("Error encountered at : %s\n", argv[index]);
      return false;
    }
  }

  int options_offset = 0;

  // Write additional DHCP options
  *(uint32_t *)msg.options = htonl(MAGIC_COOKIE);
  options_offset += sizeof(uint32_t);

  // Write DHCP message type:
  msg.options[options_offset++] = 53;
  msg.options[options_offset++] = 1;
  msg.options[options_offset++] = m;

  // Write DHCP request IP:
  msg.options[options_offset++] = 50;
  msg.options[options_offset++] = 4;
  struct in_addr tmp;
  inet_aton(r, &tmp);
  *(uint32_t *)(msg.options + options_offset) = tmp.s_addr;
  options_offset += sizeof(uint32_t);

  msg.options[options_offset++] = 54;
  msg.options[options_offset++] = 4;
  inet_aton(s, &tmp);
  *(uint32_t *)(msg.options + options_offset) = tmp.s_addr;
  options_offset += sizeof(uint32_t);

  // Print out the message
  dump_msg(stdout, &msg, sizeof(msg) + options_offset);

  // If user wants us to send DHCP request
  if (p || msg.xid != 0)
  {
    sendto(socketfd, &msg, sizeof(msg) + options_offset, 0, (struct sockaddr *) &address, sizeof(address));
    if (msg.xid == 0) {
      printf("\n++++++++++++++++\n");
      printf("SERVER RECEIVED:\n");
      dump_msg(stdout, &msg, sizeof(msg) + options_offset);
      printf("++++++++++++++++\n");
    } else if (p && msg.xid != 0) {
      msg_t msg_offer;
      msg_t msg_ack;
      memset(&msg_offer, 0, sizeof(msg));
      memset(&msg_ack, 0, sizeof(msg));
      socklen_t len = 0;
      struct sockaddr_in addr;

      memset(&msg_offer, 0, sizeof(msg_offer));
      memset(&msg_ack, 0, sizeof(msg_ack));
      memset(&addr, 0, sizeof(addr));

      ssize_t error = recvfrom(socketfd, &msg_offer, sizeof(msg_offer), 0 , (struct sockaddr *) &addr, &len);
      printf("%ld %d", error, errno);
      for (int ii = 0; ii < sizeof(msg_offer.options); ii++) {
        printf("%02x ", msg_offer.options[ii]);
        if (ii % 8 == 7) {
          printf(" ");
        }
        if (ii % 16 == 15) {
          printf("\n");
        }
      }
      printf("\n");

      printf("++++++++++++++++\n");
      printf("CLIENT RECEIVED:\n");
      dump_msg(stdout, &msg_offer, sizeof(msg_offer));
      printf("++++++++++++++++\n");
      memset(&msg.options, 0, sizeof(msg.options));

      options_offset = 0;
      // Write additional DHCP options
      *(uint32_t *)msg.options = htonl(MAGIC_COOKIE);
      options_offset += sizeof(uint32_t);

      // Write DHCP message type:
      msg.options[options_offset++] = 53;
      msg.options[options_offset++] = 1;
      msg.options[options_offset++] = m;

      // Write DHCP request IP:
      msg.options[options_offset++] = 50;
      msg.options[options_offset++] = 4;
      struct in_addr tmp = msg_offer.siaddr;
      *(uint32_t *)(msg.options + options_offset) = tmp.s_addr;
      options_offset += sizeof(uint32_t);

      msg.options[options_offset++] = 54;
      msg.options[options_offset++] = 4;
      inet_aton(s, &tmp);
      *(uint32_t *)(msg.options + options_offset) = tmp.s_addr;
      options_offset += sizeof(uint32_t);

      sendto(socketfd, &msg, sizeof(msg) + options_offset, 0, (struct sockaddr *) &address, sizeof(address));
      dump_msg(stdout, &msg, sizeof(msg) + options_offset);
      recvfrom(socketfd, &msg_ack, sizeof(msg_ack), 0 , NULL, NULL);
      printf("++++++++++++++++\n");
      printf("CLIENT RECEIVED:\n");
      dump_msg(stdout, &msg_ack, sizeof(msg_ack));
      printf("++++++++++++++++\n");
    }
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
