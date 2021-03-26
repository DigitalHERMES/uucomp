#
# Makefile generated with genmake - version 1.1  08/22/92
#
# genmake is Copyright 1991 by Edwin R. Carp
#
# GENMAKE -B/usr/bin -tsp [files]
#

CC = gcc -Wall
CFLAGS = $(INCLUDE)
SOURCES = crmail.c crnews.c uucomp.c frmail.c
OBJECTS = crmail.o crnews.o uucomp.o frmail.o
EXECUTABLES = crmail crnews uucomp frmail
PROGRAMS = crmail crnews uucomp  frmail

PREFIX=/usr

all: $(PROGRAMS) tags

crmail: crmail.o uucomp.h
	$(CC) $(CFLAGS) -o crmail crmail.o $(LDFLAGS)  -O

frmail: frmail.o uucomp.h
	$(CC) $(CFLAGS) -o frmail frmail.o $(LDFLAGS)  -O

crnews: crnews.o uucomp.h
	$(CC) $(CFLAGS) -o crnews crnews.o $(LDFLAGS)  -O

uucomp: uucomp.o uucomp.h
	$(CC) $(CFLAGS) -o uucomp uucomp.o $(LDFLAGS)  -O

install:
	install $(PROGRAMS) $(PREFIX)/bin/

clean:
	/bin/rm -f $(OBJECTS) $(EXECUTABLES) MAKELOG eddep makedep

clobber:
	/bin/rm -f $(OBJECTS) $(PROGRAMS) MAKELOG eddep makedep *~ *.bak *.BAK
	/bin/rm -f tags

hidden:
	echo "make all > MAKELOG 2>&1 &"|/bin/sh

makefile:
	genmake -B/usr/bin -tsp $(SOURCES) &

makeall:
	genmake -B/usr/bin -tsp *.c &

tags:	$(SOURCES)
	ctags $(SOURCES) > tags

