#
# test makefile for OpenPHIGS
#
# Copyright (c) 1989, 1990, 1991  X Consortium
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
# X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
# AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# 
# Except as contained in this notice, the name of the X Consortium shall not be
# used in advertising or otherwise to promote the sale, use or other dealings
# in this Software without prior written authorization from the X Consortium.
# 
# Copyright 1989, 1990, 1991 by Sun Microsystems, Inc. 
# 
#                         All Rights Reserved
# 
# Permission to use, copy, modify, and distribute this software and its 
# documentation for any purpose and without fee is hereby granted, 
# provided that the above copyright notice appear in all copies and that
# both that copyright notice and this permission notice appear in 
# supporting documentation, and that the name of Sun Microsystems,
# not be used in advertising or publicity pertaining to distribution of 
# the software without specific, written prior permission.  
# 
# SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
# INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
# SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
# DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
# WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
# ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
# SOFTWARE.
#
# Please report all bugs and problems to "fbradasc@ciaoweb.it".
#
SHELL=/bin/sh

SUBDIRS = \
	src/archive src/areas src/bundles src/concepts src/css src/error \
	src/input src/inquiry src/lines src/marker src/modeling src/namesets \
	src/rendering src/text src/viewing src/ws src/xadvanced src/xbasic

all: bin
	@for dir in $(SUBDIRS); do\
		echo "=== making $$dir ===";\
		if test ! -f $$dir/.deps; then\
			touch $$dir/.deps;\
		fi;\
		(cd $$dir;$(MAKE));\
	done

install:
	@for dir in $(SUBDIRS); do\
		echo "=== installing $$dir ===";\
		if test ! -f $$dir/.deps; then\
			touch $$dir/.deps;\
		fi;\
		(cd $$dir;$(MAKE) install);\
	done

depend: $(SUBDIRS)
	@for dir in $(SUBDIRS); do\
		echo "=== making dependencies in $$dir ===";\
		if test ! -f $$dir/.deps; then\
			touch $$dir/.deps;\
		fi;\
		(cd $$dir;$(MAKE) depend);\
	done

clean:
	-@ rm -f core config.cache *.o *.bck
	@for dir in $(SUBDIRS); do\
		if test ! -f $$dir/.deps; then\
			touch $$dir/.deps;\
		fi;\
		echo "=== cleaning $$dir ===";\
		(cd $$dir;$(MAKE) clean);\
	done

bin:
	-@mkdir bin
