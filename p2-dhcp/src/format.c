#include <stdio.h>

#include "dhcp.h"
#include "format.h"

#include <arpa/inet.h>
#include <netinet/in.h>

#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void
dump_msg (FILE *output, msg_t *msg, size_t size)
{
  printf ("------------------------------------------------------\n");
  printf ("BOOTP Options\n");
  printf ("------------------------------------------------------\n");

  // TODO: Print out the BOOTP fields as specified in the assignment

  if (msg->op == 1)
    {
      printf ("Op Code (op) = %hhu [BOOTREQUEST]\n", msg->op);
    }
  else
    {
      printf ("Op Code (op) = %hhu [BOOTREPLY]\n", msg->op);
    }

  switch (msg->htype)
    {
    case 1:
      printf ("Hardware Type (htype) = %hhu [Ethernet (10Mb)]\n", msg->htype);
      break;
    case 6:
      printf ("Hardware Type (htype) = %hhu [IEEE 802 Networks]\n",
              msg->htype);
      break;
    case 7:
      printf ("Hardware Type (htype) = %hhu [ARCNET]\n", msg->htype);
      break;
    case 15:
      printf ("Hardware Type (htype) = %hhu [Frame Relay]\n", msg->htype);
      break;
    case 18:
      printf ("Hardware Type (htype) = %hhu [Fibre Channel]\n", msg->htype);
      break;
    case 19:
      printf ("Hardware Type (htype) = %hhu [Asynchronous Transmission Mode "
              "(ATM)]\n",
              msg->htype);
      break;
    // Add more cases as needed
    default:
      printf ("Unknown hardware type (%u).\n", msg->htype);
      break;
    }

  printf ("Hardware Address Length (hlen) = %hhu\n", msg->hlen);
  printf ("Hops (hops) = %hhu\n", msg->hops);
  printf ("Transaction ID (xid) = %d (0x%x)\n", ntohl (msg->xid),
          ntohl (msg->xid));

  uint16_t days, hours, minutes, seconds;
  uint16_t originSec = ntohs (msg->secs);

  // Calculate days
  days = originSec / 86400;

  // Calculate hours
  hours = (originSec % 86400) / 3600;

  // Calculate minutes
  minutes = (originSec % 3600) / 60;

  // Remaining seconds
  seconds = originSec % 60;

  // Print in the desired format
  printf ("Seconds (secs) = %d Days, %d:%02d:%02d\n", days, hours, minutes,
          seconds);

  // printf("Seconds (secs) = 0 Days, %d:%s:%s\n", hours, str2, str1);

  printf ("Flags (flags) = %u\n", msg->flags);
  // Print ciaddr in dotted decimal notation
  printf ("Client IP Address (ciaddr) = %s\n", inet_ntoa (msg->ciaddr));

  printf ("Your IP Address (yiaddr) = %s\n", inet_ntoa (msg->yiaddr));

  printf ("Server IP Address (siaddr) = %s\n", inet_ntoa (msg->siaddr));

  printf ("Relay IP Address (giaddr) = %s\n", inet_ntoa (msg->giaddr));

  printf ("Client Ethernet Address (chaddr) = ");
  for (int i = 0; i < msg->hlen; i++)
    {
      /* code */
      printf ("%02x", msg->chaddr[i]);
    }
  printf ("\n");

  if (size > 236)
    {
      /* code */
      printf ("------------------------------------------------------\n");
      printf ("DHCP Options\n");
      printf ("------------------------------------------------------\n");

      // Check if the options field starts with the magic cookie
      uint32_t cookieCheck;
      memcpy (&cookieCheck, msg->options, sizeof (cookieCheck));
      cookieCheck = ntohl (cookieCheck);

      uint32_t sId = 0;
      struct in_addr serverId;

      if (cookieCheck != MAGIC_COOKIE)
        {
          /* code */
          return;
        }

      printf ("Magic Cookie = [OK]\n");

      uint8_t res = 0;
      // uint32_t lease = 0;
      // uint8_t *cpyOptions = msg->options;

      char *msgType = "tempMsg";

      // Find DHCP option code 53 in the options field
      for (int i = 0; i < sizeof (msg->options); ++i)
        {
          if (msg->options[i] == 53)
            {

              if (i + 2 < 128)
                {
                  /* code */
                  res = msg->options[i + 2];
                }
              break;
            }
        }

      // Return the DHCP message type as a string
      switch (res)
        {
        case DHCPDISCOVER:
          msgType = "DHCP Discover";
          printf ("Message Type = %s\n", msgType);
          break;
        case DHCPOFFER:
          msgType = "DHCP Offer";
          printf ("Message Type = %s\n", msgType);

          uint32_t totalSec = 0;
          // Find DHCP option code 51 in the options field
          for (int i = 0; i < 128; ++i)
            {
              if (msg->options[i] == 51)
                {

                  if (i + 4 < 128)
                    {
                      /* code */
                      totalSec = ntohl (*(uint32_t *)(msg->options + i + 2));
                    }
                  break;
                }
            }

          // Calculate days
          days = totalSec / 86400;
          // Calculate hours
          hours = (totalSec % 86400) / 3600;
          // Calculate minutes
          minutes = (totalSec % 3600) / 60;
          // Remaining seconds
          seconds = totalSec % 60;

          printf ("IP Address Lease Time = %d Days, %d:%02d:%02d\n", days,
                  hours, minutes, seconds);

          sId = 0;
          // Find DHCP option code 54 in the options field
          for (int i = 0; i < 128; ++i)
            {
              if (msg->options[i] == 54)
                {

                  if (i + 4 < 128)
                    {
                      /* code */
                      sId = *(uint32_t *)(msg->options + i + 2);
                    }
                  break;
                }
            }
          serverId.s_addr = sId;

          printf ("Server Identifier = %s\n", inet_ntoa (serverId));
          break;
        case DHCPREQUEST:
          msgType = "DHCP Request";
          printf ("Message Type = %s\n", msgType);

          sId = 0;
          // Find DHCP option code 50 in the options field
          for (int i = 0; i < 128; ++i)
            {
              if (msg->options[i] == 50)
                {

                  if (i + 4 < 128)
                    {
                      /* code */
                      sId = *(uint32_t *)(msg->options + i + 2);
                    }
                  break;
                }
            }
          serverId.s_addr = sId;

          printf ("Request = %s\n", inet_ntoa (serverId));

          sId = 0;
          // Find DHCP option code 54 in the options field
          for (int i = 0; i < 128; ++i)
            {
              if (msg->options[i] == 54)
                {

                  if (i + 4 < 128)
                    {
                      /* code */
                      sId = *(uint32_t *)(msg->options + i + 2);
                    }
                  break;
                }
            }
          serverId.s_addr = sId;

          printf ("Server Identifier = %s\n", inet_ntoa (serverId));
          break;
        case DHCPDECLINE:
          msgType = "DHCP Decline";
          printf ("Message Type = %s\n", msgType);

          sId = 0;
          // Find DHCP option code 50 in the options field
          for (int i = 0; i < 128; ++i)
            {
              if (msg->options[i] == 50)
                {

                  if (i + 4 < 128)
                    {
                      /* code */
                      sId = *(uint32_t *)(msg->options + i + 2);
                    }
                  break;
                }
            }
          serverId.s_addr = sId;

          printf ("Request = %s\n", inet_ntoa (serverId));

          sId = 0;
          // Find DHCP option code 54 in the options field
          for (int i = 0; i < 128; ++i)
            {
              if (msg->options[i] == 54)
                {

                  if (i + 4 < 128)
                    {
                      /* code */
                      sId = *(uint32_t *)(msg->options + i + 2);
                    }
                  break;
                }
            }
          serverId.s_addr = sId;

          printf ("Server Identifier = %s\n", inet_ntoa (serverId));
          break;
        case DHCPACK:
          msgType = "DHCP ACK";
          printf ("Message Type = %s\n", msgType);

          totalSec = 0;
          // Find DHCP option code 51 in the options field
          for (int i = 0; i < 128; ++i)
            {
              if (msg->options[i] == 51)
                {

                  if (i + 4 < 128)
                    {
                      /* code */
                      totalSec = ntohl (*(uint32_t *)(msg->options + i + 2));
                    }
                  break;
                }
            }

          // Calculate days
          days = totalSec / 86400;
          // Calculate hours
          hours = (totalSec % 86400) / 3600;
          // Calculate minutes
          minutes = (totalSec % 3600) / 60;
          // Remaining seconds
          seconds = totalSec % 60;

          printf ("IP Address Lease Time = %d Days, %d:%02d:%02d\n", days,
                  hours, minutes, seconds);

          sId = 0;
          // Find DHCP option code 54 in the options field
          for (int i = 0; i < 128; ++i)
            {
              if (msg->options[i] == 54)
                {

                  if (i + 4 < 128)
                    {
                      /* code */
                      sId = *(uint32_t *)(msg->options + i + 2);
                    }
                  break;
                }
            }
          serverId.s_addr = sId;

          printf ("Server Identifier = %s\n", inet_ntoa (serverId));
          break;
        case DHCPNAK:
          msgType = "DHCP NAK";
          printf ("Message Type = %s\n", msgType);

          sId = 0;
          // Find DHCP option code 54 in the options field
          for (int i = 0; i < 128; ++i)
            {
              if (msg->options[i] == 54)
                {

                  if (i + 4 < 128)
                    {
                      /* code */
                      sId = *(uint32_t *)(msg->options + i + 2);
                    }
                  break;
                }
            }
          serverId.s_addr = sId;

          printf ("Server Identifier = %s\n", inet_ntoa (serverId));
          break;
        case DHCPRELEASE:
          msgType = "DHCP Release";
          printf ("Message Type = %s\n", msgType);

          sId = 0;
          // Find DHCP option code 54 in the options field
          for (int i = 0; i < 128; ++i)
            {
              if (msg->options[i] == 54)
                {

                  if (i + 4 < 128)
                    {
                      /* code */
                      sId = *(uint32_t *)(msg->options + i + 2);
                    }
                  break;
                }
            }
          serverId.s_addr = sId;

          printf ("Server Identifier = %s\n", inet_ntoa (serverId));
          break;
        // Add more cases as needed...
        default:
          msgType = "Unknown DHCP Message Type";
          break;
        }
    }

  // TODO: Print out the DHCP fields as specified in the assignment
}
