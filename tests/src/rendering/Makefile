
TOP_SRCDIR=../../..

MODULES = sphere.c sphere2.c sphere3.c sphere4.c dcue.c light_tool.c
TASKS	= ../../bin/sphere ../../bin/sphere2 ../../bin/sphere3 \
	../../bin/sphere4 ../../bin/dcue ../../bin/light_tool
CLEAN	= $(TASKS)

all: $(TASKS)

include $(TOP_SRCDIR)/makeinclude

../../bin/sphere: sphere.o
	$(CC) sphere.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/sphere2: sphere2.o
	$(CC) sphere2.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/sphere3: sphere3.o
	$(CC) sphere3.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/sphere4: sphere4.o
	$(CC) sphere4.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/dcue: dcue.o
	$(CC) dcue.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/light_tool: light_tool.o
	$(CC) light_tool.o $(MOTIF_LIBS) $(LDLIBS) $(DEBUGFLAGS) -o $@
