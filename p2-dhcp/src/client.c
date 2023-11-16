#include <stdlib.h>

#include "dhcp.h"
#include "format.h"
#include "port_utils.h"

int
main (int argc, char **argv)
{
  get_args(argc, argv)
  return EXIT_SUCCESS;
}
static bool
get_args (int argc, char **argv)
{
  uint32_t x = 42;
  int t = ETH;
  uint8_t c[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  int m = DHCPDISCOVER;
  in_addr_t s;
  inet_aton("127.0.0.1", &s);
  in_addr_t r;
  inet_aton("127.0.0.2", &r);
  int index = 0;
  while (index < argc)
  {
    if (strcmp(argv[index], "-x") == 0)
    {
      x = (uint32_t) strtol(argv[index + 1], (char **)NULL, 10);
      index += 2;
    }
    if (strcmp(argv[index], "-t") == 0)
    {
      t = (int) strtol(argv[index + 1], (char **)NULL, 10);
      index += 2;
    }
    if (strcmp(argv[index], "-c") == 0)
    {
      for (i = 0; i < strlen(argv[index]); i++)
      {
        c[i] = (uint8_t) strtol(argv[index + 1], (char **)NULL, 10);
      }
      index += 2;
    }
    if (strcmp(argv[index], "-m") == 0)
    {
      m = (int) strtol(argv[index + 1], (char **)NULL, 10);
      index += 2;
    }
    if (strcmp(argv[index], "-s") == 0)
    {
      char *first = argv[index + 1];
      char *second = argv[index + 2];
      char *third = argv[index + 3];
      char *fourth = argv[index + 4];
      inet_aton(first + "." + second + "." + third + "." + fourth, *s);
      index += 5;
    }
    if (strcmp(argv[index], "-r") == 0)
    {
      char *first = argv[index + 1];
      char *second = argv[index + 2];
      char *third = argv[index + 3];
      char *fourth = argv[index + 4];
      inet_aton(first + "." + second + "." + third + "." + fourth, *r);
      index += 5;
    }
    if (strcmp(argv[index], "-p") == 0)
    {
      x = 0;
      int socketfd = socket (AF_INET, SOCK_DGRAM, 0);
      struct sockaddr_in address;
      address.sin_family = AF_INET;
      address.sin_port = (uint16_t) strtol(get_port(), (char **)NULL, 10);
      in_addr_t val;
      inet_aton("127.0.0.1", &val);
      address.sin_addr = val;
      address.sin_zero = 0;
      index += 1;
    }
  }
  return true;
}
