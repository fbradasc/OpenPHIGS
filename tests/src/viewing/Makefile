
TOP_SRCDIR=../../..

MODULES = view_ori.c pvt.c pvt2.c ride.c twod_view.c
TASKS	= ../../bin/view_ori ../../bin/pvt ../../bin/pvt2 ../../bin/ride \
	../../bin/twod_view
CLEAN	= $(TASKS)

all: $(TASKS)

include $(TOP_SRCDIR)/makeinclude

../../bin/view_ori: view_ori.o ../xbasic/openxws.o
	$(CC) view_ori.o ../xbasic/openxws.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/pvt: pvt.o ../xbasic/openxws.o
	$(CC) pvt.o ../xbasic/openxws.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/pvt2: pvt2.o ../xbasic/openxws.o
	$(CC) pvt2.o ../xbasic/openxws.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/ride: ride.o ../xbasic/openxws.o
	$(CC) ride.o ../xbasic/openxws.o $(LDLIBS) $(DEBUGFLAGS) -o $@

../../bin/twod_view: twod_view.o ../xbasic/openxws.o
	$(CC) twod_view.o ../xbasic/openxws.o $(LDLIBS) $(DEBUGFLAGS) -o $@
