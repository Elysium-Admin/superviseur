CC=g++
CFLAGS=-Wall 
LDFLAGS=
SOURCES=./src
LLIB=./lib
LLIBPRJ=../supervision/lib
LDEFS=./defs

all:   $(LLIB)/Analyseur.o $(SOURCES)/simucgistream.cpp $(SOURCES)/simucgicmd.cpp
	$(CC)  $(SOURCES)/simucgistream.cpp -l pthread -lm $(LLIBPRJ)/sockets.o $(LLIB)/Analyseur.o  -o simucgistream
	$(CC)  $(SOURCES)/simucgicmd.cpp $(LLIBPRJ)/sockets.o -o simucgicmd
	$(CC)  $(SOURCES)/lanceCmd.cpp $(LLIBPRJ)/sockets.o -o lanceCmd
	$(CC)  $(SOURCES)/rejouer.cpp  -l pthread -o rejouer
	$(CC)  $(SOURCES)/playeur.cpp  $(LLIB)/Analyseur.o -o playeur

	cp -f simucgistream ./bin
	cp -f simucgicmd ./bin
	cp -f ./scripts/* ./bin
	rm ./bin/lanceurDeCmd
	rm ./bin/startSuperviseur

$(LLIB)/Analyseur.o : $(SOURCES)/Analyseur.cpp $(LDEFS)/Analyseur.h
	$(CC) -c $(SOURCES)/Analyseur.cpp -o $@ $(CFLAGS)

clean:
	rm -f = $(LLIB)/* 
	rm -f simucgicmd simucgistream playeur rejouer lanceCmd

.PHONY: clean 


