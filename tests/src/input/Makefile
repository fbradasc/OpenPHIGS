
TOP_SRCDIR=../../..

MODULES = move.c movex.c pick.c pickx.c
TASKS	= ../../bin/move ../../bin/movex ../../bin/pick ../../bin/pickx
CLEAN	= $(TASKS)

all: $(TASKS)

.deps: western_europe.h

include $(TOP_SRCDIR)/makeinclude

../../bin/move: move.o western_europe.h
	$(CC) move.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/movex: movex.o ../xbasic/openxws.o western_europe.h
	$(CC) movex.o ../xbasic/openxws.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/pick: pick.o western_europe.h
	$(CC) pick.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/pickx: pickx.o ../xbasic/openxws.o western_europe.h
	$(CC) pickx.o ../xbasic/openxws.o $(LDLIBS) $(DEBUGFLAGS) -o $@

western_europe.h: ../namesets/western_europe.h
	$(LN) $? $@
