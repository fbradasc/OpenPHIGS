
TOP_SRCDIR=../../..

MODULES = bundles.c

TASKS	= ../../bin/bundles

CLEAN	= $(TASKS)

all: $(TASKS)

include $(TOP_SRCDIR)/makeinclude

../../bin/bundles: bundles.o
	$(CC) bundles.o $(LDLIBS) $(DEBUGFLAGS) -o $@
