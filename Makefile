all: xmt

CC=gcc
TCC=tcc

LIBS=-lm -lX11
CFLAGS=-Os -pipe -s
DEBUGCFLAGS=-Og -pipe -g

INPUT=src/* xmt.c
OUTPUT=xmt

RM=/bin/rm

.PHONY: xmt
xmt:
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(CFLAGS)

fast:
	$(TCC) $(INPUT) -o $(OUTPUT) $(LIBS) $(CFLAGS) -DSTBI_NO_SIMD

debug:
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(DEBUGCFLAGS)

clean:
	if [ -e $(OUTPUT) ]; then $(RM) $(OUTPUT); fi
