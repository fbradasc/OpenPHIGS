
TOP_SRCDIR=../../..

MODULES = css_probe.c paths.c
TASKS	= ../../bin/css_probe ../../bin/paths
CLEAN	= $(TASKS)

all: $(TASKS)

include $(TOP_SRCDIR)/makeinclude

../../bin/css_probe: css_probe.o
	$(CC) css_probe.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/paths: paths.o
	$(CC) paths.o $(LDLIBS) $(DEBUGFLAGS) -o $@
