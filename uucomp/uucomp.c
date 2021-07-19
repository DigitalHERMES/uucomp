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

#define MAX_C_PAYLOAD 4096

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

int main (int argc, char *argv[])
{
    FILE *c_file = NULL;
    char c_payload[MAX_C_PAYLOAD];


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
        if (strncmp (argv[i], "C.", 2) != 0)
        {
            printf("%s not a \"C.\" file, skipping.\n", argv[i]);
            continue;
        }
        c_file = fopen(argv[i], "r");
        if (c_file == NULL)
        {
            printf("%s could not be opened.\n", argv[i]);
            continue;
        }

        fread(c_payload ,1, MAX_C_PAYLOAD, c_file);

        c_payload[strlen(c_payload) - 1] = 0;

        printf("%s payload: %s\n", argv[i], c_payload);

        fclose(c_file);

    }

    return EXIT_SUCCESS;
}
