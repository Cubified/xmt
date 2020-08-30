all: lib html xmt

CC=gcc
TCC=tcc

LIBS=-lm -lpthread -lX11 -lXdamage -lXfixes
CFLAGS=-Os -pipe -s -Ilib -Ilib/wsServer/include
DEBUGCFLAGS=-Og -pipe -g -Ilib -Ilib/wsServer/include

INPUT=src/* xmt.c lib/wsServer/libws.a lib/miniz.c
OUTPUT=xmt

HTMLIN=web/web.html
HTMLOUT=include/web.html.h

RM=/bin/rm

INSTALLDIR=$(HOME)/.local/bin

.PHONY: lib
lib:
	$(MAKE) -s -C lib/wsServer

html:
	xxd -i $(HTMLIN) > $(HTMLOUT)

.PHONY: xmt
xmt:
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(CFLAGS)

fast:
	xxd -i $(HTMLIN) > $(HTMLOUT)
	$(TCC) $(INPUT) -o $(OUTPUT) $(LIBS) $(CFLAGS) -DSTBI_NO_SIMD

debug:
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(DEBUGCFLAGS)

install:
	test -d $(INSTALLDIR) || mkdir -p $(INSTALLDIR)
	install -pm 755 $(OUTPUT) $(INSTALLDIR)

uninstall:
	$(RM) $(INSTALLDIR)/$(OUTPUT)

clean:
	if [ -e $(OUTPUT) ]; then $(RM) $(OUTPUT); fi
