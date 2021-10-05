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
    char rmail_cmd[MAX_FILENAME];

    struct stat st;
    off_t file_size;

    FILE *tmp_mail_fp;

    char *char_ptr;
    char  *char_ptr2;
    char  *char_ptr3;

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
        sprintf(rmail_cmd, "(rmail ");
        for (int i = 1; i < argc; i++)
        {
            strcat (rmail_cmd, argv[i]);
            strcat (rmail_cmd, " ");
        }
        sprintf (rmail_cmd+strlen(rmail_cmd), "< %s)", tmp_mail);

        printf("Running: %s\n", rmail_cmd);

        system(rmail_cmd);

        goto exit_successful;
    }

    char_ptr2 = strstr(char_ptr, "filename=");

    char_ptr3 = strstr(char_ptr2, "\n");

    printf("file payload: %s\n", char_ptr3);

    // printf("aa\n%s", char_ptr);

    // now extract the payload, decode it, convert back to base64, and re-write the email...


exit_successful:
    // unlink(tmp_mail);


    return EXIT_SUCCESS;
}
