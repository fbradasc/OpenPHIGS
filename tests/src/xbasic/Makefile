
TOP_SRCDIR=../../..

MODULES = ux_xlib.c ux_xlib_rr.c Motif.c Motif_rr.c openxws.c
TASKS	= ../../bin/ux_xlib ../../bin/ux_xlib_rr ../../bin/Motif \
	../../bin/Motif_rr
CLEAN	= $(TASKS)

all: $(TASKS) openxws.o

include $(TOP_SRCDIR)/makeinclude

../../bin/ux_xlib: ux_xlib.o
	$(CC) ux_xlib.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/ux_xlib_rr: ux_xlib_rr.o
	$(CC) ux_xlib_rr.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/Motif: Motif.o
	$(CC) Motif.o $(MOTIF_LIBS) $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/Motif_rr: Motif_rr.o
	$(CC) Motif_rr.o $(MOTIF_LIBS) $(LDLIBS) $(DEBUGFLAGS) -o $@
