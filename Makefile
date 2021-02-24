PROGNAME=kuakua
CC=gcc
CMODE=release
CUSTOMOPT=3

objects= main.o scheduler.o sort.o database.o io.o fileio.o
cflags= -Wall -Wextra
optlevel=3
debuglevel=0

testflags= -O3 -g0


ifeq ($(CMODE),release)
	optlevel=3
	debuglevel=0
endif
ifeq ($(CMODE),debug)
	optlevel=0
	debuglevel=3
endif
ifeq ($(CMODE),test)
	optlevel=$(CUSTOMOPT)
	debuglevel=3
	objects+= test.o
endif
cflags+= -O$(optlevel) -g$(debuglevel)

all: $(PROGNAME)

$(PROGNAME): $(objects)
	(gcc 2>&1 $(objects) -o $(PROGNAME)) > clogs/linker.log
main.o: scheduler.h scheduler.c
scheduler.o: scheduler.h scheduler.c database.h database.c sort.h sort.c io.c io.h\
fileio.h fileio.c
database.o: database.h database.c
sort.o: sort.h sort.c
io.o: io.h io.c
fileio.o: fileio.h fileio.c		

%.o: %.c
	($(CC) -c $(cflags) 2>&1 $< -o $@) > clogs/$<.log

.PHONY: clean

clean:
	rm -f $(objects)
	rm -rf clogs/*.log
