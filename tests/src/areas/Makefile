
TOP_SRCDIR=../../..

MODULES = fas.c fasd.c fillarea.c quad.c sofas.c surface.c surface_trimmed.c \
	tristrip.c

TASKS	= ../../bin/fas ../../bin/fasd ../../bin/fillarea ../../bin/quad \
	../../bin/sofas ../../bin/surface ../../bin/surface_trimmed \
	../../bin/tristrip

CLEAN	= $(TASKS)

all: $(TASKS)

include $(TOP_SRCDIR)/makeinclude

../../bin/fas: fas.o
	$(CC) fas.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/fasd: fasd.o
	$(CC) fasd.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/fillarea: fillarea.o
	$(CC) fillarea.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/quad: quad.o
	$(CC) quad.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/sofas: sofas.o
	$(CC) sofas.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/surface: surface.o
	$(CC) surface.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/surface_trimmed: surface_trimmed.o
	$(CC) surface_trimmed.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/tristrip: tristrip.o
	$(CC) tristrip.o $(LDLIBS) $(DEBUGFLAGS) -o $@
