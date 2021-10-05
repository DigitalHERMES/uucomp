/* uucomp: uucomp
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

#define VERSION "6.66"

#define MAX_FILENAME 4096
#define S_BUF 128

#define MAX_C_PAYLOAD 4096

#define ENC_TYPE_NONE 0
#define ENC_TYPE_AUDIO 1
#define ENC_TYPE_IMAGE 2

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>

#include <b64/cdecode.h>

int main (int argc, char *argv[])
{
    char *c_filename;
    char d_filename[MAX_FILENAME];

    FILE *c_file = NULL;
    FILE *d_file = NULL;

    FILE *tmp_media;
    char tmp_media_filename[MAX_FILENAME];
    char tmp_encoded_media_filename[MAX_FILENAME];

    struct stat st;
    off_t file_size;

    char c_payload[MAX_C_PAYLOAD];
    char *d_payload; // dynamic size

    char field1[S_BUF], field2[S_BUF], field3[S_BUF], field4[S_BUF], field5[S_BUF], field6[S_BUF], field7[S_BUF], field8[S_BUF], field9[S_BUF], field11[S_BUF];
    char uu_cmd[MAX_FILENAME];

    char *char_ptr;
    char *char_ptr2;
    char *char_ptr3;
    char *char_ptr4;
    char *char_ptr5;

    char *original_filename;

    int encoding_type = ENC_TYPE_NONE;

    char *blob = NULL;

    if (argc < 2)
    {
        fprintf(stderr, "uucomp version %s by rhizomatica --\n\n", VERSION);
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "uucomp [\"C.\" file list]\n");
        fprintf(stderr, "Eg.: uucomp C.NT4DkChAAoLT C.NT8Rg3xAAoNX ...\n");
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++)
    {
        c_filename = argv[i];

        // Check if C file exists...
        if (strncmp (c_filename, "C.", 2) != 0)
        {
            printf("%s not a \"C.\" file, skipping.\n", c_filename);
            continue;
        }

        if (stat(c_filename, &st) != 0)
        {
            printf("%s could not be opened.\n", c_filename);
            continue;
        }

        file_size = st.st_size;

        if (file_size >= MAX_C_PAYLOAD)
        {
            printf("%s too big.\n", c_filename);
            continue;
        }

        c_file = fopen(c_filename, "r");
        if (c_file == NULL)
        {
            printf("%s could not be opened.\n", c_filename);
            continue;
        }

        fread(c_payload, 1, MAX_C_PAYLOAD, c_file);
        c_payload[strlen(c_payload) - 1] = 0;

        fclose(c_file);

        printf("%s payload: %s\n", c_filename, c_payload);

        sscanf(c_payload, "%s %s %s %s %s %s %s %s %s %s %s", field1,
               field2, field3, field4, field5, field6, field7, field8, field9,
               uu_cmd, field11);

        // check if we already compressed the payload
        if (strcmp (uu_cmd, "crmail") == 0)
        {
            printf("%s correspondent payload already compressed.\n", c_filename);
            continue;
        }

        // if not... check if this is an email
        if (strcmp (uu_cmd, "rmail") != 0)
        {
            printf("%s is an email - compressing.\n", c_filename);
        }

        sprintf (d_filename, "../D./%s", field2);

        if (stat(d_filename, &st) != 0)
        {
            printf("%s could not be opened.\n", d_filename);
            continue;
        }
        file_size = st.st_size;

        printf("%s size is: %ld\n", field2, file_size);

        // *************************************************
        // open D. file and check if it has compressable media
        d_file = fopen(d_filename, "r");
        if (d_file == NULL)
        {
            printf("%s could not be opened.\n", d_filename);
            continue;
        }

        d_payload = malloc(file_size);

        fread(d_payload, 1, file_size, d_file);

        fclose(d_file);

        char_ptr = strstr(d_payload, "Content-Type: multipart/mixed;");
        if (char_ptr == NULL)
        {
            printf("%s not a multi-part message.\n", d_filename);
            goto compress;
        }

        encoding_type = ENC_TYPE_NONE;

        char_ptr2 = strstr(char_ptr, "Content-Type: image/jpeg");
        if (char_ptr2 != NULL)
        {
            printf("JPEG image found.\n");
            encoding_type = ENC_TYPE_IMAGE;
        }

        if (encoding_type == ENC_TYPE_NONE)
        {
            char_ptr2 = strstr(char_ptr, "Content-Type: audio/aac");
            if (char_ptr2 != NULL)
            {
                printf("AAC audio found.\n");
                encoding_type = ENC_TYPE_AUDIO;
            }
        }

        if (encoding_type == ENC_TYPE_NONE)
            goto compress;

        original_filename = strstr(char_ptr2, "Content-Disposition:");
        if (original_filename == NULL)
        {
            encoding_type = ENC_TYPE_NONE;
            goto compress;
        }

        char_ptr3 = strstr(char_ptr2, "Content-Transfer-Encoding: base64");
        if (char_ptr3 == NULL)
        {
            encoding_type = ENC_TYPE_NONE;
            goto compress;
        }

        char_ptr4 = strstr(char_ptr3, "\n\n");

        if (char_ptr4 == NULL)
        {
            encoding_type = ENC_TYPE_NONE;
            goto compress;
        }

        char_ptr4++; char_ptr4++;

        char_ptr5 = strstr(char_ptr4, "\n\n");

        if (char_ptr5 == NULL)
        {
            encoding_type = ENC_TYPE_NONE;
            goto compress;
        }

//        printf("char_ptr4: %s\n", char_ptr4);

//        printf("char_ptr5: %s\n", char_ptr5);

        // *****************************
        // convert from base64 to binary

        base64_decodestate b64_state;
        base64_init_decodestate(&b64_state);

        blob = malloc(char_ptr5 - char_ptr4); // the decoded base64 payload will always be smaller...

        int media_size = base64_decode_block(char_ptr4, char_ptr5 - char_ptr4, blob, &b64_state);

        if (encoding_type == ENC_TYPE_IMAGE)
        {
            sprintf(tmp_media_filename, "/tmp/uucomp.%d.jpg", getpid ());
            sprintf(tmp_encoded_media_filename, "/tmp/uucomp.%d.vvc", getpid ());
        }

        if (encoding_type == ENC_TYPE_AUDIO)
        {
            sprintf(tmp_media_filename, "/tmp/uucomp.%d.aac", getpid ());
            sprintf(tmp_encoded_media_filename, "/tmp/uucomp.%d.lpcnet", getpid ());
        }

        tmp_media = fopen(tmp_media_filename, "w");

        if (tmp_media == NULL)
        {
            printf("Failure to open %s\n", tmp_media_filename);
            encoding_type = ENC_TYPE_NONE;
            free(blob);
            goto compress;
        }

        fwrite(blob, 1, media_size , tmp_media);
        fclose(tmp_media);
        free(blob);

        // ********************
        // compress the media

        char enc_cmd[MAX_FILENAME];
        if (encoding_type == ENC_TYPE_IMAGE)
        {
            sprintf(enc_cmd, "compress_image.sh %s %s", tmp_media_filename, tmp_encoded_media_filename);
            system(enc_cmd); // or popen ??
        }

        if (encoding_type == ENC_TYPE_AUDIO)
        {
            sprintf(enc_cmd, "compress_audio.sh %s %s", tmp_media_filename, tmp_encoded_media_filename);
            system(enc_cmd); // or popen ??
        }

        unlink(tmp_media_filename);

        if (stat(tmp_encoded_media_filename, &st) != 0)
        {
            printf("%s could not be opened.\n", tmp_encoded_media_filename);
            goto compress;
        }
        file_size = st.st_size;

        printf("%s size is: %ld\n", tmp_encoded_media_filename, file_size);

        if (file_size == 0)
            goto compress;

        // *************************************************
        // write new D. file... - In place...
        d_file = fopen(d_filename, "w");

        fwrite(d_payload, char_ptr2 - d_payload, 1, d_file);

        if (encoding_type == ENC_TYPE_IMAGE)
        {
            fprintf(d_file, "Content-Type: image/x-vvc\n");
        }

        if (encoding_type == ENC_TYPE_AUDIO)
        {
            fprintf(d_file, "Content-Type: audio/x-lpcnet\n");
        }

        // write the old filename...
        fwrite(original_filename, char_ptr3 - original_filename, 1, d_file);

        blob = malloc(file_size);

        tmp_media = fopen(tmp_encoded_media_filename, "r");
        fread(blob, file_size, 1, tmp_media);
        fclose(tmp_media);

        unlink(tmp_encoded_media_filename);

        fwrite(blob, file_size, 1, d_file);

        free(blob);

        fclose(d_file);

        // search for: "Content-Type: image/jpeg"
        // search for: "Content-Type: audio/aac"
        // Open the payload D. file
        // check if email contains image or audio
        // if yes, compress the media and then the whole email with gzip (and add an
        // identification in the payload indicating compressed media attached)
        // othewise just compress with gzip

    compress:
       printf("Compressing now.\n");

        char compress_cmd[MAX_FILENAME];
        sprintf (compress_cmd, "gzip -9c -fc > /tmp/uucomp.%d < %s; cp /tmp/uucomp.%d %s",
                 getpid (), d_filename, getpid (), d_filename);
        system (compress_cmd);

        sprintf (compress_cmd, "/tmp/uucomp.%d", getpid ());
        unlink (compress_cmd);

// re-writing C. file
        c_file = fopen(c_filename, "w");
        if (c_file == NULL)
        {
            printf("%s could not be opened.\n", c_filename);
            continue;
        }

        sprintf(uu_cmd, "crmail");

        fprintf(c_file, "%s %s %s %s %s %s %s %s %s %s %s\n", field1,
                field2, field3, field4, field5, field6, field7, field8, field9,
                uu_cmd, field11);

        fclose(c_file);

        free(d_payload);
    }

    return EXIT_SUCCESS;
}
