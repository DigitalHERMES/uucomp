/*
   * trim - trim white space from the beginning and end of a string
 */
#ifndef NULL
#include <stdio.h>
#endif
char *
Trim (s)
     char *s;
{
  static char news[4096], *sptr, *ptr;

  strcpy (news, s);
  sptr = news;
  ptr = sptr + strlen (s) - 1;
  while (isspace(*sptr) && *sptr != NULL)
    sptr++;
  while (isspace(*ptr) && *ptr != NULL)
    ptr--;
  *(ptr + 1) = NULL;
  return (sptr);
}
