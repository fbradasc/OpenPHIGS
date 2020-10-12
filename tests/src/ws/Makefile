
TOP_SRCDIR=../../..

MODULES = ws_is_open.c

TASKS	= ../../bin/ws_is_open
CLEAN	= $(TASKS)

all: $(TASKS)

include $(TOP_SRCDIR)/makeinclude

../../bin/ws_is_open: ws_is_open.o
	$(CC) ws_is_open.o $(LDLIBS) $(DEBUGFLAGS) -o $@
