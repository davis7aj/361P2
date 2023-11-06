#include <stdio.h>

#include "dhcp.h"
#include "format.h"

void
dump_msg (FILE *output, msg_t *msg, size_t size)
{
  fprintf (output, "------------------------------------------------------\n");
  fprintf (output, "BOOTP Options\n");
  fprintf (output, "------------------------------------------------------\n");

  // TODO: Print out the BOOTP fields as specified in the assignment

  fprintf (output, "------------------------------------------------------\n");
  fprintf (output, "DHCP Options\n");
  fprintf (output, "------------------------------------------------------\n");

  // TODO: Print out the DHCP fields as specified in the assignment

}
