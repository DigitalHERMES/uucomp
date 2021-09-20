# uBitxv6 Rhizomatica tools
# Copyright (C) 2019-2021 Rhizomatica
# Author: Rafael Diniz <rafael@riseup.net>
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

CC=gcc
prefix=/usr
CFLAGS=-g -Wall -pedantic -Wno-format-overflow -std=gnu11 -pthread -fstack-protector
LDFLAGS=-lb64

all: uucomp crmail

uucomp: uucomp.c
	$(CC) $(CFLAGS) uucomp.c -o uucomp $(LDFLAGS)

crmail: crmail.c
	$(CC) $(CFLAGS) crmail.c -o crmail $(LDFLAGS)

.PHONY: clean install
clean:
	rm -f *.o uucomp crmail

install: uucomp crmail
	install -D uucomp $(DESTDIR)$(prefix)/bin/uucomp
	install -D crmail $(DESTDIR)$(prefix)/bin/crmail
