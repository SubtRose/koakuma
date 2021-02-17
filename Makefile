prog_name = patchee
COMP_MODE = release
COMP_OPT = -O0
objects = database.o sort.o io.o fileio.o
cflags = -Wall -Wextra


ifeq ($(COMP_MODE),release)
	cflags += -O3 -g0
endif
ifeq ($(COMP_MODE),debug)
	cflags += -O0 -g3
endif
ifeq ($(COMP_MODE),test)
	cflags += -g3 $(COMP_OPT)
	objects += test.o
endif


$(prog_name): $(objects)
	gcc  $(objects) -o $(prog_name)
sort.o: sort.c sort.h database.c database.h
	(gcc -c $(cflags) 2>&1 sort.c -o sort.o) > clogs/sort.clog
database.o: database.c database.h
	(gcc -c $(cflags) 2>&1 database.c -o database.o) > clogs/database.clog
io.o: io.c io.h
	(gcc -c $(cflags) 2>&1 io.c -o io.o) > clogs/io.clog
fileio.o: fileio.h fileio.c
	(gcc -c $(cflags) 2>&1 fileio.c -o fileio.o) >clogs/fileio.clog

PHONY: clean

clean:
	rm -f $(objects)
	rm -rf clogs/*.clog
