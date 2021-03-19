char *
fixaddr (l)
     char *l;
{
  static char li[65535];
  static char oldaddress[65535];
  static char newaddress[65535];
  char token[512];
  char *ptr, *ptr2, *lptr, *tptr;

  strcpy (li, l);
/*
  lptr = strchr (l, ' ');
  if (lptr == NULL)
    return (l);
  lptr++;
*/
  lptr = l;
  ptr = strchr (lptr, '<');
  if (ptr == NULL)
    {
      strcpy (oldaddress, l);
      ptr = strchr (oldaddress, ' ');
      if (ptr == NULL)
	{
#ifdef DEBUG
	  printf ("fixaddr: check 1: oldaddress='%s'\n", oldaddress);
#endif
	  strcpy (newaddress, oldaddress);
	  while ((ptr = strchr (newaddress, '!')) != (char *) NULL)
	    {
	      if ((ptr = strchr (oldaddress, '@')) != (char *) NULL)
		{
		  *ptr = NULL;
		  strcpy (token, oldaddress);
		}
	      else
		{
		  ptr = strchr (oldaddress, '!');
		  strcpy (token, ptr + 1);
		}
#ifdef DEBUG
	      printf ("fixaddr: check 2: token='%s'\n", token);
#endif
	      ptr2 = strchr (token, '!');
	      if (ptr2 != NULL)
		{
		  *ptr2 = NULL;
		  ptr2++;
		}
	      else
		{
		  ptr2 = ptr + 1;
		  *ptr = NULL;
		  strcpy (token, oldaddress);
		}
#ifdef DEBUG
	      printf ("fixaddr: check 3: ptr2='%s'\n", ptr2);
#endif
	      strcpy (newaddress, ptr2);
	      strcat (newaddress, "@");
	      strcat (newaddress, token);
#ifdef DEBUG
	      printf ("fixaddr: check 4: newaddress='%s'\n", newaddress);
#endif
	    }
	  return (newaddress);
	}
      *ptr = NULL;
    }
  else
    {
      strcpy (oldaddress, ptr + 1);
      ptr = strchr (oldaddress, '>');
      if (ptr != NULL)
	*ptr = NULL;
      else
	oldaddress[strlen (oldaddress) - 1] = NULL;
    }
  while ((ptr = strchr (li, '!')) != (char *) NULL)
    {
      strcpy (oldaddress, li);
#ifdef DEBUG
      printf ("fixaddr: check 5: oldaddress='%s'\n", oldaddress);
#endif
      if ((ptr = strchr (oldaddress, '@')) != (char *) NULL)
	*ptr = NULL;
      strcpy (token, oldaddress);
#ifdef DEBUG
      printf ("fixaddr: check 6: token='%s'\n", token);
#endif
      ptr2 = strchr (token, '!');
      *ptr2 = NULL;
      ptr2++;
#ifdef DEBUG
      printf ("fixaddr: check 7: ptr2='%s'\n", ptr2);
#endif
      strcpy (newaddress, ptr2);
      strcat (newaddress, "@");
      strcat (newaddress, token);
#ifdef DEBUG
      printf ("fixaddr: check 8: newaddress='%s'\n", newaddress);
#endif
      replace (li, oldaddress, newaddress);
    }
  return (li);
}
