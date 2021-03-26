/*
 * frmail - filter incoming rmail, changing UUCP addresses to
 * INTERNET addresses 
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#undef NULL
#define NULL (0)
  /*
   * replace - replace str1 with str2 * returns pointer to result string
   * usage: replace(input, lookfor, replacement) 
   */
char *
replace (str, lookfor, replacement)
     char *str, *lookfor, *replacement;

{
  static char rep[65535];
  char *ptr, *ptr2;
  char c;

#ifdef DEBUG
  printf ("<!-- replace: replace '%s' with '%s' in '%s' -->\n", lookfor,
	  replacement, str);
#endif
  if ((ptr = strstr (str, lookfor)) == NULL)
    return (str);
  c = *ptr;
  *ptr = NULL;
  strcpy (rep, str);
  *ptr = c;
  ptr2 = ptr + strlen (lookfor);
  strcat (rep, replacement);
  strcat (rep, ptr2);

  /*
   * if (debugflag == TRUE) my_fprintf (fsout,"<!-- replace: returning
   * '%s' -->\n", rep); 
   */
  return (rep);
}

  /*
   * * trim - trim white space from the beginning and end of a string 
   */
#ifndef NULL
#include <stdio.h>
#endif

char *
Trim (s)
     char *s;

{
  static char news[65535], *sptr, *ptr;

  strcpy (news, s);
  sptr = news;
  ptr = sptr + strlen (s) - 1;
  while (isspace (*sptr) && *sptr != NULL)
    sptr++;
  while (isspace (*ptr) && *ptr != NULL)
    ptr--;
  *(ptr + 1) = NULL;
  return (sptr);
}

#include "fixaddr.c"
int main ()
{
  char *ptr, *ptr2, *ptr3, linein[65535], linein2[65535];
  int ignore;

  /*
   * if the line has a UUCP style address in the line, convert into
   * an INTERNET style address 
   */
  ignore = 0;
  while (fgets (linein, 65533, stdin) != (char *) NULL)
    {
      linein[strlen (linein) - 1] = NULL;
      strcpy (linein2, linein);
      if (strncmp (linein, "From ", 5) == 0 && ignore == 1)
	ignore = 0;
      if (ignore == 0)
	{
	  if (strlen (linein) == 0)
	    ignore = 1;
	  else
	    {
	      if (strncmp (linein, "From ", 5) == 0 ||
		  strncmp (linein, "From: ", 6) == 0 ||
		  strncmp (linein, "To: ", 4) == 0 ||
		  strncmp (linein, "Cc: ", 4) == 0 ||
		  strncmp (linein, "Sender: ", 8) == 0 ||
		  strncmp (linein, "Reply-To: ", 10) == 0)
		{
		  ptr = linein;
		  while ((ptr2 = strtok (ptr, " <>,:;")) != (char *) NULL)
		    {
		      ptr = NULL;
#ifdef DEBUG
		      printf ("DEBUG: ptr2='%s'\n", ptr2);
#endif
		      if (strchr (ptr2, '!') != NULL)
			{
			  ptr3 = replace (linein2, ptr2, fixaddr (ptr2));
			  strcpy (linein2, ptr3);
#ifdef DEBUG
			  printf ("DEBUG: linein2='%s'\n", linein2);
#endif
			}
		    }
#ifdef DEBUG
		  printf ("DEBUG: final linein2='%s'\n", linein2);
#endif
		  strcpy (linein, linein2);
		}
	    }
	}
      puts (linein);
    }
}
