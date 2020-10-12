
TOP_SRCDIR=../../..

MODULES = error.c
TASKS	= ../../bin/error
CLEAN	= $(TASKS)

all: $(TASKS)

include $(TOP_SRCDIR)/makeinclude

../../bin/error: error.o
	$(CC) error.o $(LDLIBS) $(DEBUGFLAGS) -o $@
