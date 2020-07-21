all: lib xmt

CC=gcc
TCC=tcc

LIBS=-lm -lpthread -lX11
CFLAGS=-Os -pipe -s -Ilib -Ilib/wsServer/include
DEBUGCFLAGS=-Og -pipe -g -Ilib -Ilib/wsServer/include

INPUT=src/* xmt.c lib/wsServer/libws.a lib/miniz.c
OUTPUT=xmt

RM=/bin/rm

.PHONY: lib
lib:
	$(MAKE) -s -C lib/wsServer

.PHONY: xmt
xmt:
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(CFLAGS)

fast:
	$(TCC) $(INPUT) -o $(OUTPUT) $(LIBS) $(CFLAGS) -DSTBI_NO_SIMD

debug:
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(DEBUGCFLAGS)

clean:
	if [ -e $(OUTPUT) ]; then $(RM) $(OUTPUT); fi
