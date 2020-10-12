##
# $XConsortium: Makefile,v 5.4 94/04/17 20:54:03 hersh Exp $
##
## 
## Copyright (c) 1990, 1991 by Sun Microsystems, Inc. and the X Consortium.
## 
##			 All Rights Reserved
## 
## Permission to use, copy, modify, and distribute this software and its 
## documentation for any purpose and without fee is hereby granted, 
## provided that the above copyright notice appear in all copies and that
## both that copyright notice and this permission notice appear in 
## supporting documentation, and that the names of Sun Microsystems
## and the X Consortium not be used in advertising or publicity 
## pertaining to distribution of the software without specific, written 
## prior permission.  
## 
## SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
## INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
## EVENT SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
## CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
## USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
## OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
## PERFORMANCE OF THIS SOFTWARE.

.SUFFIXES: .opm .html

MAN_TO_HTML=\
	@$(ECHO) "processing $< --> $@" ; \
	$(CAT) $(PHIGSMAN) $< | \
	$(TROFF) -S -s -p -t -e -Tascii -mandoc - | \
	./man2html -noheads -seealso -compress \
		-cgiurl "`basename $< .opm`" -title "`basename $< .opm`" \
	> /tmp/MAN_TO_HTML.$$ && mv /tmp/MAN_TO_HTML.$$ $@ ;

TOP_SRCDIR=../..

task: distrib install

include $(TOP_SRCDIR)/makeinclude

PHIGSMAN = phigs.macros
NAMES = names.make

include $(NAMES)

distrib:
	-@mkdir $(PHIGS_DOCS)/html

install: manpages $(NAMES)
	-\cp htmlsrc/* $(PHIGS_DOCS)/html/. 2>/dev/null
	tar -cvf - manpages | tar -C $(PHIGS_DOCS) -xvf - 2>/dev/null
	./link_phigs_man.sh $(PHIGS_DOCS)
