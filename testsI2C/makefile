CC=g++
SOURCES=./sources
LLIB=./lib
LDEFS=./defs
BIN=./bin


all : lib
	$(CC) $(SOURCES)/prgThread.cpp $(SOURCES)/LectureI2C.cpp $(LLIB)/sockets.o -lpthread -o $(BIN)/LectureI2C   
	$(CC) $(SOURCES)/clientLecture.cpp $(LLIB)/sockets.o -o $(BIN)/clientLecture

lib: $(LLIB)/sockets.o

$(LLIB)/sockets.o: $(SOURCES)/sockets.cpp $(LDEFS)/sockets.h
	$(CC) -c $(SOURCES)/sockets.cpp -o $@ 
