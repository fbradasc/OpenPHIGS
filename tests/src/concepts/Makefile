
TOP_SRCDIR=../../..

MODULES = billions.c billions_color.c billions_ind.c buttons.c first.c \
	houses.c sedit.c

TASKS	= ../../bin/billions ../../bin/billions_color ../../bin/billions_ind \
	../../bin/buttons ../../bin/first ../../bin/houses ../../bin/sedit
CLEAN	= $(TASKS)

all: $(TASKS)

include $(TOP_SRCDIR)/makeinclude

../../bin/billions: billions.o
	$(CC) billions.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/billions_color: billions_color.o
	$(CC) billions_color.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/billions_ind: billions_ind.o
	$(CC) billions_ind.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/buttons: buttons.o
	$(CC) buttons.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/first: first.o
	$(CC) first.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/houses: houses.o
	$(CC) houses.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/sedit: sedit.o
	$(CC) sedit.o $(LDLIBS) $(DEBUGFLAGS) -o $@
