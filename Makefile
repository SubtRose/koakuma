PROGNAME=kuakua
CC=gcc
CMODE=test
CUSTOMOPT=0
COPY=cp

objects= main.o sort.o database.o io.o fileio.o
tobjects= testenv/generator.o testenv/testio.o
theader= testenv/zero
srclist= main.c database.h sort.h io.h fileio.h test.h
tmp= test.h
optlevel=3
debuglevel=0
progdir:= $(shell pwd) 

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
	objects+= $(tobjects)
	theader:= testenv/test.h
endif
cflags= -std=c99 -Wall -Wextra -O$(optlevel) -g$(debuglevel)

#####################################################################################################################################################################

all: $(PROGNAME)
$(PROGNAME): $(objects)
	(gcc 2>&1 $(objects) -o $(PROGNAME)) > clog/linker.log
	make remtmp
main.o: $(srclist)
	($(CC) -c $(cflags) -iquote $(progdir) 2>&1 main.c -o main.o) > clog/main.c.log
database.o: database.h database.c
sort.o: sort.h sort.c
io.o: io.h io.c
fileio.o: fileio.h fileio.c

test.h: $(theader)
	$(COPY) $(theader) test.h
testenv/generator.o: testenv/generator.h testenv/generator.c
testenv/testio.o: testenv/testio.h testenv/testio.c
testenv/timer.o: testenv/timer.h testenv/timer.c

%.o: %.c
	echo $(progdir)
	($(CC) -c $(cflags) -iquote $(progdir) 2>&1 $< -o $@) > clog/$<.log

.PHONY: clean remtmp

clean:
	rm -rf $(objects) $(PROGNAME)
	rm -rf clog/*.log clog/testenv/*.log testenv/testlog/*.log
	rm -rf $(tmp)
remtmp:
	rm -rf $(tmp) 
