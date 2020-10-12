
TOP_SRCDIR=../../..

MODULES = circle.c curves.c plineset.c polyline.c
TASKS	= ../../bin/circle ../../bin/curves ../../bin/plineset \
	../../bin/polyline
CLEAN	= $(TASKS)

all: $(TASKS)

include $(TOP_SRCDIR)/makeinclude

../../bin/circle: circle.o
	$(CC) circle.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/curves: curves.o
	$(CC) curves.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/plineset: plineset.o
	$(CC) plineset.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/polyline: polyline.o
	$(CC) polyline.o $(LDLIBS) $(DEBUGFLAGS) -o $@
