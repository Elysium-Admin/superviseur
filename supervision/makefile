CC=g++
#CFLAGS=-Wall -std=c++11
CFLAGS=-Wall 
LDFLAGS=
SOURCES=./sources
LLIB=./lib
LDEFS=./defs
DBG=-g

all:   lib
	$(CC)  $(SOURCES)/prgDeSupervision.cpp -l pthread -lm $(LLIB)/Etats.o $(LLIB)/ListeMessages.o $(LLIB)/Empaqueteur.o $(LLIB)/commandes.o $(LLIB)/ProxySEIS.o $(LLIB)/ProxyAutomate.o $(LLIB)/automates.o $(LLIB)/sockets.o $(LLIB)/Superviseur.o $(LLIB)/ICMP.o $(LLIB)/verbose.o -o prgDeSupervision

lib: $(LLIB)/ProxySEIS.o $(LLIB)/Empaqueteur.o $(LLIB)/sockets.o $(LLIB)/Etats.o $(LLIB)/ProxyAutomate.o $(LLIB)/automates.o $(LLIB)/commandes.o $(LLIB)/Superviseur.o $(LLIB)/ListeMessages.o $(LLIB)/ICMP.o $(LLIB)/verbose.o

$(LLIB)/Empaqueteur.o: $(SOURCES)/Empaqueteur.cpp $(LDEFS)/Empaqueteur.h
	$(CC) -c $(SOURCES)/Empaqueteur.cpp -o $@ $(CFLAGS)

$(LLIB)/commandes.o: $(SOURCES)/commandes.cpp $(LDEFS)/commandes.h
	$(CC) -c $(SOURCES)/commandes.cpp -o $@ $(CFLAGS)

$(LLIB)/sockets.o: $(SOURCES)/sockets.c $(LDEFS)/sockets.h
	$(CC) -c $(SOURCES)/sockets.c -o $@ $(CFLAGS)

$(LLIB)/ProxySEIS.o: $(SOURCES)/ProxySEIS.cpp $(LDEFS)/ProxySEIS.h
	$(CC) -c $(SOURCES)/ProxySEIS.cpp -o $@ $(CFLAGS)

$(LLIB)/Etats.o: $(SOURCES)/Etats.cpp $(LDEFS)/Etats.h
	$(CC) $(DBG) -c $(SOURCES)/Etats.cpp -o $@ $(CFLAGS)

$(LLIB)/ProxyAutomate.o: $(SOURCES)/ProxyAutomate.cpp $(LDEFS)/ProxyAutomate.h
	$(CC) -c $(SOURCES)/ProxyAutomate.cpp -o $@ $(CFLAGS)

$(LLIB)/automates.o: $(SOURCES)/automates.cpp $(LDEFS)/automates.h
	$(CC) -c $(SOURCES)/automates.cpp -o $@ $(CFLAGS)

$(LLIB)/ListeMessages.o:  $(SOURCES)/ListeMessages.cpp $(LDEFS)/ListeMessages.h
	$(CC) -c $(SOURCES)/ListeMessages.cpp -o $@ $(CFLAGS)

$(LLIB)/Superviseur.o:  $(SOURCES)/Superviseur.cpp $(LDEFS)/Superviseur.h
	$(CC) $(DBG) -c $(SOURCES)/Superviseur.cpp -o $@ $(CFLAGS)

$(LLIB)/ICMP.o:  $(SOURCES)/ICMP.cpp $(LDEFS)/ICMP.h
	$(CC) $(DBG) -c $(SOURCES)/ICMP.cpp -o $@ $(CFLAGS)

$(LLIB)/verbose.o:  $(SOURCES)/verbose.cpp $(LDEFS)/verbose.h
	$(CC) $(DBG) -c $(SOURCES)/verbose.cpp -o $@ $(CFLAGS)

dbg: lib 
	$(CC) $(DBG)  $(SOURCES)/prgDeSupervision.cpp -l pthread -lm $(LLIB)/Etats.o $(LLIB)/ListeMessages.o $(LLIB)/Empaqueteur.o $(LLIB)/commandes.o $(LLIB)/ProxySEIS.o $(LLIB)/ProxyAutomate.o $(LLIB)/automates.o $(LLIB)/sockets.o $(LLIB)/Superviseur.o $(LLIB)/ICMP.o $(LLIB)/verbose.o -o prgDeSupervision

clean: 
	rm -f = $(LLIB)/* 
	rm -f prgDeSupervision

.PHONY: clean 
