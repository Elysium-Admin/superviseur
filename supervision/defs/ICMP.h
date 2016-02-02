// Entêtes C
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>

// Entêtes C pour les sockets
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

// Entêtes C++
#include <sstream>
#include <string>
#include <iostream>
using namespace std;

/**
    Classe permettant de créer des packets ICMP que l'on peut envoyer sur le réseau
    Auteur : Sylvain VISTE
    Fichiers : defs/ICMP.h, sources/ICMP.cpp
 */
class ICMP
{
public:
    // On crée un objet ICMP
	ICMP();

    // On envoie un ping à l'IP désignée avec un timeout (par default : 3) en secondes
    int ping(string ip, u_char tempsMax=3);

    // Enumérations des valeurs retournées par la fonction ping
	enum PING_REPLY{
	    PING_OK = 0,
	    PING_ERROR = 1,
	    PING_CREATE = 2,
	    PING_SEND = 4,
	    PING_TIMEOUT = 8,
	    PING_CREATE_TIMEOUT = 16,
	    PING_RECEIVE = 32,
	    PING_TOO_SMALL = 64,
	    PING_INVALID = 128
	};
};
