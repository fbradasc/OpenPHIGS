
TOP_SRCDIR=../../..

MODULES = ux_xlib2.c Motif2.c ux_visual.c
TASKS	= ../../bin/ux_xlib2 ../../bin/Motif2 ../../bin/ux_visual
CLEAN	= $(TASKS)

all: $(TASKS)

include $(TOP_SRCDIR)/makeinclude

../../bin/ux_xlib2: ux_xlib2.o
	$(CC) ux_xlib2.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/Motif2: Motif2.o
	$(CC) Motif2.o $(MOTIF_LIBS) $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/ux_visual: ux_visual.o
	$(CC) ux_visual.o $(LDLIBS) $(DEBUGFLAGS) -o $@
