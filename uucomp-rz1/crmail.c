/*
 * crmail - get compressed mail from host, uncompress
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
  char cmd2[1024];
  int i;
  FILE *log;

  if ((log = fopen (UULOG, "a")) == (FILE *) NULL)
    log = stdout;
  sprintf (cmd, "(%s|%s ", UNCOMPRESS, RMAIL);
  for (i = 1; i < argc; i++)
  {
    strcat (cmd, argv[i]);
    strcat (cmd, " ");
  }
  sprintf (cmd2, ") >> %s 2>&1", UULOG);
  strcat (cmd, cmd2);
  i = system (cmd);
  if (i != 0)
    fprintf (log, "Running '%s', rc=%d\n", cmd, i);
  exit (i);
}
