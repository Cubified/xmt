all: xmt

CC=gcc

LIBS=-lX11
CFLAGS=-Os -pipe -s -ansi -pedantic
DEBUGCFLAGS=-Og -pipe -g

INPUT=src/* xmt.c
OUTPUT=xmt

RM=/bin/rm

.PHONY: xmt
xmt:
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(CFLAGS)

debug:
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(DEBUGCFLAGS)

clean:
	if [ -e $(OUTPUT) ]; then $(RM) $(OUTPUT); fi
