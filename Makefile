prog_name = prog
COMP_MODE = release
COMP_OPT = -O0
objects = main.o scheduler.o
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
	gcc $(cflags) $(objects) -o $(prog_name)
main.o: defs.h
scheduler.o: scheduler.h

PHONY: clean

clean:
	rm -f $(objects)
