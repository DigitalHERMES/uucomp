/*
 * crnews - get compressed news from host, uncompress
 * WARNING: This may be insecure!
 */

/*
 * 
 * Copyright 1993 by Ed Carp (erc@apple.com)  All rights reserved.
 * 
 * Permission is hereby granted for any non-commercial use of this
 * program, as long as this copyright notice remains intact.  Commercial
 * users may contact me - I'm easy.
 * 
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "uucomp.h"
int main (argc, argv)
int argc;
char **argv;
{
  char cmd[1024];
  int i;

  sprintf (cmd, "%s|%s ", UNCOMPRESS, RNEWS);
  for (i = 1; i < argc; i++)
  {
    strcat (cmd, argv[i]);
    strcat (cmd, " ");
  }
  system (cmd);
  exit (0);
}
