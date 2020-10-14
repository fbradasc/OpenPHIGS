
TOP_SRCDIR=../../..

MODULES = map.c

TASKS	= ../../bin/map
CLEAN	= $(TASKS)
TESTS_INCLUDES = -I../common

all: $(TASKS)

include $(TOP_SRCDIR)/makeinclude

../../bin/map: map.o
	$(CC) map.o $(LDLIBS) $(DEBUGFLAGS) -o $@
