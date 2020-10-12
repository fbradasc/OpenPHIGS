
TOP_SRCDIR=../../..

MODULES = shapes.c simple.c

TASKS	= ../../bin/shapes ../../bin/simple
CLEAN	= $(TASKS)

all: $(TASKS)

include $(TOP_SRCDIR)/makeinclude

../../bin/shapes: shapes.o
	$(CC) shapes.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/simple: simple.o
	$(CC) simple.o $(LDLIBS) $(DEBUGFLAGS) -o $@
