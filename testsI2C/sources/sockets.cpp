// g++ sockets.c -c -o ../lib/sockets.o

#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>

using namespace std;

int creer_sock(int type, u_short port,in_addr_t Adrs_Interface)
{
	struct sockaddr_in adresse;
	int desc; /* descripteur de la socket */
	int longueur=sizeof(struct sockaddr_in); /* taille de l'adresse*/
	/* Creation de la socket */
	if ((desc=socket(AF_INET, type, 0))==-1)
	{
		cerr<<"Creation de socket impossible"<<endl;
		return -1;
	}
	/* Preparation de l'adresse d'attachement*/
	adresse.sin_family=AF_INET;
	adresse.sin_addr.s_addr=Adrs_Interface;
	adresse.sin_port=htons(port); /* numero de port en format reseau*/
	/* Demande d'attachement de la socket */
	if(bind(desc,(struct sockaddr*) &adresse, longueur)==-1)
	{
		cerr<<"Attachement de la socket impossible sur port"<<port<<endl;
		close(desc); /* attachement impossible => fermeture de la socket*/
		return -1;
	}
	return desc;
}

int sockGene(int type, int port,string adresse)
{
	return creer_sock(type,port,inet_addr(adresse.c_str()));
}

int sockTCPspecIF(int port,string Adrs_Interface)
{
	return creer_sock(SOCK_STREAM, port,inet_addr(Adrs_Interface.c_str()));
}

int sockUDPspecIF(int port,string Adrs_Interface)
{
	return creer_sock(SOCK_DGRAM, port,inet_addr(Adrs_Interface.c_str()));
}

int sockClientTCPspecIF(string Adrs_Interface)
{
	return creer_sock(SOCK_STREAM, 0,inet_addr(Adrs_Interface.c_str()));
}

int sockClientUDPspecIF(string Adrs_Interface)
{
	return creer_sock(SOCK_DGRAM, 0,inet_addr(Adrs_Interface.c_str()));
}

int sockTCPspecLoopback(int port)
{
	return creer_sock(SOCK_STREAM,port,inet_addr("127.0.0.1"));
}

int sockUDPspecLoopback(int port)
{
	return creer_sock(SOCK_DGRAM,port,inet_addr("127.0.0.1"));
}

int sockClientTCPspecLoopback()
{
	return creer_sock(SOCK_STREAM,0,inet_addr("127.0.0.1"));
}


int sockClientUDPspecLoopback()
{
	return creer_sock(SOCK_DGRAM,0,inet_addr("127.0.0.1"));
}

int sockTCPtoutesIF(int port)
{
	return creer_sock(SOCK_STREAM,port,inet_addr("0.0.0.0"));
}

int sockUDPtoutesIF(int port)
{
	return creer_sock(SOCK_DGRAM,port,inet_addr("0.0.0.0"));
}


int sockClientTCPtoutesIF()
{
	return creer_sock(SOCK_STREAM,0,inet_addr("0.0.0.0"));
}

int sockClientUDPtoutesIF()
{
	return creer_sock(SOCK_DGRAM,0,inet_addr("0.0.0.0"));
}

//_________________  FIN FICHIER _______________________________________

