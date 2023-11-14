#include <stdio.h>

#include "dhcp.h"
#include "format.h"

void
dump_msg (FILE *output, msg_t *msg, size_t size)
{
  printf ("------------------------------------------------------\n");
  printf ("BOOTP Options\n");
  printf ("------------------------------------------------------\n");

  // TODO: Print out the BOOTP fields as specified in the assignment

  if (msg->op == 1)
  {
    printf("Op Code (op) = %hhu [BOOTREQUEST]\n", msg->op);
  } else {
    printf("Op Code (op) = %hhu [BOOTREPLY]\n", msg->op);
  }

   switch (msg->htype) {
        case 1:
            printf("Hardware Type (htype) = %hhu [Ethernet (10Mb)]\n", msg->htype);
            break;
        case 6:
            printf("Hardware Type (htype) = %hhu [IEEE 802 Networks]\n", msg->htype);
            break;
        case 7:
            printf("Hardware Type (htype) = %hhu [ARCNET]\n", msg->htype);
            break;
        case 15:
            printf("Hardware Type (htype) = %hhu [Frame Relay]\n", msg->htype);
            break;
        case 18:
            printf("Hardware Type (htype) = %hhu [Fibre Channel]\n", msg->htype);
            break;
        case 19:
            printf("Hardware Type (htype) = %hhu [ATM]\n", msg->htype);
            break;
        // Add more cases as needed
        default:
            printf("Unknown hardware type (%u).\n", msg->htype);
            break;
    }

    printf("Hardware Address Length (hlen) = %hhu\n", msg->hlen);
    printf("Hops (hops) = %hhu\n", msg->hops);
    printf("Transaction ID (xid) = %u (0x%X)\n", msg->xid, msg->xid);


    int seconds = msg->secs % 60;
    int totalMinutes = msg->secs / 60;
    int minutes  = totalMinutes % 60;
    int hours = totalMinutes / 60;

    printf("Seconds (secs) = 0 Days, %d:%d:%d\n", hours, minutes, seconds);
    printf("Flags (flags) = %u\n", msg->flags);
    // Print ciaddr in dotted decimal notation
    printf("Client IP Address (ciaddr) = %u.%u.%u.%u\n",
           (msg->ciaddr >> 24) & 0xFF,
           (msg->ciaddr >> 16) & 0xFF,
           (msg->ciaddr >> 8) & 0xFF,
           msg->ciaddr & 0xFF);

    printf("Your IP Address (yiaddr) = %u.%u.%u.%u\n",
           (msg->yiaddr >> 24) & 0xFF,
           (msg->yiaddr >> 16) & 0xFF,
           (msg->yiaddr >> 8) & 0xFF,
           msg->yiaddr & 0xFF);
    
    printf("Server IP Address (siaddr) = %u.%u.%u.%u\n",
           (msg->siaddr >> 24) & 0xFF,
           (msg->siaddr >> 16) & 0xFF,
           (msg->siaddr >> 8) & 0xFF,
           msg->siaddr & 0xFF);

    printf("Relay IP Address (giaddr) = %u.%u.%u.%u\n",
           (msg->giaddr >> 24) & 0xFF,
           (msg->giaddr >> 16) & 0xFF,
           (msg->giaddr >> 8) & 0xFF,
           msg->giaddr & 0xFF);

    printf("Client Ethernet Address (chaddr) = %X\n", msg->chaddr);



  fprintf (output, "------------------------------------------------------\n");
  fprintf (output, "DHCP Options\n");
  fprintf (output, "------------------------------------------------------\n");

  // TODO: Print out the DHCP fields as specified in the assignment

}
