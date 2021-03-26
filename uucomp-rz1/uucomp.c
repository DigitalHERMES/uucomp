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
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "uucomp.h"
#undef NULL
#define NULL (0)

int main (argc, argv)
int argc;
char **argv;
{
    int i, j, ctr = 0, errflag = 0, mctr = 0, nctr = 0, skipctr = 0;
    int ac = 0;
    char scr[128], line[65536], lineout[65536];
    char line2[65536];
    char *strtok (), *strchr (), *strrchr(), *ptr, *lineptr, compfile[32],
        *basename ();
    FILE *in, *data;
    char *getenv (), *strstr ();

    printf ("uucomp rz1 28/03/21  ... by erc@pobox.com and rafael@rhizomatica.org\nscanning %d files.", argc - 1);

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
        errflag = 0;

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
                    break;                   /*
                                              * skip it if the data file isn't
                                              * there yet 
                                              */
                }
                ac = 0;			       /* assume not already
                                                        * compressed */
                if ((data = fopen (scr, "r")) != (FILE *) NULL)
                {
                    printf ("N");
                    fgets (line2, 1022, data);
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
                strcat (lineout, "c");
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
        ctr++;
    }
    sprintf (line, "/tmp/uucomp.%d", getpid ());
    unlink (line);
    printf ("\n%d skipped, %d compressed (%d mail, %d news).\n",
            skipctr, ctr, mctr, nctr);
    exit (0);
}
