#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <string.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

#include <string.h>
#include <fstream>
#include <cstdlib>
#include <unistd.h>

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

#define LG_ASCII_ETH 17

#define ETH_HW_ADDR_LEN 6
#define IP_ADDR_LEN 4
#define ETHER_HW_TYPE 1
#define OP_ARP_REQUEST 2

bool ishexadec(char c);
bool IsAdrsEthernet(string chaine);
bool IsChaineNumeric(string chaine);
bool  convAsciiToEther(string source, u_char * ptrCible);
void  convEtherToAscii(u_char * ptrSource, string & Cible);
bool  convAsciiToIp(string source, u_char * ptrCible);
void  convIPToAscii(u_char * ptrSource, string & Cible);
string lireShortHex(u_short * src);