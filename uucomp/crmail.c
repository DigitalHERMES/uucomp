/* uucomp: crmail
 * Copyright (C) 2021 Rhizomatica
 * Author: Rafael Diniz <rafael@riseup.net>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_FILENAME 4096
#define S_BUF 128

#define ENC_TYPE_NONE 0
#define ENC_TYPE_AUDIO 1
#define ENC_TYPE_IMAGE 2


int main (int argc, char *argv[])
{

    // Variables declaration
    char uncompress_cmd[MAX_FILENAME];
    char tmp_mail[MAX_FILENAME];
    char *blob;

    struct stat st;
    off_t file_size;

    FILE *tmp_mail_fp;

    char *char_ptr;

    int encoding_type = ENC_TYPE_NONE;

    // Code starts here
    sprintf(tmp_mail, "/tmp/crmail.%d", getpid ());

    sprintf(uncompress_cmd, "(gzip -dc > %s)", tmp_mail);

    system (uncompress_cmd);

    if (stat(tmp_mail, &st) != 0)
    {
        printf("%s could not be opened.\n", tmp_mail);
        return EXIT_FAILURE;
    }
    file_size = st.st_size;

    if (file_size == 0)
    {
        printf("%s has zero size.\n", tmp_mail);
        return EXIT_FAILURE;
    }

    // check if we have image/x-vvc or audio/x-lpcnet...
    tmp_mail_fp = fopen(tmp_mail, "r");

    if (tmp_mail_fp == NULL)
    {
        printf("%s could not be opened.\n", tmp_mail);
        return EXIT_FAILURE;
    }

    blob = malloc(file_size);

    fread(blob, file_size, 1, tmp_mail_fp);

    fclose(tmp_mail_fp);

    char_ptr = strstr(blob, "Content-Type: image/x-vvc");

    if (char_ptr != NULL)
    {
        printf("IMAGE found.\n");
        encoding_type = ENC_TYPE_IMAGE;
    }

    if (encoding_type == ENC_TYPE_NONE)
    {
        // not image... checking if there is audio
        char_ptr = strstr(blob, "Content-Type: audio/x-lpcnet");
        if (char_ptr != NULL)
        {
            printf("AUDIO found.\n");
            encoding_type = ENC_TYPE_AUDIO;
        }
    }


    if (encoding_type == ENC_TYPE_NONE)
    {
        // just call rmail...
        return EXIT_SUCCESS;
    }

    // now extract the payload, decode it, convert back to base64, and re-write the email...

#if 0

    sprintf (cmd, "(%s|%s ", UNCOMPRESS, RMAIL);
    for (int i = 1; i < argc; i++)
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
#endif

    // TODO
    return EXIT_SUCCESS;
}
