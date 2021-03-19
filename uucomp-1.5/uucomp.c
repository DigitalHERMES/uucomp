/*
 * uucomp - compress outgoing news/mail
 * 
 * usage: uucomp remote-systemname C.*
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
 * /usr/bin/uucomp remote-systemname C.*
 * exec /usr/lib/uucp/uucico
 * 
 * This program was written by Ed Carp (erc@apple.com).  It can be used for any
 * non-commercial purpose.  This software is freely redistributable.
 */

/*
 * 
 * Copyright 1993 by Ed Carp (erc@apple.com)  All rights reserved.
 * Copyright 2002 by Ed Carp (erc@pobox.com)  All rights reserved.
 * 
 * Permission is hereby granted for any non-commercial use of this
 * program, as long as this copyright notice remains intact.  Commercial
 * users may contact me - I'm easy.
 * 
 */

#include <stdio.h>
#include <errno.h>
#include "uucomp.h"
#undef NULL
#define NULL (0)
main (argc, argv)
int argc;
char **argv;
{
  int i, j, sw, ctr = 0, errflag = 0, mctr = 0, nctr = 0, skipctr = 0;
  int ac = 0;
  char scr[64], rcmd[10], line[1024], lineout[1024];
  char line2[1024];
  char *strtok (), *strchr (), *strrchr(), *ptr, *ptr2, *lineptr, compfile[32],
    *basename ();
  char newaddy[1024];
  FILE *in, *data;
  char callgrade;
  char *addrp, *getenv (), *strstr ();

  /*
   * printf ("uucomp 1.0 07/15/93 ... by erc@apple.com\nscanning %d
   * files.", argc - 1); printf ("uucomp 1.1 05/13/02 ... by
   * erc@pobox.com\nscanning %d files.", argc - 1); printf ("uucomp
   * 1.2 01/15/03 ... by erc@pobox.com\nscanning %d files.", argc -
   * 1); printf ("uucomp 1.3 04/03/03 ... by erc@pobox.com\nscanning
   * %d files.", argc - 1); 
  printf ("uucomp 1.4 11/11/04 ... by erc@pobox.com\nscanning %d files.", argc - 1);
   */
  printf ("uucomp 1.5 04/23/05 ... by erc@pobox.com\nscanning %d files.", argc - 1);
  addrp = getenv ("UUADDR");
  for (i = 2; i < argc; i++)
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
    fclose (in);
    line[strlen (line) - 1] = NULL;
    *lineout = NULL;
    lineptr = line;
    sw = errflag = 0;
    callgrade = 'A';		       /* default call grade */
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
	sprintf (scr, "../D./%s", ptr);
	if (access (scr, 4) == EOF)
	{
	  printf ("skip: file '%s' doesn't exist\n", scr);
	  errflag = 1;
	  break;		       /* 
				        * skip it if the data file isn't
				        * there yet 
				        */
	}
	ac = 0;			       /* assume not already
				        * compressed */
	if ((data = fopen (scr, "r")) != (FILE *) NULL)
	{
	  /*
	   * if no personal address specified, send at default call
	   * grade 
	   */
	  if (addrp == (char *) NULL)
	  {
	    printf ("N");
	    fgets (line2, 1022, data);
	  }
	  else
	  {
	    while (fgets (line2, 1022, data) != (char *) NULL)
	    {
	      if (strlen (line2) < 4)
		break;
	      /*
	       * if we find a "To: " line and our personal email
	       * address is in the line, bump up the call grade to
	       * '9' 
	       */
	      printf ("x");
	      if (strncmp (line2, "To: ", 4) == 0)
	      {
		if (strstr (line2, addrp) != (char *) NULL)
		{
		  callgrade = '9';
		  printf ("X");
		}
	      }
	    }
	  }
	  fclose (data);
	  if (*line2 == '\037' && *(line2 + 1) == '\213')
	    ac = 1;		       /* already compressed */
	}
	strcpy (compfile, scr);
      }
      if (j == 9)		       /* validate remote command */
      {
	if (strcmp (ptr, "rmail") != 0 && strcmp (ptr, "rnews") != 0)
	{
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
      if (j > 9)		       /* validate email address,
				        * strip off target system
				        * UUCP addy */
      {
	sprintf (scr, "%s!", argv[1]);
	if (strncmp (scr, ptr, strlen (scr)) == 0)
	  ptr += strlen (scr);
	/*
	 * if the line has a UUCP style address in the line, convert
	 * into an INTERNET style address 
	 */
	if (strchr (ptr, '@') == (char *) NULL
	    && strchr (ptr, '!') != (char *) NULL)
	{
	  ptr2 = strrchr (ptr, '!');
	  /*
	   * if there's more than one '!' in the addy, ignore all but 
	   * the last one 
	   */
	  if (ptr2 != strchr (ptr, '!'))
	  {
	    ptr2--;
	    while (*ptr2 != '!')
	      ptr2--;
	    *ptr2 = NULL;
	    ptr = ptr2 + 1;
	    ptr2 = strchr (ptr, '!');
	  }
	  strcpy (newaddy, ptr2 + 1);
	  strcat (newaddy, "@");
	  *ptr2 = NULL;
	  strcat (newaddy, ptr);
	  ptr = newaddy;
	}
      }
      strcat (lineout, ptr);
      strcat (lineout, " ");
    }
    if (errflag == 1)
    {
      skipctr++;
      continue;
    }
    in = fopen (argv[i], "w");
    fprintf (in, "%s\n", lineout);
    fclose (in);
    if (ac == 0)
    {
      sprintf (line, "%s -fc > /tmp/uucomp.%d < %s;cp /tmp/uucomp.%d %s",
	       COMPRESS, getpid (), compfile, getpid (), compfile);
      system (line);
    }
    strcpy (line, argv[i]);
    line[2] = callgrade;	       /* change job grade from 'C'
				        * to 'A' */
    printf ("%c", callgrade);
    rename (argv[i], line);
    ctr++;
  }
  sprintf (line, "/tmp/uucomp.%d", getpid ());
  unlink (line);
  printf ("\n%d skipped, %d compressed (%d mail, %d news).\n",
	  skipctr, ctr, mctr, nctr);
  exit (0);
}
