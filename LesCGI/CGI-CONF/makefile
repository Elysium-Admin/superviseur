CC=g++
CFLAGS=-Wall 
LDFLAGS=
SOURCES=./src
LLIB=./lib
LLIBPRJ=../../supervision/lib
LDEFS=./defs

all: $(SOURCES)/cgiStream.cpp $(SOURCES)/cgiCommande.cpp
	$(CC)  $(SOURCES)/cgiStream.cpp  $(LLIBPRJ)/sockets.o   -o cgiStream
	$(CC)  $(SOURCES)/cgiCommande.cpp $(LLIBPRJ)/sockets.o -o cgiCommande

clean:
	rm -f cgiCommande cgiStream

.PHONY: clean 

