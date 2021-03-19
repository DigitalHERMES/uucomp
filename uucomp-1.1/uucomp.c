/*
 * uucomp - compress outgoing news/mail
 * 
 * usage: uucomp C.*
 * 
 * This works for Taylor uucp (available from prep.ai.mit.edu:/pub/gnu/uucp*),
 * but I don't promise it works for anyone else's uucp package.  Basically, this
 * is a quick-n-dirty hack to get compressed mail and news to a uucp site.  This
 * becomes important when you're on the other end of a 1200 baud packet radio
 * link, where the throughput can be 60 CPS (or lower).  It also tends to hide
 * any nasties that people might want to say to you, since the packets *are*
 * public readable.  Yes, I looked at uubatch, but it was too complicated for
 * me to figure out <grin>, and it didn't work with Taylor-uucp.  This is almost
 * too simple to work...
 * 
 * To use this little guy, do something like this in the .bashrc or .profile
 * or .cshrc of the uucp's login shell:
 * 
 * cd /usr/spool/uucp/<wherever the C. and D. files are kept>
 * /usr/bin/uucomp C.*
 * exec /usr/lib/uucp/uucico
 * 
 * This program was written by Ed Carp (erc@apple.com).  It can be used for any
 * non-commercial purpose.  This software is freely redistributable.
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
#include "uucomp.h"
#undef NULL
#define NULL (0)
main (argc, argv)
int argc;
char **argv;
{
  int i, j, sw, ctr = 0, errflag = 0, mctr = 0, nctr = 0, skipctr = 0;
  char scr[64], rcmd[10], line[1024], lineout[1024];
  char *strtok (), *ptr, *lineptr, compfile[32];
  FILE *in;

  printf ("uucomp 1.1 08/04/93 ... by erc@apple.com\nscanning %d files.", argc - 1);
  for (i = 1; i < argc; i++)
  {
    if (strncmp (argv[i], "C.", 2) != 0)
    {
      skipctr++;
      continue;
    }
    if ((in = fopen (argv[i], "r+")) == (FILE *) NULL)
    {
      skipctr++;
      continue;
    }
    fgets (line, 1022, in);
    if(*line != 'E')
    {
      skipctr++;
      continue;
    }
    line[strlen (line) - 1] = NULL;
    rewind (in);
    *lineout = NULL;
    lineptr = line;
    sw = errflag = 0;
    printf (".");
    fflush (stdout);
    for (j = 0;; j++)
    {
      ptr = strtok (lineptr, " ");
      if (ptr == NULL)
	break;
      lineptr = NULL;
      if (j == 1)
      {
	if (access (ptr, 4) == EOF)
	{
#ifdef DEBUG
	  printf ("skip: file '%s' doesn't exist\n", ptr);
#endif
	  errflag = 1;
	  break;		       /*
				        * skip it if the data file isn't
				        * there yet 
				        */
	}
	strcpy (compfile, ptr);
      }
      if (j == 9)
      {
	if (strcmp (ptr, "rmail") != 0 && strcmp (ptr, "rnews") != 0)
	{
#ifdef DEBUG
	  printf ("skip: '%s' wrong command\n", ptr);
#endif
	  errflag = 1;
	  break;
	}
	if (strcmp (ptr, "rmail") == 0)
	  mctr++;
	if (strcmp (ptr, "rnews") == 0)
	  nctr++;
	sw = 1;
	strcat (lineout, "c");
      }
      strcat (lineout, ptr);
      strcat (lineout, " ");
    }
    if (errflag == 1)
    {
      skipctr++;
      fclose (in);
      continue;
    }
    fprintf (in, "%s\n", lineout);
    fclose (in);
    sprintf (line,
	     "%s -fc > /tmp/uucomp.%d < %s;cp /tmp/uucomp.%d %s",
	     COMPRESS, getpid (), compfile, getpid (), compfile);
    system (line);
    ctr++;
  }
  sprintf (line, "/tmp/uucomp.%d", getpid ());
  unlink (line);
  printf ("\n%d skipped, %d compressed (%d mail, %d news).\n",
	  skipctr, ctr, mctr, nctr);
  exit (0);
}
