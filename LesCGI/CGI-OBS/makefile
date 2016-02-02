CC=g++
CFLAGS=-Wall 
LDFLAGS=
SOURCES=./src
LLIB=./lib
LLIBPRJ=../../supervision/lib
LDEFS=./defs

all: $(SOURCES)/cgiStream.cpp $(SOURCES)/cgiStream.cpp
	$(CC)  $(SOURCES)/cgiStream.cpp  $(LLIBPRJ)/sockets.o   -o cgiStream

clean:
	rm -f cgiCommande cgiStream

.PHONY: clean 

