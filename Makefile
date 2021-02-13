prog_name = patchee
COMP_MODE = release
COMP_OPT = -O0
objects = main.o scheduler.o database.o sort.o
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
main.o: main.c scheduler.c scheduler.h
	gcc -c $(cflags) main.c -o main.o
scheduler.o: scheduler.c scheduler.h database.c database.h sort.c sort.h
	gcc -c $(cflags) scheduler.c -o scheduler.o
database.o: database.c
	gcc -c $(cflags) database.c -o database.o
sort.o: database.c database.h sort.c sort.h
	gcc -c $(cflags) sort.c -o sort.o

PHONY: clean

clean:
	rm -f $(objects)
	rm -f *.log
