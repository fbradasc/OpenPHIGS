
TOP_SRCDIR=../../..

MODULES = mclip.c orbit.c sulfate.c sulfate2.c
TASKS	= ../../bin/mclip ../../bin/orbit ../../bin/sulfate ../../bin/sulfate2
CLEAN	= $(TASKS)

all: $(TASKS)

include $(TOP_SRCDIR)/makeinclude

../../bin/mclip: mclip.o ../xbasic/openxws.o
	$(CC) mclip.o ../xbasic/openxws.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/orbit: orbit.o
	$(CC) orbit.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/sulfate: sulfate.o
	$(CC) sulfate.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/sulfate2: sulfate2.o
	$(CC) sulfate2.o $(LDLIBS) $(DEBUGFLAGS) -o $@
