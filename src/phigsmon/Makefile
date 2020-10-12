
TOP_SRCDIR=../..

UCPPFLAGS = -I../cp

MODULES = phigsmon.c

CLEAN	= $(PHIGSAPIDIR)/phigsmon

all: task

include $(TOP_SRCDIR)/makeinclude

task: $(PHIGSAPIDIR)/phigsmon

$(PHIGSAPIDIR)/phigsmon: $(MODULES:.c=.o) 
	$(CC) phigsmon.o $(LDLIBS) $(DEBUGFLAGS) -o $@
