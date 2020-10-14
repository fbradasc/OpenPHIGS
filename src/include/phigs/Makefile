
TOP_SRCDIR=../../..

all:includes

CLEAN	= $(PHIGSINCDIR)/phigs.h \
	$(PHIGSINCDIR)/attr.h \
	$(PHIGSINCDIR)/errnum.h \
	$(PHIGSINCDIR)/phigscfunc.h \
	$(PHIGSINCDIR)/phigsextrn.h \
	$(PHIGSINCDIR)/phigserr.h \
	$(PHIGSAPIDIR)/PHIGSerr.txt \
	$(PHIGSAPIDIR)/PHIGSfnc.txt
	
include $(TOP_SRCDIR)/makeinclude

includes: $(PHIGSINCDIR)/phigs.h \
	$(PHIGSINCDIR)/attr.h \
	$(PHIGSINCDIR)/errnum.h \
	$(PHIGSINCDIR)/phigscfunc.h \
	$(PHIGSINCDIR)/phigsextrn.h \
	$(PHIGSINCDIR)/phigserr.h \
	$(PHIGSAPIDIR)/PHIGSerr.txt \
	$(PHIGSAPIDIR)/PHIGSfnc.txt

$(PHIGSINCDIR)/phigs.h: phigs.h
	$(CP) $< $@

$(PHIGSINCDIR)/attr.h: attr.h
	$(CP) $< $@

$(PHIGSINCDIR)/errnum.h: errnum.h
	$(CP) $< $@

$(PHIGSINCDIR)/phigscfunc.h: phigscfunc.h
	$(CP) $< $@

$(PHIGSINCDIR)/phigsextrn.h: phigsextrn.h
	$(CP) $< $@

$(PHIGSINCDIR)/phigserr.h: phigserr.h
	$(CP) $< $@

$(PHIGSAPIDIR)/PHIGSerr.txt: PHIGSerr.txt
	$(CP) $< $@

$(PHIGSAPIDIR)/PHIGSfnc.txt: PHIGSfnc.txt
	$(CP) $< $@

errnum.h: phigserr.h errgen.awk
	$(AWK) -f errgen.awk phigserr.h > $@

PHIGSerr.txt: phigserr.h errmsgen.awk
	$(AWK) -f errmsgen.awk phigserr.h > $@
