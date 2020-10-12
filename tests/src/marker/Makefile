
TOP_SRCDIR=../../..

MODULES = marker.c
TASKS	= ../../bin/marker
CLEAN	= $(TASKS)

all: $(TASKS)

include $(TOP_SRCDIR)/makeinclude

../../bin/marker: marker.o
	$(CC) marker.o $(LDLIBS) $(DEBUGFLAGS) -o $@
