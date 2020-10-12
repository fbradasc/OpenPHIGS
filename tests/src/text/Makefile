
TOP_SRCDIR=../../..

MODULES = text.c extent.c

TASKS	= ../../bin/text ../../bin/extent
CLEAN	= $(TASKS)

all: $(TASKS)

include $(TOP_SRCDIR)/makeinclude

../../bin/text: text.o
	$(CC) text.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/extent: extent.o ../xbasic/openxws.o
	$(CC) extent.o ../xbasic/openxws.o $(LDLIBS) $(DEBUGFLAGS) -o $@
