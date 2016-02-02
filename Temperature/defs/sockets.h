#ifndef _SOCKETS_H
#define  _SOCKETS_H
#include <arpa/inet.h>	// pour tout ce qui est socket et connexe

#include <string>
using namespace std;

int creer_sock(int type, int port,in_addr_t Adrs_Interface);
int sockGene(int type, int port,string adresse);
int sockTCPspecIF(int port,string Adrs_Interface);
int sockUDPspecIF(int port,string Adrs_Interface);
int sockClientTCPspecIF(string Adrs_Interface);
int sockClientUDPspecIF(string Adrs_Interface);
int sockTCPspecLoopback(int port);
int sockUDPspecLoopback(int port);
int sockClientTCPspecLoopback();
int sockClientUDPspecLoopback();
int sockTCPtoutesIF(int port);
int sockUDPtoutesIF(int port);
int sockClientTCPtoutesIF();
int sockClientUDPtoutesIF();

#endif // _SOCKETS_H
