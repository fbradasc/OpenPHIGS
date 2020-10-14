
TOP_SRCDIR=../../..

MODULES = move.c movex.c pick.c pickx.c
TASKS	= ../../bin/move ../../bin/movex ../../bin/pick ../../bin/pickx
CLEAN	= $(TASKS)
TESTS_INCLUDES = -I../common

all: $(TASKS)

include $(TOP_SRCDIR)/makeinclude

../../bin/move: move.o
	$(CC) move.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/movex: movex.o ../xbasic/openxws.o
	$(CC) movex.o ../xbasic/openxws.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/pick: pick.o
	$(CC) pick.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/pickx: pickx.o ../xbasic/openxws.o
	$(CC) pickx.o ../xbasic/openxws.o $(LDLIBS) $(DEBUGFLAGS) -o $@
