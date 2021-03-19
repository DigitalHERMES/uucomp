#
# Makefile generated with genmake - version 1.1  08/22/92
#
# genmake is Copyright 1991 by Edwin R. Carp
#
# GENMAKE -B/usr/bin -tsp [files]
#

CC = gcc -O6
CFLAGS =  $(INCLUDE)
SOURCES = crmail.c crnews.c uucomp.c 
OBJECTS = crmail.o crnews.o uucomp.o 
PROGRAMS = /usr/bin/crmail /usr/bin/crnews /usr/bin/uucomp 

all: $(PROGRAMS) tags

/usr/bin/crmail: crmail.o 
	$(CC) $(CFLAGS) -o crmail crmail.o $(LDFLAGS)  -O
	strip crmail
	chmod 755 crmail
	mv crmail /usr/bin

/usr/bin/crnews: crnews.o 
	$(CC) $(CFLAGS) -o crnews crnews.o $(LDFLAGS)  -O
	strip crnews
	chmod 755 crnews
	mv crnews /usr/bin

/usr/bin/uucomp: uucomp.o 
	$(CC) $(CFLAGS) -o uucomp uucomp.o $(LDFLAGS)  -O
	strip uucomp
	chmod 755 uucomp
	mv uucomp /usr/bin

clean:
	/bin/rm -f $(OBJECTS) MAKELOG eddep makedep

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

